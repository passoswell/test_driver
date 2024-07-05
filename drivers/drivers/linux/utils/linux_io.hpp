/**
 * @file linux_io.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-04
 *
 * @copyright Copyright (c) 2024
 *
 */


#ifndef LINUX_IO_HPP
#define LINUX_IO_HPP

#include <stdlib.h>

int readSyscall(int fd, void* buf, size_t cnt);

int writeSyscall(int fd, void* buf, size_t cnt);


#endif /* LINUX_IO_HPP */