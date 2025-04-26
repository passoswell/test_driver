/**
 * @file idf_freertos_worker_task.hpp
 * @author your name (you@domain.com)
 * @brief Implementation of worker task using IDF FREERTOS
 * @version 0.1
 * @date 2025-04-06
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TASK_SYSTEM_IDF_FREERTOS_WORKER_TASK_HPP
#define TASK_SYSTEM_IDF_FREERTOS_WORKER_TASK_HPP


#include "task_interface/task_interface.hpp"
#include "idf_freertos_queue.hpp"


/*
  * Comment found on the implementation of xTaskCreate:
  * The idf_additions.h has not been included here yet due to inclusion
  * order. Thus we manually declare the function here.
  */
extern BaseType_t xTaskCreatePinnedToCore( TaskFunction_t pxTaskCode,
  const char * const pcName,
  const configSTACK_DEPTH_TYPE usStackDepth,
  void * const pvParameters,
  UBaseType_t uxPriority,
  TaskHandle_t * const pvCreatedTask,
  const BaseType_t xCoreID );


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
  Task();
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
  TaskHandle_t m_task_handle, m_caller_task_handle;
  bool m_terminate;
  bool m_terminate_confirmed;
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

#include "idf_freertos_worker_task.tpp"


#endif /* TASK_SYSTEM_IDF_FREERTOS_WORKER_TASK_HPP */
