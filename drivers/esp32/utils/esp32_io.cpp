/**
 * @file esp32_io.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "esp32_io.hpp"

/**
 * @brief Convert esp_err_t code into Status_t code
 *
 * @param code esp_err_t code
 * @return Status_t
 */
Status_t convertErrorCode(esp_err_t code)
{
  Status_t status;

  // TODO: Implement a thorough translation between error codes
  switch(code)
  {
    case ESP_OK:
      status = STATUS_DRV_SUCCESS;
      break;
    case ESP_FAIL:
      SET_STATUS(status, false, SRC_HAL, ERR_FAILED, (char *) esp_err_to_name(code));
      break;
    case ESP_ERR_NO_MEM:
      SET_STATUS(status, false, SRC_HAL, ERR_MEM_FULL, (char *) esp_err_to_name(code));
      break;
    case ESP_ERR_INVALID_ARG:
      SET_STATUS(status, false, SRC_HAL, ERR_PARAM, (char *) esp_err_to_name(code));
      break;
    case ESP_ERR_INVALID_SIZE:
      SET_STATUS(status, false, SRC_HAL, ERR_PARAM_SIZE, (char *) esp_err_to_name(code));
      break;
    case ESP_ERR_NOT_FOUND:
      SET_STATUS(status, false, SRC_HAL, ERR_NOT_FOUND, (char *) esp_err_to_name(code));
      break;
    case ESP_ERR_NOT_SUPPORTED:
      SET_STATUS(status, false, SRC_HAL, ERR_NOT_IMPLEMENTED, (char *) esp_err_to_name(code));
      break;
    case ESP_ERR_TIMEOUT:
      SET_STATUS(status, false, SRC_HAL, ERR_TIMEOUT, (char *) esp_err_to_name(code));
      break;
    case ESP_ERR_INVALID_CRC:
      SET_STATUS(status, false, SRC_HAL, ERR_CRC, (char *) esp_err_to_name(code));
      break;
    default:
      status.code = ERR_UNKNOWN_ERROR;
      status.description = (char *) esp_err_to_name(code);
      status.source = SRC_HAL;
      status.success = false;
      break;
  }
  return status;
}