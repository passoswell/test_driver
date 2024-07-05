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



/**
 * @brief Use the system call to read from a file
 *
 * @param fd File pointer
 * @param buf Buffer to store the data
 * @param cnt Number of bytes to read
 * @return int Number of bytes actually read
 */
int readSyscall(int fd, void* buf, size_t cnt)
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
int writeSyscall(int fd, void* buf, size_t cnt)
{
  return write(fd, buf, cnt);
}