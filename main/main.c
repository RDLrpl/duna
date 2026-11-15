#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "Transition.h"
#include "ga.h"
#include "handlers.h"

#include "gapps/control.h"
#include "api/GraphicalApi/toolbar.h"
#include "api/GraphicalApi/render.h"
#include "api/timemod/rtctime.h"
#include "api/wifi/wifidrive.h"
#include "config.h"
#include "nvs_flash.h"

static uint32_t last_toolbar_update = 0;
static uint32_t last_scroll_time    = 0;
static uint32_t last_render_check   = 0;

void app_main(void)
{
  // INIT
  init_joystick();
  init_ga();
  init_timemod();

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  wd_init();

  // WELCOME
  draw_welcome(disp);
  vTaskDelay(pdMS_TO_TICKS(1600));
  clean_screen(disp);

  vTaskDelay(pdMS_TO_TICKS(800));
  toolbar_init();
  toolbar_set_visible(true);

  // Main Menu
  if (!lvgl_port_lock(portMAX_DELAY)) return;
  set_gapp_screen(GAPP_SCREEN_MAIN);
  lvgl_port_unlock();

  // Main Cycle
  while (1) {
    uint32_t now = lv_tick_get();

    handle_joystick();
    PadHandler joy = get_pad_handler();

    if (now - last_scroll_time > menu_scroll_delay) {
      bool scroll = false;
      bool click  = false;

      if (joy.deltax != 0 || joy.deltay != 0)    scroll = standart_duna_menulogic(joy.deltax, joy.deltay, get_groller());
      if (joy.clk)           click  = menu_click_logic(get_groller(), false);
      if (joy.long_clk)      click  = menu_click_logic(get_groller(), true);

      if (scroll || click) last_scroll_time = now;
    }

    if (now - last_toolbar_update > 1000) {
      const char *time_str = get_time(false);

      toolbar_update(time_str, "100%");
      last_toolbar_update = now;
    }

    if (render_get_mode() != DUNA_MANUAL &&
        now - last_render_check >= get_render_interval()) {
      auto_render_update();
      last_render_check = now;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}