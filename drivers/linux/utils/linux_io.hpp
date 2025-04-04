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


#ifndef DRIVERS_LINUX_UTILS_LINUX_IO_HPP
#define DRIVERS_LINUX_UTILS_LINUX_IO_HPP

#include <stdlib.h>
#include <stdint.h>

#include "com_status.hpp"

int readSyscall(int fd, uint8_t *buffer, size_t cnt);

int readOnTimeoutSyscall(int fd, uint8_t *buffer, size_t cnt, uint32_t timeout_ms);

int readOnTimeoutSyscall2(int fd, uint8_t *buffer, size_t cnt, uint32_t timeout_ms);

int readOnTimeoutSyscall3(int fd, uint8_t *buffer, size_t cnt, uint32_t timeout_ms);

int writeSyscall(int fd, const uint8_t *buffer, size_t cnt);

int bytesAvailableSyscall(int fd);

int waitOnReceptionTimeoutSyscall(int fd, uint32_t size, uint32_t wait_time);

Status_t checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout, const void *handle, int fd);

Status_t convertErrnoCode(int code);


#endif /* DRIVERS_LINUX_UTILS_LINUX_IO_HPP */