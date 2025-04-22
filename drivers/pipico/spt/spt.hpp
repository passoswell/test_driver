/**
 * @file spt.hpp
 * @author your name (you@domain.com)
 * @brief Software Periodic Timer driver for pipico
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef SPT_SPT_HPP
#define SPT_SPT_HPP

#include <stdint.h>
#include <stdbool.h>

#include "peripherals_base/spt_base.hpp"

class SPT : public SptBase
{
public:

  SPT(SoftwareTimerCountUnit_t time_unit = SOFTWARE_TIMER_MILLISECONDS);

  sft_time_us_t getTimeSincePowerOnUs();

  void delay(uint32_t duration);

private:
  uint32_t m_prev_time_ms[3];
  uint32_t m_seconds_counter;
};

#endif /* SPT_SPT_HPP */