/**
 * @file com_error_code.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-09-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef COMMONS_COM_ERROR_CODE_HPP
#define COMMONS_COM_ERROR_CODE_HPP

#include <stdint.h>
#include <string_view>

/**
 * @brief Base class for error categories
 */
class ErrorCategory
{
public:

  virtual constexpr std::string_view name() const noexcept = 0;

  virtual constexpr std::string_view message(int error_value) const noexcept = 0;
};

// Forward declare trait (default = false)
template <typename T>
struct is_error_enum : std::false_type {};

class ErrorCode
{
public:
  // Default constructor
  constexpr ErrorCode()
      : m_value(0), m_category(nullptr), m_custom_message(nullptr) {}

  constexpr ErrorCode(int value, const ErrorCategory &category, const char *custom_message = nullptr)
      : m_value(value), m_category(&category), m_custom_message(custom_message) {}

  // Templated constructor for automatic conversion of enums into ErrorCode
  template <typename Enum,
            typename = std::enable_if_t<is_error_enum<Enum>::value>>
  constexpr ErrorCode(Enum error_enum_item)
    : ErrorCode(make_error_code(error_enum_item)) {}

  // Return the error value
  constexpr int value() const noexcept
  {
    return m_value;
  }

  // Return the error category
  constexpr const ErrorCategory& category() const noexcept
  {
    return *m_category;
  }

  // True if success, false if there is an error
  constexpr explicit operator bool() const noexcept
  {
    return m_value != 0;
  }

  // Returns an error message
  std::string_view message() const noexcept
  {
    if (m_custom_message != nullptr)
    {
      return m_custom_message;
    }else if(m_category != nullptr)
    {
      return m_category->message(m_value);
    }else
    {
      return "No category";
    }
  }

  // Set a custom error message
  void setMessage(const std::string_view message)
  {
    m_custom_message = message.data();
  }

private:
  int m_value;
  const ErrorCategory* m_category;
  const char* m_custom_message;
};

#endif /* COMMONS_COM_ERROR_CODE_HPP */
