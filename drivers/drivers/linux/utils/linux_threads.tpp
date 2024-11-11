/**
 * @file linux_threads.cpp
 * @author your name (you@domain.com)
 * @brief Class to manage creation and deletion of threads on linux
 * @version 0.1
 * @date 2024-11-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "linux_threads.hpp"

/**
 * @brief Construct a new LinuxThreads<INPUT_DATA, OUTPUT_DATA, MAX_QUEUE_SIZE>::LinuxThreads object
 *
 * @tparam INPUT_DATA Data type of the input
 * @tparam OUTPUT_DATA Data type of the output
 * @tparam MAX_QUEUE_SIZE Maximum number of bytes in the queue
 * @param function Pointer to the function that will execute the work
 * @param user_arg User argument passed to the worker function
 */
template <typename INPUT_DATA, typename OUTPUT_DATA, uint32_t MAX_QUEUE_SIZE>
LinuxThreads<INPUT_DATA, OUTPUT_DATA, MAX_QUEUE_SIZE>::LinuxThreads(ThreadFunction_t function, void *user_arg)
{
  m_thread_handle = nullptr;
  m_terminate = true;
  m_function = function;
  m_user_arg = user_arg;
}

/**
 * @brief Destructor
 *
 * @tparam INPUT_DATA Data type of the input
 * @tparam OUTPUT_DATA Data type of the output
 * @tparam MAX_QUEUE_SIZE Maximum number of bytes in the queue
 */
template <typename INPUT_DATA, typename OUTPUT_DATA, uint32_t MAX_QUEUE_SIZE>
LinuxThreads<INPUT_DATA, OUTPUT_DATA, MAX_QUEUE_SIZE>::~LinuxThreads()
{
  (void) terminate();
}

/**
 * @brief Create a task
 *
 * @tparam INPUT_DATA Data type of the input
 * @tparam OUTPUT_DATA Data type of the output
 * @tparam MAX_QUEUE_SIZE Maximum number of bytes in the queue
 * @return bool
 */
template <typename INPUT_DATA, typename OUTPUT_DATA, uint32_t MAX_QUEUE_SIZE>
bool LinuxThreads<INPUT_DATA, OUTPUT_DATA, MAX_QUEUE_SIZE>::create()
{
  if(m_function == nullptr || m_thread_handle != nullptr)
  {
    return false;
  }
  m_terminate = false;
  m_thread_handle = new std::thread(&LinuxThreads::run, this);
  if(m_thread_handle != nullptr)
  {
    return true;
  }
  return false;
}

/**
 * @brief Terminate a task
 *
 * @tparam INPUT_DATA Data type of the input
 * @tparam OUTPUT_DATA Data type of the output
 * @tparam MAX_QUEUE_SIZE Maximum number of bytes in the queue
 * @return bool
 */
template <typename INPUT_DATA, typename OUTPUT_DATA, uint32_t MAX_QUEUE_SIZE>
bool LinuxThreads<INPUT_DATA, OUTPUT_DATA, MAX_QUEUE_SIZE>::terminate()
{
  INPUT_DATA input_data;
  if(!m_terminate)
  {
    m_terminate = true;
    setInputData(&input_data);
    join();
    delete m_thread_handle;
    m_thread_handle = nullptr;
  }
  return true;
}

/**
 * @brief Block until the task is terminated
 *
 * @tparam INPUT_DATA Data type of the input
 * @tparam OUTPUT_DATA Data type of the output
 * @tparam MAX_QUEUE_SIZE Maximum number of bytes in the queue
 */
template <typename INPUT_DATA, typename OUTPUT_DATA, uint32_t MAX_QUEUE_SIZE>
void LinuxThreads<INPUT_DATA, OUTPUT_DATA, MAX_QUEUE_SIZE>::join()
{
  m_thread_handle->join();
}

template <typename INPUT_DATA, typename OUTPUT_DATA, uint32_t MAX_QUEUE_SIZE>
bool LinuxThreads<INPUT_DATA, OUTPUT_DATA, MAX_QUEUE_SIZE>::setInputData(const void *data, uint32_t timeout)
{
  INPUT_DATA *input_data = (INPUT_DATA *) data;
  if(input_data != nullptr)
  {
    return m_input_queue.put(input_data[0], timeout);
  }
  return false;
}

template <typename INPUT_DATA, typename OUTPUT_DATA, uint32_t MAX_QUEUE_SIZE>
bool LinuxThreads<INPUT_DATA, OUTPUT_DATA, MAX_QUEUE_SIZE>::getOutputData(void *data, uint32_t timeout)
{
  OUTPUT_DATA *output_data = static_cast<OUTPUT_DATA *>(data);
  if(output_data != nullptr)
  {
    return m_output_queue.get(output_data[0], timeout);
  }
  return false;
}

/**
 * @brief Worker task calling a work function as demanded
 *
 * @tparam INPUT_DATA Data type of the input
 * @tparam OUTPUT_DATA Data type of the output
 * @tparam MAX_QUEUE_SIZE Maximum number of bytes in the queue
 */
template <typename INPUT_DATA, typename OUTPUT_DATA, uint32_t MAX_QUEUE_SIZE>
void LinuxThreads<INPUT_DATA, OUTPUT_DATA, MAX_QUEUE_SIZE>::run()
{
  INPUT_DATA input;
  OUTPUT_DATA output;

  while(true)
  {
    m_input_queue.get(input);
    if(m_terminate)
    {
      break;
    }
    if(m_function != nullptr)
    {
      output = m_function(input, m_user_arg);
    }
    m_output_queue.put(output);
  }
}