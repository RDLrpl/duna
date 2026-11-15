#ifndef WIFIDRIVE_H
#define WIFIDRIVE_H
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum {
    DRV_MODE_IDLE = 0,
    DRV_MODE_STATION,
    DRV_MODE_MONITOR,   // TODO
} wifimode_t;

void       wd_init(void);
bool       wd_change_mode(wifimode_t target);
wifimode_t wd_get_mode(void);

bool       wd_scan_start(void);
void       wd_get_list(char *dst, size_t n);

bool       wd_connect(const char *ssid, const char *pass);
void       wd_disconnect(void);
bool       wd_connecting(void);
bool       wd_connected(void);

bool       wd_get_ip(char *dst, size_t n);
bool       wd_get_ssid(char *dst, size_t n);

#endif