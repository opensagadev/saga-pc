#include "saga.h"

i32 __cdecl GoingForwardsAlongNarrowSock(GAMEOBJECT *object) {
    i32 difference;
    i32 forwards;
    if (object->in_narrow_socket == 0)
        return 0;
    difference = RotDiff(object->sock_heading, object->field_0x276);
    if (difference < 0)
        difference = -difference;
    forwards = difference < 0x4000;
    if (object->character_context == 0x2a && (f64)object->context_duration * (f64)0.8f > object->context_timer)
        forwards = 1 - forwards;
    return forwards;
}

GAMEPAD_PC *__cdecl ViewCamGetGamePad(void) {
    return ViewCam.gamepad;
}

#pragma data_seg(".data$ViewCam")
VIEWCAM_PC ViewCam = {0,   {1000000000.0f, 0.0f, 0.0f}, -8192, -29500, 2.6600000858306885f, 0.0f, 120.0f, 0x43480000,
                      NULL};
#pragma data_seg()

typedef struct CAMERA_JUDDER_FIELDS_PC {
    u8 unknown_000[0x11c];
    NUVEC position;
    u8 unknown_128[0xa0];
    f32 time;
    f32 duration;
    u8 unknown_1d0[0x58];
    u8 reverse;
    u8 axis;
} CAMERA_JUDDER_FIELDS_PC;
STATIC_ASSERT(offsetof(CAMERA_JUDDER_FIELDS_PC, position) == 0x11c);
STATIC_ASSERT(offsetof(CAMERA_JUDDER_FIELDS_PC, time) == 0x1c8);
STATIC_ASSERT(offsetof(CAMERA_JUDDER_FIELDS_PC, duration) == 0x1cc);
STATIC_ASSERT(offsetof(CAMERA_JUDDER_FIELDS_PC, reverse) == 0x228);
STATIC_ASSERT(offsetof(CAMERA_JUDDER_FIELDS_PC, axis) == 0x229);
typedef struct LEVEL_JUDDER_FIELDS_PC {
    u8 unknown_000[0xcd];
    u8 maximum_distance;
} LEVEL_JUDDER_FIELDS_PC;
STATIC_ASSERT(offsetof(LEVEL_JUDDER_FIELDS_PC, maximum_distance) == 0xcd);

void __cdecl GameCam_Judder(GAMECAMERA *camera, f32 amount, i32 axis, NUVEC *source) {
    union {
        f32 value;
        u32 bits;
    } absolute;
    CAMERA_JUDDER_FIELDS_PC *judder;
    f32 distance;
    f32 maximum_distance;
    if (camera == NULL)
        camera = GameCam;
    judder = (CAMERA_JUDDER_FIELDS_PC *)camera;
    absolute.value = amount;
    absolute.bits &= 0x7fffffff;
    if (absolute.value > judder->time) {
        if (source != NULL) {
            distance = NuVecDist(&judder->position, source, NULL);
            maximum_distance = (f32)((LEVEL_JUDDER_FIELDS_PC *)WORLD->current_level)->maximum_distance;
            if (maximum_distance > distance) {
                absolute.value *= (maximum_distance - distance) / maximum_distance;
                if (absolute.value > judder->time) {
                    judder->duration = absolute.value;
                    judder->time = absolute.value;
                    judder->reverse = amount < 0.0f;
                }
            }
        } else {
            judder->duration = absolute.value;
            judder->time = absolute.value;
            judder->reverse = amount < 0.0f;
        }
        judder->axis = (u8)axis;
    }
}

typedef struct CAMERA_BLEND_FIELDS_PC {
    u8 unknown_000[0x128];
    NUVEC target;
    NUVEC blend_start_position;
    NUVEC desired_position;
    NUVEC blend_end_position;
    u8 unknown_158[0xc];
    NUVEC blend_start_target;
    u8 unknown_170[0xc];
    NUVEC blend_end_target;
    u8 unknown_188[0x34];
    f32 blend_time;
    f32 blend_duration;
    f32 blend_curve;
    u8 unknown_1c8[0x54];
    u16 blend_start_pitch;
    u16 desired_pitch;
    u16 blend_start_yaw;
    u16 desired_yaw;
    u16 blend_start_roll;
    u16 desired_roll;
    u8 unknown_228[2];
    u8 reset_blend;
    u8 blend_mode;
    i8 mode;
    u8 previous_mode;
    u8 previous_camera_mode;
} CAMERA_BLEND_FIELDS_PC;
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, target) == 0x128);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_start_position) == 0x134);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, desired_position) == 0x140);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_end_position) == 0x14c);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_start_target) == 0x164);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_end_target) == 0x17c);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_time) == 0x1bc);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_duration) == 0x1c0);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_curve) == 0x1c4);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_start_pitch) == 0x21c);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, desired_pitch) == 0x21e);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_start_yaw) == 0x220);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, desired_yaw) == 0x222);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_start_roll) == 0x224);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, desired_roll) == 0x226);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, reset_blend) == 0x22a);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_mode) == 0x22b);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, mode) == 0x22c);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, previous_mode) == 0x22d);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, previous_camera_mode) == 0x22e);

void __cdecl GameCam_Blend(GAMECAMERA *camera, f32 duration, f32 curve, i32 mode) {
    CAMERA_BLEND_FIELDS_PC *blend;
    if (camera == NULL)
        camera = GameCam;
    blend = (CAMERA_BLEND_FIELDS_PC *)camera;
    if (!(duration > 0.0f) || blend->mode == -1)
        return;
    blend->blend_start_pitch = blend->desired_pitch;
    blend->blend_start_yaw = blend->desired_yaw;
    blend->blend_start_roll = blend->desired_roll;
    blend->blend_mode = mode != 0 ? 2 : 1;
    blend->previous_camera_mode = blend->previous_mode;
    blend->blend_start_position = blend->desired_position;
    blend->blend_end_position = blend->desired_position;
    blend->blend_start_target = blend->target;
    blend->blend_end_target = blend->target;
    blend->reset_blend = 1;
    blend->blend_time = 0.0f;
    blend->blend_duration = duration;
    blend->blend_curve = curve;
}
