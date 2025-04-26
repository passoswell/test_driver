/**
 * @file hello_world.cpp
 * @author your name (you@domain.com)
 * @brief Example code on how to write to a uart port
 * @version 0.1
 * @date 2024-07-08
 *
 * @copyright Copyright (c) 2024
 *
 * @note In this example, a UART port configured by a handle and
 * g_uart_config_list is used to print a hello world message every second
 *
 */

#include <string.h>

#include "drivers.hpp"

// Change the lines bellow with the correct handle for your platform
#if defined(USE_LINUX)
UartHandle_t handle = (UartHandle_t)"/dev/ttyUSB0";
#elif defined(USE_ESP32)
UartHandle_t handle =
{
  .uart_number = 0,
  .rx_pin = 3,
  .tx_pin = 1,
};
#else
void *handle = nullptr;
#endif

/**
 * @brief Configuration parameters for the uart port
 */
const SettingsList_t g_uart_config_list[]
{
  ADD_PARAMETER(COMM_PARAM_BAUD, 115200),
  ADD_PARAMETER(COMM_PARAM_LINE_MODE, 0), /*!< no parity, one stop bit, no hw flow control*/
  ADD_PARAMETER(COMM_WORK_ASYNC_TX, false)
};
const uint8_t g_uart_config_list_size = sizeof(g_uart_config_list)/sizeof(g_uart_config_list[0]);


/**
 * @brief Example code that prints a Hello World message on a uart port
 */
AP_MAIN()
{
  UART my_serial(handle);
  SPT my_timer(SOFTWARE_TIMER_SECONDS);
  uint8_t message[] = "\r\nHello world!!!\r\n";
  Status_t status;

  // Configure the driver
  status = my_serial.configure(g_uart_config_list, g_uart_config_list_size);
  if (!status.success)
  {
    printf("\r\nERROR from my_serial.configure: %s", status.description);
    AP_EXIT();
  }

  while(true)
  {
    // Write a hello world message to the uart port
    status = my_serial.write(message, strlen((char *)message));
    if(!status.success)
    {
      printf("\r\nERROR from my_serial.write: %s", status.description);
      AP_EXIT();
    }
    my_timer.delay(1); // One second delay
  }

  AP_EXIT();
}