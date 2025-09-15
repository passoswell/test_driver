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
  ADD_PARAMETER(COMM_WORK_ASYNC_RX, true),
  ADD_PARAMETER(COMM_WORK_ASYNC_TX, true),
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
  ADD_PARAMETER(COMM_WORK_ASYNC_RX, true),
  ADD_PARAMETER(COMM_WORK_ASYNC_TX, true),
  ADD_PARAMETER(COMM_PARAM_RX_DIO_PIN, 3),
  ADD_PARAMETER(COMM_PARAM_TX_DIO_PIN, 1),
};
const uint8_t g_uart_config_list_size = sizeof(g_uart_config_list)/sizeof(g_uart_config_list[0]);

#else
constexpr void *handle = nullptr;
#endif


static ErrorCode rxCallback(ErrorCode status, EventsList_t event, const Buffer_t data, void *user_arg);

static ErrorCode txCallback(ErrorCode status, EventsList_t event, const Buffer_t data, void *user_arg);

// static UartBase &g_serial = UART<handle>::getInstance();
static UART<handle> g_serial;
static uint8_t g_rx_buffer[2048] = {0};
static uint8_t g_tx_buffer[100] = {0};
static uint8_t MESSAGE_HELLO_WORLD[] = "\r\nHello world!!!\r\n";
bool g_error_flag = false;

/**
 * @brief Callback class example for dio events
 */
class UartEventHandler final : public iCallback
{
public:
  UartEventHandler() = default;

  ~UartEventHandler() = default;

  // Returns a short string identifying the callback owner
  std::string name() override { return "Uart callback test";}

  // Called when the asynchronous operation is about to start
  void onStart() override {}

  void onEvent(ErrorCode status, EventsList_t event, const Buffer_t data) override
  {
    switch (event)
    {
    case EVENT_READ:
      (void) rxCallback(status, event, data, nullptr);
      break;
    case EVENT_WRITE:
      (void) txCallback(status, event, data, nullptr);
      break;
    default:
      break;
    }
  }

  // Called when an event occur if applicable
  void onEvent(ErrorCode status, EventsList_t event, const Buffer_t rx_data, const Buffer_t tx_data) override {}

private:
  uint32_t m_port, m_pin;
};

/**
 * @brief Example code that echoes what it receives through a UART port using interruption
 */
AP_MAIN()
{
  ErrorCode status;
  SPT timer;
  uint32_t bytes_read = 0, tx_bytes = 0;
  UartEventHandler uart_event_handler;

  timer.delay(1000);

  // Configure the driver
  status = g_serial.configure(g_uart_config_list, g_uart_config_list_size);
  if (!status)
  {
    std::printf("\r\nERROR from g_serial.configure: %s", status.message().data());
    AP_EXIT();
  }

  // Install callback functions for uart events
  status = g_serial.setCallback(EVENT_READ, uart_event_handler);
  status = g_serial.setCallback(EVENT_WRITE, uart_event_handler);

  // Write a hello message in async mode
  status = g_serial.write(MESSAGE_HELLO_WORLD, std::strlen((char *)MESSAGE_HELLO_WORLD));
  if (!status)
  {
    std::printf("\r\nERROR from g_serial.write: %s", status.message().data());
    AP_EXIT();
  }

  // Start the async read operation
  status = g_serial.read(g_rx_buffer, 20);
  if (!status && status.value() != static_cast<int>(UartErrorCode::kTimedOut))
  {
    std::printf("\r\nERROR from g_serial.read: %s", status.message().data());
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
 * @return ErrorCode
 */
ErrorCode rxCallback(ErrorCode status, EventsList_t event, const Buffer_t data, void *user_arg)
{
  SPT timer;
  static uint32_t counter = 0;
  if(status)
  {
    std::printf("\r\n\r\n[%03u] From reception callback: %lu bytes received\r\n", counter, data.size_bytes());

    // Wait any ongoing transmission to finish
    // Write to the uart the data received
    do
    {
      status = g_serial.write({g_rx_buffer, data.size_bytes()});
      if(!status && status.value() != static_cast<int>(UartErrorCode::kBusy))
      {
        std::printf("\r\nERROR from g_serial.write: %s", status.message().data());
        g_error_flag = true;
        break;
      }
    } while( status.value() == static_cast<int>(UartErrorCode::kBusy) );


    // Start a new async read operation
    status = g_serial.read({g_rx_buffer, sizeof(g_rx_buffer)}, 20);
    if (!status)
    {
      std::printf("\r\nERROR from g_serial.read: %s", status.message().data());
      g_error_flag = true;
    }

  }else
  {
    std::printf("\r\n\r\n[%03u] From reception callback: ended in failure: %s\r\n", counter, status.message().data());
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
 * @return ErrorCode
 */
ErrorCode txCallback(ErrorCode status, EventsList_t event, const Buffer_t data, void *user_arg)
{
  static uint32_t counter = 0;
  if(status)
  {
    std::printf("\r\n\r\n[%03u] From transmission callback:  %lu bytes transmitted\r\n", counter, data.size_bytes());
  }else
  {
    std::printf("\r\n\r\n[%03u] From transmission callback: ended in failure: %s\r\n", counter, status.message().data());
  }
  counter++;
  return status;
}