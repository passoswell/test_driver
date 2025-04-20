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

#include "linux/iic/iic.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "linux/utils/linux_io.hpp"
#include "iic_types.hpp"


/**
 * @brief Constructor
 *
 * @param port_handle A string containing the path to the peripheral
 * @param address 8 or 10 bits address
 */
IIC::IIC(const void *port_handle, uint16_t address)
{
  m_address = address;
  m_handle = port_handle;
  m_linux_handle = -1;
}

/**
 * @brief Destructor
 */
IIC::~IIC()
{
  if(m_linux_handle >= 0)
  {
    (void) close(m_linux_handle);
  }
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t IIC::configure(const DriverSettings_t *list, uint8_t list_size)
{
  Status_t status = STATUS_DRV_SUCCESS;
  bool result;

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

  if ((m_linux_handle = open((char *)m_handle, O_RDWR)) < 0)
  {
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to open the file.");
    return status;
  }

  if(m_is_async_mode)
  {
    result = m_thread_handle.create(IIC::transferDataAsync, this, 0);
    if(!result)
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to launch the IIC task.\r\n");
      return status;
    }
  }else
  {
    (void) m_thread_handle.terminate();
  }

  m_read_status = STATUS_DRV_IDLE;
  m_write_status = STATUS_DRV_IDLE;
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Set a new address of the device
 * @param address 8 or 10 bits new address
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
  Status_t status;
  DataBundle_t data_bundle;
  (void) timeout;

  status = checkInputs(data, byte_count, timeout);
  if(!status.success) { return status;}
  if(m_read_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}
  if(m_write_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}

  m_read_status = STATUS_DRV_RUNNING;
  m_bytes_read = 0;

  if(m_is_async_mode)
  {
    data_bundle.buffer = data;
    data_bundle.size = byte_count;
    data_bundle.timeout = timeout;
    if(m_thread_handle.setInputData(data_bundle, 0))
    {
      return STATUS_DRV_SUCCESS;
    }else
    {
      m_read_status = STATUS_DRV_IDLE;
      return STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    status = iicRead(data, byte_count, m_address);
    m_read_status = status;
    return status;
  }

  return STATUS_DRV_UNKNOWN_ERROR;
}

/**
 * @brief Write data synchronously
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t IIC::write(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  DataBundle_t data_bundle;
  (void) timeout;

  status = checkInputs(data, byte_count, timeout);
  if(!status.success) { return status;}
  if(m_read_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}
  if(m_write_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}

  m_write_status = STATUS_DRV_RUNNING;
  m_bytes_written = 0;

  if(m_is_async_mode)
  {
    data_bundle.buffer = data;
    data_bundle.size = byte_count;
    data_bundle.timeout = timeout;
    if(m_thread_handle.setInputData(data_bundle, 0))
    {
      return STATUS_DRV_SUCCESS;
    }else
    {
      m_write_status = STATUS_DRV_IDLE;
      return STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    status = iicWrite(data, byte_count, m_address);
    m_write_status = status;
    return status;
  }

  return STATUS_DRV_UNKNOWN_ERROR;
}

/**
 * @brief Install a callback function
 * @param event An event to trigger the call
 * @param function A function to call back
 * @param user_arg A argument used as a parameter to the function
 * @return Status_t
 */
Status_t IIC::setCallback(DriverEventsList_t event, DriverCallback_t function, void *user_arg)
{
  Status_t status = STATUS_DRV_SUCCESS;

  switch (event)
  {
  case EVENT_READ:
    if(m_read_status.code != OPERATION_RUNNING)
    {
      m_func_rx = function;
      m_arg_rx = user_arg;
    }else
    {
      status = STATUS_DRV_ERR_BUSY;
    }
    break;
  case EVENT_WRITE:
    if (m_write_status.code != OPERATION_RUNNING)
    {
      m_func_tx = function;
      m_arg_tx = user_arg;
    }else
    {
      status = STATUS_DRV_ERR_BUSY;
    }
    break;
  default:
    status = STATUS_DRV_ERR_PARAM;
    break;
  }

  return status;
}

/**
 * @brief Read data synchronously
 * @param buffer Buffer to store the data
 * @param size Number of bytes to read
 * @param address 8 or 10 bits address of the device
 * @return Status_t
 */
Status_t IIC::iicRead(uint8_t *buffer, uint32_t size, uint16_t address)
{
  Status_t status = STATUS_DRV_SUCCESS;
  int byte_count;

  if (ioctl(m_linux_handle, I2C_PERIPHERAL_7BITS_ADDRESS, address >> 1) >= 0)
  {
    byte_count = readSyscall(m_linux_handle, buffer, size);
    m_bytes_read = byte_count > 0 ? byte_count : 0;
    if (byte_count != size)
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"The number of bytes transmitted through iic is smaller than the requested.");
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
 * @param address 8 or 10 bits address of the device
 * @return Status_t
 */
Status_t IIC::iicWrite(const uint8_t *buffer, uint32_t size, uint16_t address)
{
  Status_t status = STATUS_DRV_SUCCESS;
  int byte_count;

  if (ioctl(m_linux_handle, I2C_PERIPHERAL_7BITS_ADDRESS, address >> 1) >= 0)
  {
    byte_count = writeSyscall(m_linux_handle, buffer, size);
    if (byte_count != size)
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"The number of bytes received through iic is smaller than the requested.");
    }
  }
  else
  {
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"It was not possible to set the desired peripheral address.");
  }

  return status;
}

/**
 * @brief Task method that process iic data transfer
 * @param data_bundle
 * @param user_arg
 * @return Status_t
 */
Status_t IIC::transferDataAsync(DataBundle_t data_bundle, void *user_arg)
{
  Status_t status;
  IIC *obj = static_cast<IIC *>(user_arg);
  if(obj != nullptr)
  {

    if (obj->m_read_status.code == OPERATION_RUNNING)
    {
      status = obj->iicRead(data_bundle.buffer, data_bundle.size, obj->m_address);
      obj->m_read_status = status;
      if (obj->m_func_rx != nullptr)
      {
        Buffer_t data(data_bundle.buffer, data_bundle.size);
        obj->m_func_rx(status, EVENT_READ, data, obj->m_arg_rx);
      }
    }

    if (obj->m_write_status.code == OPERATION_RUNNING)
    {
      status = obj->iicWrite(data_bundle.buffer, data_bundle.size, obj->m_address);
      obj->m_write_status = status;
      if (obj->m_func_tx != nullptr)
      {
        Buffer_t data(data_bundle.buffer, data_bundle.size);
        obj->m_func_tx(status, EVENT_WRITE, data, obj->m_arg_tx);
      }
    }

  }
  return status;
}

/**
 * @brief Verify if the inputs are in ther expected range
 * @param buffer Data buffer
 * @param size Number of bytes in the data buffer
 * @param timeout Operation timeout value
 * @param key Parameter
 * @return Status_t
 */
Status_t IIC::checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout)
{
  if(buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(m_handle == nullptr || m_linux_handle < 0) { return STATUS_DRV_BAD_HANDLE;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  return STATUS_DRV_SUCCESS;
}