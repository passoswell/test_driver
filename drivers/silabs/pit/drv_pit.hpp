/**
 * @file drv_pit.hpp
 * @author user (contact@email.com)
 * @brief Periodic Interruption Timer
 * @date 2023-10-22
 *
 * @copyright Copyright (c) 2023
 *
 */


#if __has_include("em_core.h")

#ifndef DRV_PIT_HPP_
#define DRV_PIT_HPP_


#include <stdbool.h>
#include <stdint.h>


/**
 * @brief Measurement unities compatible with DrvPIT
 */
typedef enum
{
  MilliSec, /*!< Specifies the unit of milliseconds to the counter. *//**< MilliSec */
} DrvPIT_Units_t;

/**
 * @brief Parameters used during DrvPIT operation
 */
typedef struct
{
  DrvPIT_Units_t used_unit;      /*!< Time unit  to be configured.
                                      This parameter can be any value
                                      of @ref DrvPIT_Units_t                  */
  uint32_t       timeout;        /*!< Time to count. This parameter can be
                                      any uint32_t value                      */
  uint32_t       time_count;     /*!< Actual time count.                      */
  bool           wait_overflow;  /*!< Specifies if should wait for overflow   */
  bool           status;         /*!< Specifies if the time count is done     */
  bool           is_periodic;    /*!< Specifies if the timer will start counting
                                      again after expiring                    */
} DrvPIT_Parammeters_t;


class DrvPIT final
{
public:
  void initialize();

  bool write(uint32_t timeout, bool periodic = false, DrvPIT_Units_t unit = MilliSec);
  bool read(void);

  bool halt(void);
  bool resume(void);

  uint32_t readCounter(void);
  static uint32_t readPITCounter(void);
  static void delay(uint32_t value);

  static void disableInterruptions(void);
  static void enableInterruptions(void);

protected:
  DrvPIT_Parammeters_t m_timer;
  bool m_is_enabled = false;
};

#endif /* DRV_PIT_HPP_ */

#endif /* __has_include("em_core.h") */
