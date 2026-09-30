#include "saga.h"

extern i32 ONEPLAYERPOWERUPS;
static f32 Cheat_PowerUpTime;

i32 __cdecl Cheat_IsOn(i32 cheat) {
    CHEAT_PC *entry;
    if (cheat < 0 || cheat >= CheatSystemPtr->count) {
        return 0;
    }
    entry = &CheatSystemPtr->cheats[cheat];
    if (entry->enabled != 0) {
        return 1;
    }
    if (ONEPLAYERPOWERUPS == 0 && Cheat_PowerUpTime > 0.0f) {
        if (VehicleArea != 0) {
            return (entry->flags & 0x20000) != 0;
        }
        return (entry->flags & 0x10000) != 0;
    }
    return 0;
}

i32 __cdecl Cheats_CheckFlags(u32 flags) {
    return (i32)(CheatSystemPtr->flags & flags);
}
