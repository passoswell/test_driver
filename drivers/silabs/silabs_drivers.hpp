/**
 * @file drivers.hpp
 * @author user (contact@email.com)
 * @brief Insert a brief description here
 * @date 2024-06-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef SILABS_DRIVERS_HPP_
#define SILABS_DRIVERS_HPP_


#if __has_include("drivers/silabs/uart/drv_uart.hpp")
#include "drivers/silabs/uart/drv_uart.hpp"
#endif

#if __has_include("drivers/silabs/i2c/drv_i2c.hpp")
#include "drivers/silabs/i2c/drv_i2c.hpp"
#endif

#if __has_include("drivers/silabs/pit/drv_pit.hpp")
#include "drivers/silabs/pit/drv_pit.hpp"
#endif


#endif /* SILABS_DRIVERS_HPP_ */
