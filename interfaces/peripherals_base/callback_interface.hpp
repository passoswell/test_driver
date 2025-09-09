/**
 * @file callback_interface.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-09-07
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PERIPHERALS_BASE_CALLBACK_INTERFACE_HPP
#define PERIPHERALS_BASE_CALLBACK_INTERFACE_HPP


#include <stdint.h>
#include <stdbool.h>
#include <string>

#include "commons.hpp"

class iCallback
{
public:

  iCallback() = default;

  virtual ~iCallback() = default;

  // Returns a short string identifying the callback owner
  virtual std::string name() = 0;

  // Called when the asynchronous operation is about to start
  virtual void onStart() = 0;

  // Called when an event occur if applicable
  virtual void onEvent(Status_t status, EventsList_t event, const Buffer_t data) = 0;

  // Called when an event occur if applicable
  virtual void onEvent(Status_t status, EventsList_t event, const Buffer_t rx_data, const Buffer_t tx_data) = 0;
};

#endif /* PERIPHERALS_BASE_CALLBACK_INTERFACE_HPP */
