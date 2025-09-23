/**
 * @file echo_blocking.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-22
 *
 * @copyright Copyright (c) 2025
 *
 * @note In this example, a UART port configured by a handle and
 * g_uart_config_list is used to echo whatever is received through it in
 * synchronous (blocking) mode.
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

// static UartBase &g_serial = UART<handle>::getInstance();
static UART<handle> g_serial;
static uint8_t g_rx_buffer[2048] = {0};
static uint8_t g_tx_buffer[100] = {0};
static uint8_t MESSAGE_HELLO_WORLD[] = "\r\nHello world!!!\r\n";


/**
 * @brief Example code that echoes what it receives through a UART port
 */
AP_MAIN()
{
  ErrorCode status;
  SPT timer;
  uint32_t bytes_read = 0, tx_bytes = 0;

  // COnfigure the driver
  status = g_serial.configure(g_uart_config_list, g_uart_config_list_size);
  if (!status)
  {
    std::printf("\r\nERROR from g_serial.configure: %s", status.message().data());
    AP_EXIT();
  }

  // Write a hello message to the uart
  status = g_serial.write({MESSAGE_HELLO_WORLD, std::strlen((char *)MESSAGE_HELLO_WORLD)});
  if (!status)
  {
    std::printf("\r\nERROR from g_serial.write: %s", status.message().data());
    AP_EXIT();
  }

  while(true)
  {
    do
    {
      // Read data from uart by polling
      status = g_serial.read({g_rx_buffer, sizeof(g_rx_buffer)}, 20);
      if (!status && status.value() != static_cast<int>(GenericErrorCode::kTimedOut))
      {
        std::printf("\r\nERROR from g_serial.read: %s", status.message().data());
        AP_EXIT();
      }
      bytes_read = g_serial.getBytesRead();
    }while(bytes_read == 0);

    // Write to the uart the number of bytes received
    tx_bytes = std::snprintf((char *)g_tx_buffer, sizeof(g_tx_buffer) - 1, "\r\n\r\nRead %u bytes\r\n", bytes_read);

    status = g_serial.write({g_tx_buffer, tx_bytes});
    if (!status)
    {
      std::printf("\r\nERROR from g_serial.write: %s", status.message().data());
      AP_EXIT();
    }

    // Write to the uart what was previously received
    status = g_serial.write({g_rx_buffer, bytes_read});
    if(!status)
    {
      std::printf("\r\nERROR from g_serial.write: %s", status.message().data());
      AP_EXIT();
    }
  }

  AP_EXIT();
}