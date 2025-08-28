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


#include <stdio.h>
#include <stdbool.h>

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

typedef uint16_t IicHandle_t;

template<IicHandle_t PORT_NUMBER>
class IicBus : public iIicBus
{
public:
  static iIicBus& getInstance()
  {
    static IicBus<PORT_NUMBER> instance;
    return instance;
  }

  Status_t configure(const SettingsList_t *list, uint8_t list_size) override;

  Status_t read(uint16_t address, Buffer_t data, uint32_t timeout = UINT32_MAX, Callback_t cb_function = nullptr, void *cb_arg = nullptr) override;

  Status_t write(uint16_t address, Buffer_t data, uint32_t timeout = UINT32_MAX, Callback_t cb_function = nullptr, void *cb_arg = nullptr) override;

  // A singleton should not be cloneable nor assignable
  IicBus(const IicBus&) = delete;
  IicBus(IicBus&&) = delete;
  IicBus& operator=(const IicBus&) = delete;
  IicBus& operator=(IicBus&&) = delete;

private:
  Mutex m_mutex;
  Task<IicDataBundle_t, IIC_QUEUE_SIZE, Status_t, 0> m_thread_handle;
  int m_fd;
  bool m_is_async_mode_rx, m_is_async_mode_tx;
  bool m_is_configured;

  IicBus();

  ~IicBus();

  Status_t blockingRead(DrvBuffer_t data, uint16_t address);

  Status_t blockingWrite(const DrvBuffer_t data, uint16_t address);

  static Status_t asyncTransferThread(IicDataBundle_t data_bundle, void *user_arg);

  Status_t checkInputs(const DrvBuffer_t data, uint32_t timeout);
};

#include "iic_bus.tpp"

#endif /* IIC_IIC_BUS_HPP */
