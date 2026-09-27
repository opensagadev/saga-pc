// See LICENSE (GNU GPL version 3).
// Adapted from opensagadev/saga: src/legoapi/core/input/qrand.cpp.
#include "saga.h"

i32 qseed = 0x3039;

i32 qrand(void) {
    qseed = qseed * 0x24cd + 1 & 0xffff;

    return qseed;
}
