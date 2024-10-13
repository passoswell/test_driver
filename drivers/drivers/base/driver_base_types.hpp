/**
 * @file driver_base_types.hpp
 * @author your name (you@domain.com)
 * @brief Type definitions for driver configuration
 * @version 0.1
 * @date 2024-10-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVER_BASE_TYPES_HPP
#define DRIVER_BASE_TYPES_HPP

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

typedef enum
{
  DIO_LINE_DIRECTION,
  DIO_LINE_DRIVE,
  DIO_LINE_BIAS,
  DIO_LINE_EDGE,
  DIO_LINE_INITIAL_VALUE,
  DIO_LINE_ACTIVE_STATE,
} DriverParamList_t;

/**
 * @brief Parameter - value pair for configuration
 */
typedef struct
{
  uint8_t parameter;
  uint32_t value;
}DriverSettings_t;

/**
 * @brief Macro to make it easy to add configuration parameters to a list
 */
#define ADD_PARAMETER(parameter, value) {parameter, value}

#endif /* DRIVER_BASE_TYPES_HPP */