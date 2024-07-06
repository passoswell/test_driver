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

#include "drivers/linux/utils/linux_io.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <thread>
#include <chrono>



/**
 * @brief Use the system call to read from a file
 *
 * @param fd File descriptor
 * @param buf Buffer to store the data
 * @param cnt Number of bytes to read
 * @return int Number of bytes actually read
 */
int readSyscall(int fd, void* buf, size_t cnt)
{
  if(cnt == 0) { return 0;}
  return read(fd, buf, cnt);
}

/**
 * @brief Use the system call to write to a file
 *
 * @param fd File descriptor
 * @param buf Buffer where data is stored
 * @param cnt Number of bytes to write
 * @return int Number of bytes actually written
 */
int writeSyscall(int fd, void* buf, size_t cnt)
{
  if(cnt == 0) { return 0;}
  return write(fd, buf, cnt);
}

/**
 * @brief Return the number of bytes available on the reception buffer
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
  int byte_count;

  byte_count = bytesAvailableSyscall(fd);
  if(byte_count >= size) { return byte_count;}

  timeout_counter = wait_time + 1;
  do
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    timeout_counter--;
    byte_count = bytesAvailableSyscall(fd);
  } while (byte_count >= 0 && byte_count < size && timeout_counter > 0);
  return byte_count;
}