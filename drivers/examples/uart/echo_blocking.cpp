/**
 * @file echo_blocking.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-22
 *
 * @copyright Copyright (c) 2025
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
  ADD_PARAMETER(COMM_WORK_ASYNC_RX, false),
  ADD_PARAMETER(COMM_WORK_ASYNC_TX, false)
};
uint8_t g_uart_config_list_size = sizeof(g_uart_config_list)/sizeof(g_uart_config_list[0]);


AP_MAIN()
{
  Status_t status;
  UART my_serial(handle);
  uint32_t bytes_read = 0, tx_bytes = 0;
  uint8_t buffer[1024] = {0};
  uint8_t tx_buffer[100] = {0};
  uint8_t message[] = "\r\nHello world!!!\r\n";

  status = my_serial.configure(g_uart_config_list, g_uart_config_list_size);
  if (!status.success)
  {
    printf("\r\nERROR from my_serial.configure: %s", status.description);
    AP_EXIT();
  }

  status = my_serial.write(message, strlen((char *)message));
  if(!status.success)
  {
    printf("\r\nERROR from my_serial.write: %s", status.description);
    AP_EXIT();
  }

  while(true)
  {
    do
    {
      status = my_serial.read(buffer, sizeof(buffer), 50);
      if (!status.success && status.code != ERR_TIMEOUT)
      {
        printf("\r\nERROR from my_serial.read: %s", status.description);
        AP_EXIT();
      }
      bytes_read = my_serial.getBytesRead();
    }while(bytes_read == 0);

    tx_bytes = snprintf((char *)tx_buffer, sizeof(tx_buffer) - 1, "\r\n\r\nRead %u bytes\r\n", bytes_read);

    status = my_serial.write(tx_buffer, tx_bytes);
    if (!status.success)
    {
      printf("\r\nERROR from my_serial.write: %s", status.description);
      AP_EXIT();
    }

    status = my_serial.write(buffer, bytes_read);
    if(!status.success)
    {
      printf("\r\nERROR from my_serial.write: %s", status.description);
      AP_EXIT();
    }
  }

  AP_EXIT();
}