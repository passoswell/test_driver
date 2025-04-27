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


#if __has_include("esp32/uart/uart.hpp")
#include "esp32/uart/uart.hpp"
#endif

#if __has_include("esp32/iic/iic.hpp")
#include "esp32/iic/iic.hpp"
#endif

#if __has_include("esp32/spi/spi.hpp")
#include "esp32/spi/spi.hpp"
#endif

#if __has_include("esp32/dio/dio.hpp")
#include "esp32/dio/dio.hpp"
#endif

#if __has_include("esp32/spt/spt.hpp")
#include "esp32/spt/spt.hpp"
#endif

#if __has_include("esp32/pwm/pwm.hpp")
#include "esp32/pwm/pwm.hpp"
#endif

#if __has_include("esp32/adc/adc.hpp")
#include "esp32/adc/adc.hpp"
#endif

#if __has_include("esp32/dac/dac.hpp")
#include "esp32/dac/dac.hpp"
#endif

#if __has_include("esp32/mem/mem.hpp")
#include "esp32/mem/mem.hpp"
#endif

#if __has_include("esp32/rtc/rtc.hpp")
#include "esp32/rtc/rtc.hpp"
#endif

#include "task_system/task_system.hpp"

#ifndef AP_MAIN
#define AP_MAIN() \
    extern "C" void app_main(void)
#endif /* AP_MAIN() */

#ifndef AP_MAIN_ARDUINO
#define AP_MAIN_ARDUINO()           \
    extern "C" void app_main(void)  \
    {                               \
      setup();                      \
      while(true)                   \
      {                             \
        loop();                     \
      }                             \
    }
#endif /* AP_MAIN_ARDUINO() */

#ifndef AP_EXIT
#define AP_EXIT()             \
    do                        \
    {                         \
      vTaskDelay(1);          \
    }while(true);
#endif /* AP_EXIT */


#endif /* ESP32_DRIVERS_HPP_ */