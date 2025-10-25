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


#include <cstdint>
#include <cstdbool>

#include "commons.hpp"
#include "dio_interface.hpp"
#include "peripherals_base/communication_interface.hpp"


/**
 * @brief SPI error codes category
 */
class SpiErrorCategory : public ErrorCategory
{
public:
  // Get the error category's name
  constexpr std::string_view name() const noexcept override { return "spi_interface"; }

  // Get the error's helper message
  constexpr std::string_view message(int error_value) const noexcept override
  {
    switch (static_cast<GenericErrorCode>(error_value))
    {
      case GenericErrorCode::kSuccess: return "Success";
      case GenericErrorCode::kNotImplemented: return "SPI feature not implemented";
      case GenericErrorCode::kInvalidParameter: return "Invalid SPI input parameter";
      case GenericErrorCode::kNotConfigured: return "SPI resource is not properly configured";
      case GenericErrorCode::kNullPointer: return "A null pointer was detected on SPI";
      case GenericErrorCode::kBadHandle: return "Invalid handle to the SPI resource";
      case GenericErrorCode::kTimedOut: return "SPI operation took more time than expected";
      case GenericErrorCode::kBusy: return "SPI bus already in use by another controller";
      case GenericErrorCode::kOverrun: return "New data arrived on SPI before old data was read from the hardware";
      case GenericErrorCode::kUnderrun: return "SPI hardware is ready for new data, but no data is available for transmission";
      case GenericErrorCode::kCrc: return "CRC check failed";
      default: return "Unknown SPI error";
    }
  }

  // Get an instance of the error category
  static inline const ErrorCategory &getCategory()
  {
    static SpiErrorCategory instance;
    return instance;
  }
};


typedef uint16_t SpiHandle_t;

typedef struct
{
  bool run;
  DrvBuffer_t data;
  uint32_t timeout;
  iDIO *cs_pin;
  bool cs_active_state;
  iCallback *event_handler;
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

  iSpiBus() = default;

  virtual ~iSpiBus() = default;

  virtual ErrorCode configure(const SettingsList_t *list, uint8_t list_size) = 0;

  virtual ErrorCode read(iDIO &cs_pin, bool cs_active_state, Buffer_t data, uint32_t timeout, iCallback &event_handler) = 0;

  virtual ErrorCode write(iDIO &cs_pin, bool cs_active_state, Buffer_t data, uint32_t timeout, iCallback &event_handler) = 0;
};

/**
 * @brief Interface class for SPI peripherals
 */
class iSPI : public iComm
{
public:

  iSPI() = default;

  virtual ~iSPI() = default;

};

/**
 * @brief Base class for SPI peripherals
 */
class bSPI : public iSPI
{
public:

  bSPI(iSpiBus &bus, iDIO &cs_pin, bool cs_active_state) :
  m_bus(bus), m_cs_pin(cs_pin), m_cs_active_state(cs_active_state),
  m_cb_function_rx(nullptr), m_cb_function_tx(nullptr)
  {
  }

  virtual ~bSPI() = default;

  virtual ErrorCode configure(const SettingsList_t *list, uint8_t list_size) override
  {
    return m_bus.configure(list, list_size);
  }

  virtual ErrorCode read(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.read(m_cs_pin, m_cs_active_state, data, timeout, *m_cb_function_rx);
  }

  virtual ErrorCode write(Buffer_t data, uint32_t timeout = UINT32_MAX) override
  {
    return m_bus.write(m_cs_pin, m_cs_active_state, data, timeout, *m_cb_function_tx);
  }

  virtual ErrorCode setCallback(EventsList_t event, iCallback &event_handler) override
  {
    ErrorCode status(GenericErrorCode::kSuccess, SpiErrorCategory::getCategory());
    switch(event)
    {
    case EVENT_READ:
      m_cb_function_rx = &event_handler;
      break;
    case EVENT_WRITE:
      m_cb_function_tx = &event_handler;
      break;
    default:
      status.setValue(GenericErrorCode::kInvalidParameter);
      status.setMessage("Invalid callback event for SPI");
      break;
    }
    return status;
  }

protected:
  iSpiBus &m_bus;
  iDIO &m_cs_pin;
  bool m_cs_active_state;
  iCallback *m_cb_function_rx, *m_cb_function_tx;
};


#endif /* PERIPHERALS_BASE_SPI_INTERFACE_HPP */
