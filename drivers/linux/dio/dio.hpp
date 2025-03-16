/**
 * @file dio.hpp
 * @author your name (you@domain.com)
 * @brief Give access to digital inputs and outputs on linux
 * @version 0.1
 * @date 2024-10-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVERS_LINUX_DIO_DIO_HPP
#define DRIVERS_LINUX_DIO_DIO_HPP

#include "peripherals_base/dio_base.hpp"
#include "linux/utils/linux_types.hpp"

/**
 * @brief Class that export DIO functionalities
 */
class DIO final : public DioBase
{
public:

  DIO(uint32_t line_offset, uint32_t chip_number = 0);
  virtual ~DIO();

  Status_t configure(const DriverSettings_t *list, uint8_t list_size);

  Status_t read(uint32_t &state);

  Status_t write(uint32_t value);

  Status_t toggle();

  Status_t setCallback(DriverEventsList_t edge = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

  Status_t enableCallback(bool enable, DriverEventsList_t edge = EVENT_NONE);

private:
  uint32_t m_chip_number;
  uint32_t m_line_number;
  void *m_chip_handle;
  void *m_line_handle;
  UtilsInOutSync_t m_sync;
  int m_flags;
  bool m_value;

  void readAsyncThread(void);
};

#endif /* DRIVERS_LINUX_DIO_DIO_HPP */
