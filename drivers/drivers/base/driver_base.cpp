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
  m_bytes_written = 0;
  m_bytes_read = 0;
  m_write_status = STATUS_DRV_NOT_CONFIGURED;
  m_read_status = STATUS_DRV_NOT_CONFIGURED;
}

/**
 * @brief Destructor
 */
DriverBase::~DriverBase()
{
  // Nothing is done here
}