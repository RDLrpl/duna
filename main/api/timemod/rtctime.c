#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

#include "rtctime.h"
#include "config.h"
#include "ds1302.h"
#include "esp_log.h"

static ds1302_t rtc_dev = {
    .ce_pin   = TIME_RST, 
    .io_pin   = TIME_DAT,
    .sclk_pin = TIME_CLK
};

static char buffer[128];

void init_timemod(void) {
    ds1302_init(&rtc_dev);
  
    ds1302_start(&rtc_dev, true);
}

char* get_time(bool extended_format) {
    struct tm rtc_time;

    if (ds1302_get_time(&rtc_dev, &rtc_time) != ESP_OK) {
        return NULL;
    }

    if (!extended_format) {
        snprintf(buffer, 128, "%02d:%02d",
                 rtc_time.tm_hour, rtc_time.tm_min);
    } else {
        snprintf(buffer, 128,
                 "YEAR| %04d\nMON | %02d\nDAY | %02d\nHH  | %02d\nMM  | %02d\nSS  | %02d",
                 rtc_time.tm_year + 1900,
                 rtc_time.tm_mon + 1,
                 rtc_time.tm_mday,
                 rtc_time.tm_hour,
                 rtc_time.tm_min,
                 rtc_time.tm_sec);
    }

    return buffer;
}

void set_time(void) {

}
