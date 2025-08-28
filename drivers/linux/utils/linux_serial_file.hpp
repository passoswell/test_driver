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

#include "peripherals_base/communication_interface.hpp"
#include "linux/utils/linux_types.hpp"
#include "linux/task_system/task_system.hpp"

class LinuxSerialFile : public iComm
{
public:
  LinuxSerialFile(const char *file_name);
  virtual ~LinuxSerialFile();

  Status_t configure(const SettingsList_t *list, uint8_t list_size) override;

  Status_t read(Buffer_t data, uint32_t timeout = UINT32_MAX) override;

  Status_t write(Buffer_t data, uint32_t timeout = UINT32_MAX) override;

  Status_t setCallback(EventsList_t event = EVENT_NONE, Callback_t cb_function = nullptr, void *cb_arg = nullptr) override;

  uint32_t getBytesRead()
  {
    return m_bytes_read;
  }

private:
  Task<DrvDataBundle_t, 1, Status_t, 0> m_rx_thread_handle;
  Task<DrvDataBundle_t, 1, Status_t, 0> m_tx_thread_handle;
  const char *m_handle;
  int m_linux_handle;
  bool m_terminate;
  bool m_is_async_mode_rx, m_is_async_mode_tx;
  uint32_t m_bytes_read;
  Callback_t m_func_rx, m_func_tx;
  void *m_arg_rx, *m_arg_tx;

  Status_t readBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back);
  static Status_t readFromThreadBlocking(DrvDataBundle_t data_bundle, void *self_ptr);

  Status_t writeBlocking(uint8_t *data, Size_t byte_count, uint32_t timeout, bool call_back);
  static Status_t writeFromThreadBlocking(DrvDataBundle_t data_bundle, void *self_ptr);
};

#endif /* DRIVERS_LINUX_UTILS_LINUX_SERIAL_FILE_HPP */