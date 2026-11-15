#ifndef CONFIG_H
#define CONFIG_H
#include "lvgl.h"
 
// PAD  
#define PIN_JY_XPOS  ADC_CHANNEL_0 // 32
#define PIN_JY_YPOS  ADC_CHANNEL_3 // 33
#define PIN_JY_KEY  27

#define menu_scroll_delay 200

// Miso, CLK, Mosi
#define PIN_MISO  12 
#define PIN_MOSI  14
#define PIN_CLK  13

#define DEBOUNCE_MS     30
#define LONG_PRESS_MS   800

// TIME
#define TIME_CLK 25
#define TIME_DAT 26
#define TIME_RST 18

// SD
#define PIN_SD_CS 24

// Display
#define DISPLAY_H              128
#define DISPLAY_V              64

// I2C
#define I2C_SDA_PIN          21
#define I2C_SCL_PIN          22
#define I2C_DISPLAY        0x3C

// HELPERS
#define ONPIXEL lv_color_black()
#define OFFPIXEL lv_color_white()

// DATA
#define TOOLBAR_H_PX 13
#define MOUNT_POINT "/SD"
#define DUOS_VERSION "001o"

// WIFI
#define MAX_SCAN_AP  10
#define MAX_RETRY    5

// LOW-LVL Configuration
#define PUB_BUF_SIZE    256
#define RELAXED_MODE_MS 420
#define DYNAMIC_MODE_MS 33

#define FILTER_SAMPLES  32
 
#define JY_DEADZONE_PCT         10
#define JY_OUTER_ZONE_PCT       15
#define JY_INIT_RANGE_NEG_MV    1400
#define JY_INIT_RANGE_POS_MV    1100
#define JY_MIN_VALID_SPAN_MV    200
#define JY_CENTER_DEFAULT_MV    1650
#define JY_CENTER_MAX_ERR_MV    500
#define JY_CENTER_SAMPLES       32
#define JY_CENTER_DELAY_MS      5
#define JY_ADC_VREF_MV          3300
#define JY_ADC_MAX_RAW          4095

#endif