/**
 * @file stl_worker_task.tpp
 * @author your name (you@domain.com)
 * @brief Implementation of worker task using STL C++
 * @version 0.1
 * @date 2025-04-06
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "stl_worker_task.hpp"

/**
 * @brief Reducing verbosity of templates
 *
 * @tparam TI Type of the worker function's input item
 * @tparam INPUT_SIZE Maximum number of items in the input queue
 * @tparam TO Type of the worker function's output item
 * @tparam OUTPUT_SIZE Maximum number of items in the output queue
 */
#define STL_WORKER_TASK_TEMPLATE template <typename TI, uint32_t INPUT_SIZE, typename TO, uint32_t OUTPUT_SIZE>

/**
 * @brief Reducing verbosity of templates
 */
#define STL_WORKER_TASK_CLASS Task<TI, INPUT_SIZE, TO, OUTPUT_SIZE>


/**
 * @brief Destroyer
 */
STL_WORKER_TASK_TEMPLATE
STL_WORKER_TASK_CLASS::~Task()
{
  (void) terminate();
}

/**
 * @brief Create a task and install a worker function
 *
 * @param function A worker function called by the task
 * @param user_arg An user argument passed as parameter to the worker function
 * @param priority Task priority
 * @return true if successful
 * @return false if an error occurred
 */
STL_WORKER_TASK_TEMPLATE
bool STL_WORKER_TASK_CLASS::create(ThreadFunction_t function, void *user_arg, uint32_t priority)
{
  (void) priority;
  if(function == nullptr || m_thread_handle != nullptr)
  {
    return false;
  }
  m_terminate = false;
  m_worker_function = function;
  m_user_arg = user_arg;
  m_state = TASK_STATE_RUN;
  m_profile.type = TASK_WORKER;
  m_pinned_core = 0;
  m_profile.core_number = getCoreCount() + 1;
  m_thread_handle = new std::thread(&Task::run, this);
  if(m_thread_handle == nullptr)
  {
    return false;
  }
  return true;
}

/**
 * @brief Create a task
 *
 * @param function A worker function called by the task
 * @param user_arg An user argument passed as parameter to the worker function
 * @param parameters List of configuration parameters to profile the task
 * @return true if successful
 * @return false if an error occurred
 */
STL_WORKER_TASK_TEMPLATE
bool STL_WORKER_TASK_CLASS::create(ThreadFunction_t function, void *user_arg, TaskProfile_t parameters)
{

  if(function == nullptr || m_thread_handle != nullptr)
  {
    return false;
  }

  m_profile = parameters;

  m_terminate = false;
  m_worker_function = function;
  m_user_arg = user_arg;
  m_pinned_core = 0;
  m_thread_handle = new std::thread(&Task::run, this);
  if(m_thread_handle == nullptr)
  {
    return false;
  }
  return true;
}

/**
 * @brief Suspend or resume task execution
 *
 * @param state
 * @return true if successful
 * @return false if an error occurred
 */
STL_WORKER_TASK_TEMPLATE
bool STL_WORKER_TASK_CLASS::setState(TaskState_t state)
{
  // There is no standard way of doing this using stl c++, only saving the state
  if(m_thread_handle == nullptr)
  {
    return false;
  }
  m_state = state;
  return true;
}

/**
 * @brief Get the state of the task
 *
 * @return TaskState_t
 */
STL_WORKER_TASK_TEMPLATE
TaskState_t STL_WORKER_TASK_CLASS::getState(void)
{
  return m_state;
}

/**
 * @brief Terminate a task
 *
 * @return true if successful
 * @return false if an error occurred
 */
STL_WORKER_TASK_TEMPLATE
bool STL_WORKER_TASK_CLASS::terminate()
{
  TI input_data;

  if(m_thread_handle == nullptr)
  {
    return false;
  }
  m_terminate = true;
  setInputData(input_data, 0); // Sending dummy input to wake-up worker task
  m_thread_handle->join();
  delete m_thread_handle;
  m_thread_handle = nullptr;
  m_pinned_core = 0;
  return true;
}

/**
 * @brief Return the number of cores available
 *
 * @return uint8_t
 */
STL_WORKER_TASK_TEMPLATE
uint8_t STL_WORKER_TASK_CLASS::getCoreCount(void)
{
  long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
  if(number_of_processors < 256)
  {
    return static_cast<uint8_t>(number_of_processors);
  }
  return 255;
}

/**
 * @brief Return the core where the task is running
 *
 * @return uint8_t
 */
STL_WORKER_TASK_TEMPLATE
uint8_t STL_WORKER_TASK_CLASS::getPinnedCore(void)
{
  return m_pinned_core;
}

/**
 * @brief Send data to a task
 *
 * @param data Data to pass to the worker thread
 * @param timeout Timeout value in milliseconds
 * @return true if successful
 * @return false if an error occurred
 */
STL_WORKER_TASK_TEMPLATE
bool STL_WORKER_TASK_CLASS::setInputData(const TI &data, uint32_t timeout)
{
  return m_input_queue.put(data, timeout);
}

/**
 * @brief Get data from a task
 *
 * @param data Buffer to store the worker thread's output
 * @param timeout Timeout value in milliseconds
 * @return true if successful
 * @return false if an error occurred
 */
STL_WORKER_TASK_TEMPLATE
bool STL_WORKER_TASK_CLASS::getOutputData(TO &data, uint32_t timeout)
{
  return m_output_queue.get(data, timeout);
}

/**
 * @brief Run the worker function
 */
STL_WORKER_TASK_TEMPLATE
void STL_WORKER_TASK_CLASS::run()
{
  TI input;
  TO output;
  bool result;

  // Setting the core affinity
  if(m_profile.core_number < getCoreCount())
  {
    pthread_t ptread_handle = pthread_self();
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(m_profile.core_number, &cpuset);
    (void) pthread_setaffinity_np(ptread_handle, sizeof(cpu_set_t), &cpuset);
  }

  while(true)
  {
    m_pinned_core = static_cast<uint8_t>(sched_getcpu());
    if(m_profile.type == TASK_WORKER)
    {
      result = m_input_queue.get(input, 5000);
    }else
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_profile.period));
      result = true;
    }

    if(m_terminate)
    {
      break;
    }
    if(result && (m_worker_function != nullptr) && (m_state == TASK_STATE_RUN))
    {
      output = m_worker_function(input, m_user_arg);
      m_output_queue.put(output);
    }
  }
}