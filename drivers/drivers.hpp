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


#if defined(USE_SILABS) || __has_include("sl_status.h")
#include "silabs/silabs_drivers.hpp"
#endif

#if defined(USE_STM32)
#include "stm32/stm32_drivers.hpp"
#endif

#if defined(USE_ESP32)
#include "esp32/esp32_drivers.hpp"
#endif

#if defined(USE_PIPICO)
#include "pipico/pipico_drivers.hpp"
#endif

#if defined(USE_LINUX)
#include "linux/linux_drivers.hpp"
#endif


#endif /* DRIVERS_HPP_ */
