#include "saga.h"
void __cdecl ClearLastSafeTakeOver(GAMEOBJECT *object) {
    i32 index;
    GAMEOBJECT *candidate;
    if (object == NULL || (object->portal_flags & 0x100000) != 0)
        return;
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        candidate = &Obj[index];
        if (candidate != NULL && (candidate->flags_1fc_word & 0x1001) == 0x1001 && candidate->takeover_owner == object)
            candidate->takeover_owner = NULL;
    }
}

#include <math.h>
void __cdecl ConstantRumble(GAMEOBJECT *object, f32 strength, f32 phase) {
    f32 weight;
    i32 angle;
    phase = (f32)((f64)phase + (f64)GameTimer.time_elapsed);
    phase = NuFmod(phase, 1.25f);
    weight = 0.0f;
    if (phase < 1.0f) {
        angle = (i32)((f64)phase * 32768.0 + 16384.0);
        weight = 1.0f - fabsf(NuTrigTable[(angle >> 1) & 0x7fff]);
    }
    strength = weight * strength;
    if (object == NULL)
        NewRumbleAllPlayers(strength, 0.0f, 0, 0.0f);
    else if (object->flags_1fc_word & 0x80)
        NewRumble(object->pad->bound_pad, strength, 0);
}
