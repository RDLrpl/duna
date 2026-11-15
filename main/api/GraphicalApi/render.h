#ifndef RENDER_H
#define RENDER_H
#include <stdint.h>

typedef enum {
    DUNA_MANUAL,
    DUNA_RELAXED,
    DUNA_DYNAMIC,
} render_mode_t;

render_mode_t render_get_mode(void);
void          render_set_mode(render_mode_t mode);
uint32_t      get_render_interval(void);
void          auto_render_update(void);

#endif