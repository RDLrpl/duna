#include "control.h"
#include "config.h"
#include "esp_lvgl_port.h"
#include "render.h"
#include "mainmenu.h"
#include "settings/settings.h"
#include "freertos/FreeRTOS.h"
#include "submenu/timesettings.h"
#include "subset.h"

lv_obj_t *curscreen = NULL;
lv_obj_t *groller   = NULL;
gapp_screen_t curapp = GAPP_SCREEN_MAIN;

lv_obj_t *get_groller(void) { return groller; }

void change_menusets(void) {
    utcf = 0;
    lv_roller_set_selected(groller, 0, LV_ANIM_OFF);
}

uint16_t menu_get_selected(lv_obj_t *roller)
{
    uint16_t sel = 0;
    if (roller && lvgl_port_lock(0)) {
        sel = lv_roller_get_selected(roller);
        lvgl_port_unlock();
    }
    return sel;
}

void roller_create(const char *opts)
{
    curscreen = lv_obj_create(NULL);
    groller   = lv_roller_create(curscreen);

    lv_roller_set_options(groller, opts, LV_ROLLER_MODE_INFINITE);

    lv_obj_set_style_border_width(groller, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(groller, 0, LV_PART_MAIN);
    lv_obj_set_style_text_color(groller, ONPIXEL, LV_PART_MAIN);
    lv_obj_set_style_text_font(groller, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_line_space(groller, 3, LV_PART_MAIN);

    lv_obj_set_style_bg_color(groller, OFFPIXEL, LV_PART_SELECTED);
    lv_obj_set_style_text_color(groller, ONPIXEL, LV_PART_SELECTED);
    lv_obj_set_style_text_font(groller, &lv_font_unscii_8, LV_PART_SELECTED);
    lv_obj_set_style_border_width(groller, 1, LV_PART_SELECTED);
    lv_obj_set_style_border_side(groller, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT, LV_PART_SELECTED);
    lv_obj_set_style_border_color(groller, ONPIXEL, LV_PART_SELECTED);

    lv_roller_set_visible_row_count(groller, 3);
    lv_obj_set_size(groller, lv_pct(100), lv_disp_get_ver_res(NULL));
    lv_obj_align(groller, LV_ALIGN_TOP_MID, 0, 0);
}

void roller_set_text(const char *opts)
{
    uint16_t sel = lv_roller_get_selected(groller);
    lv_roller_set_options(groller, opts, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_selected(groller, sel, LV_ANIM_OFF);
}

void set_gapp_screen(gapp_screen_t target)
{
    lv_obj_t *old = curscreen;

    switch (target) {
        case GAPP_SCREEN_MAIN:     mainmenu_init(); break;
        case GAPP_SCREEN_SETTINGS: settings_init(); break;
        default: return; 
    }

    curapp = target;
    render_set_mode(target == GAPP_SCREEN_SETTINGS ? DUNA_DYNAMIC : DUNA_MANUAL);

    lv_screen_load_anim(curscreen, LV_SCREEN_LOAD_ANIM_NONE, 0, 0, old != NULL);
}

bool menu_click_logic(lv_obj_t *roller, bool islong)
{
    if (!roller || !lvgl_port_lock(portMAX_DELAY)) return false;

    uint16_t sel = lv_roller_get_selected(roller);

    switch (curapp) {
    case GAPP_SCREEN_MAIN:
        if (!islong) {
            switch (sel) {
                case 0: set_gapp_screen(GAPP_SCREEN_SETTINGS);   break;
                case 1: set_gapp_screen(GAPP_SCREEN_MARKET);     break;
                case 2: set_gapp_screen(GAPP_SCREEN_FILEMANAGER); break;
                default: break;
            }
            change_menusets();
        }
        break;

    case GAPP_SCREEN_SETTINGS:
        if (islong) {
            if (!settings_back())
                set_gapp_screen(GAPP_SCREEN_MAIN);
                
            change_menusets();
        } else {
            settings_click(sel);
        }
        break;

    default:
        if (islong) {
            change_menusets();
            set_gapp_screen(GAPP_SCREEN_MAIN);
        }
        break;
    }

    lvgl_port_unlock();
    return true;
}

bool menu_scroll_by(lv_obj_t *roller, int32_t steps)
{
    if (!roller || steps == 0) return true;
    if (!lvgl_port_lock(portMAX_DELAY)) return false;

    int32_t total = lv_roller_get_option_count(roller);
    if (total > 0) {
        int32_t cur  = lv_roller_get_selected(roller);
        int32_t next = ((cur + steps) % total + total) % total;
        lv_roller_set_selected(roller, (uint16_t)next, LV_ANIM_OFF);
    }

    lvgl_port_unlock();
    return true;
}

bool standart_duna_menulogic(int16_t deltax, int16_t deltay, lv_obj_t *roller)
{
    bool yfunc_scroll = false;
    bool xfunc_scroll = false;

    if (deltay >=  90) yfunc_scroll = menu_scroll_by(roller, -1);
    if (deltay <= -90) yfunc_scroll = menu_scroll_by(roller,  1);

    switch (curapp) {
    case GAPP_SCREEN_SETTINGS:
        if (subsel == TIMESETTINGS && menu_get_selected(roller) == 0) {
            if (deltax >= 90) {
                gutc(-1);
                xfunc_scroll = true;
            }
            if (deltax <= -90) {
                gutc(1);
                xfunc_scroll = true;
            }
        }
        break;
    default:
        break;
    }

    return xfunc_scroll || yfunc_scroll;
}

bool dunajoy(int16_t deltax, int16_t deltay, lv_obj_t *roller)
{
    switch (curapp) {
    case GAPP_SCREEN_APIREF:
        // API REF
        return true;
    default:
        return standart_duna_menulogic(deltax, deltay, roller);
    }
}