/**
 * @file task_interface.hpp
 * @author your name (you@domain.com)
 * @brief Abstraction layer for task creation
 * @version 0.1
 * @date 2024-11-03
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef TASK_INTERFACE_HPP
#define TASK_INTERFACE_HPP

#include <stdint.h>
#include <stdbool.h>
#include <any>

/**
 * @brief Interface class for task creation
 */
class TaskInterface
{
public:
  virtual ~TaskInterface(){;}


  // Create a task
  virtual bool create() = 0;

  // Terminate a task
  virtual bool terminate() = 0;

  // Block until a task is terminated
  virtual void join() = 0;

  // Send data to a task
  virtual bool setInputData(const void *data, uint32_t timeout = UINT32_MAX) = 0;

  // Get data from a task
  virtual bool getOutputData(void *data, uint32_t timeout = UINT32_MAX) = 0;
};

#endif /* TASK_INTERFACE_HPP */