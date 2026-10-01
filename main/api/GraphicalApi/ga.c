#include "config.h"
#include "esp_lcd_panel_ssd1306.h"
#include "driver/i2c_master.h"
#include "esp_lcd_panel_io.h"
#include <esp_lvgl_port.h>
#include "ga.h"

lv_disp_t *disp;

void init_ga(void) {
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = I2C_SCL_PIN,
        .sda_io_num = I2C_SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));
    esp_lcd_panel_io_handle_t io_handle = NULL;

    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = I2C_DISPLAY,
        .scl_speed_hz = 400 * 1000,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_bit_offset = 6,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(bus_handle, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = -1,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, false));

    lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_cfg.task_affinity = 1;
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = DISPLAY_H * DISPLAY_V,
        .double_buffer = false,
        .hres = DISPLAY_H,
        .vres = DISPLAY_V,
        .monochrome = true,
        .color_format = LV_COLOR_FORMAT_I1,
        .flags = {
            .swap_bytes = false,
            .sw_rotate = false,
            .full_refresh = false,
        }
    };

    disp = lvgl_port_add_disp(&disp_cfg);

    if (lvgl_port_lock(0)) {
        lv_timer_t *refr_timer = lv_display_get_refr_timer(disp);
        lv_timer_pause(refr_timer);

        lv_theme_t *th = lv_theme_mono_init(disp, false, LV_FONT_DEFAULT);
        lv_disp_set_theme(disp, th);

        lv_timer_resume(refr_timer);
        lvgl_port_unlock();
    }
}

void clean_screen(lv_disp_t *disp) {
    if (!lvgl_port_lock(0)) return;

    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    lv_obj_clean(scr);
    lvgl_port_unlock();
}