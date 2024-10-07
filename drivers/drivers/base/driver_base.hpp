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
  Callback_t m_func;
  void *m_arg;

  DriverBase();
  virtual ~DriverBase();

  // Initialization function
  virtual Status_t configure(const DriverParamList_t *list, uint8_t list_size);

  // Callback
  virtual Status_t setCallback(uint8_t event, Callback_t function, void *user_arg = nullptr);
};

#endif  // DRIVER_HPP