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

#include "spt.hpp"

#include "pico/stdlib.h"

/**
 * @brief Constructor
 */
SPT::SPT(SoftwareTimerCountUnit_t time_unit) : SptBase(time_unit)
{
  m_prev_time_ms[0] = 0;
  m_prev_time_ms[1] = 0;
  m_prev_time_ms[2] = 0;
}

/**
 * @brief Get the time since power on in microseconds
 * @return sft_time_us_t
 */
sft_time_us_t SPT::getTimeSincePowerOnUs()
{
  return (sft_time_us_t) to_us_since_boot(get_absolute_time());
}

/**
 * @brief Blocking delay for a specified duration
 * @param duration The specified time to delay
 */
void SPT::delay(uint32_t duration)
{
  switch(m_unit)
  {
    case SOFTWARE_TIMER_SECONDS:
      sleep_us(duration/1000000);
      break;
    case SOFTWARE_TIMER_MILLISECONDS:
      sleep_ms(duration);
      break;
    case SOFTWARE_TIMER_MICROSECONDS:
      sleep_us(duration);
      break;
    default:
      sleep_ms(duration);
      break;
  }
}