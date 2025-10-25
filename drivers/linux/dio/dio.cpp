/**
 * @file dio.cpp
 * @author your name (you@domain.com)
 * @brief Give access to digital inputs and outputs on linux
 * @version 0.1
 * @date 2024-10-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "linux/dio/dio.hpp"

#include <gpiod.h>
#include <unistd.h>
#include <cerrno>  // For errno
#include <cstring> // For strerror

/**
 * @brief Constructor
 *
 * @param line_offset GPIO identifier
 * @param chip_number Path to the corresponding gpiochip device file.
 */
DIO::DIO(uint32_t line_offset, uint32_t chip_number)
{
  m_chip_number = chip_number;
  m_line_number = line_offset;
  m_chip_handle = nullptr;
  m_line_handle = nullptr;
  m_flags = 0;
  m_value = false;

  m_event_handler = nullptr;
  m_edge = EVENT_NONE;

  m_sync.run = false;
  m_sync.terminate = false;
  m_sync.buffer = nullptr;
  m_sync.size = 0;
  m_sync.func = nullptr;
  m_sync.arg = nullptr;
}

/**
 * @brief Destuctor
 */
DIO::~DIO()
{
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);

  if(m_sync.thread != nullptr)
  {
    locker1.lock();
    m_sync.terminate = true;
    m_sync.run = true;
    locker1.unlock();
    m_sync.condition.notify_one();
    m_sync.thread->join();
    delete m_sync.thread;
  }

  if(m_line_handle != nullptr)
  {
    gpiod_line_release((struct gpiod_line *)m_line_handle);
  }
  if(m_chip_handle != nullptr)
  {
    gpiod_chip_close((struct gpiod_chip *)m_chip_handle);
  }
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return ErrorCode
 */
ErrorCode DIO::configure(const SettingsList_t *list, uint8_t list_size)
{
  ErrorCode result(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
  struct gpiod_line_request_config settings =
  {
    .consumer = "my_driver",
    .request_type = GPIOD_LINE_REQUEST_DIRECTION_INPUT,
    .flags = GPIOD_LINE_REQUEST_FLAG_BIAS_DISABLE
  };
  int ret;

  if(list != nullptr)
  {
    for(uint8_t i = 0; i < list_size; i++)
    {
      switch (list[i].parameter)
      {
        case DIO_LINE_DIRECTION:
          if(settings.request_type > GPIOD_LINE_REQUEST_DIRECTION_OUTPUT) { continue;}
          if(list[i].value == DIO_DIRECTION_OUTPUT){settings.request_type = GPIOD_LINE_REQUEST_DIRECTION_OUTPUT;}
          break;
        case DIO_LINE_DRIVE:
          if(list[i].value == DIO_DRIVE_OPEN_DRAIN){settings.flags |= GPIOD_LINE_REQUEST_FLAG_OPEN_DRAIN;}
          if(list[i].value == DIO_DRIVE_OPEN_SOURCE){settings.flags |= GPIOD_LINE_REQUEST_FLAG_OPEN_SOURCE;}
          break;
        case DIO_LINE_BIAS:
          settings.flags &= ~GPIOD_LINE_REQUEST_FLAG_BIAS_DISABLE;
          if(list[i].value == DIO_BIAS_DISABLED){settings.flags |= GPIOD_LINE_REQUEST_FLAG_BIAS_DISABLE;}
          if(list[i].value == DIO_BIAS_PULL_UP){settings.flags |= GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP;}
          if(list[i].value == DIO_BIAS_PULL_DOWN){settings.flags |= GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_DOWN;}
          break;
        case DIO_LINE_INITIAL_VALUE:
          if(list[i].value == DIO_STATE_LOW){m_value = false;}
          if(list[i].value == DIO_STATE_HIGH){m_value = true;}
          break;
        break;
      default:
        break;
      }
    }
  }

  m_flags = settings.flags;
  m_chip_handle = gpiod_chip_open_by_number(m_chip_number);
  if (m_chip_handle != nullptr)
  {
    m_line_handle = gpiod_chip_get_line((struct gpiod_chip *)m_chip_handle, m_line_number);
    if (m_line_handle != nullptr)
    {
      ret = gpiod_line_request((struct gpiod_line *)m_line_handle, &settings, m_value);
      if (ret >= 0)
      {
        return result;
      }else
      {
        result.setValue(GenericErrorCode::kFailed);
        gpiod_line_release((struct gpiod_line *)m_line_handle);
      }
    }else
    {
      result.setValue(GenericErrorCode::kFailed);
    }
    gpiod_chip_close((struct gpiod_chip *)m_chip_handle);
  }else
  {
    result.setValue(GenericErrorCode::kFailed);
    result.setMessage(strerror(errno));
  }

  return result;
}

/**
 * @brief Read from a digital pin
 * @param state The state of the digital pin
 * @return ErrorCode
 */
ErrorCode DIO::read(bool &state)
{
  ErrorCode result(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
  int val;
  if(m_line_handle == nullptr)
  {
    result.setValue(GenericErrorCode::kNullPointer);
    return result;
  }
  val = gpiod_line_get_value((struct gpiod_line *)m_line_handle);
  if(val == 0){state = false;}
  else if(val == 1) {state = true;}
  else
  {
    result.setValue(GenericErrorCode::kFailed);
  }
  return result;
}

/**
 * @brief Write to a digital output pin
 * @param state The state to set in the gpio
 * @return ErrorCode
 */
ErrorCode DIO::write(bool value)
{
  ErrorCode result(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
  int ret;
  if(m_line_handle == nullptr)
  {
    result.setValue(GenericErrorCode::kNullPointer);
    return result;
  }
  ret = gpiod_line_set_value((struct gpiod_line *)m_line_handle, (int) value);
  if(ret == 0)
  {
    m_value = (bool) value;
  }else
  {
    result.setValue(GenericErrorCode::kFailed);
  }
  return result;
}

/**
 * @brief Toggle the state of a digital output
 * @return ErrorCode
 */
ErrorCode DIO::toggle()
{
  return write(!m_value);
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
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);
  ErrorCode status(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
  struct gpiod_line_request_config settings =
  {
    .consumer = "my_driver",
    .request_type = GPIOD_LINE_REQUEST_DIRECTION_INPUT,
    .flags = GPIOD_LINE_REQUEST_FLAG_BIAS_DISABLE
  };
  int ret, val = 0;

  if(m_line_handle == nullptr || m_chip_handle == nullptr)
  {
    status.setValue(GenericErrorCode::kNullPointer);
    return status;
  }
  settings.flags = m_flags;

  if(!enable)
  {
    if(m_sync.thread != nullptr)
    {
      // locker1.lock();
      m_sync.terminate = true;
      m_sync.run = true;
      // locker1.unlock();
      m_sync.condition.notify_one();
      m_sync.thread->join();
      delete m_sync.thread;
    }
    return status;
  }

  switch (m_edge)
  {
    case EVENT_EDGE_RISING:
      gpiod_line_release((struct gpiod_line *)m_line_handle);
      settings.request_type = GPIOD_LINE_REQUEST_EVENT_RISING_EDGE;
      m_line_handle = gpiod_chip_get_line((struct gpiod_chip *)m_chip_handle, m_line_number);
      ret = gpiod_line_request((struct gpiod_line *)m_line_handle, &settings, val);
      break;
    case EVENT_EDGE_FALLING:
      gpiod_line_release((struct gpiod_line *)m_line_handle);
      settings.request_type = GPIOD_LINE_REQUEST_EVENT_FALLING_EDGE;
      m_line_handle = gpiod_chip_get_line((struct gpiod_chip *)m_chip_handle, m_line_number);
      ret = gpiod_line_request((struct gpiod_line *)m_line_handle, &settings, val);
      break;
    case EVENT_EDGE_BOTH:
      gpiod_line_release((struct gpiod_line *)m_line_handle);
      settings.request_type = GPIOD_LINE_REQUEST_EVENT_BOTH_EDGES;
      m_line_handle = gpiod_chip_get_line((struct gpiod_chip *)m_chip_handle, m_line_number);
      ret = gpiod_line_request((struct gpiod_line *)m_line_handle, &settings, val);
      break;
    case EVENT_NONE:
      if(m_sync.thread != nullptr)
      {
        // locker1.lock();
        m_sync.terminate = true;
        m_sync.run = true;
        // locker1.unlock();
        m_sync.condition.notify_one();
        m_sync.thread->join();
      }
      return status;
      break;
    default:
      status.setValue(GenericErrorCode::kInvalidParameter);
      return status;
      break;
  }

  if(ret < 0)
  {
    status.setValue(GenericErrorCode::kFailed);
    return status;
  }
  if(m_sync.thread == nullptr)
  {
    m_sync.thread = new std::thread(&DIO::readAsyncThread, this);
  }

  return status;
}

/**
 * @brief A thread that listens to gpio edge events
 */
void DIO::readAsyncThread(void)
{
  struct timespec ts = {0, 100000000};
  struct gpiod_line_event event;
  EventsList_t edge;
  ErrorCode status(GenericErrorCode::kSuccess, DioErrorCategory::getCategory());
  uint8_t state[1];
  int ret;

  while(!m_sync.terminate)
  {
    if(m_line_handle == nullptr) break;
    ret = gpiod_line_event_wait((struct gpiod_line *)m_line_handle, &ts);
    if (ret <= 0) { continue; }
    ret = gpiod_line_event_read((struct gpiod_line *)m_line_handle, &event);
    if (ret < 0) { continue; }
    if(m_event_handler == nullptr) { continue; }
    switch(event.event_type)
    {
      case GPIOD_LINE_EVENT_RISING_EDGE:
        edge = EVENT_EDGE_RISING;
        state[0] = true;
        status.setValue(GenericErrorCode::kSuccess);
        break;
      case GPIOD_LINE_EVENT_FALLING_EDGE:
        edge = EVENT_EDGE_FALLING;
        state[0] = false;
        status.setValue(GenericErrorCode::kSuccess);
        break;
      default:
        edge = EVENT_NONE;
        state[0] = false;
        status.setValue(GenericErrorCode::kFailed);
        break;
    }
    m_event_handler->onEvent(status, edge, state);
  }
  m_sync.terminate = false;
  m_sync.run = false;
}