/**
 * @file drv_spt.hpp
 * @author your name (you@domain.com)
 * @brief Software Periodic Timer driver for esp32
 * @version 0.1
 * @date 2024-09-15
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

private:
  uint32_t m_prev_time_ms[3];
  uint32_t m_seconds_counter;
};