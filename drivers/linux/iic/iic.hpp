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

#ifndef DRIVERS_LINUX_IIC_IIC_HPP
#define DRIVERS_LINUX_IIC_IIC_HPP


#include <stdio.h>
#include <stdbool.h>

#include "peripherals_base/iic_base.hpp"
#include "linux/utils/linux_types.hpp"
#include "linux/task_system/task_system.hpp"

/**
 * @brief Base class for iic drivers
 */
class IIC final : public IicBase
{
public:
  IIC(const void *port_handle, uint16_t address);

  ~IIC();

  Status_t configure(const DriverSettings_t *list, uint8_t list_size);

  void setAddress(uint16_t address);

  using DriverInOutBase::read;
  Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  using DriverInOutBase::write;
  Status_t write(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  Status_t setCallback(DriverEventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

private:
  Task<DataBundle_t, IIC_QUEUE_SIZE, Status_t, 0> m_thread_handle;
  uint16_t m_address;
  int m_linux_handle;

  Status_t iicRead(uint8_t *buffer, uint32_t size, uint16_t address);

  Status_t iicWrite(const uint8_t *buffer, uint32_t size, uint16_t address);

  static Status_t transferDataAsync(DataBundle_t data_bundle, void *user_arg);

  Status_t checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout);
};

#endif /* DRIVERS_LINUX_IIC_IIC_HPP */