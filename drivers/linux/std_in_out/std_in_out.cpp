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

#include "linux/utils/linux_io.hpp"

struct termios StdInOut::m_backup_termios_structure = {};;
uint32_t StdInOut::m_termios_counter = 0;

/**
* @brief Constructor
*/
StdInOut::StdInOut() : LinuxSerialFile(STD_OUT_FILE)
{
  // Nothing is done here
}

/**
* @brief Destructor
*/
StdInOut::~StdInOut()
{
  FILE *handle = nullptr;
  if(m_termios_counter == 1)
  {
    handle = fopen("/dev/fd/0", "r+");
    if(handle != nullptr)
    {
      tcsetattr(handle->_fileno, TCSANOW, &m_backup_termios_structure);
      fclose(handle);
    }
    m_termios_counter--;
  }else if (m_termios_counter > 0)
  {
    m_termios_counter--;
  }
}

/**
 * @brief Configure a list of parameters
 * @param list List of parameter-value pairs
 * @param list_size Number of parameters on the list
 * @return Status_t
 */
Status_t StdInOut::configure(const DriverSettings_t *list, uint8_t list_size)
{
  Status_t status;

  if(m_termios_counter == 0)
  {
    FILE *handle = fopen("/dev/fd/0", "r+");
    if(handle == nullptr)
    {
      SET_STATUS(status, false, SRC_DRIVER, ERR_FAILED, (char *)"Failed to open file.\r\n");
      return status;
    }
    tcgetattr(handle->_fileno, &m_backup_termios_structure);
    fclose(handle);
  }

  m_termios_counter++;

  return LinuxSerialFile::configure(list, list_size);
}