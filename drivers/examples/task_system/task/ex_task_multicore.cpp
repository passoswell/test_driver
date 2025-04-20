/**
 * @file ex_task_multicore.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-17
 *
 * @copyright Copyright (c) 2025
 *
 * @note The test consists on creating two tasks on cores 0 and 1 using the
 * appropriate API. Then, the task is run 15 times by sending data and getting
 * the processed data back. The data sent to the tasks is a random number, the
 * tasks just send the input data back as output.
 *
 */

#include "drivers.hpp"

uint16_t taskFunction(uint16_t data, void *user_arg)
{
  uint8_t core_number = *static_cast<uint8_t *>(user_arg);
  printf("Task function from core %u received %u\r\n", core_number, data);
  return data;
}

AP_MAIN()
{
  SPT my_timer;
  Task<uint16_t, 1, uint16_t, 1> my_task_core0, my_task_core1;
  uint16_t item;
  TaskProfile_t parameters;
  uint8_t core_task0 = 0, core_task1 = 1;

  printf("\r\nThe platform running this test has %u cores available\r\n", my_task_core0.getCoreCount());

  parameters.name = "Task 0";
  parameters.type = TASK_WORKER;
  parameters.stack_size = 2048;
  parameters.priority = 5;
  parameters.core_number = 0;
  if (my_task_core0.create(taskFunction, &core_task0, parameters))
  {
    core_task0 = my_task_core0.getPinnedCore();
    printf("\r\nTask created successfully on core %u, running example \r\n", core_task0);
  }
  else
  {
    printf("\r\nTask creation failed, aborting example \r\n");
    AP_EXIT();
  }

  parameters.core_number = 1;
  if (my_task_core1.create(taskFunction, &core_task1, parameters))
  {
    core_task1 = my_task_core1.getPinnedCore();
    printf("\r\nTask created successfully on core %u, running example \r\n", core_task1);
  }
  else
  {
    printf("\r\nTask creation failed, aborting example \r\n");
    AP_EXIT();
  }

  for (uint8_t i = 0; i < 15; i++)
  {
    my_task_core0.setInputData(10 * i, 1000);
    my_task_core0.getOutputData(item, 1000);
    core_task0 = my_task_core0.getPinnedCore();
    if (item != 10 * i)
    {
      printf("Found an error, sent %u to the task and received %u to task on core 0\r\n", 10 * i, item);
    }

    my_task_core1.setInputData(10 * i, 1000);
    my_task_core1.getOutputData(item, 1000);
    core_task1 = my_task_core1.getPinnedCore();
    if (item != 10 * i)
    {
      printf("Found an error, sent %u to the task and received %u to task on core 1\r\n", 10 * i, item);
    }
  }
  printf("\r\nEnd of the example \r\n");
  AP_EXIT();
}