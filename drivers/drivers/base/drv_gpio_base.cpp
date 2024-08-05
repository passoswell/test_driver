/**
 * @file drv_gpio_base.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "drv_gpio_base.hpp"

/**
 * @brief Template for a method to configure a parameter
 * @param parameter An item from DrvGpioConfigureList_t list
 * @param value The corresponding value for the parameter
 * @return Status_t
 */
Status_t DrvGpioBase::configure(uint8_t parameter, uint32_t value)
{
  (void) parameter;
  (void) value;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Template for a method to configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t DrvGpioBase::configure(const DrvGpioConfigureList_t *list, uint8_t list_size)
{
  (void) list;
  (void) list_size;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Template method to read from a digital pin
 * @param state The state of the digital pin
 * @return Status_t
 */
Status_t DrvGpioBase::read(bool &state)
{
  (void) state;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Template method to write to a digital output pin
 * @param state The state to set in the gpio
 * @return Status_t
 */
Status_t DrvGpioBase::write(bool state)
{
  (void) state;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Template function to install an event callback function
 *
 * @param edge The edge that will trigger the event
 * @param func The callback function
 * @param arg A user parameter
 * @return Status_t
 */
Status_t DrvGpioBase::setCallback(DrvGpioEdge_t edge, DrvGpioCallback_t func, void *arg)
{
  (void) edge;
  (void) func;
  (void) arg;
  return STATUS_DRV_NOT_IMPLEMENTED;
}
