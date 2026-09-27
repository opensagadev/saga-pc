#include "saga.h"

void NuPartEnableRayCasts(i32 enabled) {
    part_raycasts_enabled = enabled;
}

void NuPartSetSeed(i32 seed) {
    partseed = (u32)seed;
}

u32 NuPartGetSeed(void) {
    return partseed;
}
