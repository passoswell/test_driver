/**
 * @file linux_serial_file.hpp
 * @author your name (you@domain.com)
 * @brief Gives access to a linux tty file as a serial port
 * @version 0.1
 * @date 2025-03-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef DRIVERS_LINUX_UTILS_LINUX_SERIAL_FILE_HPP
#define DRIVERS_LINUX_UTILS_LINUX_SERIAL_FILE_HPP

#include <stdio.h>
#include <stdbool.h>

#include "peripherals_base/uart_base.hpp"
#include "linux/utils/linux_types.hpp"
#include "linux/task_system/task_system.hpp"

class LinuxSerialFile : public UartBase
{
public:
  LinuxSerialFile(const char *file_name);
  virtual ~LinuxSerialFile();

  Status_t configure(const SettingsList_t *list, uint8_t list_size);

  using UartBase::read;
  Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  using UartBase::write;
  Status_t write(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);

  Status_t setCallback(EventsList_t event = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

private:
  Task<DataBundle_t, UART_QUEUE_SIZE, Status_t, 0> m_rx_thread_handle;
  Task<DataBundle_t, UART_QUEUE_SIZE, Status_t, 0> m_tx_thread_handle;
  int m_linux_handle;
  bool m_terminate;

  Status_t readBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back);
  static Status_t readFromThreadBlocking(DataBundle_t data_bundle, void *self_ptr);

  Status_t writeBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back);
  static Status_t writeFromThreadBlocking(DataBundle_t data_bundle, void *self_ptr);
};

#endif /* DRIVERS_LINUX_UTILS_LINUX_SERIAL_FILE_HPP */