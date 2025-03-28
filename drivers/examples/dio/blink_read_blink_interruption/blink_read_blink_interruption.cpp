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

#include "drivers.hpp"

/**
 * @brief DIO pins
 */
#define DIO_INPUT_LINE_NUMBER                                                203
#define DIO_INPUT_CHIP_NUMBER                                                  0
#define DIO_OUTPUT_LINE_NUMBER                                                 6
#define DIO_OUTPUT_CHIP_NUMBER                                                 0

/**
 * @brief Digital input parameters
 */
const DriverSettings_t g_dio_input_list[]
{
  ADD_PARAMETER(DIO_LINE_DIRECTION, DIO_DIRECTION_INPUT),
  ADD_PARAMETER(DIO_LINE_DRIVE, DIO_DRIVE_PUSH_PULL),
  ADD_PARAMETER(DIO_LINE_BIAS, DIO_BIAS_PULL_UP)
};
uint8_t g_dio_input_list_size = sizeof(g_dio_input_list)/sizeof(g_dio_input_list[0]);

/**
 * @brief Digital output parameters
 */
const DriverSettings_t g_dio_output_list[]
{
  ADD_PARAMETER(DIO_LINE_DIRECTION, DIO_DIRECTION_OUTPUT),
  ADD_PARAMETER(DIO_LINE_DRIVE, DIO_DRIVE_PUSH_PULL),
  ADD_PARAMETER(DIO_LINE_BIAS, DIO_BIAS_DISABLED),
  ADD_PARAMETER(DIO_LINE_INITIAL_VALUE, DIO_STATE_LOW)
};
uint8_t g_dio_output_list_size = sizeof(g_dio_output_list)/sizeof(g_dio_output_list[0]);

/**
 * @brief Callback function for dio events
 */
Status_t inputCallback(Status_t status, DriverEventsList_t event, const Buffer_t data, void *user_arg)
{
  if(status.success)
  {
    if(event == EVENT_EDGE_BOTH)
    {
      // printf("Read %u, rising edge, on line #%u\r\n", state, GPIO_INPUT_LINE_NUMBER);
    }else
    {
      // printf("Read %u, falling edge, on line #%u\r\n", state, GPIO_INPUT_LINE_NUMBER);
    }
  }else
  {
    // printf("Error reading event on line #%u\r\n", GPIO_INPUT_LINE_NUMBER);
  }
  return status;
}

/**
 * @brief Connect the input and output digital pint together and run the code
 *        using break points. If printf is available, uncomment the lines
 *        where a call to it appears
 */
int main(void)
{
  bool input_value, output_value = false;
  DIO input(DIO_INPUT_LINE_NUMBER, DIO_INPUT_CHIP_NUMBER);
  DIO output(DIO_OUTPUT_LINE_NUMBER, DIO_OUTPUT_CHIP_NUMBER);

  input.configure(g_dio_input_list, g_dio_input_list_size);
  output.configure(g_dio_output_list, g_dio_output_list_size);

  input.configure(g_dio_input_list, g_dio_input_list_size);
  output.configure(g_dio_output_list, g_dio_output_list_size);
  input.setCallback(EVENT_EDGE_BOTH, inputCallback, nullptr);

  while(true)
  {
    // printf("Wrote %u on line #%u\r\n", output_value, GPIO_OUTPUT_LINE_NUMBER);
    for(uint8_t i = 0; i < 5; i++)
    {
      sleep(1);
    }
    output_value = !output_value;
    output.write(output_value);
  }

  return 0;
}