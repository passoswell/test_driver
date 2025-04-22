/**
 * @file dio_base.hpp
 * @author your name (you@domain.com)
 * @brief Interface class for DIO
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

#include "driver_base/driver_inout_base.hpp"

constexpr uint32_t DIO_INVALID_LINE = UINT32_MAX;
constexpr uint32_t DIO_INVALID_PORT = UINT32_MAX;

/**
 * @brief Base class for drivers on digital inputs and outputs
 */
class DioBase : public DriverInOutBase
{
public:

  virtual Status_t configure(const SettingsList_t *list, uint8_t list_size) = 0;

  virtual Status_t read(bool &state) = 0;

  virtual Status_t write(bool value) = 0;

  virtual Status_t toggle() = 0;

  virtual Status_t setCallback(EventsList_t edge = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr) = 0;

  virtual Status_t enableCallback(bool enable, EventsList_t event = EVENT_NONE) = 0;
};

#endif /* DIO_BASE_HPP */
