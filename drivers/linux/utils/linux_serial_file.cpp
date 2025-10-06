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

#include <cstdio>
#include <cstring>

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
  m_event_handler_rx = nullptr;
  m_event_handler_tx = nullptr;
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
 * @return ErrorCode
 */
ErrorCode LinuxSerialFile::configure(const SettingsList_t *list, uint8_t list_size)
{
  ErrorCode status = SerialFileErrorCode::kSuccess;
  struct termios termios_structure;
  bool result;

  if(m_handle == nullptr) { return SerialFileErrorCode::kNullPointer;}

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
      status = SerialFileErrorCode::kFailed;
      status.setMessage("Failed to launch LinuxSerialFile task for reading");
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
      status = SerialFileErrorCode::kFailed;
      status.setMessage("Failed to launch LinuxSerialFile task for writing");
      return status;
    }
  }else
  {
    (void) m_tx_thread_handle.terminate();
  }

  m_linux_handle = open((char *)m_handle, O_RDWR | O_NOCTTY);
  if (m_linux_handle < 0)
  {
    status = SerialFileErrorCode::kFailed;
    status.setMessage("Failed to open file");
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

  return SerialFileErrorCode::kSuccess;
}

/**
 * @brief Read data from a linux file
 * @param data Buffer to store the data on reception
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return ErrorCode
 */
ErrorCode LinuxSerialFile::read(DrvBuffer_t data, uint32_t timeout)
{
  ErrorCode status;
  int bytes_read = 0;
  DrvDataBundle_t data_bundle;

  status = checkInputs(data, timeout);
  if(status) { return status;}

  m_bytes_read = 0;

  if(m_is_async_mode_rx)
  {
    data_bundle.rx.data = data;
    data_bundle.rx.timeout = timeout;
    if(m_rx_thread_handle.setInputData(data_bundle, 0))
    {
      status = SerialFileErrorCode::kSuccess;
    }else
    {
      status = SerialFileErrorCode::kBusy;
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
 * @return ErrorCode
 */
ErrorCode LinuxSerialFile::write(DrvBuffer_t data, uint32_t timeout)
{
  ErrorCode status;
  int bytes_written, drain_status;
  DrvDataBundle_t data_bundle;

  status = checkInputs(data, timeout);
  if(status) { return status;}

  if(m_is_async_mode_tx)
  {
    data_bundle.tx.data = data;
    data_bundle.tx.timeout = timeout;
    if(m_tx_thread_handle.setInputData(data_bundle, 0))
    {
      status = SerialFileErrorCode::kSuccess;
    }else
    {
      status = SerialFileErrorCode::kBusy;
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
 * @return ErrorCode
 */
ErrorCode LinuxSerialFile::setCallback(EventsList_t event, iCallback &event_handler)
{
  ErrorCode status = SerialFileErrorCode::kSuccess;

  switch (event)
  {
  case EVENT_READ:
    m_event_handler_rx = &event_handler;
    break;
  case EVENT_WRITE:
    m_event_handler_tx = &event_handler;
    break;
  default:
    status = SerialFileErrorCode::kInvalidParameter;
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
 * @return ErrorCode
 */
ErrorCode LinuxSerialFile::readBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back)
{
  ErrorCode status = SerialFileErrorCode::kSuccess;
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
    status = SerialFileErrorCode::kFailed;
    status.setMessage(std::strerror(errno));
  }else
  {
    m_bytes_read = bytes_read;
    if(bytes_read == 0)
    {
      status = SerialFileErrorCode::kTimedOut;
    }
  }

  if(call_back && m_event_handler_rx != nullptr)
  {
    Buffer_t data_container(data, m_bytes_read);
    m_event_handler_rx->onEvent(status, EVENT_READ, data_container);
  }

  return status;
}

/**
 * @brief Thread to perform file read operation in parallel
 * @param data_bundle Data needed to perform the operation
 * @param self_ptr A pointer to a object of type LinuxSerialFile
 * @return ErrorCode
 */
ErrorCode LinuxSerialFile::readFromThreadBlocking(DrvDataBundle_t data_bundle, void *self_ptr)
{
  LinuxSerialFile *obj = static_cast<LinuxSerialFile *>(self_ptr);
  if(obj != nullptr)
  {
    return obj->readBlocking(data_bundle.rx.data.data(), data_bundle.rx.data.size(), data_bundle.rx.timeout, true);
  }
  return SerialFileErrorCode::kNullPointer;
}

/**
 * @brief Write data synchronously
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @param call_back True if should call the callback function
 * @return ErrorCode
 */
ErrorCode LinuxSerialFile::writeBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back)
{
  ErrorCode status = SerialFileErrorCode::kSuccess;
  int bytes_written, drain_status;
  bytes_written = writeSyscall(m_linux_handle, data, byte_count);
  if (byte_count >= 0)
  {
    drain_status = tcdrain(m_linux_handle);
    if (drain_status < 0)
    {
      status = SerialFileErrorCode::kFailed;
      status.setMessage(std::strerror(errno));
    }
  }
  else
  {
    status = SerialFileErrorCode::kFailed;
    status.setMessage(std::strerror(errno));
  }

  if(call_back && m_event_handler_tx != nullptr)
  {
    Buffer_t data_container(data, m_bytes_read);
    m_event_handler_tx->onEvent(status, EVENT_WRITE, data_container);
  }

  return status;
}

/**
 * @brief Thread to perform file write operation in parallel
 * @param data_bundle Data needed to perform the operation
 * @param self_ptr A pointer to a object of type LinuxSerialFile
 * @return ErrorCode
 */
ErrorCode LinuxSerialFile::writeFromThreadBlocking(DrvDataBundle_t data_bundle, void *self_ptr)
{
  LinuxSerialFile *obj = static_cast<LinuxSerialFile *>(self_ptr);
  if(obj != nullptr)
  {
    return obj->writeBlocking(data_bundle.rx.data.data(), data_bundle.rx.data.size(), data_bundle.rx.timeout, true);
  }
  return SerialFileErrorCode::kNullPointer;
}

/**
 * @brief Verify if the inputs are in ther expected range
 *
 * @param buffer Data buffer
 * @param size Number of bytes in the data buffer
 * @param timeout Operation timeout value
 * @param key Parameter
 * @return ErrorCode
 */
ErrorCode LinuxSerialFile::checkInputs(const DrvBuffer_t data, uint32_t timeout)
{
  if(data.data() == nullptr) { return SerialFileErrorCode::kNullPointer;}
  if(m_linux_handle < 0) { return SerialFileErrorCode::kBadHandle;}
  if(data.size_bytes() == 0) { return SerialFileErrorCode::kInvalidParameter;}
  return SerialFileErrorCode::kSuccess;
}
