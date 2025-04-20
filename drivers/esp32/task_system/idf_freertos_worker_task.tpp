/**
 * @file idf_freertos_worker_task.tpp
 * @author your name (you@domain.com)
 * @brief Implementation of worker task using IDF FREERTOS
 * @version 0.1
 * @date 2025-04-06
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "idf_freertos_worker_task.hpp"

/**
 * @brief Reducing verbosity of templates
 *
 * @tparam TI Type of the worker function's input item
 * @tparam INPUT_SIZE Maximum number of items in the input queue
 * @tparam TO Type of the worker function's output item
 * @tparam OUTPUT_SIZE Maximum number of items in the output queue
 */
#define FREERTOS_WORKER_TASK_TEMPLATE template <typename TI, uint32_t INPUT_SIZE, typename TO, uint32_t OUTPUT_SIZE>

/**
 * @brief Reducing verbosity of templates
 */
#define FREERTOS_WORKER_TASK_CLASS Task<TI, INPUT_SIZE, TO, OUTPUT_SIZE>

/**
 * @brief Constructor
 */
FREERTOS_WORKER_TASK_TEMPLATE
FREERTOS_WORKER_TASK_CLASS::Task()
{
  m_terminate = false;
  m_task_handle = nullptr;
  m_caller_task_handle = nullptr;
  m_pinned_core = 0;
}

/**
 * @brief Destroyer
 */
FREERTOS_WORKER_TASK_TEMPLATE
FREERTOS_WORKER_TASK_CLASS::~Task()
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
FREERTOS_WORKER_TASK_TEMPLATE
bool FREERTOS_WORKER_TASK_CLASS::create(ThreadFunction_t function, void *user_arg, uint32_t priority)
{
  BaseType_t freertos_return;
  TI input_data;
  TO output_data;

  if(function == nullptr || m_task_handle != nullptr)
  {
    return false;
  }

  if(priority > configMAX_PRIORITIES)
  {
    priority = configMAX_PRIORITIES;
  }

  m_terminate = false;
  m_caller_task_handle = nullptr;
  m_worker_function = function;
  m_user_arg = user_arg;
  m_state = TASK_STATE_RUN;
  m_profile.type = TASK_WORKER;

  for(uint16_t i = 0; i < INPUT_SIZE; i++)
  {
    if(!m_input_queue.get(input_data, 0))
    {
      break;
    }
  }

  for(uint16_t i = 0; i < OUTPUT_SIZE; i++)
  {
    if(!m_output_queue.get(output_data, 0))
    {
      break;
    }
  }


  freertos_return = xTaskCreate(
    [](void *parameter) -> void {Task *obj = static_cast<Task*>(parameter); obj->run();},
    nullptr,
    2 * configMINIMAL_STACK_SIZE,
    this,
    priority,
    &m_task_handle
  );
  return freertos_return == pdPASS;
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
FREERTOS_WORKER_TASK_TEMPLATE
bool FREERTOS_WORKER_TASK_CLASS::create(ThreadFunction_t function, void *user_arg, TaskProfile_t parameters)
{
  BaseType_t freertos_return;
  TI input_data;
  TO output_data;

  if(function == nullptr || m_task_handle != nullptr)
  {
    return false;
  }

  if(m_profile.priority > configMAX_PRIORITIES)
  {
    m_profile.priority = configMAX_PRIORITIES;
  }

  if(m_profile.stack_size < configMINIMAL_STACK_SIZE)
  {
    m_profile.stack_size = configMINIMAL_STACK_SIZE;
  }

  m_profile = parameters;
  m_profile.core_number = m_profile.core_number % configNUMBER_OF_CORES;

  m_terminate = false;
  m_caller_task_handle = nullptr;
  m_worker_function = function;
  m_user_arg = user_arg;
  m_state = TASK_STATE_RUN;

  for(uint16_t i = 0; i < INPUT_SIZE; i++)
  {
    if(!m_input_queue.get(input_data, 0))
    {
      break;
    }
  }

  for(uint16_t i = 0; i < OUTPUT_SIZE; i++)
  {
    if(!m_output_queue.get(output_data, 0))
    {
      break;
    }
  }

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

  freertos_return = xTaskCreatePinnedToCore(
    [](void *parameter) -> void {Task *obj = static_cast<Task*>(parameter); obj->run();},
    m_profile.name,
    m_profile.stack_size,
    this,
    m_profile.priority,
    &m_task_handle,
    m_profile.core_number
  );
  return freertos_return == pdPASS;
}

/**
 * @brief Suspend or resume task execution
 *
 * @param state
 * @return true if successful
 * @return false if an error occurred
 */
FREERTOS_WORKER_TASK_TEMPLATE
bool FREERTOS_WORKER_TASK_CLASS::setState(TaskState_t state)
{
  if(m_task_handle == nullptr)
  {
    return false;
  }
  if(state == TASK_STATE_RUN)
  {
    vTaskResume(m_task_handle);
  }else if(state == TASK_STATE_SUSPEND)
  {
    vTaskSuspend(m_task_handle);
  }
  m_state = state;
  return true;
}

/**
 * @brief Get the state of the task
 *
 * @return TaskState_t
 */
FREERTOS_WORKER_TASK_TEMPLATE
TaskState_t FREERTOS_WORKER_TASK_CLASS::getState(void)
{
  return m_state;
}

/**
 * @brief Terminate a task
 *
 * @return true if successful
 * @return false if an error occurred
 */
FREERTOS_WORKER_TASK_TEMPLATE
bool FREERTOS_WORKER_TASK_CLASS::terminate()
{
  TI input_data;

  if (m_task_handle == nullptr)
  {
    return false;
  }
  m_caller_task_handle = xTaskGetCurrentTaskHandle();
  m_terminate = true;
  setInputData(input_data, 0); // Sending dummy input to wake-up worker task
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // blocks until notified
  m_task_handle = nullptr;
  m_pinned_core = 0;
  return true;
}

/**
 * @brief Return the number of cores available
 *
 * @return uint8_t
 */
FREERTOS_WORKER_TASK_TEMPLATE
uint8_t FREERTOS_WORKER_TASK_CLASS::getCoreCount(void)
{
  return (uint8_t) configNUMBER_OF_CORES;
}

/**
 * @brief Return the core where the task is running
 *
 * @return uint8_t
 */
FREERTOS_WORKER_TASK_TEMPLATE
uint8_t FREERTOS_WORKER_TASK_CLASS::getPinnedCore(void)
{
  return  m_pinned_core;
}

/**
 * @brief Send data to a task
 *
 * @param data Data to pass to the worker thread
 * @param timeout Timeout value in milliseconds
 * @return true if successful
 * @return false if an error occurred
 */
FREERTOS_WORKER_TASK_TEMPLATE
bool FREERTOS_WORKER_TASK_CLASS::setInputData(const TI &data, uint32_t timeout)
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
FREERTOS_WORKER_TASK_TEMPLATE
bool FREERTOS_WORKER_TASK_CLASS::getOutputData(TO &data, uint32_t timeout)
{
  return m_output_queue.get(data, timeout);
}

/**
 * @brief Run the worker function
 */
FREERTOS_WORKER_TASK_TEMPLATE
void FREERTOS_WORKER_TASK_CLASS::run()
{
  TI input;
  TO output;
  bool result;

  while(true)
  {
    m_pinned_core = (uint8_t) xPortGetCoreID();
    if(m_profile.type == TASK_WORKER)
    {
      result = m_input_queue.get(input);
    }else
    {
      vTaskDelay(m_profile.period/portTICK_PERIOD_MS);
      result = true;
    }

    if(m_terminate)
    {
      break;
    }
    if(result && (m_worker_function != nullptr))
    {
      output = m_worker_function(input, m_user_arg);
      m_output_queue.put(output);
    }
  }

  if(m_caller_task_handle != nullptr)
  {
    xTaskNotifyGive(m_caller_task_handle); // signal termination
  }
  vTaskDelete(nullptr);
}