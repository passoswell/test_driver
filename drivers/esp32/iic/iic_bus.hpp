/**
 * @file iic_bus.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-08-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef IIC_IIC_BUS_HPP
#define IIC_IIC_BUS_HPP


#include <cstdio>
#include <cstdbool>

#include "peripherals_base/iic_interface.hpp"
#include "esp32/task_system/task_system.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


/**
 * @brief Interface class for IIC bus
 */
template<IicHandle_t PORT_NUMBER>
class IicBus final: public iIicBus
{
public:

  static iIicBus& getInstance()
  {
    static IicBus<PORT_NUMBER> instance;
    return instance;
  }

  ErrorCode configure(const SettingsList_t *list, uint8_t list_size) override;

  ErrorCode read(uint16_t address, Buffer_t data, uint32_t timeout, iCallback &event_handler) override;

  ErrorCode write(uint16_t address, Buffer_t data, uint32_t timeout, iCallback &event_handler) override;

  // A singleton should not be cloneable nor assignable
  IicBus(const IicBus&) = delete;
  IicBus(IicBus&&) = delete;
  IicBus& operator=(const IicBus&) = delete;
  IicBus& operator=(IicBus&&) = delete;

private:
  bool m_is_async_mode_rx, m_is_async_mode_tx;
  bool m_is_configured;

  IicBus() = default;

  ~IicBus() = default;

  ErrorCode checkInputs(const uint8_t *buffer, uint32_t size, uint32_t timeout);
};

#include "iic_bus.tpp"


#endif /* IIC_IIC_BUS_HPP */
