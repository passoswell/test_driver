//#include "drivers/drivers.hpp"
//#include <iostream>
#include "drivers.hpp"
// #include <unistd.h>
#include <string.h>

DrvCommBase tester;
// DrvStdInOut console;
uint8_t message0[] = "\033\143";
uint8_t message1[] = "Hello world!!!";
uint8_t message2[] = "Hello1!!!";
uint8_t message3[] = "This is a new adventure!!!";
uint8_t message4[] = "If you can read this message, blocking mode UART is working!!!";
DrvUART serial((void *)"/dev/ttyUSB0");
//DrvUART serial((void *)"/dev/ttyACM0");

int main(void)
{
  Status_t status;
  uint8_t buffer[100];
  uint32_t byte_count;
  // console.write(message0, strlen((char *)message0));
  // console.write(message1, strlen((char *)message1));
  // console.write(message1, strlen((char *)message1));
  // console.write(message1, strlen((char *)message1));
  // // console.read(buffer, CHARACTER_COUNT);
  // console.write(buffer, CHARACTER_COUNT);

  status = serial.configure(nullptr, 0);
  if(!status.success)
  {
    // console.write((uint8_t *)status.description, strlen(status.description));
    return -1;
  }

  // serial.write(message2, strlen((char *)message2));
  // //usleep(4*250000);
  // serial.read(buffer, strlen((char *)message2));
  // byte_count = serial.bytesRead();
  // console.write(buffer, byte_count);

  // serial.write(message3, strlen((char *)message3));
  // //usleep(4*250000);
  // serial.read(buffer, strlen((char *)message3));
  // byte_count = serial.bytesRead();
  // console.write(buffer, byte_count);

  // serial.write(message4, strlen((char *)message4));
  // //usleep(4*250000);
  // serial.read(buffer, strlen((char *)message4));
  // byte_count = serial.bytesRead();
  // console.write(buffer, byte_count);

  return 0;
}