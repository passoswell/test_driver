/**
 * @file idf_critical_mutex.hpp
 * @author your name (you@domain.com)
 * @brief Critical section mutex implementation using IDF FREERTOS
 * @version 0.1
 * @date 2025-04-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TASK_SYSTEM_IDF_CRITICAL_MUTEX_HPP
#define TASK_SYSTEM_IDF_CRITICAL_MUTEX_HPP

#include <stdint.h>

#include "task_interface/mutex_interface.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"

/**
 * @brief A mutex based on disabling interruptions for critical sections
 */
class CriticalMutex : public MutexInterface
{
public:
  CriticalMutex()
  {
    portMUX_INITIALIZE(&m_spinlock);
  }
  ~CriticalMutex() = default;

  // Get the ownership of a mutex or timeout
  bool lock(uint32_t timeout_ms = UINT32_MAX) override
  {
    // portENTER_CRITICAL is non-blocking on single core
    // On dual core, it disables interrupts on that core and acquires the spinlock
    portENTER_CRITICAL(&m_spinlock);
    return true;
  }

  // Release the ownership of a mutex
  bool unlock() override
  {
    portEXIT_CRITICAL(&m_spinlock);
    return true;
  }

private:
  portMUX_TYPE m_spinlock;
};

#endif /* TASK_SYSTEM_IDF_CRITICAL_MUTEX_HPP */
