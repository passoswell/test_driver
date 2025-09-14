/**
 * @file esp32_io.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef UTILS_ESP32_IO_HPP
#define UTILS_ESP32_IO_HPP

#include <cstring>

#include "commons.hpp"
#include "esp_err.h"

std::string_view getErrorMessage(esp_err_t code);

ErrorCode convertErrorCode(esp_err_t code);

#endif /* UTILS_ESP32_IO_HPP */
