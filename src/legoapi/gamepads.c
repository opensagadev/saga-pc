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

f32 __cdecl GamePad_Rotate(GAMEOBJECT *object) {
    GAMEPAD_PC *pad;
    i32 index, current_delta, previous_delta;
    if (object == Player[0])
        index = 0;
    else if (object == Player[1])
        index = 1;
    else
        return 0.0f;
    pad = object->pad;
    if (pad->input_magnitude == 0.0f || PlayerPadCurrentSpeed[index] == 0.0f || PlayerPadPreviousSpeed[index] == 0.0f)
        return 0.0f;
    current_delta = RotDiff(PlayerPadCurrentAngle[index], pad->input_angle);
    previous_delta = RotDiff(PlayerPadPreviousAngle[index], PlayerPadCurrentAngle[index]);
    if (!((current_delta < 0 && previous_delta < 0 && current_delta > -0x2000 && previous_delta > -0x2000) ||
          (current_delta > 0 && previous_delta > 0 && current_delta < 0x2000 && previous_delta < 0x2000)))
        return 0.0f;
    return (f32)(((f64)current_delta * (1.0 / 65536.0)) / FRAMETIME);
}

i32 __cdecl GamePad_Waggle(GAMEPAD_PC *pad) {
    i32 delta;
    if (pad->input_magnitude != 0.0f) {
        delta = RotDiff(pad->previous_input_angle, pad->input_angle);
        if (delta < 0)
            delta = -delta;
        if (delta > 0x2000)
            return 1;
    }
    return (pad->input_magnitude != 0.0f) != (pad->previous_input_magnitude != 0.0f);
}

void __cdecl NewRumble(void *pad, f32 strength, i32 ignored) {
    i32 amount;
    (void)ignored;
    if (pad != NULL) {
        amount = (i32)(strength * 255.0);
        if (amount > 255)
            amount = 255;
        NuSound3AddRumble(pad, 0.0f, amount, 0, strength);
    }
}

void __cdecl NewBuzz(void *pad, f32 duration, i32 ignored) {
    (void)ignored;
    if (pad != NULL)
        NuSound3AddRumble(pad, duration, 0, 0, 0.0f);
}

void __cdecl NewBuzzFrames(void *pad, i32 frames, i32 ignored) {
    (void)ignored;
    if (pad != NULL)
        NuSound3AddRumble(pad, (f32)frames / DEFAULTFPS, 0, 0, 0.0f);
}

void __cdecl TakeHitRumble(GAMEOBJECT *object, f32 strength) {
    if (object != NULL && (object->flags_1fc & 0x80) != 0) {
        NewRumble(object->pad->bound_pad, strength, 0);
        NewBuzz(object->pad->bound_pad, 0.1f, 0);
    }
}

GAMEPAD_PC *__cdecl GamePad_Allocate(void) {
    i32 index;
    for (index = 0; index < 64; ++index) {
        if ((GamePadStorage[index].allocated & 1) == 0) {
            GamePadStorage[index].allocated |= 1;
            return (GAMEPAD_PC *)&GamePadStorage[index];
        }
    }
    return NULL;
}

void __cdecl ResetRumble(RUMBLE_PACKET_RESOURCE_PC *packet) {
    packet->duration = 0.0f;
    packet->amount = 0.0f;
    packet->active = 0;
}

void __cdecl UpdateRumble(RUMBLE_PACKET_RESOURCE_PC *packet) {
    if (packet->duration > 0.0f)
        packet->duration -= FRAMETIME;
    if (packet->amount > 0.0f)
        packet->amount -= FRAMETIME;
    if (packet->active != 0)
        --packet->active;
}
