/**
 * @file uart.hpp
 * @author your name (you@domain.com)
 * @brief Give access to UART functionalities
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef UART_UART_HPP
#define UART_UART_HPP


#include <stdio.h>
#include <stdbool.h>

#include "peripherals_base/uart_base.hpp"
#include "esp32/task_system/task_system.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


typedef struct
{
  uint8_t uart_number;
  uint16_t rx_pin;
  uint16_t tx_pin;
} UartHandle_t;


/**
 * @brief Class that implements UART communication
 */
class UART : public UartBase
{
public:
  UART(const UartHandle_t port_handle);

  ~UART();

  Status_t configure(const SettingsList_t *list, uint8_t list_size);

  using UartBase::read;
  Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  using UartBase::write;
  Status_t write(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  Status_t setCallback(EventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

private:
  UartHandle_t m_handle;
  bool m_terminate;
  QueueHandle_t m_event_queue;
  TaskHandle_t m_event_task_handle;
  DataBundle_t m_data;
  Mutex m_rx_mutex, m_tx_mutex;
  Task<uint8_t, 1, Status_t, 1> m_tx_monitor_task_handle;

  Status_t checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout);

  void rxEventTask(void);

  Status_t txMonitorTask(uint8_t data);
};


#endif /* UART_UART_HPP */
