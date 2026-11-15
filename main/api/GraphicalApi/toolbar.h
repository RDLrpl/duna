#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <stdbool.h>

void toolbar_init(void);
bool toolbar_update(const char * time_str, const char * charge_str);

void toolbar_set_visible(bool visible);

#endif