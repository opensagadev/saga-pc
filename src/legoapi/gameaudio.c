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

extern void(__cdecl *ExtraHurtSfxFn)(GAMEOBJECT *);
void __cdecl PlayHurtSfx(GAMEOBJECT *object) {
    CHARACTERDATA_PC *character = object->character_data;
    i32 sfx = character->game_character->sfx_hurt;
    if (sfx == -1) {
        if (character->model_flags & 0x44002010)
            goto extra;
        sfx = GameAudio->sfx_ids[(object->flags_ef4 & 0x800) != 0 ? 0x16 : 0x17];
    }
    if (sfx != -1)
        PlaySfxById(sfx, &object->collision_position);
extra:
    if (ExtraHurtSfxFn != NULL)
        ExtraHurtSfxFn(object);
}
