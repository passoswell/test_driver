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
#include "linux/utils/linux_threads.hpp"


/**
 * @brief Class that implements UART communication
 */
class UART final : public UartBase
{
public:
  UART(const void *port_handle);

  ~UART();

  Status_t configure(const DriverSettings_t *list, uint8_t list_size);

  using UartBase::read;
  Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  using UartBase::write;
  Status_t write(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  Status_t setCallback(DriverEventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

private:
  LinuxThreads<DataBundle_t, Status_t, UART_QUEUE_SIZE, 0> m_rx_thread_handle;
  LinuxThreads<DataBundle_t, Status_t, UART_QUEUE_SIZE, 0> m_tx_thread_handle;
  int m_linux_handle;
  bool m_terminate;

  Status_t readBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back);
  static Status_t readFromThreadBlocking(DataBundle_t data_bundle, void *user_arg);

  Status_t writeBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back);
  static Status_t writeFromThreadBlocking(DataBundle_t data_bundle, void *user_arg);

  Status_t checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout);
};


#endif /* DRIVERS_LINUX_UART_UART_HPP */