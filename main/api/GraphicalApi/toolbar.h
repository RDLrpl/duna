#ifndef TOOLBAR_H
#define TOOLBAR_H

void toolbar_init(void);
void toolbar_update(const char * time_str, const char * charge_str);
void toolbar_set_visible(bool visible);

#endif