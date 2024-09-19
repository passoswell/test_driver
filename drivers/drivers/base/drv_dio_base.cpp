/**
 * @file drv_dio_base.cpp
 * @author your name (you@domain.com)
 * @brief Base class for drivers on digital inputs and outputs
 * @version 0.1
 * @date 2024-07-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "drv_dio_base.hpp"

/**
 * @brief Template for a method to configure a parameter
 * @param parameter An item from DioConfigureList_t list
 * @param value The corresponding value for the parameter
 * @return Status_t
 * @note
 *       If implemented, should perform the configuration while keeping
 *       untouched other settings as much as possible
 */
Status_t DrvDioBase::configure(uint8_t parameter, uint32_t value)
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
 * @note
 *       Return an error if list is null or if list_size is zero
 *       Default configurations if some parameters are missing:
 *       DIO_LINE_DIRECTION should default to DIO_DIRECTION_INPUT
 *       DIO_LINE_DRIVE should default to DIO_DRIVE_PUSH_PULL
 *       DIO_LINE_BIAS should default to DIO_BIAS_DISABLED
 *       DIO_LINE_EDGE should be ignored
 *       DIO_LINE_INITIAL_VALUE should default to false (0, zero, low)
 *       DIO_LINE_ACTIVE_STATE should default to true (1, one, high)
 */
Status_t DrvDioBase::configure(const DioSettings_t *list, uint8_t list_size)
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
Status_t DrvDioBase::read(bool &state)
{
  (void) state;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Template method to write to a digital output pin
 * @param state The state to set in the dio
 * @return Status_t
 */
Status_t DrvDioBase::write(bool state)
{
  (void) state;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Template method to toggle the state of a digital output
 * @return Status_t
 */
Status_t DrvDioBase::toggle()
{
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Template function to install an event callback function
 * @param edge The edge that will trigger the event
 * @param func The callback function
 * @param arg A user parameter
 * @return Status_t
 * @note
 *       The method must configure the dio interruption according to the edge
 *       Return error if func is null
 */
Status_t DrvDioBase::setCallback(DioEdge_t edge, DioCallback_t func, void *arg)
{
  (void) edge;
  (void) func;
  (void) arg;
  return STATUS_DRV_NOT_IMPLEMENTED;
}
