/**
 * @file dio_interface.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-08-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PERIPHERALS_BASE_DIO_INTERFACE_HPP
#define PERIPHERALS_BASE_DIO_INTERFACE_HPP


#include <stdint.h>
#include <stdbool.h>

#include "commons.hpp"
#include "peripherals_base/callback_interface.hpp"


/**
 * @brief Interface class for DIO
 */
class iDIO
{
public:

  iDIO() = default;

  virtual ~iDIO() = default;

  virtual Status_t configure(const SettingsList_t *list, uint8_t list_size) = 0;

  virtual Status_t read(bool &state) = 0;

  virtual Status_t write(bool value) = 0;

  virtual Status_t toggle() = 0;

  virtual Status_t setEventCallback(EventsList_t edge, iCallback &event_handler) = 0;

  virtual Status_t enableInterruption(bool enable) = 0;
};


#endif /* PERIPHERALS_BASE_DIO_INTERFACE_HPP */
