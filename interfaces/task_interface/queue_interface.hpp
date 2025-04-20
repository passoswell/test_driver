/**
 * @file queue_interface.hpp
 * @author your name (you@domain.com)
 * @brief Interface layer for queue creation
 * @version 0.1
 * @date 2025-04-01
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TASK_INTERFACE_QUEUE_INTERFACE_HPP
#define TASK_INTERFACE_QUEUE_INTERFACE_HPP

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Interface class for queues
 *
 * @tparam Derived The class that is inheriting from QueueInterface
 * @tparam T Type name for th items in the queue
 */
template <typename Derived, typename T>
class QueueInterface
{
public:

  // Enqueue data, timeout in milliseconds
  bool put(const T &item, uint32_t timeout = UINT32_MAX)
  {
    return static_cast<Derived*>(this)->put(item, timeout);
  }

  // Dequeue data, timeout in milliseconds
  bool get(T &item, uint32_t timeout = UINT32_MAX)
  {
    return static_cast<Derived*>(this)->get(item, timeout);
  }

  // Get the maximum number of elements the queue can store simultaneously
  uint32_t getMaxSize()
  {
    return static_cast<Derived*>(this)->getMaxSize();
  }

  // Get the number of elements in the queue
  uint32_t getActualSize()
  {
    return static_cast<Derived*>(this)->getActualSize();
  }

  QueueInterface() = default;

  // Virtual destructor
  virtual ~QueueInterface() = default;

  // Prevent copying
  QueueInterface(const QueueInterface&) = delete;
  QueueInterface& operator=(const QueueInterface&) = delete;

};


#endif /* TASK_INTERFACE_QUEUE_INTERFACE_HPP */
