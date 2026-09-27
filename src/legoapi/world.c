#include "saga.h"
#include <string.h>

void WorldInfo_InitOnce(void) {
    memset(WorldInfo, 0, sizeof(WorldInfo));
}

WORLDINFO *WorldInfo_CurrentlyActive(void) {
    return WORLD;
}

WORLDINFO *WorldInfo_CurrentlyLoading(void) {
    return LWORLD;
}

i32 __cdecl WorldInfo_OtherLevel(WORLDINFO *world) {
    WORLDINFO *other = &WorldInfo[0];
    if (world == &WorldInfo[0]) {
        other = &WorldInfo[1];
    }
    if (other->loaded != 0) {
        return other->level_idx;
    }
    return -1;
}
