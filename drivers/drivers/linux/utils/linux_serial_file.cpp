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

 #include "linux_serial_file.hpp"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "drivers/linux/utils/linux_io.hpp"

 /**
 * @brief Constructor
 * @param port_handle A string containing the path to the peripheral
 */
LinuxSerialFile::LinuxSerialFile(const char *port_handle) :
m_rx_thread_handle(LinuxSerialFile::readFromThreadBlocking, this),
m_tx_thread_handle(LinuxSerialFile::writeFromThreadBlocking, this)
{
  m_handle = port_handle;
  m_linux_handle = -1;
  m_is_async_mode = false;
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
Status_t LinuxSerialFile::configure(const DriverSettings_t *list, uint8_t list_size)
{
  Status_t status;
  struct termios termios_structure;

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
          m_is_async_mode = (bool) list[i].value;
          break;
        default:
          break;
      }
    }
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

  if(m_is_async_mode)
  {
    if(!m_rx_thread_handle.create() || !m_tx_thread_handle.create())
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to launch one or more LinuxSerialFile tasks.\r\n");
      return status;
    }
  }else
  {
    (void) m_rx_thread_handle.terminate();
    (void) m_tx_thread_handle.terminate();
  }

  m_read_status = STATUS_DRV_IDLE;
  m_write_status = STATUS_DRV_IDLE;
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Read data from a linux file
 * @param data Buffer to store the data on reception
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t LinuxSerialFile::read(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  int bytes_read = 0;
  DataBundle_t data_bundle;

  status = checkInputs(data, byte_count, timeout, m_handle, m_linux_handle);
  if(!status.success) { return status;}
  if(m_read_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}

  m_read_status = STATUS_DRV_RUNNING;
  m_read_status.success = false;
  m_bytes_read = 0;

  if(m_is_async_mode)
  {
    data_bundle.buffer = data;
    data_bundle.size = byte_count;
    data_bundle.timeout = timeout;
    if(m_rx_thread_handle.setInputData(&data_bundle, 0))
    {
      status = STATUS_DRV_SUCCESS;
    }else
    {
      m_read_status = STATUS_DRV_IDLE;
      status = STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    status = readBlocking(data, byte_count, timeout, false);
    m_read_status = status;
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
Status_t LinuxSerialFile::write(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  int bytes_written, drain_status;
  DataBundle_t data_bundle;

  status = checkInputs(data, byte_count, timeout, m_handle, m_linux_handle);
  if(!status.success) { return status;}
  if(m_write_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}

  m_write_status = STATUS_DRV_RUNNING;
  m_write_status.success = false;
  m_bytes_written = 0;

  if(m_is_async_mode)
  {
    data_bundle.buffer = data;
    data_bundle.size = byte_count;
    data_bundle.timeout = timeout;
    if(m_tx_thread_handle.setInputData(&data_bundle, 0))
    {
      status = STATUS_DRV_SUCCESS;
    }else
    {
      m_write_status = STATUS_DRV_IDLE;
      status = STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    status = writeBlocking(data, byte_count, timeout, false);
    m_write_status = status;
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
Status_t LinuxSerialFile::setCallback(DriverEventsList_t event, DriverCallback_t function, void *user_arg)
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
  }else if(bytes_read == 0)
  {
    status = STATUS_DRV_TIMED_OUT;
  }else
  {
    m_bytes_read = bytes_read;
  }

  m_read_status = status;

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
Status_t LinuxSerialFile::readFromThreadBlocking(DataBundle_t data_bundle, void *self_ptr)
{
  LinuxSerialFile *obj = static_cast<LinuxSerialFile *>(self_ptr);
  if(obj != nullptr)
  {
    return obj->readBlocking(data_bundle.buffer, data_bundle.size, data_bundle.timeout, true);
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
    else
    {
      m_bytes_written = bytes_written;
    }
  }
  else
  {
    status = convertErrnoCode(errno);
  }

  m_write_status = status;

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
Status_t LinuxSerialFile::writeFromThreadBlocking(DataBundle_t data_bundle, void *self_ptr)
{
  LinuxSerialFile *obj = static_cast<LinuxSerialFile *>(self_ptr);
  if(obj != nullptr)
  {
    return obj->writeBlocking(data_bundle.buffer, data_bundle.size, data_bundle.timeout, true);
  }
  return STATUS_DRV_NULL_POINTER;
}
