#include "toolbar.h"
#include "config.h"
#include "esp_lvgl_port.h"
#include "misc/lv_text_private.h"
#include "lvgl.h"

static lv_obj_t * toolbar_canvas = NULL;
static uint16_t canvas_buffer[DISPLAY_H * TOOLBAR_H_PX * 2];

void toolbar_init(void)
{
    if (!lvgl_port_lock(0)) return;

    lv_obj_t * top = lv_layer_top();

    toolbar_canvas = lv_canvas_create(top);
    lv_canvas_set_buffer(toolbar_canvas, canvas_buffer, DISPLAY_H, TOOLBAR_H_PX, LV_COLOR_FORMAT_RGB565);
    lv_obj_set_pos(toolbar_canvas, 0, 0);

    lv_obj_remove_flag(toolbar_canvas, LV_OBJ_FLAG_CLICKABLE);

    lvgl_port_unlock();
}

void toolbar_update(const char * time_str, const char * charge_str)
{
    if (!toolbar_canvas) return;
    if (!lvgl_port_lock(0)) return;

    lv_canvas_fill_bg(toolbar_canvas, OFFPIXEL, LV_OPA_COVER);

    lv_layer_t layer;
    lv_canvas_init_layer(toolbar_canvas, &layer);

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = ONPIXEL;
    line_dsc.width = 1;
    line_dsc.p1 = (lv_point_precise_t){0, 9};
    line_dsc.p2 = (lv_point_precise_t){DISPLAY_H, 9};
    lv_draw_line(&layer, &line_dsc);

    lv_draw_label_dsc_t name;
    lv_draw_label_dsc_init(&name);
    name.text = "DuNa";
    name.color = ONPIXEL;
    name.font = &lv_font_unscii_8;
    lv_area_t coords_name = { .x1 = 5, .y1 = 0, .x2 = DISPLAY_H - 5, .y2 = 9 };

    lv_draw_label_dsc_t time;
    lv_draw_label_dsc_init(&time);
    time.text = time_str;
    time.color = ONPIXEL;
    time.font = &lv_font_unscii_8;
    lv_area_t coords_time = { .x1 = DISPLAY_H - 84, .y1 = 0, .x2 = DISPLAY_H - 5, .y2 = 9 };

    lv_draw_label_dsc_t charge;
    lv_draw_label_dsc_init(&charge);
    charge.text = charge_str;
    charge.color = ONPIXEL;
    charge.font = &lv_font_unscii_8;
    lv_area_t coords_charge = { .x1 = DISPLAY_H - 39, .y1 = 0, .x2 = DISPLAY_H - 5, .y2 = 9 };

    lv_draw_label(&layer, &name, &coords_name);
    lv_draw_label(&layer, &time, &coords_time);
    lv_draw_label(&layer, &charge, &coords_charge);

    lv_canvas_finish_layer(toolbar_canvas, &layer);

    lvgl_port_unlock();
}

void toolbar_set_visible(bool visible)
{
    if (!toolbar_canvas) return;
    if (!lvgl_port_lock(0)) return;

    if (visible) lv_obj_remove_flag(toolbar_canvas, LV_OBJ_FLAG_HIDDEN);
    else         lv_obj_add_flag(toolbar_canvas, LV_OBJ_FLAG_HIDDEN);

    lvgl_port_unlock();
}