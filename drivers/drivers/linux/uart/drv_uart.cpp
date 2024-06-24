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

#include "drivers/linux/uart/drv_uart.hpp"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>


int readSerial(int fd, void* buf, size_t cnt);

int writeSerial(int fd, void* buf, size_t cnt);

/**
 * @brief Constructor
 * @param port_handle A string containing the path to the peripheral
 */
DrvUART::DrvUART(void *port_handle)
{
  m_handle = port_handle;
  m_linux_handle = -1;
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
    termios_structure.c_cc[VTIME] = 0;
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
  int byte_count;
  if(m_handle == nullptr || m_linux_handle < 0)
  {
    return STATUS_DRV_NULL_POINTER;
  }

  byte_count = readSerial(m_linux_handle, buffer, size);
  if(byte_count < 0)
  {
    return STATUS_DRV_UNKNOWN_ERROR;
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
  int byte_count;
  if(m_handle == nullptr || m_linux_handle < 0)
  {
    return STATUS_DRV_NULL_POINTER;
  }

  byte_count = writeSerial(m_linux_handle, buffer, size);
  if(byte_count < 0)
  {
    return STATUS_DRV_UNKNOWN_ERROR;
  }
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