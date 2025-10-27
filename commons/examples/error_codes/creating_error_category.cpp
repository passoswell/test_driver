/**
 * @file creating_error_category.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-10-26
 *
 * @copyright Copyright (c) 2025
 *
 */


// This example shows:

// 1. How to define your own error enum
// 2. How to create a custom error category class implementing the ErrorCategory interface
// 3. How to provide meaningful error messages for each error code
// 4. How to enable automatic conversion from your enum to ErrorCode
// 5. Different ways to create error codes using your custom category

#include <cstring>

#include "drivers.hpp"


// First, define your custom error codes enum
enum class NetworkErrorCode
{
  kSuccess = 0,
  kConnectionLost,
  kTimeout,
  kInvalidAddress,
  kPortInUse,
  kDnsError
};

// Create your custom error category class
class NetworkErrorCategory : public ErrorCategory
{
public:
  // Implement the name() method to identify your category
  constexpr std::string_view name() const noexcept override
  {
    return "network";
  }

  // Implement the message() method to provide descriptive messages for each error code
  constexpr std::string_view message(int error_value) const noexcept override
  {
    switch (static_cast<NetworkErrorCode>(error_value))
    {
    case NetworkErrorCode::kSuccess:
      return "Operation successful";
    case NetworkErrorCode::kConnectionLost:
      return "Connection lost";
    case NetworkErrorCode::kTimeout:
      return "Operation timed out";
    case NetworkErrorCode::kInvalidAddress:
      return "Invalid network address";
    case NetworkErrorCode::kPortInUse:
      return "Port already in use";
    case NetworkErrorCode::kDnsError:
      return "DNS resolution failed";
    default:
      return "Unknown network error";
    }
  }

  // Provide a static method to get the singleton instance
  static inline const ErrorCategory &getCategory()
  {
    static NetworkErrorCategory instance;
    return instance;
  }
};

// Create a makeErrorCode function for your enum
constexpr ErrorCode makeErrorCode(
    NetworkErrorCode error_code,
    const ErrorCategory &category = NetworkErrorCategory::getCategory(),
    const char *custom_message = nullptr)
{
  return ErrorCode(static_cast<int>(error_code), category, custom_message);
}

// Enable automatic conversion from your enum to ErrorCode
template <>
struct is_error_enum<NetworkErrorCode> : std::true_type
{
};

// Usage example:
AP_MAIN()
{
  // Create error code directly from enum
  ErrorCode error1 = NetworkErrorCode::kConnectionLost;
  std::printf("%s (%d): %s\r\n", error1.category().name().data(), error1.value(), error1.message().data());

  // Create with custom message
  ErrorCode error2(NetworkErrorCode::kTimeout, NetworkErrorCategory::getCategory(), "Connection timed out after 30 seconds");
  std::printf("%s (%d): %s\r\n", error2.category().name().data(), error2.value(), error2.message().data());

  // Create using makeErrorCode
  ErrorCode error3 = makeErrorCode(NetworkErrorCode::kPortInUse);
  std::printf("%s (%d): %s\r\n", error3.category().name().data(), error3.value(), error3.message().data());

  AP_EXIT();
}