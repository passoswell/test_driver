/**
 * @file ex_queue.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-18
 *
 * @copyright Copyright (c) 2025
 *
 * @note The test consists on creating a queue with five positions, trying to
 * put six items, and then remove those 6 items
 *
 */

#include <stdio.h>

#include "drivers.hpp"

AP_MAIN()
{
  Queue<uint16_t, 5> my_queue;
  uint16_t item;

  printf("\r\nHello world!!!\r\n");

  printf("Maximum queue capacity: %u\r\n", my_queue.getMaxSize());
  printf("Elements on queue: %u\r\n", my_queue.getActualSize());

  for (uint16_t i = 0; i < 6; i++)
  {
    if (my_queue.put(i, 5000))
    {
      printf("Success enqueuing %u\r\n", i);
      printf("\r\nElements on queue: %u\r\n", my_queue.getActualSize());
    }
    else
    {
      printf("Failed enqueuing %u\r\n", i);
    }
  }

  for (uint16_t i = 0; i < 6; i++)
  {
    if (my_queue.get(item, 5000))
    {
      printf("Success dequeuing %u\r\n", item);
      printf("\r\nElements on queue: %u\r\n", my_queue.getActualSize());
    }
    else
    {
      printf("Failed dequeuing %u\r\n", i);
    }
  }

  AP_EXIT();
}