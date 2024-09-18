/**
 * @file drv_dio_base.hpp
 * @author your name (you@domain.com)
 * @brief Base class for drivers on digital inputs and outputs
 * @version 0.1
 * @date 2024-07-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef DRV_DIO_BASE_HPP
#define DRV_DIO_BASE_HPP


#include <stdio.h>
#include <stdbool.h>

#include "commons.hpp"
#include "dio/dio_interface.hpp"

/**
 * @brief Base class for drivers on digital inputs and outputs
 */
class DrvDioBase : public DioInterface
{
public:
  uint8_t m_dio_line_number;
  DioCallback_t m_func;
  void *m_arg;

  Status_t configure(uint8_t parameter, uint32_t value);

  Status_t configure(const DioConfigureList_t *list, uint8_t list_size);

  Status_t read(bool &state);

  Status_t write(bool state);

  Status_t toggle();

  Status_t setCallback(DioEdge_t edge, DioCallback_t func = nullptr, void *arg = nullptr);
};

#endif /* DRV_DIO_BASE_HPP */
