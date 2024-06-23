//#include "drivers/drivers.hpp"
#include <iostream>
#include "drivers.hpp"

DrvCommBase tester;
DrvStdInOut serial;
constexpr uint8_t CHARACTER_COUNT = 1;

int main(void)
{
  Status_t status;
  uint8_t buffer[10];
  serial.write((uint8_t *)"Hello world!!!", sizeof("Hello world!!!") - 1);
  serial.write((uint8_t *)"Hello world!!!", sizeof("Hello world!!!") - 1);
  serial.write((uint8_t *)"Hello world!!!", sizeof("Hello world!!!") - 1);
  serial.read(buffer, CHARACTER_COUNT);
  serial.write(buffer, CHARACTER_COUNT);
  return 0;
}