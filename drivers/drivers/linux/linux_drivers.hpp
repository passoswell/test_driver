/**
 * @file linux_drivers.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef LINUX_DRIVERS_HPP_
#define LINUX_DRIVERS_HPP_


#if __has_include("drivers/linux/std_in_out/drv_std_in_out.hpp")
#include "drivers/linux/std_in_out/drv_std_in_out.hpp"
#endif

#if __has_include("drivers/linux/uart/drv_uart.hpp")
#include "drivers/linux/uart/drv_uart.hpp"
#endif

#if __has_include("drivers/linux/i2c/drv_i2c.hpp")
#include "drivers/linux/i2c/drv_i2c.hpp"
#endif

#if __has_include("drivers/linux/spi/drv_spi.hpp")
#include "drivers/linux/spi/drv_spi.hpp"
#endif

#if __has_include("drivers/linux/dio/drv_dio.hpp")
#include "drivers/linux/dio/drv_dio.hpp"
#endif

#if __has_include("drivers/linux/spt/drv_spt.hpp")
#include "drivers/linux/spt/drv_spt.hpp"
#endif


#endif /* LINUX_DRIVERS_HPP_ */