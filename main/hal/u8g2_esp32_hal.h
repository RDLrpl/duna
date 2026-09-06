#pragma once

#include <driver/gpio.h>
#include <driver/spi_master.h>
#include "u8g2.h"

#define U8G2_ESP32_HAL_UNDEFINED (-1)

typedef struct {
    gpio_num_t clk;
    gpio_num_t mosi;
    gpio_num_t cs;
    gpio_num_t reset;
    gpio_num_t dc;
} u8g2_esp32_hal_t;

#define U8G2_ESP32_HAL_DEFAULT { \
    .clk = U8G2_ESP32_HAL_UNDEFINED, .mosi = U8G2_ESP32_HAL_UNDEFINED, \
    .cs = U8G2_ESP32_HAL_UNDEFINED, .reset = U8G2_ESP32_HAL_UNDEFINED, \
    .dc = U8G2_ESP32_HAL_UNDEFINED }

void u8g2_esp32_hal_init(u8g2_esp32_hal_t u8g2_esp32_hal_param);

uint8_t u8g2_esp32_spi_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);