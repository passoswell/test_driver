/**
 * @file linux_threads.hpp
 * @author your name (you@domain.com)
 * @brief Class to manage creation and deletion of threads on linux
 * @version 0.1
 * @date 2024-11-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef LINUX_THREADS_HPP
#define LINUX_THREADS_HPP

#include <stdint.h>
#include <stdbool.h>
#include <thread>
#include <chrono>
#include <functional>

#include "linux_types.hpp"
#include "linux_queue.hpp"
#include "task_interface/task_interface.hpp"

/**
 * @brief Task implementation for linux systems
 *
 * @tparam INPUT_DATA Data type of the input
 * @tparam OUTPUT_DATA Data type of the output
 * @tparam MAX_QUEUE_SIZE Maximum number of bytes in the queue
 */
template <typename INPUT_DATA, typename OUTPUT_DATA, uint32_t MAX_IN_QUEUE_SIZE, uint32_t MAX_OUT_QUEUE_SIZE = MAX_IN_QUEUE_SIZE>
class LinuxThreads : public TaskInterface
{
public:
  using ThreadFunction_t = std::function<OUTPUT_DATA(INPUT_DATA data, void *user_arg)>;
  LinuxThreads(ThreadFunction_t function, void *user_arg);

  ~LinuxThreads();

  bool create();

  bool terminate();

  void join();

  bool setInputData(const void *data, uint32_t timeout = UINT32_MAX);

  bool getOutputData(void *data, uint32_t timeout = UINT32_MAX);

private:
  std::thread *m_thread_handle;
  bool m_terminate;
  ThreadFunction_t m_function;
  void *m_user_arg;
  LinuxQueue<INPUT_DATA, MAX_IN_QUEUE_SIZE> m_input_queue;
  LinuxQueue<OUTPUT_DATA, MAX_OUT_QUEUE_SIZE> m_output_queue;

  void run();
};

#include "linux_threads.tpp"

#endif /* LINUX_THREADS_HPP */