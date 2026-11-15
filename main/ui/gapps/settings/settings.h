#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>
#include <stdint.h>

void settings_init(void);
void settings_refresh(void);
void settings_click(uint16_t sel);
bool settings_back(void);

#endif