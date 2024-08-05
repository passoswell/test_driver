/**
 * @file blink_read_blink_interruption.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <iostream>
#include <unistd.h>
#include <stdio.h>

#include "drivers.hpp"

#define GPIO_INPUT_LINE_NUMBER                                               203
#define GPIO_INPUT_CHIP_NUMBER                                                 0
#define GPIO_OUTPUT_LINE_NUMBER                                                6
#define GPIO_OUTPUT_CHIP_NUMBER                                                0

const DrvGpioConfigure_t g_gpio_input_list[]
{
  ADD_GPIO_PARAMETER(DRV_GPIO_LINE_DIRECTION, DRV_GPIO_DIRECTION_INPUT),
  ADD_GPIO_PARAMETER(DRV_GPIO_LINE_DRIVE, DRV_GPIO_DRIVE_PUSH_PULL),
  ADD_GPIO_PARAMETER(DRV_GPIO_LINE_BIAS, DRV_GPIO_BIAS_DISABLED)
};
uint8_t g_gpio_input_list_size = sizeof(g_gpio_input_list)/sizeof(DrvGpioConfigure_t);

const DrvGpioConfigure_t g_gpio_output_list[]
{
  ADD_GPIO_PARAMETER(DRV_GPIO_LINE_DIRECTION, DRV_GPIO_DIRECTION_OUTPUT),
  ADD_GPIO_PARAMETER(DRV_GPIO_LINE_DRIVE, DRV_GPIO_DRIVE_PUSH_PULL),
  ADD_GPIO_PARAMETER(DRV_GPIO_LINE_BIAS, DRV_GPIO_BIAS_DISABLED),
  ADD_GPIO_PARAMETER(DRV_GPIO_LINE_INITIAL_VALUE, DRV_GPIO_STATE_LOW)
};
uint8_t g_gpio_output_list_size = sizeof(g_gpio_output_list)/sizeof(DrvGpioConfigure_t);


void inputCallback(Status_t status, uint16_t line, DrvGpioEdge_t edge, bool state, void *arg)
{
  if(status.success)
  {
    if(edge == DRV_GPIO_EDGE_RISING)
    {
      printf("Read %u, rising edge, on line #%u\r\n", state, GPIO_INPUT_LINE_NUMBER);
    }else
    {
      printf("Read %u, falling edge, on line #%u\r\n", state, GPIO_INPUT_LINE_NUMBER);
    }
  }else
  {
    printf("Error reading event on line #%u\r\n", GPIO_INPUT_LINE_NUMBER);
  }
}


int main(void)
{
  bool input_value, output_value = false;
  DrvGPIO input(GPIO_INPUT_LINE_NUMBER, GPIO_INPUT_CHIP_NUMBER);
  DrvGPIO output(GPIO_OUTPUT_LINE_NUMBER, GPIO_OUTPUT_CHIP_NUMBER);

  input.configure(g_gpio_input_list, g_gpio_input_list_size);
  output.configure(g_gpio_output_list, g_gpio_output_list_size);
  input.setCallback(DRV_GPIO_EDGE_BOTH, inputCallback, nullptr);

  while(true)
  {
    printf("Wrote %u on line #%u\r\n", output_value, GPIO_OUTPUT_LINE_NUMBER);
    for(uint8_t i = 0; i < 5; i++)
    {
      sleep(1);
    }
    output_value = !output_value;
    output.write(output_value);
  }

  return 0;
}