//#include "drivers/drivers.hpp"
#include <iostream>
#include "drivers.hpp"
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "drivers/linux/i2c/basic_graphics.h"
#include "drivers/linux/i2c/dev_ssd1306.hpp"

DrvCommBase tester;
DrvStdInOut console;
uint8_t message0[] = "\033\143";
uint8_t message1[] = "Hello world!!!";
uint8_t message2[] = "Hello1!!!";
uint8_t message3[] = "This is a new adventure!!!";
uint8_t message4[] = "If you can read this message, blocking mode UART is working!!!";
DrvUART serial((void *)"/dev/ttyUSB0");
// DrvUART serial((void *)"/dev/ttyACM0");
DrvI2C two_wire((void *)"/dev/i2c-0");
SSD1306 display(&two_wire);

// int main(void)
// {
//   Status_t status;
//   uint8_t buffer[100];
//   uint32_t byte_count;
//   console.write(message0, strlen((char *)message0));
//   console.write(message1, strlen((char *)message1));
//   console.write(message2, strlen((char *)message2));
//   console.write(message3, strlen((char *)message3));
//   // console.read(buffer, CHARACTER_COUNT);
//   // console.write(buffer, CHARACTER_COUNT);

//   // console.writeAsync(message1, strlen((char *)message1), 0);
//   // console.readAsync(buffer, 10, 0);

//   // while(true)
//   // {
//   //   // buffer[0] = sleep(1);
//   //   // buffer[0] += '0';
//   //   // console.write(buffer, 1, 0);
//   //   // console.write(message1, strlen((char *)message1));

//   //   // if(console.isWriteAsyncDone())
//   //   // {
//   //   //   buffer[0] = sleep(1);
//   //   //   buffer[0] += '1';
//   //   //   console.write(buffer, 1, 0);
//   //   //   console.writeAsync(message1, strlen((char *)message1), 0);
//   //   // }

//   //   if(console.isReadAsyncDone())
//   //   {
//   //     console.write(buffer, 10, 0);
//   //     console.readAsync(buffer, 10, 0);
//   //   }
//   // }

//   status = serial.configure(nullptr, 0);
//   if(!status.success)
//   {
//     console.write((uint8_t *)status.description, strlen(status.description));
//     return -1;
//   }

//   serial.write(message4, 9, 0);
//   serial.readAsync(buffer, 10,0);
//   // serial.read(buffer, 10, 0, 5000);
//   while (!serial.isReadAsyncDone()){}
//   std::cout << "Received " << serial.bytesRead() << " bytes from serial" << std::endl;
//   console.write(buffer, serial.bytesRead(),0);

//   while(1)
//   {
//     // if(serial.isWriteAsyncDone())
//     // {
//     //   serial.writeAsync(message1, strlen((char *)message1), 0);
//     // }
//     // if(serial.isReadAsyncDone())
//     // {
//     //   if(console.isWriteAsyncDone())
//     //   {
//     //     console.write(buffer, strlen((char *)message1),0);
//     //   }
//     //   serial.readAsync(buffer, strlen((char *)message1),0);
//     // }

//     // serial.writeAsync(message1, strlen((char *)message1), 0);
//     // // serial.write(message1, strlen((char *)message1), 0);
//     // while (!serial.isWriteAsyncDone()){}
//     // serial.readAsync(buffer, strlen((char *)message1),0);
//     // // serial.read(buffer, strlen((char *)message1),0);
//     // while (!serial.isReadAsyncDone()){}
//     // console.write(buffer, strlen((char *)message1),0);
//     // while (!console.isWriteAsyncDone()){}
//   }

//   return 0;

//   serial.write(message2, strlen((char *)message2));
//   //usleep(4*250000);
//   serial.read(buffer, strlen((char *)message2));
//   byte_count = serial.bytesRead();
//   console.write(buffer, byte_count);

//   serial.write(message3, strlen((char *)message3));
//   //usleep(4*250000);
//   serial.read(buffer, strlen((char *)message3));
//   byte_count = serial.bytesRead();
//   console.write(buffer, byte_count);

//   serial.write(message4, strlen((char *)message4));
//   //usleep(4*250000);
//   serial.read(buffer, strlen((char *)message4));
//   byte_count = serial.bytesRead();
//   console.write(buffer, byte_count);

//   return 0;
// }






/* Prototype to a possible function to write the pixels to the display */
void DISP_WritePixels(uint8_t *Buffer, uint16_t Size)
{
  /* Your display code here */
  display.writePixels(Buffer, Size);
}

/* Clears the buffer */
void DISP_ClearBuffer(uint8_t *Buffer, uint32_t Size, uint16_t color)
{
  for(int i=0; i<Size; i++) {Buffer[i] = color;}
}

/* Blocking delay */
void Delay(void)
{
  /* Change for your system's delay */
  //for(int i = 0; i < 5000000; i++);
  std::this_thread::sleep_for(std::chrono::seconds(3));
}


int main(void)
{
  /* Writing in an area with size 128 x 64 */
  static uint8_t buffer[128 * 8]; /* Pixels buffer */
  static uint32_t buffer_size = sizeof(buffer)/sizeof(uint8_t);
  static BGFX_Parameters_t BGFX_1;
  uint16_t main_color = 1; //Turns on a pixel
  uint16_t background_color = 0; //Turns off a pixel
  uint32_t counter = 0;

  BGFX_1.HEIGHT      = 64;
  BGFX_1.WIDTH       = 128;
  BGFX_1.WidthPixelsOnWrite  = 1;
  BGFX_1.HeightPixelsOnWrite = 8; /* Number of pixels in a page */
  BGFX_1.Rotation = 0; /* No software rotation intended */
  BGFX_1.ColorScheme = BGFX_MONOCHROMATIC;
  BGFX_1.Buffer      = (void*)buffer;
  BGFX_1.DrawPixel   = NULL;
  BGFX_1.GfxFont     = NULL;

  console.write((uint8_t *)"Initializing the display", sizeof((uint8_t *)"Initializing the display"));
  display.initialize();
  console.write((uint8_t *)"Setting cursor to (0, 0)", sizeof((uint8_t *)"Setting cursor to (0, 0)"));
  display.setCursor(0,0);
  console.write((uint8_t *)"Running demo", sizeof((uint8_t *)"Running demo"));

  /* Setting rotation to zero */
  BGFX_SetRotation(0, &BGFX_1);


  /* Cleaning the buffer */
  DISP_ClearBuffer(buffer, buffer_size, background_color);

  /* Drawing a few lines to the buffer */
  BGFX_DrawLine(1, 1, 1, 62, main_color, BGFX_1);     /* Top    */
  BGFX_DrawLine(1, 62, 126, 62, main_color, BGFX_1);  /* Right  */
  BGFX_DrawLine(126, 62, 126, 1, main_color, BGFX_1); /* Bottom */
  BGFX_DrawLine(126, 1, 1, 1, main_color, BGFX_1);    /* Left   */

  BGFX_DrawLine(0, 0, 127, 63, main_color, BGFX_1);   /* 1st diagonal */
  BGFX_DrawLine(0, 63, 127, 0, main_color, BGFX_1);   /* 2nd diagonal */

  /* Updating the display */
  DISP_WritePixels(buffer, buffer_size);

  Delay();


  /* Cleaning the buffer, a different background color */
  DISP_ClearBuffer(buffer, buffer_size, main_color);

  /* Drawing a few lines to the buffer with different color */
  BGFX_DrawLine(1, 1, 1, 62, background_color, BGFX_1);     /* Top    */
  BGFX_DrawLine(1, 62, 126, 62, background_color, BGFX_1);  /* Right  */
  BGFX_DrawLine(126, 62, 126, 1, background_color, BGFX_1); /* Bottom */
  BGFX_DrawLine(126, 1, 1, 1, background_color, BGFX_1);    /* Left   */

  BGFX_DrawLine(0, 0, 127, 63, background_color, BGFX_1);   /* 1st diagonal */
  BGFX_DrawLine(0, 63, 127, 0, background_color, BGFX_1);   /* 2nd diagonal */

  /* Updating the display */
  DISP_WritePixels(buffer, buffer_size);

  Delay();


  /* Cleaning the buffer */
  DISP_ClearBuffer(buffer, buffer_size, background_color);

  /* Drawing rectangles */
  BGFX_DrawRect(25, 0, 10, 64, main_color, BGFX_1);
  BGFX_DrawRectFill(51, 0, 10, 64, main_color, BGFX_1);

  /* Updating the display */
  DISP_WritePixels(buffer, buffer_size);

  Delay();


  /* Cleaning the buffer */
  DISP_ClearBuffer(buffer, buffer_size, background_color);

  /* Drawing round rectangles */
  BGFX_DrawRoundRect(0, 0, 50, 64, 10, main_color, BGFX_1);
  BGFX_DrawRoundRectFill(128-50, 0, 50, 64, 10, main_color, BGFX_1);

  /* Updating the display */
  DISP_WritePixels(buffer, buffer_size);

  Delay();


  /* Cleaning the buffer */
  DISP_ClearBuffer(buffer, buffer_size, background_color);

  /* Drawing triangles */
  BGFX_DrawTriangle(25, 0, 50, 63, 0, 63, main_color, BGFX_1);
  BGFX_DrawTriangleFill(127-25, 0, 127, 63, 127-50, 63, main_color, BGFX_1);

  /* Updating the display */
  DISP_WritePixels(buffer, buffer_size);

  Delay();


  /* Cleaning the buffer */
  DISP_ClearBuffer(buffer, buffer_size, background_color);

  /* Drawing circles */
  BGFX_DrawCircleFill(60, 32, 30, main_color, BGFX_1);
  BGFX_DrawCircle(30, 32, 10, main_color, BGFX_1);
  BGFX_DrawCircle(90, 32, 10, main_color, BGFX_1);
  BGFX_DrawCircle(10, 10, 10, main_color, BGFX_1);
  BGFX_DrawCircle(60, 10, 10, main_color, BGFX_1);

  /* Updating the display */
  DISP_WritePixels(buffer, buffer_size);

  Delay();


  /* Plotting a sine wave */
  /* Cleaning the space where the plot will be positioned */
  BGFX_DrawRectFill(0, 25, 128, 25, main_color, BGFX_1);


  /* Pre-computing sine table */
  uint16_t sine_table[128];
  for(uint32_t i = 0; i < 128; i++)
  {
    float auxf = sinf((float)(i + counter)*0.2);
    auxf *= 12;
    auxf += 37;
    sine_table[i] = auxf;
  }

  uint8_t head = 0;
  uint8_t tail = 127;
  while(1)
  {
    for(uint32_t i = 0; i < 128; i++)
    {
      if(head+i < 128)
      {
        BGFX_DrawPixel(i, sine_table[head+i], background_color, BGFX_1);
      }else
      {
        BGFX_DrawPixel(i, sine_table[(head+i)-128], background_color, BGFX_1);
      }
      if(tail+i < 128)
      {
        BGFX_DrawPixel(i, sine_table[tail+i], main_color, BGFX_1);
      }else
      {
        BGFX_DrawPixel(i, sine_table[(tail+i)-128], main_color, BGFX_1);
      }
    }
    head++;  if(head >= 128){head = 0;}
    tail++;  if(tail >= 128){tail = 0;}
    DISP_WritePixels(buffer, buffer_size);
  }
}