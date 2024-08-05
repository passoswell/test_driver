/**
 * @file drv_gpio.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-07-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRV_GPIO_HPP
#define DRV_GPIO_HPP


#include <stdint.h>
#include <stdbool.h>

#include "commons.hpp"
#include "drivers/base/drv_gpio_base.hpp"
#include "drivers/linux/utils/linux_types.hpp"

/**
 * @brief Class that export GPIO functionalities
 */
class DrvGPIO : public DrvGpioBase
{
public:

  DrvGPIO(uint8_t line_offset, uint8_t chip_number);
  virtual ~DrvGPIO();

  // Status_t configure(uint8_t parameter, uint32_t value);
  Status_t configure(const DrvGpioConfigure_t *list, uint8_t list_size);

  Status_t read(bool &state);
  Status_t write(bool value);

  Status_t setCallback(DrvGpioEdge_t edge, DrvGpioCallback_t func = nullptr, void *arg = nullptr);

private:
  uint8_t m_gpio_chip_number;
  void *m_gpio_chip_handle;
  void *m_gpio_line_handle;
  UtilsInOutSync_t m_sync;
  int m_flags;

  void readAsyncThread(void);
};

#endif /* DRV_GPIO_HPP */