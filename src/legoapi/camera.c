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
