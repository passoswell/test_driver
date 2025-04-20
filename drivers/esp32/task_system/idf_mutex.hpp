/**
 * @file idf_mutex.hpp
 * @author your name (you@domain.com)
 * @brief Mutex implementation using IDF FREERTOS
 * @version 0.1
 * @date 2025-04-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TASK_SYSTEM_IDF_MUTEX_HPP
#define TASK_SYSTEM_IDF_MUTEX_HPP

#include <stdint.h>

#include "task_interface/mutex_interface.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/**
 * @brief Mutex implementation
 */
class Mutex : public MutexInterface
{
public:
  Mutex()
  {
    m_mutex = xSemaphoreCreateMutex();
  }

  ~Mutex() override
  {
    if (m_mutex != nullptr)
    {
      vSemaphoreDelete(m_mutex);
    }
  }

  // Get the ownership of a mutex or timeout
  bool lock(uint32_t timeout_ms = UINT32_MAX) override
  {
    if (m_mutex == nullptr)
    {
      return false;
    }
    return xSemaphoreTake(m_mutex, timeout_ms/portTICK_PERIOD_MS) == pdTRUE;
  }

  // Release the ownership of a mutex
  bool unlock() override
  {
    if (m_mutex == nullptr)
    {
      return false;
    }
    return xSemaphoreGive(m_mutex) == pdTRUE;
  }

private:
  SemaphoreHandle_t m_mutex = nullptr;
};

#endif /* TASK_SYSTEM_IDF_MUTEX_HPP */
