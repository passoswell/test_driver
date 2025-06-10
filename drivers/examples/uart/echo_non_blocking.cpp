/**
 * @file echo_non_blocking.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-26
 *
 * @copyright Copyright (c) 2025
 *
 * @note In this example, a UART port configured by a handle and
 * g_uart_config_list is used to echo whatever is received through it in
 * asynchronous (non-blocking) mode.
 *
 */

#include <string.h>

#include "drivers.hpp"

// Change the lines bellow with the correct handle for your platform
#if defined(USE_LINUX)
UartHandle_t handle = 100; // /dev/serial100
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
  ADD_PARAMETER(COMM_WORK_ASYNC_RX, true),
  ADD_PARAMETER(COMM_WORK_ASYNC_TX, true)
};
const uint8_t g_uart_config_list_size = sizeof(g_uart_config_list)/sizeof(g_uart_config_list[0]);


static Status_t rxCallback(Status_t status, EventsList_t event, const Buffer_t data, void *user_arg);

static Status_t txCallback(Status_t status, EventsList_t event, const Buffer_t data, void *user_arg);

static UART g_serial(handle);
static uint8_t g_rx_buffer[2048] = {0};
static uint8_t g_tx_buffer[100] = {0};
static uint8_t MESSAGE_HELLO_WORLD[] = "\r\nHello world!!!\r\n";
bool g_error_flag = false;


/**
 * @brief Example code that echoes what it receives through a UART port using interruption
 */
AP_MAIN()
{
  Status_t status;
  SPT timer;
  uint32_t bytes_read = 0, tx_bytes = 0;

  timer.delay(1000);

  // Configure the driver
  status = g_serial.configure(g_uart_config_list, g_uart_config_list_size);
  if (!status.success)
  {
    printf("\r\nERROR from g_serial.configure: %s", status.description);
    AP_EXIT();
  }

  // Install callback functions for uart events
  status = g_serial.setCallback(EVENT_READ, rxCallback, nullptr);
  status = g_serial.setCallback(EVENT_WRITE, txCallback, nullptr);

  // Write a hello message in async mode
  status = g_serial.write(MESSAGE_HELLO_WORLD, strlen((char *)MESSAGE_HELLO_WORLD));
  if (!status.success)
  {
    printf("\r\nERROR from g_serial.write: %s", status.description);
    AP_EXIT();
  }

  // Start the async read operation
  status = g_serial.read(g_rx_buffer, sizeof(g_rx_buffer), 20);
  if (!status.success && status.code != ERR_TIMEOUT)
  {
    printf("\r\nERROR from g_serial.read: %s", status.description);
    AP_EXIT();
  }

  while(true)
  {
    // Nothing is done here
    timer.delay(1000);
    if(g_error_flag)
    {
      break;
    }
  }

  AP_EXIT();
}

/**
 * @brief Callback on end of transmission
 *
 * @param status Status of end of operation
 * @param event The event that generated the call
 * @param data The data used during the call (buffer ans size)
 * @param user_arg User supplied argument, not used
 * @return Status_t
 */
Status_t rxCallback(Status_t status, EventsList_t event, const Buffer_t data, void *user_arg)
{
  SPT timer;
  static uint32_t counter = 0;
  if(status.success)
  {
    printf("\r\n\r\n[%03u] From reception callback: %lu bytes received\r\n", counter, data.size_bytes());

    // Wait any ongoing transmission to finish
    while (g_serial.getWriteStatus().code == OPERATION_RUNNING)
    {
      timer.delay(1);
    }

    // Write to the uart the data received
    status = g_serial.write(g_rx_buffer, data.size_bytes());
    if(!status.success)
    {
      printf("\r\nERROR from g_serial.write: %s", status.description);
      g_error_flag = true;
    }

    // Start a new async read operation
    status = g_serial.read(g_rx_buffer, sizeof(g_rx_buffer), 20);
    if (!status.success)
    {
      printf("\r\nERROR from g_serial.read: %s", status.description);
      g_error_flag = true;
    }

  }else
  {
    printf("\r\n\r\n[%03u] From reception callback: ended in failure: %s\r\n", counter, status.description);
  }
  counter++;
  return status;
}

/**
 * @brief Callback on end of transmission
 *
 * @param status Status of end of operation
 * @param event The event that generated the call
 * @param data The data used during the call (buffer ans size)
 * @param user_arg User supplied argument, not used
 * @return Status_t
 */
Status_t txCallback(Status_t status, EventsList_t event, const Buffer_t data, void *user_arg)
{
  static uint32_t counter = 0;
  if(status.success)
  {
    printf("\r\n\r\n[%03u] From transmission callback:  %lu bytes transmitted\r\n", counter, data.size_bytes());
  }else
  {
    printf("\r\n\r\n[%03u] From transmission callback: ended in failure: %s\r\n", counter, status.description);
  }
  counter++;
  return status;
}