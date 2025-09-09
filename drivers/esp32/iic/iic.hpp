/**
 * @file iic.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-26
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef IIC_IIC_HPP
#define IIC_IIC_HPP

#include "esp32/iic/iic_bus.hpp"

template<IicHandle_t PORT_NUMBER>
class IIC final: public bIIC
{
public:

  IIC(uint16_t address) : bIIC(IicBus<PORT_NUMBER>::getInstance(), address)
  {
    ;
  }

  ~IIC() = default;
};

#endif /* IIC_IIC_HPP */
