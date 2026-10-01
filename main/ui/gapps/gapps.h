#ifndef GAPPS_H
#define GAPPS_H
#include "lvgl.h"

typedef enum {
    GAPP_SCREEN_MAIN,
    GAPP_SCREEN_SETTINGS,
    GAPP_SCREEN_MARKET,
    GAPP_SCREEN_FILEMANAGER,
    SCREEN_APIREF
} gapp_screen_t;

extern gapp_screen_t curapp;
extern lv_obj_t *groller;
extern lv_obj_t *curscreen;

void init_main_GAPP(void);
void init_settings_GAPP(void);

void set_gapp_screen(gapp_screen_t target_screen);

void gapp_click_logic(bool is_long);

#endif
