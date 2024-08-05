/**
 * @file drv_gpio.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "drv_gpio.hpp"

#include  "driver/gpio.h"

/**
 * @brief Constructor
 * @param line_offset GPIO number
 */
DrvGPIO::DrvGPIO(uint8_t line_offset)
{
  m_gpio_line_number = line_offset;
}

/**
 * @brief Destructor
 */
DrvGPIO::~DrvGPIO()
{
  // Nothing is done here
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t DrvGPIO::configure(const DrvGpioConfigure_t *list, uint8_t list_size)
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
  int line_direction = DRV_GPIO_DIRECTION_OUTPUT;
  int line_drive = DRV_GPIO_DRIVE_PUSH_PULL;
  esp_err_t esp_err;
  int val = 0;

  if(list != nullptr)
  {
    for(uint8_t i = 0; i < list_size; i++)
    {
      switch (list[i].parameter)
      {
        case DRV_GPIO_LINE_DIRECTION:
          if(list[i].value == DRV_GPIO_DIRECTION_INPUT){ line_direction = DRV_GPIO_DIRECTION_INPUT;}
          if(list[i].value == DRV_GPIO_DIRECTION_OUTPUT){ line_direction = DRV_GPIO_DIRECTION_OUTPUT;}
          break;
        case DRV_GPIO_LINE_DRIVE:
          if(list[i].value == DRV_GPIO_DRIVE_OPEN_DRAIN){ line_drive = DRV_GPIO_DRIVE_OPEN_DRAIN;}
          break;
        case DRV_GPIO_LINE_BIAS:
          if(list[i].value == DRV_GPIO_BIAS_DISABLED)
          {
            settings.pull_up_en = GPIO_PULLUP_DISABLE;
            settings.pull_down_en = GPIO_PULLDOWN_DISABLE;
          }
          if(list[i].value == DRV_GPIO_BIAS_PULL_UP)
          {
            settings.pull_up_en = GPIO_PULLUP_ENABLE;
            settings.pull_down_en = GPIO_PULLDOWN_DISABLE;
          }
          if(list[i].value == DRV_GPIO_BIAS_PULL_DOWN)
          {
            settings.pull_up_en = GPIO_PULLUP_DISABLE;
            settings.pull_down_en = GPIO_PULLDOWN_ENABLE;
          }
          break;
        case DRV_GPIO_LINE_INITIAL_VALUE:
          if(list[i].value == DRV_GPIO_STATE_LOW){val = 0;}
          if(list[i].value == DRV_GPIO_STATE_HIGH){val = 1;}
          break;
      default:
        break;
      }
    }
  }

  if(line_direction == DRV_GPIO_DIRECTION_INPUT)
  {
    settings.mode = GPIO_MODE_INPUT;
  }else
  {
    if(line_drive == DRV_GPIO_DRIVE_PUSH_PULL)
    {
      settings.mode = GPIO_MODE_OUTPUT;
    }else
    {
      settings.mode = GPIO_MODE_OUTPUT_OD;
    }
  }

  settings.pin_bit_mask = 1 << m_gpio_line_number;
  esp_err = gpio_set_level((gpio_num_t) m_gpio_line_number, val);
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
Status_t DrvGPIO::read(bool &state)
{
  state = gpio_get_level((gpio_num_t) m_gpio_line_number) == 0 ? false : true;
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Write to a digital output pin
 * @param state The state to set in the gpio
 * @return Status_t
 */
Status_t DrvGPIO::write(bool value)
{
  esp_err_t esp_err;
  esp_err = gpio_set_level((gpio_num_t) m_gpio_line_number, value);
  if(esp_err == ESP_OK)
  {
    return STATUS_DRV_SUCCESS;
  }else
  {
    return STATUS_DRV_ERR_PARAM;
  }
}

/**
 * @brief Install an event callback function
 *
 * @param edge The edge that will trigger the event
 * @param func The callback function
 * @param arg A user parameter
 * @return Status_t
 */
Status_t DrvGPIO::setCallback(DrvGpioEdge_t edge, DrvGpioCallback_t func, void *arg)
{
  gpio_int_type_t interruption_type;
  if(func == nullptr) { return STATUS_DRV_NULL_POINTER;}

  // install gpio isr service
  gpio_install_isr_service(0);

  switch(edge)
  {
    case DRV_GPIO_EDGE_RISING:
      interruption_type = GPIO_INTR_POSEDGE;
      break;
    case DRV_GPIO_EDGE_FALLING:
      interruption_type = GPIO_INTR_NEGEDGE;
      break;
    case DRV_GPIO_EDGE_BOTH:
      interruption_type = GPIO_INTR_ANYEDGE;
      break;
    default:
      gpio_set_intr_type((gpio_num_t) m_gpio_line_number, GPIO_INTR_DISABLE);
      gpio_isr_handler_remove((gpio_num_t) m_gpio_line_number);
      m_func = nullptr;
      m_arg = nullptr;
      return STATUS_DRV_SUCCESS;
      break;
  }

  m_func = func;
  m_arg = arg;
  gpio_set_intr_type((gpio_num_t)m_gpio_line_number, interruption_type);
  gpio_isr_handler_add((gpio_num_t)m_gpio_line_number,
                       [] (void *arg) -> void
                       {
                        DrvGPIO *self = (DrvGPIO *) arg;
                        self->callback();
                       },
                       this);

  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Callback method, called when a configured edge event occurs
 */
void DrvGPIO::callback(void)
{
  DrvGpioEdge_t edge = DRV_GPIO_EDGE_FALLING;
  bool state;
  if(m_func != nullptr)
  {
    read(state);
    if(state) { edge = DRV_GPIO_EDGE_RISING;}
    m_func(STATUS_DRV_SUCCESS, m_gpio_line_number, edge, state, m_arg);
  }
}