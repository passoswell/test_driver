/**
 * @file drv_spt.hpp
 * @author your name (you@domain.com)
 * @brief Software Periodic Timer driver for linux
 * @version 0.1
 * @date 2024-09-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <stdint.h>
#include <stdbool.h>

#include "drivers/base/drv_spt_base.hpp"

class DrvSPT final : public DrvSptBase
{
public:

  DrvSPT(SoftwareTimerCountUnit_t time_unit = SOFTWARE_TIMER_MILLISECONDS);

  uint32_t getTimeSincePowerOn();

  void delay(uint32_t duration);
};