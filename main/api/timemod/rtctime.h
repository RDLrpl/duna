#ifndef RTCTIME_H
#define RTCTIME_H

#include <stdbool.h>
#include <sys/time.h>

void init_timemod(void);
char* get_time(bool extended_format);

#endif
