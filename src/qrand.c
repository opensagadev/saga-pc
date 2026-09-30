#include "saga.h"

i32 qseed = 0x3039;

i32 qrand(void) {
    qseed = qseed * 0x24cd + 1 & 0xffff;

    return qseed;
}

i32 getqseed(void) {
    return qseed;
}
