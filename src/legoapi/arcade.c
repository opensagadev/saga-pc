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
