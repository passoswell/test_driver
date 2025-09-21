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
 * @brief Error codes for SPI peripherals
 */
enum class UartErrorCode
{
  kSuccess = 0,
  kFailed,
  kInvalidParameter,
  kNotConfigured,
  kNullPointer,
  kBadHandle,
  kTimedOut,                /*!< Operation took more time than expected */
  kBusy,                    /*!< Bus already in use by another controller */
  kParity,                  /*!< Parity error */
  kFrame,                   /*!< Frame error */
  kOverrun,                 /*!< Overrun */
  kUnderrun,                /*!< Underrun */
};

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
    switch (static_cast<UartErrorCode>(error_value))
    {
      case UartErrorCode::kSuccess: return "Success";
      case UartErrorCode::kInvalidParameter: return "Invalid input parameter";
      case UartErrorCode::kNotConfigured: return "Resource is not properly configured";
      case UartErrorCode::kNullPointer: return "A null pointer was detected";
      case UartErrorCode::kBadHandle: return "Invalid handle to the resource";
      case UartErrorCode::kTimedOut: return "Operation took more time than expected";
      case UartErrorCode::kBusy: return "Bus already in use by another controller";
      case UartErrorCode::kParity: return "Parity check failed";
      case UartErrorCode::kFrame: return "Frame error";
      case UartErrorCode::kOverrun: return "New data arrived before old data was read from the hardware";
      case UartErrorCode::kUnderrun: return "Hardware is ready for new data, but no data is available for transmission";
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

/**
 * @brief Function overload, convert enum class into an ErrorCode
 *
 * @param error_code A value from enum UartErrorCode
 * @return ErrorCode
 */
inline ErrorCode makeErrorCode(UartErrorCode error_code)
{
  return {static_cast<int>(error_code), UartErrorCategory::getCategory()};
}

/**
 * @brief Specializing ErrorCode to use the specialized makeErrorCode's definition above
 */
template <>
struct is_error_enum<UartErrorCode> : std::true_type {};


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
    ErrorCode status = UartErrorCode::kSuccess;
    switch(event)
    {
    case EVENT_READ:
      m_cb_function_rx = &event_handler;
      break;
    case EVENT_WRITE:
      m_cb_function_tx = &event_handler;
      break;
    default:
      status = UartErrorCode::kInvalidParameter;
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
