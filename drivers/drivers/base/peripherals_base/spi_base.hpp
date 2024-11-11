/**
 * @file spi_base.hpp
 * @author your name (you@domain.com)
 * @brief Interface class for SPI
 * @version 0.1
 * @date 2024-10-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef SPI_BASE_HPP
#define SPI_BASE_HPP


#include <stdio.h>
#include <stdbool.h>

#include "drivers/base/driver_base/driver_inout_base.hpp"

/**
 * @brief Base class for spi drivers
 */
class SpiBase : public DriverInOutBase
{
public:

  virtual Status_t configure(const DriverSettings_t *list, uint8_t list_size) = 0;

  using DriverInOutBase::read;
  virtual Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX) = 0;

  using DriverInOutBase::write;
  virtual Status_t write(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX) = 0;

  virtual Status_t transfer(uint8_t *rx_data, const uint8_t *tx_data, Size_t byte_count, uint32_t timeout = UINT32_MAX) = 0;
  virtual Status_t transfer(Buffer_t rx_data, const Buffer_t *tx_data, uint32_t timeout = UINT32_MAX) = 0;

  virtual Status_t setCallback(DriverEventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr) = 0;
};

#endif /* SPI_BASE_HPP */