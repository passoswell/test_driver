/**
 * @file driver_in_base.cpp
 * @author your name (you@domain.com)
 * @brief
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
  m_read_status = STATUS_DRV_NOT_CONFIGURED;
}

/**
 * @brief Destructor
 */
DriverInBase::~DriverInBase()
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
Status_t DriverInBase::configure(const DriverParamList_t *list, uint8_t list_size)
{
  (void) list;
  (void) list_size;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Read an integer
 *
 * @param data Buffer to store the integer read
 * @return Status_t
 */
Status_t DriverInBase::read(uint32_t &data)
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
Status_t DriverInBase::read(uint8_t *data, Size_t byte_count)
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
Status_t DriverInBase::read(Buffer_t data)
{
  return read(data.data(), data.size());
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
  return STATUS_DRV_NOT_IMPLEMENTED;
}