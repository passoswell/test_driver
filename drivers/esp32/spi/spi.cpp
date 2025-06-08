/**
 * @file spi.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-05-31
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "esp32/spi/spi.hpp"
#include "esp32/utils/esp32_io.hpp"

/**
 * @brief Constructor
 *
 * @param handle SPI number
 * @param cs Chip select object
 * @param cs_active_state State to select the device
 */
SPI::SPI(const SpiHandle_t handle, DIO &cs, bool cs_active_state) : m_cs(cs)
{
  m_handle = handle;
  m_esp_handle = nullptr;
  m_cs_active_state = cs_active_state;
  m_is_async_mode = false;
}

/**
 * @brief Destructor
 */
SPI::~SPI()
{
  if(m_esp_handle != nullptr)
  {
    spi_bus_remove_device(m_esp_handle);
  }
}

/**
 * @brief Configure a list of parameters
 *
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t SPI::configure(const SettingsList_t *list, uint8_t list_size)
{
  esp_err_t ret;
  spi_bus_config_t bus_parameters;
  spi_device_interface_config_t device_parameters;
  int mode = 0, max_baud = 1000000;

  // Configuring the BUS
  bus_parameters.miso_io_num = -1;
  bus_parameters.mosi_io_num = -1;
  bus_parameters.sclk_io_num = -1;
  bus_parameters.data2_io_num = -1;
  bus_parameters.data3_io_num = -1;
  bus_parameters.data4_io_num = -1;
  bus_parameters.data5_io_num = -1;
  bus_parameters.data6_io_num = -1;
  bus_parameters.data7_io_num = -1;
  bus_parameters.max_transfer_sz = 0;
  bus_parameters.flags = 0;
  bus_parameters.intr_flags = 0;
  bus_parameters.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO;

  m_read_status = STATUS_DRV_NOT_CONFIGURED;
  m_write_status = STATUS_DRV_NOT_CONFIGURED;

  if(list != nullptr && list_size != 0)
  {
    for(uint8_t i = 0; i < list_size; i++)
    {
      switch(list[i].parameter)
      {
      case COMM_PARAM_BAUD:
        max_baud = list[i].value;
        break;
      case COMM_PARAM_CLOCK_SPEED:
        max_baud = list[i].value;
        break;
      case COMM_PARAM_LINE_MODE:
        mode = list[i].value;
        if(mode > 3 || mode < 0)
        {
          mode = 0;
        }
        break;
      case COMM_WORK_ASYNC:
        m_is_async_mode = (bool)list[i].value;
        break;
      case COMM_WORK_ASYNC_RX:
        m_is_async_mode = (bool)list[i].value;
        break;
      case COMM_WORK_ASYNC_TX:
        m_is_async_mode = (bool)list[i].value;
        break;
      case COMM_PARAM_TX_DIO_PIN:
        bus_parameters.mosi_io_num = list[i].value;
        break;
      case COMM_PARAM_RX_DIO_PIN:
        bus_parameters.miso_io_num = list[i].value;
        break;
      case COMM_PARAM_CK_DIO_PIN:
        bus_parameters.sclk_io_num = list[i].value;
        break;
      default:
        break;
      }
    }
  }else
  {
    return STATUS_DRV_ERR_PARAM;
  }

  //Initialize the SPI bus
  ret = spi_bus_initialize((spi_host_device_t)m_handle, &bus_parameters, SPI_DMA_CH_AUTO);
  if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
  {
    return convertErrorCode(ret);
  }

  // Configuring the device
  device_parameters.command_bits = 0;
  device_parameters.address_bits = 0;
  device_parameters.dummy_bits = 0;
  device_parameters.duty_cycle_pos = 128; // 50% duty cycle
  device_parameters.cs_ena_pretrans = 8;  // delay in bits after selecting
  device_parameters.cs_ena_posttrans = 8; // delay in bits before unselecting
  device_parameters.clock_source = SPI_CLK_SRC_DEFAULT;
  device_parameters.clock_speed_hz = max_baud;
  device_parameters.mode = mode;
  device_parameters.spics_io_num = -1;
  device_parameters.queue_size = 1;
  device_parameters.flags = 0;
  device_parameters.pre_cb = cs_select;    // Callback to select the device
  device_parameters.post_cb = cs_unselect; // Callback to unselect the device
  device_parameters.input_delay_ns = 0; // Leave at 0 unless you know you need a delay

  // Attach the EEPROM to the SPI bus
  ret = spi_bus_add_device((spi_host_device_t)m_handle, &device_parameters, &m_esp_handle);
  if (ret != ESP_OK)
  {
    // cleanup;
    if (m_esp_handle)
    {
      spi_bus_remove_device(m_esp_handle);
      m_esp_handle = NULL;
    }
    return convertErrorCode(ret);
  }

  return STATUS_DRV_SUCCESS;
}

/**
 * @brief Read data from the bus
 *
 * @param data Buffer to store the data
 * @param byte_count Number of bytes to read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t SPI::read(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  if(data == nullptr)
  {
    return STATUS_DRV_NULL_POINTER;
  }
  if(byte_count == 0)
  {
    return STATUS_DRV_ERR_PARAM_SIZE;
  }
  if(m_is_async_mode)
  {
    return STATUS_DRV_NOT_IMPLEMENTED;
  }else
  {
    return xSpiXfer(nullptr, data, byte_count);
  }
}

/**
 * @brief Write data to the bus
 *
 * @param data Buffer where data is stored
 * @param byte_count Number of bytes to write
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t SPI::write(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  if(data == nullptr)
  {
    return STATUS_DRV_NULL_POINTER;
  }
  if(byte_count == 0)
  {
    return STATUS_DRV_ERR_PARAM_SIZE;
  }
  if(m_is_async_mode)
  {
    return STATUS_DRV_NOT_IMPLEMENTED;
  }else
  {
    return xSpiXfer(data, nullptr, byte_count);
  }
}

/**
 * @brief Read and write data simultaneously through the bus
 *
 * @param rx_data Buffer to store the data read
 * @param tx_data Buffer where data to write is stored
 * @param byte_count Number of bytes to write and read
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t SPI::transfer(uint8_t *rx_data, uint8_t *tx_data, Size_t byte_count, uint32_t timeout)
{
  if(rx_data == nullptr || tx_data == nullptr)
  {
    return STATUS_DRV_NULL_POINTER;
  }
  if(byte_count == 0)
  {
    return STATUS_DRV_ERR_PARAM_SIZE;
  }
  if(m_is_async_mode)
  {
    return STATUS_DRV_NOT_IMPLEMENTED;
  }else
  {
    return xSpiXfer(tx_data, rx_data, byte_count);
  }
}

/**
 * @brief Read and write data simultaneously through the bus
 *
 * @param rx_data Buffer to store the data read
 * @param tx_data Buffer where data to write is stored
 * @param timeout Time to wait in milliseconds before returning an error
 * @return Status_t
 */
Status_t SPI::transfer(Buffer_t rx_data, Buffer_t tx_data, uint32_t timeout)
{
  if(m_is_async_mode)
  {
    return STATUS_DRV_NOT_IMPLEMENTED;
  }else
  {
    return xSpiXfer(tx_data.data(), rx_data.data(), tx_data.size_bytes());
  }
}

/**
 * @brief Install a callback function
 *
 * @param event An event to trigger the call
 * @param function A function
 * @param user_arg A argument used as a parameter to the function
 * @return Status_t
 */
Status_t SPI::setCallback(EventsList_t event, DriverCallback_t function, void *user_arg)
{
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Perform a data transaction on the bus
 *
 * @param txBuf Buffer where data to write is stored
 * @param rxBuf Buffer to store the data read
 * @param byte_count Number of bytes to write and read
 * @return Status_t
 */
Status_t SPI::xSpiXfer(uint8_t *txBuf, uint8_t *rxBuf, uint32_t byte_count)
{
  Status_t status;
  spi_transaction_t trans_desc;
  esp_err_t ret;
  trans_desc.flags = 0;
  trans_desc.length = 8 * byte_count;
  trans_desc.rx_buffer = rxBuf;
  trans_desc.rxlength = trans_desc.length;
  trans_desc.tx_buffer = txBuf;
  trans_desc.user = this;
  return convertErrorCode(spi_device_polling_transmit(m_esp_handle, &trans_desc));
}

/**
 * @brief Callback to select the device
 *
 * @param t A pointer to a SPI object
 */
void SPI::cs_select(spi_transaction_t* t)
{
  SPI *obj = (SPI *) t->user;
  if(obj != nullptr)
  {
    obj->m_cs.write(obj->m_cs_active_state);
  }
}

/**
 * @brief Callback to unselect the device
 *
 * @param t A pointer to a SPI object
 */
void SPI::cs_unselect(spi_transaction_t* t)
{
  SPI *obj = (SPI *) t->user;
  if(obj != nullptr)
  {
    obj->m_cs.write(!obj->m_cs_active_state);
  }
}