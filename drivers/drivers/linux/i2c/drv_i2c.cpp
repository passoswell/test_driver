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

static int readSerial(int fd, void* buf, size_t cnt);

static int writeSerial(int fd, void* buf, size_t cnt);


/**
 * @brief Constructor
 * @param port_handle A string containing the path to the peripheral
 */
DrvI2C::DrvI2C(void *port_handle)
{
  m_handle = port_handle;
  m_linux_handle = -1;
}

/**
 * @brief Destructor
 */
DrvI2C::~DrvI2C()
{
  if(m_linux_handle >= 0)
  {
    close(m_linux_handle);
  }
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
  int byte_count;
  Status_t status = STATUS_DRV_SUCCESS;
  (void) timeout;

  if(m_handle == nullptr || buffer == nullptr){return STATUS_DRV_NULL_POINTER;}

  status = lock(address_8bits);
  if(!status.success){return status;}

  // TODO: read errno for all syscall failures
  if ((m_linux_handle = open((char *)m_handle, O_RDWR)) >= 0)
  {
    if (ioctl(m_linux_handle, I2C_PERIPHERAL_7BITS_ADDRESS, address_8bits >> 1) >= 0)
    {

      byte_count = readSerial(m_linux_handle, (char *)buffer, size);
      if (byte_count != size)
      {
        SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"The number of bytes transmitted through i2c is smaller than the requested.");
      }

      close(m_linux_handle);
    }else
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"It was not possible to set the desired peripheral address.");
    }
    (void)unlock(address_8bits);
  }else
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
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t DrvI2C::write(uint8_t *buffer, uint32_t size, uint8_t address_8bits, uint32_t timeout)
{
  int byte_count;
  Status_t status;
  (void) timeout;

  if(m_handle == nullptr || buffer == nullptr){return STATUS_DRV_NULL_POINTER;}

  status = lock(address_8bits);
  if(!status.success){return status;}

  // TODO: read errno for all syscall failures
  if ((m_linux_handle = open((char *)m_handle, O_RDWR)) >= 0)
  {
    if (ioctl(m_linux_handle, I2C_PERIPHERAL_7BITS_ADDRESS, address_8bits >> 1) >= 0)
    {

      byte_count = writeSerial(m_linux_handle, (char *)buffer, size);
      if (byte_count != size)
      {
        m_bytes_read = byte_count;
        SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"The number of bytes received through i2c is smaller than the requested.");
      }

      close(m_linux_handle);
    }else
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"It was not possible to set the desired peripheral address.");
    }
    (void)unlock(address_8bits);
  }else
  {
    SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to open the file.");
  }
  return status;

}

/**
 * @brief Use the system call to read from a file
 *
 * @param fd File pointer
 * @param buf Buffer to store the data
 * @param cnt Number of bytes to read
 * @return int Number of bytes actually read
 */
int readSerial(int fd, void* buf, size_t cnt)
{
  return read(fd, buf, cnt);
}

/**
 * @brief Use the system call to write to a file
 *
 * @param fd File pointer
 * @param buf Buffer where data is stored
 * @param cnt Number of bytes to write
 * @return int Number of bytes actually written
 */
int writeSerial(int fd, void* buf, size_t cnt)
{
  return write(fd, buf, cnt);
}

#endif