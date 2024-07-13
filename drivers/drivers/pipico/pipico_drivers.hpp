/**
 * @file pipico_drivers.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-10
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef PIPICO_DRIVERS_HPP_
#define PIPICO_DRIVERS_HPP_


#if __has_include("drivers/pipico/uart/drv_uart.hpp")
#include "drivers/pipico/uart/drv_uart.hpp"
#endif

#if __has_include("drivers/pipico/i2c/drv_i2c.hpp")
#include "drivers/pipico/i2c/drv_i2c.hpp"
#endif

#if __has_include("drivers/pipico/spi/drv_spi.hpp")
#include "drivers/pipico/spi/drv_spi.hpp"
#endif

#if __has_include("drivers/pipico/gpio/drv_gpio.hpp")
#include "drivers/pipico/gpio/drv_gpio.hpp"
#endif


#endif /* PIPICO_DRIVERS_HPP_ */