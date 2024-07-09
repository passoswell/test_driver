/**
 * @file hello_world.cpp
 * @author your name (you@domain.com)
 * @brief Example code on how to write to a uart port
 * @version 0.1
 * @date 2024-07-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "drivers.hpp"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#if defined(USE_LINUX)
#include <iostream>
void *handle = (void *) "/dev/ttyUSB0";
#endif


void printErrorMessage(Status_t status);


DrvUART serial(handle);


const InOutStreamConfigure_t g_uart_config_list[]
{
  ADD_PARAMETER(DRV_PARAM_BAUD, 115200),
  ADD_PARAMETER(DRV_PARAM_LINE_MODE, 0), /*!< no parity, one stop bit, no hw flow control*/
};
const uint32_t g_uart_config_list_size = sizeof(g_uart_config_list) / sizeof(InOutStreamConfigure_t);

/**
 * @brief Example code that prints a Hello World message on a uart port
 *
 * @return int
 */
int main(void)
{
  uint8_t message[] = "Hello world!!!\r\n";
  Status_t status;

  status = serial.configure(g_uart_config_list, g_uart_config_list_size);
  if (!status.success)
  {
    printErrorMessage(status);
    return -1;
  }

  while(true)
  {
    status = serial.write(message, strlen((char *)message));
    if(!status.success)
    {
      printErrorMessage(status);
      return -1;
    }
  }
  return 0;
}

/**
 * @brief Prints a message on the linux console
 *
 * @param status
 */
void printErrorMessage(Status_t status)
{
#if defined(USE_LINUX)
  std::cout << status.description << std::endl;
#endif
}