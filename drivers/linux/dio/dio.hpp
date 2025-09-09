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

#include "peripherals_base/dio_interface.hpp"
#include "linux/utils/linux_types.hpp"

/**
 * @brief Class that export DIO functionalities
 */
class DIO final: public iDIO
{
public:

  DIO(uint32_t line_offset, uint32_t chip_number = 0);

  ~DIO();

  Status_t configure(const SettingsList_t *list, uint8_t list_size) override;

  Status_t read(bool &state) override;

  Status_t write(bool value) override;

  Status_t toggle() override;

  Status_t setEventCallback(EventsList_t edge, iCallback &event_handler) override;

  Status_t enableInterruption(bool enable) override;

private:
  uint32_t m_chip_number;
  uint32_t m_line_number;
  void *m_chip_handle;
  void *m_line_handle;
  UtilsInOutSync_t m_sync;
  int m_flags;
  bool m_value;
  iCallback *m_event_handler;
  EventsList_t m_edge;

  void readAsyncThread(void);
};

#endif /* DRIVERS_LINUX_DIO_DIO_HPP */
