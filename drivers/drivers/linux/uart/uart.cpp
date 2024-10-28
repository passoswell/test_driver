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

#include "drivers/linux/uart/uart.hpp"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "drivers/linux/utils/linux_io.hpp"

static speed_t convertSpeed(uint32_t speed);

/**
 * @brief Constructor
 * @param port_handle A string containing the path to the peripheral
 */
UART::UART(void *port_handle)
{
  m_handle = port_handle;
  m_linux_handle = -1;
  m_terminate = false;
  m_is_async_mode = false;

  m_sync_rx.run = false;
  m_sync_rx.buffer = nullptr;
  m_sync_rx.size = 0;
  m_sync_rx.timeout = 0;
  m_sync_rx.func = nullptr;
  m_sync_rx.arg = nullptr;
  m_sync_rx.thread = new std::thread(&UART::readAsyncThread, this);

  m_sync_tx.run = false;
  m_sync_tx.buffer = nullptr;
  m_sync_tx.size = 0;
  m_sync_tx.timeout = 0;
  m_sync_tx.func = nullptr;
  m_sync_tx.arg = nullptr;
  m_sync_tx.thread = new std::thread(&UART::writeAsyncThread, this);
}

/**
 * @brief Destructor
 */
UART::~UART()
{
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex,  std::defer_lock);
  std::unique_lock<std::mutex> locker2(m_sync_tx.mutex,  std::defer_lock);

  locker1.lock();
  locker2.lock();
  m_terminate = true;
  locker1.unlock();
  locker2.unlock();

  m_sync_rx.condition.notify_one();
  m_sync_tx.condition.notify_one();

  m_sync_rx.thread->join();
  m_sync_tx.thread->join();

  if(m_linux_handle >= 0)
  {
    (void) close(m_linux_handle);
  }
  delete m_sync_tx.thread;
  delete m_sync_rx.thread;
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
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex,  std::defer_lock);
  Status_t status;
  int bytes_read = 0;

  status = checkInputs(data, byte_count, timeout);
  if(!status.success) { return status;}
  if(m_sync_rx.run) { return STATUS_DRV_ERR_BUSY;}
  if(!locker1.try_lock())
  {
    return STATUS_DRV_ERR_BUSY;
  }

  m_bytes_read = 0;
  m_read_status = STATUS_DRV_RUNNING;
  m_sync_rx.run = true;

  if(m_is_async_mode)
  {
    m_sync_rx.run = true;
    m_sync_rx.buffer = data;
    m_sync_rx.size = byte_count;
    m_sync_rx.timeout = timeout;
    locker1.unlock();
    m_sync_rx.condition.notify_one();
    return STATUS_DRV_SUCCESS;
  }else
  {
    status = readBlocking(data, byte_count, timeout);
    m_sync_rx.run = false;
    m_read_status = status;
    locker1.unlock();
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
Status_t UART::write(const uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex,  std::defer_lock);
  Status_t status;
  int bytes_written, drain_status;
  (void) timeout;

  status = checkInputs(data, byte_count, timeout);
  if(!status.success) { return status;}
  if(m_sync_tx.run) { return STATUS_DRV_ERR_BUSY;}
  if(!locker1.try_lock())
  {
    return STATUS_DRV_ERR_BUSY;
  }

  m_bytes_written = 0;
  m_write_status = STATUS_DRV_RUNNING;
  m_sync_tx.run = true;

  if(m_is_async_mode)
  {
    m_sync_tx.buffer_const = data;
    m_sync_tx.size = byte_count;
    m_sync_tx.timeout = timeout;
    locker1.unlock();
    m_sync_tx.condition.notify_one();
    return STATUS_DRV_SUCCESS;
  }else
  {
    status = writeBlocking(data, byte_count, timeout);
    m_sync_tx.run = false;
    m_write_status = status;
    locker1.unlock();
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
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex,  std::defer_lock);
  std::unique_lock<std::mutex> locker2(m_sync_rx.mutex,  std::defer_lock);
  Status_t status = STATUS_DRV_SUCCESS;

  if(m_sync_rx.run || m_sync_tx.run) { return STATUS_DRV_ERR_BUSY;}

  if(!locker1.try_lock())
  {
    return STATUS_DRV_ERR_BUSY;
  }else if(!locker2.try_lock())
  {
    locker1.unlock();
    return STATUS_DRV_ERR_BUSY;
  }

  switch (event)
  {
  case EVENT_READ:
    m_sync_rx.func = function;
    m_sync_rx.arg = user_arg;
    break;
  case EVENT_WRITE:
    m_sync_tx.func = function;
    m_sync_tx.arg = user_arg;
    break;
  default:
    status = STATUS_DRV_ERR_PARAM;
    break;
  }

  locker1.unlock();
  locker2.unlock();
  return status;
}

/**
 * @brief Read data synchronously
 * @param data Buffer to store the data
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t UART::readBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout)
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
    status = STATUS_DRV_ERR_TIMEOUT;
  }else
  {
    m_bytes_read = bytes_read;
  }

  return status;
}

/**
 * @brief Thread that reads data from an uart port
 */
void UART::readAsyncThread(void)
{
  Status_t status;
  int32_t byte_count;
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex);

  while(true)
  {
    m_sync_rx.condition.wait(locker1, [this]{ return this->m_sync_rx.run | this->m_terminate; });
    if (m_terminate)
    {
      break;
    }

    m_bytes_read = 0;
    byte_count = 0;

    status = readBlocking(m_sync_rx.buffer, m_sync_rx.size, m_sync_rx.timeout);

    if (m_sync_rx.func != nullptr)
    {
      Buffer_t data(m_sync_rx.buffer, m_bytes_read);
      m_sync_rx.func(status, EVENT_READ, data, m_sync_rx.arg);
    }

    m_read_status = status;
    m_sync_rx.run = false;
  }
}

/**
 * @brief Write data synchronously
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t UART::writeBlocking(const uint8_t *data, Size_t byte_count, uint32_t timeout)
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
  return status;
}

/**
 * @brief Thread that writes data to an uart port
 */
void UART::writeAsyncThread(void)
{
  Status_t status;
  uint32_t byte_count;
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex);

  while(true)
  {
    m_sync_tx.condition.wait(locker1, [this]{ return this->m_sync_tx.run | this->m_terminate; });
    if(m_terminate) { break;}

    status = writeBlocking(m_sync_tx.buffer_const, m_sync_tx.size, m_sync_tx.timeout);

    if(m_sync_tx.func != nullptr)
    {
      Buffer_t data(m_sync_tx.buffer, m_bytes_written);
      m_sync_tx.func(status, EVENT_WRITE, data, m_sync_tx.arg);
    }

    m_write_status = status;
    m_sync_tx.run = false;
  }
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
