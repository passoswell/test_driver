/**
 * @file drv_dio.cpp
 * @author your name (you@domain.com)
 * @brief Give access to digital inputs and outputs on esp32
 * @version 0.1
 * @date 2024-08-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "drv_dio.hpp"

#include  "driver/gpio.h"

/**
 * @brief Constructor
 * @param line_offset GPIO number
 */
DrvDIO::DrvDIO(uint8_t line_offset)
{
  m_dio_line_number = line_offset;
  m_value = false;
}

/**
 * @brief Destructor
 */
DrvDIO::~DrvDIO()
{
  // Nothing is done here
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t DrvDIO::configure(const DioConfigure_t *list, uint8_t list_size)
{
  Status_t success;
  gpio_config_t settings =
  {
    .pin_bit_mask = 0,
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
#if SOC_GPIO_SUPPORT_PIN_HYS_FILTER
    .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE,
#endif
  };
  int line_direction = DIO_DIRECTION_OUTPUT;
  int line_drive = DIO_DRIVE_PUSH_PULL;
  esp_err_t esp_err;

  if(list != nullptr)
  {
    for(uint8_t i = 0; i < list_size; i++)
    {
      switch (list[i].parameter)
      {
        case DIO_LINE_DIRECTION:
          if(list[i].value == DIO_DIRECTION_INPUT){ line_direction = DIO_DIRECTION_INPUT;}
          if(list[i].value == DIO_DIRECTION_OUTPUT){ line_direction = DIO_DIRECTION_OUTPUT;}
          break;
        case DIO_LINE_DRIVE:
          if(list[i].value == DIO_DRIVE_OPEN_DRAIN){ line_drive = DIO_DRIVE_OPEN_DRAIN;}
          break;
        case DIO_LINE_BIAS:
          if(list[i].value == DIO_BIAS_DISABLED)
          {
            settings.pull_up_en = GPIO_PULLUP_DISABLE;
            settings.pull_down_en = GPIO_PULLDOWN_DISABLE;
          }
          if(list[i].value == DIO_BIAS_PULL_UP)
          {
            settings.pull_up_en = GPIO_PULLUP_ENABLE;
            settings.pull_down_en = GPIO_PULLDOWN_DISABLE;
          }
          if(list[i].value == DIO_BIAS_PULL_DOWN)
          {
            settings.pull_up_en = GPIO_PULLUP_DISABLE;
            settings.pull_down_en = GPIO_PULLDOWN_ENABLE;
          }
          break;
        case DIO_LINE_INITIAL_VALUE:
          if(list[i].value == DIO_STATE_LOW){m_value = false;}
          if(list[i].value == DIO_STATE_HIGH){m_value = true;}
          break;
      default:
        break;
      }
    }
  }

  if(line_direction == DIO_DIRECTION_INPUT)
  {
    settings.mode = GPIO_MODE_INPUT;
  }else
  {
    if(line_drive == DIO_DRIVE_PUSH_PULL)
    {
      settings.mode = GPIO_MODE_OUTPUT;
    }else
    {
      settings.mode = GPIO_MODE_OUTPUT_OD;
    }
  }

  settings.pin_bit_mask = 1 << m_dio_line_number;
  esp_err = gpio_set_level((gpio_num_t) m_dio_line_number, m_value);
  esp_err |= gpio_config(&settings);
  if(esp_err == ESP_OK)
  {
    success = STATUS_DRV_SUCCESS;
  }else
  {
    success = STATUS_DRV_ERR_PARAM;
  }
  return success;
}

/**
 * @brief Read from a digital pin
 * @param state The state of the digital pin
 * @return Status_t
 */
Status_t DrvDIO::read(bool &state)
{
  state = gpio_get_level((gpio_num_t) m_dio_line_number) == 0 ? false : true;
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Write to a digital output pin
 * @param state The state to set in the gpio
 * @return Status_t
 */
Status_t DrvDIO::write(bool value)
{
  esp_err_t esp_err;
  esp_err = gpio_set_level((gpio_num_t) m_dio_line_number, value);
  if(esp_err == ESP_OK)
  {
    m_value = value;
    return STATUS_DRV_SUCCESS;
  }else
  {
    return STATUS_DRV_ERR_PARAM;
  }
}

/**
 * @brief Toggle the state of a digital output
 * @return Status_t
 */
Status_t DrvDIO::toggle()
{
  m_value = !m_value;
  return write(m_value);
}

/**
 * @brief Install an event callback function
 *
 * @param edge The edge that will trigger the event
 * @param func The callback function
 * @param arg A user parameter
 * @return Status_t
 */
Status_t DrvDIO::setCallback(DioEdge_t edge, DioCallback_t func, void *arg)
{
  gpio_int_type_t interruption_type;
  if(func == nullptr) { return STATUS_DRV_NULL_POINTER;}

  // install gpio isr service
  gpio_install_isr_service(0);

  switch(edge)
  {
    case DIO_EDGE_RISING:
      interruption_type = GPIO_INTR_POSEDGE;
      break;
    case DIO_EDGE_FALLING:
      interruption_type = GPIO_INTR_NEGEDGE;
      break;
    case DIO_EDGE_BOTH:
      interruption_type = GPIO_INTR_ANYEDGE;
      break;
    default:
      gpio_set_intr_type((gpio_num_t) m_dio_line_number, GPIO_INTR_DISABLE);
      gpio_isr_handler_remove((gpio_num_t) m_dio_line_number);
      m_func = nullptr;
      m_arg = nullptr;
      return STATUS_DRV_SUCCESS;
      break;
  }

  m_func = func;
  m_arg = arg;
  gpio_set_intr_type((gpio_num_t)m_dio_line_number, interruption_type);
  gpio_isr_handler_add((gpio_num_t)m_dio_line_number,
                       [] (void *arg) -> void
                       {
                        DrvDIO *self = (DrvDIO *) arg;
                        self->callback();
                       },
                       this);

  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Callback method, called when a configured edge event occurs
 */
void DrvDIO::callback(void)
{
  DioEdge_t edge = DIO_EDGE_FALLING;
  bool state;
  if(m_func != nullptr)
  {
    read(state);
    if(state) { edge = DIO_EDGE_RISING;}
    m_func(STATUS_DRV_SUCCESS, m_dio_line_number, edge, state, m_arg);
  }
}