/**
 * @file drv_uart.hpp
 * @author Wellington Passos (contact@email.com)
 * @brief Give access to UART functionalities
 * @date 2023-11-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef DRIVERS_LINUX_UART_UART_HPP
#define DRIVERS_LINUX_UART_UART_HPP


#include <stdio.h>
#include <stdbool.h>

#include "peripherals_base/uart_base.hpp"
#include "linux/utils/linux_types.hpp"
#include "linux/task_system/task_system.hpp"


typedef uint16_t UartHandle_t;


/**
 * @brief Class that implements UART communication
 */
class UART : public UartBase
{
public:
  UART(const UartHandle_t port_number);

  ~UART();

  Status_t configure(const SettingsList_t *list, uint8_t list_size);

  using UartBase::read;
  Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  using UartBase::write;
  Status_t write(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  Status_t setCallback(EventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

protected:
  UartHandle_t m_port_number;
  Task<DataBundle_t, UART_QUEUE_SIZE, Status_t, 0> m_rx_thread_handle;
  Task<DataBundle_t, UART_QUEUE_SIZE, Status_t, 0> m_tx_thread_handle;
  int m_fd;
  bool m_terminate;

  Status_t readBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back);
  static Status_t readFromThreadBlocking(DataBundle_t data_bundle, void *user_arg);

  Status_t writeBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back);
  static Status_t writeFromThreadBlocking(DataBundle_t data_bundle, void *user_arg);

  Status_t checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout);
};


#endif /* DRIVERS_LINUX_UART_UART_HPP */