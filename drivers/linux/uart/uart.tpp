/**
 * @file uart.cpp
 * @author your name (you@domain.com)
 * @brief Give access to UART functionalities
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2024
 *
 */

// #include "linux/uart/uart.hpp"

// #include <termios.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <sys/ioctl.h>
// #include <stdio.h>

// #include "linux/utils/linux_io.hpp"

// static speed_t convertSpeed2(uint32_t speed);

// /**
//  * @brief Constructor
//  */
// template<UartHandle_t PORT_NUMBER>
// UART<PORT_NUMBER>::UART() : m_bus(UartBus<PORT_NUMBER>::getInstance())
// {
//   ;
// }

// /**
//  * @brief Configure a list of parameters
//  *
//  * @param list List of parameter-value pairs
//  * @param list_size Number of parameters on the list
//  * @return Status_t
//  */
// template<UartHandle_t PORT_NUMBER>
// Status_t UART<PORT_NUMBER>::configure(const SettingsList_t *list, uint8_t list_size)
// {
//   return m_bus.configure(list, list_size);
// }

// /**
//  * @brief Read data
//  *
//  * @param data Buffer to store the data
//  * @param byte_count Number of bytes to read
//  * @param timeout Time to wait in milliseconds before returning an error
//  * @return Status_t
//  */
// template <UartHandle_t PORT_NUMBER>
// Status_t UART<PORT_NUMBER>::read(DrvBuffer_t data, uint32_t timeout)
// {
//   return m_bus.read(data, timeout, m_func_rx, m_arg_rx);
// }

// /**
//  * @brief Write data
//  *
//  * @param data Buffer where data is stored
//  * @param byte_count Number of bytes to write
//  * @param timeout Time to wait in milliseconds before returning an error
//  * @return Status_t
//  */
// template<UartHandle_t PORT_NUMBER>
// Status_t UART<PORT_NUMBER>::write(DrvBuffer_t data, uint32_t timeout)
// {
//   return m_bus.write(data, timeout, m_func_tx, m_arg_tx);
// }

// /**
//  * @brief Install a callback function
//  *
//  * @param event An event to trigger the call
//  * @param function A function to call back
//  * @param user_arg A argument used as a parameter to the function
//  * @return Status_t
//  */
// template<UartHandle_t PORT_NUMBER>
// Status_t UART<PORT_NUMBER>::setCallback(EventsList_t event, DriverCallback_t function, void *user_arg)
// {
//   Status_t status = STATUS_DRV_SUCCESS;

//   switch (event)
//   {
//   case EVENT_READ:
//     if (m_read_status.code != OPERATION_RUNNING)
//     {
//       m_func_rx = function;
//       m_arg_rx = user_arg;
//     }
//     else
//     {
//       status = STATUS_DRV_ERR_BUSY;
//     }
//     break;
//   case EVENT_WRITE:
//     if (m_write_status.code != OPERATION_RUNNING)
//     {
//       m_func_tx = function;
//       m_arg_tx = user_arg;
//     }
//     else
//     {
//       status = STATUS_DRV_ERR_BUSY;
//     }
//     break;
//   default:
//     status = STATUS_DRV_ERR_PARAM;
//     break;
//   }

//   return status;
// }