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
class StdInOut : public LinuxSerialFile
{
public:

  static StdInOut& get_instance();

  // A singleton should not be cloneable nor assignable
  StdInOut(const StdInOut&) = delete;
  StdInOut(StdInOut&&) = delete;
  StdInOut& operator=(const StdInOut&) = delete;
  StdInOut& operator=(StdInOut&&) = delete;


private:

  StdInOut();

  ~StdInOut();
};

#endif /* DRIVERS_LINUX_STD_IN_OUT_STD_IN_OUT_HPP */