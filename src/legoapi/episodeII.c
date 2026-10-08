#include "saga.h"

i32 __cdecl KaminoInside(void) {
    if (WORLD->area != NULL && WORLD->area == KAMINO_ADATA) {
        if (WORLD->current_level == LevelFixupSlotPc0087b12c) {
            if (CUTSTOPGAME != 0 || GameCam->sock == 5 || GameCam->sock == 6)
                return 1;
        } else if (WORLD->current_level == KAMINOE_LDATA) {
            if (CUTSTOPGAME == 0 && GameCam->sock != 0x1e)
                return 1;
        }
    }
    return 0;
}
i32 __cdecl KaminoDiscoOn(void) {
    return KaminoDiscoMusicStatePc.mode == 2;
}
