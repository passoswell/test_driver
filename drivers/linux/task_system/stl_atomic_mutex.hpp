/**
 * @file stl_atomic_mutex.hpp
 * @author your name (you@domain.com)
 * @brief Implementation of a mutex using a atomic flag
 * @version 0.1
 * @date 2025-04-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TASK_SYSTEM_STL_ATOMIC_MUTEX_HPP
#define TASK_SYSTEM_STL_ATOMIC_MUTEX_HPP

#include <mutex>
#include <chrono>
#include <atomic>
#include <thread>
#include <stdint.h>

#include "task_interface/mutex_interface.hpp"

/**
 * @brief A mutex based on an atomic variable
 */
class AtomicMutex : public MutexInterface
{
public:
  AtomicMutex()
  {
    m_flag.clear();
  }
  ~AtomicMutex() = default;

  // Get the ownership of a mutex or timeout
  bool lock(uint32_t timeout_ms = UINT32_MAX) override
  {
    auto start = std::chrono::steady_clock::now();
    while (true)
    {
      if (!m_flag.test_and_set(std::memory_order_acquire))
      {
        return true; // Lock acquired
      }

      auto now = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
      if (elapsed.count() >= timeout_ms)
      {
        return false; // Timed out
      }

      std::this_thread::yield(); // Yield to reduce CPU use during spinning
    }
  }

  // Release the ownership of a mutex
  bool unlock() override
  {
    m_flag.clear(std::memory_order_release);
    return true;
  }

private:
  std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};

#endif /* TASK_SYSTEM_STL_ATOMIC_MUTEX_HPP */
