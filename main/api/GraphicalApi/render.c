#include "render.h"
#include "config.h"
#include "esp_lvgl_port.h"
#include "gapps/control.h"
#include "gapps/settings/settings.h"

static render_mode_t cur_mode = DUNA_MANUAL;

render_mode_t render_get_mode(void) { return cur_mode; }

void render_set_mode(render_mode_t mode)
{
    cur_mode = mode;
}

uint32_t get_render_interval(void)
{
    switch (cur_mode) {
    case DUNA_RELAXED: return RELAXED_MODE_MS;
    case DUNA_DYNAMIC: return DYNAMIC_MODE_MS;
    default:           return UINT32_MAX;
    }
}

void auto_render_update(void)
{
    if (cur_mode == DUNA_MANUAL) return;
    if (!lvgl_port_lock(0)) return;

    switch (curapp) {
    case GAPP_SCREEN_SETTINGS:
        settings_refresh();
        break;
    default:
        break;
    }

    lvgl_port_unlock();
}