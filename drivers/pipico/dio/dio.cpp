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
 * @brief Constructor
 * @param line_offset DIO number
 */
DIO::DIO(uint32_t line_offset, uint32_t port)
{
  m_line_number = line_offset;
  m_line_bias = DIO_BIAS_DISABLED;
  m_event_handler = nullptr;
  m_edge = EVENT_NONE;
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
 * @return ErrorCode
 */
ErrorCode DIO::configure(const SettingsList_t *list, uint8_t list_size)
{
  ErrorCode success(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
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
          if(list[i].value == DIO_DRIVE_OPEN_DRAIN)
          {
            success.setValue(GenericErrorCode::kInvalidParameter);
            return success;
          }
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
  return success;
}

/**
 * @brief Read from a digital pin
 * @param state The state of the digital pin
 * @return ErrorCode
 */
ErrorCode DIO::read(bool &state)
{
  state = gpio_get(m_line_number);
  return makeErrorCode(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
}

/**
 * @brief Write to a digital output pin
 * @param state The state to set in the gpio
 * @return ErrorCode
 */
ErrorCode DIO::write(bool value)
{
  gpio_put(m_line_number, value);
  return makeErrorCode(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
}

/**
 * @brief Toggle the state of a digital output
 * @return ErrorCode
 */
ErrorCode DIO::toggle()
{
  gpio_xor_mask(1 << m_line_number);
  return makeErrorCode(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
}

/**
 * @brief Install an event callback function
 *
 * @param edge The edge that will trigger the event
 * @param event_handler The callback object
 * @return ErrorCode
 */
ErrorCode DIO::setEventCallback(EventsList_t edge, iCallback &event_handler)
{
  m_event_handler = &event_handler;
  m_edge = edge;
  return makeErrorCode(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
}

/**
 * @brief Enable or disable callback operation
 *
 * @param enable True to enable callback operation
 * @return ErrorCode
 */
ErrorCode DIO::enableInterruption(bool enable)
{
  uint32_t interruption_type;

  if(!enable)
  {
    gpio_set_irq_enabled(m_line_number, GPIO_IRQ_LEVEL_LOW, false);

    for (auto iterator = m_dio_ptr.begin(); iterator != m_dio_ptr.end(); iterator++)
    {
      DIO *obj;
      obj = *iterator;
      // if the current index is needed:
      // auto i = std::distance(m_dio_ptr.begin(), iterator);
      if (obj->m_line_number == m_line_number)
      {
        m_dio_ptr.erase(iterator);
        break;
      }
    }
    return makeErrorCode(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
  }

  switch(m_edge)
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
      return makeErrorCode(GenericErrorCode::kInvalidParameter, DioErrorCategory::getCategory());
      break;
  }

  m_dio_ptr.push_back(this);
  gpio_set_irq_enabled_with_callback(m_line_number, interruption_type, true, &drvDioCallback);

  // Setting bias here because pipico disables its configuration when setting interruption
  if(m_line_bias == DIO_BIAS_PULL_UP)
  {
    gpio_pull_up(m_line_number);
  }else if(m_line_bias == DIO_BIAS_PULL_DOWN)
  {
    gpio_pull_down(m_line_number);
  }

  return makeErrorCode(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
}

/**
 * @brief Callback function called when a configured edge event occurs
 *
 * @param dio GPIO number
 * @param events A mask with the events that occurred
 */
void drvDioCallback(unsigned int dio, uint32_t events)
{
  EventsList_t edge = EVENT_EDGE_FALLING;
  uint8_t state[1] = {false};
  ErrorCode success(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());

  for (auto iterator = DIO::m_dio_ptr.begin(); iterator != DIO::m_dio_ptr.end(); iterator++)
  {
    DIO *obj;
    obj = *iterator;
    // if the current index is needed:
    // auto i = std::distance(DIO::m_dio_ptr.begin(), iterator);
    if (obj->m_line_number == dio)
    {
      if(obj->m_event_handler != nullptr)
      {
        if(events & GPIO_IRQ_EDGE_RISE)
        {
          edge = EVENT_EDGE_RISING;
          state[0] = true;
        }
        obj->m_event_handler->onEvent(success, edge, state);
      }
      break;
    }
  }
}