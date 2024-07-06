#include <stdlib.h>
#include <stddef.h>
#include "basic_graphics.h"
#include "glcdfont.h"

#ifndef min
#define min(a, b)                                      (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
    do                                                                         \
    {                                                                          \
      int16_t t = a;                                                           \
      a = b;                                                                   \
      b = t;                                                                   \
    }while(0);
#endif





void BGFX_DrawPixel_01(uint16_t x, uint16_t y, uint16_t color ,
    BGFX_Parameters_t Display);

void BGFX_DrawPixel_8(uint16_t x, uint16_t y, uint16_t color,
    BGFX_Parameters_t Display);

void BGFX_DrawPixel_16(uint16_t x, uint16_t y, uint16_t color,
    BGFX_Parameters_t Display);

void BGFX_DrawCircleHelper(uint16_t x0, uint16_t y0, uint16_t r,
    uint8_t cornername, uint16_t color, BGFX_Parameters_t Display);

void BGFX_DrawCircleFillHelper(uint16_t x0, uint16_t y0, uint16_t r,
    uint8_t corners, int16_t delta, uint16_t color, BGFX_Parameters_t Display);



void BGFX_DrawPixel(uint16_t x, uint16_t y, uint16_t color,
    BGFX_Parameters_t Display)
{
  int16_t t;

  /* Updating rotation setting, in case it was changed outside this library*/
  switch (Display.Rotation) {
  case 0:
  case 2:
    Display.Width = Display.WIDTH;
    Display.Height = Display.HEIGHT;
    break;
  case 1:
  case 3:
    Display.Width = Display.HEIGHT;
    Display.Height = Display.WIDTH;
    break;
  }

  /* Testing if point is out of border */
  if ((x >= Display.Width) || (y >= Display.Height))
  {
    return;
  }

  /* Applying rotation */
  switch (Display.Rotation) {
  case 1:
    t = x;
    x = Display.WIDTH - 1 - y;
    y = t;
    break;
  case 2:
    x = Display.WIDTH - 1 - x;
    y = Display.HEIGHT - 1 - y;
    break;
  case 3:
    t = x;
    x = y;
    y = Display.HEIGHT - 1 - t;
    break;
  }

  if (Display.DrawPixel != NULL) {
    /* Using user supplied function, if supplied */
    Display.DrawPixel(x, y, color);
  }else if(Display.Buffer != NULL){

    /* Using one of the default functions */
    switch(Display.ColorScheme){
    case BGFX_MONOCHROMATIC:
      BGFX_DrawPixel_01(x, y, color, Display);
      break;
    case BGFX_8BITS:
      BGFX_DrawPixel_8(x, y, color, Display);
      break;
    case BGFX_16BITS:
      BGFX_DrawPixel_16(x, y, color, Display);
      break;
    default:
      BGFX_DrawPixel_01(x, y, color, Display);
      break;

    }
  }

}





void BGFX_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
    uint16_t color, BGFX_Parameters_t Display)
{
  int16_t steep;
  int16_t dx, dy, err, ystep;

  steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep) {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  dx = x1 - x0;
  dy = abs(y1 - y0);

  err = dx / 2;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      BGFX_DrawPixel(y0, x0, color, Display);
    } else {
      BGFX_DrawPixel(x0, y0, color, Display);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}





void BGFX_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    uint16_t color, BGFX_Parameters_t Display)
{
  BGFX_DrawLine(x        , y        , x + w - 1, y        , color, Display);
  BGFX_DrawLine(x + w - 1, y        , x + w - 1, y + h - 1, color, Display);
  BGFX_DrawLine(x + w - 1, y + h - 1, x        , y + h - 1, color, Display);
  BGFX_DrawLine(x        , y + h - 1, x        , y        , color, Display);
}







void BGFX_DrawRectFill(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    uint16_t color, BGFX_Parameters_t Display)
{
  uint16_t i;
  for (i = x; i < x + w; i++) {
    BGFX_DrawLine(i, y, i, y + h - 1, color, Display);
  }
}




void BGFX_DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
    int16_t r, uint16_t color, BGFX_Parameters_t Display)
{
  int16_t max_radius;

  max_radius = ((w < h) ? w : h) / 2; /* 1/2 minor axiss */
  if (r > max_radius){
    r = max_radius;
  }
  /* smarter version */
  /* Top */
  BGFX_DrawLine(x + r, y        , x - r + w, y        , color, Display);
  /* Bottom */
  BGFX_DrawLine(x + r, y + h - 1, x - r + w, y + h - 1, color, Display);
  /* Left */
  BGFX_DrawLine(x        , y + r, x        , y + h - r, color, Display);
  /* Right */
  BGFX_DrawLine(x + w - 1, y + r, x + w - 1, y + h - r, color, Display);
  /* draw four corners */
  BGFX_DrawCircleHelper(x + r, y + r, r, 1, color, Display);
  BGFX_DrawCircleHelper(x + w - r - 1, y + r, r, 2, color, Display);
  BGFX_DrawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color, Display);
  BGFX_DrawCircleHelper(x + r, y + h - r - 1, r, 8, color, Display);
}




void BGFX_DrawRoundRectFill(int16_t x, int16_t y, int16_t w, int16_t h,
    int16_t r, uint16_t color, BGFX_Parameters_t Display) {
  int16_t max_radius;

  max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius){
    r = max_radius;
  }
  /* smarter version */
  BGFX_DrawRectFill(x + r, y, w - 2 * r, h, color, Display);
  /* draw four corners */
  BGFX_DrawCircleFillHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color,
      Display);
  BGFX_DrawCircleFillHelper(x + r, y + r, r, 2, h - 2 * r - 1, color,
      Display);
}




void BGFX_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color,
    BGFX_Parameters_t Display)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  uint16_t x = 0;
  uint16_t y = r;

  BGFX_DrawPixel(x0, y0 + r, color, Display);
  BGFX_DrawPixel(x0, y0 - r, color, Display);
  BGFX_DrawPixel(x0 + r, y0, color, Display);
  BGFX_DrawPixel(x0 - r, y0, color, Display);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    BGFX_DrawPixel(x0 + x, y0 + y, color,  Display);
    BGFX_DrawPixel(x0 - x, y0 + y, color,  Display);
    BGFX_DrawPixel(x0 + x, y0 - y, color,  Display);
    BGFX_DrawPixel(x0 - x, y0 - y, color,  Display);
    BGFX_DrawPixel(x0 + y, y0 + x, color,  Display);
    BGFX_DrawPixel(x0 - y, y0 + x, color,  Display);
    BGFX_DrawPixel(x0 + y, y0 - x, color,  Display);
    BGFX_DrawPixel(x0 - y, y0 - x, color,  Display);
  }
}




void BGFX_DrawCircleFill(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color,
    BGFX_Parameters_t Display)
{
  BGFX_DrawLine(x0, y0 - r, x0, y0 + r, color, Display);
  BGFX_DrawCircleFillHelper(x0, y0, r, 3, 0, color, Display);
}






void BGFX_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
    int16_t x2, int16_t y2, uint16_t color, BGFX_Parameters_t Display) {
  BGFX_DrawLine(x0, y0, x1, y1, color, Display);
  BGFX_DrawLine(x1, y1, x2, y2, color, Display);
  BGFX_DrawLine(x2, y2, x0, y0, color, Display);
}






void BGFX_DrawTriangleFill(uint16_t x0, uint16_t y0, uint16_t x1,
    uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color,
    BGFX_Parameters_t Display)
{
  int16_t a, b, y, last;

  /* Sort coordinates by Y order (y2 >= y1 >= y0) */
  if (y0 > y1) {
    _swap_int16_t(y0, y1);
    _swap_int16_t(x0, x1);
  }
  if (y1 > y2) {
    _swap_int16_t(y2, y1);
    _swap_int16_t(x2, x1);
  }
  if (y0 > y1) {
    _swap_int16_t(y0, y1);
    _swap_int16_t(x0, x1);
  }

  if (y0 == y2) { /* Handle awkward all-on-same-line case as its own thing */
    a = b = x0;
    if (x1 < a){
      a = x1;
    }else if (x1 > b){
      b = x1;
    }
    if (x2 < a){
      a = x2;
    }else if (x2 > b){
      b = x2;
    }
    BGFX_DrawLine(a, y0, a + b - a + 1, y0,color, Display);
    return;
  }

  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
      dx12 = x2 - x1, dy12 = y2 - y1;
  int32_t sa = 0, sb = 0;

   /*
    * For upper part of triangle, find scanline crossings for segments
    * 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    * is included here (and second loop will be skipped, avoiding a /0
    * error there), otherwise scanline y1 is skipped here and handled
    * in the second loop...which also avoids a /0 error here if y0=y1
    * (flat-topped triangle).
    */
  if (y1 == y2){
    last = y1; /* Include y1 scanline */
  }else{
    last = y1 - 1; /* Skip it */
  }

  for (y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
     */
    if (a > b){
      _swap_int16_t(a, b);
    }
    BGFX_DrawLine(a, y, a + b - a + 1, y,color, Display);
    /* writeFastHLine(a, y, b - a + 1, color); */
  }

  /* For lower part of triangle, find scanline crossings for segments
   * 0-2 and 1-2.  This loop is skipped if y1=y2.
   */
  sa = (int32_t)dx12 * (y - y1);
  sb = (int32_t)dx02 * (y - y0);
  for (; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
     */
    if (a > b){
      _swap_int16_t(a, b);
    }
    BGFX_DrawLine(a, y, a + b - a + 1, y,color, Display);
  }
}






void BGFX_DrawChar(uint16_t x, uint16_t y, uint8_t c, uint16_t color,
    uint16_t bg, uint8_t size_x, uint8_t size_y, BGFX_Parameters_t Display)
{

  if (Display.GfxFont == NULL) { /* 'Classic' built-in font */

    if ((x >= Display.Width) ||              /* Clip right  */
        (y >= Display.Height) ||             /* Clip bottom */
        ((x + 6 * size_x - 1) < 0) ||        /* Clip left   */
        ((y + 8 * size_y - 1) < 0))          /* Clip top    */
    {
      return;
    }

    if (!Display.Cp437 && (c >= 176)){
      c++; /* Handle 'classic' charset behavior */
    }


    for (int8_t i = 0; i < 5; i++) { /* Char bitmap = 5 columns */
      uint8_t line = glcdfont[c * 5 + i];
      for (int8_t j = 0; j < 8; j++, line >>= 1) {
        if (line & 1) {
          if (size_x == 1 && size_y == 1){
            BGFX_DrawPixel(x + i, y + j, color, Display);
          }else{
            BGFX_DrawRectFill(x + i * size_x, y + j * size_y, size_x, size_y,
                color, Display);
          }
        } else if (bg != color) {
          if (size_x == 1 && size_y == 1){
            BGFX_DrawPixel(x + i, y + j, bg, Display);
          }else{
            BGFX_DrawRectFill(x + i * size_x, y + j * size_y, size_x, size_y,
                bg, Display);
          }
        }
      }
    }
    if (bg != color) { /* If opaque, draw vertical line for last column */
      if (size_x == 1 && size_y == 1){
        BGFX_DrawLine(x + 5, y, x + 5, y + 8, bg, Display);
      }else{
        BGFX_DrawRectFill(x + 5 * size_x, y, size_x, 8 * size_y, bg, Display);
      }
    }

  }
}




void BGFX_SendString(uint16_t X, uint16_t Y, uint8_t *Buffer, uint32_t Length,
    uint16_t Color, uint16_t Bgc, uint8_t SizeX, uint8_t SizeY,
    BGFX_Parameters_t Display)
{
  uint32_t i;

  if (Display.GfxFont == NULL) {
    for(i = 0; i< Length; i++){
      BGFX_DrawChar(X, Y, *(Buffer + i), Color, Bgc, SizeX,
          SizeY, Display);
      X += SizeX * 6; /* Advance x one char */
    }
  }
}






void BGFX_SetRotation(uint8_t Rotation, BGFX_Parameters_t *Display)
{
  Display->Rotation = (Rotation & 3);
  switch (Display->Rotation) {
  case 0:
  case 2:
    Display->Width = Display->WIDTH;
    Display->Height = Display->HEIGHT;
    break;
  case 1:
  case 3:
    Display->Width = Display->HEIGHT;
    Display->Height = Display->WIDTH;
    break;
  }
}


/**************************************************************************/
/*!
    @brief  Draw a pixel to a framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  color Binary (on or off) color to fill with
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawPixel_01(uint16_t x, uint16_t y, uint16_t color,
    BGFX_Parameters_t Display)
{
  uint8_t *ptr;

  if(Display.HeightPixelsOnWrite > 1 && Display.WidthPixelsOnWrite == 1)
  {
    /* There are 8 vertical pixels in one byte */
    _swap_int16_t(x, y);
  }
  else if(Display.HeightPixelsOnWrite == 1 && Display.WidthPixelsOnWrite > 1)
  {
    /* There are 8 horizontal pixels in one byte */
  }

  //ptr = &Display.Buffer[(x / 8) * Display.WIDTH + y];
  ptr = (uint8_t *)Display.Buffer;
  ptr += (x / 8) * Display.WIDTH + y;

  if (color){
    *ptr |= 1 << (x & 7);
  }else{
    *ptr &= ~(1 << (x & 7));
  }
}


/**************************************************************************/
/*!
    @brief  Draw a pixel to the canvas framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  color 8-bit Color to fill with. Only lower byte of uint16_t is used.
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawPixel_8(uint16_t x, uint16_t y, uint16_t color,
    BGFX_Parameters_t Display)
{
  uint8_t *p = (uint8_t *)Display.Buffer;
  p[x + y * Display.WIDTH] = color & 0xFF;
}


/**************************************************************************/
/*!
    @brief  Draw a pixel to the canvas framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  color 16-bit 5-6-5 Color to fill with
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawPixel_16(uint16_t x, uint16_t y, uint16_t color,
    BGFX_Parameters_t Display)
{
  uint16_t *p = (uint16_t *)Display.Buffer;
  p[x + y * Display.WIDTH] = color;
}



/**************************************************************************/
/*!
    @brief  Quarter-circle drawer, used to do circles and roundrects
    @param  x0   Center-point x coordinate
    @param  y0   Center-point y coordinate
    @param  r   Radius of circle
    @param  cornername  Mask bit #1 or bit #2 to indicate which quarters of
            the circle we're doing
    @param  color 16-bit 5-6-5 Color to draw with
    @param  Display Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawCircleHelper(uint16_t x0, uint16_t y0, uint16_t r,
    uint8_t cornername, uint16_t color, BGFX_Parameters_t Display) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  uint16_t x = 0;
  uint16_t y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      BGFX_DrawPixel(x0 + x, y0 + y, color, Display);
      BGFX_DrawPixel(x0 + y, y0 + x, color, Display);
    }
    if (cornername & 0x2) {
      BGFX_DrawPixel(x0 + x, y0 - y, color, Display);
      BGFX_DrawPixel(x0 + y, y0 - x, color, Display);
    }
    if (cornername & 0x8) {
      BGFX_DrawPixel(x0 - y, y0 + x, color, Display);
      BGFX_DrawPixel(x0 - x, y0 + y, color, Display);
    }
    if (cornername & 0x1) {
      BGFX_DrawPixel(x0 - y, y0 - x, color, Display);
      BGFX_DrawPixel(x0 - x, y0 - y, color, Display);
    }
  }
}



/**************************************************************************/
/*!
    @brief  Quarter-circle drawer with fill, used for circles and roundrects
    @param  x0       Center-point x coordinate
    @param  y0       Center-point y coordinate
    @param  r        Radius of circle
    @param  corners  Mask bits indicating which quarters we're doing
    @param  delta    Offset from center-point, used for round-rects
    @param  color    16-bit 5-6-5 Color to fill with
    @param  Display  Structure to display parameters and functions
 */
/**************************************************************************/
void BGFX_DrawCircleFillHelper(uint16_t x0, uint16_t y0, uint16_t r,
    uint8_t corners, int16_t delta, uint16_t color, BGFX_Parameters_t Display)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  uint16_t x = 0;
  uint16_t y = r;
  uint16_t px = x;
  uint16_t py = y;

  delta++; /* Avoid some +1's in the loop */

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    /* These checks avoid double-drawing certain lines, important
     * for the SSD1306 library which has an INVERT drawing mode.
     */
    if (x < (y + 1)) {
      if (corners & 1){
        BGFX_DrawLine(x0 + x, y0 - y, x0 + x, y0 + y + delta, color, Display);
      }
      if (corners & 2){
        BGFX_DrawLine(x0 - x, y0 - y, x0 - x, y0 + y + delta, color, Display);
      }
    }
    if (y != py) {
      if (corners & 1){
        BGFX_DrawLine(x0 + py, y0 - px, x0 + py, y0 + px + delta, color, Display);
      }
      if (corners & 2){
        BGFX_DrawLine(x0 - py, y0 - px, x0 - py, y0 + px + delta, color, Display);
      }
      py = y;
    }
    px = x;
  }
}

