/**
 * @file driver_base.cpp
 * @author your name (you@domain.com)
 * @brief Base class for drivers
 * @version 0.1
 * @date 2024-10-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "driver_base.hpp"

/**
 * @brief Constructor
 */
DriverBase::DriverBase()
{
  m_func = nullptr;
  m_arg = nullptr;
}

/**
 * @brief Destructor
 */
DriverBase::~DriverBase()
{
  // Nothing is done here
}

/**
 * @brief Configure a list of parameters
 *
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t DriverBase::configure(const DriverSettings_t *list, uint8_t list_size)
{
  (void) list;
  (void) list_size;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Install a callback function
 *
 * @param event An event to trigger the call
 * @param function A function
 * @param user_arg A argument used as a parameter to the function
 * @return Status_t
 */
Status_t DriverBase::setCallback(uint8_t event, bool enable, Callback_t function, void *user_arg)
{
  (void) event;
  (void) function;
  (void) user_arg;
  return STATUS_DRV_NOT_IMPLEMENTED;
}