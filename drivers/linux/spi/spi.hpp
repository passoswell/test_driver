/**
 * @file spi.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-11-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVERS_LINUX_SPI_SPI_HPP
#define DRIVERS_LINUX_SPI_SPI_HPP

#include "peripherals_base/spi_base.hpp"
#include "linux/utils/linux_types.hpp"
#include "linux/utils/linux_threads.hpp"

/**
 * @brief Base class for spi drivers
 */
class SPI final : public SpiBase
{
public:
  SPI(const void *port_handle);

  ~SPI();

  Status_t configure(const DriverSettings_t *list, uint8_t list_size);

  using DriverInOutBase::read;
  Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  using DriverInOutBase::write;
  Status_t write(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  Status_t transfer(uint8_t *rx_data, uint8_t *tx_data, Size_t byte_count, uint32_t timeout = UINT32_MAX);
  Status_t transfer(Buffer_t rx_data, Buffer_t tx_data, uint32_t timeout = UINT32_MAX);

  Status_t setCallback(DriverEventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

private:
  LinuxThreads<DataBundle_t, Status_t, SPI_QUEUE_SIZE, 0> m_thread_handle;
  int m_linux_handle;
  uint32_t m_speed;

  Status_t xSpiXfer(uint8_t *txBuf, uint8_t *rxBuf, uint32_t byte_count);

  Status_t checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout);

  static Status_t transferDataAsync(DataBundle_t data_bundle, void *self_ptr);
};

#endif /* DRIVERS_LINUX_SPI_SPI_HPP */