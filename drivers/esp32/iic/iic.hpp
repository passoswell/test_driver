/**
 * @file iic.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef IIC_IIC_HPP
#define IIC_IIC_HPP

#include "peripherals_base/iic_base.hpp"

/**
 * @brief Handle for the ESP IDF port of IIC driver
 */
typedef struct
{
  uint8_t iic_number;
  uint16_t sda_pin;
  uint16_t scl_pin;
} IicHandle_t;

/**
 * @brief Base class for iic drivers
 */
class IIC : public IicBase
{
public:
  IIC(const IicHandle_t port_handle, uint16_t address = 0);

  ~IIC();

  Status_t configure(const SettingsList_t *list, uint8_t list_size);

  void setAddress(uint16_t address);

  using IicBase::read;
  Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  using IicBase::write;
  Status_t write(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  Status_t setCallback(EventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

protected:
  IicHandle_t m_handle;
  uint16_t m_address;

  Status_t checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout);
};

#endif /* IIC_IIC_HPP */
