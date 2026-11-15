#ifndef CONTROL_H
#define CONTROL_H
#include <stdbool.h>
#include <stdint.h>
#include "lvgl.h"

typedef enum {
    GAPP_SCREEN_MAIN,
    GAPP_SCREEN_SETTINGS,
    GAPP_SCREEN_MARKET,
    GAPP_SCREEN_FILEMANAGER,
    GAPP_SCREEN_APIREF
} gapp_screen_t;

extern gapp_screen_t curapp;
extern lv_obj_t *curscreen;
extern lv_obj_t *groller;

lv_obj_t *get_groller(void);
uint16_t  menu_get_selected(lv_obj_t *roller);

void roller_create(const char *opts);
void roller_set_text(const char *opts);

void set_gapp_screen(gapp_screen_t target);
bool menu_click_logic(lv_obj_t *roller, bool islong);

bool standart_duna_menulogic(int16_t deltax, int16_t deltay, lv_obj_t *roller);

#endif