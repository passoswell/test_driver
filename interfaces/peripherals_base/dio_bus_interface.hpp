/**
 * @file dio_bus_interface.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-08-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PERIPHERALS_BASE_DIO_BUS_INTERFACE_HPP
#define PERIPHERALS_BASE_DIO_BUS_INTERFACE_HPP


#include <stdint.h>
#include <stdbool.h>

#include "commons.hpp"
#include "peripherals_base/callback_interface.hpp"


/**
 * @brief Interface class for Dio bus
 */
class DioBusInterface
{
public:

  DioBusInterface() = default;

  virtual ~DioBusInterface() = default;

  virtual Status_t configure(const SettingsList_t *list, uint8_t list_size) = 0;

  virtual Status_t read(uint32_t &value) = 0;

  virtual Status_t read(uint32_t pin, bool &value) = 0;

  virtual Status_t write(uint32_t value) = 0;

  virtual Status_t write(uint32_t pin, bool value) = 0;

  virtual Status_t toggle(uint32_t pin_mask) = 0;

  virtual Status_t setEventCallback(uint32_t pin, EventsList_t edge, iCallback &event_handler) = 0;

  virtual Status_t enableEventCallback(uint32_t pin, bool enable) = 0;
};


#endif /* PERIPHERALS_BASE_DIO_BUS_INTERFACE_HPP */
