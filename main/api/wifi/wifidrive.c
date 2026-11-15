#include "wifidrive.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "config.h"

static char                wifilist[1024] = "";
static char                ip_str[16]     = "";
static volatile wifimode_t curent_mode    = DRV_MODE_IDLE;
static volatile bool       connected      = false;
static volatile bool       want_connect   = false;
static volatile int        retry_cnt      = 0;
static bool                inited         = false;

static SemaphoreHandle_t list_mutex    = NULL;
static SemaphoreHandle_t mode_mutex    = NULL;
static SemaphoreHandle_t scan_done_sem = NULL;

char *rssibar(int8_t rssi)
{
    if (rssi >= -55) return "####";
    if (rssi >= -65) return "### ";
    if (rssi >= -70) return "##  ";
    if (rssi >= -80) return "#   ";
    if (rssi >= -95) return ".   ";
    return "";
}

static void on_scan_done(void)
{
    wifi_ap_record_t ap[MAX_SCAN_AP];
    uint16_t num = MAX_SCAN_AP;
    memset(ap, 0, sizeof(ap));

    if (esp_wifi_scan_get_ap_records(&num, ap) == ESP_OK) {
        xSemaphoreTake(list_mutex, portMAX_DELAY);
        wifilist[0] = '\0';
        size_t len = 0;
        for (int i = 0; i < num; i++) {
            char *lock = (ap[i].authmode == WIFI_AUTH_OPEN) ? "0" : "*";

            int n = snprintf(wifilist + len, sizeof(wifilist) - len, "%.8s |%s %s\n",
                             (char *)ap[i].ssid, lock, rssibar(ap[i].rssi));
            if (n < 0 || (size_t)n >= sizeof(wifilist) - len) break;
            len += n;
        }
        xSemaphoreGive(list_mutex);
    }
    xSemaphoreGive(scan_done_sem);
}

static void on_disconnected(void)
{
    connected = false;
    ip_str[0] = '\0';

    if (!want_connect) return;

    if (retry_cnt < MAX_RETRY) {
        retry_cnt++;
        esp_wifi_connect();
    } else {
        want_connect = false;
    }
}

static void on_got_ip(ip_event_got_ip_t *e)
{
    snprintf(ip_str, sizeof(ip_str), IPSTR, IP2STR(&e->ip_info.ip));
    retry_cnt = 0;
    connected = true;
}

static void event_handler(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    if (base == WIFI_EVENT) {
        if (id == WIFI_EVENT_SCAN_DONE)             on_scan_done();
        else if (id == WIFI_EVENT_STA_DISCONNECTED) on_disconnected();
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        on_got_ip((ip_event_got_ip_t *)data);
    }
}

void wd_init(void)
{
    if (inited) return;

    list_mutex    = xSemaphoreCreateMutex();
    mode_mutex    = xSemaphoreCreateMutex();
    scan_done_sem = xSemaphoreCreateBinary();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    inited = true;
}

wifimode_t wd_get_mode(void) { return curent_mode; }

static void drop_connection(void)
{
    want_connect = false;
    connected    = false;
    ip_str[0]    = '\0';
    esp_wifi_disconnect();
}

bool wd_change_mode(wifimode_t target)
{
    if (!inited) return false;
    if (target != DRV_MODE_IDLE && target != DRV_MODE_STATION) return false;

    bool ok = true;
    xSemaphoreTake(mode_mutex, portMAX_DELAY);

    if (target != curent_mode) {
        if (target == DRV_MODE_STATION) {
            if (esp_wifi_set_mode(WIFI_MODE_STA) == ESP_OK && esp_wifi_start() == ESP_OK) {
                curent_mode = DRV_MODE_STATION;
            } else {
                esp_wifi_stop();
                ok = false;
            }
        } else {
            drop_connection();
            esp_wifi_stop();
            curent_mode = DRV_MODE_IDLE;
        }
    }

    xSemaphoreGive(mode_mutex);
    return ok;
}

bool wd_scan_start(void)
{
    if (curent_mode != DRV_MODE_STATION) return false;

    wifi_scan_config_t cfg = {
        .ssid        = NULL,
        .bssid       = NULL,
        .channel     = 0,
        .show_hidden = false,
        .scan_type   = WIFI_SCAN_TYPE_ACTIVE,
    };

    xSemaphoreTake(scan_done_sem, 0);
    return esp_wifi_scan_start(&cfg, false) == ESP_OK;
}

void wd_get_list(char *dst, size_t n)
{
    xSemaphoreTake(list_mutex, portMAX_DELAY);
    strlcpy(dst, wifilist, n);
    xSemaphoreGive(list_mutex);
}

bool wd_connect(const char *ssid, const char *pass)
{
    if (!ssid || !ssid[0]) return false;
    if (!wd_change_mode(DRV_MODE_STATION)) return false;

    drop_connection();

    wifi_config_t wc = {0};
    strlcpy((char *)wc.sta.ssid, ssid, sizeof(wc.sta.ssid));
    if (pass && pass[0]) {
        strlcpy((char *)wc.sta.password, pass, sizeof(wc.sta.password));
        wc.sta.threshold.authmode = WIFI_AUTH_WPA_PSK;
    } else {
        wc.sta.threshold.authmode = WIFI_AUTH_OPEN;
    }

    if (esp_wifi_set_config(WIFI_IF_STA, &wc) != ESP_OK) return false;

    retry_cnt    = 0;
    want_connect = true;
    if (esp_wifi_connect() != ESP_OK) {
        want_connect = false;
        return false;
    }
    return true;
}

void wd_disconnect(void)
{
    drop_connection();
}

bool wd_connecting(void) { return want_connect && !connected; }
bool wd_connected(void)  { return connected; }

bool wd_get_ip(char *dst, size_t n)
{
    if (!connected) return false;
    strlcpy(dst, ip_str, n);
    return true;
}

bool wd_get_ssid(char *dst, size_t n)
{
    wifi_ap_record_t ap;
    if (!connected || esp_wifi_sta_get_ap_info(&ap) != ESP_OK) return false;

    snprintf(dst, n, "%.32s", (const char *)ap.ssid);
    return true;
}