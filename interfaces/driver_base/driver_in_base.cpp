/**
 * @file driver_in_base.cpp
 * @author your name (you@domain.com)
 * @brief Base class for input drivers
 * @version 0.1
 * @date 2024-10-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "driver_in_base.hpp"

/**
 * @brief Constructor
 */
DriverInBase::DriverInBase()
{
  m_bytes_read = 0;
  m_bytes_available = 0;
  m_func_rx = nullptr;
  m_arg_rx = nullptr;
  m_read_status = STATUS_DRV_NOT_CONFIGURED;
  m_is_async_mode_rx = false;
}

/**
 * @brief Destructor
 */
DriverInBase::~DriverInBase()
{
  // Nothing is done here
}

/**
 * @brief Read an integer
 *
 * @param data Buffer to store the integer read
 * @return Status_t
 */
Status_t DriverInBase::read(bool &data)
{
  (void) data;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Read a float
 *
 * @param data Buffer to store the float read
 * @return Status_t
 */
Status_t DriverInBase::read(float &data)
{
  (void) data;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Read a certain amount of bytes
 *
 * @param data Buffer to store the data
 * @param byte_count Number of bytes to read
 * @return Status_t
 */
Status_t DriverInBase::read(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  (void) data;
  (void) byte_count;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Read a certain amount of bytes
 *
 * @param data Buffer and size
 * @return Status_t
 */
Status_t DriverInBase::read(Buffer_t data, uint32_t timeout)
{
  return read(data.data(), data.size(), timeout);
}

/**
 * @brief Get number of bytes in the driver's internal read buffer
 *
 * @return Size_t
 */
Size_t DriverInBase::getBytesAvailable()
{
  return m_bytes_available;
}

/**
 * @brief Get number of bytes read in the last operation
 *
 * @return Size_t
 */
Size_t DriverInBase::getBytesRead()
{
  return m_bytes_read;
}

/**
 * @brief Get read operation status
 *
 * @return Status_t
 */
Status_t DriverInBase::getReadStatus()
{
  return m_read_status;
}