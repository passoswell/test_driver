/**
 * @file blink_read_blink_interruption.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 *
 * @note Connect a digital input and a digital output together and run the code.
 * The output can be configure changing the value of "DIO_OUTPUT_LINE_NUMBER",
 * and "DIO_OUTPUT_CHIP_NUMBER", while the input is configured through
 * "DIO_INPUT_LINE_NUMBER" and "DIO_INPUT_CHIP_NUMBER". If printf is available,
 * the state of both input and output is printed through the default serial
 * port / terminal interface. Else, comment out the calls to printf and
 * use a debugger and break points.
 *
 */

#include "drivers.hpp"

/**
 * @brief DIO pins
 */
#if defined(USE_LINUX)

constexpr uint32_t DIO_INPUT_LINE_NUMBER = 203;
constexpr uint32_t DIO_INPUT_CHIP_NUMBER = 0;
constexpr uint32_t DIO_OUTPUT_LINE_NUMBER = 6;
constexpr uint32_t DIO_OUTPUT_CHIP_NUMBER = 0;

#elif defined(USE_ESP32)

constexpr uint32_t DIO_INPUT_LINE_NUMBER = 4;
constexpr uint32_t DIO_INPUT_CHIP_NUMBER = 0;
constexpr uint32_t DIO_OUTPUT_LINE_NUMBER = 2;
constexpr uint32_t DIO_OUTPUT_CHIP_NUMBER = 0;

#elif defined(USE_PIPICO)

constexpr uint32_t DIO_INPUT_LINE_NUMBER = 24;
constexpr uint32_t DIO_INPUT_CHIP_NUMBER = 0;
constexpr uint32_t DIO_OUTPUT_LINE_NUMBER = 25;
constexpr uint32_t DIO_OUTPUT_CHIP_NUMBER = 0;

#endif

/**
 * @brief Digital input parameters
 */
const SettingsList_t g_dio_input_list[]
{
  ADD_PARAMETER(DIO_LINE_DIRECTION, DIO_DIRECTION_INPUT),
  ADD_PARAMETER(DIO_LINE_DRIVE, DIO_DRIVE_PUSH_PULL),
  ADD_PARAMETER(DIO_LINE_BIAS, DIO_BIAS_PULL_UP)
};
uint8_t g_dio_input_list_size = sizeof(g_dio_input_list)/sizeof(g_dio_input_list[0]);

/**
 * @brief Digital output parameters
 */
const SettingsList_t g_dio_output_list[]
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
Status_t inputCallback(Status_t status, EventsList_t event, const Buffer_t data, void *user_arg)
{
  if(status.success)
  {
    if(event == EVENT_EDGE_RISING)
    {
      printf("Detected rising edge on line #%u\r\n", DIO_INPUT_LINE_NUMBER);
    }else
    {
      printf("Detected falling edge on line #%u\r\n", DIO_INPUT_LINE_NUMBER);
    }
  }else
  {
    printf("Error reading event on line #%u\r\n", DIO_INPUT_LINE_NUMBER);
  }
  return status;
}

/**
 * @brief  Blink an LED and read a digital input
 */
AP_MAIN()
{
  Status_t code;
  SPT my_timer;
  bool input_value, output_value = false;
  DIO input(DIO_INPUT_LINE_NUMBER, DIO_INPUT_CHIP_NUMBER);
  DIO output(DIO_OUTPUT_LINE_NUMBER, DIO_OUTPUT_CHIP_NUMBER);

  code = input.configure(g_dio_input_list, g_dio_input_list_size);
  if(!code.success)
  {
    printf("Failed to configure the digital input\r\n");
    AP_EXIT();
  }
  code = output.configure(g_dio_output_list, g_dio_output_list_size);
  if(!code.success)
  {
    printf("Failed to configure the digital output\r\n");
    AP_EXIT();
  }

  (void) input.setCallback(EVENT_EDGE_BOTH, inputCallback, nullptr);
  code = input.enableCallback(true, EVENT_EDGE_BOTH);
  if(!code.success)
  {
    printf("Failed to enable interruption for the digital input\r\n");
    AP_EXIT();
  }else
  {
    printf("Enabled 'EVENT_EDGE_BOTH' interruption for the digital input\r\n");
  }

  while(true)
  {
    printf("Writing %u on line #%u\r\n", output_value, DIO_OUTPUT_LINE_NUMBER);
    (void) output.write(output_value);
    output_value = !output_value;
    my_timer.delay(2000);
  }
}