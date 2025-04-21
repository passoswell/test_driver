/**
 * @file blink.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-03-27
 *
 * @copyright Copyright (c) 2025
 *
 * @note In this example, a LED is kept blinking with a period of one second.
 * The LED pin can be configure changing the value of "DIO_OUTPUT_LINE_NUMBER",
 * while the LED port is configured through "DIO_OUTPUT_CHIP_NUMBER".
 *
 */

#include <stdio.h>

#include "drivers.hpp"

/**
 * @brief DIO pins
 */
#if defined(USE_LINUX)

constexpr uint32_t DIO_OUTPUT_LINE_NUMBER = 6;
constexpr uint32_t DIO_OUTPUT_CHIP_NUMBER = 0;

#elif defined(USE_ESP32)

constexpr uint32_t DIO_OUTPUT_LINE_NUMBER = 2;
constexpr uint32_t DIO_OUTPUT_CHIP_NUMBER = 0;

#endif

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
AP_MAIN()
{
  Status_t code;
  SPT my_timer;
  bool input_value, output_value = false;
  DIO output(DIO_OUTPUT_LINE_NUMBER, DIO_OUTPUT_CHIP_NUMBER);

  code = output.configure(g_dio_output_list, g_dio_output_list_size);
  if(!code.success)
  {
    printf("Failed to configure the UART port\r\n");
    AP_EXIT();
  }

  while(true)
  {
    output_value = !output_value;
    (void) output.write(output_value);
    if(output_value)
    {
      printf("LED pin is set to HIGH\r\n");
    }else
    {
      printf("LED pin is set to LOW\r\n");
    }
    my_timer.delay(500);
  }

  AP_EXIT();
}