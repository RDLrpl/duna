#include "handlers.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static PadHandler duna_pad_handler = {
    .encoder_pos = 0,
    .encoder_change = 0,
    .clk_us = 0,
    .long_clk = false,
    .clk = false
};

PadHandler* api_get_padhandler(void) {
    return &duna_pad_handler;
}

void encoder_event_handler(const rotary_encoder_event_t *event, void *ctx)
{
    switch (event->type) {

        case RE_ET_CHANGED: {
            int diff = event->diff;

            duna_pad_handler.encoder_change = diff;
            duna_pad_handler.encoder_pos += diff;
            break;
        }

        case RE_ET_BTN_PRESSED:
            duna_pad_handler.clk_us = esp_timer_get_time();
            duna_pad_handler.long_clk = false;
            duna_pad_handler.clk = false;
            break;

        case RE_ET_BTN_RELEASED:
            if (duna_pad_handler.clk_us > 0) {
                duna_pad_handler.hold_clk_duration = esp_timer_get_time() - duna_pad_handler.clk_us;
                if (duna_pad_handler.hold_clk_duration >= ENCODER_MIN_DURATION_US) {
                    duna_pad_handler.clk = true;
                }
            }

            if (duna_pad_handler.hold_clk_duration >= 800000) {
                duna_pad_handler.long_clk = true;
                duna_pad_handler.clk = false;
            } 

            duna_pad_handler.clk_us = 0;
            break;

        default:
            break;
    }
}