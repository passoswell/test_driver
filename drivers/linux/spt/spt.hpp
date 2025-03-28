/**
 * @file spt.hpp
 * @author your name (you@domain.com)
 * @brief Software Periodic Timer driver for linux
 * @version 0.1
 * @date 2024-09-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVERS_LINUX_SPT_SPT_HPP
#define DRIVERS_LINUX_SPT_SPT_HPP

#include <stdint.h>
#include <stdbool.h>

#include "peripherals_base/spt_base.hpp"

class SPT final : public SptBase
{
public:

  SPT(SoftwareTimerCountUnit_t time_unit = SOFTWARE_TIMER_MILLISECONDS);

  sft_time_us_t getTimeSincePowerOnUs();

  void delay(uint32_t duration);
};

#endif /* DRIVERS_LINUX_SPT_SPT_HPP */