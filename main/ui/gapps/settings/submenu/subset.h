#ifndef SUBSET_H
#define SUBSET_H

typedef enum {
    SUB_NONE,
    WIFISETTINGS,
    BLESETTINGS,
    TIMESETTINGS,
} sub_settings_t;

extern sub_settings_t subsel;

#endif