/**
 * @file inoutstream.hpp
 * @author your name (you@domain.com)
 * @brief Base class for communication
 * @version 0.1
 * @date 2024-06-01
 *
 * @copyright Copyright (c) 2024
 *
 */


#ifndef INOUTSTREAM_HPP
#define INOUTSTREAM_HPP


#include <stdint.h>
#include <stdbool.h>
#include <atomic>
#include "com_status.hpp"

/**
 * @brief Definition of a callback function type for inoutstream callbacks
 *
 * @param error An error code
 * @param buffer Data pointer supplied by the caller
 * @param size Number of bytes successfully transfered during operation
 * @param arg An argument supplied by the caller
 * @return Status_t
 */
typedef Status_t (*InOutStreamCallback_t)(Status_t error, uint8_t *buffer, uint32_t size, void *arg);

/**
 * @brief Parameter - value pair for configuration
 */
typedef struct
{
  uint8_t parameter;
  const void *p_value;
}InOutStreamConfigure_t;

/**
 * @brief Define a mutex structure
 */
typedef struct
{
  std::atomic_flag lock;
  uint8_t key;
} InOutStreamMutex_t;

/**
 * @brief Macro to make it easy to add configuration parameters to a list
 */
#define ADD_PARAMETER(parameter, value) {parameter, value},

/**
 * @brief List of possible events
 */
typedef enum
{
  INOUTSTREAM_TRANSMIT,
  INOUTSTREAM_RECEIVE,
  INOUTSTREAM_TRANSMIT_RECEIVE,
}InOutStreamEvent_t;


constexpr uint8_t INOUTSTREAM_INVALID_ID = 0;
constexpr uint32_t INOUTSTREAM_NO_KEY = 0xFF;


class InOutStream
{
public:

  virtual ~InOutStream(){};

  virtual Status_t configure(uint8_t parameter, const void *p_value) = 0;
  virtual Status_t configure(const InOutStreamConfigure_t *list, uint8_t list_size) = 0;

  virtual Status_t lock(uint8_t key) = 0;
  virtual Status_t unlock(uint8_t key) = 0;

  virtual Status_t read(uint8_t *buffer, uint32_t size, uint8_t key, uint32_t timeout = UINT32_MAX) = 0;
  virtual Status_t write(uint8_t *buffer, uint32_t size, uint8_t key, uint32_t timeout = UINT32_MAX) = 0;

  virtual Status_t readAsync(uint8_t *buffer, uint32_t size, uint8_t key, InOutStreamCallback_t func = nullptr, void *arg = nullptr) = 0;
  virtual Status_t abortReadAsync() = 0;
  virtual bool isReadAsyncDone() = 0;
  virtual uint32_t bytesRead() = 0;
  virtual Status_t readAsyncDoneCallback(Status_t error, uint8_t *buffer, uint32_t size, void *arg = nullptr) = 0;

  virtual Status_t writeAsync(uint8_t *buffer, uint32_t size, uint8_t key, InOutStreamCallback_t func = nullptr, void *argr = nullptr) = 0;
  virtual Status_t abortWriteAsync() = 0;
  virtual bool isWriteAsyncDone() = 0;
  virtual uint32_t bytesWritten() = 0;
  virtual Status_t writeAsyncDoneCallback(Status_t error, uint8_t *buffer, uint32_t size, void *arg = nullptr) = 0;

  virtual bool isOperationDone() = 0;
};


#endif /* INOUTSTREAM_HPP */
