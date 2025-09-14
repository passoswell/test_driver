/**
 * @file iic_interface.hpp
 * @author your name (you@domain.com)
 * @brief Interface class and methods for IIC
 * @version 0.1
 * @date 2025-08-05
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PERIPHERALS_BASE_IIC_INTERFACE_HPP
#define PERIPHERALS_BASE_IIC_INTERFACE_HPP


#include <stdint.h>
#include <stdbool.h>

#include "commons.hpp"
#include "peripherals_base/communication_interface.hpp"


/**
 * @brief Error codes for IIC peripherals
 */
enum class IicErrorCode
{
  kSuccess = 0,
  kFailed,
  kNotImplemented,
  kInvalidParameter,
  kNotConfigured,
  kNullPointer,
  kBadHandle,
  kTimedOut,                /*!< Operation took more time than expected */
  kAddressNotAcknowledged,  /*!< The address was not acknowledged */
  kBusy,                    /*!< Bus already in use by another controller */
  kBusError,                /*!< Some device needs fault recovery */
  kArbitrationLost,         /*!< Multi-master arbitration lost */
};

/**
 * @brief IIC error codes category
 */
class IicErrorCategory : public ErrorCategory
{
public:
  // Get the error category's name
  constexpr std::string_view name() const noexcept override { return "i2c_interface"; }

  // Get the error's helper message
  constexpr std::string_view message(int error_value) const noexcept override
  {
    switch (static_cast<IicErrorCode>(error_value))
    {
      case IicErrorCode::kSuccess: return "Success";
      case IicErrorCode::kNotImplemented: return "Feature not implemented";
      case IicErrorCode::kInvalidParameter: return "Invalid input parameter";
      case IicErrorCode::kNotConfigured: return "Resource is not properly configured";
      case IicErrorCode::kNullPointer: return "A null pointer was detected";
      case IicErrorCode::kBadHandle: return "Invalid handle to the resource";
      case IicErrorCode::kTimedOut: return "Operation took more time than expected";
      case IicErrorCode::kAddressNotAcknowledged: return "The address was not acknowledged";
      case IicErrorCode::kBusy: return "Bus already in use by another controller";
      case IicErrorCode::kBusError: return "Some device or the peripheral needs fault recovery";
      case IicErrorCode::kArbitrationLost: return "Multi-master arbitration lost";
      default: return "Unknown IIC error";
    }
  }

  // Get an instance of the error category
  static inline const ErrorCategory& getCategory()
  {
  static IicErrorCategory instance;
  return instance;
  }
};

/**
 * @brief Function overload, convert enum class into an ErrorCode
 *
 * @param error_code A value from enum IicErrorCode
 * @return ErrorCode
 */
inline ErrorCode make_error_code(IicErrorCode error_code)
{
  return {static_cast<int>(error_code), IicErrorCategory::getCategory()};
}

/**
 * @brief Specializing ErrorCode to use the specialized make_error_code's definition above
 */
template <>
struct is_error_enum<IicErrorCode> : std::true_type {};


/**
 * @brief Type definition for IIC port number
 */
typedef uint16_t IicHandle_t;

/**
 * @brief Data used during asynchronous IIC transfers
 */
typedef struct
{
  bool run;
  DrvBuffer_t data;
  uint32_t timeout;
  uint16_t address;
  iCallback *event_handle;
} IicDataBundle2_t;

/**
 * @brief Data used during asynchronous IIC transfers
 */
typedef struct
{
  IicDataBundle2_t rx;
  IicDataBundle2_t tx;
} IicDataBundle_t;

/**
 * @brief Interface class for IIC bus
 */
class iIicBus
{
public:

  iIicBus() = default;

  virtual ~iIicBus() = default;

  virtual ErrorCode configure(const SettingsList_t *list, uint8_t list_size) = 0;

  virtual ErrorCode read(uint16_t address, Buffer_t data, uint32_t timeout, iCallback &event_handler) = 0;

  virtual ErrorCode write(uint16_t address, Buffer_t data, uint32_t timeout, iCallback &event_handler) = 0;
};

/**
 * @brief Interface class for IIC peripherals
 */
class iIIC : public iComm
{
public:

  iIIC() = default;

  virtual ~iIIC() = default;

};

/**
 * @brief Base class for IIC peripherals
 */
class bIIC : public iIIC
{
public:

  bIIC(iIicBus &bus, uint16_t address) :
  m_bus(bus), m_address(address),
  m_cb_function_rx(nullptr), m_cb_function_tx(nullptr)
  {
  }

  virtual ~bIIC() = default;

  virtual ErrorCode configure(const SettingsList_t *list, uint8_t list_size) override
  {
    return m_bus.configure(list, list_size);
  }

  virtual ErrorCode read(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.read(m_address, data, timeout, *m_cb_function_rx);
  }

  virtual ErrorCode write(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.write(m_address, data, timeout, *m_cb_function_tx);
  }

  virtual ErrorCode setCallback(EventsList_t event, iCallback &event_handler) override
  {
    ErrorCode status = IicErrorCode::kSuccess;
    switch(event)
    {
    case EVENT_READ:
      m_cb_function_rx = &event_handler;
      break;
    case EVENT_WRITE:
      m_cb_function_tx = &event_handler;
      break;
    default:
      status = IicErrorCode::kInvalidParameter;
      break;
    }
    return status;
  }

protected:
  iIicBus &m_bus;
  uint16_t m_address;
  iCallback *m_cb_function_rx, *m_cb_function_tx;
};

#endif /* PERIPHERALS_BASE_IIC_INTERFACE_HPP */
