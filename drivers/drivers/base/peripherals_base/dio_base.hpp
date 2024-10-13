/**
 * @file dio_base.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DIO_BASE_HPP
#define DIO_BASE_HPP


#include <stdio.h>
#include <stdbool.h>

#include "drivers/base/driver_base/driver_inout_base.hpp"
#include "drivers/linux/utils/linux_types.hpp"

constexpr uint32_t DIO_INVALID_LINE = UINT32_MAX;
constexpr uint32_t DIO_INVALID_PORT = UINT32_MAX;

/**
 * @brief Base class for drivers on digital inputs and outputs
 */
class DioBase : public DriverInOutBase
{
public:

  virtual Status_t configure(const DriverSettings_t *list, uint8_t list_size);

  virtual Status_t read(uint32_t &state);

  virtual Status_t write(uint32_t value);

  virtual Status_t toggle();

  virtual Status_t setCallback(uint8_t edge, bool enable, Callback_t function = nullptr, void *user_arg = nullptr);
};

#endif /* DIO_BASE_HPP */
