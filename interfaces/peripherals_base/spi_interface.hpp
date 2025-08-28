/**
 * @file spi_interface.hpp
 * @author your name (you@domain.com)
 * @brief Interface class and methods for SPI
 * @version 0.1
 * @date 2025-08-05
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef PERIPHERALS_BASE_SPI_INTERFACE_HPP
#define PERIPHERALS_BASE_SPI_INTERFACE_HPP


#include <stdint.h>
#include <stdbool.h>

#include "commons.hpp"
#include "dio_interface.hpp"
#include "peripherals_base/communication_interface.hpp"


typedef struct
{
  bool run;
  DrvBuffer_t data;
  uint32_t timeout;
  iDIO *cs_pin;
  Callback_t cb_function;
  void *cb_arg;
} SpiDataBundle2_t;

typedef struct
{
  SpiDataBundle2_t rx;
  SpiDataBundle2_t tx;
} SpiDataBundle_t;


/**
 * @brief Interface class for SPI bus
 */
class iSpiBus
{
public:

  virtual Status_t configure(const SettingsList_t *list, uint8_t list_size) = 0;

  virtual Status_t read(iDIO &cs_pin, bool cs_active_state, Buffer_t data, uint32_t timeout, Callback_t cb_function, void *cb_arg) = 0;

  virtual Status_t write(iDIO &cs_pin, bool cs_active_state, Buffer_t data, uint32_t timeout, Callback_t cb_function, void *cb_arg) = 0;
};

/**
 * @brief Interface class for SPI peripherals
 */
class iSPI : public iComm
{
public:

};

/**
 * @brief Base class for SPI peripherals
 */
class bSPI : public iSPI
{
public:

  bSPI(iSpiBus &bus, iDIO &cs_pin, bool cs_active_state) :
  m_bus(bus), m_cs_pin(cs_pin), m_cs_active_state(cs_active_state)
  {
    m_cb_function_rx = nullptr;
    m_cb_function_tx = nullptr;
    m_cb_arg_rx = nullptr;
    m_cb_arg_tx = nullptr;
  }

  virtual ~bSPI() = default;

  virtual Status_t configure(const SettingsList_t *list, uint8_t list_size) override
  {
    return m_bus.configure(list, list_size);
  }

  virtual Status_t read(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.read(m_cs_pin, m_cs_active_state, data, timeout, m_cb_function_rx, m_cb_arg_rx);
  }

  virtual Status_t write(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.write(m_cs_pin, m_cs_active_state, data, timeout, m_cb_function_tx, m_cb_arg_tx);
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
  iSpiBus &m_bus;
  iDIO &m_cs_pin;
  bool m_cs_active_state;
  Callback_t m_cb_function_rx, m_cb_function_tx;
  void *m_cb_arg_rx, *m_cb_arg_tx;
};


#endif /* PERIPHERALS_BASE_SPI_INTERFACE_HPP */
