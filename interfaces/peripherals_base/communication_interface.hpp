/**
 * @file communication_interface.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-08-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PERIPHERALS_BASE_COMMUNICATION_INTERFACE_HPP
#define PERIPHERALS_BASE_COMMUNICATION_INTERFACE_HPP


#include <stdint.h>
#include <stdbool.h>

#include "commons.hpp"

class iComm
{
public:
  virtual Status_t configure(const SettingsList_t *list, uint8_t list_size) = 0;

  virtual Status_t read(Buffer_t data, uint32_t timeout = UINT32_MAX) = 0;

  virtual Status_t write(Buffer_t data, uint32_t timeout = UINT32_MAX) = 0;

  virtual Status_t setCallback(EventsList_t event = EVENT_NONE, Callback_t cb_function = nullptr, void *cb_arg = nullptr) = 0;
};

#endif /* PERIPHERALS_BASE_COMMUNICATION_INTERFACE_HPP */
