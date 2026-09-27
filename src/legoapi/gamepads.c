#include "saga.h"

#include <string.h>

void __cdecl GamePads_Init(void) {
    i32 i;

    memset(GamePadStorage, 0, sizeof(GamePadStorage));
    GamePadStorage[0].pad = (NUPAD_INIT_STATE *)Game_NuPad[0];
    if (GamePadStorage[0].pad != NULL) {
        GamePadStorage[0].pad->init_type = 0x2a;
        GamePadStorage[0].pad->init_flags = 0x60;
        GamePadStorage[0].pad->init_port = 0x38;
    }

    GamePadStorage[1].pad = (NUPAD_INIT_STATE *)Game_NuPad[1];
    if (GamePadStorage[1].pad != NULL) {
        GamePadStorage[1].pad->init_type = 0x2a;
        GamePadStorage[1].pad->init_flags = 0x60;
        GamePadStorage[1].pad->init_port = 0x38;
    }
}
