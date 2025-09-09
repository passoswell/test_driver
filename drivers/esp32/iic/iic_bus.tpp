/**
 * @file iic_bus.tpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-08-17
 *
 * @copyright Copyright (c) 2025
 *
 */

 #include "iic_bus.hpp"
#include "driver/i2c.h"


/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
template<IicHandle_t PORT_NUMBER>
Status_t IicBus<PORT_NUMBER>::configure(const SettingsList_t *list, uint8_t list_size)
{
  Status_t status;
  i2c_config_t i2c_config;

  if(m_is_configured)
  {
    return STATUS_DRV_SUCCESS;
  }

  i2c_config.mode = I2C_MODE_MASTER;
  i2c_config.sda_io_num = GPIO_NUM_MAX;
  i2c_config.scl_io_num = GPIO_NUM_MAX;
  i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
  i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
  i2c_config.master.clk_speed = 100000; // 100 kHz
  i2c_config.clk_flags = 0;


  if(list != nullptr && list_size != 0)
  {
    for(uint8_t i = 0; i < list_size; i++)
    {
      switch(list[i].parameter)
      {
      case COMM_WORK_ASYNC_RX:
        m_is_async_mode_rx = (bool)list[i].value;
        break;
      case COMM_WORK_ASYNC_TX:
        m_is_async_mode_tx = (bool)list[i].value;
        break;
      case COMM_PARAM_CLOCK_SPEED:
        i2c_config.master.clk_speed = list[i].value;
        break;
      case COMM_USE_PULL_UP:
        if(list[i].value == 0)
        {
          i2c_config.sda_pullup_en = GPIO_PULLUP_DISABLE;
          i2c_config.scl_pullup_en = GPIO_PULLUP_DISABLE;
        }else
        {
          i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
          i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
        }
        break;
      case COMM_PARAM_SDA_DIO_PIN:
        i2c_config.sda_io_num = (int)list[i].value;
        break;
      case COMM_PARAM_CK_DIO_PIN:
        i2c_config.scl_io_num = (int)list[i].value;
        break;
      default:
        break;
      }
    }
  }

  status = convertErrorCode( i2c_param_config((i2c_port_t)PORT_NUMBER, &i2c_config) );
  if(!status.success)
  {
    return status;
  }

  status = convertErrorCode( i2c_driver_install((i2c_port_t)PORT_NUMBER, i2c_config.mode, 0, 0, 0) );
  if(!status.success)
  {
    return status;
  }

  m_is_configured = true;
  return STATUS_DRV_SUCCESS;
}

template <IicHandle_t PORT_NUMBER>
Status_t IicBus<PORT_NUMBER>::read(uint16_t address, Buffer_t data, uint32_t timeout, iCallback &event_handler)
{
  Status_t status;
  status = checkInputs(data.data(), data.size_bytes(), timeout);
  if(!status.success)
  {
    return status;
  }

  if(m_is_async_mode_rx)
  {
    status = STATUS_DRV_NOT_IMPLEMENTED;
  }else
  {
    status = convertErrorCode( i2c_master_read_from_device((i2c_port_t)PORT_NUMBER, address, data.data(), data.size_bytes(), timeout / portTICK_PERIOD_MS) );
  }

  return status;
}

template <IicHandle_t PORT_NUMBER>
Status_t IicBus<PORT_NUMBER>::write(uint16_t address, Buffer_t data, uint32_t timeout, iCallback &event_handler)
{
  Status_t status;
  status = checkInputs(data.data(), data.size_bytes(), timeout);
  if(!status.success)
  {
    return status;
  }

  if(m_is_async_mode_tx)
  {
    status = STATUS_DRV_NOT_IMPLEMENTED;
  }else
  {
    status = convertErrorCode(i2c_master_write_to_device((i2c_port_t)PORT_NUMBER, address, data.data(), data.size_bytes(), timeout / portTICK_PERIOD_MS));
  }

  return status;
}

/**
 * @brief Verify if the inputs are in ther expected range
 * @param buffer Data buffer
 * @param size Number of bytes in the data buffer
 * @param timeout Operation timeout value
 * @return Status_t
 */
template <IicHandle_t PORT_NUMBER>
Status_t IicBus<PORT_NUMBER>::checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout)
{
  if(buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  return STATUS_DRV_SUCCESS;
}