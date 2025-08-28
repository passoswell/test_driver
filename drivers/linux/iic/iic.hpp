/**
 * @file iic.hpp
 * @author your name (you@domain.com)
 * @brief Give access to IIC functionalities
 * @version 0.1
 * @date 2024-10-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRIVERS_LINUX_IIC_IIC_HPP
#define DRIVERS_LINUX_IIC_IIC_HPP


#include <stdio.h>
#include <stdbool.h>

#include "linux/iic/iic_bus.hpp"

template<IicHandle_t PORT_NUMBER>
class IIC : public bIIC
{
public:

  IIC(uint16_t address) : bIIC(IicBus<PORT_NUMBER>::getInstance(), address)
  {
  }
};

#endif /* DRIVERS_LINUX_IIC_IIC_HPP */