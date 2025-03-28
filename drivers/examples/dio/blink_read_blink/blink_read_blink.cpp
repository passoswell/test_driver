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

#include "drivers.hpp"

#define DIO_INPUT_LINE_NUMBER                                                203
#define DIO_INPUT_CHIP_NUMBER                                                  0
#define DIO_OUTPUT_LINE_NUMBER                                                 6
#define DIO_OUTPUT_CHIP_NUMBER                                                 0

const DriverSettings_t g_dio_input_list[]
{
  ADD_PARAMETER(DIO_LINE_DIRECTION, DIO_DIRECTION_INPUT),
  ADD_PARAMETER(DIO_LINE_DRIVE, DIO_DRIVE_PUSH_PULL),
  ADD_PARAMETER(DIO_LINE_BIAS, DIO_BIAS_PULL_UP)
};
uint8_t g_dio_input_list_size = sizeof(g_dio_input_list)/sizeof(g_dio_input_list[0]);

const DriverSettings_t g_dio_output_list[]
{
  ADD_PARAMETER(DIO_LINE_DIRECTION, DIO_DIRECTION_OUTPUT),
  ADD_PARAMETER(DIO_LINE_DRIVE, DIO_DRIVE_PUSH_PULL),
  ADD_PARAMETER(DIO_LINE_BIAS, DIO_BIAS_DISABLED),
  ADD_PARAMETER(DIO_LINE_INITIAL_VALUE, DIO_STATE_LOW)
};
uint8_t g_dio_output_list_size = sizeof(g_dio_output_list)/sizeof(g_dio_output_list[0]);


int main(void)
{
  bool input_value, output_value = false;
  DIO input(DIO_INPUT_LINE_NUMBER, DIO_INPUT_CHIP_NUMBER);
  DIO output(DIO_OUTPUT_LINE_NUMBER, DIO_OUTPUT_CHIP_NUMBER);

  input.configure(g_dio_input_list, g_dio_input_list_size);
  output.configure(g_dio_output_list, g_dio_output_list_size);

  while(true)
  {
    // printf("Wrote %u on line #%u\r\n", output_value, DIO_OUTPUT_LINE_NUMBER);
    for(uint8_t i = 0; i < 5; i++)
    {
      input.read(input_value);
      // printf("Read %u on line #%u\r\n", input_value, DIO_INPUT_LINE_NUMBER);
      sleep(1);
    }
    output_value = !output_value;
    output.write(output_value);
  }

  return 0;
}