#ifndef CONFIG_H
#define CONFIG_H
#include "lvgl.h"

// ENCODER
#define PIN_ENC_A  19
#define PIN_ENC_B  18
#define PIN_ENC_KEY  23

#define ENCODER_MIN_DURATION_US 10
#define LONG_PRESS_US           210000

// Miso, CLK, Mosi
#define PIN_MISO  12
#define PIN_MOSI  14
#define PIN_CLK  13

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

#endif