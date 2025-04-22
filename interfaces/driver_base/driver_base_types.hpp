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
 * @brief List of possible dio states
 */typedef enum
{
  DIO_STATE_LOW = 0,
  DIO_STATE_HIGH,
}DioActiveState_t;

/**
 * @brief List of possible dio edge events
 */
typedef enum
{
  EVENT_NONE,

  // DIO events
  EVENT_EDGE_RISING,
  EVENT_EDGE_FALLING,
  EVENT_EDGE_BOTH,

  // Communication events
  EVENT_READ,
  EVENT_WRITE,
  EVENT_READ_WRITE,
}EventsList_t;

typedef enum
{
  PARITY_NONE,
  PARITY_EVEN,
  PARITY_ODD
}ParityList_t;

typedef enum
{
  // DIO parameters
  DIO_LINE_DIRECTION,           /*!< One of DioDirection_t options */
  DIO_LINE_DRIVE,               /*!< One of DioDirection_t options */
  DIO_LINE_BIAS,                /*!< One of DioBias_t options */
  DIO_LINE_EDGE,                /*!< One of EventsList_t options */
  DIO_LINE_INITIAL_VALUE,       /*!< true or false, or DioActiveState_t */
  DIO_LINE_ACTIVE_STATE,        /*!< true or false, or DioActiveState_t */

  // Communication peripherals
  COMM_WORK_ASYNC_TX,           /*!< true for async, false for sync operation*/
  COMM_WORK_ASYNC_RX,           /*!< true for async, false for sync operation*/
  COMM_PARAM_BAUD,              /*!< Bit rate in bits per second*/
  COMM_PARAM_CLOCK_SPEED,       /*!< Bit rate in Hz */

  COMM_PARAM_TX_DIO,            /*!< DIO number for tx pin */
  COMM_PARAM_RX_DIO,            /*!< DIO number for rx pin */
  COMM_PARAM_CK_DIO,            /*!< DIO number for clock pin */
  COMM_PARAM_CS_DIO,            /*!< DIO number for chip select pin */
  COMM_PARAM_CS_POLARITY,       /*!< Active level (true or false) */
  COMM_PARAM_CTS_DIO,           /*!< DIO number for clear to send pin */
  COMM_PARAM_RTS_DIO,           /*!< DIO number for request to send pin */

  COMM_PARAM_STOP_BITS,         /*!< Number of stop bits */
  COMM_PARAM_PARITY,            /*!< One of ParityList_t options */
  COMM_PARAM_LINE_MODE,
  COMM_PARAM_START_DELAY_US,
  COMM_USE_HW_CRC,
  COMM_USE_HW_CKSUM,
  COMM_USE_PULL_UP,
} ParametersList_t;

/**
 * @brief Parameter - value pair for configuration
 */
typedef struct
{
  uint8_t parameter;
  uint32_t value;
}SettingsList_t;

/**
 * @brief Macro to make it easy to add configuration parameters to a list
 */
#define ADD_PARAMETER(parameter, value) {parameter, value}

using DriverCallback_t = std::function<Status_t(Status_t status, EventsList_t event, const Buffer_t data, void *user_arg)>;

#endif /* DRIVER_BASE_TYPES_HPP */