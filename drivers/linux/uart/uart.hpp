/**
 * @file drv_uart.hpp
 * @author Wellington Passos (contact@email.com)
 * @brief Give access to UART functionalities
 * @date 2023-11-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef DRIVERS_LINUX_UART_UART_HPP
#define DRIVERS_LINUX_UART_UART_HPP


#include <stdio.h>
#include <stdbool.h>

#include "linux/uart/uart_bus.hpp"

template<UartHandle_t PORT_NUMBER>
class UART final: public bUART
{
public:

  UART(iDIO *rs485_pin = nullptr) : bUART(UartBus<PORT_NUMBER>::getInstance(), *rs485_pin)
  {
  }

  ~UART() = default;
};

#endif /* DRIVERS_LINUX_UART_UART_HPP */