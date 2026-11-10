#include "saga.h"
void __cdecl ClearLastSafeTakeOver(GAMEOBJECT *object) {
    i32 index;
    GAMEOBJECT *candidate;
    if (object == NULL || (object->portal_flags & 0x100000) != 0)
        return;
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        candidate = &Obj[index];
        if (candidate != NULL && (candidate->flags_1fc_word & 1) && (candidate->flags_1fc_word & 0x1000) &&
            candidate->takeover_owner == object)
            candidate->takeover_owner = NULL;
    }
}

#include <math.h>
void __cdecl ConstantRumble(GAMEOBJECT *object, f32 strength, f32 phase) {
    i32 angle;
    phase += GameTimer.time_elapsed;
    phase = NuFmod(phase, 1.25f);
    if (phase < 1.0f) {
        angle = (i32)((f64)phase * 32768.0 + 16384.0);
        phase = NuTrigTable[(angle >> 1) & 0x7fff];
        *(u32 *)&phase &= 0x7fffffff;
        phase = 1.0f - phase;
    } else {
        phase = 0.0f;
    }
    strength = phase * strength;
    if (object != NULL) {
        if (object->flags_1fc_word & 0x80)
            NewRumble(object->pad->bound_pad, strength, 0);
    } else
        NewRumbleAllPlayers(strength, 0.0f, 0, 0);
}
