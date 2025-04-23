/**
 * @file driver_out_base.hpp
 * @author your name (you@domain.com)
 * @brief Base class for output drivers
 * @version 0.1
 * @date 2024-10-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVER_OUT_BASE_HPP
#define DRIVER_OUT_BASE_HPP

#include "commons.hpp"
#include "driver_base.hpp"

class DriverOutBase : virtual public DriverBase
{
public:

  Size_t m_bytes_written;
  Size_t m_bytes_pending;
  Status_t m_write_status;
  DriverCallback_t m_func_tx;
  void *m_arg_tx;
  bool m_is_async_mode_tx;

  DriverOutBase();
  virtual ~DriverOutBase();

  // Transmit data
  virtual Status_t write(bool data);
  virtual Status_t write(float data);
  virtual Status_t write(uint8_t *data, Size_t byte_count, uint32_t timeout = UINT32_MAX);
  // virtual Status_t write(Buffer_t data, uint32_t timeout = UINT32_MAX);

  // Bytes exchanged
  virtual Size_t getBytesPending();
  virtual Size_t getBytesWritten();

  // Error handling
  virtual Status_t getWriteStatus();
};

#endif /* DRIVER_OUT_BASE_HPP*/