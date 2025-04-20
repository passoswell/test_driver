/**
 * @file stl_critical_mutex.hpp
 * @author your name (you@domain.com)
 * @brief Critical section mutex implementation using STL C++
 * @version 0.1
 * @date 2025-04-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TASK_SYSTEM_STL_CRITICAL_MUTEX_HPP
#define TASK_SYSTEM_STL_CRITICAL_MUTEX_HPP

#include "stl_atomic_mutex.hpp"

/**
 * @brief A mutex based on disabling interruptions for critical sections
 */
class CriticalMutex : public MutexInterface
{
public:
  ~CriticalMutex() = default;

  // Get the ownership of a mutex or timeout
  bool lock(uint32_t timeout_ms = UINT32_MAX) override
  {
    return m_mutex.lock(timeout_ms);
  }

  // Release the ownership of a mutex
  bool unlock() override
  {
    return m_mutex.unlock();
  }

private:
  AtomicMutex m_mutex;
};

#endif /* TASK_SYSTEM_STL_CRITICAL_MUTEX_HPP */
