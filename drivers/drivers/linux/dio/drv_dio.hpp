/**
 * @file drv_dio.hpp
 * @author your name (you@domain.com)
 * @brief Give access to digital inputs and outputs on linux
 * @version 0.1
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRV_DIO_HPP
#define DRV_DIO_HPP


#include <stdint.h>
#include <stdbool.h>

#include "commons.hpp"
#include "drivers/base/drv_dio_base.hpp"
#include "drivers/linux/utils/linux_types.hpp"

/**
 * @brief Class that export DIO functionalities
 */
class DrvDIO final : public DrvDioBase
{
public:

  DrvDIO(uint32_t line_offset, uint32_t chip_number = 0);
  virtual ~DrvDIO();

  // Status_t configure(uint8_t parameter, uint32_t value);
  Status_t configure(const DioSettings_t *list, uint8_t list_size);

  Status_t read(bool &state);
  Status_t write(bool value);
  Status_t toggle();

  Status_t setCallback(DioEdge_t edge, DioCallback_t func = nullptr, void *arg = nullptr);

private:
  uint32_t m_chip_number;
  void *m_chip_handle;
  void *m_line_handle;
  UtilsInOutSync_t m_sync;
  int m_flags;
  bool m_value;

  void readAsyncThread(void);
};

#endif /* DRV_DIO_HPP */