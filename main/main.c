#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "Transition.h"
#include "ga.h"
#include "handlers.h"
#include "driver.h"

#include "gapps/gapps.h"
#include "api/GraphicalApi/toolbar.h"

uint32_t last_toolbar_update = 0;

static void input_task(void *arg)
{
    TickType_t last = xTaskGetTickCount();
    while (1) {
        uipad();
        vTaskDelayUntil(&last, pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
  encoder_init();
  init_ga();

  draw_welcome(disp);
  vTaskDelay(pdMS_TO_TICKS(1600));
  clean_screen(disp);

  toolbar_init();
  toolbar_set_visible(true);

  set_gapp_screen(curapp);

  xTaskCreate(input_task, "input", 4096, NULL, 5, NULL);

  // Main Cycle
  while (1) {
    uint32_t now = lv_tick_get();

    if (now - last_toolbar_update > 1000) {
      toolbar_update("00:00", "100%");
      last_toolbar_update = now;
    }
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}


