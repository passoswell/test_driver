/**
 * @file drv_comm_base.cpp
 * @author user (contact@email.com)
 * @brief Base class with default behavior for communication drivers
 * @date 2024-06-22
 *
 * @copyright Copyright (c) 2024
 *
 */


#include "drivers/base/drv_comm_base.hpp"


/**
 * @brief Constructor
 */
DrvCommBase::DrvCommBase()
{
  m_mutex.key = INOUTSTREAM_NO_KEY;
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
 * @param parameter
 * @param p_value
 * @return Status_t
 */
Status_t DrvCommBase::configure(uint8_t parameter, const void *p_value)
{
  (void) parameter;
  (void) p_value;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Template for a method to configure a list of parameters
 * @param list List of parameters
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t DrvCommBase::configure(const InOutStreamConfigure_t *list, uint8_t list_size)
{
  (void) list;
  (void) list_size;
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Lock a peripheral to one id
 * @return Status_t
 */
Status_t DrvCommBase::lock(uint8_t key)
{
  if(m_mutex.key == key)
  {
    return STATUS_DRV_SUCCESS;
  }else if(!m_mutex.lock.test_and_set(std::memory_order_acquire))
  {
    m_mutex.key = key;
    return STATUS_DRV_SUCCESS;
  }
  return STATUS_DRV_UNKNOWN_ERROR;
}

/**
 * @brief Unlock a peripheral
 * @return Status_t
 */
Status_t DrvCommBase::unlock(uint8_t key)
{
  if(m_mutex.key == key)
  {
    m_mutex.key = INOUTSTREAM_NO_KEY;
    m_mutex.lock.clear(std::memory_order_release);
    return STATUS_DRV_SUCCESS;
  }
  return STATUS_DRV_UNKNOWN_ERROR;
}

/**
 * @brief Template for a method to read data synchronously
 * @param buffer Buffer to store the data
 * @param size Number of bytes to read
 * @param key A parameter with different uses depending on the implementation
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvCommBase::read(uint8_t *buffer, uint32_t size, uint8_t key, uint32_t timeout)
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
 * @param size Number of bytes to write
 * @param key A parameter with different uses depending on the implementation
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvCommBase::write(uint8_t *buffer, uint32_t size, uint8_t key, uint32_t timeout)
{
  (void) buffer;
  (void) size;
  (void) key;
  (void) timeout;
  return STATUS_DRV_NOT_IMPLEMENTED;
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
Status_t DrvCommBase::readAsync(uint8_t *buffer, uint32_t size, uint8_t key, InOutStreamCallback_t func, void *arg)
{
  (void) buffer;
  (void) size;
  (void) key;
  (void) func;
  (void) arg;
  return STATUS_DRV_NOT_IMPLEMENTED;
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
  return 0;
}

/**
 * @brief Template method for asynchronous reception callback
 * @param buffer Buffer where data is stored
 * @param size Number of bytes read
 * @param arg User-defined parameter
 * @return Status_t
 */
Status_t DrvCommBase::readAsyncDoneCallback(Status_t error, uint8_t *buffer, uint32_t size, void *arg)
{
  (void) error;
  (void) buffer;
  (void) size;
  (void) arg;
  m_is_read_done = true;
  m_is_operation_done = true;
  return STATUS_DRV_NOT_IMPLEMENTED;
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
Status_t DrvCommBase::writeAsync(uint8_t *buffer, uint32_t size, uint8_t key, InOutStreamCallback_t func, void *arg)
{
  (void) buffer;
  (void) size;
  (void) key;
  (void) func;
  (void) arg;
  return STATUS_DRV_NOT_IMPLEMENTED;
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
 * @brief Template method for asynchronous transmission callback
 * @param buffer Buffer where data is stored
 * @param size Number of bytes to write
 * @param arg User-defined parameter
 * @return Status_t
 */
Status_t DrvCommBase::writeAsyncDoneCallback(Status_t error, uint8_t *buffer, uint32_t size, void *arg)
{
  (void) error;
  (void) buffer;
  (void) size;
  (void) arg;
  m_is_write_done = true;
  m_is_operation_done = true;
  return STATUS_DRV_NOT_IMPLEMENTED;
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
