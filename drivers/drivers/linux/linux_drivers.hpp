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


#if __has_include("drivers/linux/std_in_out/std_in_out.hpp")
#include "drivers/linux/std_in_out/std_in_out.hpp"
#endif

#if __has_include("drivers/linux/uart/uart.hpp")
#include "drivers/linux/uart/uart.hpp"
#endif

#if __has_include("drivers/linux/iic/iic.hpp")
#include "drivers/linux/iic/iic.hpp"
#endif

#if __has_include("drivers/linux/spi/spi.hpp")
#include "drivers/linux/spi/spi.hpp"
#endif

#if __has_include("drivers/linux/dio/dio.hpp")
#include "drivers/linux/dio/dio.hpp"
#endif

#if __has_include("drivers/linux/spt/drv_spt.hpp")
#include "drivers/linux/spt/drv_spt.hpp"
#endif


#endif /* LINUX_DRIVERS_HPP_ */