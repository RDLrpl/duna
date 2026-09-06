#include "ui.h"
#include "api/handlers.h"
#include <stdio.h>
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "esp_flash.h"
#include "spi_flash_mmap.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "esp_app_format.h"
#include "esp_image_format.h"

typedef enum {
    SCREEN_MAIN,
    SCREEN_SETTINGS,
    SCREEN_ORBIT, // File Manager
    SCREEN_APPSHUB,
    SCREEN_APIREF, // Custom Apps
} ScreenState; 

static const char* gapps[] = {
    "|Settings",
    "|OrbitFM",
    "|AppsHub",
};

static uint8_t cursor_pos = 0;
static int8_t encoder_mv = 0;

static uint32_t menu_options = 2;

static ScreenState current_screen = SCREEN_MAIN;
static bool need_redraw = true;

void draw_toolbar(u8g2_t *disp) {
    u8g2_SetFont(disp, u8g2_font_helvB12_tr);

    u8g2_SetDrawColor(disp, 1);
    
    u8g2_DrawBox(disp, 0, 0, 128, 16);
    u8g2_SetDrawColor(disp, 0);
    u8g2_DrawStr(disp, 0, 15, "Duna");
    u8g2_SetFont(disp, u8g2_font_helvB08_tr);
    
    u8g2_DrawStr(disp, 100, 15, "00:00");
    u8g2_DrawStr(disp, 70, 15, "000%");
 
    u8g2_SetDrawColor(disp, 1);
}

static bool rom_cached = false;
static uint32_t cached_rom_used = 0;

static void update_rom_cache_once(void) {
    if (rom_cached) return;

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_image_metadata_t data;
    const esp_partition_pos_t running_pos = {
        .offset = running->address,
        .size = running->size,
    };

    if (esp_image_verify(ESP_IMAGE_VERIFY, &running_pos, &data) == ESP_OK) {
        cached_rom_used = data.image_len;
    }
    rom_cached = true;
}


void draw_main(u8g2_t *disp) {
    draw_toolbar(disp);

    u8g2_SetFont(disp, u8g2_font_9x15_tr);

    u8g2_DrawStr(disp, 2, 29, gapps[0]);
    u8g2_DrawStr(disp, 2, 45, gapps[1]);
    u8g2_DrawStr(disp, 2, 61, gapps[2]);

    // cursor

    int8_t cursor_y = 29 + (cursor_pos) * 16;
    int8_t cursor_x = 2 + u8g2_GetStrWidth(disp, gapps[cursor_pos]) + 4;

    u8g2_DrawStr(disp, cursor_x, cursor_y, "<");
}

void draw_settings(u8g2_t *disp) {
    draw_toolbar(disp);

    u8g2_SetFont(disp, u8g2_font_6x12_tr);

    // RAM
    char ram_status_buf[64];

    size_t total_ram = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    size_t free_ram = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);

    snprintf(ram_status_buf, sizeof(ram_status_buf), "RaM: %.1f/%.1fKB", (total_ram - free_ram) / 1024.0, total_ram / 1024.0);

    // ROM
    update_rom_cache_once();
    char rom_status_buf[64];
    snprintf(rom_status_buf, sizeof(rom_status_buf), "Os: %.1fKB", cached_rom_used / 1024.0);

    // Uptime
    char uptime_buf[32];
    int64_t uptime_sec = esp_timer_get_time() / 1000000;
    snprintf(uptime_buf, sizeof(uptime_buf), "Uptime: %lld:%lld:%lld",
             uptime_sec / 3600, (uptime_sec / 60) % 60, uptime_sec % 60);

    u8g2_DrawStr(disp, 2, 29, "Hold|Exit/CLK|Update");

    // UI
    char* settings[] = {
        ram_status_buf,
        rom_status_buf,
        uptime_buf,
        "WiFi",
        "BT",
    };

    int8_t scroll_offset = 0;
    if (cursor_pos >= 3) {
        scroll_offset = cursor_pos - 3 + 1;
    }

    for (int8_t i = 0; i < 3; i++) {
        int8_t item_idx = scroll_offset + i;

        if (item_idx >= 5) break; 

        int8_t current_y = 44 + (i * 9);

        u8g2_DrawStr(disp, 2, current_y, settings[item_idx]);

        if (item_idx == cursor_pos) {
            int8_t cursor_x = 2 + u8g2_GetStrWidth(disp, settings[item_idx]) + 4;
            u8g2_DrawStr(disp, cursor_x, current_y, "<");
        }
    }
}

void uipad(void) {
    PadHandler *pad = api_get_padhandler();

    encoder_mv += pad->encoder_change;
    pad->encoder_change = 0;

    switch (current_screen)
    {
        case SCREEN_SETTINGS:
            if (pad->long_clk) {
                pad->long_clk = false;

                current_screen = SCREEN_MAIN;
                cursor_pos = 0;
                encoder_mv = 0;
                menu_options = 2;

                need_redraw = true;
            }
            if (pad->clk) {
                pad->clk = false; 
                need_redraw = true;
            }
            break;
        case SCREEN_MAIN:
            if (pad->clk) {
                pad->clk = false; 

                if (cursor_pos == 0) {
                    current_screen = SCREEN_SETTINGS;
                    menu_options = 4;
                } 

                cursor_pos = 0;
                encoder_mv = 0;

                need_redraw = true;
            }  
            break;

        default:
            break;
    }

    if (encoder_mv >= 3) {
        if (cursor_pos == menu_options) {
            cursor_pos = 0;
            encoder_mv = 0;

            need_redraw = true;
        } else {
            cursor_pos += 1;
            encoder_mv = 0;

            need_redraw = true;
        }
    } else if (encoder_mv <= -3) {
        if (cursor_pos == 0) {
            cursor_pos = menu_options;
            encoder_mv = 0;

            need_redraw = true;
        } else {
            cursor_pos -= 1;
            encoder_mv = 0;

            need_redraw = true;
        }
    }
}


void ui_update(u8g2_t *disp) {
    uipad();

    if (!need_redraw) {
        return;
    }
    u8g2_ClearBuffer(disp);

    
    switch (current_screen)
    {
        case SCREEN_MAIN:
            draw_main(disp);
            break;
        case SCREEN_SETTINGS:
            draw_settings(disp);
            break;
        case SCREEN_APPSHUB:
            draw_main(disp);
            break;
        case SCREEN_APIREF:
            draw_main(disp);
            break;
        case SCREEN_ORBIT:
            draw_main(disp);
            break;
    }
    
    u8g2_SendBuffer(disp);

    need_redraw = false;
}