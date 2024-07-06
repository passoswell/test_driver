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

Status_t convertErrorCode(int code);

/**
 * @brief Constructor
 * @param port_handle A string containing the path to the peripheral
 */
DrvUART::DrvUART(void *port_handle)
{
  m_handle = port_handle;
  m_linux_handle = -1;

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
  if(m_linux_handle >= 0)
  {
    (void) close(m_linux_handle);
  }
  delete m_sync_tx.thread;
  delete m_sync_rx.thread;
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameters
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t DrvUART::configure(const InOutStreamConfigure_t *list, uint8_t list_size)
{
  Status_t status;
  struct termios termios_structure;
  if(m_handle == nullptr)
  {
    return STATUS_DRV_NULL_POINTER;
  }
  if(list == nullptr || list_size == 0)
  {
    // m_linux_handle = open((char *)m_handle, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    m_linux_handle = open((char *)m_handle, O_RDWR | O_NOCTTY);
    if (m_linux_handle < 0)
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to open file.\r\n");
      return status;
    }
    tcgetattr(m_linux_handle, &termios_structure);
    cfmakeraw(&termios_structure);
    cfsetispeed(&termios_structure, B1152000);
    cfsetospeed(&termios_structure, B1152000);
    termios_structure.c_cc[VMIN] = 1;
    termios_structure.c_cc[VTIME] = 1;
    tcflush(m_linux_handle, TCIFLUSH);
    tcflush(m_linux_handle, TCIFLUSH);
    tcsetattr(m_linux_handle, TCSANOW, &termios_structure);
  }
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
Status_t DrvUART::read(uint8_t *buffer, uint32_t size, uint8_t key, uint32_t timeout)
{
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex,  std::defer_lock);
  int byte_count = 0;
  (void) key;

  if(m_handle == nullptr || m_linux_handle < 0) { return STATUS_DRV_NULL_POINTER;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  if(!locker1.try_lock()) { return STATUS_DRV_ERR_BUSY;}

  m_bytes_read = 0;
  m_is_read_done = false;
  m_is_operation_done = false;
  m_sync_rx.run = true;

  byte_count = waitOnReceptionTimeoutSyscall(m_linux_handle, size, timeout);

  if(byte_count > size)
  {
    byte_count = size;
  }
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
Status_t DrvUART::write(uint8_t *buffer, uint32_t size, uint8_t key, uint32_t timeout)
{
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex,  std::defer_lock);
  int byte_count;
  (void) key;
  (void) timeout;

  if(m_handle == nullptr || m_linux_handle < 0) { return STATUS_DRV_NULL_POINTER;}
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
Status_t DrvUART::readAsync(uint8_t *buffer, uint32_t size, uint8_t key, InOutStreamCallback_t func, void *arg)
{
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex,  std::defer_lock);
  (void) key;
  if(m_handle == nullptr || m_linux_handle < 0) { return STATUS_DRV_NULL_POINTER;}
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
 * @brief Return the number of bytes read in the last read operation
 * @return uint32_t
 */
uint32_t DrvUART::bytesRead()
{
  return m_bytes_read;
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
Status_t DrvUART::writeAsync(uint8_t *buffer, uint32_t size, uint8_t key, InOutStreamCallback_t func, void *arg)
{
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex,  std::defer_lock);
  (void) key;

  if(m_handle == nullptr || m_linux_handle < 0) { return STATUS_DRV_NULL_POINTER;}
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

void DrvUART::readAsyncThread(void)
{
  Status_t success;
  uint32_t byte_count;
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex);

  while(true)
  {
    m_sync_rx.condition.wait(locker1, [this]{ return this->m_sync_rx.run; });
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
    else
    {
      readAsyncDoneCallback(success, m_sync_rx.buffer, m_bytes_read);
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
    m_sync_tx.condition.wait(locker1, [this]{ return this->m_sync_tx.run; });
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
    }else
    {
      writeAsyncDoneCallback(success, m_sync_tx.buffer, m_sync_tx.size);
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

#endif
