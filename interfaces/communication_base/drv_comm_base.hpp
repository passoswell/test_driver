/**
 * @file drv_comm_base.hpp
 * @author user (contact@email.com)
 * @brief Base class with default behavior for communication drivers
 * @date 2024-06-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRV_COMM_BASE_HPP
#define DRV_COMM_BASE_HPP


#include "inoutstream/inoutstream_interface.hpp"


/**
 * @brief List of valid parameters to use with configure method
 * @note Not all options have an effect on a given driver
 */
typedef enum
{
  DRV_PARAM_BAUD,
  DRV_PARAM_CLOCK_SPEED,
  DRV_PARAM_TX_GPIO,
  DRV_PARAM_RX_GPIO,
  DRV_PARAM_CK_GPIO,
  DRV_PARAM_CS_PARAM,
  DRV_PARAM_CTS_GPIO,
  DRV_PARAM_RTS_GPIO,
  DRV_PARAM_STOP_BITS,
  DRV_PARAM_LINE_MODE,
  DRV_PARAM_START_DELAY_US,
  DRV_USE_HW_PARITY,
  DRV_USE_HW_FLOW_CTRL,
  DRV_USE_HW_CRC,
  DRV_USE_HW_CKSUM,
  DRV_USE_PULL_UP,
}DrvCommBaseParameters_t;


class DrvCommBase : public InOutStreamInterface
{
public:

  void *m_handle;
  bool m_is_read_done;
  bool m_is_write_done;
  bool m_is_operation_done;
  InOutStreamMutex_t m_mutex;
  bool m_is_initialized;
  uint32_t m_bytes_read;

  DrvCommBase();

  virtual ~DrvCommBase();

  virtual Status_t configure(uint8_t parameter, uint32_t value);
  virtual Status_t configure(const InOutStreamSettings_t *list, uint8_t list_size);

  Status_t lock(uint32_t key);
  Status_t unlock(uint32_t key);

  virtual Status_t read(uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout = UINT32_MAX);
  virtual Status_t read(Buffer_t buffer, uint32_t key, uint32_t timeout = UINT32_MAX);

  virtual Status_t write(uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout = UINT32_MAX);
  virtual Status_t write(Buffer_t buffer, uint32_t key, uint32_t timeout = UINT32_MAX);

  virtual Status_t readAsync(uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout = UINT32_MAX, InOutStreamCallback_t func = nullptr, void *arg = nullptr);
  virtual Status_t readAsync(Buffer_t buffer, uint32_t key, uint32_t timeout = UINT32_MAX, InOutStreamCallback_t func = nullptr, void *arg = nullptr);
  virtual Status_t abortReadAsync();
  virtual bool isReadAsyncDone();
  virtual uint32_t bytesRead();

  virtual Status_t writeAsync(const uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout = UINT32_MAX, InOutStreamCallback_t func = nullptr, void *arg = nullptr);
  virtual Status_t writeAsync(const Buffer_t buffer, uint32_t key, uint32_t timeout = UINT32_MAX, InOutStreamCallback_t func = nullptr, void *arg = nullptr);
  virtual Status_t abortWriteAsync();
  virtual bool isWriteAsyncDone();
  virtual uint32_t bytesWritten();

  virtual bool isOperationDone();

  static uint8_t getID();
};


#endif /* DRV_COMM_BASE_HPP */
