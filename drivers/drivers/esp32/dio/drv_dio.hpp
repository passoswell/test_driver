/**
 * @file drv_dio.hpp
 * @author your name (you@domain.com)
 * @brief Give access to digital inputs and outputs on esp32
 * @version 0.1
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRV_DIO_HPP
#define DRV_DIO_HPP


#include <stdio.h>
#include <stdbool.h>

#include "commons.hpp"
#include "drivers/base/drv_dio_base.hpp"

/**
 * @brief Class that export DIO functionalities
 */
class DrvDIO : public  DrvDioBase
{
public:

  DrvDIO(uint8_t line_offset);
  virtual ~DrvDIO();

  // Status_t configure(uint8_t parameter, uint32_t value);
  Status_t configure(const DioConfigure_t *list, uint8_t list_size);

  Status_t read(bool &state);
  Status_t write(bool value);
  Status_t toggle();

  Status_t setCallback(DioEdge_t edge, DioCallback_t func = nullptr, void *arg = nullptr);

  void callback(void);

private:
  int m_flags;
  bool m_value;
};

#endif /* DRV_DIO_HPP */