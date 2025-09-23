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


#include <cstdint>
#include <cstdbool>

#include "commons.hpp"
#include "dio_interface.hpp"
#include "peripherals_base/communication_interface.hpp"


/**
 * @brief UART error codes category
 */
class UartErrorCategory : public ErrorCategory
{
public:
  // Get the error category's name
  constexpr std::string_view name() const noexcept override { return "uart_interface"; }

  // Get the error's helper message
  constexpr std::string_view message(int error_value) const noexcept override
  {
    switch (static_cast<GenericErrorCode>(error_value))
    {
      case GenericErrorCode::kSuccess: return "Success";
      case GenericErrorCode::kInvalidParameter: return "Invalid UART input parameter";
      case GenericErrorCode::kNotConfigured: return "UART resource is not properly configured";
      case GenericErrorCode::kNullPointer: return "A null pointer was detected on UART";
      case GenericErrorCode::kBadHandle: return "Invalid UART handle to the resource";
      case GenericErrorCode::kTimedOut: return "UART operation took more time than expected";
      case GenericErrorCode::kBusy: return "UART bus already in use by another controller";
      case GenericErrorCode::kParity: return "UART parity check failed";
      case GenericErrorCode::kFrame: return "UART frame error";
      case GenericErrorCode::kOverrun: return "New UART data arrived before old data was read from the hardware";
      case GenericErrorCode::kUnderrun: return "UART hardware is ready for new data, but no data is available for transmission";
      default: return "Unknown UART error";
    }
  }

  // Get an instance of the error category
  static inline const ErrorCategory &getCategory()
  {
    static UartErrorCategory instance;
    return instance;
  }
};


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

  virtual ErrorCode configure(const SettingsList_t *list, uint8_t list_size) = 0;

  virtual ErrorCode read(iDIO &rs485_pin, Buffer_t data, uint32_t timeout, iCallback &event_handler) = 0;

  virtual ErrorCode write(iDIO &rs485_pin, Buffer_t data, uint32_t timeout, iCallback &event_handler) = 0;

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

  virtual ErrorCode configure(const SettingsList_t *list, uint8_t list_size) override
  {
    return m_bus.configure(list, list_size);
  }

  virtual ErrorCode read(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.read(m_rs485_pin, data, timeout, *m_cb_function_rx);
  }

  virtual ErrorCode write(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.write(m_rs485_pin, data, timeout, *m_cb_function_tx);
  }

  virtual uint32_t getBytesRead() override
  {
    return m_bus.getBytesRead();
  }

  virtual ErrorCode setCallback(EventsList_t event, iCallback &event_handler) override
  {
    ErrorCode status = GenericErrorCode::kSuccess;
    switch(event)
    {
    case EVENT_READ:
      m_cb_function_rx = &event_handler;
      break;
    case EVENT_WRITE:
      m_cb_function_tx = &event_handler;
      break;
    default:
      status = GenericErrorCode::kInvalidParameter;
      status.setMessage("Invalid callback event for UART");
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
