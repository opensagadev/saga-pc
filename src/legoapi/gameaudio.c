#include "saga.h"

extern i32 BeenAttacked;

void __cdecl NewAreaMusicChanges(void) {
    BeenAttacked = 0;
}

i32 __cdecl GameAudio_GetPlrSfxBits(void *object_ptr) {
    GAMEOBJECT *object = (GAMEOBJECT *)object_ptr;
    i32 bits = 0;
    if (object != NULL && (i8)object->flags_1fc < 0)
        bits = (u32)1 << (object->field_280 & 31);
    return bits;
}
