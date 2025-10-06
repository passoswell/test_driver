/**
 * @file write_read_eeprom.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-26
 *
 * @copyright Copyright (c) 2025
 *
 * @note In this example, a I2C port configured by a handle and
 * g_iic_config_list. The port is used to read and write BYTES_TO_WRITE bytes
 * to the EEPROM model AT24C32
 * https://ww1.microchip.com/downloads/en/DeviceDoc/doc0336.pdf
 * This device can receive batches of at most 32 bytes, while any amount of
 * bytes can be read sequentially.
 *
 */

#include <cstring>

#include "drivers.hpp"

// Change the lines bellow with the correct handle for your platform
#if defined(USE_LINUX)
constexpr IicHandle_t handle = 0; // /dev/i2c-0
#elif defined(USE_ESP32)
constexpr IicHandle_t handle = 0;
// {
//   .iic_number = 0,
//   .sda_pin = 21,
//   .scl_pin = 22,
// };
#else
void *handle = nullptr;
#endif

/**
 * @brief Configuration parameters for the iic port
 */
const SettingsList_t g_iic_config_list[]
{
  ADD_PARAMETER(COMM_PARAM_CLOCK_SPEED, 100000),
  ADD_PARAMETER(COMM_USE_PULL_UP, true),
  ADD_PARAMETER(COMM_WORK_ASYNC_RX, false),
  ADD_PARAMETER(COMM_WORK_ASYNC_TX, false),
  ADD_PARAMETER(COMM_PARAM_SDA_DIO_PIN, 21),
  ADD_PARAMETER(COMM_PARAM_CS_DIO_PIN, 22),
};
const uint8_t g_iic_config_list_size = sizeof(g_iic_config_list)/sizeof(g_iic_config_list[0]);

// Start address on the EEPROM to access
const uint16_t START_ADDRESS = 0x0000;
// Number of bytes to write and read
const uint16_t BYTES_TO_WRITE = 512;

static uint16_t g_eeprom_address = 0x50;
static IIC<handle> g_iic(g_eeprom_address);
static uint8_t g_addr_table[5];
static uint8_t g_rx_buffer[BYTES_TO_WRITE];
static uint8_t g_tx_buffer[BYTES_TO_WRITE];
static uint8_t g_add_value = 1;


// Read data from the EEPROM memory
static ErrorCode mem_read(uint16_t address, uint8_t *data, uint16_t size);
// Write data to EEPROM memory
static ErrorCode mem_write(uint16_t address, uint8_t *data, uint16_t size);
static void printBytes(uint16_t start_address, uint8_t *data, uint16_t size);


/**
 * @brief Write and read data to an eeprom
 */
AP_MAIN()
{
  ErrorCode status;
  SPT timer;

  timer.delay(2000);

  std::printf("\r\n\r\nTest program started\r\n");

  std::memset(g_addr_table, 0, sizeof(g_addr_table));

  status = g_iic.configure(g_iic_config_list, g_iic_config_list_size);
  if(status)
  {
    std::printf("\r\nERROR failed to configure: %s\r\n", status.message().data());
    AP_EXIT();
  }

  // Compute data to write to the memory
  for(uint16_t i = 0; i < BYTES_TO_WRITE; i++)
  {
    if((i & 255) == 0)
    {
      g_add_value += 3;
    }
    g_tx_buffer[i] = 2 * i + g_add_value;
  }


  // Reading from the memory
  std::printf("\r\nReading %u bytes from memory\r\n", BYTES_TO_WRITE);
  status = mem_read(START_ADDRESS, g_rx_buffer, BYTES_TO_WRITE);
  if(status)
  {
    std::printf("\r\nERROR from mem_read: %s\r\n", status.message().data());
    AP_EXIT();
  }
  std::printf("Read from the memory:\t[Address] Value\r\n");
  printBytes(START_ADDRESS, g_rx_buffer, BYTES_TO_WRITE);



  // Writing to the memory
  std::printf("Writing %u bytes to memory\r\n", BYTES_TO_WRITE);
  status = mem_write(START_ADDRESS, g_tx_buffer, BYTES_TO_WRITE);
  if(status)
  {
    std::printf("\r\nERROR from mem_write: %s\r\n", status.message().data());
    AP_EXIT();
  }
  std::printf("Wrote to the memory:\t[Address] Value\r\n");
  printBytes(START_ADDRESS, g_tx_buffer, BYTES_TO_WRITE);



  // Reading from the memory
  std::printf("\r\nReading %u bytes from memory\r\n", BYTES_TO_WRITE);
  status = mem_read(START_ADDRESS, g_rx_buffer, BYTES_TO_WRITE);
  if(status)
  {
    std::printf("\r\nERROR from mem_read: %s\r\n", status.message().data());
    AP_EXIT();
  }
  std::printf("Read from the memory:\t[Address] Value\r\n");
  printBytes(START_ADDRESS, g_rx_buffer, BYTES_TO_WRITE);

  std::printf("This is the end of the test\r\n");




  AP_EXIT();
}


/**
 * @brief Read data from the EEPROM memory
 *
 * @param address First memory address
 * @param data Data buffer
 * @param size Number of bytes to read
 * @return ErrorCode
 */
ErrorCode mem_read(uint16_t address, uint8_t *data, uint16_t size)
{
  ErrorCode status;
  uint8_t reg_addr_buffer[2];

  // Reading from the memory
  reg_addr_buffer[0] = (address >> 8) & 0x0F;
  reg_addr_buffer[1] = address & 0xFF;

  status = g_iic.write(reg_addr_buffer, 100);
  if(status)
  {
    std::printf("\r\nERROR from my_serial.write: %s\r\n", status.message().data());
    return status;
  }
  // while(!g_iic.getWriteStatus().success);

  status = g_iic.read({data, size}, 100);
  if(status)
  {
    std::printf("\r\nERROR from my_serial.read: %s\r\n", status.message().data());
    return status;
  }
  // while(!g_iic.getReadStatus().success);

  return status;

}

/**
 * @brief Write data to EEPROM memory
 *
 * @param address First memory address
 * @param data Data buffer
 * @param size Number of bytes to write
 * @return ErrorCode
 */
ErrorCode mem_write(uint16_t address, uint8_t *data, uint16_t size)
{
  SPT timer;
  ErrorCode status;
  uint8_t buffer[34];
  const uint8_t divisor = 32;
  uint8_t loop_count = size / divisor;
  uint8_t bytes_last_loop = size % divisor;

  for(uint16_t i = 0; i < loop_count; i++)
  {
    buffer[0] = (address >> 8) & 0x0F;
    buffer[1] = address & 0xFF;

    std::memcpy(&buffer[2], data, divisor);

    status = g_iic.write({buffer, divisor + 2}, 100);
    if(status)
    {
      std::printf("\r\nERROR from my_serial.write: %s\r\n", status.message().data());
      return status;
    }
    // while(!g_iic.getWriteStatus().success);
    timer.delay(25);
    address += divisor;
    data += divisor;
  }

  if(bytes_last_loop > 0)
  {
    buffer[0] = (address >> 8) & 0x0F;
    buffer[1] = address & 0xFF;

    std::memcpy(&buffer[2], data, bytes_last_loop);

    status = g_iic.write({buffer, bytes_last_loop + 2u}, 100);
    if(status)
    {
      std::printf("\r\nERROR from my_serial.write: %s\r\n", status.message().data());
      return status;
    }
    // while(!g_iic.getWriteStatus().success);
    timer.delay(25);
  }

  return status;
}

/**
 * @brief Print a array on the terminal
 *
 * @param address First memory address
 * @param data Data buffer
 * @param size Number of bytes to print
 */
void printBytes(uint16_t start_address, uint8_t *data, uint16_t size)
{
  for(uint16_t i = 0; i < size; i++)
  {
    if(((i&15) == 0) && i != 0)
    {
      std::printf("\r\n");
    }
    std::printf("[%03X] %3u   ", i, data[i]);
  }
  std::printf("\r\n\r\n");
}