/**
 * @file driver.hpp
 * @author your name (you@domain.com)
 * @brief Base class for drivers
 * @version 0.1
 * @date 2024-10-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVER_HPP
#define DRIVER_HPP

#include "commons.hpp"
#include "driver_in_base.hpp"
#include "driver_out_base.hpp"

class DriverBase : public DriverInBase, public DriverOutBase
{
public:
  DriverBase();
  virtual ~DriverBase();
};

#endif  // DRIVER_HPP