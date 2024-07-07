/**
 * @file drv_i2c.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#if __has_include("sys/ioctl.h")

#include "drivers/linux/i2c/drv_i2c.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "drivers/linux/utils/linux_io.hpp"

static Status_t convertErrorCode(int code);



/* ----- commands for the ioctl like i2c_command call:
 * note that additional calls are defined in the algorithm and hw
 *  dependent layers - these can be listed here, or see the
 *  corresponding header files.
 */
        /* -> bit-adapter specific ioctls  */
#define I2C_RETRIES  0x0701  /* number of times a device address      */
        /* should be polled when not            */
                                /* acknowledging       */
#define I2C_TIMEOUT  0x0702  /* set timeout - call with int     */


/* this is for i2c-dev.c  */
#define I2C_PERIPHERAL_7BITS_ADDRESS  0x0703  /* Change slave address      */
        /* Attn.: Slave address is 7 or 10 bits */
#define I2C_PERIPHERAL_7BITS_ADDRESS_FORCE  0x0706  /* Change slave address   */
        /* Attn.: Slave address is 7 or 10 bits */
        /* This changes the address, even if it */
        /* is already taken!      */
#define I2C_TENBIT  0x0704  /* 0 for 7 bit addrs, != 0 for 10 bit  */

#define I2C_FUNCS  0x0705  /* Get the adapter functionality */
#define I2C_RDWR  0x0707  /* Combined R/W transfer (one stop only)*/
#define I2C_PEC    0x0708  /* != 0 for SMBus PEC                   */
#if 0
#define I2C_ACK_TEST  0x0710  /* See if a slave is at a specific address */
#endif

#define I2C_SMBUS  0x0720  /* SMBus-level access */


int convertPort(char *handle);


/**
 * @brief Constructor
 * @param port_handle A string containing the path to the peripheral
 */
DrvI2C::DrvI2C(void *port_handle)
{
  m_handle = port_handle;
  m_linux_handle = -1;
  m_is_reading = false;
  m_is_writing = false;
  m_terminate = false;

  m_sync.run = false;
  m_sync.buffer = nullptr;
  m_sync.size = 0;
  m_sync.func = nullptr;
  m_sync.arg = nullptr;
  m_sync.thread = new std::thread(&DrvI2C::asyncThread, this);
}

/**
 * @brief Destructor
 */
DrvI2C::~DrvI2C()
{
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);

  locker1.lock();
  m_terminate = true;
  m_sync.run = true;
  locker1.unlock();

  m_sync.condition.notify_one();

  m_sync.thread->join();

  if(m_linux_handle >= 0)
  {
    close(m_linux_handle);
  }
  delete m_sync.thread;
}

/**
 * @brief Read data synchronously
 * @param buffer Buffer to store the data
 * @param size Number of bytes to read
 * @param address_8bits The 8 bits address of the device
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvI2C::read(uint8_t *buffer, uint32_t size, uint8_t address_8bits, uint32_t timeout)
{
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);
  int byte_count;
  Status_t status = STATUS_DRV_SUCCESS;
  (void) timeout;

  if(m_handle == nullptr || buffer == nullptr){return STATUS_DRV_NULL_POINTER;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  if(!locker1.try_lock()) { return STATUS_DRV_ERR_BUSY;}

  m_is_read_done = false;
  m_is_operation_done = false;
  m_sync.run = false;

  status = i2cRead(buffer, size, address_8bits);

  m_is_read_done = true;
  m_is_operation_done = true;
  m_sync.run = false;
  locker1.unlock();
  return status;
}

/**
 * @brief Write data synchronously
 * @param buffer Buffer where data is stored
 * @param size Number of bytes to write
 * @param address_8bits The 8 bits address of the device
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvI2C::write(uint8_t *buffer, uint32_t size, uint8_t address_8bits, uint32_t timeout)
{
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);
  int byte_count;
  Status_t status = STATUS_DRV_SUCCESS;
  (void) timeout;

  if(m_handle == nullptr || buffer == nullptr){return STATUS_DRV_NULL_POINTER;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  if(!locker1.try_lock()) { return STATUS_DRV_ERR_BUSY;}

  m_is_write_done = false;
  m_is_operation_done = false;
  m_sync.run = false;

  status = i2cWrite(buffer, size, address_8bits);

  m_is_write_done = true;
  m_is_operation_done = true;
  m_sync.run = false;
  locker1.unlock();
  return status;
}

/**
 * @brief Read data asynchronously
 * @param buffer Buffer to store the data
 * @param size Number of bytes to read
 * @param address_8bits The 8 bits address of the device
 * @param func Pointer to a function to call at the end of operation
 * @param arg Parameter to pass to the callback function
 * @return Status_t
 */
Status_t DrvI2C::readAsync(uint8_t *buffer, uint32_t size, uint8_t address_8bits, InOutStreamCallback_t func, void *arg)
{
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);

  if(m_handle == nullptr || buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  if(m_sync.run) { return STATUS_DRV_ERR_BUSY;}
  if(!locker1.try_lock()) { return STATUS_DRV_ERR_BUSY;}

  m_is_reading = true;
  m_bytes_read = 0;
  m_is_read_done = false;
  m_is_operation_done = false;
  m_sync.run = true;
  m_sync.buffer = buffer;
  m_sync.size = size;
  m_sync.key = address_8bits;
  m_sync.func = func;
  m_sync.arg = arg;
  locker1.unlock();
  m_sync.condition.notify_one();

  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Asynchronous read operation status
 * @return true if operation done, false otherwise
 */
bool DrvI2C::isReadAsyncDone()
{
  bool status;
  std::unique_lock<std::mutex> locker1(m_sync.mutex, std::defer_lock);
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
Status_t DrvI2C::writeAsync(uint8_t *buffer, uint32_t size, uint8_t address_8bits, InOutStreamCallback_t func, void *arg)
{
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);

  if(m_handle == nullptr || buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  if(m_sync.run) { return STATUS_DRV_ERR_BUSY;}
  if(!locker1.try_lock()) { return STATUS_DRV_ERR_BUSY;}

  m_is_writing = true;
  m_is_write_done = false;
  m_is_operation_done = false;
  m_sync.run = true;
  m_sync.buffer = buffer;
  m_sync.size = size;
  m_sync.key = address_8bits;
  m_sync.func = func;
  m_sync.arg = arg;
  locker1.unlock();
  m_sync.condition.notify_one();

  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Asynchronous write operation status
 * @return true if operation done, false otherwise
 */
bool DrvI2C::isWriteAsyncDone()
{
  bool status;
  std::unique_lock<std::mutex> locker1(m_sync.mutex,  std::defer_lock);
  if(!locker1.try_lock()) { return false;}
  status = m_is_write_done;
  locker1.unlock();
  return status;
}

/**
 * @brief Read data synchronously
 *
 * @param buffer Buffer to store the data
 * @param size Number of bytes to read
 * @param address_8bits The 8 bits address of the device
 * @return Status_t
 */
Status_t DrvI2C::i2cRead(uint8_t *buffer, uint32_t size, uint8_t address_8bits)
{
  Status_t status = STATUS_DRV_SUCCESS;
  int byte_count;

  // TODO: read errno for all syscall failures
  m_bytes_read = 0;
  if ((m_linux_handle = open((char *)m_handle, O_RDWR)) >= 0)
  {
    if (ioctl(m_linux_handle, I2C_PERIPHERAL_7BITS_ADDRESS, address_8bits >> 1) >= 0)
    {

      byte_count = readSyscall(m_linux_handle, (char *)buffer, size);
      m_bytes_read = byte_count > 0 ? byte_count : 0;
      if (byte_count != size)
      {
        SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"The number of bytes transmitted through i2c is smaller than the requested.");
      }
    }
    else
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"It was not possible to set the desired peripheral address.");
    }
    (void)close(m_linux_handle);
  }
  else
  {
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to open the file.");
  }
  return status;
}

/**
 * @brief Write data synchronously
 * @param buffer Buffer where data is stored
 * @param size Number of bytes to write
 * @param address_8bits The 8 bits address of the device
 * @return Status_t
 */
Status_t DrvI2C::i2cWrite(uint8_t *buffer, uint32_t size, uint8_t address_8bits)
{
  Status_t status = STATUS_DRV_SUCCESS;
  int byte_count;

  // TODO: read errno for all syscall failures
  if ((m_linux_handle = open((char *)m_handle, O_RDWR)) >= 0)
  {
    if (ioctl(m_linux_handle, I2C_PERIPHERAL_7BITS_ADDRESS, address_8bits >> 1) >= 0)
    {

      byte_count = writeSyscall(m_linux_handle, (char *)buffer, size);
      if (byte_count != size)
      {
        SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"The number of bytes received through i2c is smaller than the requested.");
      }

    }else
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"It was not possible to set the desired peripheral address.");
    }
    close(m_linux_handle);
  }else
  {
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to open the file.");
  }
  return status;
}

/**
 * @brief Thread to read or write data asynchronously
 *
 */
void DrvI2C::asyncThread(void)
{
  Status_t status;
  uint32_t byte_count;
  std::unique_lock<std::mutex> locker1(m_sync.mutex);

  while(true)
  {
    m_sync.condition.wait(locker1, [this]{ return this->m_sync.run; });
    if(m_terminate) { break;}

    if (m_is_reading)
    {
      status = i2cRead(m_sync.buffer, m_sync.size, m_sync.key);
      if (m_sync.func != nullptr)
      {
        m_sync.func(status, m_sync.buffer, m_bytes_read, m_sync.arg);
      }
      else
      {
        readAsyncDoneCallback(status, m_sync.buffer, m_bytes_read);
      }
      m_is_read_done = true;
      m_is_operation_done = true;
      m_is_reading = false;
      m_sync.run = false;
    }

    if(m_is_writing)
    {
      status = i2cWrite(m_sync.buffer, m_sync.size, m_sync.key);
      if (m_sync.func != nullptr)
      {
        m_sync.func(status, m_sync.buffer, m_sync.size, m_sync.arg);
      }
      else
      {
        writeAsyncDoneCallback(status, m_sync.buffer, m_sync.size);
      }
      m_is_write_done = true;
      m_is_operation_done = true;
      m_is_writing = false;
      m_sync.run = false;
    }
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
