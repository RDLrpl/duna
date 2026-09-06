#ifndef HANDLERS_H
#define HANDLERS_H

#include "u8g2.h"
#include "encoder.h" 

#include <stdbool.h>
#include <stdint.h>

#define ENCODER_MIN_DURATION_US 50000

typedef struct {
    // encoder
    int encoder_pos;
    int encoder_change;
    
    int64_t clk_us;
    int64_t hold_clk_duration;

    bool clk;
    bool long_clk;
    
} PadHandler;

void encoder_event_handler(const rotary_encoder_event_t *event, void *ctx);

PadHandler* api_get_padhandler(void);

#endif