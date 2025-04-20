/**
 * @file spi.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-11-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "linux/spi/spi.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>

#include "linux/utils/linux_io.hpp"

/**
 * @brief Constructor
 * @param port_handle A string containing the path to the peripheral
 */
SPI::SPI(const void *port_handle)
{
  m_handle = port_handle;
  m_linux_handle = -1;
}

/**
 * @brief Destructor
 */
SPI::~SPI()
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
Status_t SPI::configure(const DriverSettings_t *list, uint8_t list_size)
{
  Status_t status = STATUS_DRV_SUCCESS;
  char mode = 0;
  char n_bits = 8;
  int max_baud = 1000000;
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
      case COMM_PARAM_BAUD:
        m_speed = list[i].value;
        if(m_speed <= 0)
        {
          m_speed = max_baud;
        }
        break;
      case COMM_PARAM_CLOCK_SPEED:
        m_speed = list[i].value;
        if(m_speed <= 0)
        {
          m_speed = max_baud;
        }
        break;
      case COMM_PARAM_LINE_MODE:
        mode = list[i].value;
        if(mode > 3 || mode < 0)
        {
          mode = 0;
        }
        break;
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

  if (ioctl(m_linux_handle, SPI_IOC_WR_MODE, &mode) < 0)
  {
    close(m_linux_handle);
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to configure spi mode.");
    return status;
  }

  if (ioctl(m_linux_handle, SPI_IOC_WR_BITS_PER_WORD, &n_bits) < 0)
  {
    close(m_linux_handle);
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to configure spi bits per word.");
    return status;
  }

  if (ioctl(m_linux_handle, SPI_IOC_WR_MAX_SPEED_HZ, &max_baud) < 0)
  {
    close(m_linux_handle);
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to configure spi clock frequency (Hz).");
    return status;
  }

  if(m_is_async_mode)
  {
    result = m_thread_handle.create(SPI::transferDataAsync, this, 0);
    if(!result)
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to launch the spi task.\r\n");
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
 * @brief Read data from the bus
 * @param data Buffer to store the data
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t SPI::read(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  DataBundle_t data_bundle;

  status = checkInputs(data, byte_count, timeout);
  if(!status.success) { return status;}
  if(m_read_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}
  if(m_write_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}

  m_read_status = STATUS_DRV_RUNNING;
  m_bytes_read = 0;

  if(m_is_async_mode)
  {
    data_bundle.rx_buffer = data;
    data_bundle.rx_size = byte_count;
    data_bundle.tx_buffer = nullptr;
    data_bundle.tx_size = 0;
    data_bundle.timeout = timeout;
    if(m_thread_handle.setInputData(data_bundle, 0))
    {
      status = STATUS_DRV_SUCCESS;
    }else
    {
      m_read_status = STATUS_DRV_IDLE;
      status = STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    status = xSpiXfer(nullptr, data, byte_count);
    m_read_status = status;
  }

  return status;
}

/**
 * @brief Write data to the bus
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t SPI::write(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  DataBundle_t data_bundle;

  status = checkInputs(data, byte_count, timeout);
  if(!status.success) { return status;}
  if(m_read_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}
  if(m_write_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}

  m_write_status = STATUS_DRV_RUNNING;
  m_bytes_written = 0;

  if(m_is_async_mode)
  {
    data_bundle.rx_buffer = nullptr;
    data_bundle.rx_size = 0;
    data_bundle.tx_buffer = data;
    data_bundle.tx_size = byte_count;
    data_bundle.timeout = timeout;
    if(m_thread_handle.setInputData(data_bundle, 0))
    {
      status = STATUS_DRV_SUCCESS;
    }else
    {
      m_write_status = STATUS_DRV_IDLE;
      status = STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    status = xSpiXfer(data, nullptr, byte_count);
    m_write_status = status;
  }

  return status;
}

/**
 * @brief Read and write data simultaneously through the bus
 * @param rx_data Buffer to store the data read
 * @param tx_data Buffer where data to write is stored
 * @param byte_count Number of bytes to write and read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t SPI::transfer(uint8_t *rx_data, uint8_t *tx_data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  DataBundle_t data_bundle;

  status = checkInputs(rx_data, byte_count, timeout);
  if(!status.success) { return status;}
  if(m_read_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}
  if(m_write_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}

  m_read_status = STATUS_DRV_RUNNING;
  m_write_status = STATUS_DRV_RUNNING;
  m_bytes_read = 0;
  m_bytes_written = 0;

  if(m_is_async_mode)
  {
    data_bundle.rx_buffer = rx_data;
    data_bundle.rx_size = byte_count;
    data_bundle.tx_buffer = tx_data;
    data_bundle.tx_size = byte_count;
    data_bundle.timeout = timeout;
    if(m_thread_handle.setInputData(data_bundle, 0))
    {
      status = STATUS_DRV_SUCCESS;
    }else
    {
      m_read_status = STATUS_DRV_IDLE;
      m_write_status = STATUS_DRV_IDLE;
      status = STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    status = xSpiXfer(tx_data, rx_data, byte_count);
    m_read_status = status;
    m_write_status = status;
  }

  return status;
}

/**
 * @brief Read and write data simultaneously through the bus
 * @param rx_data Buffer to store the data read
 * @param tx_data Buffer where data to write is stored
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t SPI::transfer(Buffer_t rx_data, Buffer_t tx_data, uint32_t timeout)
{
  return transfer(rx_data.data(), tx_data.data(), rx_data.size(), timeout);
}

/**
 * @brief Install a callback function
 * @param event An event to trigger the call
 * @param function A function
 * @param user_arg A argument used as a parameter to the function
 * @return Status_t
 */
Status_t SPI::setCallback(DriverEventsList_t event, DriverCallback_t function, void *user_arg)
{
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Perform a data transaction on the bus
 * @param txBuf Buffer where data to write is stored
 * @param rxBuf Buffer to store the data read
 * @param byte_count Number of bytes to write and read
 * @return Status_t
 */
Status_t SPI::xSpiXfer(uint8_t *txBuf, uint8_t *rxBuf, uint32_t byte_count)
{
  Status_t status;
  struct spi_ioc_transfer spi;

   memset(&spi, 0, sizeof(spi));

  spi.tx_buf        = (uintptr_t)txBuf;
  spi.rx_buf        = (uintptr_t)rxBuf;
  spi.len           = byte_count;
  spi.speed_hz      = m_speed;
  spi.delay_usecs   = 0;
  spi.bits_per_word = 8;
  spi.cs_change     = 0;

  if (ioctl(m_linux_handle, SPI_IOC_MESSAGE(1), &spi) >= 0)
  {
    status = STATUS_DRV_SUCCESS;
  }else
  {
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to transfer data over spi.");
  }
  return status;
}

/**
 * @brief Verify if the inputs are in the expected range
 * @param buffer Data buffer
 * @param size Number of bytes in the data buffer
 * @param timeout Operation timeout value
 * @param key Parameter
 * @return Status_t
 */
Status_t SPI::checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout)
{
  if(buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(m_handle == nullptr || m_linux_handle < 0) { return STATUS_DRV_BAD_HANDLE;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Working thread that perform a data transaction on the bus
 * @param data_bundle Data needed to perform the operation
 * @param self_ptr A pointer to a SPI object
 * @return Status_t
 */
Status_t SPI::transferDataAsync(DataBundle_t data_bundle, void *self_ptr)
{
  SPI *obj = static_cast<SPI *>(self_ptr);
  if(obj == nullptr)
  {
    return STATUS_DRV_NULL_POINTER;
  }
  if(data_bundle.rx_size > data_bundle.tx_size)
  {
    obj->m_read_status = obj->xSpiXfer(data_bundle.tx_buffer, data_bundle.rx_buffer, data_bundle.rx_size);
    return obj->m_read_status;
  }else if(data_bundle.rx_size < data_bundle.tx_size)
  {
    obj->m_write_status = obj->xSpiXfer(data_bundle.tx_buffer, data_bundle.rx_buffer, data_bundle.tx_size);
    return obj->m_write_status;
  }else
  {
    obj->m_read_status = obj->xSpiXfer(data_bundle.tx_buffer, data_bundle.rx_buffer, data_bundle.rx_size);
    obj->m_write_status = obj->m_read_status;
    return obj->m_read_status;
  }
}