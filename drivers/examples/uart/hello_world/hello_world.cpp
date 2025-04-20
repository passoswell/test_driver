/**
 * @file hello_world.cpp
 * @author your name (you@domain.com)
 * @brief Example code on how to write to a uart port
 * @version 0.1
 * @date 2024-07-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <string.h>

#include "drivers.hpp"

static void errorHandler(Status_t status);

// Change the lines bellow with the correct handle for your platform
#if defined(USE_LINUX)
void *handle = (void *) "/dev/ttyUSB0";
#else
void *handle = nullptr;
#endif

/**
 * @brief Configuration parameters for the uart port
 */
const DriverSettings_t g_uart_config_list[]
{
  ADD_PARAMETER(COMM_PARAM_BAUD, 115200),
  ADD_PARAMETER(COMM_PARAM_LINE_MODE, 0), /*!< no parity, one stop bit, no hw flow control*/
  ADD_PARAMETER(COMM_WORK_ASYNC, true)
};
uint8_t g_uart_config_list_size = sizeof(g_uart_config_list)/sizeof(g_uart_config_list[0]);


/**
 * @brief Example code that prints a Hello World message on a uart port
 */
AP_MAIN()
{
  UART my_serial(handle);
  SPT my_timer(SOFTWARE_TIMER_SECONDS);
  uint8_t message[] = "Hello world!!!\r\n";
  Status_t status;

  status = my_serial.configure(g_uart_config_list, g_uart_config_list_size);
  if (!status.success)
  {
    AP_EXIT();
    return status.code; // Should not get here
  }

  while(true)
  {
    // status = my_serial.write(message);
    status = my_serial.write(message, strlen((char *)message)); // Another write format
    if(!status.success)
    {
      AP_EXIT();
      return status.code; // Should not get here
    }
    my_timer.delay(1); // One second delay
  }

  AP_EXIT();
}