/**
 * @file drv_std_in_out.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "drivers/linux/std_in_out/drv_std_in_out.hpp"



/**
 * @brief Constructor
 */
DrvStdInOut::DrvStdInOut()
{
  m_terminate = false;

  m_in_file = fopen("/dev/fd/0", "r+");
  m_sync_rx.run = false;
  m_sync_rx.buffer = nullptr;
  m_sync_rx.size = 0;
  m_sync_rx.func = nullptr;
  m_sync_rx.arg = nullptr;
  m_sync_rx.thread = new std::thread(&DrvStdInOut::readAsyncThread, this);

  m_out_file = fopen("/dev/fd/1", "w");
  m_sync_tx.run = false;
  m_sync_tx.buffer = nullptr;
  m_sync_tx.size = 0;
  m_sync_tx.func = nullptr;
  m_sync_tx.arg = nullptr;
  m_sync_tx.thread = new std::thread(&DrvStdInOut::writeAsyncThread, this);
}

/**
 * @brief Destructor
 */
DrvStdInOut::~DrvStdInOut()
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

  if (m_in_file != nullptr)
  {
    fclose(m_in_file);
  }
  if (m_out_file != nullptr)
  {
    fclose(m_out_file);
  }
  delete m_sync_tx.thread;
  delete m_sync_rx.thread;
}

/**
 * @brief Read data synchronously
 * @param buffer Buffer to store the data
 * @param size Number of bytes to read
 * @param key Not used
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvStdInOut::read(uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout)
{
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex,  std::defer_lock);
  uint32_t counter = 0;
  uint8_t byte;
  (void) key;
  (void) timeout;

  if (buffer == nullptr || m_in_file == nullptr || buffer == nullptr)
  {
    return STATUS_DRV_NULL_POINTER;
  }
  if(size == 0)
  {
    return STATUS_DRV_ERR_PARAM_SIZE;
  }
  if(!locker1.try_lock())
  {
    return STATUS_DRV_ERR_BUSY;
  }

  m_is_read_done = false;
  m_is_operation_done = false;
  m_sync_rx.run = true;

  while (bytesRead() < size)
  {
  }
  m_bytes_read = fread(buffer, 1, size, m_in_file);

  m_is_read_done = true;
  m_is_operation_done = true;
  m_sync_rx.run = false;
  locker1.unlock();
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
Status_t DrvStdInOut::write(uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout)
{
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex,  std::defer_lock);
  (void) key;
  (void) timeout;

  if(buffer == nullptr || m_out_file == nullptr || buffer == nullptr)
  {
    return STATUS_DRV_NULL_POINTER;
  }
  if(size == 0)
  {
    return STATUS_DRV_ERR_PARAM_SIZE;
  }
  if(!locker1.try_lock())
  {
    return STATUS_DRV_ERR_BUSY;
  }

  m_is_write_done = false;
  m_is_operation_done = false;
  m_sync_tx.run = true;

  fwrite(buffer, 1, size, m_out_file);
  fwrite("\r\n", 1, 2, m_out_file);
  fflush(m_out_file);

  m_is_write_done = true;
  m_is_operation_done = true;
  m_sync_tx.run = false;
  locker1.unlock();
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
Status_t DrvStdInOut::readAsync(uint8_t *buffer, uint32_t size, uint32_t key, InOutStreamCallback_t func, void *arg)
{
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex,  std::defer_lock);
  (void) key;
  if (buffer == nullptr || m_in_file == nullptr || buffer == nullptr)
  {
    return STATUS_DRV_NULL_POINTER;
  }
  if(size == 0)
  {
    return STATUS_DRV_ERR_PARAM_SIZE;
  }
  if(m_sync_rx.run)
  {
    return STATUS_DRV_ERR_BUSY;
  }
  if(!locker1.try_lock())
  {
    return STATUS_DRV_ERR_BUSY;
  }

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
bool DrvStdInOut::isReadAsyncDone()
{
  bool status;
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex, std::defer_lock);
  if(!locker1.try_lock()) { return false;}
  status = m_is_read_done;
  locker1.unlock();
  return status;
}

/**
 * @brief Return the number of bytes read in the last readAsync operation
 * @return uint32_t
 */
uint32_t DrvStdInOut::bytesRead()
{
  uint32_t prev = ftell(m_in_file);
  fseek(m_in_file, 0L, SEEK_END);
  uint32_t sz = ftell(m_in_file);
  fseek(m_in_file, prev, SEEK_SET); // go back to where we were
  return sz;
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
Status_t DrvStdInOut::writeAsync(uint8_t *buffer, uint32_t size, uint32_t key, InOutStreamCallback_t func, void *arg)
{
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex,  std::defer_lock);
  (void) key;
  if (buffer == nullptr || m_in_file == nullptr || buffer == nullptr)
  {
    return STATUS_DRV_NULL_POINTER;
  }
  if(size == 0)
  {
    return STATUS_DRV_ERR_PARAM_SIZE;
  }
  if(m_sync_tx.run)
  {
    return STATUS_DRV_ERR_BUSY;
  }
  if(!locker1.try_lock())
  {
    return STATUS_DRV_ERR_BUSY;
  }

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
bool DrvStdInOut::isWriteAsyncDone()
{
  bool status;
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex,  std::defer_lock);
  if(!locker1.try_lock()) { return false;}
  status = m_is_write_done;
  locker1.unlock();
  return status;
}

void DrvStdInOut::readAsyncThread(void)
{
  uint32_t prev, bytes_available = 0;
  std::unique_lock<std::mutex> locker1(m_sync_rx.mutex);
  while(true)
  {
    m_sync_rx.condition.wait(locker1, [this]{ return this->m_sync_rx.run | this->m_terminate; });
    if(m_terminate)
    {
      std::cout << "Terminating readAsyncThread thread" << std::endl;
      break;
    }
    while (bytes_available < m_sync_rx.size)
    {
      prev = ftell(m_in_file);
      fseek(m_in_file, 0L, SEEK_END);
      bytes_available = ftell(m_in_file);
      fseek(m_in_file, prev, SEEK_SET); // go back to where we were
    }
    m_bytes_read = fread(m_sync_rx.buffer, 1, m_sync_rx.size, m_in_file);
    if (m_sync_rx.func != nullptr)
    {
      m_sync_rx.func(STATUS_DRV_SUCCESS, m_sync_rx.buffer, bytes_available, m_sync_rx.arg);
    }
    m_is_read_done = true;
    m_is_operation_done = true;
    m_sync_rx.run = false;
  }
}

void DrvStdInOut::writeAsyncThread(void)
{
  std::unique_lock<std::mutex> locker1(m_sync_tx.mutex);
  while(true)
  {
    m_sync_tx.condition.wait(locker1, [this]{ return this->m_sync_tx.run | this->m_terminate; });
    if(m_terminate)
    {
      std::cout << "Terminating writeAsyncThread thread" << std::endl;
      break;
    }
    fwrite(m_sync_tx.buffer, 1, m_sync_tx.size, m_out_file);
    fwrite("\r\n", 1, 2, m_out_file);
    fflush(m_out_file);
    if(m_sync_tx.func != nullptr)
    {
      m_sync_tx.func(STATUS_DRV_SUCCESS, m_sync_tx.buffer, m_sync_tx.size, m_sync_tx.arg);
    }
    m_is_write_done = true;
    m_is_operation_done = true;
    m_sync_tx.run = false;
  }
}