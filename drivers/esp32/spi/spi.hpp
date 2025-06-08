/**
 * @file spi.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-05-31
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef SPI_SPI_HPP
#define SPI_SPI_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"

#include "peripherals_base/spi_base.hpp"
#include "esp32/dio/dio.hpp"

typedef uint32_t SpiHandle_t;

/**
 * @brief Base class for spi drivers
 */
class SPI : public SpiBase
{
public:

  SPI(const SpiHandle_t handle, DIO &cs, bool cs_active_state);
  ~SPI();

  Status_t configure(const SettingsList_t *list, uint8_t list_size);

  Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  Status_t write(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  Status_t transfer(uint8_t *rx_data, uint8_t *tx_data, Size_t byte_count, uint32_t timeout = UINT32_MAX);
  Status_t transfer(Buffer_t rx_data, Buffer_t tx_data, uint32_t timeout = UINT32_MAX);

  Status_t setCallback(EventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

private:
  SpiHandle_t m_handle;
  spi_device_handle_t m_esp_handle;
  bool m_cs_active_state;
  DIO &m_cs;

  Status_t xSpiXfer(uint8_t *txBuf, uint8_t *rxBuf, uint32_t byte_count);

  static void cs_select(spi_transaction_t* t);

  static void cs_unselect(spi_transaction_t* t);
};

#endif /* SPI_SPI_HPP */
