/**
 * @file dio_interface.hpp
 * @author your name (you@domain.com)
 * @brief Interface class for digital inputs and outputs
 * @version 0.1
 * @date 2024-09-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DIO_INTERFACE_HPP
#define DIO_INTERFACE_HPP

#include <stdio.h>
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
}DioConfigure_t;

/**
 * @brief Interface class for digital inputs and outputs
 */
class DioInterface
{
public:
  uint8_t m_dio_line_number;
  DioCallback_t m_func;
  void *m_arg;

  // Configure a parameter
  Status_t configure(uint8_t parameter, uint32_t value);

  // Configure a list of parameters
  Status_t configure(const DioConfigureList_t *list, uint8_t list_size);

  // Read from a digital pin
  Status_t read(bool &state);

  // Write to a digital output pin
  Status_t write(bool state);

  // Toggle the state of a digital output
  Status_t toggle();

  // Install an event callback function
  Status_t setCallback(DioEdge_t edge, DioCallback_t func = nullptr, void *arg = nullptr);
};

#endif