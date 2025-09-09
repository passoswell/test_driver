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


typedef uint16_t IicHandle_t;

typedef struct
{
  bool run;
  DrvBuffer_t data;
  uint32_t timeout;
  uint16_t address;
  iCallback *event_handle;
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

  iIicBus() = default;

  virtual ~iIicBus() = default;

  virtual Status_t configure(const SettingsList_t *list, uint8_t list_size) = 0;

  virtual Status_t read(uint16_t address, Buffer_t data, uint32_t timeout, iCallback &event_handler) = 0;

  virtual Status_t write(uint16_t address, Buffer_t data, uint32_t timeout, iCallback &event_handler) = 0;
};

/**
 * @brief Interface class for IIC peripherals
 */
class iIIC : public iComm
{
public:

  iIIC() = default;

  virtual ~iIIC() = default;

};

/**
 * @brief Base class for IIC peripherals
 */
class bIIC : public iIIC
{
public:

  bIIC(iIicBus &bus, uint16_t address) :
  m_bus(bus), m_address(address),
  m_cb_function_rx(nullptr), m_cb_function_tx(nullptr)
  {
  }

  virtual ~bIIC() = default;

  virtual Status_t configure(const SettingsList_t *list, uint8_t list_size) override
  {
    return m_bus.configure(list, list_size);
  }

  virtual Status_t read(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.read(m_address, data, timeout, *m_cb_function_rx);
  }

  virtual Status_t write(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.write(m_address, data, timeout, *m_cb_function_tx);
  }

  virtual Status_t setCallback(EventsList_t event, iCallback &event_handler) override
  {
    Status_t status = STATUS_DRV_SUCCESS;
    switch(event)
    {
    case EVENT_READ:
      m_cb_function_rx = &event_handler;
      break;
    case EVENT_WRITE:
      m_cb_function_tx = &event_handler;
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
  iCallback *m_cb_function_rx, *m_cb_function_tx;
};

#endif /* PERIPHERALS_BASE_IIC_INTERFACE_HPP */
