/**
 * @file linux_io.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "linux/utils/linux_io.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <thread>
#include <chrono>
#include <poll.h>
#include <sys/types.h>
#include <string.h>



/**
 * @brief Use the system call to read from a file
 *
 * @param fd File descriptor
 * @param buffer Buffer to store the data
 * @param cnt Number of bytes to read
 * @return int Number of bytes actually read
 */
int readSyscall(int fd, uint8_t *buffer, size_t cnt)
{
  if(cnt == 0 || buffer == nullptr) { return 0;}
  return read(fd, buffer, cnt);
}

/**
 * @brief Use system calls to read from a file or wait on timeout
 *
 * @param fd File descriptor
 * @param buffer Buffer to store the data
 * @param cnt Number of bytes to read
 * @param timeout_ms Max. time to wait for data
 * @return int Number of bytes actually read
 */
int readOnTimeoutSyscall(int fd, uint8_t *buffer, size_t cnt, uint32_t timeout_ms)
{
  struct pollfd fds[1];
  if(cnt == 0 || buffer == nullptr) { return 0;}
  int byte_count = 0, bytes_read = 0, ready;

  // Set up the pollfd structure for UART to check for data to read
  fds[0].fd = fd;
  fds[0].events = POLLIN;

  // Poll for data
  auto start = std::chrono::steady_clock::now();
  auto end = start;
  auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
  do
  {

    if(bytes_read == 0)
    {
      ready = poll(fds, 1, UINT32_MAX);
    }else
    {
      ready = poll(fds, 1, timeout_ms - elapsed_time);
    }

    if (ready < 0)
    {
      bytes_read = -1;
      break;
    } else if (ready == 0)
    {
      break;
    }else
    {

      byte_count = bytesAvailableSyscall(fd);
      if (byte_count < 0)
      {
        bytes_read = -1;
        break;
      }else if(byte_count > 0)
      {
        start = std::chrono::steady_clock::now();
        if(byte_count > (cnt - bytes_read))
        {
          byte_count = cnt - bytes_read;
        }
        byte_count = readSyscall(fd, buffer + bytes_read, byte_count);
        if (byte_count < 0)
        {
          bytes_read = -1;
          break;
        }
        bytes_read += byte_count;

      }
    }
    end = std::chrono::steady_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
  } while((bytes_read < cnt) && (elapsed_time < timeout_ms));

  return bytes_read;
}

/**
 * @brief Use system calls to read from a file or wait on timeout
 *
 * @param fd File descriptor
 * @param buffer Buffer to store the data
 * @param cnt Number of bytes to read
 * @param timeout_ms Max. time to wait for data
 * @return int Number of bytes actually read
 */
int readOnTimeoutSyscall2(int fd, uint8_t *buffer, size_t cnt, uint32_t timeout_ms)
{
  if(cnt == 0 || buffer == nullptr) { return 0;}
  int byte_count = 0, bytes_read = 0, ready;

  byte_count = waitOnReceptionTimeoutSyscall(fd, cnt, timeout_ms);
  if (byte_count < 0)
  {
    bytes_read = -1;
  }
  else if (byte_count > 0)
  {
    byte_count = readSyscall(fd, buffer, byte_count);
    if (byte_count < 0)
    {
      bytes_read = -1;
    }else
    {
      bytes_read = byte_count;
    }
  }

  return bytes_read;
}

/**
 * @brief Use system calls to read from a file or wait on timeout
 *
 * @param fd File descriptor
 * @param buffer Buffer to store the data
 * @param cnt Number of bytes to read
 * @param timeout_ms Max. time to wait for data
 * @return int Number of bytes actually read
 */
int readOnTimeoutSyscall3(int fd, uint8_t *buffer, size_t cnt, uint32_t timeout_ms)
{
  struct pollfd fds[1];
  if(cnt == 0 || buffer == nullptr) { return 0;}
  int byte_count = 0, bytes_read = 0, ready;

  // Set up the pollfd structure for UART to check for data to read
  fds[0].fd = fd;
  fds[0].events = POLLIN;

  // Poll for data
  auto start = std::chrono::steady_clock::now();
  auto end = start;
  auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
  do
  {
    ready = poll(fds, 1, timeout_ms - elapsed_time);
    if (ready < 0)
    {
      bytes_read = -1;
      break;
    } else if (ready == 0)
    {
      break;
    }else
    {
      byte_count = bytesAvailableSyscall(fd);
      if (byte_count < 0)
      {
        bytes_read = -1;
        break;
      }else if(byte_count > 0)
      {
        start = std::chrono::steady_clock::now();
        if(byte_count > (cnt - bytes_read))
        {
          byte_count = cnt - bytes_read;
        }
        byte_count = readSyscall(fd, buffer + bytes_read, byte_count);
        if (byte_count < 0)
        {
          bytes_read = -1;
          break;
        }
        bytes_read += byte_count;
      }
    }
    end = std::chrono::steady_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
  } while(elapsed_time < timeout_ms);

  return bytes_read;
}

/**
 * @brief Use the system call to write to a file
 *
 * @param fd File descriptor
 * @param buffer Buffer where data is stored
 * @param cnt Number of bytes to write
 * @return int Number of bytes actually written
 */
int writeSyscall(int fd, const uint8_t *buffer, size_t cnt)
{
  if(cnt == 0 || buffer == nullptr) { return 0;}
  return write(fd, buffer, cnt);
}

/**
 * @brief Return the number of bytes available on the reception bufferfer
 *
 * @param fd File descriptor
 * @return int
 */
int bytesAvailableSyscall(int fd)
{
  int byte_count;
  ioctl(fd, FIONREAD, &byte_count);
  return byte_count;
}

/**
 * @brief Wait for the reception of a number of bytes until it timeout
 *
 * @param fd File descriptor
 * @param size Number of bytes to wait for
 * @param wait_time Time to wait for in milliseconds
 * @return int
 */
int waitOnReceptionTimeoutSyscall(int fd, uint32_t size, uint32_t wait_time)
{
  uint32_t timeout_counter;
  uint32_t smaller_wait_time = 1;
  int byte_count;

  byte_count = bytesAvailableSyscall(fd);
  if(byte_count >= size) { return byte_count;}

  timeout_counter = wait_time;
  if(wait_time > 128)
  {
    smaller_wait_time = 128;
  }
  do
  {
    if((timeout_counter/smaller_wait_time) == 0)
    {
      smaller_wait_time = timeout_counter;
    }
    timeout_counter -= smaller_wait_time;
    std::this_thread::sleep_for(std::chrono::milliseconds(smaller_wait_time));
    byte_count = bytesAvailableSyscall(fd);
  } while (byte_count >= 0 && byte_count < size && timeout_counter > 0);
  return byte_count;
}

/**
 * @brief Verify if the inputs are in ther expected range
 *
 * @param buffer Data buffer
 * @param size Number of bytes in the data buffer
 * @param timeout Operation timeout value
 * @param handle A pointer that should not be nullptr
 * @param fd A linux's file descriptor, should not be negative
 * @return Status_t
 */
Status_t checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout, const void *handle, int fd)
{
  if(buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(handle == nullptr || fd < 0) { return STATUS_DRV_BAD_HANDLE;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Convert errno code into Status_t code
 *
 * @param code errno code
 * @return Status_t
 */
Status_t convertErrnoCode(int code)
{
  Status_t status;

  // TODO: Implement a thorough translation between error codes
  switch(code)
  {
    case 0:
      status = STATUS_DRV_SUCCESS;
      break;
    default:
      status.code = ERR_UNKNOWN_ERROR;
      status.description = strerror(code);
      status.source = SRC_HAL;
      status.success = false;
      break;
  }
  return status;
}