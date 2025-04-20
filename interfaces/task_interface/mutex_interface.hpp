/**
 * @file mutex_interface.hpp
 * @author your name (you@domain.com)
 * @brief Interface layer for mutex creation
 * @version 0.1
 * @date 2025-04-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TASK_INTERFACE_MUTEX_INTERFACE_HPP
#define TASK_INTERFACE_MUTEX_INTERFACE_HPP

#include <stdint.h>

/**
 * @brief Interface class for mutexes
 *
 */
class MutexInterface
{
public:
  virtual ~MutexInterface() = default;

  // Lock the mutex, waits up to timeout_ms milliseconds
  virtual bool lock(uint32_t timeout_ms = UINT32_MAX) = 0;

  // Unlock the mutex
  virtual bool unlock() = 0;
};

#endif /* TASK_INTERFACE_MUTEX_INTERFACE_HPP */
