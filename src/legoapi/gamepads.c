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

void __cdecl GamePad_InitButtons(void) {
    GAMEPAD_JUMP = 0x40;
    GAMEPAD_ACTION = 0x80;
    GAMEPAD_SPECIAL = 0x20;
    GAMEPAD_TAG = 0x10;
    GAMEPAD_SKIP = 0x840;
    GAMEPAD_MENUSELECT = 0x40;
    GAMEPAD_MENUCANCEL = 0x120;
}

STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(LEVELDATA_PC, flags) == 0x64);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, digital_buttons) == 0x70);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, digital_buttons_pressed) == 0x78);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_right_x) == 0x90);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_right_y) == 0x91);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_left_x) == 0x92);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_left_y) == 0x93);

i32 __cdecl ReadPad(i32 port) {
    GAMEPAD_RUNTIME_INPUT_PC *gamepad = (GAMEPAD_RUNTIME_INPUT_PC *)&GamePadStorage[port];
    FRAME_PAD_PC *pad = gamepad->bound_pad;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 mode;
    f32 axis_x, axis_y;
    u32 directions;
    if (GamePads_IgnoreInputFn != NULL && GamePads_IgnoreInputFn() != 0) {
        mode = 1;
    } else if (readpads_always != 0 || world == NULL || world->current_level == NULL ||
               world->current_level == TITLES_LDATA || world->current_level == CREDITS_LDATA ||
               (world->current_level->flags & 0x400) != 0) {
        mode = 2;
    } else if (port >= PLAYERCOUNT || Player[port] == NULL) {
        mode = 1;
    } else if ((((GAMEOBJECT *)Player[port])->flags_1fc & 0x80) != 0) {
        mode = 2;
    } else {
        mode = 3;
    }
    gamepad->left_held = 0;
    gamepad->left_pressed = 0;
    gamepad->right_held = 0;
    gamepad->right_pressed = 0;
    gamepad->held = 0;
    gamepad->pressed = 0;
    if (pad == NULL || GamePadReadBackendPc(pad, 0) == 0)
        return mode == 1 ? 1 : 0;
    if (mode == 1)
        return 1;
    if (mode == 3) {
        gamepad->pressed |= pad->digital_buttons_pressed & GAMEPAD_START;
        return 3;
    }
    axis_x = (f32)pad->analog_left_x - 127.5f;
    axis_y = (f32)pad->analog_left_y - 127.5f;
    directions = 0;
    if (axis_x < -85.0f)
        directions = GAMEPAD_DLEFT;
    else if (axis_x > 85.0f)
        directions = GAMEPAD_DRIGHT;
    if (axis_y < -85.0f)
        directions |= GAMEPAD_DUP;
    else if (axis_y > 85.0f)
        directions |= GAMEPAD_DDOWN;
    gamepad->left_held = directions;
    gamepad->left_pressed = directions & ~gamepad->left_previous;
    gamepad->left_previous = directions;
    axis_x = (f32)pad->analog_right_x - 127.5f;
    axis_y = (f32)pad->analog_right_y - 127.5f;
    directions = 0;
    if (axis_x < -85.0f)
        directions = GAMEPAD_DLEFT;
    else if (axis_x > 85.0f)
        directions = GAMEPAD_DRIGHT;
    if (axis_y < -85.0f)
        directions |= GAMEPAD_DUP;
    else if (axis_y > 85.0f)
        directions |= GAMEPAD_DDOWN;
    gamepad->right_held = directions;
    gamepad->right_pressed = directions & ~gamepad->right_previous;
    gamepad->right_previous = directions;
    gamepad->input_ready = 1;
    gamepad->held = pad->digital_buttons;
    gamepad->pressed = pad->digital_buttons_pressed;
    GamePadsActivePadPc = gamepad->bound_pad;
    return mode;
}

void __cdecl ReadPads(void) {
    i32 port;
    for (port = 0; port < 2; ++port)
        ReadPad(port);
    readpads_always = 0;
}

i32 __cdecl GamePads_SkipMovie(void) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 skipped = 0;
    i32 pressed_device = 0;
    i32 kind, value, port;
    for (port = 0; port < 2; ++port)
        ReadPad(port);
    readpads_always = 0;
    if ((world == NULL && GAMEDEMO != 0) || (world != NULL && world->current_level == TITLES_LDATA)) {
        if (PcInputSelectPressedDevicePc(0, 0x1a, &kind, &value, 1) >= 0)
            pressed_device = 1;
    }
    if ((GamePadStorage[0].menu_buttons_held & GAMEPAD_SKIP) != 0 || pressed_device != 0 ||
        (GamePadStorage[1].menu_buttons_held & GAMEPAD_SKIP) != 0)
        skipped = 1;
    PCPresentationPlatformLeafPc();
    if (NuKeyboard_db(0x1c) != 0 || NuKeyboard_db(0x39) != 0 || 0 != NuKeyboard_db(1))
        return 1;
    return skipped;
}
