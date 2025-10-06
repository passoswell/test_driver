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

#include <cstring>

#include "drivers.hpp"

// Change the lines bellow with the correct handle for your platform
#if defined(USE_LINUX)

constexpr UartHandle_t handle = 100; // /dev/serial100

/**
 * @brief Configuration parameters for the uart port
 */
const SettingsList_t g_uart_config_list[]
{
  ADD_PARAMETER(COMM_PARAM_BAUD, 115200),
  ADD_PARAMETER(COMM_PARAM_LINE_MODE, 0), /*!< no parity, one stop bit, no hw flow control*/
  ADD_PARAMETER(COMM_WORK_ASYNC_RX, false),
  ADD_PARAMETER(COMM_WORK_ASYNC_TX, false),
};
const uint8_t g_uart_config_list_size = sizeof(g_uart_config_list)/sizeof(g_uart_config_list[0]);

#elif defined(USE_ESP32)

constexpr UartHandle_t handle = 0;

/**
 * @brief Configuration parameters for the uart port
 */
const SettingsList_t g_uart_config_list[]
{
  ADD_PARAMETER(COMM_PARAM_BAUD, 115200),
  ADD_PARAMETER(COMM_PARAM_LINE_MODE, 0), /*!< no parity, one stop bit, no hw flow control*/
  ADD_PARAMETER(COMM_WORK_ASYNC_RX, false),
  ADD_PARAMETER(COMM_WORK_ASYNC_TX, false),
  ADD_PARAMETER(COMM_PARAM_RX_DIO_PIN, 3),
  ADD_PARAMETER(COMM_PARAM_TX_DIO_PIN, 1),
};
const uint8_t g_uart_config_list_size = sizeof(g_uart_config_list)/sizeof(g_uart_config_list[0]);

#else
constexpr void *handle = nullptr;
#endif


/**
 * @brief Example code that prints a Hello World message on a uart port
 */
AP_MAIN()
{
  // UartBase &my_serial = UART<handle>::getInstance();
  UART<handle> my_serial;
  SPT my_timer(SOFTWARE_TIMER_SECONDS);
  uint8_t message[] = "Hello world!!!";
  uint8_t buffer[100];
  ErrorCode status;
  int result;
  uint32_t counter = 0;

  std::printf("\r\nCode is running\r\n");

  // Configure the driver
  status = my_serial.configure(g_uart_config_list, g_uart_config_list_size);
  if (status)
  {
    std::printf("\r\nERROR from my_serial.configure: %s", status.message().data());
    AP_EXIT();
  }

  while(true)
  {
    // Write a hello world message to the uart port
    result = std::snprintf((char *)buffer, sizeof(buffer)-1, "[%03u] %s\r\n", counter, message);
    if(result < 0)
    {
      std::printf("\r\nERROR generating hello world message\r\n");
      AP_EXIT();
    }
    status = my_serial.write({buffer, (uint32_t)result});
    if(status)
    {
      std::printf("\r\nERROR from my_serial.write: %s", status.message().data());
      AP_EXIT();
    }
    my_timer.delay(1); // One second delay
    counter++;
  }

  AP_EXIT();
}