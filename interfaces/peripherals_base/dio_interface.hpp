/**
 * @file dio_interface.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-08-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PERIPHERALS_BASE_DIO_INTERFACE_HPP
#define PERIPHERALS_BASE_DIO_INTERFACE_HPP


#include <stdint.h>
#include <stdbool.h>

#include "commons.hpp"
#include "peripherals_base/callback_interface.hpp"


/**
 * @brief Error codes for SPI peripherals
 */
enum class DioErrorCode
{
  kSuccess = 0,
  kFailed,
  kInvalidParameter,
  kNotConfigured,
  kNullPointer,
  kBadHandle,
  kTimedOut,
};

/**
 * @brief DIO error codes category
 */
class DioErrorCategory : public ErrorCategory
{
public:
  // Get the error category's name
  constexpr std::string_view name() const noexcept override { return "dio_interface"; }

  // Get the error's helper message
  constexpr std::string_view message(int error_value) const noexcept override
  {
    switch (static_cast<DioErrorCode>(error_value))
    {
      case DioErrorCode::kSuccess: return "Success";
      case DioErrorCode::kInvalidParameter: return "Invalid input parameter";
      case DioErrorCode::kNotConfigured: return "Resource is not properly configured";
      case DioErrorCode::kNullPointer: return "A null pointer was detected";
      case DioErrorCode::kBadHandle: return "Invalid handle to the resource";
      case DioErrorCode::kTimedOut: return "Operation took more time than expected";
      default: return "Unknown DIO error";
    }
  }

  // Get an instance of the error category
  static inline const ErrorCategory& getCategory()
  {
  static DioErrorCategory instance;
  return instance;
  }
};

/**
 * @brief Function overload, convert enum class into an ErrorCode
 *
 * @param error_code A value from enum DioErrorCode
 * @return ErrorCode
 */
inline ErrorCode make_error_code(DioErrorCode error_code)
{
  return {static_cast<int>(error_code), DioErrorCategory::getCategory()};
}

/**
 * @brief Specializing ErrorCode to use the specialized make_error_code's definition above
 */
template <>
struct is_error_enum<DioErrorCode> : std::true_type {};


/**
 * @brief Interface class for DIO
 */
class iDIO
{
public:

  iDIO() = default;

  virtual ~iDIO() = default;

  virtual ErrorCode configure(const SettingsList_t *list, uint8_t list_size) = 0;

  virtual ErrorCode read(bool &state) = 0;

  virtual ErrorCode write(bool value) = 0;

  virtual ErrorCode toggle() = 0;

  virtual ErrorCode setEventCallback(EventsList_t edge, iCallback &event_handler) = 0;

  virtual ErrorCode enableInterruption(bool enable) = 0;
};


#endif /* PERIPHERALS_BASE_DIO_INTERFACE_HPP */
