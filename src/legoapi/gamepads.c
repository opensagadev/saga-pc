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

u16 __cdecl GamePad_InputAngle(GAMEOBJECT *object, GAMEPAD_PC *pad) {
    if ((object->flags_1fc_word & 0x80) != 0 && object->sock_position.sock_index != -1) {
        SOCK_PC *sock = &WORLD->sock_sys->socks[object->sock_position.sock_index];
        if ((sock->flags & 0x40) != 0)
            return (u16)(pad->input_angle + object->sock_heading + sock->input_yaw);
    }
    return (u16)(pad->input_angle + GameCam->input_yaw);
}
