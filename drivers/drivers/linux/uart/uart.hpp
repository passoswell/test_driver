/**
 * @file drv_uart.hpp
 * @author Wellington Passos (contact@email.com)
 * @brief Give access to UART functionalities
 * @date 2023-11-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef UART_HPP
#define UART_HPP


#include <stdio.h>
#include <stdbool.h>

#include "drivers/base/peripherals_base/uart_base.hpp"
#include "drivers/linux/utils/linux_types.hpp"


/**
 * @brief Class that implements UART communication
 */
class UART final : public UartBase
{
public:
  UART(void *port_handle);

  ~UART();

  Status_t configure(const DriverSettings_t *list, uint8_t list_size);

  using UartBase::read;
  Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  using UartBase::write;
  Status_t write(const uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  Status_t setCallback(DriverEventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

private:
  int m_linux_handle;
  UtilsInOutSync_t m_sync_rx, m_sync_tx;
  bool m_terminate;

  Status_t readBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout);
  void readAsyncThread(void);

  Status_t writeBlocking(const uint8_t *data, Size_t byte_count, uint32_t timeout);
  void writeAsyncThread(void);

  Status_t checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout);
};


#endif /* UART_HPP */