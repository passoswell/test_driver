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


#include <cstdint>
#include <cstdbool>

#include "commons.hpp"
#include "peripherals_base/callback_interface.hpp"


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
    switch (static_cast<GenericErrorCode>(error_value))
    {
      case GenericErrorCode::kSuccess: return "Success";
      case GenericErrorCode::kInvalidParameter: return "Invalid DIO input parameter";
      case GenericErrorCode::kNotConfigured: return "DIO is not properly configured";
      case GenericErrorCode::kNullPointer: return "A null pointer was detected on DIO";
      case GenericErrorCode::kBadHandle: return "Invalid handle to the DIO";
      case GenericErrorCode::kTimedOut: return "DIO operation took more time than expected";
      case GenericErrorCode::kFailed: return "Unknown DIO error";
      default: return generic_category.message(error_value); // Using GenericErrorCode with generic error messages
    }
  }

  // Get an instance of the error category
  static inline const ErrorCategory &getCategory()
  {
    static DioErrorCategory instance;
    return instance;
  }
};


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
