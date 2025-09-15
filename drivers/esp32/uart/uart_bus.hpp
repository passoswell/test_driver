/**
 * @file uart_bus.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-08-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef UART_UART_BUS_HPP
#define UART_UART_BUS_HPP


#include <cstdio>
#include <cstdbool>

#include "peripherals_base/uart_interface.hpp"
#include "esp32/task_system/task_system.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


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
  UartHandle_t m_handle;
  bool m_terminate;
  QueueHandle_t m_event_queue;
  TaskHandle_t m_event_task_handle;
  Mutex m_rx_mutex, m_tx_mutex;
  bool m_is_async_mode_rx, m_is_async_mode_tx;
  Task<UartDataBundle_t, 1, ErrorCode, 1> m_tx_monitor_task_handle;
  Task<UartDataBundle_t, 1, ErrorCode, 1> m_rx_monitor_task_handle;

  uint32_t m_bytes_read;

  UartBus();

  ~UartBus();

  ErrorCode checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout);

  ErrorCode rxMonitorTask(UartDataBundle_t data_bundle);

  ErrorCode blockingRead(Buffer_t data, uint32_t timeout);

  ErrorCode txMonitorTask(UartDataBundle_t data_bundle);

  ErrorCode blockingWrite(Buffer_t data, uint32_t timeout, bool wait_end_of_transmission);

  void terminateRxEventTask(void);
};

#include "uart_bus.tpp"


#endif /* UART_UART_BUS_HPP */
