/**
 * @file stl_worker_task.hpp
 * @author your name (you@domain.com)
 * @brief Implementation of worker task using STL C++
 * @version 0.1
 * @date 2025-04-06
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TASK_SYSTEM_STL_WORKER_TASK_HPP
#define TASK_SYSTEM_STL_WORKER_TASK_HPP

#include "task_interface/task_interface.hpp"
#include "stl_queue.hpp"

/**
 * @brief A wrapper method for creating tasks
 *
 * @tparam TI Type of the worker function's input item
 * @tparam INPUT_SIZE Maximum number of items in the input queue
 * @tparam TO Type of the worker function's output item
 * @tparam OUTPUT_SIZE Maximum number of items in the output queue
 */
template <typename TI, uint32_t INPUT_SIZE, typename TO, uint32_t OUTPUT_SIZE>
class Task : public TaskBase<Task<TI, INPUT_SIZE, TO, OUTPUT_SIZE>, TI, INPUT_SIZE, TO, OUTPUT_SIZE>
{
public:
  Task() = default;
  ~Task();

  using ThreadFunction_t = std::function<TO(TI &data, void *user_arg)>;

  // Create a task
  bool create(ThreadFunction_t function, void *user_arg, uint32_t priority);

  // Create a task
  bool create(ThreadFunction_t function, void *user_arg, TaskProfile_t parameters);

  // Suspend or resume a task
  bool setState(TaskState_t state);

  // Get the state of the task
  TaskState_t getState(void);

  // Terminate a task
  bool terminate();

  // Return the number of cores
  uint8_t getCoreCount(void);

  // Return the core whe the task is running
  uint8_t getPinnedCore(void);

  // Send data to a task, timeout in milliseconds
  bool setInputData(const TI &data, uint32_t timeout = UINT32_MAX);

  // Get data from a task, timeout in milliseconds
  bool getOutputData(TO &data, uint32_t timeout = UINT32_MAX);

private:
  std::thread *m_thread_handle = nullptr;
  bool m_terminate;
  ThreadFunction_t m_worker_function;
  void *m_user_arg;
  Queue<TI, INPUT_SIZE> m_input_queue;
  Queue<TO, OUTPUT_SIZE> m_output_queue;
  TaskState_t m_state;
  TaskProfile_t m_profile;
  uint8_t m_pinned_core;

  // Run the worker function
  void run();
};

#include "stl_worker_task.tpp"

#endif /* TASK_SYSTEM_STL_WORKER_TASK_HPP */
