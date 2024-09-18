/**
 * @file esp32_drivers.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-10
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef ESP32_DRIVERS_HPP_
#define ESP32_DRIVERS_HPP_


#if __has_include("drivers/esp32/uart/drv_uart.hpp")
#include "drivers/esp32/uart/drv_uart.hpp"
#endif

#if __has_include("drivers/esp32/i2c/drv_i2c.hpp")
#include "drivers/esp32/i2c/drv_i2c.hpp"
#endif

#if __has_include("drivers/esp32/spi/drv_spi.hpp")
#include "drivers/esp32/spi/drv_spi.hpp"
#endif

#if __has_include("drivers/esp32/dio/drv_dio.hpp")
#include "drivers/esp32/dio/drv_dio.hpp"
#endif

#if __has_include("drivers/esp32/spt/drv_spt.hpp")
#include "drivers/esp32/spt/drv_spt.hpp"
#endif


#endif /* ESP32_DRIVERS_HPP_ */