/**
 * @file idf_freertos_queue.hpp
 * @author your name (you@domain.com)
 * @brief Implementation of thead-safe queue using IDF FREERTOS
 * @version 0.1
 * @date 2025-04-05
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TASK_SYSTEM_IDF_FREERTOS_QUEUE_HPP
#define TASK_SYSTEM_IDF_FREERTOS_QUEUE_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "task_interface/queue_interface.hpp"

/**
 * @brief Thread-safe queue
 *
 * @tparam T Type of the items held by the queue
 * @tparam SIZE Maximum number of items the queue can hold
 */
template<typename T, uint32_t SIZE>
class Queue : public QueueInterface<Queue<T, SIZE>, T>
{
public:
  Queue();
  ~Queue();

  // Enqueue data, timeout in milliseconds
  bool put(const T &item, uint32_t timeout = UINT32_MAX);

  // Dequeue data, timeout in milliseconds
  bool get(T &item, uint32_t timeout = UINT32_MAX);

  // Get the maximum number of elements the queue can store simultaneously
  uint32_t getMaxSize();

  // Get the number of elements in the queue
  uint32_t getActualSize();

private:
  QueueHandle_t m_queue;
};

#include "idf_freertos_queue.tpp"

#endif /* TASK_SYSTEM_IDF_FREERTOS_QUEUE_HPP */
