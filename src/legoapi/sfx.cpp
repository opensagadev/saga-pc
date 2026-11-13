#include "saga.h"

extern "C" f32 AUDIOFADELEVEL;

void SetAudioFadeLevel(f32 level) {
    AUDIOFADELEVEL = level;
}

f32 GetAudioFadeLevel(void) {
    return AUDIOFADELEVEL;
}

void SfxCheckMusicOnOff(OPTIONSSAVE *options) {
    (void)options;
}

extern "C" void __cdecl PlaySfxByIdAndSetVolumeAndPitch(i32 sfx_id, NUVEC *position, f32 volume, f32 pitch) {
    PlaySfxByIdEx(sfx_id, position, volume, pitch);
}

typedef struct level_sfx_gizmotype_s {
    u8 unknown_00[0x94];
    void(__cdecl *add_level_sfx)(WORLDINFO *, void *, i32 *, i32 *, i32);
    u8 unknown_98[4];
} LEVEL_SFX_GIZMOTYPE_PC;
STATIC_ASSERT(sizeof(LEVEL_SFX_GIZMOTYPE_PC) == 0x9c);
STATIC_ASSERT(offsetof(LEVEL_SFX_GIZMOTYPE_PC, add_level_sfx) == 0x94);

extern "C" {

    void __cdecl AddLevelSfxGizmoSys(GIZMOSYS_PREFIX_PC *system, WORLDINFO *world, i32 *ids, i32 *count, i32 capacity) {
        LEVEL_SFX_GIZMOTYPE_PC *type;
        GIZMOSET_PC *set;
        i32 i;
        if (gizmotypes == NULL || system == NULL)
            return;
        type = (LEVEL_SFX_GIZMOTYPE_PC *)((GIZMOTYPES_PC *)gizmotypes)->types;
        set = system->sets;
        for (i = 0; i < ((GIZMOTYPES_PC *)gizmotypes)->count; ++i, ++set, ++type)
            if (type->add_level_sfx != NULL)
                type->add_level_sfx(world, set->data, ids, count, capacity);
    }
}

extern "C" void __cdecl PlaySfxByIdAndSetPitch(i32 sfx_id, NUVEC *position, f32 pitch) {
    PlaySfxByIdEx(sfx_id, position, 1.0f, pitch);
}

extern "C" {
    typedef struct footstep_audio_callbacks_pc_s {
        i32(__cdecl *override_footstep)(GAMEOBJECT *, i32);
    } FOOTSTEP_AUDIO_CALLBACKS_PC;
    STATIC_ASSERT(offsetof(FOOTSTEP_AUDIO_CALLBACKS_PC, override_footstep) == 0);
}

extern "C" i32 __cdecl IsSfxLooping(i32 id) {
    if (id != -1)
        return (SfxInfo[id].sample_id_flags >> 15) & 1;
    return -1;
}

#include <stdio.h>

extern "C" i32 __cdecl InitSpecialSfx(void *world_ptr) {
    SPECIAL_RESOURCE_WORLD_PC *world = (SPECIAL_RESOURCE_WORLD_PC *)world_ptr;
    if (!world)
        return 0;
    world->buffer.addr = (world->buffer.addr + 3) & ~3u;
    world->entries = (SPECIAL_RESOURCE_ENTRY_PC *)world->buffer.addr;
    world->buffer.addr += 0xf00;
    world->entry_count = 0;
    memset(world->entries, 0, 0xf00);
    world->buffer.addr = (world->buffer.addr + 15) & ~15u;
    world->events = (SPECIAL_RESOURCE_EVENT_PC *)world->buffer.addr;
    world->buffer.addr = (world->buffer.addr + 0xc03) & ~3u;
    world->event_count = 0;
    memset(world->events, 0, 0xc00);
    return 1;
}
extern "C" void __cdecl LoadSpecialSfxFile(void *world_ptr) {
    SPECIAL_RESOURCE_WORLD_PC *world = (SPECIAL_RESOURCE_WORLD_PC *)world_ptr;
    char path[256];
    sprintf(path, "%s.sfx", world->config_file);
    if (NuFileExists(path))
        SpecialSfxLoad(path, world);
}
