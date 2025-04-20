/**
 * @file stl_mutex.hpp
 * @author your name (you@domain.com)
 * @brief Mutex implementation using STL C++
 * @version 0.1
 * @date 2025-04-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TASK_SYSTEM_STL_MUTEX_HPP
#define TASK_SYSTEM_STL_MUTEX_HPP

#include <mutex>
#include <chrono>
#include <stdint.h>

#include "task_interface/mutex_interface.hpp"

/**
 * @brief Mutex implementation
 */
class Mutex : public MutexInterface
{
public:
  ~Mutex() = default;

  // Get the ownership of a mutex or timeout
  bool lock(uint32_t timeout_ms = UINT32_MAX) override
  {
    return m_mutex.try_lock_for(std::chrono::milliseconds(timeout_ms));
  }

  // Release the ownership of a mutex
  bool unlock() override
  {
    m_mutex.unlock();
    return true; // std::mutex doesn't report unlock success
  }

private:
  std::timed_mutex m_mutex;
};

#endif /* TASK_SYSTEM_STL_MUTEX_HPP */
