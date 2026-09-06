#include "Transition.h"

void draw_welcome(u8g2_t *disp) {
    u8g2_ClearBuffer(disp);
    u8g2_SetFont(disp, u8g2_font_helvB12_tr);
    u8g2_DrawStr(disp, 15, 30, "Duna");
    u8g2_DrawStr(disp, 30, 50, "Device");
    
    u8g2_SendBuffer(disp);
}
