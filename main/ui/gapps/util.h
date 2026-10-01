#ifndef UTIL_H
#define UTIL_H
#include "lvgl.h"

bool menu_scroll_by(lv_obj_t *roller, int32_t steps);

uint16_t menu_get_selected(lv_obj_t *roller);

#endif