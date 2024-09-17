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
  m_start_time = getTimeSincePowerOn();
  m_duration = duration;
  m_mode = mode;
  m_is_running = true;
}

/**
 * @brief Check if the timer has expired
 * @return true if timer overflow occurred
 * @return false if still counting
 */
bool DrvSptBase::hasExpired()
{
  if (m_is_running && ((getTimeSincePowerOn() - m_start_time) >= m_duration))
  {
    if (m_mode == SOFTWARE_TIMER_ONE_SHOT)
    {
      m_is_running = false; // Stop the timer if it was one-shot
    }
    else if (m_mode == SOFTWARE_TIMER_PERIODIC)
    {
      m_start_time = getTimeSincePowerOn(); // Restart the timer for periodic mode
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
  if (m_is_running)
  {
    return getTimeSincePowerOn() - m_start_time;
  }
  else
  {
    return 0; // If the timer isn't running, no time has elapsed
  }
}

/**
 * @brief Get the time since power on
 * @return uint32_t
 */
uint32_t DrvSptBase::getTimeSincePowerOn()
{
  return 0;
}

/**
 * @brief Blocking delay for a specified duration
 * @param duration
 */
void DrvSptBase::delay(uint32_t duration)
{
  uint32_t start_time = getTimeSincePowerOn();
  while (getTimeSincePowerOn() - start_time < duration)
  {
    // Busy-wait until the delay period has passed
  }
}