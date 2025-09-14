/**
 * @file dio.hpp
 * @author your name (you@domain.com)
 * @brief Give access to digital inputs and outputs on pipico
 * @version 0.1
 * @date 2024-10-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DIO_HPP
#define DIO_HPP

#include "peripherals_base/dio_interface.hpp"

/**
 * @brief Class that export DIO functionalities
 */
class DIO final: public iDIO
{
public:
  uint32_t m_line_number;
  iCallback *m_event_handler;
  static std::vector<DIO*> m_dio_ptr; // Vector of DIO pointers, used for the interruption callback

  DIO(uint32_t line_offsetline_offset, uint32_t port = 0);

  ~DIO();

  ErrorCode configure(const SettingsList_t *list, uint8_t list_size) override;

  ErrorCode read(bool &state) override;

  ErrorCode write(bool value) override;

  ErrorCode toggle() override;

  ErrorCode setEventCallback(EventsList_t edge, iCallback &event_handler) override;

  ErrorCode enableInterruption(bool enable) override;

private:
  void *m_line_handle;
  DioBias_t m_line_bias;
  int m_flags;
  bool m_value;
  EventsList_t m_edge;

  static void drvDioCallback(unsigned int dio, uint32_t events);
};

#endif /* DIO_HPP */