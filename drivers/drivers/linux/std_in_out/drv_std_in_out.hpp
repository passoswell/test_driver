/**
 * @file drv_std_in_out.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRV_STD_IN_OUT_HPP
#define DRV_STD_IN_OUT_HPP

#include "drivers/base/drv_comm_base.hpp"

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

enum
{
  TESTUART_FILE = 0,
  TESTUART_FILENAME,
  TESTUART_BAUD,
};

class DrvStdInOut : public DrvCommBase
{
public:
  DrvStdInOut();

  ~DrvStdInOut();

  Status_t read(uint8_t *buffer, uint32_t size, uint8_t key = 0, uint32_t timeout = UINT32_MAX);

  Status_t write(uint8_t *buffer, uint32_t size, uint8_t key = 0, uint32_t timeout = UINT32_MAX);

  uint32_t bytesRead();

private:
  FILE *m_in_file, *m_out_file;
};

#endif