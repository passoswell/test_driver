/**
 * @file iic_base.hpp
 * @author your name (you@domain.com)
 * @brief Interface class for IIC
 * @version 0.1
 * @date 2024-10-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef IIC_BASE_HPP
#define IIC_BASE_HPP


#include <stdio.h>
#include <stdbool.h>

#include "drivers/base/driver_base/driver_inout_base.hpp"
#include "drivers/linux/utils/linux_types.hpp"

/**
 * @brief Base class for iic drivers
 */
class IicBase : public DriverInOutBase
{
public:

  virtual Status_t configure(const DriverSettings_t *list, uint8_t list_size) = 0;

  virtual void setAddress(uint16_t address) = 0;

  using DriverInOutBase::read;
  virtual Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX) = 0;

  using DriverInOutBase::write;
  virtual Status_t write(const uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX) = 0;

  virtual Status_t setCallback(uint8_t event, bool enable, Callback_t function = nullptr, void *user_arg = nullptr) = 0;
};

#endif /* IIC_BASE_HPP */