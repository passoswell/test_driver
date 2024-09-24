/**
 * @file dio_interface_types.hpp
 * @author your name (you@domain.com)
 * @brief Set of useful data types and definitions for DIOs
 * @version 0.1
 * @date 2024-09-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DIO_INTERFACE_TYPES_HPP
#define DIO_INTERFACE_TYPES_HPP

#include <stdint.h>
#include <stdbool.h>

#include "commons.hpp"

/**
 * @brief List of possible dio directions
 */
typedef enum
{
  DIO_DIRECTION_INPUT,
  DIO_DIRECTION_OUTPUT,
}DioDirection_t;

/**
 * @brief List of possible dio drives
 */
typedef enum
{
  DIO_DRIVE_PUSH_PULL,
  DIO_DRIVE_OPEN_DRAIN,
  DIO_DRIVE_OPEN_SOURCE,
}DioDrive_t;

/**
 * @brief List of possible dio bias
 */
typedef enum
{
  DIO_BIAS_DISABLED,
  DIO_BIAS_PULL_UP,
  DIO_BIAS_PULL_DOWN,
}DioBias_t;

/**
 * @brief List of possible dio edge events
 */
typedef enum
{
  DIO_EDGE_NONE,
  DIO_EDGE_RISING,
  DIO_EDGE_FALLING,
  DIO_EDGE_BOTH,
}DioEdge_t;

/**
 * @brief List of possible dio states
 */typedef enum
{
  DIO_STATE_LOW = 0,
  DIO_STATE_HIGH,
}DioActiveState_t;

/**
 * @brief Configuration list for configurable DIO parameters
 */
typedef enum
{
  DIO_LINE_DIRECTION,
  DIO_LINE_DRIVE,
  DIO_LINE_BIAS,
  DIO_LINE_EDGE,
  DIO_LINE_INITIAL_VALUE,
  DIO_LINE_ACTIVE_STATE,
} DioConfigureList_t;

/**
 * @brief Macro to make it easy to add configuration parameters to a list
 */
#define ADD_DIO_PARAMETER(parameter, value) {parameter, value}

/**
 * @brief Type definition for callback functions
 */
typedef void (*DioCallback_t)(Status_t status, uint16_t line, DioEdge_t edge, bool state, void *arg);

/**
 * @brief Parameter - value pair for configuration
 */
typedef struct
{
  uint8_t parameter;
  uint32_t value;
}DioSettings_t;

constexpr uint32_t DIO_INVALID_LINE = UINT32_MAX;
constexpr uint32_t DIO_INVALID_PORT = UINT32_MAX;


#endif /* DIO_INTERFACE_TYPES_HPP */