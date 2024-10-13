/**
 * @file uart_base.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef UART_BASE_HPP
#define UART_BASE_HPP


#include <stdio.h>
#include <stdbool.h>

#include "drivers/base/driver_base/driver_inout_base.hpp"
#include "drivers/linux/utils/linux_types.hpp"

/**
 * @brief Base class for uart drivers
 */
class UartBase : public DriverInOutBase
{
public:

  virtual Status_t configure(const DriverSettings_t *list, uint8_t list_size){;}

  using DriverInOutBase::read;
  virtual Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX){;}

  using DriverInOutBase::write;
  virtual Status_t write(const uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX){;}

  virtual Status_t setCallback(uint8_t event, bool enable, Callback_t function = nullptr, void *user_arg = nullptr){;}
};

#endif /* UART_BASE_HPP */