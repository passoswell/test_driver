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

#ifndef DRIVERS_LINUX_DIO_DIO_HPP
#define DRIVERS_LINUX_DIO_DIO_HPP

#include "peripherals_base/dio_base.hpp"
#include "esp32/task_system/task_system.hpp"

/**
 * @brief Class that export DIO functionalities
 */
class DIO : public  DioBase
{
public:

  DIO(uint32_t line_offset, uint32_t port = 0);
  virtual ~DIO();

  Status_t configure(const SettingsList_t *list, uint8_t list_size);

  Status_t read(bool &state);

  Status_t write(bool value);

  Status_t toggle();

  Status_t setCallback(EventsList_t edge = EVENT_NONE, DriverCallback_t function = nullptr, void *user_arg = nullptr);

  Status_t enableCallback(bool enable, EventsList_t edge = EVENT_NONE);

private:
  uint32_t m_line_number;
  bool m_value;
  TaskHandle_t m_obj_task_handle;
  TaskHandle_t m_dio_event_task_handle;
  bool m_terminate;

  static void callback(void *arg);

  void dioEventHandlerTask(void);

  void terminateDioEventHandlerTask(void);
};

#endif /* DRIVERS_LINUX_DIO_DIO_HPP */