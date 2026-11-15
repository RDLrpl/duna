#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int16_t deltax;
    int16_t deltay;
    bool clk;
    bool long_clk;
} PadHandler;

void init_joystick(void);
void handle_joystick(void);
PadHandler get_pad_handler(void);

#endif