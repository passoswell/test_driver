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


#if __has_include("drivers/pipico/uart/uart.hpp")
#include "drivers/pipico/uart/uart.hpp"
#endif

#if __has_include("drivers/pipico/iic/iic.hpp")
#include "drivers/pipico/iic/iic.hpp"
#endif

#if __has_include("drivers/pipico/spi/spi.hpp")
#include "drivers/pipico/spi/spi.hpp"
#endif

#if __has_include("drivers/pipico/dio/dio.hpp")
#include "drivers/pipico/dio/dio.hpp"
#endif

#if __has_include("drivers/pipico/spt/drv_spt.hpp")
#include "drivers/pipico/spt/drv_spt.hpp"
#endif


#endif /* PIPICO_DRIVERS_HPP_ */