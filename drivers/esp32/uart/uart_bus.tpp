/**
 * @file uart_bus.tpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-08-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "uart_bus.hpp"
#include "esp32/utils/esp32_io.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"

/**
 * @brief Constructor
 */
template<UartHandle_t PORT_NUMBER>
UartBus<PORT_NUMBER>::UartBus()
{
  m_handle = PORT_NUMBER;
  m_event_task_handle = nullptr;
  m_event_queue = nullptr;
  m_terminate = false;
}

/**
 * @brief Destroyer
 */
template<UartHandle_t PORT_NUMBER>
UartBus<PORT_NUMBER>::~UartBus()
{
  terminateRxEventTask();
  (void) uart_driver_delete((uart_port_t) PORT_NUMBER);
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return ErrorCode
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::configure(const SettingsList_t *list, uint8_t list_size)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  esp_err_t esp_error = ESP_OK;
  bool result = true;
  uart_config_t uart_config;
  int rx_pin = 0, tx_pin = 0;
  int rx_fifo_full_thr = 1;
  TaskProfile_t task_parameters;

  m_rx_mutex.lock();
  m_tx_mutex.lock();

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

  task_parameters.core_number = configNUMBER_OF_CORES - 1;
  task_parameters.type = TASK_WORKER;
  task_parameters.stack_size = TASK_MINIMAL_STACK_SIZE;
  task_parameters.priority = configMAX_PRIORITIES - 1;
  task_parameters.period = 0;
  task_parameters.name = nullptr;

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
          task_parameters.priority = list[i].value;
          break;

        case COMM_PARAM_TASK_STACK_SIZE:
          task_parameters.stack_size = list[i].value;
          break;

        case COMM_PARAM_CORE_AFFINITY:
          task_parameters.core_number = list[i].value % configNUMBER_OF_CORES;
          break;

        case COMM_PARAM_RX_DIO_PIN:
          rx_pin = list[i].value;
          break;

        case COMM_PARAM_TX_DIO_PIN:
          tx_pin = list[i].value;
          break;

        default:
          break;
      }
    }
  }

  (void) uart_driver_delete((uart_port_t) PORT_NUMBER);
  esp_error = uart_driver_install((uart_port_t) PORT_NUMBER, 2048, 2048, 10, &m_event_queue, 0);
  if(esp_error != ESP_OK)
  {
    status.setValue(GenericErrorCode::kFailed);
    status.setMessage( getErrorMessage( esp_error ) );
    m_rx_mutex.unlock();
    m_tx_mutex.unlock();
    return status;
  }

  // Configuring the driver
  esp_error = uart_param_config((uart_port_t) PORT_NUMBER, &uart_config);
  if(esp_error != ESP_OK)
  {
    status.setValue(GenericErrorCode::kFailed);
    status.setMessage( getErrorMessage( esp_error ) );
    (void) uart_driver_delete((uart_port_t) PORT_NUMBER);
    m_rx_mutex.unlock();
    m_tx_mutex.unlock();
    return status;
  }

  esp_error = uart_set_rx_timeout((uart_port_t) PORT_NUMBER, 1);
  if(esp_error != ESP_OK)
  {
    status.setValue(GenericErrorCode::kFailed);
    status.setMessage( getErrorMessage( esp_error ) );
    (void) uart_driver_delete((uart_port_t) PORT_NUMBER);
    m_rx_mutex.unlock();
    m_tx_mutex.unlock();
    return status;
  }

  if(m_is_async_mode_rx)
  {
    // Configuring the threshold level for rx FIFO full interruptions
    // on async reception mode
    int fifo_length = UART_HW_FIFO_LEN((uart_port_t)PORT_NUMBER);
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

    uart_set_always_rx_timeout((uart_port_t)PORT_NUMBER, true);

    if (m_event_task_handle == nullptr)
    {
      task_parameters.name = "uart_rxMonitorTask";
      result = m_rx_monitor_task_handle.create([](UartDataBundle_t data_bundle, void *arg) -> ErrorCode {return static_cast<UartBus*>(arg)->rxMonitorTask(data_bundle);}, this, task_parameters);
      if(!result)
      {
        status.setValue(GenericErrorCode::kFailed);
        status.setMessage("Failed to launch UartBus task for reception");
      }
    }
  }else
  {
    terminateRxEventTask();
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

  esp_error = uart_set_rx_full_threshold((uart_port_t) PORT_NUMBER, rx_fifo_full_thr);
  if(esp_error != ESP_OK)
  {
    status.setValue(GenericErrorCode::kFailed);
    status.setMessage( getErrorMessage( esp_error ) );
    (void) uart_driver_delete((uart_port_t) PORT_NUMBER);
    m_rx_mutex.unlock();
    m_tx_mutex.unlock();
    return status;
  }

  if(m_is_async_mode_tx)
  {
    task_parameters.name = "uart_txMonitorTask";
    result = m_tx_monitor_task_handle.create([](UartDataBundle_t data_bundle, void *arg) -> ErrorCode {return static_cast<UartBus*>(arg)->txMonitorTask(data_bundle);}, this, task_parameters);
    if (!result)
    {
      status.setValue(GenericErrorCode::kFailed);
      status.setMessage("Failed to launch UartBus task for transmission");
    }
  }else
  {
    m_tx_monitor_task_handle.terminate();
  }



  // Configuring uart pins
  esp_error = uart_set_pin((uart_port_t) PORT_NUMBER, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  if(esp_error != ESP_OK)
  {
    status.setValue(GenericErrorCode::kFailed);
    status.setMessage( getErrorMessage( esp_error ) );
    (void) uart_driver_delete((uart_port_t) PORT_NUMBER);
    m_rx_mutex.unlock();
    m_tx_mutex.unlock();
    return status;
  }

  m_rx_mutex.unlock();
  m_tx_mutex.unlock();
  return status;
}

/**
 * @brief Read data
 * @param data Buffer to store the data
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return ErrorCode
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::read(iDIO &rs485_pin, Buffer_t data, uint32_t timeout, iCallback &event_handler)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  UartDataBundle_t data_bundle;

  status = checkInputs(data.data(), data.size_bytes(), timeout);
  if(status) { return status;}

  m_bytes_read = 0;

  if(m_is_async_mode_rx)
  {
    data_bundle.rx.run = true;
    data_bundle.rx.data = data;
    data_bundle.rx.timeout = timeout;
    data_bundle.rx.rs485_pin = &rs485_pin;
    data_bundle.rx.event_handler = &event_handler;
    m_rx_monitor_task_handle.getOutputData(status, 0);
    if (m_rx_monitor_task_handle.setInputData(data_bundle, timeout))
    {
      status.setValue(GenericErrorCode::kSuccess);
    } else
    {
      status.setValue(GenericErrorCode::kBusy);
    }
  }else
  {
    status = blockingRead(data, timeout);
  }
  return status;
}

/**
 * @brief Write data
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @return ErrorCode
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::write(iDIO &rs485_pin, Buffer_t data, uint32_t timeout, iCallback &event_handler)
{
  int tx_bytes = 0;
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  UartDataBundle_t data_bundle;

  status = checkInputs(data.data(), data.size_bytes(), timeout);
  if(status) { return status;}

  if(m_is_async_mode_tx)
  {
    data_bundle.tx.run = true;
    data_bundle.tx.data = data;
    data_bundle.tx.timeout = timeout;
    data_bundle.tx.rs485_pin = &rs485_pin;
    data_bundle.tx.event_handler = &event_handler;
    m_tx_monitor_task_handle.getOutputData(status, 0);
    if (m_tx_monitor_task_handle.setInputData(data_bundle, timeout))
    {
      status.setValue(GenericErrorCode::kSuccess);
    } else
    {
      status.setValue(GenericErrorCode::kBusy);
    }
  }else
  {
    status = blockingWrite(data, timeout, true);
  }

  return status;
}

template<UartHandle_t PORT_NUMBER>
uint32_t UartBus<PORT_NUMBER>::getBytesAvailable()
{
  size_t bytes_available = 0;
  (void) uart_get_buffered_data_len((uart_port_t) PORT_NUMBER, &bytes_available);
  return (uint32_t) bytes_available;
}

template<UartHandle_t PORT_NUMBER>
uint32_t UartBus<PORT_NUMBER>::getBytesRead()
{
  return m_bytes_read;
}

/**
 * @brief Verify if the inputs are in ther expected range
 * @param buffer Data buffer
 * @param size Number of bytes in the data buffer
 * @param timeout Operation timeout value
 * @return ErrorCode
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  if(buffer == nullptr) { status.setValue(GenericErrorCode::kNullPointer);}
  if(size == 0) { status.setValue(GenericErrorCode::kInvalidParameter);}
  return status;
}

template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::rxMonitorTask(UartDataBundle_t data_bundle)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  uint32_t bytes_available, bytes_read;
  uart_event_t event;
  size_t buffered_size;
  uint8_t rx_state = 0;
  uint8_t bytes_received = 0;
  int length = 0;

  // Read data if already available in the buffer
  bytes_available = getBytesAvailable();
  if(bytes_available >= data_bundle.rx.data.size_bytes())
  {
    status = blockingRead(data_bundle.rx.data, data_bundle.rx.timeout);
    return status;
  }

  // Wait for events about data received
  while(data_bundle.rx.run)
  {

    if (xQueueReceive(m_event_queue, (void *)&event, (TickType_t)portMAX_DELAY) == pdFALSE)
    {
      continue;
    }

    switch (event.type)
    {

    case UART_DATA:
      // Event of UART receiving data
      /*We'd better handler data event fast, there would be much more data
      events than other types of events. If we take too much time on data event,
      the queue might be full.*/
      if (!event.timeout_flag)
      {
        break;
      }

      bytes_available = getBytesAvailable();
      if (bytes_available == 0)
      {
        break;
      }
      if (bytes_available <= data_bundle.rx.data.size_bytes())
      {
        bytes_read = bytes_available;
      } else
      {
        bytes_read = data_bundle.rx.data.size_bytes();
      }

      status = blockingRead({data_bundle.rx.data.data(), bytes_read}, data_bundle.rx.timeout);
      if (data_bundle.rx.event_handler != nullptr)
      {
        data_bundle.rx.event_handler->onEvent(status, EVENT_WRITE, {data_bundle.rx.data.data(), m_bytes_read});
      }
      data_bundle.rx.run = false;
      break;

    case UART_FIFO_OVF:      // Event of HW FIFO overflow detected
      // If fifo overflow happened, you should consider adding flow control for
      // your application. The ISR has already reset the rx FIFO,
      // As an example, we directly flush the rx buffer here in order
      // to read more data.
      uart_flush_input((uart_port_t)PORT_NUMBER);
      xQueueReset(m_event_queue);
      break;
    case UART_BUFFER_FULL:  // Event of UART ring buffer full
      // If buffer full happened, you should consider increasing
      // your buffer size
      // As an example, we directly flush the rx buffer here in order
      // to read more data.
      uart_flush_input((uart_port_t)PORT_NUMBER);
      xQueueReset(m_event_queue);
      break;
    case UART_BREAK:         // Event of UART RX break detected
      break;
    case UART_PARITY_ERR:    // Event of UART parity check error
      break;
    case UART_FRAME_ERR:     // Event of UART frame error
      break;
    case UART_PATTERN_DET:   // UART_PATTERN_DET
      break;
    default:                 // Others
      break;
    }
  }

  return status;
}

template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::blockingRead(Buffer_t data, uint32_t timeout)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  int rx_bytes;
  rx_bytes = uart_read_bytes((uart_port_t)PORT_NUMBER, data.data(), data.size_bytes(), timeout / portTICK_PERIOD_MS);
  if(rx_bytes < 0)
  {
    m_bytes_read = 0;
    status.setValue(GenericErrorCode::kFailed);
  }else
  {
    m_bytes_read = rx_bytes;
    if (rx_bytes > 0)
    {
      status.setValue(GenericErrorCode::kSuccess);
    }
    else if (rx_bytes == 0)
    {
      status.setValue(GenericErrorCode::kTimedOut);
    }
  }
  return status;
}

/**
 * @brief Monitors end of transmission of data through uart
 */
template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::txMonitorTask(UartDataBundle_t data_bundle)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  status = blockingWrite(data_bundle.tx.data, data_bundle.tx.timeout, true);
  if(data_bundle.tx.event_handler != nullptr)
  {
    data_bundle.tx.event_handler->onEvent(status, EVENT_WRITE, data_bundle.tx.data);
  }
  return status;
}

template<UartHandle_t PORT_NUMBER>
ErrorCode UartBus<PORT_NUMBER>::blockingWrite(Buffer_t data, uint32_t timeout, bool wait_end_of_transmission)
{
  ErrorCode status(GenericErrorCode::kSuccess, UartErrorCategory::getCategory());
  esp_err_t esp_error = ESP_OK;
  int tx_bytes;
  tx_bytes = uart_write_bytes((uart_port_t) PORT_NUMBER, data.data(), data.size_bytes());
  if(tx_bytes != data.size_bytes())
  {
    status.setValue(GenericErrorCode::kFailed);
    return status;
  }
  if(wait_end_of_transmission)
  {
    esp_error = uart_wait_tx_done((uart_port_t) PORT_NUMBER, portMAX_DELAY);
    if(esp_error == ESP_OK)
    {
      status.setValue(GenericErrorCode::kSuccess);
    }else
    {
      status.setValue(GenericErrorCode::kFailed);
      status.setMessage(getErrorMessage(esp_error));
    }
  }
  return status;
}

/**
 * @brief Send a command for rxEventTask to terminate itself
 */
template<UartHandle_t PORT_NUMBER>
void UartBus<PORT_NUMBER>::terminateRxEventTask(void)
{
  if(m_event_task_handle != nullptr)
  {
    uart_event_t event;
    m_terminate = true;
    (void) xQueueSend(m_event_queue, &event, 0);
  }
}