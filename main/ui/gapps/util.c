#include "util.h"
#include "esp_lvgl_port.h"

bool menu_scroll_by(lv_obj_t *roller, int32_t steps) {
    if (roller == NULL || steps == 0) return true;

    if (!lvgl_port_lock(20)) {
        return false;
    }

    int32_t total = lv_roller_get_option_count(roller);
    if (total > 0) {
        int32_t current = lv_roller_get_selected(roller);
        int32_t next = ((current + steps) % total + total) % total;
        lv_roller_set_selected(roller, (uint16_t)next, LV_ANIM_OFF);
    }

    lvgl_port_unlock();
    return true;
}

uint16_t menu_get_selected(lv_obj_t *roller) {
    uint16_t sel = 0;
    if (roller && lvgl_port_lock(0)) {
        sel = lv_roller_get_selected(roller);
        lvgl_port_unlock();
    }
    return sel;
}
