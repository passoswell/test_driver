/**
 * @file task_interface.hpp
 * @author your name (you@domain.com)
 * @brief Interface layer for worker task creation
 * @version 0.1
 * @date 2024-11-03
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef TASK_INTERFACE_HPP
#define TASK_INTERFACE_HPP

#include <functional>
#include <stdint.h>
#include <stdbool.h>

#include "queue_interface.hpp"


/**
 * @brief Definition of some task priorities
 */
enum
{
  TASK_IDLE_PRIORITY = 0,
  TASK_LOWEST_PRIORITY,
  TASK_LOW_PRIORITY,
  TASK_MEDIUM_PRIORITY,
  TASK_HIGH_PRIORITY,
  TASK_HIGHEST_PRIORITY,
};

typedef enum
{
  TASK_STATE_RUN,
  TASK_STATE_SUSPEND
}TaskState_t;

/**
 * @brief List of types of tasks
 */
typedef enum
{
  TASK_WORKER,
  TASK_PERIODIC,
}TaskType_t;

/**
 * @brief Definition of some task parameters
 */
typedef struct
{
  const char *name;    /*!< The name of the task */
  uint32_t stack_size; /*!< Number of bytes reserved for the stack */
  uint32_t priority;   /*!< Higher the number, higher the priority */
  uint32_t period;     /*!< If periodic, define the execution period in ms */
  uint8_t core_number; /*!< A suggestion about the core to run the task */
  TaskType_t type;     /*!< The task operation when running */
}TaskProfile_t;

 /**
  * @brief Interface class for worker task creation
  */
class TaskInterface
{
public:
  using ThreadFunction_t = std::function<void(void *user_arg)>;
  // // Create a task
  // virtual bool create(ThreadFunction_t function, void *user_arg, uint32_t priority) = 0;

  // // Create a task
  // virtual bool create(ThreadFunction_t function, void *user_arg, TaskProfile_t parameters) = 0;

  // Suspend or resume a task
  virtual bool setState(TaskState_t state) = 0;

  // Get the state of the task
  virtual TaskState_t getState(void) = 0;

  // Terminate a task
  virtual bool terminate() = 0;

  // Return the number of cores
  virtual uint8_t getCoreCount(void) = 0;

  // Return the core whe the task is running
  virtual uint8_t getPinnedCore(void) = 0;

  TaskInterface() = default;

  virtual ~TaskInterface() = default;

  // Prevent copying
  TaskInterface(const TaskInterface&) = delete;
  TaskInterface& operator=(const TaskInterface&) = delete;

protected:
  void run();
};

/**
 * @brief Interface class for worker task creation
 *
 * @tparam Derived The class that is inheriting from QueueInterface
 * @tparam TI Type of the input work data
 * @tparam IN_QUEUE_SIZE Size of the input queue
 * @tparam TO Type of the output processed data
 * @tparam OUT_QUEUE_SIZE Size of the output data
 */
template <typename Derived, typename TI, uint32_t IN_QUEUE_SIZE, typename TO, uint32_t OUT_QUEUE_SIZE>
class TaskBase : public TaskInterface
{
public:
  using ThreadFunction_t = std::function<TO(TI &data, void *user_arg)>;
  // Create a task
  bool create(ThreadFunction_t function, void *user_arg, uint32_t priority)
  {
    return static_cast<Derived *>(this)->create(function, user_arg, priority);
  }

  // Create a task
  bool create(ThreadFunction_t function, void *user_arg, TaskProfile_t parameters)
  {
    return static_cast<Derived*>(this)->create(function, user_arg, parameters);
  }

  // Suspend or resume a task
  bool setState(TaskState_t state)
  {
    return static_cast<Derived*>(this)->setState(state);
  }

  // Get the state of the task
  TaskState_t getState(void)
  {
    return static_cast<Derived*>(this)->getState();
  }

  // Terminate a task
  bool terminate()
  {
    return static_cast<Derived *>(this)->terminate();
  }

  // Return the number of cores
  uint8_t getCoreCount(void)
  {
    return static_cast<Derived *>(this)->getCoreCount();
  }

  // Return the core whe the task is running
  uint8_t getPinnedCore(void)
  {
    return static_cast<Derived *>(this)->getPinnedCore();
  }

  // Send data to a task, timeout in milliseconds
  bool setInputData(const TI &data, uint32_t timeout = UINT32_MAX)
  {
    return static_cast<Derived *>(this)->setInputData(data, timeout);
  }

  // Get data from a task, timeout in milliseconds
  bool getOutputData(TO &data, uint32_t timeout = UINT32_MAX)
  {
    return static_cast<Derived *>(this)->getOutputData(data, timeout);
  }

  TaskBase() = default;

  virtual ~TaskBase() = default;

  // Prevent copying
  TaskBase(const TaskBase &) = delete;
  TaskBase &operator=(const TaskBase &) = delete;

protected:
  // Run the worker function
  void run();
};

#endif /* TASK_INTERFACE_HPP */