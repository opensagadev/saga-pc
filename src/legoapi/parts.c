#include "saga.h"

void NuPartSetSeed(i32 seed) {
    partseed = (u32)seed;
}

u32 NuPartGetSeed(void) {
    return partseed;
}
