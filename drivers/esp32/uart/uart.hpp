/**
 * @file uart.hpp
 * @author your name (you@domain.com)
 * @brief Give access to UART functionalities
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef UART_UART_HPP
#define UART_UART_HPP


#include <stdio.h>
#include <stdbool.h>

#include "esp32/uart/uart_bus.hpp"

template<UartHandle_t PORT_NUMBER>
class UART : public bUART
{
public:

  UART(iDIO *rs485_pin = nullptr) : bUART(UartBus<PORT_NUMBER>::getInstance(), *rs485_pin)
  {
  }
};

#endif /* UART_UART_HPP */
