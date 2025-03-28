/**
 * @file software_timer_interface.hpp
 * @author your name (you@domain.com)
 * @brief Interface class for a software timer
 * @version 0.1
 * @date 2024-09-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef SOFTWARE_TIMER_INTERFACE_HPP
#define SOFTWARE_TIMER_INTERFACE_HPP

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t sft_time_us_t;

/**
 * @brief Modes of operation for software timers
 */
typedef enum
{
  SOFTWARE_TIMER_ONE_SHOT, // Timer runs once
  SOFTWARE_TIMER_PERIODIC  // Timer restarts after expiration
}SoftwareTimerMode_t;

/**
 * @brief Count units for software timers
 */
typedef enum
{
  SOFTWARE_TIMER_SECONDS,
  SOFTWARE_TIMER_MILLISECONDS,
  SOFTWARE_TIMER_MICROSECONDS,
}SoftwareTimerCountUnit_t;

/**
 * @brief Interface for software timers
 */
class SoftwareTimerInterface
{
public:

  virtual ~SoftwareTimerInterface(){};

  // Start the timer
  virtual void start(uint32_t duration, SoftwareTimerMode_t mode = SOFTWARE_TIMER_ONE_SHOT) = 0;

  // Check if the timer has expired
  virtual bool hasExpired() = 0;

  // Stop the timer
  virtual void stop() = 0;

  // Check if the timer is currently running
  virtual bool isRunning() = 0;

  // Get the time that has passed since the timer was started
  virtual uint32_t getElapsedTime() = 0;

  // Get the time in units since power-on
  virtual sft_time_us_t getTimeSincePowerOnUs() = 0;

  // Blocking delay for a specified duration
  virtual void delay(uint32_t duration) = 0;
};

#endif /* SOFTWARE_TIMER_INTERFACE_HPP */