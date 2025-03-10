/**
 * @file drv_spt.cpp
 * @author your name (you@domain.com)
 * @brief Software Periodic Timer driver for linux
 * @version 0.1
 * @date 2024-09-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "drv_spt.hpp"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <thread>

/**
 * @brief Constructor
 */
DrvSPT::DrvSPT(SoftwareTimerCountUnit_t time_unit) : DrvSptBase(time_unit)
{
  // Nothing is done here
}

/**
 * @brief Get the time since power on in microseconds
 * @return uint64_t
 */
sft_time_us_t DrvSPT::getTimeSincePowerOnUs()
{
  sft_time_us_t now_time = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::high_resolution_clock::now().time_since_epoch()).count();
  return now_time;
}

/**
 * @brief Blocking delay for a specified duration
 * @param duration The specified time to delay
 */
void DrvSPT::delay(uint32_t duration)
{
  switch(m_unit)
  {
    case SOFTWARE_TIMER_SECONDS:
      std::this_thread::sleep_for(std::chrono::seconds(duration));
      break;
    case SOFTWARE_TIMER_MILLISECONDS:
      std::this_thread::sleep_for(std::chrono::milliseconds(duration));
      break;
    case SOFTWARE_TIMER_MICROSECONDS:
      std::this_thread::sleep_for(std::chrono::microseconds(duration));
      break;
    default:
      std::this_thread::sleep_for(std::chrono::milliseconds(duration));
      break;
  }
}