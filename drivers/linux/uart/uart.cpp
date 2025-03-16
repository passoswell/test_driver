/**
 * @file uart.cpp
 * @author your name (you@domain.com)
 * @brief Give access to UART functionalities
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "linux/uart/uart.hpp"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "linux/utils/linux_io.hpp"

static speed_t convertSpeed(uint32_t speed);

/**
 * @brief Constructor
 * @param port_handle A string containing the path to the peripheral
 */
UART::UART(const void *port_handle) :
m_rx_thread_handle(UART::readFromThreadBlocking, this),
m_tx_thread_handle(UART::writeFromThreadBlocking, this)
{
  m_handle = port_handle;
  m_linux_handle = -1;
  m_is_async_mode = false;
}

/**
 * @brief Destructor
 */
UART::~UART()
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
Status_t UART::configure(const DriverSettings_t *list, uint8_t list_size)
{
  Status_t status;
  struct termios termios_structure;
  speed_t speed = B1152000;
  uint32_t stop_bits_count = 1;
  bool use_parity = false, use_hw_flow_ctrl = false;

  if(m_handle == nullptr) { return STATUS_DRV_NULL_POINTER;}
  m_read_status = STATUS_DRV_NOT_CONFIGURED;
  m_write_status = STATUS_DRV_NOT_CONFIGURED;

  if(list != nullptr && list_size != 0)
  {
    for(uint8_t i = 0; i < list_size; i++)
    {
      switch(list[i].parameter)
      {
        case COMM_PARAM_STOP_BITS:
          if(list[i].value == 2) { stop_bits_count = 2;}
          break;
        case COMM_USE_HW_PARITY:
          use_parity = true;
          break;
        case COMM_USE_HW_FLOW_CTRL:
          use_hw_flow_ctrl = true;
          break;
        case COMM_PARAM_BAUD:
          speed = convertSpeed(list[i].value);
          break;
        case COMM_PARAM_CLOCK_SPEED:
          speed = convertSpeed(list[i].value);
          break;
        case COMM_PARAM_LINE_MODE:
          if(list[i].value == 0) { use_parity = false; stop_bits_count = 1; use_hw_flow_ctrl = false;}
          if(list[i].value == 1) { use_parity = true; stop_bits_count = 1; use_hw_flow_ctrl = false;}
          if(list[i].value == 2) { use_parity = false; stop_bits_count = 2; use_hw_flow_ctrl = false;}
          if(list[i].value == 3) { use_parity = true; stop_bits_count = 2; use_hw_flow_ctrl = false;}
          if(list[i].value == 4) { use_parity = false; stop_bits_count = 1; use_hw_flow_ctrl = true;}
          if(list[i].value == 5) { use_parity = true; stop_bits_count = 1; use_hw_flow_ctrl = true;}
          if(list[i].value == 6) { use_parity = false; stop_bits_count = 2; use_hw_flow_ctrl = true;}
          if(list[i].value == 7) { use_parity = true; stop_bits_count = 2; use_hw_flow_ctrl = true;}
          break;
        case COMM_WORK_ASYNC:
          m_is_async_mode = (bool) list[i].value;
          break;
        default:
          break;
      }
    }
  }

  // m_linux_handle = open((char *)m_handle, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
  m_linux_handle = open((char *)m_handle, O_RDWR | O_NOCTTY);
  if (m_linux_handle < 0)
  {
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to open file.\r\n");
    return status;
  }
  tcgetattr(m_linux_handle, &termios_structure);
  cfmakeraw(&termios_structure);
  cfsetispeed(&termios_structure, speed);
  cfsetospeed(&termios_structure, speed);
  if(use_parity)
  {
    termios_structure.c_cflag |= PARENB;
  }else
  {
    termios_structure.c_cflag &= ~PARENB;
  }
  if(use_hw_flow_ctrl)
  {
    termios_structure.c_cflag |= CRTSCTS;
  }else
  {
    termios_structure.c_cflag &= ~CRTSCTS;
  }
  if(stop_bits_count == 1)
  {
    termios_structure.c_cflag &= ~CSTOPB;
  }else
  {
    termios_structure.c_cflag |= CSTOPB;
  }

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
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to launch one or more UART tasks.\r\n");
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
 * @brief Read data
 * @param data Buffer to store the data
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t UART::read(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  int bytes_read = 0;
  DataBundle_t data_bundle;

  status = checkInputs(data, byte_count, timeout);
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
      return STATUS_DRV_SUCCESS;
    }else
    {
      m_read_status = STATUS_DRV_IDLE;
      return STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    status = readBlocking(data, byte_count, timeout, false);
    m_read_status = status;
    return status;
  }

  return STATUS_DRV_UNKNOWN_ERROR;
}

/**
 * @brief Write data
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t UART::write(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  int bytes_written, drain_status;
  DataBundle_t data_bundle;

  status = checkInputs(data, byte_count, timeout);
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
      return STATUS_DRV_SUCCESS;
    }else
    {
      m_write_status = STATUS_DRV_IDLE;
      return STATUS_DRV_ERR_BUSY;
    }
  }else
  {
    status = writeBlocking(data, byte_count, timeout, false);
    m_write_status = status;
    return status;
  }

  return STATUS_DRV_UNKNOWN_ERROR;
}

/**
 * @brief Install a callback function
 *
 * @param event An event to trigger the call
 * @param function A function to call back
 * @param user_arg A argument used as a parameter to the function
 * @return Status_t
 */
Status_t UART::setCallback(DriverEventsList_t event, DriverCallback_t function, void *user_arg)
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
 * @return Status_t
 */
Status_t UART::readBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back)
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
 * @brief Read data synchronously
 * @param data_bundle Data needed to perform the operation
 * @return Status_t
 */
Status_t UART::readFromThreadBlocking(DataBundle_t data_bundle, void *user_arg)
{
  UART *obj = static_cast<UART *>(user_arg);
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
 * @return Status_t
 */
Status_t UART::writeBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back)
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

Status_t UART::writeFromThreadBlocking(DataBundle_t data_bundle, void *user_arg)
{
  UART *obj = static_cast<UART *>(user_arg);
  if(obj != nullptr)
  {
    return obj->writeBlocking(data_bundle.buffer, data_bundle.size, data_bundle.timeout, true);
  }
  return STATUS_DRV_NULL_POINTER;
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
Status_t UART::checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout)
{
  if(buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(m_handle == nullptr || m_linux_handle < 0) { return STATUS_DRV_BAD_HANDLE;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Convert a speed value into something linux can understand
 *
 * @param speed
 * @return speed_t
 */
speed_t convertSpeed(uint32_t speed)
{
  speed_t output;

  switch(speed)
  {
    // POSIX compliant options
    case 0: output = B0; break;
    case 50: output = B50; break;
    case 75: output = B75; break;
    case 110: output = B110; break;
    case 134: output = B134; break;
    case 150: output = B150; break;
    case 200: output = B200; break;
    case 300: output = B300; break;
    case 600: output = B600; break;
    case 1200: output = B1200; break;
    case 1800: output = B1800; break;
    case 2400: output = B2400; break;
    case 4800: output = B4800; break;
    case 9600: output = B9600; break;
    case 19200: output = B19200; break;
    case 38400: output = B38400; break;
    case 57600: output = B57600; break;
    case 115200: output = B115200; break;
    case 230400: output = B230400; break;
    case 460800: output = B460800; break;

    // Extra output baud rates (not in POSIX)
    case 500000: output = B500000; break;
    case 576000: output = B576000; break;
    case 921600: output = B921600; break;
    case 1000000: output = B1000000; break;
    case 1152000: output = B1152000; break;
    case 1500000: output = B1500000; break;
    case 2000000: output = B2000000; break;
    case 2500000: output = B2500000; break;
    case 3000000: output = B3000000; break;
    case 3500000: output = B3500000; break;
    case 4000000: output = B4000000; break;
    default: output = B115200; break;
  }
  return output;
}
