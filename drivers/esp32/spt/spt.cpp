/**
 * @file spt.cpp
 * @author your name (you@domain.com)
 * @brief Software Periodic Timer driver for esp32
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "spt.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

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
  return (sft_time_us_t) esp_timer_get_time();
}

/**
 * @brief Blocking delay for a specified duration
 * @param duration The specified time to delay
 */
void SPT::delay(uint32_t duration)
{
  TickType_t xDelay = duration / portTICK_PERIOD_MS;
  switch(m_unit)
  {
    case SOFTWARE_TIMER_SECONDS:
      vTaskDelay(1000 * duration / portTICK_PERIOD_MS);
      break;
    case SOFTWARE_TIMER_MILLISECONDS:
      vTaskDelay(duration / portTICK_PERIOD_MS);
      break;
    case SOFTWARE_TIMER_MICROSECONDS:
      vTaskDelay(duration / (1000 * portTICK_PERIOD_MS));
      break;
    default:
      vTaskDelay(1000 * duration / portTICK_PERIOD_MS);
      break;
  }
}