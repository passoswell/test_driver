/**
 * @file inoutstream_interface_types.hpp
 * @author your name (you@domain.com)
 * @brief Set of useful data types and definitions for InOutStreams
 * @version 0.1
 * @date 2024-09-19
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef INOUTSTREAM_INTERFACE_TYPES_HPP
#define INOUTSTREAM_INTERFACE_TYPES_HPP


#include <stdint.h>
#include <stdbool.h>
#include <atomic>
#include <span>

#include "commons.hpp"

/**
 * @brief Definition of a callback function type for inoutstream callbacks
 *
 * @param error An error code
 * @param buffer Data pointer supplied by the caller
 * @param size Number of bytes successfully transferred during operation
 * @param arg An argument supplied by the caller
 * @return Status_t
 */
using InOutStreamCallback_t = std::function<Status_t(Status_t error, const uint8_t *buffer, uint32_t size, void *arg)>;

/**
 * @brief Parameter - value pair for configuration
 */
typedef struct
{
  uint8_t parameter;
  uint32_t value;
}InOutStreamSettings_t;

/**
 * @brief Define a mutex structure
 */
typedef struct
{
  std::atomic_flag lock;
  uint32_t key;
} InOutStreamMutex_t;

/**
 * @brief Macro to make it easy to add configuration parameters to a list
 */
#define ADD_PARAMETER(parameter, value) {parameter, value}

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
// Represents a state where the lock is free
constexpr uint32_t INOUTSTREAM_EMPTY_KEY = UINT32_MAX - 1;
// Represents a state where the key system is bypassed
constexpr uint32_t INOUTSTREAM_MASTER_KEY = UINT32_MAX;


#endif /* INOUTSTREAM_INTERFACE_TYPES_HPP */