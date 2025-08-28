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
class DIO : public iDIO
{
public:
  uint32_t m_line_number;
  Callback_t m_func;
  void *m_arg;
  static std::vector<DIO*> m_dio_ptr; // Vector of DIO pointers, used for the interruption callback

  DIO(uint32_t line_offsetline_offset, uint32_t port = 0);
  virtual ~DIO();

  Status_t configure(const SettingsList_t *list, uint8_t list_size) override;

  Status_t read(bool &state) override;

  Status_t write(bool value) override;

  Status_t toggle() override;

  Status_t setEventCallback(EventsList_t edge = EVENT_NONE, Callback_t function = nullptr, void *user_arg = nullptr) override;

  Status_t enableInterruption(bool enable) override;

private:
  void *m_line_handle;
  DioBias_t m_line_bias;
  int m_flags;
  bool m_value;
  EventsList_t m_edge;

  static void drvDioCallback(unsigned int dio, uint32_t events);
};

#endif /* DIO_HPP */