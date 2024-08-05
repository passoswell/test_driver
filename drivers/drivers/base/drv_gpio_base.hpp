/**
 * @file drv_gpio_base.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRV_GPIO_BASE_HPP
#define DRV_GPIO_BASE_HPP


#include <stdio.h>
#include <stdbool.h>

#include "commons.hpp"

/**
 * @brief List of possible gpio directions
 */
typedef enum
{
  DRV_GPIO_DIRECTION_INPUT,
  DRV_GPIO_DIRECTION_OUTPUT,
}DrvGpioDirection_t;

/**
 * @brief List of possible gpio drives
 */
typedef enum
{
  DRV_GPIO_DRIVE_PUSH_PULL,
  DRV_GPIO_DRIVE_OPEN_DRAIN,
  DRV_GPIO_DRIVE_OPEN_SOURCE,
}DrvGpioDrive_t;

/**
 * @brief List of possible gpio bias
 */
typedef enum
{
  DRV_GPIO_BIAS_DISABLED,
  DRV_GPIO_BIAS_PULL_UP,
  DRV_GPIO_BIAS_PULL_DOWN,
}DrvGpioBias_t;

/**
 * @brief List of possible gpio edge events
 */
typedef enum
{
  DRV_GPIO_EDGE_NONE,
  DRV_GPIO_EDGE_RISING,
  DRV_GPIO_EDGE_FALLING,
  DRV_GPIO_EDGE_BOTH,
}DrvGpioEdge_t;

/**
 * @brief List of possible gpio states
 */typedef enum
{
  DRV_GPIO_STATE_LOW = 0,
  DRV_GPIO_STATE_HIGH,
}DrvGpioActiveState_t;

/**
 * @brief Configuration list for configurable GPIO parameters
 */
typedef enum
{
  DRV_GPIO_LINE_DIRECTION,
  DRV_GPIO_LINE_DRIVE,
  DRV_GPIO_LINE_BIAS,
  DRV_GPIO_LINE_EDGE,
  DRV_GPIO_LINE_INITIAL_VALUE,
  DRV_GPIO_LINE_ACTIVE_STATE,
} DrvGpioConfigureList_t;

/**
 * @brief Macro to make it easy to add configuration parameters to a list
 */
#define ADD_GPIO_PARAMETER(parameter, value) {parameter, value}

/**
 * @brief Type definition for callback functions
 */
typedef void (*DrvGpioCallback_t)(Status_t status, uint16_t line, DrvGpioEdge_t edge, bool state, void *arg);

/**
 * @brief Parameter - value pair for configuration
 */
typedef struct
{
  uint8_t parameter;
  uint32_t value;
}DrvGpioConfigure_t;

/**
 * @brief Class that export GPIO functionalities
 */
class DrvGpioBase
{
public:
  uint8_t m_gpio_line_number;
  DrvGpioCallback_t m_func;
  void *m_arg;

  Status_t configure(uint8_t parameter, uint32_t value);

  Status_t configure(const DrvGpioConfigureList_t *list, uint8_t list_size);

  Status_t read(bool &state);

  Status_t write(bool state);

  Status_t setCallback(DrvGpioEdge_t edge, DrvGpioCallback_t func = nullptr, void *arg = nullptr);
};

#endif /* DRV_GPIO_BASE_HPP */
