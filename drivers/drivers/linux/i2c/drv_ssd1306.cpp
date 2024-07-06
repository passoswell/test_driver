/**
 * @file drv_ssd1306.cpp
 * @author user (contact@email.com)
 * @brief Insert a brief description here
 * @date 2023-11-15
 *
 * @copyright Copyright (c) 2023
 *
 */


#include "dev_ssd1306.hpp"
#include <string.h>


/**
 * @brief Configuration bytes in sequence
 */
const uint8_t SD1306_InitSeq[] =
{
 SD1306_DISPLAYOFF,        SD1306_SETMULTIPLEXRATIO,     0x3F,
 SD1306_SETDISPLAYOFFSET,    0x00,               SD1306_SETSTARTLINE,
 SD1306_SEGREMAP | 1,      SD1306_COMSCANDEC,        SD1306_SETCOMPINS,
 0x12,               SD1306_SETCONTRAST,       0xFF,
 SD1306_ENTIREDISPLAYON_RESUME,  SD1306_NORMALDISPLAY,       SD1306_SETDISPLAYCLOCKDIV,
 0x80,               SD1306_SETPRECHARGE,      0x25,
 SD1306_SETVCOMDETECT,       0x20,             SD1306_PAGEADDR,
 0x00,               0x07,               0xB0,
 0x10,               0x00,               SD1306_MEMORYMODE,
 0x00,               SD1306_CHARGEPUMP,        0x14,
 SD1306_DISPLAYON
};

/**
 * @brief Constructor
 * @param port_handle Vendor supplied port handle
 * @param address I2C address
 */
SSD1306::SSD1306(InOutStream *port_handle, uint8_t address_8bits)
{
  m_driver = port_handle;
  m_address = address_8bits;
  m_is_initialized = false;
}

/**
 * @brief Destructor
 */
SSD1306::~SSD1306()
{
  // Nothing is done here
}

/**
 * @brief Initialize and configure the display
 * @return true if successful
 */
bool SSD1306::initialize(void)
{
  bool returncode;

  // DrvI2C::initialize();

  for(uint16_t index = 0; index < sizeof(SD1306_InitSeq); index++)
  {
    returncode = writeCommand(SD1306_InitSeq[index]);
    if(!returncode) break;
  }
  m_is_initialized = true;
  return returncode;
}

/**
 * @brief Put the cursor on the desired position
 * @param row Desired row
 * @param column Desired column
 * @return
 */
bool SSD1306::setCursor(uint8_t row, uint8_t column)
{
  uint8_t command;

  if(!m_is_initialized) initialize();

  // Set row
  command = SD1306_PAGESTARTADDR | row;
  if(!writeCommand(command)) return false;
  // Set column
  command = SD1306_SETHIGHCOLUMN | ( column >> 4 );
  if(!writeCommand(command)) return false;
  // Set column
  command = 0x0F & column;
  if(!writeCommand(command)) return false;

  return true;
}

/**
 * @brief Send a text to the display using 8x16 characters
 * @param buffer Pointer to the text
 * @param size Size in characters of the text
 * @param start_row Start row
 * @param start_column Start column
 * @param color Desired print color
 * @return
 */
bool SSD1306::write(uint8_t *buffer,
                    uint16_t size,
                    uint8_t start_row,
                    uint8_t start_column,
                    uint8_t color)
{
  uint8_t row = start_row;
  uint8_t column = start_column;

  if(!m_is_initialized) initialize();

  if((size == 0) || (buffer == nullptr) ) return false;

  for(uint16_t index = 0; index < size; index ++)
  {
    writeChar(buffer[index], row, column, color);
    // Rolling over to the next column and line for 8x16 characters
    column++;
    if( column >= 16 ){
      column = 0;
      row += 2;
      if( row >= 8 ){
        row = 0;
      }
    }
  }
  return true;
}

/**
 * @brief Send a 8x16 character to a desired position
 * @param byte The character to send
 * @param row Desired row to position the character
 * @param col Desired column to position the character
 * @param color Desired print color
 * @return true if successful
 */
bool SSD1306::writeChar(uint8_t character,
                        uint8_t row,
                        uint8_t column,
                        uint8_t color)
{
  uint8_t data;

  if(!m_is_initialized) initialize();

  column <<= 3;

  // Set cursor position
  if(!setCursor(row, column)) return false;

  // Write the first part of the character
  for(uint8_t index = 0; index < 8; index++)
  {
    data = SD1306_Font8x16[character- 0x20][index];
    if( color > 0 ) data ^= 0xFF;
    if(!writeData(data)) return false;
  }

  // Set cursor position again
  row++;
  if(!setCursor(row, column)) return false;

  // Write the last part of the character
  for(uint8_t index = 8; index < 16; index++)
  {
    data = SD1306_Font8x16[character- 0x20][index];
    if( color > 0 ) data ^= 0xFF;
    if(!writeData(data)) return false;
  }

  return true;
}

/**
 * @brief Clears the display
 * @param color Desired color
 * @return true if successful
 */
bool SSD1306::clearScreen(uint8_t color)
{
  uint8_t data = 0;

  if(!m_is_initialized) initialize();

  if(color > 0) data = 0xFF;
  for(uint8_t row = 0; row < 8; row++)
  {
    if(!setCursor(row, 0)) return false;
    for(uint8_t column = 0; column < SSD1306_COL_SIZE; column++)
    {
      if(!writeData(data)) return false;
    }
  }
  return true;
}

/**
 * @brief Template for a method to write data asynchronously
 * @param buffer Buffer where data is stored
 * @param size Number of bytes to write
 * @return Status_t
 */
Status_t SSD1306::writeAsync(uint8_t *buffer, uint32_t size, uint8_t key, InOutStreamCallback_t func, void *arg)
{
  (void) key;
  if(!m_is_initialized) initialize();

  if(size == 0) return STATUS_DRV_ERR_PARAM_SIZE;
  if(buffer == nullptr) return STATUS_DRV_NULL_POINTER;

  for(uint16_t index = 0; index < size; index ++)
  {

    switch(buffer[index])
    {
      case '\0':
        break;

      case '\b':
        // Non-destructive backspace
        if(m_column != 0)
        {
          m_column--;
        }
        break;

      case '\t':
        // Non-destructive horizontal tab
        m_column += 6;
        if(m_column >= 16)
        {
          m_column %= 16;
          m_row += 2;
          m_row %= 8;
        }
        break;

      case '\n':
        // Destructive newline
        m_column = 0;
        m_row += 2;
        if( m_row >= 8 )
        {
          m_row = 0;
        }
        for(uint8_t j = 0; j < 16; j++)
        {
          writeChar(' ', m_row, m_column + j, 0);
        }
        break;

      case '\r':
        // Non-destructive carriage return
        m_column = 0;
        break;

      case '\033':
        // Escape code
        if(buffer[index+1] == '\143')
        {
          m_column = 0;
          m_row = 0;
          index++;
          clearScreen(0);
        }else if(buffer[index+1] == '[')
        {
          switch(buffer[index+2])
          {
            case 'A':
              m_column = 0;
              if( m_row == 0)
              {
                m_row = 6;
              }else
              {
                m_row -= 2;
              }
              index+= 2;
              break;

            case 'B':
              m_column = 0;
              m_row += 2;
              if( m_row >= 8 )
              {
                m_row = 0;
              }
              index+= 2;
              break;

            default:
              index+= 2;
              break;
          }
        }
       break;

      default:
        writeChar(buffer[index], m_row, m_column, 0);
        // Rolling over to the next column and line for 8x16 characters
        m_column++;
        if( m_column >= 16 )
        {
          m_column = 0;
          m_row += 2;
          if( m_row >= 8 )
          {
            m_row = 0;
          }
          for(uint8_t j = 0; j < 16; j++)
          {
            writeChar(' ', m_row, m_column + j, 0);
          }
        }
        break;
    }
  }

  if(func != nullptr)
  {
    func(STATUS_DRV_SUCCESS, buffer, size, arg);
  }

  return STATUS_DRV_SUCCESS;
}

/**
 * @brief
 *
 * @param buffer
 * @param size
 * @return Status_t
 */
Status_t SSD1306::writePixels(uint8_t *buffer, uint32_t size)
{
  Status_t status;
  uint8_t internal_buffer[2];
  internal_buffer[0] = SD1306_DATA;
  for(uint32_t i = 0; i < size; i++)
  {
    internal_buffer[1] = buffer[i];
    status = m_driver->write(internal_buffer, 2, m_address);
  }
  return status;
}

/**
 * @brief Send a command to the display
 * @param Command A command byte
 * @return true if successful
 */
bool SSD1306::writeCommand(uint8_t command)
{
  uint8_t buffer[2];
  buffer[0] = SD1306_COMMAND;
  buffer[1] = command;
  return m_driver->write(buffer, 2, m_address).success;
}

/**
 * @brief Send data to the display
 * @param data A data byte
 * @return true if successful
 */
bool SSD1306::writeData(uint8_t data)
{
  uint8_t buffer[2];
  buffer[0] = SD1306_DATA;
  buffer[1] = data;
  return m_driver->write(buffer, 2, m_address).success;
}
