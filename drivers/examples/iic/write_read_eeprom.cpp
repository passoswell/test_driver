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

#include <string.h>

#include "drivers.hpp"

// Change the lines bellow with the correct handle for your platform
#if defined(USE_LINUX)
const IicPeripheral_t g_peripheral = 17;
IicHandle_t g_handle = (IicHandle_t)"/dev/i2c-17";
#elif defined(USE_ESP32)
IicHandle_t handle =
{
  .iic_number = 0,
  .sda_pin = 21,
  .scl_pin = 22,
};
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
  ADD_PARAMETER(COMM_WORK_ASYNC_TX, false)
};
const uint8_t g_iic_config_list_size = sizeof(g_iic_config_list)/sizeof(g_iic_config_list[0]);

// Start address on the EEPROM to access
const uint16_t START_ADDRESS = 0x0000;
// Number of bytes to write and read
const uint16_t BYTES_TO_WRITE = 512;


static IIC<g_peripheral> g_iic(g_handle, 0x50);
static uint8_t g_addr_table[5];
static uint8_t g_rx_buffer[BYTES_TO_WRITE];
static uint8_t g_tx_buffer[BYTES_TO_WRITE];
static uint8_t g_add_value = 1;


// Read data from the EEPROM memory
static Status_t mem_read(uint16_t address, uint8_t *data, uint16_t size);
// Write data to EEPROM memory
static Status_t mem_write(uint16_t address, uint8_t *data, uint16_t size);
static void printBytes(uint16_t start_address, uint8_t *data, uint16_t size);


/**
 * @brief Write and read data to an eeprom
 */
AP_MAIN()
{
  Status_t status;
  SPT timer;

  timer.delay(2000);

  printf("\r\n\r\nTest program started\r\n");

  memset(g_addr_table, 0, sizeof(g_addr_table));

  status = g_iic.configure(g_iic_config_list, g_iic_config_list_size);
  if(!status.success)
  {
    printf("\r\nERROR failed to configure: %s\r\n", status.description);
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
  printf("\r\nReading %u bytes from memory\r\n", BYTES_TO_WRITE);
  status = mem_read(START_ADDRESS, g_rx_buffer, BYTES_TO_WRITE);
  if(!status.success)
  {
    printf("\r\nERROR from mem_read: %s\r\n", status.description);
    AP_EXIT();
  }
  printf("Read from the memory:\t[Address] Value\r\n");
  printBytes(START_ADDRESS, g_rx_buffer, BYTES_TO_WRITE);



  // Writing to the memory
  printf("Writing %u bytes to memory\r\n", BYTES_TO_WRITE);
  status = mem_write(START_ADDRESS, g_tx_buffer, BYTES_TO_WRITE);
  if(!status.success)
  {
    printf("\r\nERROR from mem_write: %s\r\n", status.description);
    AP_EXIT();
  }
  printf("Wrote to the memory:\t[Address] Value\r\n");
  printBytes(START_ADDRESS, g_tx_buffer, BYTES_TO_WRITE);



  // Reading from the memory
  printf("\r\nReading %u bytes from memory\r\n", BYTES_TO_WRITE);
  status = mem_read(START_ADDRESS, g_rx_buffer, BYTES_TO_WRITE);
  if(!status.success)
  {
    printf("\r\nERROR from mem_read: %s\r\n", status.description);
    AP_EXIT();
  }
  printf("Read from the memory:\t[Address] Value\r\n");
  printBytes(START_ADDRESS, g_rx_buffer, BYTES_TO_WRITE);

  printf("This is the end of the test\r\n");




  AP_EXIT();
}


/**
 * @brief Read data from the EEPROM memory
 *
 * @param address First memory address
 * @param data Data buffer
 * @param size Number of bytes to read
 * @return Status_t
 */
Status_t mem_read(uint16_t address, uint8_t *data, uint16_t size)
{
  Status_t status;
  uint8_t reg_addr_buffer[2];

  // Reading from the memory
  reg_addr_buffer[0] = (address >> 8) & 0x0F;
  reg_addr_buffer[1] = address & 0xFF;

  status = g_iic.write(reg_addr_buffer, 2, 100);
  if(!status.success)
  {
    printf("\r\nERROR from my_serial.write: %s\r\n", status.description);
    return status;
  }
  while(!g_iic.getWriteStatus().success);

  status = g_iic.read(data, size, 100);
  if(!status.success)
  {
    printf("\r\nERROR from my_serial.read: %s\r\n", status.description);
    return status;
  }
  while(!g_iic.getReadStatus().success);

  return STATUS_DRV_SUCCESS;

}

/**
 * @brief Write data to EEPROM memory
 *
 * @param address First memory address
 * @param data Data buffer
 * @param size Number of bytes to write
 * @return Status_t
 */
Status_t mem_write(uint16_t address, uint8_t *data, uint16_t size)
{
  SPT timer;
  Status_t status;
  uint8_t buffer[34];
  const uint8_t divisor = 32;
  uint8_t loop_count = size / divisor;
  uint8_t bytes_last_loop = size % divisor;

  for(uint16_t i = 0; i < loop_count; i++)
  {
    buffer[0] = (address >> 8) & 0x0F;
    buffer[1] = address & 0xFF;

    memcpy(&buffer[2], data, divisor);

    status = g_iic.write(buffer, divisor + 2, 100);
    if(!status.success)
    {
      printf("\r\nERROR from my_serial.write: %s\r\n", status.description);
      return status;
    }
    while(!g_iic.getWriteStatus().success);
    timer.delay(25);
    address += divisor;
    data += divisor;
  }

  if(bytes_last_loop > 0)
  {
    buffer[0] = (address >> 8) & 0x0F;
    buffer[1] = address & 0xFF;

    memcpy(&buffer[2], data, bytes_last_loop);

    status = g_iic.write(buffer, bytes_last_loop + 2, 100);
    if(!status.success)
    {
      printf("\r\nERROR from my_serial.write: %s\r\n", status.description);
      return status;
    }
    while(!g_iic.getWriteStatus().success);
    timer.delay(25);
  }

  return STATUS_DRV_SUCCESS;
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
      printf("\r\n");
    }
    printf("[%03X] %3u   ", i, data[i]);
  }
  printf("\r\n\r\n");
}