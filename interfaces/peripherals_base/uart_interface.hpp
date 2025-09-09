/**
 * @file uart_interface.hpp
 * @author your name (you@domain.com)
 * @brief Interface class and methods for UART
 * @version 0.1
 * @date 2025-08-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PERIPHERALS_BASE_UART_INTERFACE_HPP
#define PERIPHERALS_BASE_UART_INTERFACE_HPP


#include <stdint.h>
#include <stdbool.h>

#include "commons.hpp"
#include "dio_interface.hpp"
#include "peripherals_base/communication_interface.hpp"


typedef uint16_t UartHandle_t;

typedef struct
{
  bool run;
  DrvBuffer_t data;
  uint32_t timeout;
  iDIO *rs485_pin;
  iCallback *event_handler;
} UartDataBundle2_t;

typedef struct
{
  UartDataBundle2_t rx;
  UartDataBundle2_t tx;
} UartDataBundle_t;


/**
 * @brief Interface class for UART bus
 */
class iUartBus
{
public:

  iUartBus() = default;

  virtual ~iUartBus() = default;

  virtual Status_t configure(const SettingsList_t *list, uint8_t list_size) = 0;

  virtual Status_t read(iDIO &rs485_pin, Buffer_t data, uint32_t timeout, iCallback &event_handler) = 0;

  virtual Status_t write(iDIO &rs485_pin, Buffer_t data, uint32_t timeout, iCallback &event_handler) = 0;

  virtual uint32_t getBytesAvailable() = 0;

  virtual uint32_t getBytesRead() = 0;
};

/**
 * @brief Interface class for UART peripherals
 */
class iUART : public iComm
{
public:

  iUART() = default;

  virtual ~iUART() = default;

  virtual uint32_t getBytesRead() = 0;
};

/**
 * @brief Base class for UART peripherals
 */
class bUART : public iUART
{
public:

  bUART(iUartBus &bus, iDIO &rs485_pin) :
  m_bus(bus), m_rs485_pin(rs485_pin),
  m_cb_function_rx(nullptr), m_cb_function_tx(nullptr)
  {
  }

  virtual ~bUART() = default;

  virtual Status_t configure(const SettingsList_t *list, uint8_t list_size) override
  {
    return m_bus.configure(list, list_size);
  }

  virtual Status_t read(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.read(m_rs485_pin, data, timeout, *m_cb_function_rx);
  }

  virtual Status_t write(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.write(m_rs485_pin, data, timeout, *m_cb_function_tx);
  }

  virtual uint32_t getBytesRead() override
  {
    return m_bus.getBytesRead();
  }

  virtual Status_t setCallback(EventsList_t event, iCallback &event_handler) override
  {
    Status_t status = STATUS_DRV_SUCCESS;
    switch(event)
    {
    case EVENT_READ:
      m_cb_function_rx = &event_handler;
      break;
    case EVENT_WRITE:
      m_cb_function_tx = &event_handler;
      break;
    default:
      status = STATUS_DRV_ERR_PARAM;
      break;
    }
    return status;
  }

protected:
  iUartBus &m_bus;
  iDIO &m_rs485_pin;
  iCallback *m_cb_function_rx, *m_cb_function_tx;
};


#endif /* PERIPHERALS_BASE_UART_INTERFACE_HPP */
