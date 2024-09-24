/**
 * @file drv_uart.hpp
 * @author Wellington Passos (contact@email.com)
 * @brief Insert a brief description here
 * @date 2023-11-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#if __has_include("termios.h")


#ifndef DRV_UART_HPP_
#define DRV_UART_HPP_


#include <stdio.h>
#include <stdbool.h>

#include "drivers/base/drv_comm_base.hpp"
#include "drivers/linux/utils/linux_types.hpp"


/**
 * @brief Class that implements UART communication
 */
class DrvUART final : public DrvCommBase
{
public:
  DrvUART(void *port_handle);

  ~DrvUART();

  Status_t configure(const InOutStreamSettings_t *list, uint8_t list_size);

  Status_t read(uint8_t *buffer, uint32_t size, uint32_t key = INOUTSTREAM_NO_KEY, uint32_t timeout = 0xFFFFFFFF);

  Status_t write(uint8_t *buffer, uint32_t size, uint32_t key = INOUTSTREAM_NO_KEY, uint32_t timeout = 0xFFFFFFFF);

  Status_t readAsync(uint8_t *buffer, uint32_t size, uint32_t key = INOUTSTREAM_NO_KEY, InOutStreamCallback_t func = nullptr, void *arg = nullptr);
  bool isReadAsyncDone();

  Status_t writeAsync(uint8_t *buffer, uint32_t size, uint32_t key = INOUTSTREAM_NO_KEY, InOutStreamCallback_t func = nullptr, void *arg = nullptr);
  bool isWriteAsyncDone();

private:
  int m_linux_handle;
  UtilsInOutSync_t m_sync_rx, m_sync_tx;
  bool m_terminate;

  void readAsyncThread(void);

  void writeAsyncThread(void);
};


#endif /* DRV_UART_HPP_ */


#endif