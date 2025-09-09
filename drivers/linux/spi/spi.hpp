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

#include "linux/spi/spi_bus.hpp"

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

#endif /* DRIVERS_LINUX_SPI_SPI_HPP */