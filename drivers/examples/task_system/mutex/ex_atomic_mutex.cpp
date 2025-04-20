/**
 * @file ex_atomic_mutex.cpp
 * @author your name (you@domain.com)
 * @brief Example code on how to use AtomicMutex class
 * @version 0.1
 * @date 2025-04-17
 *
 * @copyright Copyright (c) 2025
 *
 * @note The test code consists on running two tasks that increment a global
 * variable from 0 up to 1000000 simultaneously. This means there is a shared
 * resource that is accessed at the same time by two tasks. The example runs
 * the test twice. The first time with tasks using no mutex. The second time
 * using tasks that use a global mutex to take ownership of the global variable.
 * The expected result is a correct value on the global variable only when using
 * a mutex.
 */

#include <stdio.h>

#include "drivers.hpp"

constexpr uint32_t EX_MUTEX_TASK_PRIORITY = 0;
static AtomicMutex g_mutex;
static uint32_t g_counter;

/**
 * @brief Callback function using mutex to increment g_counter
 *
 * @param data Task number
 * @param user_arg User argument, not used
 * @return uint16_t
 */
static uint16_t callbackMutex(uint16_t data, void *user_arg);
uint16_t callbackMutex(uint16_t data, void *user_arg)
{
  (void) user_arg;
  printf("Task %u is running\r\n", data);
  for(uint32_t i = 0; i < 1000000; i++)
  {
    g_mutex.lock();
    g_counter = g_counter + 1;
    g_mutex.unlock();
  }
  printf("Task %u finished executing\r\n", data);
  return data;
}

/**
 * @brief Callback function not using mutex to increment g_counter
 *
 * @param data
 * @param user_arg
 * @return uint16_t
 */
static uint16_t callbackNoMutex(uint16_t data, void *user_arg);
uint16_t callbackNoMutex(uint16_t data, void *user_arg)
{
  (void) user_arg;
  for(uint32_t i = 0; i < 1000000; i++)
  {
    g_counter = g_counter + 1;
  }
  printf("Task %u finished executing\r\n", data);
  return data;
}

/**
 * @brief Entry point function
 */
AP_MAIN()
{
  Task<uint16_t, 1, uint16_t, 1> my_task[2];

  printf("\r\nTesting without mutex\r\n");
  g_counter = 0;

  for(uint8_t i = 0; i < sizeof(my_task)/sizeof(Task<uint16_t, 1, uint16_t, 1>); i++)
  {
    my_task[i].create(callbackNoMutex, nullptr, EX_MUTEX_TASK_PRIORITY);
    my_task[i].setInputData((uint16_t)i);
  }

  for(uint8_t i = 0; i < sizeof(my_task)/sizeof(Task<uint16_t, 1, uint16_t, 1>); i++)
  {
    uint16_t data;
    my_task[i].getOutputData(data);
    printf("Task %u returned data\r\n", i);
  }

  printf("Counter value: %u\r\n", g_counter);

  printf("\r\nTesting with mutex\r\n");
  g_counter = 0;

  for(uint8_t i = 0; i < sizeof(my_task)/sizeof(Task<uint16_t, 1, uint16_t, 1>); i++)
  {
    my_task[i].terminate();
    my_task[i].create(callbackMutex, nullptr, EX_MUTEX_TASK_PRIORITY);
    my_task[i].setInputData((uint16_t)i);
  }

  for(uint8_t i = 0; i < sizeof(my_task)/sizeof(Task<uint16_t, 1, uint16_t, 1>); i++)
  {
    uint16_t data;
    my_task[i].getOutputData(data);
    printf("Task %u returned data\r\n", i);
  }

  printf("Counter value: %u\r\n", g_counter);

  AP_EXIT();
}