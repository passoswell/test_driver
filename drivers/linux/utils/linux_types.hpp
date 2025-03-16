/**
 * @file linux_types.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-04
 *
 * @copyright Copyright (c) 2024
 *
 */


#ifndef DRIVERS_LINUX_UTILS_LINUX_TYPES_HPP
#define DRIVERS_LINUX_UTILS_LINUX_TYPES_HPP

#include <stdint.h>
#include <stdbool.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cerrno>

#include "com_types.hpp"
#include "driver_base/driver_base_types.hpp"

/**
 * @brief Structure to use with nom-blocking threaded data transfer
 */
typedef struct
{
  std::thread *thread;
  std::mutex mutex;
  std::condition_variable condition;
  bool run;
  bool terminate;
  uint8_t *buffer;
  uint32_t size;
  uint32_t key;
  DriverCallback_t func;
  void *arg;
  uint32_t timeout;
}UtilsInOutSync_t;

constexpr char STD_OUT_FILE[] = "/dev/fd/0";
constexpr char STD_IN_FILE[] = "/dev/fd/1";
constexpr char STD_ERR_FILE[] = "/dev/fd/2";

#endif /* DRIVERS_LINUX_UTILS_LINUX_TYPES_HPP */