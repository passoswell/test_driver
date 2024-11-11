/**
 * @file driver_in_base.hpp
 * @author your name (you@domain.com)
 * @brief Base class for input drivers
 * @version 0.1
 * @date 2024-10-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVER_IN_HPP
#define DRIVER_IN_HPP

#include "commons.hpp"
#include "driver_base.hpp"

class DriverInBase : virtual public DriverBase
{
public:

  Size_t m_bytes_read;
  Size_t m_bytes_available;
  Status_t m_read_status;
  DriverCallback_t m_func_rx;
  void *m_arg_rx;

  DriverInBase();
  virtual ~DriverInBase();

  // Receive data
  virtual Status_t read(uint32_t &data);
  virtual Status_t read(float &data);
  virtual Status_t read(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);
  virtual Status_t read(Buffer_t data, uint32_t timeout = UINT32_MAX);

  // Bytes exchanged
  virtual Size_t getBytesAvailable();
  virtual Size_t getBytesRead();

  // Error handling
  virtual Status_t getReadStatus();
};

#endif  // DRIVER_IN_HPP