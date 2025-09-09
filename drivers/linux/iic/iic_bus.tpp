/**
 * @file iic_bus.tpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-07-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "linux/iic/iic_bus.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "linux/utils/linux_io.hpp"

/**
 * @brief Configure a list of parameters
 *
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
template<IicHandle_t PORT_NUMBER>
Status_t IicBus<PORT_NUMBER>::configure(const SettingsList_t *list, uint8_t list_size)
{
  static Mutex mutex;
  Status_t status = STATUS_DRV_SUCCESS;
  bool result;
  char port_name[100];
  int n_bytes;

  mutex.lock();
  if(m_is_configured)
  {
    mutex.unlock();
    return STATUS_DRV_SUCCESS;
  }


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

  if(m_is_async_mode_rx || m_is_async_mode_tx)
  {
    result = m_thread_handle.create(IicBus::asyncTransferThread, this, 0);
    if(!result)
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to launch the IIC task.\r\n");
      mutex.unlock();
      return status;
    }
  }else
  {
    (void) m_thread_handle.terminate();
  }

  n_bytes = snprintf(port_name, sizeof(port_name) - 1, "/dev/i2c-%u", PORT_NUMBER);
  if(n_bytes < 0)
  {
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to find the file name for uart driver.\r\n");
    mutex.unlock();
    return status;
  }
  m_fd = open(port_name, O_RDWR);
  if (m_fd < 0)
  {
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to open the file.");
    mutex.unlock();
    return status;
  }

  m_is_configured = true;
  mutex.unlock();
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Read data synchronously
 *
 * @param data Buffer to store the data
 * @param timeout Time to wait in milliseconds before returning an error
 * @param cb_function
 * @param cb_arg
 * @return Status_t
 */
template<IicHandle_t PORT_NUMBER>
Status_t IicBus<PORT_NUMBER>::read(uint16_t address, Buffer_t data, uint32_t timeout, iCallback &event_handler)
{
  Status_t status;
  IicDataBundle_t data_bundle;
  (void) timeout;

  status = checkInputs(data, timeout);
  if(!status.success) { return status;}


  if(m_is_async_mode_rx)
  {
    data_bundle.rx.run = true;
    data_bundle.rx.data = data;
    data_bundle.rx.address = address;
    data_bundle.rx.timeout = timeout;
    data_bundle.rx.event_handle = &event_handler;
    data_bundle.tx.run = false;
    data_bundle.tx.event_handle = nullptr;
    if(m_thread_handle.setInputData(data_bundle, timeout))
    {
      status = STATUS_DRV_SUCCESS;
    }else
    {
      status = STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    if(m_mutex.lock(timeout))
    {
      status = blockingRead(data, address);
      m_mutex.unlock();
    }else
    {
      status = STATUS_DRV_ERR_BUSY;
    }
  }

  return status;
}

/**
 * @brief Write data synchronously
 *
 * @param data Buffer where data is stored
 * @param timeout Time to wait in milliseconds before returning an error
 * @param cb_function
 * @param cb_arg
 * @return Status_t
 */
template<IicHandle_t PORT_NUMBER>
Status_t IicBus<PORT_NUMBER>::write(uint16_t address, Buffer_t data, uint32_t timeout, iCallback &event_handler)
{
  Status_t status;
  IicDataBundle_t data_bundle;
  (void) timeout;

  status = checkInputs(data, timeout);
  if(!status.success) { return status;}


  if(m_is_async_mode_tx)
  {
    data_bundle.tx.run = true;
    data_bundle.tx.data = data;
    data_bundle.tx.address = address;
    data_bundle.tx.timeout = timeout;
    data_bundle.tx.event_handle = &event_handler;
    data_bundle.rx.run = false;
    data_bundle.rx.event_handle = nullptr;
    if(m_thread_handle.setInputData(data_bundle, timeout))
    {
      status = STATUS_DRV_SUCCESS;
    }else
    {
      status = STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    if(m_mutex.lock(timeout))
    {
      status = blockingWrite(data, address);
      m_mutex.unlock();
    }else
    {
      status = STATUS_DRV_ERR_BUSY;
    }
  }

  return status;
}

/**
 * @brief Constructor
 *
 * @param port_handle A string containing the path to the peripheral
 * @param address 7 or 10 bits address
 */
template<IicHandle_t PORT_NUMBER>
IicBus<PORT_NUMBER>::IicBus()
{
  m_fd = -1;
  m_is_async_mode_rx = false;
  m_is_async_mode_tx = false;
  m_is_configured = false;
}

/**
 * @brief Destructor
 */
template<IicHandle_t PORT_NUMBER>
IicBus<PORT_NUMBER>::~IicBus()
{
  if(m_fd >= 0)
  {
    (void) close(m_fd);
  }
}

/**
 * @brief Read data synchronously
 *
 * @param buffer Buffer to store the data
 * @param address 7 or 10 bits address of the device
 * @return Status_t
 */
template<IicHandle_t PORT_NUMBER>
Status_t IicBus<PORT_NUMBER>::blockingRead(DrvBuffer_t data, uint16_t address)
{
  Status_t status = STATUS_DRV_SUCCESS;
  int byte_count;

  if (ioctl(m_fd, I2C_PERIPHERAL_7BITS_ADDRESS, address) >= 0)
  {
    byte_count = readSyscall(m_fd, data.data(), data.size_bytes());
    if (byte_count != data.size_bytes())
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
 *
 * @param buffer Buffer where data is stored
 * @param address 7 or 10 bits address of the device
 * @return Status_t
 */
template<IicHandle_t PORT_NUMBER>
Status_t IicBus<PORT_NUMBER>::blockingWrite(const DrvBuffer_t data, uint16_t address)
{
  Status_t status = STATUS_DRV_SUCCESS;
  int byte_count;

  if (ioctl(m_fd, I2C_PERIPHERAL_7BITS_ADDRESS, address) >= 0)
  {
    byte_count = writeSyscall(m_fd, data.data(), data.size_bytes());
    if (byte_count != data.size_bytes())
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
 *
 * @param data_bundle
 * @param user_arg
 * @return Status_t
 */
template<IicHandle_t PORT_NUMBER>
Status_t IicBus<PORT_NUMBER>::asyncTransferThread(IicDataBundle_t data_bundle, void *user_arg)
{
  Status_t status = STATUS_DRV_NULL_POINTER;
  IicBus *obj = static_cast<IicBus *>(user_arg);

  if(obj != nullptr)
  {

    if(data_bundle.tx.run)
    {
      status = obj->blockingWrite(data_bundle.tx.data, data_bundle.tx.address);
      if (data_bundle.tx.event_handle != nullptr)
      {
        data_bundle.tx.event_handle->onEvent(status, EVENT_WRITE, data_bundle.tx.data);
      }
    }

    if(data_bundle.rx.run)
    {
      status = obj->blockingRead(data_bundle.rx.data, data_bundle.rx.address);
      if (data_bundle.rx.event_handle != nullptr)
      {
        data_bundle.rx.event_handle->onEvent(status, EVENT_READ, data_bundle.rx.data);
      }
    }

  }
  return status;
}

/**
 * @brief Verify if the inputs are in ther expected range
 *
 * @param buffer Data buffer
 * @param size Number of bytes in the data buffer
 * @param timeout Operation timeout value
 * @param key Parameter
 * @return Status_t
 */
template<IicHandle_t PORT_NUMBER>
Status_t IicBus<PORT_NUMBER>::checkInputs(const DrvBuffer_t data, uint32_t timeout)
{
  if(!m_is_configured) { return STATUS_DRV_NOT_CONFIGURED;}
  if(data.data() == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(m_fd < 0) { return STATUS_DRV_BAD_HANDLE;}
  if(data.size_bytes() == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  return STATUS_DRV_SUCCESS;
}