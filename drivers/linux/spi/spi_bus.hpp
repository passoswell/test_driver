/**
 * @file spi_bus.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef SPI_SPI_BUS_HPP
#define SPI_SPI_BUS_HPP


#include <cstdio>
#include <cstdbool>

#include "peripherals_base/spi_interface.hpp"
#include "linux/utils/linux_types.hpp"
#include "linux/task_system/task_system.hpp"
#if __has_include("setup.hpp")
#include "setup.hpp"
#endif

#ifndef SPI_QUEUE_SIZE
#define SPI_QUEUE_SIZE                                                         1
#endif

template<SpiHandle_t PORT_NUMBER>
class SpiBus final: public iSpiBus
{
public:

  static iSpiBus& getInstance()
  {
    static SpiBus<PORT_NUMBER> instance;
    return instance;
  }

  ErrorCode configure(const SettingsList_t *list, uint8_t list_size) override;

  ErrorCode read(iDIO &cs_pin, bool cs_active_state, Buffer_t data, uint32_t timeout, iCallback &event_handler) override;

  ErrorCode write(iDIO &cs_pin, bool cs_active_state, Buffer_t data, uint32_t timeout, iCallback &event_handler) override;

  // A singleton should not be cloneable nor assignable
  SpiBus(const SpiBus&) = delete;
  SpiBus(SpiBus&&) = delete;
  SpiBus& operator=(const SpiBus&) = delete;
  SpiBus& operator=(SpiBus&&) = delete;

private:
  Mutex m_mutex;
  Task<SpiDataBundle_t, SPI_QUEUE_SIZE, ErrorCode, 0> m_thread_handle;
  int m_fd;
  bool m_is_async_mode_rx, m_is_async_mode_tx;
  bool m_is_configured;
  uint32_t m_speed;

  SpiBus();

  ~SpiBus();

  ErrorCode blockingTransfer(uint8_t *txBuf, uint8_t *rxBuf, uint32_t byte_count);

  static ErrorCode asyncTransferThread(SpiDataBundle_t data_bundle, void *user_arg);

  ErrorCode checkInputs(const DrvBuffer_t data, uint32_t timeout);
};

#include "spi_bus.tpp"


#endif /* SPI_SPI_BUS_HPP */
