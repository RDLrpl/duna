#include "Transition.h"
#include "esp_lvgl_port.h"
#include "ga.h"

static lv_style_t style;
static bool style_initialized = false;

void draw_welcome(lv_disp_t *disp) {
    clean_screen(disp);

    if (lvgl_port_lock(0)) {
        lv_obj_t *scr = lv_disp_get_scr_act(disp);

        if (!style_initialized) {
            lv_style_init(&style);
            lv_style_set_text_font(&style, &lv_font_unscii_16);
            style_initialized = true;
        }

        lv_obj_t *d1 = lv_label_create(scr);
        lv_label_set_text(d1, "DuNA");

        lv_obj_t *d2 = lv_label_create(scr);
        lv_label_set_text(d2, "-DEV");

        lv_obj_add_style(d1, &style, 0);
        lv_obj_add_style(d2, &style, 0);

        lv_obj_align(d1, LV_ALIGN_LEFT_MID, 20, 0);
        lv_obj_align(d2, LV_ALIGN_LEFT_MID, 50, 20);

        lvgl_port_unlock();
    }
}

