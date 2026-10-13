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

void __cdecl PlayGruntSfx(GAMEOBJECT *object) {
    CHARACTERDATA_PC *character = object->character_data;
    i32 sfx = character->game_character->sfx_grunt;
    if (sfx == -1) {
        if (character->model_flags & 0x44002010)
            return;
        sfx = GameAudio->sfx_ids[(object->flags_ef4 & 0x800) != 0 ? 0x14 : 0x15];
    }
    if (sfx != -1)
        PlaySfxById(sfx, &object->collision_position);
}

extern NUVEC nusound_special_positions[5];

void __cdecl GameAudio_PlaySfxById(i32 sfx_id, NUVEC *position, i32 flags, i32 volume) {
    (void)volume;
    if (flags == 0) {
        PlaySfxById(sfx_id, position);
        return;
    }
    if ((flags & ~2) == 1) {
        nusound_special_positions[1] = *position;
        PlaySfxById(sfx_id, &nusound_special_positions[1]);
    }
    flags -= 2;
    if ((u32)flags <= 1) {
        nusound_special_positions[2] = *position;
        PlaySfxById(sfx_id, &nusound_special_positions[2]);
    }
}

void __cdecl GameAudio_PlaySfx(i32 sfx, NUVEC *position, i32 flags, i32 volume) {
    if ((u32)sfx < 0x54) {
        GameAudio_PlaySfxById(GameAudio->sfx_ids[sfx], position, flags, volume);
    }
}

i32 __cdecl GameAudio_GetSfxId(i32 sfx) {
    if ((u32)sfx <= 0x53)
        return GameAudio->sfx_ids[sfx];
    return -1;
}

void __cdecl GameAudio_AddSfx(i32 code, i32 *ids, i32 *count, i32 capacity) {
    i32 id, i;
    if (ids == NULL || count == NULL || *count >= capacity)
        return;
    if ((u32)code >= 0x54)
        return;
    id = GameAudio->sfx_ids[code];
    if (id == -1)
        return;
    for (i = 0; i < *count; ++i)
        if (ids[i] == id)
            return;
    ids[*count] = id;
    ++*count;
}

void __cdecl GameAudio_SetActionMusicTimes(f32 initial_delay, f32 hold_time) {
    ActionMusicStickyTimeoutPc[0] = initial_delay;
    ActionMusicStickyTimeoutPc[1] = hold_time;
}

void __cdecl GameAudio_Reset(void) {
    i32 i;

    GameAudio = &GameAudioStorage;
    memset(&GameAudioStorage, 0, sizeof(GameAudioStorage));
    for (i = 0; i < 84; i++) {
        GameAudioStorage.sfx_names[i] = NULL;
        GameAudioStorage.sfx_ids[i] = -1;
    }
}
