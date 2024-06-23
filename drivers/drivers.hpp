/**
 * @file drivers.hpp
 * @author user (contact@email.com)
 * @brief Insert a brief description here
 * @date 2024-06-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVERS_HPP_
#define DRIVERS_HPP_


#include "drivers/base/drv_comm_base.hpp"

#if defined(USE_SILABS) || __has_include("sl_status.h")
#include "drivers/silabs/silabs_drivers.hpp"
#endif

#if defined(USE_STM32)
#include "drivers/stm32/stm32_drivers.hpp"
#endif

#if defined(USE_ESP32)
#include "drivers/esp/esp32_drivers.hpp"
#endif

#if defined(USE_LINUX)
#include "drivers/linux/linux_drivers.hpp"
#endif


#endif /* DRIVERS_HPP_ */
