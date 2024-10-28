/**
 * @file dio.hpp
 * @author your name (you@domain.com)
 * @brief Give access to digital inputs and outputs on esp32
 * @version 0.1
 * @date 2024-10-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DIO_HPP
#define DIO_HPP

#include "drivers/base/peripherals_base/dio_base.hpp"

/**
 * @brief Class that export DIO functionalities
 */
class DIO : public  DioBase
{
public:
  uint32_t m_line_number;

  DIO(uint32_t line_offset, uint32_t port = 0);
  virtual ~DIO();

  Status_t configure(const DriverSettings_t *list, uint8_t list_size);

  Status_t read(uint32_t &state);

  Status_t write(uint32_t value);

  Status_t toggle();

  Status_t setCallback(DriverEventsList_t edge = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

  Status_t enableCallback(bool enable, DriverEventsList_t edge = EVENT_NONE);

  void callback(void);

private:
  int m_flags;
  bool m_value;
};

#endif /* DIO_HPP */