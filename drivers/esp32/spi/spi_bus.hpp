/**
 * @file spi_bus.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-08-23
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef SPI_SPI_BUS_HPP
#define SPI_SPI_BUS_HPP

#include "peripherals_base/spi_interface.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"

typedef uint16_t SpiHandle_t;

/**
 * @brief SPI bus class for ESP32
 */
template<SpiHandle_t PORT_NUMBER>
class SpiBus : public iSpiBus
{
public:
  static iSpiBus& getInstance()
  {
    static SpiBus<PORT_NUMBER> instance;
    return instance;
  }

  Status_t configure(const SettingsList_t *list, uint8_t list_size) override;

  Status_t read(iDIO &cs_pin, bool cs_active_state, Buffer_t data, uint32_t timeout, Callback_t cb_function = nullptr, void *cb_arg = nullptr) override;

  Status_t write(iDIO &cs_pin, bool cs_active_state, Buffer_t data, uint32_t timeout, Callback_t cb_function = nullptr, void *cb_arg = nullptr) override;

  // A singleton should not be cloneable nor assignable
  SpiBus(const SpiBus&) = delete;
  SpiBus(SpiBus&&) = delete;
  SpiBus& operator=(const SpiBus&) = delete;
  SpiBus& operator=(SpiBus&&) = delete;

private:
  bool m_is_async_mode_rx, m_is_async_mode_tx;
  bool m_is_configured;
  spi_device_handle_t m_esp_handle;
  iDIO *m_cs;
  bool m_cs_active_state;

  SpiBus() = default;

  ~SpiBus() = default;

  Status_t checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout);

  Status_t xSpiXfer(uint8_t *txBuf, uint8_t *rxBuf, uint32_t byte_count);

  static void cs_select(spi_transaction_t* t);

  static void cs_unselect(spi_transaction_t* t);
};

#include "spi_bus.tpp"

#endif /* SPI_SPI_BUS_HPP */
