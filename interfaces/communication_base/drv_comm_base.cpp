/**
 * @file drv_comm_base.cpp
 * @author user (contact@email.com)
 * @brief Base class with default behavior for communication drivers
 * @date 2024-06-22
 *
 * @copyright Copyright (c) 2024
 *
 */


#include "drv_comm_base.hpp"


/**
 * @brief Constructor
 */
DrvCommBase::DrvCommBase()
{
  m_mutex.key = INOUTSTREAM_EMPTY_KEY;
  m_mutex.lock.clear(std::memory_order_release);
  m_handle = nullptr;
  m_is_initialized = false;
  m_is_read_done = true;
  m_is_write_done = true;
  m_is_operation_done = true;
  m_bytes_read = 0;
}

/**
 * @brief Destructor
 */
DrvCommBase::~DrvCommBase()
{
  // Nothing is done here
}

/**
 * @brief Template for a method to configure a parameter
 * @param parameter An item from DrvCommBaseParameters_t list
 * @param value The corresponding value for the parameter
 * @return Status_t
 */
Status_t DrvCommBase::configure(uint8_t parameter, uint32_t value)
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
Status_t DrvCommBase::configure(const InOutStreamSettings_t *list, uint8_t list_size)
{
  (void) list;
  (void) list_size;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Lock a peripheral to one id
 * @return Status_t
 */
Status_t DrvCommBase::lock(uint32_t key)
{
  switch(key)
  {
    case INOUTSTREAM_MASTER_KEY:
      return STATUS_DRV_SUCCESS;
      break;
    case INOUTSTREAM_EMPTY_KEY:
      return STATUS_DRV_ERR_PARAM;
      break;
    default:
      if (m_mutex.key == key)
      {
        return STATUS_DRV_ERR_BUSY;
      } else if (!m_mutex.lock.test_and_set(std::memory_order_acquire))
      {
        m_mutex.key = key;
        return STATUS_DRV_SUCCESS;
      }
      break;
  }
  return STATUS_DRV_ERR_NOT_OWNED;
}

/**
 * @brief Unlock a peripheral
 * @return Status_t
 */
Status_t DrvCommBase::unlock(uint32_t key)
{
  switch(key)
  {
    case INOUTSTREAM_MASTER_KEY:
      return STATUS_DRV_SUCCESS;
      break;
    case INOUTSTREAM_EMPTY_KEY:
      return STATUS_DRV_ERR_PARAM;
      break;
    default:
      if (m_mutex.key == key)
      {
        m_mutex.key = INOUTSTREAM_EMPTY_KEY;
        m_mutex.lock.clear(std::memory_order_release);
        return STATUS_DRV_SUCCESS;
      }
      break;
  }
  return STATUS_DRV_ERR_NOT_OWNED;
}

/**
 * @brief Template for a method to read data synchronously
 * @param buffer Buffer to store the data
 * @param size Number of bytes to read
 * @param key A parameter with different uses depending on the implementation
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvCommBase::read(uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout)
{
  (void) buffer;
  (void) size;
  (void) key;
  (void) timeout;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Template for a method to read data synchronously
 * @param buffer Buffer to store the data
 * @param key A parameter with different uses depending on the implementation
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvCommBase::read(Buffer_t buffer, uint32_t key, uint32_t timeout)
{
  return read(buffer.data(), buffer.size(), key, timeout);
}

/**
 * @brief Template for a method to write data synchronously
 * @param buffer Buffer where data is stored
 * @param size Number of bytes to write
 * @param key A parameter with different uses depending on the implementation
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvCommBase::write(uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout)
{
  (void) buffer;
  (void) size;
  (void) key;
  (void) timeout;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Template for a method to write data synchronously
 * @param buffer Buffer where data is stored
 * @param key A parameter with different uses depending on the implementation
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvCommBase::write(Buffer_t buffer, uint32_t key, uint32_t timeout)
{
  return write(buffer.data(), buffer.size(), key, timeout);
}

/**
 * @brief Template for a method to read data asynchronously
 * @param buffer Buffer to store the data
 * @param size Number of bytes to read
 * @param key A parameter with different uses depending on the implementation
 * @param func Pointer to a function to call at the end of operation
 * @param arg Parameter to pass to the callback function
 * @return Status_t
 */
Status_t DrvCommBase::readAsync(uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout, InOutStreamCallback_t func, void *arg)
{
  (void) buffer;
  (void) size;
  (void) key;
  (void) timeout;
  (void) func;
  (void) arg;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Template for a method to read data asynchronously
 * @param buffer Buffer to store the data
 * @param key A parameter with different uses depending on the implementation
 * @param func Pointer to a function to call at the end of operation
 * @param arg Parameter to pass to the callback function
 * @return Status_t
 */
Status_t DrvCommBase::readAsync(Buffer_t buffer, uint32_t key, uint32_t timeout, InOutStreamCallback_t func, void *arg)
{
  return readAsync(buffer.data(), buffer.size(), key, timeout, func, arg);
}

/**
 * @brief Template for a method to abort an ongoing readAsync operation
 */
Status_t DrvCommBase::abortReadAsync()
{
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Asynchronous read operation status
 * @return true if operation done, false otherwise
 */
bool DrvCommBase::isReadAsyncDone()
{
  return m_is_read_done;
}

/**
 * @brief Template for a method to return the number of bytes read in the last readAsync operation
 * @return uint32_t
 */
uint32_t DrvCommBase::bytesRead()
{
  return m_bytes_read;
}

/**
 * @brief Template for a method to write data asynchronously
 * @param buffer Buffer where data is stored
 * @param size Number of bytes to write
 * @param key A parameter with different uses depending on the implementation
 * @param func Pointer to a function to call at the end of operation
 * @param arg Parameter to pass to the callback function
 * @return Status_t
 */
Status_t DrvCommBase::writeAsync(const uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout, InOutStreamCallback_t func, void *arg)
{
  (void) buffer;
  (void) size;
  (void) key;
  (void) timeout;
  (void) func;
  (void) arg;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Template for a method to write data asynchronously
 * @param buffer Buffer where data is stored
 * @param key A parameter with different uses depending on the implementation
 * @param func Pointer to a function to call at the end of operation
 * @param arg Parameter to pass to the callback function
 * @return Status_t
 */
Status_t DrvCommBase::writeAsync(const Buffer_t buffer, uint32_t key, uint32_t timeout, InOutStreamCallback_t func, void *arg)
{
  return writeAsync(buffer.data(), buffer.size(), key, timeout, func, arg);
}

/**
 * @brief Template for a method to abort an ongoing writeAsync operation
 */
Status_t DrvCommBase::abortWriteAsync()
{
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Asynchronous write operation status
 * @return true if operation done, false otherwise
 */
bool DrvCommBase::isWriteAsyncDone()
{
  return m_is_write_done;
}

/**
 * @brief Template for a method to return the number of bytes written in the last writeAsync operation
 * @return uint32_t
 */
uint32_t DrvCommBase::bytesWritten()
{
  return 0;
}

/**
 * @brief Asynchronous operation (read, write or read-write) status
 * @return true if operation done, false otherwise
 */
bool DrvCommBase::isOperationDone()
{
  return m_is_operation_done;
}

/**
 * @brief Generate a unique ID for a device
 * @return uint8_t
 */
uint8_t DrvCommBase::getID()
{
  static uint8_t id_generator = INOUTSTREAM_INVALID_ID + 1;

  if(id_generator != INOUTSTREAM_INVALID_ID)
  {
    id_generator++;
  }

  return id_generator;
}
