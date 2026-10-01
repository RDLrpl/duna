#ifndef FS_H
#define FS_H

#include <stdbool.h>

bool* is_mounted(void);

void init_sd(void);
void mount_sd(void);

#endif