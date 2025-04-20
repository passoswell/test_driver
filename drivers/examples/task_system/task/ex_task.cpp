/**
 * @file ex_task.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-17
 *
 * @copyright Copyright (c) 2025
 *
 * @note The test consists on creating one task using the appropriate API. Then,
 * the task is run 15 times by sending data and getting the processed data back.
 * The data sent to the tasks is a random number, the task just send the input
 * data back as output.
 */

#include "drivers.hpp"

uint16_t taskFunction(uint16_t data, void *user_arg)
{
  printf("Task function received %u\r\n", data);
  return data;
}

AP_MAIN()
{
  SPT my_timer;
  Task<uint16_t, 1, uint16_t, 1> my_task;
  uint16_t item;

  if (my_task.create(taskFunction, nullptr, 5))
  {
    printf("\r\nTask created successfully, running example \r\n");
  }
  else
  {
    printf("\r\nTask creation failed, aborting example \r\n");
    AP_EXIT();
  }

  for (uint8_t i = 0; i < 15; i++)
  {
    my_task.setInputData(10 * i, 1000);
    my_task.getOutputData(item, 1000);
    if (item != 10 * i)
    {
      printf("Found an error, sent %u to the task and received %u \r\n", 10 * i, item);
    }
  }
  printf("\r\nEnd of the example \r\n");
  AP_EXIT();
}