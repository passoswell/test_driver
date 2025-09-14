/**
 * @file linux_serial_file.hpp
 * @author your name (you@domain.com)
 * @brief Gives access to a linux tty file as a serial port
 * @version 0.1
 * @date 2025-03-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef DRIVERS_LINUX_UTILS_LINUX_SERIAL_FILE_HPP
#define DRIVERS_LINUX_UTILS_LINUX_SERIAL_FILE_HPP

#include <stdio.h>
#include <stdbool.h>

#include "peripherals_base/communication_interface.hpp"
#include "linux/utils/linux_types.hpp"
#include "linux/task_system/task_system.hpp"


/**
 * @brief Error codes
 */
enum class SerialFileErrorCode
{
  kSuccess = 0,
  kFailed,
  kInvalidParameter,
  kNotConfigured,
  kNullPointer,
  kBadHandle,
  kTimedOut,                /*!< Operation took more time than expected */
  kBusy,                    /*!< Bus already in use by another controller */
};

/**
 * @brief Error codes category
 */
class SerialFileErrorCategory : public ErrorCategory
{
public:
  // Get the error category's name
  constexpr std::string_view name() const noexcept override { return "i2c_interface"; }

  // Get the error's helper message
  constexpr std::string_view message(int error_value) const noexcept override
  {
    switch (static_cast<SerialFileErrorCode>(error_value))
    {
      case SerialFileErrorCode::kSuccess: return "Success";
      case SerialFileErrorCode::kInvalidParameter: return "Invalid input parameter";
      case SerialFileErrorCode::kNotConfigured: return "Resource is not properly configured";
      case SerialFileErrorCode::kNullPointer: return "A null pointer was detected";
      case SerialFileErrorCode::kBadHandle: return "Invalid handle to the resource";
      case SerialFileErrorCode::kTimedOut: return "Operation took more time than expected";
      case SerialFileErrorCode::kBusy: return "Bus already in use by another controller";
      default: return "Unknown IIC error";
    }
  }

  // Get an instance of the error category
  static inline const ErrorCategory& getCategory()
  {
  static SerialFileErrorCategory instance;
  return instance;
  }
};

/**
 * @brief Function overload, convert enum class into an ErrorCode
 *
 * @param error_code A value from enum SerialFileErrorCode
 * @return ErrorCode
 */
inline ErrorCode make_error_code(SerialFileErrorCode error_code)
{
  return {static_cast<int>(error_code), SerialFileErrorCategory::getCategory()};
}

/**
 * @brief Specializing ErrorCode to use the specialized make_error_code's definition above
 */
template <>
struct is_error_enum<SerialFileErrorCode> : std::true_type {};

/**
 * @brief
 */
class LinuxSerialFile : public iComm
{
public:

  LinuxSerialFile(const char *file_name);

  virtual ~LinuxSerialFile();

  ErrorCode configure(const SettingsList_t *list, uint8_t list_size) override;

  ErrorCode read(Buffer_t data, uint32_t timeout = UINT32_MAX) override;

  ErrorCode write(Buffer_t data, uint32_t timeout = UINT32_MAX) override;

  ErrorCode setCallback(EventsList_t event, iCallback &event_handler) override;

  uint32_t getBytesRead()
  {
    return m_bytes_read;
  }

private:
  Task<DrvDataBundle_t, 1, ErrorCode, 0> m_rx_thread_handle;
  Task<DrvDataBundle_t, 1, ErrorCode, 0> m_tx_thread_handle;
  const char *m_handle;
  int m_linux_handle;
  bool m_terminate;
  bool m_is_async_mode_rx, m_is_async_mode_tx;
  uint32_t m_bytes_read;
  iCallback *m_event_handler_rx, *m_event_handler_tx;

  ErrorCode readBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back);
  static ErrorCode readFromThreadBlocking(DrvDataBundle_t data_bundle, void *self_ptr);

  ErrorCode writeBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back);
  static ErrorCode writeFromThreadBlocking(DrvDataBundle_t data_bundle, void *self_ptr);

  ErrorCode checkInputs(const DrvBuffer_t data, uint32_t timeout);
};

#endif /* DRIVERS_LINUX_UTILS_LINUX_SERIAL_FILE_HPP */