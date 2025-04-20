/**
 * @file idf_freertos_queue.tpp
 * @author your name (you@domain.com)
 * @brief Implementation of thead-safe queue using IDF FREERTOS
 * @version 0.1
 * @date 2025-04-06
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "idf_freertos_queue.hpp"

/**
 * @brief Constructor
 *
 * @tparam T Type of the items held by the queue
 * @tparam SIZE Maximum number of items the queue can hold
 */
template <typename T, uint32_t SIZE>
Queue<T, SIZE>::Queue()
{
  m_queue = xQueueCreate(SIZE, sizeof(T));
}

/**
 * @brief Destroyer
 *
 * @tparam T Type of the items held by the queue
 * @tparam SIZE Maximum number of items the queue can hold
 */
template <typename T, uint32_t SIZE>
Queue<T, SIZE>::~Queue()
{
  if (m_queue != nullptr)
  {
    vQueueDelete(m_queue);
  }
}

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
  if(m_queue == nullptr)
  {
    return false;
  }
  return xQueueSendToBack(m_queue, &item, timeout/portTICK_PERIOD_MS) == pdTRUE;
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
  if(m_queue == nullptr)
  {
    return false;
  }
  return xQueueReceive(m_queue, &item, timeout/portTICK_PERIOD_MS) == pdTRUE;
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
  if(m_queue == nullptr)
  {
    return 0;
  }
  return uxQueueMessagesWaiting(m_queue);
}