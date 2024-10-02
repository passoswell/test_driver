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
#include "drivers/linux/dio/drv_dio.hpp"


/**
 * @brief Class that implements UART communication
 */
class DrvUART final : public DrvCommBase
{
public:
  DrvUART(void *port_handle, DrvDIO *read_write_dio = nullptr, bool dio_read_state = false);

  ~DrvUART();

  Status_t configure(const InOutStreamSettings_t *list, uint8_t list_size);

  using DrvCommBase::read;
  Status_t read(uint8_t *buffer, uint32_t size, uint32_t key = INOUTSTREAM_MASTER_KEY, uint32_t timeout = UINT32_MAX);

  using DrvCommBase::write;
  Status_t write(const uint8_t *buffer, uint32_t size, uint32_t key = INOUTSTREAM_MASTER_KEY, uint32_t timeout = UINT32_MAX);

  using DrvCommBase::readAsync;
  Status_t readAsync(uint8_t *buffer, uint32_t size, uint32_t key = INOUTSTREAM_MASTER_KEY, uint32_t timeout = UINT32_MAX, InOutStreamCallback_t func = nullptr, void *arg = nullptr);
  bool isReadAsyncDone();

  using DrvCommBase::writeAsync;
  Status_t writeAsync(const uint8_t *buffer, uint32_t size, uint32_t key = INOUTSTREAM_MASTER_KEY, uint32_t timeout = UINT32_MAX, InOutStreamCallback_t func = nullptr, void *arg = nullptr);
  bool isWriteAsyncDone();

private:
  int m_linux_handle;
  UtilsInOutSync_t m_sync_rx, m_sync_tx;
  bool m_terminate;
  DrvDIO *m_read_write_dio;
  bool m_dio_read_state;
  uint32_t m_dio_delay_us;

  void readAsyncThread(void);

  void writeAsyncThread(void);

  Status_t checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout, uint32_t key);
};


#endif /* DRV_UART_HPP_ */


#endif