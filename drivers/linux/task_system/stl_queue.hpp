/**
 * @file stl_queue.hpp
 * @author your name (you@domain.com)
 * @brief Implementation of thead-safe queue using STL C++
 * @version 0.1
 * @date 2025-04-05
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TASK_SYSTEM_STL_QUEUE_HPP
#define TASK_SYSTEM_STL_QUEUE_HPP

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

#include "task_interface/queue_interface.hpp"

/**
 * @brief Thread-safe queue using std::array, mutex and condition_variable
 *
 * @tparam T Type of the items held by the queue
 * @tparam SIZE Maximum number of items the queue can hold
 */
template <typename T, uint32_t SIZE>
class Queue : public QueueInterface<Queue<T, SIZE>, T>
{
public:
  Queue() = default;
  ~Queue() = default;

  // Enqueue data, timeout in milliseconds
  bool put(const T &item, uint32_t timeout = UINT32_MAX);

  // Dequeue data, timeout in milliseconds
  bool get(T &item, uint32_t timeout = UINT32_MAX);

  // Get the maximum number of elements the queue can store simultaneously
  uint32_t getMaxSize();

  // Get the number of elements in the queue
  uint32_t getActualSize();

private:
  std::array<T, SIZE> m_data;
  std::mutex m_mutex;
  std::condition_variable m_not_empty;
  std::condition_variable m_not_full;
  uint32_t m_front_index;
  uint32_t m_rear_index;
  uint32_t m_count;
};

#include "stl_queue.tpp"

#endif /* TASK_SYSTEM_STL_QUEUE_HPP */
