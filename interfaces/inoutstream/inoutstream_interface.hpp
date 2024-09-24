/**
 * @file inoutstream_interface.hpp
 * @author your name (you@domain.com)
 * @brief Interface class for communication
 * @version 0.1
 * @date 2024-06-01
 *
 * @copyright Copyright (c) 2024
 *
 */


#ifndef INOUTSTREAM_INTERFACE_HPP
#define INOUTSTREAM_INTERFACE_HPP


#include "inoutstream_interface_types.hpp"


class InOutStreamInterface
{
public:

  virtual ~InOutStreamInterface(){};

  // Configure a parameter
  virtual Status_t configure(uint8_t parameter, uint32_t value) = 0;

  // Configure a list of parameters
  virtual Status_t configure(const InOutStreamSettings_t *list, uint8_t list_size) = 0;

  // Lock a peripheral to one key (e.g., chip select, address, etc.)
  virtual Status_t lock(uint32_t key) = 0;

  // Unlock a peripheral
  virtual Status_t unlock(uint32_t key) = 0;

  // Read data synchronously
  virtual Status_t read(uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout = UINT32_MAX) = 0;

  // Write data synchronously
  virtual Status_t write(uint8_t *buffer, uint32_t size, uint32_t key, uint32_t timeout = UINT32_MAX) = 0;

  // Read data asynchronously
  virtual Status_t readAsync(uint8_t *buffer, uint32_t size, uint32_t key, InOutStreamCallback_t func = nullptr, void *arg = nullptr) = 0;

  // Abort an ongoing readAsync operation
  virtual Status_t abortReadAsync() = 0;

  // Asynchronous read operation status
  virtual bool isReadAsyncDone() = 0;

  // Return the number of bytes read in the last readAsync operation
  virtual uint32_t bytesRead() = 0;

  // Write data asynchronously
  virtual Status_t writeAsync(uint8_t *buffer, uint32_t size, uint32_t key, InOutStreamCallback_t func = nullptr, void *arg = nullptr) = 0;

  // Abort an ongoing writeAsync operation
  virtual Status_t abortWriteAsync() = 0;

  // Write operation status
  virtual bool isWriteAsyncDone() = 0;

  // Return the number of bytes written in the last writeAsync operation
  virtual uint32_t bytesWritten() = 0;

  // Asynchronous operation (read, write or read-write) status
  virtual bool isOperationDone() = 0;
};


#endif /* INOUTSTREAM_INTERFACE_HPP */
