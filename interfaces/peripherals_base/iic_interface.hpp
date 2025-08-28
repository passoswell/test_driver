/**
 * @file iic_interface.hpp
 * @author your name (you@domain.com)
 * @brief Interface class and methods for IIC
 * @version 0.1
 * @date 2025-08-05
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PERIPHERALS_BASE_IIC_INTERFACE_HPP
#define PERIPHERALS_BASE_IIC_INTERFACE_HPP


#include <stdint.h>
#include <stdbool.h>

#include "commons.hpp"
#include "peripherals_base/communication_interface.hpp"


typedef struct
{
  bool run;
  DrvBuffer_t data;
  uint32_t timeout;
  uint16_t address;
  Callback_t cb_function;
  void *cb_arg;
} IicDataBundle2_t;

typedef struct
{
  IicDataBundle2_t rx;
  IicDataBundle2_t tx;
} IicDataBundle_t;


/**
 * @brief Interface class for IIC bus
 */
class iIicBus
{
public:

  virtual Status_t configure(const SettingsList_t *list, uint8_t list_size) = 0;

  virtual Status_t read(uint16_t address, Buffer_t data, uint32_t timeout = UINT32_MAX, Callback_t cb_function = nullptr, void *cb_arg = nullptr) = 0;

  virtual Status_t write(uint16_t address, Buffer_t data, uint32_t timeout = UINT32_MAX, Callback_t cb_function = nullptr, void *cb_arg = nullptr) = 0;
};

/**
 * @brief Interface class for IIC peripherals
 */
class iIIC : public iComm
{
public:

};

/**
 * @brief Base class for IIC peripherals
 */
class bIIC : public iIIC
{
public:

  bIIC(iIicBus &bus, uint16_t address) :
  m_bus(bus), m_address(address)
  {
    m_cb_function_rx = nullptr;
    m_cb_function_tx = nullptr;
    m_cb_arg_rx = nullptr;
    m_cb_arg_tx = nullptr;
  }

  virtual ~bIIC() = default;

  virtual Status_t configure(const SettingsList_t *list, uint8_t list_size) override
  {
    return m_bus.configure(list, list_size);
  }

  virtual Status_t read(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.read(m_address, data, timeout, m_cb_function_rx, m_cb_arg_rx);
  }

  virtual Status_t write(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.write(m_address, data, timeout, m_cb_function_tx, m_cb_arg_tx);
  }

  virtual Status_t setCallback(EventsList_t event = EVENT_NONE, Callback_t cb_function = nullptr, void *cb_arg = nullptr) override
  {
    Status_t status = STATUS_DRV_SUCCESS;
    switch(event)
    {
    case EVENT_READ:
      m_cb_function_rx = cb_function;
      m_cb_arg_rx = cb_arg;
      break;
    case EVENT_WRITE:
      m_cb_function_tx = cb_function;
      m_cb_arg_tx = cb_arg;
      break;
    default:
      status = STATUS_DRV_ERR_PARAM;
      break;
    }
    return status;
  }

protected:
  iIicBus &m_bus;
  uint16_t m_address;
  Callback_t m_cb_function_rx, m_cb_function_tx;
  void *m_cb_arg_rx, *m_cb_arg_tx;
};

#endif /* PERIPHERALS_BASE_IIC_INTERFACE_HPP */
