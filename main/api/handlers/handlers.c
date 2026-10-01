#include "handlers.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#include "config.h"
#include "esp_log.h"
#include "encoder.h"

static portMUX_TYPE enc_mux = portMUX_INITIALIZER_UNLOCKED;

typedef struct {
    int encoder_pos;
    int encoder_change;

    int64_t clk_us;
    int64_t hold_clk_duration;

    bool clk;
    bool long_clk;

} PadHandler;

static PadHandler duna_pad_handler = {
    .encoder_pos = 0,
    .encoder_change = 0,
    .clk_us = 0,
    .hold_clk_duration = 0,
    .long_clk = false,
    .clk = false
};

PadHandler* api_get_padhandler(void) {
    return &duna_pad_handler;
}

int32_t pad_take_encoder_change(void) {
    int32_t v;
    portENTER_CRITICAL(&enc_mux);
    v = duna_pad_handler.encoder_change;
    duna_pad_handler.encoder_change = 0;
    portEXIT_CRITICAL(&enc_mux);
    return v;
}

pad_event_t pad_take_event(void) {
    pad_event_t e = PAD_NONE;
    portENTER_CRITICAL(&enc_mux);
    if (duna_pad_handler.long_clk)      e = PAD_LONG;
    else if (duna_pad_handler.clk)      e = PAD_CLICK;
    duna_pad_handler.long_clk = false;
    duna_pad_handler.clk = false;
    portEXIT_CRITICAL(&enc_mux);
    return e;
}

static rotary_encoder_handle_t encoder_handle = NULL;

void encoder_init(void) {
    rotary_encoder_config_t config = ROTARY_ENCODER_DEFAULT_CONFIG();

    config.pin_a = (gpio_num_t)PIN_ENC_A;
    config.pin_b = (gpio_num_t)PIN_ENC_B;
    config.pin_btn = (gpio_num_t)PIN_ENC_KEY;
    config.btn_pressed_level = 0;
    config.enable_internal_pullup = true;
    config.callback = encoder_event_handler;
    config.callback_ctx = NULL;

    esp_err_t err = rotary_encoder_create(&config, &encoder_handle);
    if (err != ESP_OK) {
        ESP_LOGE("pad", "rotary_encoder_create failed: %d", err);
        return;
    }
}

void encoder_event_handler(const rotary_encoder_event_t *event, void *ctx)
{
    switch (event->type) {

        case RE_ET_CHANGED: {
            int diff = event->diff;

            portENTER_CRITICAL_SAFE(&enc_mux);
            duna_pad_handler.encoder_change += diff;
            duna_pad_handler.encoder_pos += diff;
            portEXIT_CRITICAL_SAFE(&enc_mux);
            break;
        }

        case RE_ET_BTN_PRESSED: {
            int64_t t = esp_timer_get_time();

            portENTER_CRITICAL_SAFE(&enc_mux);
            duna_pad_handler.clk_us = t;
            portEXIT_CRITICAL_SAFE(&enc_mux);
            break;
        }

        case RE_ET_BTN_RELEASED: {
            int64_t t = esp_timer_get_time();

            portENTER_CRITICAL_SAFE(&enc_mux);
            if (duna_pad_handler.clk_us > 0) {
                int64_t dur = t - duna_pad_handler.clk_us;
                duna_pad_handler.hold_clk_duration = dur;

                if (dur >= LONG_PRESS_US) {
                    duna_pad_handler.long_clk = true;
                } else if (dur >= ENCODER_MIN_DURATION_US) {
                    duna_pad_handler.clk = true;
                }

                duna_pad_handler.clk_us = 0;
            }
            portEXIT_CRITICAL_SAFE(&enc_mux);
            break;
        }

        case RE_ET_BTN_LONG_PRESSED:
        case RE_ET_BTN_CLICKED:
        default:
            break;
    }
}