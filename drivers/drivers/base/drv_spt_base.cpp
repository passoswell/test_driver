/**
 * @file drv_spt_base.cpp
 * @author your name (you@domain.com)
 * @brief Base class for a software timer driver
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "drv_spt_base.hpp"

/**
 * @brief Constructor
 */
DrvSptBase::DrvSptBase(SoftwareTimerCountUnit_t time_unit)
{
  m_start_time = 0;
  m_duration = 0;
  m_is_running = false;
  m_mode = SOFTWARE_TIMER_ONE_SHOT;
  m_unit = time_unit;
}

/**
 * @brief Destructor
 */
DrvSptBase::~DrvSptBase()
{
  // Nothing is done here
}

/**
 * @brief Start the timer
 * @param duration Time to count
 * @param mode Operation mode
 */
void DrvSptBase::start(uint32_t duration, SoftwareTimerMode_t mode)
{
  m_start_time = getTimeSincePowerOnUs();
  m_mode = mode;
  switch(m_unit)
  {
    case SOFTWARE_TIMER_SECONDS:
      m_duration = duration * 1000000;
      break;
    case SOFTWARE_TIMER_MILLISECONDS:
      m_duration = duration * 1000;
      break;
    case SOFTWARE_TIMER_MICROSECONDS:
      m_duration = duration;
      break;
    default:
      m_duration = duration * 1000;
      break;
  }
  m_is_running = true;
}

/**
 * @brief Check if the timer has expired
 * @return true if timer overflow occurred
 * @return false if still counting
 */
bool DrvSptBase::hasExpired()
{
  if (!m_is_running)
  {
    return true;
  }

  sft_time_us_t now_time = getTimeSincePowerOnUs();
  sft_time_us_t difference = now_time - m_start_time;
  if (difference >= m_duration)
  {
    if (m_mode == SOFTWARE_TIMER_ONE_SHOT)
    {
      m_is_running = false; // Stop the timer if it was one-shot
    }
    else if (m_mode == SOFTWARE_TIMER_PERIODIC)
    {
      m_start_time = getTimeSincePowerOnUs(); // Restart the timer for periodic mode
    }
    return true;
  }
  return false;
}

/**
 * @brief Stop the timer
 */
void DrvSptBase::stop()
{
  m_is_running = false;
}

/**
 * @brief Check if the timer is currently running
 * @return true if running
 * @return false if not running
 */
bool DrvSptBase::isRunning()
{
  return m_is_running;
}

/**
 * @brief Get the time that has passed since the timer was started
 * @return uint32_t
 */
uint32_t DrvSptBase::getElapsedTime()
{
  sft_time_us_t now_time = getTimeSincePowerOnUs();
  sft_time_us_t difference = now_time - m_start_time;
  uint32_t time;
  if(difference > UINT32_MAX)
  {
    return UINT32_MAX;
  }
  switch(m_unit)
  {
    case SOFTWARE_TIMER_SECONDS:
      time = difference / 1000000;
      break;
    case SOFTWARE_TIMER_MILLISECONDS:
      time = difference / 1000;
      break;
    case SOFTWARE_TIMER_MICROSECONDS:
      time = difference;
      break;
    default:
      time = difference / 1000;
      break;
  }
  return time;
}

/**
 * @brief Get the time since power on in microseconds
 * @return uint32_t
 */
sft_time_us_t DrvSptBase::getTimeSincePowerOnUs()
{
  return 0;
}

/**
 * @brief Blocking delay for a specified duration
 * @param duration
 */
void DrvSptBase::delay(uint32_t duration)
{
  sft_time_us_t start_time = getTimeSincePowerOnUs();
  sft_time_us_t now_time, time_difference;
  time_difference = start_time;
  while (time_difference < duration)
  {
    // Busy-wait until the delay period has passed
    now_time = getTimeSincePowerOnUs();
    time_difference = now_time - start_time;
    switch(m_unit)
    {
      case SOFTWARE_TIMER_SECONDS:
        time_difference = time_difference / 1000000;
        break;
      case SOFTWARE_TIMER_MILLISECONDS:
        time_difference = time_difference / 1000;
        break;
      case SOFTWARE_TIMER_MICROSECONDS:
        break;
      default:
        time_difference = time_difference / 1000;
        break;
    }
  }
}