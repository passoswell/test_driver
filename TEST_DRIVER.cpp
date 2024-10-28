#include "drivers.hpp"
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>


void run_test(void)
{
  // Add test code here
}


#if defined(USE_ESP32)

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#include "esp_task_wdt.h"


extern "C" void app_main(void)
{
  esp_task_wdt_deinit();

  run_test();

  while(1)
  {
    vTaskDelay(portMAX_DELAY);
  }
}

#else

int main(void)
{
  run_test();
  return 0;
}

#endif