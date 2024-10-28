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


#if __has_include("drivers/esp32/uart/uart.hpp")
#include "drivers/esp32/uart/uart.hpp"
#endif

#if __has_include("drivers/esp32/iic/iic.hpp")
#include "drivers/esp32/i2c/iic.hpp"
#endif

#if __has_include("drivers/esp32/spi/spi.hpp")
#include "drivers/esp32/spi/spi.hpp"
#endif

#if __has_include("drivers/esp32/dio/dio.hpp")
#include "drivers/esp32/dio/dio.hpp"
#endif

#if __has_include("drivers/esp32/spt/drv_spt.hpp")
#include "drivers/esp32/spt/drv_spt.hpp"
#endif


#endif /* ESP32_DRIVERS_HPP_ */