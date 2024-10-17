/**
 * @file dev_ssd1306_types.hpp
 * @author user (contact@email.com)
 * @brief Insert a brief description here
 * @date 2023-11-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef DEV_SSD1306_TYPES_HPP_
#define DEV_SSD1306_TYPES_HPP_



/**
 * @brief  SD1306 registers and values definition.
 */

#define SD1306_ADDRESS                                                      0x3C
#define SD1306_COMMAND                                                      0x00
#define SD1306_DATA                                                         0x60
#define SD1306_MAX_CHAR_SIZE                                                16*4
#define SSD1306_ROW_SIZE                                                      64
#define SSD1306_COL_SIZE                                                     128

#define SD1306_SETCONTRAST                                                  0x81
#define SD1306_ENTIREDISPLAYON_RESUME                                       0xA4
#define SD1306_ENTIREDISPLAYON                                              0xA5
#define SD1306_NORMALDISPLAY                                                0xA6
#define SD1306_INVERTDISPLAY                                                0xA7
#define SD1306_DISPLAYOFF                                                   0xAE
#define SD1306_DISPLAYON                                                    0xAF

/* Scrolling Command */
#define SD1306_RIGHT_HORIZONTAL_SCROLL                                      0x26
#define SD1306_LEFT_HORIZONTAL_SCROLL                                       0x27
#define SD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL                         0x29
#define SD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL                          0x2A
#define SD1306_DEACTIVATE_SCROLL                                            0x2E
#define SD1306_ACTIVATE_SCROLL                                              0x2F
#define SD1306_SET_VERTICAL_SCROLL_AREA                                     0xA3

/* Addressing Setting Command */
#define SD1306_SETLOWCOLUMN                                                 0x00
#define SD1306_SETHIGHCOLUMN                                                0x10
#define SD1306_MEMORYMODE                                                   0x20
#define SD1306_COLUMNADDR                                                   0x21
#define SD1306_PAGEADDR                                                     0x22
#define SD1306_PAGESTARTADDR                                                0xB0

/* Hardware Configuration (Panel resolution and layout related) Command */
#define SD1306_SETSTARTLINE                                                 0x40
#define SD1306_SEGREMAP                                                     0xA0
#define SD1306_SETMULTIPLEXRATIO                                            0xA8
#define SD1306_COMSCANINC                                                   0xC0
#define SD1306_COMSCANDEC                                                   0xC8
#define SD1306_SETDISPLAYOFFSET                                             0xD3
#define SD1306_SETCOMPINS                                                   0xDA

/* Timing and Driving Scheme Setting Command  */
#define SD1306_SETDISPLAYCLOCKDIV                                           0xD5
#define SD1306_SETPRECHARGE                                                 0xD9
#define SD1306_SETVCOMDETECT                                                0xDB
#define SD1306_NOP                                                          0xE3

#define SD1306_CHARGEPUMP                                                   0x8D
#define SD1306_EXTERNALVCC                                                   0x1
#define SD1306_SWITCHCAPVCC                                                  0x2

#define SD1306_CL_WHITE                                                        0
#define SD1306_CL_BLACK                                                        1


#endif /* DEV_SSD1306_TYPES_HPP_ */
