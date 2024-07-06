/**
 * @file drv_i2c.hpp
 * @author user (contact@email.com)
 * @brief Insert a brief description here
 * @date 2024-06-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#if __has_include("sys/ioctl.h")


#ifndef DRV_I2C_HPP_
#define DRV_I2C_HPP_


#include <stdio.h>
#include <stdbool.h>

#include "drivers/base/drv_comm_base.hpp"
#include "drivers/linux/utils/linux_types.hpp"


/**
 * @brief Class that implements I2C communication
 */
class DrvI2C final : public DrvCommBase
{
public:

  DrvI2C(void *port_handle);
  virtual ~DrvI2C();

  Status_t read(uint8_t *buffer, uint32_t size, uint8_t address_8bits, uint32_t timeout = 0xFFFFFFFF);

  Status_t write(uint8_t *buffer, uint32_t size, uint8_t address_8bits, uint32_t timeout = 0xFFFFFFFF);

  Status_t readAsync(uint8_t *buffer, uint32_t size, uint8_t address_8bits, InOutStreamCallback_t func = nullptr, void *arg = nullptr);

  Status_t writeAsync(uint8_t *buffer, uint32_t size, uint8_t address_8bits, InOutStreamCallback_t func = nullptr, void *arg = nullptr);

private:
  uint8_t m_address; //8 bits address
  int m_linux_handle;
  bool m_is_reading, m_is_writing;
  UtilsInOutSync_t m_sync;

  void asyncThread(void);
};


#endif /* DRV_I2C_HPP_ */


#endif /* __has_include("em_i2c.h" */
