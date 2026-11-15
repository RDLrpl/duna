#include "settings.h"
#include "control.h"
#include "config.h"
#include "subset.h"
#include "api/timemod/rtctime.h"
#include "esp_heap_caps.h"
#include "timesettings.h"
#include "wifidrive.h"

#define SETTINGS_BUF 640

sub_settings_t subsel = SUB_NONE;
static char     text[SETTINGS_BUF];
static uint16_t parent_sel;
static lv_timer_t *refresh_timer;

static void build(char *out, size_t n)
{
    switch (subsel) {
    case TIMESETTINGS: {
        const char *t = get_time(true);
        lv_snprintf(out, n, "Set | UTC %d\n%s", gutc(0), t ? t : "RTC ERROR");
        break;
    }
    case WIFISETTINGS: {
        static char wl[400];
        char ip[16]   = "";
        char ssid[33] = "";

        wd_get_list(wl, sizeof wl);
        if (wd_connected()) {
            wd_get_ip(ip, sizeof ip);
            wd_get_ssid(ssid, sizeof ssid);
        }

        lv_snprintf(out, n,
            "SCAN | %s\n"
            "PING | 8.8.8.8\n"
            "STAT | %s\n"
            "CURN | %.20s\n"
            "%s",
            wd_get_mode() != DRV_MODE_STATION ? "off" :
                wd_connecting()               ? "connecting..." : "WiFi",
            wd_connected() ? ip : "no net",
            ssid,
            wl
        );
        break;
    }
    case BLESETTINGS:
        lv_snprintf(out, n, "TODO");
        break;

    default: {
        size_t   total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
        size_t   free_ = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
        uint32_t up_s  = lv_tick_get() / 1000;

        lv_snprintf(out, n,
            "RAM  | %u/%uKB\n"
            "WLAN | set\n"
            "BLE  | set\n"
            "TIME | set\n"
            "UP   | %02u:%02u:%02u\n"
            "DuOS | " DUOS_VERSION,
            (unsigned)((total - free_) / 1024), (unsigned)(total / 1024),
            (unsigned)(up_s / 3600), (unsigned)((up_s / 60) % 60), (unsigned)(up_s % 60));
        break;
    }
    }
}

static void refresh_cb(lv_timer_t *t)
{
    if (curapp == GAPP_SCREEN_SETTINGS) settings_refresh();
}

void settings_init(void)
{
    subsel = SUB_NONE;
    build(text, sizeof text);
    roller_create(text);
    if (!refresh_timer) refresh_timer = lv_timer_create(refresh_cb, 1000, NULL);
}

void settings_refresh(void)
{
    build(text, sizeof text);
    roller_set_text(text);
}

void settings_click(uint16_t sel)
{
    if (subsel == WIFISETTINGS) {
        if (sel == 0) wd_scan_start();
        return;
    }

    if (subsel != SUB_NONE) return;

    switch (sel) {
        case 1:
            subsel = WIFISETTINGS;
            wd_change_mode(DRV_MODE_STATION);
            break;
        case 2: subsel = BLESETTINGS;  break;
        case 3: subsel = TIMESETTINGS; break;
        default: return;
    }

    parent_sel = sel;
    settings_refresh();
    lv_roller_set_selected(groller, 0, LV_ANIM_OFF);
}

bool settings_back(void)
{
    if (subsel == SUB_NONE) return false;

    if (subsel == WIFISETTINGS) wd_change_mode(DRV_MODE_IDLE);

    subsel = SUB_NONE;
    settings_refresh();
    lv_roller_set_selected(groller, parent_sel, LV_ANIM_OFF);
    return true;
}