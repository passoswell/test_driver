/**
 * @file driver_out_base.cpp
 * @author your name (you@domain.com)
 * @brief Base class for output drivers
 * @version 0.1
 * @date 2024-10-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "driver_out_base.hpp"

/**
 * @brief Constructor
 */
DriverOutBase::DriverOutBase()
{
  m_bytes_written = 0;
  m_bytes_pending = 0;
  m_func_tx = nullptr;
  m_arg_tx = nullptr;
  m_write_status = STATUS_DRV_NOT_CONFIGURED;
  m_is_async_mode_tx = false;
}

/**
 * @brief Destructor
 */
DriverOutBase::~DriverOutBase()
{
  // Nothing is done here
}

/**
 * @brief Write one integer
 *
 * @param data Integer to write
 * @return Status_t
 */
Status_t DriverOutBase::write(bool data)
{
  (void) data;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Write one float
 *
 * @param data Float to write
 * @return Status_t
 */
Status_t DriverOutBase::write(float data)
{
  (void) data;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Write a certain amount of bytes
 *
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @return Status_t
 */
Status_t DriverOutBase::write(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  (void) data;
  (void) byte_count;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Write a certain amount of bytes
 *
 * @param data Buffer and size
 * @return Status_t
 */
Status_t DriverOutBase::write(Buffer_t data, uint32_t timeout)
{
  return write(data.data(), data.size(), timeout);
}

/**
 * @brief Get the number of bytes on the driver's internal write buffer
 *
 * @return Size_t
 */
Size_t DriverOutBase::getBytesPending()
{
  return m_bytes_pending;
}

/**
 * @brief Get number of bytes written in the last operation
 *
 * @return Size_t
 */
Size_t DriverOutBase::getBytesWritten()
{
  return m_bytes_written;
}

/**
 * @brief Get write operation status
 *
 * @return Status_t
 */
Status_t DriverOutBase::getWriteStatus()
{
  return m_write_status;
}