/**
 * @file event_interface.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-09-07
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PERIPHERALS_BASE_EVENT_INTERFACE_HPP
#define PERIPHERALS_BASE_EVENT_INTERFACE_HPP

#include <string>
#include <system_error>

/**
 * @brief Base interface for event categories
 */
class iEvent
{
public:

  iEvent() = default;

  virtual ~iEvent() = default;

  virtual const char* name() const noexcept = 0;
  virtual std::string message(int ev) const = 0;
};

/**
 * @brief Main EventCode_t class
 */
class EventCode_t final
{
public:
  EventCode_t() noexcept : m_value(0), m_category(nullptr){}

  EventCode_t(int value, const iEvent& category) noexcept
    : m_value(value), m_category(&category)
  {
  }

  ~EventCode_t() = default;

  int value() const noexcept
  {
    return m_value;
  }

  const iEvent& category() const noexcept
  {
    return *m_category;
  }

  std::string message() const
  {
    return category().message(value());
  }

private:
  int m_value;
  const iEvent* m_category;
};


#endif /* PERIPHERALS_BASE_EVENT_INTERFACE_HPP */
