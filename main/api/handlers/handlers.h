#ifndef HANDLERS_H
#define HANDLERS_H

#include "encoder.h"

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PAD_NONE = 0,
    PAD_CLICK,
    PAD_LONG
} pad_event_t;

void encoder_event_handler(const rotary_encoder_event_t *event, void *ctx);
void encoder_init(void);

int32_t     pad_take_encoder_change(void);
pad_event_t pad_take_event(void);

#endif