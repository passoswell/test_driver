/**
 * @file uart_bus.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-07-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef UART_UART_BUS_HPP
#define UART_UART_BUS_HPP


#include <stdio.h>
#include <stdbool.h>

#include "peripherals_base/uart_interface.hpp"
#include "linux/utils/linux_types.hpp"
#include "linux/task_system/task_system.hpp"
#if __has_include("setup.hpp")
#include "setup.hpp"
#endif

#ifndef UART_QUEUE_SIZE
#define UART_QUEUE_SIZE                                                        1
#endif


/**
 * @brief Class that implements UART communication
 */
template<UartHandle_t PORT_NUMBER>
class UartBus final: public iUartBus
{
public:

  static iUartBus& getInstance()
  {
    static UartBus<PORT_NUMBER> instance;
    return instance;
  }

  ErrorCode configure(const SettingsList_t *list, uint8_t list_size) override;

  ErrorCode read(iDIO &rs485_pin, Buffer_t data, uint32_t timeout, iCallback &event_handler) override;

  ErrorCode write(iDIO &rs485_pin, Buffer_t data, uint32_t timeout, iCallback &event_handler) override;

  uint32_t getBytesAvailable() override;

  uint32_t getBytesRead() override;

  // A singleton should not be cloneable nor assignable
  UartBus(const UartBus&) = delete;
  UartBus(UartBus&&) = delete;
  UartBus& operator=(const UartBus&) = delete;
  UartBus& operator=(UartBus&&) = delete;

protected:
  Task<UartDataBundle_t, UART_QUEUE_SIZE, ErrorCode, 0> m_rx_thread_handle;
  Task<UartDataBundle_t, UART_QUEUE_SIZE, ErrorCode, 0> m_tx_thread_handle;
  Mutex m_rx_mutex, m_tx_mutex;
  bool m_is_async_mode_rx, m_is_async_mode_tx;
  bool m_is_configured;
  int m_fd;
  uint32_t m_bytes_read;

  UartBus();

  virtual ~UartBus();

  ErrorCode blockingRead(Buffer_t data, uint32_t timeout, bool use_idle_line_detection);

  static ErrorCode asyncReadThread(UartDataBundle_t data_bundle, void *user_arg);

  ErrorCode blockingWrite(Buffer_t data, uint32_t timeout, bool wait_end_of_transmission);

  static ErrorCode asyncWriteThread(UartDataBundle_t data_bundle, void *user_arg);

  ErrorCode checkInputs(const Buffer_t data, uint32_t timeout);
};

#include "uart_bus.tpp"


#endif /* UART_UART_BUS_HPP */
