/**
 * @file dio.cpp
 * @author your name (you@domain.com)
 * @brief Give access to digital inputs and outputs on pipico
 * @version 0.1
 * @date 2024-10-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "dio.hpp"

#include <vector>

#include "hardware/gpio.h"

/**
 * @brief Vector of DIO pointers, used for the interruption callback
 */
std::vector<DIO*> g_dio_ptr;

void drvDioCallback(uint dio, uint32_t events);

/**
 * @brief Constructor
 * @param line_offset DIO number
 */
DIO::DIO(uint32_t line_offset, uint32_t port)
{
  m_line_number = line_offset;
  m_line_bias = DIO_BIAS_DISABLED;
}

/**
 * @brief Destructor
 */
DIO::~DIO()
{
  // Nothing is done here
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t DIO::configure(const SettingsList_t *list, uint8_t list_size)
{
  Status_t success;
  DioDirection_t line_direction = DIO_DIRECTION_INPUT;
  DioDrive_t line_drive = DIO_DRIVE_PUSH_PULL;
  DioBias_t line_bias = DIO_BIAS_DISABLED;
  int val = 0;

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
          if(list[i].value == DIO_DRIVE_PUSH_PULL){ line_drive = DIO_DRIVE_PUSH_PULL;}
          if(list[i].value == DIO_DRIVE_OPEN_DRAIN){ return STATUS_DRV_ERR_PARAM;}
          break;
        case DIO_LINE_BIAS:
          if(list[i].value == DIO_BIAS_DISABLED) { line_bias = DIO_BIAS_DISABLED;}
          if(list[i].value == DIO_BIAS_PULL_UP) { line_bias = DIO_BIAS_PULL_UP;}
          if(list[i].value == DIO_BIAS_PULL_DOWN) { line_bias = DIO_BIAS_PULL_DOWN;}
          break;
        case DIO_LINE_INITIAL_VALUE:
          if(list[i].value == DIO_STATE_LOW){val = 0;}
          if(list[i].value == DIO_STATE_HIGH){val = 1;}
          break;
      default:
        break;
      }
    }
  }

  gpio_init(m_line_number);
  if(line_direction == DIO_DIRECTION_INPUT)
  {
    gpio_set_dir(m_line_number, GPIO_IN);
  }else
  {
    gpio_put(m_line_number, val);
    gpio_set_dir(m_line_number, GPIO_OUT);
  }
  if(line_bias == DIO_BIAS_PULL_UP)
  {
    gpio_pull_up(m_line_number);
  }else if(line_bias == DIO_BIAS_PULL_DOWN)
  {
    gpio_pull_down(m_line_number);
  }

  m_line_bias = line_bias;
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Read from a digital pin
 * @param state The state of the digital pin
 * @return Status_t
 */
Status_t DIO::read(bool &state)
{
  state = gpio_get(m_line_number);
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Write to a digital output pin
 * @param state The state to set in the gpio
 * @return Status_t
 */
Status_t DIO::write(bool value)
{
  gpio_put(m_line_number, value);
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Toggle the state of a digital output
 * @return Status_t
 */
Status_t DIO::toggle()
{
  gpio_xor_mask(1 << m_line_number);
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Install an event callback function
 *
 * @param edge The edge that will trigger the event
 * @param func The callback function
 * @param arg A user parameter
 * @return Status_t
 */
Status_t DIO::setCallback(EventsList_t edge, DriverCallback_t function, void *user_arg)
{
  m_func = function;
  m_arg = user_arg;
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Enable or disable callback operation
 *
 * @param enable True to enable callback operation
 * @return Status_t
 */
Status_t DIO::enableCallback(bool enable, EventsList_t edge)
{
  uint32_t interruption_type;

  if(!enable)
  {
    gpio_set_irq_enabled(m_line_number, GPIO_IRQ_LEVEL_LOW, false);

    for (auto it = g_dio_ptr.begin(); it != g_dio_ptr.end(); it++)
    {
      DIO *obj;
      obj = *it;
      // if the current index is needed:
      auto i = std::distance(g_dio_ptr.begin(), it);
      if (obj->m_line_number == m_line_number)
      {
        g_dio_ptr.erase(it);
        break;
      }
    }
    return STATUS_DRV_SUCCESS;
  }

  switch(edge)
  {
    case EVENT_EDGE_RISING:
      interruption_type = GPIO_IRQ_EDGE_RISE;
      break;
    case EVENT_EDGE_FALLING:
      interruption_type = GPIO_IRQ_EDGE_FALL;
      break;
    case EVENT_EDGE_BOTH:
      interruption_type = GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL;
      break;
    default:
      return STATUS_DRV_ERR_PARAM;
      break;
  }

  g_dio_ptr.push_back(this);
  gpio_set_irq_enabled_with_callback(m_line_number, interruption_type, true, &drvDioCallback);

  // Setting bias here because pipico disables its configuration when setting interruption
  if(m_line_bias == DIO_BIAS_PULL_UP)
  {
    gpio_pull_up(m_line_number);
  }else if(m_line_bias == DIO_BIAS_PULL_DOWN)
  {
    gpio_pull_down(m_line_number);
  }

  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Callback function called when a configured edge event occurs
 *
 * @param dio GPIO number
 * @param events A mask with the events that occurred
 */
void drvDioCallback(uint dio, uint32_t events)
{
  EventsList_t edge = EVENT_EDGE_FALLING;
  uint8_t state[1] = {false};

  for (auto it = g_dio_ptr.begin(); it != g_dio_ptr.end(); it++)
  {
    DIO *obj;
    obj = *it;
    // if the current index is needed:
    auto i = std::distance(g_dio_ptr.begin(), it);
    if (obj->m_line_number == dio)
    {
      if(obj->m_func != nullptr)
      {
        if(events & GPIO_IRQ_EDGE_RISE)
        {
          edge = EVENT_EDGE_RISING;
          state[0] = true;
        }
        obj->m_func(STATUS_DRV_SUCCESS, edge, state, obj->m_arg);
      }
      break;
    }
  }
}