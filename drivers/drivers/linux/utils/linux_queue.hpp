/**
 * @file linux_queue.hpp
 * @author your name (you@domain.com)
 * @brief Thread safe queue implementation for linux
 * @version 0.1
 * @date 2024-11-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef LINUX_QUEUE_HPP
#define LINUX_QUEUE_HPP

#include <stdint.h>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

/**
 * @brief Thread safe queue implementation for linux
 *
 * @tparam T The data type of elements stored in the queue
 * @tparam MAX_SIZE The maximum number of elements the queue can store simultaneously
 */
template <typename T, uint32_t MAX_SIZE>
class LinuxQueue
{
public:
  LinuxQueue() {;}

  // Enqueue data, timeout in milliseconds
  bool put(const T &item, uint32_t timeout = UINT32_MAX)
  {
    std::unique_lock<std::mutex> lock(mutex);
    std::cv_status status = std::cv_status::no_timeout;

    if(queue.size() >= max_size)
    {
      status = not_full.wait_for(lock, std::chrono::milliseconds(timeout)); // Wait until queue is not full or timeout
    }

    if(status == std::cv_status::no_timeout)
    {
      queue.push(item);
      not_empty.notify_one(); // Notify that the queue is not empty now
      return true;
    }
    return false;
  }

  // Dequeue data, timeout in milliseconds
  bool get(T &item, uint32_t timeout = UINT32_MAX)
  {
    std::unique_lock<std::mutex> lock(mutex);
    std::cv_status status = std::cv_status::no_timeout;

    if(queue.empty())
    {
      status = not_empty.wait_for(lock, std::chrono::milliseconds(timeout)); // Wait until queue is not empty or timeout
    }

    if(status == std::cv_status::no_timeout)
    {
      item = queue.front();
      queue.pop();
      not_full.notify_one(); // Notify that the queue is not full now
      return true;
    }
    return false;
  }

  // Get the maximum number of elements the queue can store simultaneously
  uint32_t getMaxSize()
  {
    return max_size;
  }

  // Get the number of elements in the queue
  uint32_t getActualSize()
  {
    return queue.size();
  }

private:
  std::queue<T> queue;
  std::mutex mutex;
  std::condition_variable not_empty;
  std::condition_variable not_full;
  size_t max_size = MAX_SIZE;
};

#endif /* LINUX_QUEUE_HPP */