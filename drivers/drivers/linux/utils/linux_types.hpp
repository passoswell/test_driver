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


#ifndef LINUX_UTILS_HPP
#define LINUX_UTILS_HPP

#include <stdint.h>
#include <stdbool.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cerrno>

#include "drivers/base/drv_comm_base.hpp"

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
  uint8_t key;
  InOutStreamCallback_t func;
  void *arg;
}UtilsInOutSync_t;

#endif /* LINUX_UTILS_HPP */