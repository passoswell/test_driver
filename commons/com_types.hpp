/**
 * @file com_types.hpp
 * @author Useful type definitions
 * @brief
 * @version 0.1
 * @date 2024-09-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef COM_TYPES_HPP
#define COM_TYPES_HPP

#include <stdbool.h>
#include <stdint.h>
#include <span>
#include <functional>

using Buffer_t = std::span<uint8_t>;

using Size_t = int32_t;

using Callback_t = std::function<Status_t(Status_t status, uint8_t event, const Buffer_t data, void *user_arg)>;

#endif /* COM_TYPES_HPP */