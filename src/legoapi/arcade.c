#include "saga.h"

i32 __cdecl Arcade_GetMode(u32 *flags) {
    if (Arcade == 0) {
        if (flags != NULL) {
            *flags = 0;
        }
        return -1;
    }
    if (flags != NULL) {
        *flags = Arcade_Mode[ArcadeItem.mode].flags;
    }
    return ArcadeItem.mode;
}

typedef struct ARCADE_COIN_PACKET_PC {
    u32 coins;
    f32 scale;
} ARCADE_COIN_PACKET_PC;
STATIC_ASSERT(offsetof(ARCADE_COIN_PACKET_PC, coins) == 0);
STATIC_ASSERT(offsetof(ARCADE_COIN_PACKET_PC, scale) == 4);
