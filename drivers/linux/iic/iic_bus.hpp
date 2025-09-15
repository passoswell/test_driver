/**
 * @file iic_bus.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-07-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef IIC_IIC_BUS_HPP
#define IIC_IIC_BUS_HPP


#include <cstdio>
#include <cstdbool>

// #include "peripherals_base/iic_base.hpp"
#include "peripherals_base/iic_interface.hpp"
#include "linux/iic/iic_types.hpp"
#include "linux/utils/linux_types.hpp"
#include "linux/task_system/task_system.hpp"
#if __has_include("setup.hpp")
#include "setup.hpp"
#endif

#ifndef IIC_QUEUE_SIZE
#define IIC_QUEUE_SIZE                                                         1
#endif

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
  Mutex m_mutex;
  Task<IicDataBundle_t, IIC_QUEUE_SIZE, ErrorCode, 0> m_thread_handle;
  int m_fd;
  bool m_is_async_mode_rx, m_is_async_mode_tx;
  bool m_is_configured;

  IicBus();

  ~IicBus();

  ErrorCode blockingRead(DrvBuffer_t data, uint16_t address);

  ErrorCode blockingWrite(const DrvBuffer_t data, uint16_t address);

  static ErrorCode asyncTransferThread(IicDataBundle_t data_bundle, void *user_arg);

  ErrorCode checkInputs(const DrvBuffer_t data, uint32_t timeout);
};

#include "iic_bus.tpp"

#endif /* IIC_IIC_BUS_HPP */
