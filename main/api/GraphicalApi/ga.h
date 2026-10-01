#ifndef GA_H
#define GA_H

#include "lvgl.h"

extern lv_disp_t * disp;

void init_ga(void);
void clean_screen(lv_disp_t *disp);

#endif