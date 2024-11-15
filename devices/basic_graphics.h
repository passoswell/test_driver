/**
 * @file  basic_graphics.h
 * @date  14-January-2021
 * @brief Graphical library with basic functions.
 *
 * @author
 * @author
 *
 * Original header:
 *
 *
 * This is the core graphics library for all our displays, providing a common
 * set of graphics primitives (points, lines, circles, etc.).  It needs to be
 * paired with a hardware-specific library for each display device we carry
 * (to handle the lower-level functions).
 *
 * Adafruit invests time and resources providing this open source code, please
 * support Adafruit & open-source hardware by purchasing products from Adafruit!
 *
 * Copyright (c) 2013 Adafruit Industries.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef BASIC_GRAPHICS_H
#define BASIC_GRAPHICS_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>
#include "gfxfont.h"


/**
 * @brief Types of display colors.
 */
typedef enum
{
  BGFX_MONOCHROMATIC = 0, /*!< Single color display */
  BGFX_8BITS,             /*!< 8-bits color display */
  BGFX_16BITS,            /*!< 16-bits color display */
  BGFX_24BITS,            /*!< 24-bits color display */
  BGFX_RGB,               /*!< RGB color display */
}BGFX_ColorScheme_t;


/**
 * @brief Structure with data related to the display
 */
typedef struct
{
  uint16_t WIDTH;       /*!< This is the 'raw' display width - never changes */
  uint16_t HEIGHT;      /*!< This is the 'raw' display height - never changes */
  uint16_t Width;       /*!< Display width as modified by current rotation */
  uint16_t Height;      /*!< Display height as modified by current rotation */
  uint16_t WidthPixelsOnWrite;    /*!< Number of horizontal pixels in a byte */
  uint16_t HeightPixelsOnWrite;   /*!< Number of vertical pixels in a byte */
  BGFX_ColorScheme_t ColorScheme; /*!< Color configuration on display */
  uint8_t Rotation;     /*!< Display rotation (0 thru 3), use BGFX_SetRotation*/
  uint8_t Cp437;        /*!< If set, use correct CP437 charset
                             (default is off) */
  BGFXfont *GfxFont;    /*!< Pointer to special font, not implemented */

  void *Buffer;         /*!< Pointer to a buffer of any type */

  void (*DrawPixel)(uint16_t x, uint16_t y, uint16_t color); /*!< Writes pixel
                                                                 to a buffer */
}BGFX_Parameters_t;


/**************************************************************************/
/*!
    @brief  Draw a pixel to the canvas framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  color
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawPixel(uint16_t x, uint16_t y, uint16_t color, BGFX_Parameters_t Display);


/**************************************************************************/
/*!
    @brief    Write a line.  Bresenham's algorithm - thx wikpedia
    @param    x0  Start point x coordinate
    @param    y0  Start point y coordinate
    @param    x1  End point x coordinate
    @param    y1  End point y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
    @param    Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
    uint16_t color, BGFX_Parameters_t Display);


/**************************************************************************/
/*!
    @brief    Draw a rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    color 16-bit 5-6-5 Color to draw with
    @param    Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    uint16_t color, BGFX_Parameters_t Display);


/**************************************************************************/
/*!
    @brief  Fill a rectangle completely with one color. Update in subclasses if
            desired!
    @param  x   Top left corner x coordinate
    @param  y   Top left corner y coordinate
    @param  w   Width in pixels
    @param  h   Height in pixels
    @param  color 16-bit 5-6-5 Color to fill with
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawRectFill(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    uint16_t color, BGFX_Parameters_t Display);


/**************************************************************************/
/*!
    @brief  Draw a rounded rectangle with no fill color
    @param  x   Top left corner x coordinate
    @param  y   Top left corner y coordinate
    @param  w   Width in pixels
    @param  h   Height in pixels
    @param  r   Radius of corner rounding
    @param  color 16-bit 5-6-5 Color to draw with
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
    int16_t r, uint16_t color, BGFX_Parameters_t Display);


/**************************************************************************/
/*!
    @brief  Draw a rounded rectangle with fill color
    @param  x   Top left corner x coordinate
    @param  y   Top left corner y coordinate
    @param  w   Width in pixels
    @param  h   Height in pixels
    @param  r   Radius of corner rounding
    @param  color 16-bit 5-6-5 Color to draw/fill with
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawRoundRectFill(int16_t x, int16_t y, int16_t w, int16_t h,
    int16_t r, uint16_t color, BGFX_Parameters_t Display);


/**************************************************************************/
/*!
    @brief  Draw a circle outline
    @param  x0   Center-point x coordinate
    @param  y0   Center-point y coordinate
    @param  r   Radius of circle
    @param  color 16-bit 5-6-5 Color to draw with
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color,
    BGFX_Parameters_t Display);


/**************************************************************************/
/*!
    @brief  Draw a circle with filled color
    @param  x0   Center-point x coordinate
    @param  y0   Center-point y coordinate
    @param  r   Radius of circle
    @param  color 16-bit 5-6-5 Color to fill with
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawCircleFill(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color,
    BGFX_Parameters_t Display);


/**************************************************************************/
/*!
    @brief  Draw a triangle with no fill color
    @param  x0  Vertex #0 x coordinate
    @param  y0  Vertex #0 y coordinate
    @param  x1  Vertex #1 x coordinate
    @param  y1  Vertex #1 y coordinate
    @param  x2  Vertex #2 x coordinate
    @param  y2  Vertex #2 y coordinate
    @param  color 16-bit 5-6-5 Color to draw with
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
    int16_t x2, int16_t y2, uint16_t color, BGFX_Parameters_t Display);


/**************************************************************************/
/*!
   @brief   Draw a triangle with color-fill
    @param  x0  Vertex #0 x coordinate
    @param  y0  Vertex #0 y coordinate
    @param  x1  Vertex #1 x coordinate
    @param  y1  Vertex #1 y coordinate
    @param  x2  Vertex #2 x coordinate
    @param  y2  Vertex #2 y coordinate
    @param  color 16-bit 5-6-5 Color to fill/draw with
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawTriangleFill(uint16_t x0, uint16_t y0, uint16_t x1,
    uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color,
    BGFX_Parameters_t Display);


/**************************************************************************/
/*!
    @brief  Draw a single character
    @param  x   Bottom left corner x coordinate
    @param  y   Bottom left corner y coordinate
    @param  c   The 8-bit font-indexed character (likely ascii)
    @param  color 16-bit 5-6-5 Color to draw chraracter with
    @param  bg 16-bit 5-6-5 Color to fill background with (if same as color,
             no background)
    @param  size_x  Font magnification level in X-axis, 1 is 'original' size
    @param  size_y  Font magnification level in Y-axis, 1 is 'original' size
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawChar(uint16_t x, uint16_t y, const char c, uint16_t color,
    uint16_t bg, uint8_t size_x, uint8_t size_y, BGFX_Parameters_t Display);


/**************************************************************************/
/*!
    @brief  Draw a single character
    @param  X   Bottom left corner x coordinate
    @param  Y   Bottom left corner y coordinate
    @param  Buffer   The 8-bit font-indexed text (likely ascii)
    @param  Length   Size in characters of text
    @param  Color 16-bit 5-6-5 Color to draw chraracter with
    @param  Bgc 16-bit 5-6-5 Color to fill background with (if same as color,
   no background)
    @param  SizeX  Font magnification level in X-axis, 1 is 'original' size
    @param  SizeY  Font magnification level in Y-axis, 1 is 'original' size
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_SendString(uint16_t X, uint16_t Y, const char *Buffer, uint32_t Length,
    uint16_t Color, uint16_t Bgc, uint8_t SizeX, uint8_t SizeY,
    BGFX_Parameters_t Display);


/**************************************************************************/
/*!
    @brief  Set rotation setting for display, no image resizing performed
    @param  Rotation   0 thru 3 corresponding to 4 cardinal rotations
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_SetRotation(uint8_t Rotation, BGFX_Parameters_t *Display);


#ifdef __cplusplus
}
#endif

#endif /* BASIC_GRAPHICS_H */
