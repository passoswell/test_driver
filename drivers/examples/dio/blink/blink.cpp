/**
 * @file blink.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-03-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "drivers.hpp"

/**
 * @brief LED pin
 */
#define DIO_OUTPUT_LINE_NUMBER                                                 6
#define DIO_OUTPUT_CHIP_NUMBER                                                 0

/**
 * @brief LED configuration parameters
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
 * @brief Blinks a LED every 1 second
 */
int main(void)
{
  SPT my_timer;
  bool input_value, output_value = false;
  DIO output(DIO_OUTPUT_LINE_NUMBER, DIO_OUTPUT_CHIP_NUMBER);

  output.configure(g_dio_output_list, g_dio_output_list_size);

  while(true)
  {
    output_value = !output_value;
    output.write(output_value);
    my_timer.delay(1000);
  }

  return 0;
}