#include "saga.h"

NUANG NuAngAdd(NUANG a, NUANG b) {
    NUANG sum = (a + b) & 0xffff;
    // Retail VC8 uses cmp 0x8000; jl for this normalization.
    if (sum >= 0x8000) {
        sum -= 0x10000;
    }
    return sum;
}

NUANG NuAngSub(NUANG a, NUANG b) {
    NUANG difference = (a - b) & 0xffff;
    if (difference >= 0x8000) {
        difference -= 0x10000;
    }
    return difference;
}

void NuAng2AltSol(NUANG *out_x, NUANG *out_y, NUANG *out_z, NUANG x, NUANG y, NUANG z) {
    *out_x = x + 0x8000;
    *out_y = -0x8000 - y;
    *out_z = z + 0x8000;
}
