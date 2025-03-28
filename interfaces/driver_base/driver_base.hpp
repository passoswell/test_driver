/**
 * @file driver.hpp
 * @author your name (you@domain.com)
 * @brief Base class for drivers
 * @version 0.1
 * @date 2024-10-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVER_BASE_HPP
#define DRIVER_BASE_HPP

#include "commons.hpp"
#include "driver_base_types.hpp"

class DriverBase
{
public:
  void const *m_handle;
  DriverCallback_t m_func;
  void *m_arg;
  bool m_is_async_mode;

  DriverBase();
  virtual ~DriverBase();

  // Initialization function
  virtual Status_t configure(const DriverSettings_t *list, uint8_t list_size);

  // Callback installation
  virtual Status_t setCallback(DriverEventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

  // Enable / disable callback
  virtual Status_t enableCallback(bool enable, DriverEventsList_t event = EVENT_NONE);
};

#endif /* DRIVER_BASE_HPP */