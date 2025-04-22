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


#if __has_include("pipico/uart/uart.hpp")
#include "pipico/uart/uart.hpp"
#endif

#if __has_include("pipico/iic/iic.hpp")
#include "pipico/iic/iic.hpp"
#endif

#if __has_include("pipico/spi/spi.hpp")
#include "pipico/spi/spi.hpp"
#endif

#if __has_include("pipico/dio/dio.hpp")
#include "pipico/dio/dio.hpp"
#endif

#if __has_include("pipico/spt/spt.hpp")
#include "pipico/spt/spt.hpp"
#endif

#include "pico/stdlib.h"

#ifndef AP_MAIN
#define AP_MAIN() \
    int main(void)
#endif /* AP_MAIN() */

#ifndef AP_MAIN_ARDUINO
#define AP_MAIN_ARDUINO() \
    int main(void)\
    {             \
      setup();    \
      while(true) \
      {           \
        loop();   \
      }           \
    }
#endif /* AP_MAIN_ARDUINO() */

#ifndef AP_EXIT
#define AP_EXIT() return 0;
#endif /* AP_EXIT */


#endif /* PIPICO_DRIVERS_HPP_ */