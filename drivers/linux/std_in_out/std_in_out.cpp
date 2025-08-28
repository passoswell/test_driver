/**
 * @file std_in_out.cpp
 * @author your name (you@domain.com)
 * @brief Give access to standard io as a serial port
 * @version 0.1
 * @date 2024-06-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "linux/std_in_out/std_in_out.hpp"

#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <termios.h>

#include "linux/utils/linux_io.hpp"

/**
* @brief Constructor
*/
StdInOut::StdInOut() : LinuxSerialFile(STD_OUT_FILE)
{
}

/**
* @brief Destructor
*/
StdInOut::~StdInOut()
{
  FILE *handle = nullptr;
  struct termios backup_termios_structure;

  backup_termios_structure.c_iflag = 27906;
  backup_termios_structure.c_oflag = 5;
  backup_termios_structure.c_cflag = 1215;
  backup_termios_structure.c_lflag = 35387;
  backup_termios_structure.c_line = 0;
  for(uint8_t i = 0; i < sizeof(backup_termios_structure.c_cc); i++)
  {
    backup_termios_structure.c_cc[i] = 0;
  }
  backup_termios_structure.c_cc[0] = 3;
  backup_termios_structure.c_cc[1] = 28;
  backup_termios_structure.c_cc[2] = 127;
  backup_termios_structure.c_cc[3] = 21;
  backup_termios_structure.c_cc[4] = 4;
  backup_termios_structure.c_cc[5] = 0;
  backup_termios_structure.c_cc[6] = 1;
  backup_termios_structure.c_cc[7] = 0;
  backup_termios_structure.c_cc[8] = 17;
  backup_termios_structure.c_cc[9] = 19;
  backup_termios_structure.c_cc[10] = 26;
  backup_termios_structure.c_cc[11] = 255;
  backup_termios_structure.c_cc[12] = 18;
  backup_termios_structure.c_cc[13] = 15;
  backup_termios_structure.c_cc[14] = 23;
  backup_termios_structure.c_cc[15] = 22;
  backup_termios_structure.c_cc[16] = 255;
  backup_termios_structure.c_ispeed = 15;
  backup_termios_structure.c_ospeed = 15;

  handle = fopen("/dev/fd/0", "r+");
  if(handle != nullptr)
  {
    tcsetattr(handle->_fileno, TCSANOW, &backup_termios_structure);
    fclose(handle);
  }
}

/**
 * @brief Return a reference to a singleton of the type StdInOut
 * @return StdInOut&
 */
StdInOut& StdInOut::get_instance()
{
  static StdInOut instance;
  return instance;
}