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
#include "esp32/utils/esp32_io.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"

/**
 * @brief Constructor
 * @param port_handle Pointer to a handle, not used by this driver
 */
UART::UART(const UartHandle_t port_handle)
{
  m_handle = port_handle;
}

/**
 * @brief Destroyer
 */
UART::~UART()
{
  (void) uart_driver_delete((uart_port_t) m_handle.uart_number);
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t UART::configure(const SettingsList_t *list, uint8_t list_size)
{
  Status_t status;
  uart_config_t uart_config;
  int rx_pin = 0, tx_pin = 0;

  m_read_status = STATUS_DRV_NOT_CONFIGURED;
  m_write_status = STATUS_DRV_NOT_CONFIGURED;

  uart_config.baud_rate = 115200;
  uart_config.data_bits = UART_DATA_8_BITS;
  uart_config.parity = UART_PARITY_DISABLE;
  uart_config.stop_bits = UART_STOP_BITS_1;
  uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
  uart_config.source_clk = UART_SCLK_DEFAULT;
  uart_config.rx_flow_ctrl_thresh = 0;//UART_HW_FIFO_LEN(uart_num) - 1;

  if(list != nullptr && list_size != 0)
  {
    for(uint8_t i = 0; i < list_size; i++)
    {
      switch(list[i].parameter)
      {
        case COMM_PARAM_STOP_BITS:
          uart_config.stop_bits = UART_STOP_BITS_1;
          if(list[i].value == 2) { uart_config.stop_bits = UART_STOP_BITS_2;}
          break;

        case COMM_PARAM_BAUD:
          uart_config.baud_rate = (int)list[i].value;
          break;

        case COMM_PARAM_CLOCK_SPEED:
          uart_config.baud_rate = (int)list[i].value;
          break;

        case COMM_PARAM_LINE_MODE:
          if(list[i].value & 1){uart_config.parity = UART_PARITY_EVEN;}
          else{uart_config.parity = UART_PARITY_DISABLE;}

          if(list[i].value & 2){uart_config.stop_bits = UART_STOP_BITS_2;}
          else{uart_config.stop_bits = UART_STOP_BITS_1;}

          if(list[i].value & 4){uart_config.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS;}
          else{uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;}
          break;

        case COMM_WORK_ASYNC_RX:
          m_is_async_mode_rx = (bool) list[i].value;
          break;

        case COMM_WORK_ASYNC_TX:
          m_is_async_mode_tx = (bool) list[i].value;
          break;

        default:
          break;
      }
    }
  }

  // We won't use a buffer for sending data.
  status = convertErrorCode(uart_driver_install((uart_port_t) m_handle.uart_number, 2048, 0, 0, NULL, 0));
  if(!status.success)
  {
    return status;
  }
  status = convertErrorCode(uart_param_config((uart_port_t) m_handle.uart_number, &uart_config));
  if(!status.success)
  {
    (void) uart_driver_delete((uart_port_t) m_handle.uart_number);
    return status;
  }
  // status = convertErrorCode(uart_set_pin((uart_port_t) m_handle.uart_number, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  status = convertErrorCode(uart_set_pin((uart_port_t) m_handle.uart_number, m_handle.tx_pin, m_handle.rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  if(!status.success)
  {
    (void) uart_driver_delete((uart_port_t) m_handle.uart_number);
    return status;
  }

  m_read_status = STATUS_DRV_IDLE;
  m_write_status = STATUS_DRV_IDLE;
  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Read data
 * @param data Buffer to store the data
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t UART::read(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  status = checkInputs(data, byte_count, timeout);
  if(!status.success) { return status;}
  if(m_read_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}

  m_read_status = STATUS_DRV_RUNNING;
  m_read_status.success = false;
  m_bytes_read = 0;
  int rx_bytes = uart_read_bytes((uart_port_t) m_handle.uart_number, data, byte_count, timeout / portTICK_PERIOD_MS);
  if (rx_bytes > 0)
  {
    m_bytes_read = rx_bytes;
    m_read_status = STATUS_DRV_SUCCESS;
  }else if(rx_bytes == 0)
  {
    m_read_status = STATUS_DRV_ERR_TIMEOUT;
  }else
  {
    m_read_status = STATUS_DRV_ERR_FAILED;
  }
  return m_read_status;
}

/**
 * @brief Write data
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t UART::write(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  status = checkInputs(data, byte_count, timeout);
  if(!status.success) { return status;}
  if(m_write_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}

  m_write_status = STATUS_DRV_RUNNING;
  m_write_status.success = false;
  m_bytes_written = 0;
  int tx_bytes = uart_write_bytes((uart_port_t) m_handle.uart_number, data, byte_count);
  if(tx_bytes > 0)
  {
    m_bytes_written = tx_bytes;
    m_write_status = STATUS_DRV_SUCCESS;
  }else
  {
    m_write_status = STATUS_DRV_ERR_FAILED;
  }
  return m_write_status;
}

/**
 * @brief Install a callback function
 *
 * @param event An event to trigger the call
 * @param function A function to call back
 * @param user_arg A argument used as a parameter to the function
 * @return Status_t
 */
Status_t UART::setCallback(EventsList_t event, DriverCallback_t function, void *user_arg)
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
 * @param buffer Data buffer
 * @param size Number of bytes in the data buffer
 * @param timeout Operation timeout value
 * @param key Parameter
 * @return Status_t
 */
Status_t UART::checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout)
{
  if(buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  return STATUS_DRV_SUCCESS;
}