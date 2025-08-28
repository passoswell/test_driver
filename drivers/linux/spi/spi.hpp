/**
 * @file spi.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-11-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVERS_LINUX_SPI_SPI_HPP
#define DRIVERS_LINUX_SPI_SPI_HPP

#include "linux/spi/spi_bus.hpp"

template<SpiHandle_t PORT_NUMBER>
class SPI : public bSPI
{
public:

  SPI(iDIO &cs_pin, bool cs_active_state) : bSPI(IicBus<PORT_NUMBER>::getInstance(), cs_pin, cs_active_state)
  {
    ;
  }

  ~SPI() = default;
};

// #include "peripherals_base/spi_base.hpp"
// #include "linux/spi/spi_bus.hpp"
// #include "linux/dio/dio.hpp"
// #include "linux/utils/linux_types.hpp"
// #include "linux/task_system/task_system.hpp"


// /**
//  * @brief Base class for spi drivers
//  */
// template<SpiHandle_t PORT_NUMBER>
// class SPI : public SpiBase
// {
// public:
//   SPI(uint32_t cs_line_offset, uint32_t cs_chip_number = 0) :
//   m_cs(cs_line_offset, cs_chip_number),
//   m_bus(SpiBus<PORT_NUMBER>::getInstance())
//   {}

//   ~SPI(){}

//   Status_t configure(const SettingsList_t *list, uint8_t list_size)
//   {
//     return m_bus.configure(list, list_size);
//   }

//   using DriverInOutBase::read;
//   Status_t read(DrvBuffer_t data, uint32_t timeout = UINT32_MAX)
//   {
//     return m_bus.read(m_cs, data, timeout, m_func_rx, m_arg_rx);
//   }

//   using DriverInOutBase::write;
//   Status_t write(DrvBuffer_t data, uint32_t timeout = UINT32_MAX)
//   {
//     return m_bus.write(m_cs, data, timeout, m_func_tx, m_arg_tx);
//   }

//   Status_t setCallback(EventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr)
// {
//   Status_t status = STATUS_DRV_SUCCESS;

//   switch (event)
//   {
//   case EVENT_READ:
//     if(m_read_status.code != OPERATION_RUNNING)
//     {
//       m_func_rx = function;
//       m_arg_rx = user_arg;
//     }else
//     {
//       status = STATUS_DRV_ERR_BUSY;
//     }
//     break;
//   case EVENT_WRITE:
//     if (m_write_status.code != OPERATION_RUNNING)
//     {
//       m_func_tx = function;
//       m_arg_tx = user_arg;
//     }else
//     {
//       status = STATUS_DRV_ERR_BUSY;
//     }
//     break;
//   default:
//     status = STATUS_DRV_ERR_PARAM;
//     break;
//   }

//   return status;
// }

// private:
//   SpiBus<PORT_NUMBER> &m_bus;
//   DIO m_cs;
// };

// // #include "spi.tpp"

#endif /* DRIVERS_LINUX_SPI_SPI_HPP */