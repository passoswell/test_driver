/**
 * @file iic.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "iic.hpp"
#include "esp32/utils/esp32_io.hpp"
#include "driver/i2c.h"

IIC::IIC(const IicHandle_t port_handle, uint16_t address)
{
  m_handle = port_handle;
}

IIC::~IIC()
{
  (void) i2c_driver_delete((i2c_port_t)m_handle.i2c_number);
}

Status_t IIC::configure(const SettingsList_t *list, uint8_t list_size)
{
  Status_t status;
  i2c_config_t i2c_config;

  i2c_config.mode = I2C_MODE_MASTER;
  i2c_config.sda_io_num = m_handle.sda_pin;
  i2c_config.scl_io_num = m_handle.scl_pin;
  i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
  i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
  i2c_config.master.clk_speed = 100000; // 100 kHz
  i2c_config.clk_flags = 0;

  status = convertErrorCode( i2c_param_config((i2c_port_t)m_handle.i2c_number, &i2c_config) );
  if(!status.success)
  {
    return status;
  }

  status = convertErrorCode( i2c_driver_install((i2c_port_t)m_handle.i2c_number, i2c_config.mode, 0, 0, 0) );
  if(!status.success)
  {
    return status;
  }

  return STATUS_DRV_SUCCESS;
}

void IIC::setAddress(uint16_t address)
{
  m_address = address;
}

Status_t IIC::read(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  status = checkInputs(data, byte_count, timeout);
  if(!status.success)
  {
    return status;
  }

  return convertErrorCode( i2c_master_read_from_device((i2c_port_t)m_handle.i2c_number, m_address, data, byte_count, timeout / portTICK_PERIOD_MS) );
}

Status_t IIC::write(uint8_t *data, Size_t byte_count, uint32_t timeout)
{
  Status_t status;
  status = checkInputs(data, byte_count, timeout);
  if(!status.success)
  {
    return status;
  }

  return convertErrorCode( i2c_master_write_to_device((i2c_port_t)m_handle.i2c_number, m_address, data, byte_count, timeout / portTICK_PERIOD_MS) );
}

Status_t IIC::setCallback(EventsList_t event, DriverCallback_t function, void *user_arg)
{
  return STATUS_DRV_NOT_IMPLEMENTED;
}

/**
 * @brief Verify if the inputs are in ther expected range
 * @param buffer Data buffer
 * @param size Number of bytes in the data buffer
 * @param timeout Operation timeout value
 * @return Status_t
 */
Status_t IIC::checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout)
{
  if(buffer == nullptr) { return STATUS_DRV_NULL_POINTER;}
  if(size == 0) { return STATUS_DRV_ERR_PARAM_SIZE;}
  return STATUS_DRV_SUCCESS;
}