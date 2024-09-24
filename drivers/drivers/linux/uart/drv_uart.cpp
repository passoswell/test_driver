/**
 * @file drv_uart.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#if __has_include("termios.h")

#include "drivers/linux/uart/drv_uart.hpp"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "drivers/linux/utils/linux_io.hpp"

static Status_t convertErrorCode(int code);

static speed_t convertSpeed(uint32_t speed);

/**
 * @brief Constructor
 * @param port_handle A string containing the path to the peripheral
 */
DrvUART::DrvUART(void *port_handle)
{
  m_handle = port_handle;
  m_linux_handle = -1;
  m_terminate = false;

  m_sync_rx.run = false;
  m_sync_rx.buffer = nullptr;
  m_sync_rx.size = 0;
  m_sync_rx.func = nullptr;
  m_sync_rx.arg = nullptr;
  m_sync_rx.thread = new std::thread(&DrvUART::readAsyncThread, this);

  m_sync_tx.run = false;
  m_sync_tx.buffer = nullptr;
  m_sync_tx.size = 0;
  m_sync_tx.func = nullptr;
  m_sync_tx.arg = nullptr;
  m_sync_tx.thread = new std::thread(&DrvUART::writeAsyncThread, this);
}

/**
 * @brief Destructor
 */
DrvUART::~DrvUART()
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
Status_t DrvUART::configure(const InOutStreamSettings_t *list, uint8_t list_size)
{
  Status_t status;
  struct termios termios_structure;
  speed_t speed = B1152000;
  uint32_t stop_bits_count = 1;
  bool use_parity = false, use_hw_flow_ctrl = false;

  if(m_handle == nullptr) { return STATUS_DRV_NULL_POINTER;}

  if(list != nullptr && list_size != 0)
  {
    for(uint8_t i = 0; i < list_size; i++)
    {
      switch(list[i].parameter)
      {
        case DRV_PARAM_STOP_BITS:
          if(list[i].value == 2) { stop_bits_count = 2;}
          break;
        case DRV_USE_HW_PARITY:
          use_parity = true;
          break;
        case DRV_USE_HW_FLOW_CTRL:
          use_hw_flow_ctrl = true;
          break;
        case DRV_PARAM_BAUD:
          speed = convertSpeed(list[i].value);
          break;
        case DRV_PARAM_CLOCK_SPEED:
          speed = convertSpeed(list[i].value);
          break;
        case DRV_PARAM_LINE_MODE:
          if(list[i].value == 0) { use_parity = false; stop_bits_count = 1; use_hw_flow_ctrl = false;}
          if(list[i].value == 1) { use_parity = true; stop_bits_count = 1; use_hw_flow_ctrl = false;}
          if(list[i].value == 2) { use_parity = false; stop_bits_count = 2; use_hw_flow_ctrl = false;}
          if(list[i].value == 3) { use_parity = true; stop_bits_count = 2; use_hw_flow_ctrl = false;}
          if(list[i].value == 4) { use_parity = false; stop_bits_count = 1; use_hw_flow_ctrl = true;}
          if(list[i].value == 5) { use_parity = true; stop_bits_count = 1; use_hw_flow_ctrl = true;}
          if(list[i].value == 6) { use_parity = false; stop_bits_count = 2; use_hw_flow_ctrl = true;}
          if(list[i].value == 7) { use_parity = true; stop_bits_count = 2; use_hw_flow_ctrl = true;}
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
  termios_structure.c_cc[VMIN] = 1;
  termios_structure.c_cc[VTIME] = 1;
  tcflush(m_linux_handle, TCIFLUSH);
  tcflush(m_linux_handle, TCIFLUSH);
  tcsetattr(m_linux_handle, TCSANOW, &termios_structure);

  m_is_initialized = true;
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Read data synchronously
 * @param buffer Buffer to store the data
 * @param size Number of bytes to read
 * @param key Not used
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvUART::read(uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout)
{
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex,  std::defer_lock);
  int byte_count = 0;
  (void) key;

  if(!m_is_initialized && !configure(nullptr, 0).success) { return STATUS_DRV_NULL_POINTER;}
  if(m_handle == nullptr || m_linux_handle < 0 || buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  if(!locker1.try_lock()) { return STATUS_DRV_ERR_BUSY;}

  m_bytes_read = 0;
  m_is_read_done = false;
  m_is_operation_done = false;
  m_sync_rx.run = true;

  auto start = std::chrono::steady_clock::now();
  auto end = start;
  std::chrono::duration<double> elapsed_time;
  do
  {
    byte_count += readSyscall(m_linux_handle, buffer + byte_count, size - byte_count);
    end = std::chrono::steady_clock::now();
    elapsed_time = end - start;
  } while ((byte_count < size) || ((elapsed_time.count() * 1000) < timeout));

  byte_count = readSyscall(m_linux_handle, buffer, byte_count);

  m_is_read_done = true;
  m_is_operation_done = true;
  m_sync_rx.run = false;
  locker1.unlock();

  if(byte_count < 0)
  {
    return convertErrorCode(errno);;
  }
  m_bytes_read = byte_count;
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Write data synchronously
 * @param buffer Buffer where data is stored
 * @param size Number of bytes to write
 * @param key Not used
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvUART::write(uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout)
{
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex,  std::defer_lock);
  int byte_count;
  (void) key;
  (void) timeout;

  if(!m_is_initialized && !configure(nullptr, 0).success) { return STATUS_DRV_NULL_POINTER;}
  if(m_handle == nullptr || m_linux_handle < 0 || buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  if(!locker1.try_lock()) { return STATUS_DRV_ERR_BUSY;}

  m_is_write_done = false;
  m_is_operation_done = false;
  m_sync_tx.run = true;

  byte_count = writeSyscall(m_linux_handle, buffer, size);

  m_is_write_done = true;
  m_is_operation_done = true;
  m_sync_tx.run = false;

  if(byte_count < 0)
  {
    return convertErrorCode(errno);;
  }
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Read data asynchronously
 * @param buffer Buffer to store the data
 * @param size Number of bytes to read
 * @param key Not used
 * @param func Pointer to a function to call at the end of operation
 * @param arg Parameter to pass to the callback function
 * @return Status_t
 */
Status_t DrvUART::readAsync(uint8_t *buffer, uint32_t size, uint32_t key, InOutStreamCallback_t func, void *arg)
{
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex,  std::defer_lock);

  if(!m_is_initialized && !configure(nullptr, 0).success) { return STATUS_DRV_NULL_POINTER;}
  if(m_handle == nullptr || m_linux_handle < 0 || buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  if(m_sync_rx.run) { return STATUS_DRV_ERR_BUSY;}
  if(!locker1.try_lock()) { return STATUS_DRV_ERR_BUSY;}

  m_bytes_read = 0;
  m_is_read_done = false;
  m_is_operation_done = false;
  m_sync_rx.run = true;
  m_sync_rx.buffer = buffer;
  m_sync_rx.size = size;
  m_sync_rx.key = key;
  m_sync_rx.func = func;
  m_sync_rx.arg = arg;
  locker1.unlock();
  m_sync_rx.condition.notify_one();

  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Asynchronous read operation status
 * @return true if operation done, false otherwise
 */
bool DrvUART::isReadAsyncDone()
{
  bool status;
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex, std::defer_lock);
  if(!locker1.try_lock()) { return false;}
  status = m_is_read_done;
  locker1.unlock();
  return status;
}

/**
 * @brief Write data asynchronously
 * @param buffer Buffer where data is stored
 * @param size Number of bytes to write
 * @param key Not used
 * @param func Pointer to a function to call at the end of operation
 * @param arg Parameter to pass to the callback function
 * @return Status_t
 */
Status_t DrvUART::writeAsync(uint8_t *buffer, uint32_t size, uint32_t key, InOutStreamCallback_t func, void *arg)
{
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex,  std::defer_lock);

  if(!m_is_initialized && !configure(nullptr, 0).success) { return STATUS_DRV_NULL_POINTER;}
  if(m_handle == nullptr || m_linux_handle < 0 || buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  if(m_sync_tx.run) { return STATUS_DRV_ERR_BUSY;}
  if(!locker1.try_lock()) { return STATUS_DRV_ERR_BUSY;}

  m_is_write_done = false;
  m_is_operation_done = false;
  m_sync_tx.run = true;
  m_sync_tx.buffer = buffer;
  m_sync_tx.size = size;
  m_sync_tx.key = key;
  m_sync_tx.func = func;
  m_sync_tx.arg = arg;
  locker1.unlock();
  m_sync_tx.condition.notify_one();

  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Asynchronous write operation status
 * @return true if operation done, false otherwise
 */
bool DrvUART::isWriteAsyncDone()
{
  bool status;
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex,  std::defer_lock);
  if(!locker1.try_lock()) { return false;}
  status = m_is_write_done;
  locker1.unlock();
  return status;
}

void DrvUART::readAsyncThread(void)
{
  Status_t success;
  uint32_t byte_count;
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex);

  while(true)
  {
    m_sync_rx.condition.wait(locker1, [this]{ return this->m_sync_rx.run | this->m_terminate; });
    if(m_terminate) { break;}
    byte_count = readSyscall(m_linux_handle, m_sync_rx.buffer, m_sync_rx.size);
    if(byte_count > 0)
    {
      m_bytes_read = byte_count;
      success = STATUS_DRV_SUCCESS;
    }else
    {
      m_bytes_read = 0;
      success = convertErrorCode(errno);
    }
    if (m_sync_rx.func != nullptr)
    {
      m_sync_rx.func(success, m_sync_rx.buffer, m_bytes_read, m_sync_rx.arg);
    }
    m_is_read_done = true;
    m_is_operation_done = true;
    m_sync_rx.run = false;
  }
}

void DrvUART::writeAsyncThread(void)
{
  Status_t success;
  uint32_t byte_count;
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex);

  while(true)
  {
    m_sync_tx.condition.wait(locker1, [this]{ return this->m_sync_tx.run | this->m_terminate; });
    if(m_terminate) { break;}
    byte_count = writeSyscall(m_linux_handle, m_sync_tx.buffer, m_sync_tx.size);
    if(byte_count == m_sync_tx.size)
    {
      m_bytes_read = byte_count;
      success = STATUS_DRV_SUCCESS;
    }else
    {
      m_bytes_read = 0;
      if(byte_count > 0){m_bytes_read = byte_count;}
      success = convertErrorCode(errno);
    }
    if(m_sync_tx.func != nullptr)
    {
      m_sync_tx.func(success, m_sync_tx.buffer, m_sync_tx.size, m_sync_tx.arg);
    }
    m_is_write_done = true;
    m_is_operation_done = true;
    m_sync_tx.run = false;
  }
}

Status_t convertErrorCode(int code)
{
  Status_t status;
  switch(code)
  {
    case 0:
      status = STATUS_DRV_SUCCESS;
      break;
    default:
      status = STATUS_DRV_UNKNOWN_ERROR;
      break;
  }
  return status;
}

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

#endif
