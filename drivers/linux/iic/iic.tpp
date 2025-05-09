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
#include <string.h>

#include "linux/utils/linux_io.hpp"
#include "iic_types.hpp"


template<IicPeripheral_t PERIPHERAL>
IicHandle_t IIC<PERIPHERAL>::m_handle = nullptr;

template<IicPeripheral_t PERIPHERAL>
Task<DataBundle_t, IIC_QUEUE_SIZE, Status_t, 0> IIC<PERIPHERAL>::m_thread_handle;

template<IicPeripheral_t PERIPHERAL>
Mutex IIC<PERIPHERAL>::m_mutex;

template<IicPeripheral_t PERIPHERAL>
int IIC<PERIPHERAL>::m_linux_handle = -1;

template<IicPeripheral_t PERIPHERAL>
std::atomic_uint8_t IIC<PERIPHERAL>::m_instances_counter = ATOMIC_VAR_INIT(0);

template<IicPeripheral_t PERIPHERAL>
bool IIC<PERIPHERAL>::m_is_async_mode_rx = false;

template<IicPeripheral_t PERIPHERAL>
bool IIC<PERIPHERAL>::m_is_async_mode_tx = false;

template<IicPeripheral_t PERIPHERAL>
bool IIC<PERIPHERAL>::m_is_initialised = false;


/**
 * @brief Constructor
 *
 * @param port_handle A string containing the path to the peripheral
 * @param address 7 or 10 bits address
 */
template<IicPeripheral_t PERIPHERAL>
IIC<PERIPHERAL>::IIC(const IicHandle_t port_handle, uint16_t address)
{
  if(port_handle == nullptr)
  {
    return;
  }
  if(m_handle == nullptr)
  {
    m_handle = port_handle;
  }
  m_instances_counter++;
  m_address = address;
}

/**
 * @brief Destructor
 */
template<IicPeripheral_t PERIPHERAL>
IIC<PERIPHERAL>::~IIC()
{
  m_instances_counter--;
  if(m_instances_counter == 0)
  {
    if(m_linux_handle >= 0)
    {
      (void) close(m_linux_handle);
      m_linux_handle = -1;
      m_is_initialised = false;
    }
  }
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
template<IicPeripheral_t PERIPHERAL>
Status_t IIC<PERIPHERAL>::configure(const SettingsList_t *list, uint8_t list_size)
{
  Status_t status = STATUS_DRV_SUCCESS;
  char file_name[20];
  bool result;

  if(m_handle == nullptr) { return STATUS_DRV_NULL_POINTER;}

  m_mutex.lock();
  if(m_is_initialised)
  {
    m_mutex.unlock();
    m_read_status = STATUS_DRV_IDLE;
    m_write_status = STATUS_DRV_IDLE;
    return STATUS_DRV_CONFIGURED;
  }
  m_is_initialised = true;
  m_mutex.unlock();

  if(list != nullptr && list_size != 0)
  {
    for(uint8_t i = 0; i < list_size; i++)
    {
      switch(list[i].parameter)
      {
      case COMM_WORK_ASYNC_RX:
        m_is_async_mode_rx = (bool)list[i].value;
        break;
      case COMM_WORK_ASYNC_TX:
        m_is_async_mode_tx = (bool)list[i].value;
        break;
      default:
        break;
      }
    }
  }

  (void) m_thread_handle.terminate();
  if(m_is_async_mode_rx || m_is_async_mode_tx)
  {
    result = m_thread_handle.create(IIC::transferDataAsync, nullptr, 0);
    if(!result)
    {
      m_is_initialised = false;
      m_read_status = STATUS_DRV_NOT_CONFIGURED;
      m_write_status = STATUS_DRV_NOT_CONFIGURED;
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to launch the IIC task.\r\n");
      return status;
    }
  }

  (void) snprintf(file_name, sizeof(file_name)-1, "/dev/i2c-%u", PERIPHERAL);
  if ((m_linux_handle = open(file_name, O_RDWR)) < 0)
  {
    m_is_initialised = false;
    m_read_status = STATUS_DRV_NOT_CONFIGURED;
    m_write_status = STATUS_DRV_NOT_CONFIGURED;
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to open the file.");
    return status;
  }

  m_read_status = STATUS_DRV_IDLE;
  m_write_status = STATUS_DRV_IDLE;
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Set a new address of the device
 * @param address 7 or 10 bits new address
 */
template<IicPeripheral_t PERIPHERAL>
void IIC<PERIPHERAL>::setAddress(uint16_t address)
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
template<IicPeripheral_t PERIPHERAL>
Status_t IIC<PERIPHERAL>::read(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  DataBundle_t data_bundle;
  (void) timeout;

  status = checkInputs(data, byte_count, timeout);
  if(!status.success) { return status;}

  if(m_is_async_mode_rx)
  {
    m_read_status = STATUS_DRV_RUNNING;
    m_bytes_read = 0;
    data_bundle.buffer = data;
    data_bundle.size = byte_count;
    data_bundle.timeout = timeout;
    data_bundle.obj_ptr = this;
    if(m_thread_handle.setInputData(data_bundle, timeout))
    {
      status = STATUS_DRV_SUCCESS;
    }else
    {
      m_read_status = STATUS_DRV_IDLE;
      status = STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    m_mutex.lock();
    m_read_status = STATUS_DRV_RUNNING;
    m_bytes_read = 0;
    status = iicRead(data, byte_count, m_address);
    m_read_status = status;
    m_mutex.unlock();
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
template<IicPeripheral_t PERIPHERAL>
Status_t IIC<PERIPHERAL>::write(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  DataBundle_t data_bundle;
  (void) timeout;

  status = checkInputs(data, byte_count, timeout);
  if(!status.success) { return status;}


  if(m_is_async_mode_tx)
  {
    m_write_status = STATUS_DRV_RUNNING;
    m_bytes_written = 0;
    data_bundle.buffer = data;
    data_bundle.size = byte_count;
    data_bundle.timeout = timeout;
    data_bundle.obj_ptr = this;
    if(m_thread_handle.setInputData(data_bundle, timeout))
    {
      status = STATUS_DRV_SUCCESS;
    }else
    {
      m_write_status = STATUS_DRV_IDLE;
      status = STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    m_mutex.lock();
    m_write_status = STATUS_DRV_RUNNING;
    m_bytes_written = 0;
    status = iicWrite(data, byte_count, m_address);
    m_write_status = status;
    m_mutex.unlock();
  }

  return status;
}

/**
 * @brief Install a callback function
 * @param event An event to trigger the call
 * @param function A function to call back
 * @param user_arg A argument used as a parameter to the function
 * @return Status_t
 */
template<IicPeripheral_t PERIPHERAL>
Status_t IIC<PERIPHERAL>::setCallback(EventsList_t event, DriverCallback_t function, void *user_arg)
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
 * @param address 7 or 10 bits address of the device
 * @return Status_t
 */
template<IicPeripheral_t PERIPHERAL>
Status_t IIC<PERIPHERAL>::iicRead(uint8_t *buffer, uint32_t size, uint16_t address)
{
  Status_t status = STATUS_DRV_SUCCESS;
  int byte_count;

  if (ioctl(m_linux_handle, I2C_PERIPHERAL_7BITS_ADDRESS, address) >= 0)
  {
    byte_count = readSyscall(m_linux_handle, buffer, size);
    m_bytes_read = byte_count > 0 ? byte_count : 0;
    if(byte_count < 0)
    {
      status = convertErrnoCode(errno);
    } else if (byte_count != size)
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
 * @param address 7 or 10 bits address of the device
 * @return Status_t
 */
template<IicPeripheral_t PERIPHERAL>
Status_t IIC<PERIPHERAL>::iicWrite(const uint8_t *buffer, uint32_t size, uint16_t address)
{
  Status_t status = STATUS_DRV_SUCCESS;
  int byte_count;

  if (ioctl(m_linux_handle, I2C_PERIPHERAL_7BITS_ADDRESS, address) >= 0)
  {
    byte_count = writeSyscall(m_linux_handle, buffer, size);
    if(byte_count < 0)
    {
      status = convertErrnoCode(errno);
    } else if (byte_count != size)
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
template<IicPeripheral_t PERIPHERAL>
Status_t IIC<PERIPHERAL>::transferDataAsync(DataBundle_t data_bundle, void *user_arg)
{
  Status_t status;
  IIC *obj = static_cast<IIC *>(data_bundle.obj_ptr);
  (void) user_arg;

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
template<IicPeripheral_t PERIPHERAL>
Status_t IIC<PERIPHERAL>::checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout)
{
  if(!m_is_initialised) {return STATUS_DRV_ERR_PARAM;}
  if(buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(m_handle == nullptr || m_linux_handle < 0) { return STATUS_DRV_BAD_HANDLE;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  return STATUS_DRV_SUCCESS;
}