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

#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "commons.hpp"
#include "driver_base_types.hpp"

class DriverBase
{
public:
  void *m_handle;
  Callback_t m_func;
  void *m_arg;
  bool m_is_async_mode;

  DriverBase();
  virtual ~DriverBase();

  // Initialization function
  virtual Status_t configure(const DriverSettings_t *list, uint8_t list_size) = 0;

  // Callback
  virtual Status_t setCallback(uint8_t event, bool enable, Callback_t function = nullptr, void *user_arg = nullptr) = 0;
};

#endif  // DRIVER_HPP