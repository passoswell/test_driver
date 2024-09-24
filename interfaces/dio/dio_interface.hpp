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

#include "dio_interface_types.hpp"

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
  Status_t configure(const DioSettings_t *list, uint8_t list_size);

  // Read from a digital pin
  Status_t read(bool &state);

  // Write to a digital output pin
  Status_t write(bool state);

  // Toggle the state of a digital output
  Status_t toggle();

  // Install an event callback function
  Status_t setCallback(DioEdge_t edge, DioCallback_t func = nullptr, void *arg = nullptr);
};

#endif /* DIO_INTERFACE_HPP */