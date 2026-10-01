#include "driver.h"
#include <stdio.h>
#include "ga.h"
#include "handlers/handlers.h"
#include "gapps/util.h"
#include "gapps/gapps.h"
#include <config.h>

static int32_t pending_steps = 0;

void uipad(void) {
    switch (pad_take_event()) {
        case PAD_LONG:  gapp_click_logic(true);  break;
        case PAD_CLICK: gapp_click_logic(false); break;
        default: break;
    }

    pending_steps += pad_take_encoder_change();
    if (pending_steps == 0) return;

    if (menu_scroll_by(groller, pending_steps)) {
        pending_steps = 0;
    }
}

void dunaDriver(void) {
    uipad();
}