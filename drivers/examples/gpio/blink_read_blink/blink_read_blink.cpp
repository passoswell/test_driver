/**
 * @file blink_read_blink.cpp
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


int main(void)
{
  bool input_value, output_value = false;
  DrvGPIO input(GPIO_INPUT_LINE_NUMBER, GPIO_INPUT_CHIP_NUMBER);
  DrvGPIO output(GPIO_OUTPUT_LINE_NUMBER, GPIO_OUTPUT_CHIP_NUMBER);

  input.configure(g_gpio_input_list, g_gpio_input_list_size);
  output.configure(g_gpio_output_list, g_gpio_output_list_size);

  while(true)
  {
    printf("Wrote %u on line #%u\r\n", output_value, GPIO_OUTPUT_LINE_NUMBER);
    for(uint8_t i = 0; i < 5; i++)
    {
      input.read(input_value);
      printf("Read %u on line #%u\r\n", input_value, GPIO_INPUT_LINE_NUMBER);
      sleep(1);
    }
    output_value = !output_value;
    output.write(output_value);
  }

  return 0;
}