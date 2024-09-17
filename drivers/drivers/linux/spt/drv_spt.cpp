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
 * @brief Get the time since power on
 * @return uint32_t
 */
uint32_t DrvSPT::getTimeSincePowerOn()
{
  uint32_t time;
  static auto start_time = std::chrono::high_resolution_clock::now();
  auto clock_now = std::chrono::high_resolution_clock::now();
  double diff;

  switch(m_unit)
  {
    case SOFTWARE_TIMER_SECONDS:
      diff = std::chrono::duration_cast<std::chrono::seconds>(clock_now - start_time).count();
      break;
    case SOFTWARE_TIMER_MILLISECONDS:
      diff = std::chrono::duration_cast<std::chrono::milliseconds>(clock_now - start_time).count();
      break;
    case SOFTWARE_TIMER_MICROSECONDS:
      diff = std::chrono::duration_cast<std::chrono::microseconds>(clock_now - start_time).count();
      break;
    default:
      diff = 0;
      break;
  }
  auto integer = std::floor(diff / UINT32_MAX);
  time = ((diff / UINT32_MAX) - integer) * UINT32_MAX;
  return time;
}

/**
 * @brief Blocking delay for a specified duration
 * @param duration
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
      DrvSptBase::delay(duration);
      break;
  }
}