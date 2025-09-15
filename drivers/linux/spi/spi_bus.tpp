/**
 * @file spi_bus.tpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "linux/spi/spi_bus.hpp"

#include <cstring>

#include <unistd.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "linux/utils/linux_io.hpp"

/**
 * @brief Constructor
 */
template<SpiHandle_t PORT_NUMBER>
SpiBus<PORT_NUMBER>::SpiBus()
{
  m_fd = -1;
}

/**
 * @brief Destructor
 */
template<SpiHandle_t PORT_NUMBER>
SpiBus<PORT_NUMBER>::~SpiBus()
{
  if(m_fd >= 0)
  {
    (void) close(m_fd);
  }
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return ErrorCode
 */
template<SpiHandle_t PORT_NUMBER>
ErrorCode SpiBus<PORT_NUMBER>::configure(const SettingsList_t *list, uint8_t list_size)
{
  ErrorCode status = SpiErrorCode::kSuccess;
  char mode = 0;
  char n_bits = 8;
  int max_baud = 1000000;
  bool result;
  char port_name[100];
  int n_bytes;

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
    result = m_thread_handle.create(SpiBus::asyncTransferThread, this, 0);
    if(!result)
    {
      status = SpiErrorCode::kFailed;
      status.setMessage("Failed to launch the spi task");
      return status;
    }
  }else
  {
    (void) m_thread_handle.terminate();
  }

  n_bytes = std::snprintf(port_name, sizeof(port_name) - 1, "/dev/spi-%u", PORT_NUMBER);
  if(n_bytes < 0)
  {
    status = SpiErrorCode::kFailed;
    status.setMessage("Failed to find the file name for uart driver");
    return status;
  }
  m_fd = open(port_name, O_RDWR);
  if (m_fd < 0)
  {
    status = SpiErrorCode::kFailed;
    status.setMessage("Failed to open the file");
    return status;
  }

  if (ioctl(m_fd, SPI_IOC_WR_MODE, &mode) < 0)
  {
    close(m_fd);
    status = SpiErrorCode::kFailed;
    status.setMessage("Failed to configure spi mode");
    return status;
  }

  if (ioctl(m_fd, SPI_IOC_WR_BITS_PER_WORD, &n_bits) < 0)
  {
    close(m_fd);
    status = SpiErrorCode::kFailed;
    status.setMessage("Failed to configure spi bits per word");
    return status;
  }

  if (ioctl(m_fd, SPI_IOC_WR_MAX_SPEED_HZ, &max_baud) < 0)
  {
    close(m_fd);
    status = SpiErrorCode::kFailed;
    status.setMessage("Failed to configure spi clock frequency (Hz)");
    return status;
  }

  return status;
}

/**
 * @brief Read data from the bus
 * @param data Buffer to store the data
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return ErrorCode
 */
template<SpiHandle_t PORT_NUMBER>
ErrorCode SpiBus<PORT_NUMBER>::read(iDIO &cs_pin, bool cs_active_state, Buffer_t data, uint32_t timeout, iCallback &event_handler)
{
  ErrorCode status;
  SpiDataBundle_t data_bundle;

  status = checkInputs(data, timeout);
  if(!status) { return status;}


  if(m_is_async_mode_rx)
  {
    data_bundle.rx.run = true;
    data_bundle.rx.data = data;
    data_bundle.rx.cs_pin = &cs_pin;
    data_bundle.rx.event_handler = &event_handler;
    data_bundle.rx.timeout = timeout;
    data_bundle.tx.run = false;
    if(m_thread_handle.setInputData(data_bundle, 0))
    {
      status = SpiErrorCode::kSuccess;
    }else
    {
      status = SpiErrorCode::kBusy;
    }
  }else
  {
    status = blockingTransfer(nullptr, data.data(), data.size_bytes());
  }

  return status;
}

/**
 * @brief Write data to the bus
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @return ErrorCode
 */
template<SpiHandle_t PORT_NUMBER>
ErrorCode SpiBus<PORT_NUMBER>::write(iDIO &cs_pin, bool cs_active_state, Buffer_t data, uint32_t timeout, iCallback &event_handler)
{
  ErrorCode status;
  SpiDataBundle_t data_bundle;

  status = checkInputs(data, timeout);
  if(!status) { return status;}


  if(m_is_async_mode_tx)
  {
    data_bundle.tx.run = true;
    data_bundle.tx.data = data;
    data_bundle.tx.cs_pin = &cs_pin;
    data_bundle.tx.event_handler = &event_handler;
    data_bundle.tx.timeout = timeout;
    data_bundle.rx.run = false;
    if(m_thread_handle.setInputData(data_bundle, 0))
    {
      status = SpiErrorCode::kSuccess;
    }else
    {
      status = SpiErrorCode::kBusy;
    }
  }else
  {
    status = blockingTransfer(data.data(), nullptr, data.size_bytes());
  }

  return status;
}

// /**
//  * @brief Read and write data simultaneously through the bus
//  * @param rx_data Buffer to store the data read
//  * @param tx_data Buffer where data to write is stored
//  * @param byte_count Number of bytes to write and read
//  * @param timeout Time to wait in milliseconds before returning an error
//  * @return ErrorCode
//  */
// template<SpiHandle_t PORT_NUMBER>
// ErrorCode SpiBus<PORT_NUMBER>::transfer(uint8_t *rx_data, uint8_t *tx_data, Size_t byte_count, uint32_t timeout)
// {
//   ErrorCode status;
//   SpiDataBundle_t data_bundle;

//   status = checkInputs(rx_data, byte_count, timeout);
//   if(!status) { return status;}
//   if(m_read_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}
//   if(m_write_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}

//   m_read_status = STATUS_DRV_RUNNING;
//   m_write_status = STATUS_DRV_RUNNING;
//   m_bytes_read = 0;
//   m_bytes_written = 0;

//   if(m_is_async_mode_rx && m_is_async_mode_tx)
//   {
//     data_bundle.rx_buffer = rx_data;
//     data_bundle.rx_size = byte_count;
//     data_bundle.tx_buffer = tx_data;
//     data_bundle.tx_size = byte_count;
//     data_bundle.timeout = timeout;
//     if(m_thread_handle.setInputData(data_bundle, 0))
//     {
//       status = STATUS_DRV_SUCCESS;
//     }else
//     {
//       m_read_status = STATUS_DRV_IDLE;
//       m_write_status = STATUS_DRV_IDLE;
//       status = STATUS_DRV_ERR_BUSY;
//     }
//   }else
//   {
//     status = blockingTransfer(tx_data, rx_data, byte_count);
//     m_read_status = status;
//     m_write_status = status;
//   }

//   return status;
// }

// /**
//  * @brief Read and write data simultaneously through the bus
//  * @param rx_data Buffer to store the data read
//  * @param tx_data Buffer where data to write is stored
//  * @param timeout Time to wait in milliseconds before returning an error
//  * @return ErrorCode
//  */
// template<SpiHandle_t PORT_NUMBER>
// ErrorCode SpiBus<PORT_NUMBER>::transfer(Buffer_t rx_data, Buffer_t tx_data, uint32_t timeout)
// {
//   return transfer(rx_data.data(), tx_data.data(), rx_data.size(), timeout);
// }

/**
 * @brief Perform a data transaction on the bus
 * @param txBuf Buffer where data to write is stored
 * @param rxBuf Buffer to store the data read
 * @param byte_count Number of bytes to write and read
 * @return ErrorCode
 */
template<SpiHandle_t PORT_NUMBER>
ErrorCode SpiBus<PORT_NUMBER>::blockingTransfer(uint8_t *txBuf, uint8_t *rxBuf, uint32_t byte_count)
{
  ErrorCode status;
  struct spi_ioc_transfer spi;

  std::memset(&spi, 0, sizeof(spi));

  spi.tx_buf        = (uintptr_t)txBuf;
  spi.rx_buf        = (uintptr_t)rxBuf;
  spi.len           = byte_count;
  spi.speed_hz      = m_speed;
  spi.delay_usecs   = 0;
  spi.bits_per_word = 8;
  spi.cs_change     = 0;

  if (ioctl(m_fd, SPI_IOC_MESSAGE(1), &spi) >= 0)
  {
    status = SpiErrorCode::kSuccess;
  }else
  {
    status = SpiErrorCode::kFailed;
    status.setMessage("Failed to transfer data over spi");
  }
  return status;
}

/**
 * @brief Working thread that perform a data transaction on the bus
 * @param data_bundle Data needed to perform the operation
 * @param self_ptr A pointer to a SpiBus object
 * @return ErrorCode
 */
template<SpiHandle_t PORT_NUMBER>
ErrorCode SpiBus<PORT_NUMBER>::asyncTransferThread(SpiDataBundle_t data_bundle, void *user_arg)
{
  ErrorCode status;
  SpiBus *obj = static_cast<SpiBus *>(user_arg);
  if(obj == nullptr)
  {
    return SpiErrorCode::kNullPointer;
  }

  if(data_bundle.rx.run && data_bundle.tx.run)
  {

  }else
  {
    if (data_bundle.rx.run)
    {
      status = obj->blockingTransfer(nullptr, data_bundle.rx.data.data(), data_bundle.rx.data.size_bytes());
      if (data_bundle.rx.event_handler != nullptr)
      {
        data_bundle.rx.event_handler->onEvent(status, EVENT_READ, data_bundle.rx.data);
      }
    }
    if (data_bundle.tx.run)
    {
      status = obj->blockingTransfer(data_bundle.tx.data.data(), nullptr, data_bundle.tx.data.size_bytes());
      if (data_bundle.tx.event_handler != nullptr)
      {
        data_bundle.tx.event_handler->onEvent(status, EVENT_READ, data_bundle.tx.data);
      }
    }
  }

  return status;
}

/**
 * @brief Verify if the inputs are in the expected range
 *
 * @param buffer Data buffer
 * @param size Number of bytes in the data buffer
 * @param timeout Operation timeout value
 * @param key Parameter
 * @return ErrorCode
 */
template<SpiHandle_t PORT_NUMBER>
ErrorCode SpiBus<PORT_NUMBER>::checkInputs(const Buffer_t data, uint32_t timeout)
{
  if(!m_is_configured) { return SpiErrorCode::kNotConfigured;}
  if(data.data() == nullptr) { return SpiErrorCode::kNullPointer;}
  if(m_fd < 0) { return SpiErrorCode::kBadHandle;}
  if(data.size_bytes() == 0) { return SpiErrorCode::kInvalidParameter;}
  return SpiErrorCode::kSuccess;
}