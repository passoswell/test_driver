/**
 * @file drv_uart.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "drv_uart.hpp"

/**
 * @brief Constructor
 * @param port_handle A string containing the path to the peripheral
 */
DrvUART::DrvUART(void *port_handle)
{
  (void) port_handle;
}

/**
 * @brief Destructor
 */
DrvUART::~DrvUART()
{
  // Nothing is done here
}
