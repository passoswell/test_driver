/**
 * @file iic.cpp
 * @author your name (you@domain.com)
 * @brief Give access to IIC functionalities
 * @version 0.1
 * @date 2024-10-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "drivers/linux/iic/iic.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "drivers/linux/utils/linux_io.hpp"
#include "iic_types.hpp"


/**
 * @brief Constructor
 *
 * @param port_handle A string containing the path to the peripheral
 * @param address 8 or 10 bits address
 */
IIC::IIC(void *port_handle, uint16_t address)
{
  m_address = address;
  m_handle = port_handle;
  m_linux_handle = -1;
  m_terminate = false;
  m_is_configured = false;

  m_sync.run = false;
  m_sync.buffer = nullptr;
  m_sync.size = 0;
  m_sync.func = nullptr;
  m_sync.arg = nullptr;
  m_sync.thread = nullptr;
}

/**
 * @brief Destructor
 */
IIC::~IIC()
{
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);

  locker1.lock();
  m_terminate = true;
  m_sync.run = true;
  locker1.unlock();

  m_sync.condition.notify_one();

  m_sync.thread->join();

  if(m_linux_handle >= 0)
  {
    close(m_linux_handle);
  }
  delete m_sync.thread;
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t IIC::configure(const DriverSettings_t *list, uint8_t list_size)
{
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);
  Status_t status = STATUS_DRV_SUCCESS;

  if(m_handle == nullptr) { return STATUS_DRV_NULL_POINTER;}
  m_read_status = STATUS_DRV_NOT_CONFIGURED;
  m_write_status = STATUS_DRV_NOT_CONFIGURED;

  if(list != nullptr && list_size != 0)
  {
    for(uint8_t i = 0; i < list_size; i++)
    {
      switch(list[i].parameter)
      {
      case COMM_WORK_ASYNC:
        m_is_async_mode = (bool)list[i].value;
        break;
      default:
        break;
      }
    }
  }

  if(m_is_async_mode)
  {
    if(m_sync.thread == nullptr)
    {
      m_sync.run = false;
      m_sync.buffer = nullptr;
      m_sync.size = 0;
      m_sync.func = nullptr;
      m_sync.arg = nullptr;
      m_sync.thread = new std::thread(&IIC::asyncThread, this);
    }
  }else if(m_sync.thread != nullptr)
  {
    locker1.lock();
    m_terminate = true;
    m_sync.run = true;
    locker1.unlock();
    m_sync.condition.notify_one();
    m_sync.thread->join();
  }

  if(m_linux_handle >= 0)
  {
    if ((m_linux_handle = open((char *)m_handle, O_RDWR)) < 0)
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to open the file.");
      return status;
    }
  }

  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Set a new address of the device
 *
 * @param address The new address
 */
void IIC::setAddress(uint16_t address)
{
  m_address = address;
}

/**
 * @brief Read data synchronously
 * @param data Buffer to store the data
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t IIC::read(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);
  Status_t status = STATUS_DRV_SUCCESS;
  (void) timeout;

  if(m_handle == nullptr || data == nullptr){return STATUS_DRV_NULL_POINTER;}
  if(byte_count == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  if(m_sync.run) { return STATUS_DRV_ERR_BUSY;}
  if(!locker1.try_lock()) { return STATUS_DRV_ERR_BUSY;}

  m_bytes_read = 0;
  m_read_status = STATUS_DRV_RUNNING;
  m_sync.run = true;

  if(m_is_async_mode)
  {
    m_sync.buffer = data;
    m_sync.size = byte_count;
    m_sync.key = m_address;
    m_sync.timeout = timeout;
    locker1.unlock();
    m_sync.condition.notify_one();
    status = STATUS_DRV_SUCCESS;
  }else
  {
    status = i2cRead(data, byte_count, m_address);
    m_read_status = status;
    m_sync.run = false;
    locker1.unlock();
  }

  return status;
}

/**
 * @brief Write data synchronously
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t IIC::write(const uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);
  Status_t status = STATUS_DRV_SUCCESS;
  (void) timeout;

  if(m_handle == nullptr || data == nullptr){return STATUS_DRV_NULL_POINTER;}
  if(byte_count == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  if(m_sync.run) { return STATUS_DRV_ERR_BUSY;}
  if(!locker1.try_lock()) { return STATUS_DRV_ERR_BUSY;}

  m_bytes_written = 0;
  m_write_status = STATUS_DRV_RUNNING;
  m_sync.run = true;

  if(m_is_async_mode)
  {
    m_sync.run = true;
    m_sync.buffer_const = data;
    m_sync.size = byte_count;
    m_sync.key = m_address;
    locker1.unlock();
    m_sync.condition.notify_one();
    status = STATUS_DRV_SUCCESS;
  }else
  {
    status = i2cWrite(data, byte_count, m_address);
    m_write_status = status;
    m_sync.run = false;
    locker1.unlock();
  }

  return status;
}

/**
 * @brief Install a callback function
 *
 * @param event An event to trigger the call
 * @param function A function to call back
 * @param user_arg A argument used as a parameter to the function
 * @return Status_t
 */
Status_t IIC::setCallback(DriverEventsList_t event, DriverCallback_t function, void *user_arg)
{
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Read data synchronously
 *
 * @param buffer Buffer to store the data
 * @param size Number of bytes to read
 * @param address_8bits The 8 bits address of the device
 * @return Status_t
 */
Status_t IIC::i2cRead(uint8_t *buffer, uint32_t size, uint16_t address_8bits)
{
  Status_t status = STATUS_DRV_SUCCESS;
  int byte_count;

  if (ioctl(m_linux_handle, I2C_PERIPHERAL_7BITS_ADDRESS, address_8bits >> 1) >= 0)
  {
    byte_count = readSyscall(m_linux_handle, buffer, size);
    m_bytes_read = byte_count > 0 ? byte_count : 0;
    if (byte_count != size)
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"The number of bytes transmitted through i2c is smaller than the requested.");
    }
  }else
  {
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"It was not possible to set the desired peripheral address.");
  }

  return status;
}

/**
 * @brief Write data synchronously
 * @param buffer Buffer where data is stored
 * @param size Number of bytes to write
 * @param address_8bits The 8 bits address of the device
 * @return Status_t
 */
Status_t IIC::i2cWrite(const uint8_t *buffer, uint32_t size, uint16_t address_8bits)
{
  Status_t status = STATUS_DRV_SUCCESS;
  int byte_count;

  if (ioctl(m_linux_handle, I2C_PERIPHERAL_7BITS_ADDRESS, address_8bits >> 1) >= 0)
  {
    byte_count = writeSyscall(m_linux_handle, buffer, size);
    if (byte_count != size)
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"The number of bytes received through i2c is smaller than the requested.");
    }
  }
  else
  {
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"It was not possible to set the desired peripheral address.");
  }

  return status;
}

/**
 * @brief Thread to read or write data asynchronously
 */
void IIC::asyncThread(void)
{
  Status_t status;
  uint32_t byte_count;
  std::unique_lock<std::mutex> locker1(m_sync.mutex);

  while(true)
  {
    m_sync.condition.wait(locker1, [this]{ return this->m_sync.run; });
    if(m_terminate) { break;}

    if (m_read_status.code == OPERATION_RUNNING)
    {
      status = i2cRead(m_sync.buffer, m_sync.size, m_sync.key);
      if (m_sync.func != nullptr)
      {
        // m_sync.func(status, m_sync.buffer, m_bytes_read, m_sync.arg);
      }
      m_read_status = status;
      m_sync.run = false;
    }

    if(m_write_status.code == OPERATION_RUNNING)
    {
      status = i2cWrite(m_sync.buffer, m_sync.size, m_sync.key);
      if (m_sync.func != nullptr)
      {
        // m_sync.func(status, m_sync.buffer, m_sync.size, m_sync.arg);
      }
      m_write_status = status;
      m_sync.run = false;
    }
  }
}