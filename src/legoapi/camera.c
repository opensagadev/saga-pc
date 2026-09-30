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
