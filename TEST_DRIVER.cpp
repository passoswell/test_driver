//#include "drivers/drivers.hpp"
//#include <iostream>
#include "drivers.hpp"
// #include <unistd.h>
#include <string.h>


#if defined(USE_ESP32)

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

void run_esp_test(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}



extern "C" void app_main(void)
{
  // run_test();
  run_esp_test();
}
#else



DrvCommBase tester;
// DrvStdInOut console;
uint8_t message0[] = "\033\143";
uint8_t message1[] = "Hello world!!!";
uint8_t message2[] = "Hello1!!!";
uint8_t message3[] = "This is a new adventure!!!";
uint8_t message4[] = "If you can read this message, blocking mode UART is working!!!";
DrvUART serial((void *)"/dev/ttyUSB0");
//DrvUART serial((void *)"/dev/ttyACM0");

int run_test(void)
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



int main(void)
{
  return run_test();
}
#endif