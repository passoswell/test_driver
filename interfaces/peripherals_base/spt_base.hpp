/**
 * @file spt_base.hpp
 * @author your name (you@domain.com)
 * @brief Base class for a software timer driver
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRV_SPT_BASE_HPP
#define DRV_SPT_BASE_HPP

#include "software_timer_interface/software_timer_interface.hpp"

class SptBase : public SoftwareTimerInterface
{
public:
  sft_time_us_t m_start_time;     // Time at which the timer was started
  sft_time_us_t m_duration;       // m_duration for which the timer should run
  bool m_is_running;         // Indicates if the timer is currently running
  SoftwareTimerMode_t m_mode;        // The mode of the timer (ONE_SHOT or PERIODIC)
  SoftwareTimerCountUnit_t m_unit;   // The count unit

  SptBase(SoftwareTimerCountUnit_t time_unit = SOFTWARE_TIMER_MILLISECONDS);

  virtual ~SptBase();

  void start(uint32_t duration, SoftwareTimerMode_t mode = SOFTWARE_TIMER_ONE_SHOT);

  bool hasExpired();

  void stop();

  bool isRunning();

  uint32_t getElapsedTime();

  virtual sft_time_us_t getTimeSincePowerOnUs();

  virtual void delay(uint32_t duration);
};

#endif /* DRV_SPT_BASE_HPP */