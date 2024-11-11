/**
 * @file iic.hpp
 * @author your name (you@domain.com)
 * @brief Give access to IIC functionalities
 * @version 0.1
 * @date 2024-10-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef IIC_HPP
#define IIC_HPP


#include <stdio.h>
#include <stdbool.h>

#include "drivers/base/peripherals_base/iic_base.hpp"
#include "drivers/linux/utils/linux_types.hpp"

/**
 * @brief Base class for iic drivers
 */
class IIC : public IicBase
{
public:
  IIC(void *port_handle, uint16_t address);

  ~IIC();

  Status_t configure(const DriverSettings_t *list, uint8_t list_size);

  void setAddress(uint16_t address);

  using DriverInOutBase::read;
  Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  using DriverInOutBase::write;
  Status_t write(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  Status_t setCallback(DriverEventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

private:
  uint16_t m_address;
  int m_linux_handle;
  bool m_terminate;
  bool m_is_configured;
  UtilsInOutSync_t m_sync;

  Status_t i2cRead(uint8_t *buffer, uint32_t size, uint16_t address_8bits);

  Status_t i2cWrite(const uint8_t *buffer, uint32_t size, uint16_t address_8bits);

  void asyncThread(void);
};

#endif /* IIC_HPP */