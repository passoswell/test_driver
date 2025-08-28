/**
 * @file linux_serial_files.cpp
 * @author your name (you@domain.com)
 * @brief Gives access to a linux tty file as a serial port
 * @version 0.1
 * @date 2025-03-03
 *
 * @copyright Copyright (c) 2025
 *
 */

 #include "linux/utils/linux_serial_file.hpp"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "linux/utils/linux_io.hpp"

 /**
 * @brief Constructor
 * @param port_handle A string containing the path to the peripheral
 */
LinuxSerialFile::LinuxSerialFile(const char *port_handle)
{
  m_handle = port_handle;
  m_linux_handle = -1;
  m_is_async_mode_rx = false;
  m_is_async_mode_tx = false;
  m_bytes_read = 0;
  m_func_rx = nullptr;
  m_func_tx = nullptr;
  m_arg_rx = nullptr;
  m_arg_tx = nullptr;
}

/**
 * @brief Destructor
 */
LinuxSerialFile::~LinuxSerialFile()
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
Status_t LinuxSerialFile::configure(const SettingsList_t *list, uint8_t list_size)
{
  Status_t status;
  struct termios termios_structure;
  bool result;

  if(m_handle == nullptr) { return STATUS_DRV_NULL_POINTER;}

  if(list != nullptr && list_size != 0)
  {
    for(uint8_t i = 0; i < list_size; i++)
    {
      switch(list[i].parameter)
      {
      case COMM_WORK_ASYNC_RX:
        m_is_async_mode_rx = (bool) list[i].value;
        break;
      case COMM_WORK_ASYNC_TX:
        m_is_async_mode_tx = (bool) list[i].value;
        break;
      default:
        break;
      }
    }
  }

  if(m_is_async_mode_rx)
  {
    result = m_rx_thread_handle.create(LinuxSerialFile::readFromThreadBlocking, this, 0);
    if(!result)
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to launch LinuxSerialFile task for reading.\r\n");
      return status;
    }
  }else
  {
    (void) m_rx_thread_handle.terminate();
  }

  if(m_is_async_mode_tx)
  {
    result &= m_tx_thread_handle.create(LinuxSerialFile::writeFromThreadBlocking, this, 0);
    if(!result)
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to launch LinuxSerialFile task for writing.\r\n");
      return status;
    }
  }else
  {
    (void) m_tx_thread_handle.terminate();
  }

  m_linux_handle = open((char *)m_handle, O_RDWR | O_NOCTTY);
  if (m_linux_handle < 0)
  {
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to open file.\r\n");
    return status;
  }
  tcgetattr(m_linux_handle, &termios_structure);
  cfmakeraw(&termios_structure);

  // For more info on how to setup VMIN and VTIME,
  // please refer to http://www.unixwiz.net/techtips/termios-vmin-vtime.html
  termios_structure.c_cc[VMIN] = 0;
  termios_structure.c_cc[VTIME] = 0;
  tcflush(m_linux_handle, TCIFLUSH);
  tcflush(m_linux_handle, TCIFLUSH);
  tcsetattr(m_linux_handle, TCSANOW, &termios_structure);

  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Read data from a linux file
 * @param data Buffer to store the data on reception
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t LinuxSerialFile::read(DrvBuffer_t data, uint32_t timeout)
{
  Status_t status;
  int bytes_read = 0;
  DrvDataBundle_t data_bundle;

  status = checkInputs(data.data(), data.size_bytes(), timeout, m_handle, m_linux_handle);
  if(!status.success) { return status;}

  m_bytes_read = 0;

  if(m_is_async_mode_rx)
  {
    data_bundle.rx.data = data;
    data_bundle.rx.timeout = timeout;
    if(m_rx_thread_handle.setInputData(data_bundle, 0))
    {
      status = STATUS_DRV_SUCCESS;
    }else
    {
      status = STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    status = readBlocking(data.data(), data.size_bytes(), timeout, false);
  }

  return status;
}

/**
 * @brief Write data to a linux file
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t LinuxSerialFile::write(DrvBuffer_t data, uint32_t timeout)
{
  Status_t status;
  int bytes_written, drain_status;
  DrvDataBundle_t data_bundle;

  status = checkInputs(data.data(), data.size_bytes(), timeout, m_handle, m_linux_handle);
  if(!status.success) { return status;}

  if(m_is_async_mode_tx)
  {
    data_bundle.tx.data = data;
    data_bundle.tx.timeout = timeout;
    if(m_tx_thread_handle.setInputData(data_bundle, 0))
    {
      status = STATUS_DRV_SUCCESS;
    }else
    {
      status = STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    status = writeBlocking(data.data(), data.size_bytes(), timeout, false);
  }

  return status;
}

/**
 * @brief Install a callback function associated with an event
 * @param event An event to trigger the call
 * @param function A function to call back on the occurrence of the event
 * @param user_arg A argument used as a parameter to the callback function
 * @return Status_t
 */
Status_t LinuxSerialFile::setCallback(EventsList_t event, Callback_t function, void *user_arg)
{
  Status_t status = STATUS_DRV_SUCCESS;

  switch (event)
  {
  case EVENT_READ:
    m_func_rx = function;
    m_arg_rx = user_arg;
    break;
  case EVENT_WRITE:
    m_func_tx = function;
    m_arg_tx = user_arg;
    break;
  default:
    status = STATUS_DRV_ERR_PARAM;
    break;
  }

  return status;
}

/**
 * @brief Read data synchronously
 * @param data Buffer to store the data
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @param call_back True if should call the callback function
 * @return Status_t
 */
Status_t LinuxSerialFile::readBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back)
{
  Status_t status = STATUS_DRV_SUCCESS;
  int bytes_read = 0;
  if (timeout == 0)
  {
    bytes_read = readSyscall(m_linux_handle, data, byte_count);
  }
  else
  {
    bytes_read = readOnTimeoutSyscall(m_linux_handle, data, byte_count, timeout);
  }

  if(bytes_read < 0)
  {
    status = convertErrnoCode(errno);
  }else
  {
    m_bytes_read = bytes_read;
    if(bytes_read == 0)
    {
      status = STATUS_DRV_TIMED_OUT;
    }
  }

  if(call_back && m_func_rx != nullptr)
  {
    Buffer_t data_container(data, m_bytes_read);
    m_func_rx(status, EVENT_READ, data_container, m_arg_rx);
  }

  return status;
}

/**
 * @brief Thread to perform file read operation in parallel
 * @param data_bundle Data needed to perform the operation
 * @param self_ptr A pointer to a object of type LinuxSerialFile
 * @return Status_t
 */
Status_t LinuxSerialFile::readFromThreadBlocking(DrvDataBundle_t data_bundle, void *self_ptr)
{
  LinuxSerialFile *obj = static_cast<LinuxSerialFile *>(self_ptr);
  if(obj != nullptr)
  {
    return obj->readBlocking(data_bundle.rx.data.data(), data_bundle.rx.data.size(), data_bundle.rx.timeout, true);
  }
  return STATUS_DRV_NULL_POINTER;
}

/**
 * @brief Write data synchronously
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @param call_back True if should call the callback function
 * @return Status_t
 */
Status_t LinuxSerialFile::writeBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back)
{
  Status_t status = STATUS_DRV_SUCCESS;
  int bytes_written, drain_status;
  bytes_written = writeSyscall(m_linux_handle, data, byte_count);
  if (byte_count >= 0)
  {
    drain_status = tcdrain(m_linux_handle);
    if (drain_status < 0)
    {
      status = convertErrnoCode(errno);
    }
  }
  else
  {
    status = convertErrnoCode(errno);
  }

  if(call_back && m_func_tx != nullptr)
  {
    Buffer_t data_container(data, m_bytes_read);
    m_func_tx(status, EVENT_WRITE, data_container, m_arg_tx);
  }

  return status;
}

/**
 * @brief Thread to perform file write operation in parallel
 * @param data_bundle Data needed to perform the operation
 * @param self_ptr A pointer to a object of type LinuxSerialFile
 * @return Status_t
 */
Status_t LinuxSerialFile::writeFromThreadBlocking(DrvDataBundle_t data_bundle, void *self_ptr)
{
  LinuxSerialFile *obj = static_cast<LinuxSerialFile *>(self_ptr);
  if(obj != nullptr)
  {
    return obj->writeBlocking(data_bundle.rx.data.data(), data_bundle.rx.data.size(), data_bundle.rx.timeout, true);
  }
  return STATUS_DRV_NULL_POINTER;
}
