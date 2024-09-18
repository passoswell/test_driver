/**
 * @file drv_gpio.cpp
 * @author your name (you@domain.com)
 * @brief Give access to digital inputs and outputs on linux
 * @version 0.1
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 *
 */


#include "drivers/linux/dio/drv_dio.hpp"

#include <errno.h>
#include <gpiod.h>
#include <unistd.h>

/**
 * @brief Constructor
 * @param line_offset GPIO identifier
 * @param chip_number Path to the corresponding gpiochip device file.
 */
DrvDIO::DrvDIO(uint8_t line_offset, uint8_t chip_number)
{
  m_dio_chip_number = chip_number;
  m_dio_line_number = line_offset;
  m_dio_chip_handle = nullptr;
  m_dio_line_handle = nullptr;
  m_flags = 0;
  m_value = false;

  m_func = nullptr;
  m_arg = nullptr;

  m_sync.run = false;
  m_sync.terminate = false;
  m_sync.buffer = nullptr;
  m_sync.size = 0;
  m_sync.func = nullptr;
  m_sync.arg = nullptr;
}

/**
 * @brief Destructor
 */
DrvDIO::~DrvDIO()
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
  }

  if(m_dio_line_handle != nullptr)
  {
    gpiod_line_release((struct gpiod_line *)m_dio_line_handle);
  }
  if(m_dio_chip_handle != nullptr)
  {
    gpiod_chip_close((struct gpiod_chip *)m_dio_chip_handle);
  }
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t DrvDIO::configure(const DioConfigure_t *list, uint8_t list_size)
{
  Status_t result;
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
  m_dio_chip_handle = gpiod_chip_open_by_number(m_dio_chip_number);
  if (m_dio_chip_handle != nullptr)
  {
    m_dio_line_handle = gpiod_chip_get_line((struct gpiod_chip *)m_dio_chip_handle, m_dio_line_number);
    if (m_dio_line_handle != nullptr)
    {
      ret = gpiod_line_request((struct gpiod_line *)m_dio_line_handle, &settings, m_value);
      if (ret >= 0)
      {
        return STATUS_DRV_SUCCESS;
      }else
      {
        result = STATUS_DRV_UNKNOWN_ERROR;
        gpiod_line_release((struct gpiod_line *)m_dio_line_handle);
      }
    }else
    {
      result = STATUS_DRV_UNKNOWN_ERROR;
    }
    gpiod_chip_close((struct gpiod_chip *)m_dio_chip_handle);
  }else
  {
    result = STATUS_DRV_UNKNOWN_ERROR;
  }

  return result;
}

/**
 * @brief Read from a digital pin
 * @param state The state of the digital pin
 * @return Status_t
 */
Status_t DrvDIO::read(bool &state)
{
  int val = gpiod_line_get_value((struct gpiod_line *)m_dio_line_handle);
  if(val < 0) {return STATUS_DRV_UNKNOWN_ERROR;}
  if(val == 0){state = false;}
  else {state = true;}
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Write to a digital output pin
 * @param state The state to set in the gpio
 * @return Status_t
 */
Status_t DrvDIO::write(bool value)
{
  int ret = gpiod_line_set_value((struct gpiod_line *)m_dio_line_handle, (int) value);
  if(ret < 0) {return STATUS_DRV_UNKNOWN_ERROR;}
  m_value = value;
  return STATUS_DRV_SUCCESS;
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
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);
  struct gpiod_line_request_config settings =
  {
    .consumer = "my_driver",
    .request_type = GPIOD_LINE_REQUEST_DIRECTION_INPUT,
    .flags = GPIOD_LINE_REQUEST_FLAG_BIAS_DISABLE
  };
  int ret, val = 0;
  settings.flags = m_flags;

  switch (edge)
  {
    case DIO_EDGE_RISING:
      if(func == nullptr) return STATUS_DRV_NULL_POINTER;
      gpiod_line_release((struct gpiod_line *)m_dio_line_handle);
      settings.request_type = GPIOD_LINE_REQUEST_EVENT_RISING_EDGE;
      m_dio_line_handle = gpiod_chip_get_line((struct gpiod_chip *)m_dio_chip_handle, m_dio_line_number);
      ret = gpiod_line_request((struct gpiod_line *)m_dio_line_handle, &settings, val);
      break;
    case DIO_EDGE_FALLING:
      if(func == nullptr) return STATUS_DRV_NULL_POINTER;
      gpiod_line_release((struct gpiod_line *)m_dio_line_handle);
      settings.request_type = GPIOD_LINE_REQUEST_EVENT_FALLING_EDGE;
      m_dio_line_handle = gpiod_chip_get_line((struct gpiod_chip *)m_dio_chip_handle, m_dio_line_number);
      ret = gpiod_line_request((struct gpiod_line *)m_dio_line_handle, &settings, val);
      break;
    case DIO_EDGE_BOTH:
      if(func == nullptr) return STATUS_DRV_NULL_POINTER;
      gpiod_line_release((struct gpiod_line *)m_dio_line_handle);
      settings.request_type = GPIOD_LINE_REQUEST_EVENT_BOTH_EDGES;
      m_dio_line_handle = gpiod_chip_get_line((struct gpiod_chip *)m_dio_chip_handle, m_dio_line_number);
      ret = gpiod_line_request((struct gpiod_line *)m_dio_line_handle, &settings, val);
      break;
    default:
      if(m_func != nullptr)
      {
        locker1.lock();
        m_sync.terminate = true;
        m_sync.run = true;
        locker1.unlock();
        m_sync.condition.notify_one();
        m_sync.thread->join();
      }
      return STATUS_DRV_SUCCESS;
      break;
  }

  if(ret < 0) { return STATUS_DRV_UNKNOWN_ERROR;}
  m_func = func;
  m_arg = arg;
  m_sync.thread = new std::thread(&DrvDIO::readAsyncThread, this);

  return STATUS_DRV_SUCCESS;
}

/**
 * @brief A thread that listens to gpio edge events
 */
void DrvDIO::readAsyncThread(void)
{
  struct timespec ts = {0, 100000000};
  struct gpiod_line_event event;
  DioEdge_t edge;
  Status_t status;
  bool state;
  int ret;

  while(!m_sync.terminate)
  {
    ret = gpiod_line_event_wait((struct gpiod_line *)m_dio_line_handle, &ts);
    if (ret <= 0) { continue; }
    ret = gpiod_line_event_read((struct gpiod_line *)m_dio_line_handle, &event);
    if (ret < 0) { continue; }
    if(m_func == nullptr) { continue; }
    switch(event.event_type)
    {
      case GPIOD_LINE_EVENT_RISING_EDGE:
        edge = DIO_EDGE_RISING;
        state = true;
        status = STATUS_DRV_SUCCESS;
        break;
      case GPIOD_LINE_EVENT_FALLING_EDGE:
        edge = DIO_EDGE_FALLING;
        state = false;
        status = STATUS_DRV_SUCCESS;
        break;
      default:
        edge = DIO_EDGE_NONE;
        read(state);
        status = STATUS_DRV_UNKNOWN_ERROR;
        break;
    }
    m_func(status, m_dio_line_number, edge, state, m_sync.arg);
  }
  m_sync.terminate = false;
  m_sync.run = false;
}