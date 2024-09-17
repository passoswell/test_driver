/**
 * @file drv_spt.cpp
 * @author your name (you@domain.com)
 * @brief Software Periodic Timer driver for pipico
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "drv_spt.hpp"

#include "pico/stdlib.h"

/**
 * @brief Constructor
 */
DrvSPT::DrvSPT(SoftwareTimerCountUnit_t time_unit) : DrvSptBase(time_unit)
{
  m_prev_time_ms[0] = 0;
  m_prev_time_ms[1] = 0;
  m_prev_time_ms[2] = 0;
}

/**
 * @brief Get the time since power on
 * @return uint32_t
 */
uint32_t DrvSPT::getTimeSincePowerOn()
{
  uint32_t integer, fractional, time;
  double diff;
  uint64_t time_u64;

  switch(m_unit)
  {
    case SOFTWARE_TIMER_SECONDS:
      m_prev_time_ms[0] = to_ms_since_boot(get_absolute_time());
      integer = (m_prev_time_ms[0] - m_prev_time_ms[1] + m_prev_time_ms[2]) / 1000;
      fractional = (m_prev_time_ms[0] - m_prev_time_ms[1] + m_prev_time_ms[2]) % 1000;
      m_seconds_counter = m_seconds_counter + integer;
      m_prev_time_ms[1] = m_prev_time_ms[0];
      m_prev_time_ms[2] = fractional;
      time = m_seconds_counter;
      break;
    case SOFTWARE_TIMER_MILLISECONDS:
      time = to_ms_since_boot(get_absolute_time());
      break;
    case SOFTWARE_TIMER_MICROSECONDS:
      time_u64 = to_us_since_boot(get_absolute_time());
      time = time_u64 & 0xFFFFFFFF;
      break;
    default:
      diff = 0;
      break;
  }
  return time;
}