/**
 * @file std_in_out.hpp
 * @author your name (you@domain.com)
 * @brief Give access to standard io as a serial port
 * @version 0.1
 * @date 2024-06-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVERS_LINUX_STD_IN_OUT_STD_IN_OUT_HPP
#define DRIVERS_LINUX_STD_IN_OUT_STD_IN_OUT_HPP


#include <stdio.h>
#include <stdbool.h>

#include "linux/utils/linux_serial_file.hpp"

/**
 * @brief Gives access to standard io as a serial port
 */
class StdInOut final : public LinuxSerialFile
{
public:
  StdInOut();

  ~StdInOut();

  Status_t configure(const DriverSettings_t *list, uint8_t list_size);

private:
  static struct termios m_backup_termios_structure;
  static uint32_t m_termios_counter;
};

#endif /* DRIVERS_LINUX_STD_IN_OUT_STD_IN_OUT_HPP */