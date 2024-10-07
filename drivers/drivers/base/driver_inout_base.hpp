/**
 * @file driver_inout_base.hpp
 * @author your name (you@domain.com)
 * @brief Base class for drivers
 * @version 0.1
 * @date 2024-10-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVER_INOUT_HPP
#define DRIVER_INOUT_HPP

#include "commons.hpp"
#include "driver_in_base.hpp"
#include "driver_out_base.hpp"

class DriverInOutBase : public DriverInBase, public DriverOutBase
{
public:
  DriverInOutBase(){;}
  virtual ~DriverInOutBase(){;}
};

#endif  // DRIVER_INOUT_HPP