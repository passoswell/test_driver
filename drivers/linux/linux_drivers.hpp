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


#if __has_include("linux/std_in_out/std_in_out.hpp")
#include "linux/std_in_out/std_in_out.hpp"
#endif

#if __has_include("linux/uart/uart.hpp")
#include "linux/uart/uart.hpp"
#endif

#if __has_include("linux/iic/iic.hpp")
#include "linux/iic/iic.hpp"
#endif

#if __has_include("linux/spi/spi.hpp")
#include "linux/spi/spi.hpp"
#endif

#if __has_include("linux/dio/dio.hpp")
#include "linux/dio/dio.hpp"
#endif

#if __has_include("linux/spt/spt.hpp")
#include "linux/spt/spt.hpp"
#endif

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


#endif /* LINUX_DRIVERS_HPP_ */