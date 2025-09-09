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

#include "esp32/spi/spi_bus.hpp"

template<SpiHandle_t PORT_NUMBER>
class SPI final: public bSPI
{
public:

  SPI(iDIO &cs_pin, bool cs_active_state) : bSPI(IicBus<PORT_NUMBER>::getInstance(), cs_pin, cs_active_state)
  {
    ;
  }

  ~SPI() = default;
};

#endif /* SPI_SPI_HPP */
