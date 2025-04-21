/**
 * @file uart.cpp
 * @author your name (you@domain.com)
 * @brief Give access to UART functionalities
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "uart.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"

/**
 * @brief Constructor
 *
 * @param port_handle Pointer to a handle, not used by this driver
 */
UART::UART(const void *port_handle)
{
  m_handle = port_handle;
}

/**
 * @brief Destroyer
 */
UART::~UART()
{

}

Status_t UART::configure(const DriverSettings_t *list, uint8_t list_size)
{
  Status_t status;
  uart_config_t uart_config;

  // if(m_handle == nullptr) { return STATUS_DRV_NULL_POINTER;}
  m_read_status = STATUS_DRV_NOT_CONFIGURED;
  m_write_status = STATUS_DRV_NOT_CONFIGURED;

  uart_config.baud_rate = 115200;
  uart_config.data_bits = UART_DATA_8_BITS;
  uart_config.parity = UART_PARITY_DISABLE;
  uart_config.stop_bits = UART_STOP_BITS_1;
  uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
  uart_config.source_clk = UART_SCLK_DEFAULT;
  uart_config.rx_flow_ctrl_thresh = 0;//UART_HW_FIFO_LEN(uart_num) - 1;

  // if(list != nullptr && list_size != 0)
  // {
  //   for(uint8_t i = 0; i < list_size; i++)
  //   {
  //     switch(list[i].parameter)
  //     {
  //       case COMM_PARAM_STOP_BITS:
  //         uart_config.stop_bits = UART_STOP_BITS_1;
  //         if(list[i].value == 2) { uart_config.stop_bits = UART_STOP_BITS_2;}
  //         break;
  //       case COMM_PARAM_BAUD:
  //         uart_config.baud_rate = (int)list[i].value;
  //         break;
  //       case COMM_PARAM_CLOCK_SPEED:
  //         uart_config.baud_rate = (int)list[i].value;
  //         break;
  //       case COMM_PARAM_LINE_MODE:
  //         if(list[i].value & 1){uart_config.parity = UART_PARITY_EVEN;}
  //         else{uart_config.parity = UART_PARITY_DISABLE;}

  //         if(list[i].value & 2){uart_config.stop_bits = UART_STOP_BITS_2;}
  //         else{uart_config.stop_bits = UART_STOP_BITS_1;}

  //         if(list[i].value & 4){uart_config.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS;}
  //         else{uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;}
  //         break;
  //       case COMM_WORK_ASYNC:
  //         m_is_async_mode = (bool) list[i].value;
  //         break;
  //       default:
  //         break;
  //     }
  //   }
  // }

  // We won't use a buffer for sending data.
  uart_driver_install(UART_NUM_0, 2048, 0, 0, NULL, 0);
  uart_param_config(UART_NUM_0, &uart_config);
  // uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_set_pin(UART_NUM_0, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);



  return STATUS_DRV_SUCCESS;
}

Status_t UART::read(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  // Status_t status = checkInputs(data, byte_count, timeout);
  // if(!status.success) { return status;}
  m_read_status = STATUS_DRV_RUNNING;
  m_read_status.success = false;
  m_bytes_available = 0;
  int rxBytes = uart_read_bytes(UART_NUM_0, data, byte_count, 1000 / portTICK_PERIOD_MS);
  if (rxBytes > 0)
  {
    m_bytes_available = rxBytes;
  }
  m_read_status = STATUS_DRV_SUCCESS;
  return STATUS_DRV_SUCCESS;
}


Status_t UART::write(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  // Status_t status = checkInputs(data, byte_count, timeout);
  // if(!status.success) { return status;}
  m_write_status = STATUS_DRV_RUNNING;
  m_write_status.success = false;
  m_bytes_written = 0;
  int txBytes = uart_write_bytes(UART_NUM_0, data, byte_count);
  if(txBytes > 0)
  {
    ;
  }
  m_write_status = STATUS_DRV_SUCCESS;
  return STATUS_DRV_SUCCESS;
}

Status_t UART::setCallback(DriverEventsList_t event, DriverCallback_t function, void *user_arg)
{
  Status_t status = STATUS_DRV_SUCCESS;

  switch (event)
  {
  case EVENT_READ:
    if(m_read_status.code != OPERATION_RUNNING)
    {
      m_func_rx = function;
      m_arg_rx = user_arg;
    }else
    {
      status = STATUS_DRV_ERR_BUSY;
    }
    break;
  case EVENT_WRITE:
    if (m_write_status.code != OPERATION_RUNNING)
    {
      m_func_tx = function;
      m_arg_tx = user_arg;
    }else
    {
      status = STATUS_DRV_ERR_BUSY;
    }
    break;
  default:
    status = STATUS_DRV_ERR_PARAM;
    break;
  }

  return status;
}

/**
 * @brief Verify if the inputs are in ther expected range
 *
 * @param buffer Data buffer
 * @param size Number of bytes in the data buffer
 * @param timeout Operation timeout value
 * @param key Parameter
 * @return Status_t
 */
Status_t UART::checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout)
{
  if(buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(m_handle == nullptr) { return STATUS_DRV_BAD_HANDLE;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  return STATUS_DRV_SUCCESS;
}