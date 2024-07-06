/**
 * @file dev_ssd1306.hpp
 * @author user (contact@email.com)
 * @brief Insert a brief description here
 * @date 2023-11-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef DEV_SSD1306_HPP_
#define DEV_SSD1306_HPP_

#include "inoutstream/inoutstream.hpp"
#include "drivers/base/drv_comm_base.hpp"
#include "dev_ssd1306_types.hpp"
#include "dev_ssd1306_fonts.hpp"

enum SSD1306_addresses{SSE1306_0x3D = 0x7A, SSE1306_0x3C = 0x78};


class SSD1306 final : public DrvCommBase
{
public:
  InOutStream *m_driver;
  uint8_t m_row = 0, m_column = 0;

  SSD1306(InOutStream *port_handle, uint8_t address_8bits = SSE1306_0x3C);
  ~SSD1306();
  bool initialize(void);

  bool setCursor(uint8_t row, uint8_t column);

  bool write(uint8_t *buffer,
             uint16_t size,
             uint8_t start_row,
             uint8_t start_column,
             uint8_t color);
  bool writeChar(uint8_t character, uint8_t row, uint8_t column, uint8_t color);

  bool clearScreen(uint8_t color = 0);

  bool reset(void){return true;}

  Status_t writeAsync(uint8_t *buffer, uint32_t size, uint8_t key, InOutStreamCallback_t func = nullptr, void *arg = nullptr);

  Status_t writePixels(uint8_t *buffer, uint32_t size);

protected:
  bool m_is_initialized;
  uint8_t m_address;

  bool writeCommand(uint8_t command);
  bool writeData(uint8_t data);
};


#endif /* DEV_SSD1306_HPP_ */
