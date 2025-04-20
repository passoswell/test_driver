/**
 * @file stl_queue.tpp
 * @author your name (you@domain.com)
 * @brief Implementation of thead-safe queue using STL C++
 * @version 0.1
 * @date 2025-04-05
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "stl_queue.hpp"

/**
 * @brief Enqueue data, timeout in milliseconds
 *
 * @tparam T Type of the items held by the queue
 * @tparam SIZE Maximum number of items the queue can hold
 * @param item An item to store
 * @param timeout Maximum time in milliseconds to wait if the queue is full
 * @return true if successful
 * @return false if failed
 */
template <typename T, uint32_t SIZE>
bool Queue<T, SIZE>::put(const T &item, uint32_t timeout)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  std::cv_status status = std::cv_status::no_timeout;

  if (SIZE == 0)
  {
    return false;
  }

  // Using a loop to avoid spurious wake-ups
  auto deadline = std::chrono::steady_clock::now();
  deadline += std::chrono::milliseconds(timeout);
  while (m_count >= SIZE)
  {
    if (timeout == 0 || status == std::cv_status::timeout)
    {
      return false;
    }
    status = m_not_full.wait_until(lock, deadline);
  }

  m_data[m_rear_index] = item;
  m_rear_index = (m_rear_index + 1) % SIZE;
  m_count++;
  m_not_empty.notify_one(); // Notify that the queue is not empty now
  return true;
}

/**
 * @brief Dequeue data, timeout in milliseconds
 *
 * @tparam T Type of the items held by the queue
 * @tparam SIZE Maximum number of items the queue can hold
 * @param item The recovered item
 * @param timeout Maximum time in milliseconds to wait if the queue is empty
 * @return true if successful
 * @return false if failed
 */
template <typename T, uint32_t SIZE>
bool Queue<T, SIZE>::get(T &item, uint32_t timeout)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  std::cv_status status = std::cv_status::no_timeout;

  if (SIZE == 0)
  {
    return false;
  }

  // Using a loop to avoid spurious wake-ups
  auto deadline = std::chrono::steady_clock::now();
  deadline += std::chrono::milliseconds(timeout);
  while (m_count == 0)
  {
    if (timeout == 0 || status == std::cv_status::timeout)
    {
      return false;
    }
    status = m_not_empty.wait_until(lock, deadline);
  }

  if (m_count == 0)
  {
    // Wait until queue is not empty or timeout
    status = m_not_empty.wait_for(lock, std::chrono::milliseconds(timeout));
  }

  item = m_data[m_front_index];
  m_front_index = (m_front_index + 1) % SIZE;
  m_count--;
  m_not_full.notify_one(); // Notify that the queue is not full now
  return true;
}

/**
 * @brief Get the maximum number of items the queue can store simultaneously
 *
 * @tparam T Type of the items held by the queue
 * @tparam SIZE Maximum number of items the queue can hold
 * @return uint32_t
 */
template <typename T, uint32_t SIZE>
uint32_t Queue<T, SIZE>::getMaxSize()
{
  return SIZE;
}

/**
 * @brief Get the number of items in the queue
 *
 * @tparam T Type of the items held by the queue
 * @tparam SIZE Maximum number of items the queue can hold
 * @return uint32_t
 */
template <typename T, uint32_t SIZE>
uint32_t Queue<T, SIZE>::getActualSize()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  return m_count;
}