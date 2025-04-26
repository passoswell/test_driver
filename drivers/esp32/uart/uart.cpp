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

#include <string.h>

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
  m_event_task_handle = nullptr;
  m_event_queue = nullptr;
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
  int rx_fifo_full_thr = 1;
  uint32_t task_priority = 12;
  uint8_t core_affinity = 1;

  m_read_status = STATUS_DRV_NOT_CONFIGURED;
  m_write_status = STATUS_DRV_NOT_CONFIGURED;

  uart_config.baud_rate = 115200;
  uart_config.data_bits = UART_DATA_8_BITS;
  uart_config.parity = UART_PARITY_DISABLE;
  uart_config.stop_bits = UART_STOP_BITS_1;
  uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
  uart_config.source_clk = UART_SCLK_DEFAULT;
#if (SOC_UART_LP_NUM >= 1)
  uart_config.lp_source_clk = LP_UART_SCLK_DEFAULT;
#endif
  uart_config.rx_flow_ctrl_thresh = 0;

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

        case COMM_PARAM_TASK_PRIORITY:
          task_priority = list[i].value;
          break;

        case COMM_PARAM_CORE_AFFINITY:
          core_affinity = list[i].value % configNUMBER_OF_CORES;
          break;

        default:
          break;
      }
    }
  }

  (void) uart_driver_delete((uart_port_t) m_handle.uart_number);
  status = convertErrorCode(uart_driver_install((uart_port_t) m_handle.uart_number, 2048, 0, 10, &m_event_queue, 0));
  if(!status.success)
  {
    return status;
  }

  // Configuring the driver
  status = convertErrorCode(uart_param_config((uart_port_t) m_handle.uart_number, &uart_config));
  if(!status.success)
  {
    (void) uart_driver_delete((uart_port_t) m_handle.uart_number);
    return status;
  }

  uart_set_rx_timeout((uart_port_t) m_handle.uart_number, 1);

  if(m_is_async_mode_rx)
  {
    // Configuring the threshold level for rx FIFO full interruptions
    // on async reception mode
    int fifo_length = UART_HW_FIFO_LEN((uart_port_t)m_handle.uart_number);
    if (uart_config.baud_rate > 115200)
    {
      if ((uart_config.baud_rate / 9600) > fifo_length / 2)
      {
        rx_fifo_full_thr = fifo_length - (uart_config.baud_rate / 9600);
      }
      else
      {
        rx_fifo_full_thr = fifo_length / 2;
      }
    }else
    {
      rx_fifo_full_thr = fifo_length - 10;
    }

    uart_set_always_rx_timeout((uart_port_t)m_handle.uart_number, true);

    m_rx_mutex.lock();
    m_func_rx = nullptr;
    m_arg_rx = nullptr;
    m_data.rx_buffer = nullptr;
    m_data.rx_size = 0;
    if (m_event_task_handle == nullptr)
    {
      xTaskCreatePinnedToCore([](void *arg) -> void { static_cast<UART *>(arg)->rxEventTask(); }, "uart_event_task", 2048, this, task_priority, &m_event_task_handle, core_affinity);
    }
    m_rx_mutex.unlock();
  }else
  {
    // Configuring the threshold level for rx FIFO full interruptions
    // on sync reception mode
    if(uart_config.baud_rate <= 9600)
    {
      rx_fifo_full_thr = 1;
    }else
    {
      rx_fifo_full_thr = uart_config.baud_rate / 9600;
    }
  }

  status = convertErrorCode(uart_set_rx_full_threshold((uart_port_t) m_handle.uart_number, rx_fifo_full_thr));
  if(!status.success)
  {
    (void) uart_driver_delete((uart_port_t) m_handle.uart_number);
    return status;
  }

  if(m_is_async_mode_tx)
  {
    m_tx_mutex.lock();
    m_func_tx = nullptr;
    m_arg_tx = nullptr;
    m_data.tx_buffer = nullptr;
    m_data.tx_size = 0;
    m_tx_monitor_task_handle.create([](uint8_t data, void *arg) -> Status_t {return static_cast<UART*>(arg)->txMonitorTask(data);}, this, task_priority);
    m_tx_mutex.unlock();
  }else
  {
    m_tx_monitor_task_handle.terminate();
  }



  // Configuring uart pins
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

  if(m_is_async_mode_rx)
  {
    m_rx_mutex.lock();
    m_data.rx_buffer = data;
    m_data.rx_size = byte_count;
    status = STATUS_DRV_SUCCESS;
    m_rx_mutex.unlock();
  }else
  {
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
    status = m_read_status;
  }
  return status;
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
  int tx_bytes = 0;
  Status_t status;
  status = checkInputs(data, byte_count, timeout);
  if(!status.success) { return status;}
  if(m_write_status.code == OPERATION_RUNNING) { return STATUS_DRV_ERR_BUSY;}

  m_write_status = STATUS_DRV_RUNNING;
  m_write_status.success = false;
  m_bytes_written = 0;

  if(m_is_async_mode_tx)
  {
    m_tx_mutex.lock();
    m_data.tx_buffer = data;
    m_data.tx_size = byte_count;
    tx_bytes = uart_write_bytes((uart_port_t) m_handle.uart_number, data, byte_count);
    if(tx_bytes > 0)
    {
      (void) m_tx_monitor_task_handle.getOutputData(status, 0);
      (void) m_tx_monitor_task_handle.setInputData(data[0], 0);
      m_bytes_written = tx_bytes;
      status = STATUS_DRV_SUCCESS;
    }else
    {
      status = STATUS_DRV_ERR_FAILED;
    }
    m_tx_mutex.unlock();
  }else
  {
    tx_bytes = uart_write_bytes((uart_port_t) m_handle.uart_number, data, byte_count);
    if(tx_bytes > 0)
    {
      m_bytes_written = tx_bytes;
      status = STATUS_DRV_SUCCESS;
    }else
    {
      status = STATUS_DRV_ERR_FAILED;
    }
    m_write_status = status;
  }

  return status;
}

/**
 * @brief Install a callback function
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

/**
 * @brief Manages uart reception events coming from an ISR
 */
void UART::rxEventTask(void)
{
  uart_event_t event;
  size_t buffered_size;
  uint8_t rx_state = 0;
  uint8_t bytes_received = 0;
  int length = 0;

  while(true)
  {
    //Waiting for UART event.
    if (xQueueReceive(m_event_queue, (void *)&event, (TickType_t)portMAX_DELAY))
    {
      switch (event.type)
      {
      // Event of UART receiving data
      /*We'd better handler data event fast, there would be much more data events than
      other types of events. If we take too much time on data event, the queue might
      be full.*/
      case UART_DATA:
        if(!event.timeout_flag)
        {
          // printf("\r\nEntered event task, UART_DATA case, FIFO filled above threshold\r\n");
          continue;
        }
        m_rx_mutex.lock();
        if(m_data.rx_buffer == nullptr)
        {
          // printf("\r\nEntered event task, UART_DATA case, buffer is null\r\n");
          m_rx_mutex.unlock();
          continue;
        }
        uart_get_buffered_data_len((uart_port_t) m_handle.uart_number, (size_t*)&length);
        if(length == 0)
        {
          // printf("\r\nEntered event task, UART_DATA case, read zero bytes\r\n");
          m_rx_mutex.unlock();
          continue;
        }
        // printf("\r\nEntered event task, UART_DATA case, idle line detected\r\n");
        if(length <= m_data.rx_size)
        {
          m_bytes_read = length;
        }else
        {
          m_bytes_read = m_data.rx_size;
        }
        uart_read_bytes((uart_port_t) m_handle.uart_number, m_data.rx_buffer, m_bytes_read, portMAX_DELAY);
        m_read_status = STATUS_DRV_SUCCESS;
        if(m_func_rx != nullptr)
        {
          Buffer_t data(m_data.rx_buffer, m_bytes_read);
          m_data.rx_buffer = nullptr;
          m_data.rx_size = 0;
          m_rx_mutex.unlock();
          m_func_rx(m_read_status, EVENT_WRITE, data, m_arg_rx);
        }else
        {
          m_rx_mutex.unlock();
        }
        break;
      // Event of HW FIFO overflow detected
      case UART_FIFO_OVF:
        // If fifo overflow happened, you should consider adding flow control for your application.
        // The ISR has already reset the rx FIFO,
        // As an example, we directly flush the rx buffer here in order to read more data.
        printf("FIFO overflow\r\n");
        uart_flush_input((uart_port_t) m_handle.uart_number);
        xQueueReset(m_event_queue);
        break;
      // Event of UART ring buffer full
      case UART_BUFFER_FULL:
        // If buffer full happened, you should consider increasing your buffer size
        // As an example, we directly flush the rx buffer here in order to read more data.
        printf("Buffer full\r\n");
        uart_flush_input((uart_port_t) m_handle.uart_number);
        xQueueReset(m_event_queue);
        break;
      // Event of UART RX break detected
      case UART_BREAK:
        break;
      // Event of UART parity check error
      case UART_PARITY_ERR:
        break;
      // Event of UART frame error
      case UART_FRAME_ERR:
        break;
      // UART_PATTERN_DET
      case UART_PATTERN_DET:
        break;
      // Others
      default:
        break;
      }
    }
  }
  vTaskDelete(NULL);
}

/**
 * @brief Monitors end of transmission of data through uart
 */
Status_t UART::txMonitorTask(uint8_t data)
{
  Status_t status;
  status = convertErrorCode(uart_wait_tx_done((uart_port_t) m_handle.uart_number, portMAX_DELAY));
  m_tx_mutex.lock();
  m_write_status = STATUS_DRV_SUCCESS;
  if(m_func_tx != nullptr)
  {
    Buffer_t data(m_data.tx_buffer, m_data.tx_size);
    m_data.tx_buffer = nullptr;
    m_data.tx_size = 0;
    m_tx_mutex.unlock();
    m_func_tx(status, EVENT_WRITE, data, m_arg_tx);
  }else
  {
    m_tx_mutex.unlock();
  }
  return status;
}