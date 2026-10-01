#include "gapps.h"
#include "ga.h"
#include "util.h"
#include <config.h>
#include "esp_lvgl_port.h"
#include "misc/lv_text_private.h"
#include "GraphicalApi/toolbar.h"
#include "lvgl.h"

gapp_screen_t curapp = GAPP_SCREEN_MAIN;

lv_obj_t *curscreen = NULL;
lv_obj_t *groller = NULL; 

void gapp_click_logic(bool is_long) {
    uint16_t sel = menu_get_selected(groller);

    switch (curapp) {
        case GAPP_SCREEN_MAIN:
            if (is_long) {
                // toolbar_set_visible(false);

                return;
            }
            switch (sel) {
                case 0:
                    curapp = GAPP_SCREEN_SETTINGS;

                    set_gapp_screen(curapp);
                    break;
                case 1:
                    curapp = GAPP_SCREEN_MARKET;

                    set_gapp_screen(curapp);
                    break;
                case 2:
                    curapp = GAPP_SCREEN_FILEMANAGER;

                    set_gapp_screen(curapp);
                    break;
                default:
                    break;
            }
            break;
        case GAPP_SCREEN_SETTINGS:
            if (is_long) {
                curapp = GAPP_SCREEN_MAIN;

                set_gapp_screen(curapp);
                return;
            }

            break;
        case GAPP_SCREEN_MARKET:      break;
        case GAPP_SCREEN_FILEMANAGER: break;
        case SCREEN_APIREF:           break;
    }
}

void set_gapp_screen(gapp_screen_t target_screen)
{
    if (!lvgl_port_lock(0)) return;

    lv_obj_t *old = curscreen;

    switch (target_screen) {
        case GAPP_SCREEN_MAIN:     init_main_GAPP();     break;
        case GAPP_SCREEN_SETTINGS: init_settings_GAPP(); break;
        default:
            lvgl_port_unlock();
            return;
    }

    curapp = target_screen;

    lv_screen_load_anim(curscreen, LV_SCREEN_LOAD_ANIM_NONE, 0, 0, old != NULL);

    lvgl_port_unlock();
}

void init_main_GAPP(void)
{
    if (!lvgl_port_lock(0)) return;

    curscreen = lv_obj_create(NULL);

    groller = lv_roller_create(curscreen);
    
    lv_roller_set_options(groller,
                          "Settings\n"
                          "Market\n"
                          "FileManager\n"
                          "DunaDvG1-100",
                          LV_ROLLER_MODE_INFINITE);

    lv_obj_set_style_border_width(groller, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(groller, 0, LV_PART_MAIN);

    lv_obj_set_style_text_color(groller, ONPIXEL, LV_PART_MAIN);

    lv_obj_set_style_bg_color(groller, OFFPIXEL, LV_PART_SELECTED);
    lv_obj_set_style_text_color(groller, ONPIXEL, LV_PART_SELECTED);

    lv_obj_set_style_text_font(groller, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_font(groller, &lv_font_unscii_8, LV_PART_SELECTED); 
    
    lv_obj_set_style_text_line_space(groller, 3, LV_PART_MAIN);

    lv_obj_set_style_border_width(groller, 1, LV_PART_SELECTED);
    lv_obj_set_style_border_side(groller, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT, LV_PART_SELECTED);
    lv_obj_set_style_border_color(groller, ONPIXEL, LV_PART_SELECTED);

    lv_roller_set_visible_row_count(groller, 3);

    lv_obj_set_size(groller, lv_pct(100), lv_disp_get_ver_res(NULL));
    lv_obj_align(groller, LV_ALIGN_TOP_MID, 0, 0);

    lv_screen_load(curscreen);

    lvgl_port_unlock();
}

void init_settings_GAPP(void)
{
    if (!lvgl_port_lock(0)) return;

    curscreen = lv_obj_create(NULL);

    groller = lv_roller_create(curscreen);
    
    lv_roller_set_options(groller,
                          "TODO\n"
                          "_____",
                          LV_ROLLER_MODE_INFINITE);

    lv_obj_set_style_border_width(groller, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(groller, 0, LV_PART_MAIN);

    lv_obj_set_style_text_color(groller, ONPIXEL, LV_PART_MAIN);

    lv_obj_set_style_bg_color(groller, OFFPIXEL, LV_PART_SELECTED);
    lv_obj_set_style_text_color(groller, ONPIXEL, LV_PART_SELECTED);

    lv_obj_set_style_text_font(groller, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_font(groller, &lv_font_unscii_8, LV_PART_SELECTED); 
    
    lv_obj_set_style_text_line_space(groller, 3, LV_PART_MAIN);

    lv_obj_set_style_border_width(groller, 1, LV_PART_SELECTED);
    lv_obj_set_style_border_side(groller, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT, LV_PART_SELECTED);
    lv_obj_set_style_border_color(groller, ONPIXEL, LV_PART_SELECTED);

    lv_roller_set_visible_row_count(groller, 3);

    lv_obj_set_size(groller, lv_pct(100), lv_disp_get_ver_res(NULL));
    lv_obj_align(groller, LV_ALIGN_TOP_MID, 0, 0);

    lv_screen_load(curscreen);

    lvgl_port_unlock();
}

/*
void init_market_GAPP(void)
{
    curscreen = lv_obj_create(NULL);

    groller = lv_roller_create(curscreen);
}

void init_filemanager_GAPP(void)
{
    curscreen = lv_obj_create(NULL);

    groller = lv_roller_create(curscreen);
}

void init_apiref(void)
{
    curscreen = lv_obj_create(NULL);

    groller = lv_roller_create(curscreen);
}
*/