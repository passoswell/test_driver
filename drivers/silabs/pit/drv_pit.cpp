/**
 * @file drv_pit.cpp
 * @author user (contact@email.com)
 * @brief Periodic Interruption Timer
 * @date 2023-10-22
 *
 * @copyright Copyright (c) 2023
 *
 */


#if __has_include("em_core.h")


#include "drv_pit.hpp"
#if __has_include("FreeRTOS.h")
#define DRV_PIT_USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#else
#define DRV_PIT_USE_SLEEPTIMER
#include "em_core.h"
#include "sl_sleeptimer.h"
#endif

/**
 * Maximum value in ms. When the tick is UINT32_MAX - 1.
 * DRV_PIT_MAX_COUNT_MS = sl_sleeptimer_tick_to_ms(UINT32_MAX)
 */
#if defined(DRV_PIT_USE_SLEEPTIMER)
#define DRV_PIT_MAX_COUNT_MS                                           0x7CFFFFF
#else
#define DRV_PIT_MAX_COUNT_MS                                          UINT32_MAX
#endif

/**
 * @brief Configure internal hardware (if not done by vendor IDE)
 */
void DrvPIT::initialize()
{
  m_is_enabled = true;
  m_timer.status = true;
}

/**
 * @brief  Start time counting routine.
 * @param  ID : Requester's ID
 * @param  timeout : How many ticks to count
 * @param  unit : Tick's time base.
 *         This parameter can be one of the following values:
 *             @arg MilliSec: Counter is set to milliseconds base.
 * @retval Result : Result of Operation
 *         This parameter can be one of the following values:
 *              @arg true: All ok, time is being counted
 *              @arg Else: Some error happened.
 */
bool DrvPIT::write(uint32_t timeout, bool periodic, DrvPIT_Units_t unit)
{
  uint32_t counts_to_overflow = 0;
  uint32_t current_pit_count = 0;
  (void) unit;

  if(timeout == 0) return false;
  if(timeout > DRV_PIT_MAX_COUNT_MS) timeout = DRV_PIT_MAX_COUNT_MS;

  current_pit_count = readPITCounter();

  counts_to_overflow = DRV_PIT_MAX_COUNT_MS - readPITCounter();

  if (counts_to_overflow >= timeout)
  {
    /* Count will finish before the overflow event */
    m_timer.timeout = current_pit_count + timeout;
    m_timer.wait_overflow = false;
  }
  else
  {
    /* Count will finish after the overflow event */
    m_timer.timeout = timeout - counts_to_overflow;
    m_timer.wait_overflow = true;
  }

  m_timer.is_periodic = periodic;
  m_timer.status = false;
  m_timer.time_count = current_pit_count;

  return true;
}

/**
 * @brief  Routine that checks if the requested time has passed.
 * @param  ID : Requester's ID
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *          @arg true: All ok, time is being counted
 *          @arg false: Time has not passed yet, keep waiting.
 *          @arg Else: Some error happened.
 */
bool DrvPIT::read(void)
{
  uint32_t current_pit_count;

  if (m_timer.status == true) return true; // Timer already expired

  current_pit_count = readPITCounter();

  if (m_timer.wait_overflow != false)
  {
    if (current_pit_count < m_timer.time_count)
    {
      m_timer.wait_overflow = false;
    }
    return false;
  }
  else
  {
    if (current_pit_count >= m_timer.timeout)
    {
      m_timer.status = true;
      if(m_timer.is_periodic)
      {
        write(m_timer.timeout, m_timer.is_periodic, m_timer.used_unit);
      }
      return true;
    }
    else
    {
      return false;
    }
  }
}

/**
 * @brief  Stop driver's operation.
 * @retval Result : Result of Operation
 *         This parameter can be one of the following values:
 *             @arg true: All ok, time is being counted
 *             @arg Else: Some error happened.
 */
bool DrvPIT::halt(void)
{
  /* Simply inform that process went well and mark that driver is disabled.   */
  m_is_enabled = false;
  return true;
}

/**
 * @brief  Resume driver's operation.
 * @retval Result : Result of Operation
 *         This parameter can be one of the following values:
 *            @arg true: All ok, time is being counted
 *            @arg Else: Some error happened.
 */
bool DrvPIT::resume(void)
{
  /* Simply inform that process went well and mark that driver is running.  */
  m_is_enabled = true;
  return true;
}

/**
 * @brief Get the current timer value
 *
 * @param ID Timer ID
 * @return uint32_t Timer value
 */
uint32_t DrvPIT::readCounter(void)
{
  uint32_t current_pit_count = readPITCounter();
  if ((m_timer.time_count >= m_timer.timeout) &&
      (current_pit_count < m_timer.time_count))
  {
    return (DRV_PIT_MAX_COUNT_MS - m_timer.time_count) + current_pit_count;
  }
  else
  {
    return current_pit_count - m_timer.time_count;
  }
}

/**
 * Return the actual tick counter in milliseconds
 *
 * @return uint32_t Tick in milliseconds
 */
uint32_t DrvPIT::readPITCounter(void)
{
#if defined(DRV_PIT_USE_FREERTOS)
  // Using FreeRTOS
  return xTaskGetTickCount();
#elif defined(DRV_PIT_USE_SLEEPTIMER)
  // Using Silabs' sleep timer
  return (uint32_t)sl_sleeptimer_tick_to_ms(sl_sleeptimer_get_tick_count());
#endif
}

/**
 * @brief Blocking delay
 */
void DrvPIT::delay(uint32_t value)
{
#if defined(DRV_PIT_USE_FREERTOS)
  // Using FreeRTOS for delay
  vTaskDelay(value);
#else
  // Using DrvPIT for delay
  DrvPIT timer;
  timer.write(value, MilliSec);
  while(!timer.read());

  // Using Silabs' sleep timer for delay
  // sl_sleeptimer_delay_millisecond(value);// 0xfffffffe
#endif
}

/**
 * @brief
 */
void DrvPIT::disableInterruptions(void)
{
#if defined(DRV_PIT_USE_FREERTOS)
  taskDISABLE_INTERRUPTS();
#else
  CORE_CRITICAL_IRQ_DISABLE();
#endif
}

/**
 * @brief
 */
void DrvPIT::enableInterruptions(void)
{
#if defined(DRV_PIT_USE_FREERTOS)
  taskENABLE_INTERRUPTS();
#else
  CORE_CRITICAL_IRQ_ENABLE();
#endif
}

#endif /* __has_include("em_core.h") */
