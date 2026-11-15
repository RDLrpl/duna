#include "timesettings.h"

int8_t utcf = 0;

int8_t gutc(int8_t utcfstep) {
    utcf += utcfstep;
    
    if (utcf >= 14) {
        utcf = 14;
    }
    if (utcf <= -12) {
        utcf = -12;
    }

    return utcf;
}