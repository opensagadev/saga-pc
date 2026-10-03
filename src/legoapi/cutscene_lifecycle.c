#include "saga.h"

struct state_anim_pc_s;

typedef struct cut_rigid_lifecycle_pc_s {
    CUT_RIGID_GENERIC_PC generic;
    struct state_anim_pc_s *state_animation;
    u8 unknown_54[4];
    u8 flags;
    u8 unknown_59[3];
} CUT_RIGID_LIFECYCLE_PC;
STATIC_ASSERT(offsetof(CUT_RIGID_LIFECYCLE_PC, state_animation) == 0x50);
STATIC_ASSERT(offsetof(CUT_RIGID_LIFECYCLE_PC, flags) == 0x58);
STATIC_ASSERT(sizeof(CUT_RIGID_LIFECYCLE_PC) == 0x5c);
STATIC_ASSERT(sizeof(CUT_CHARACTER_GENERIC_PC) == 0x64);

typedef struct cut_rigid_instance_pc_s {
    NUSPECIALHANDLE special;
    u8 state_index;
    u8 unknown_0d[3];
} CUT_RIGID_INSTANCE_PC;
STATIC_ASSERT(offsetof(CUT_RIGID_INSTANCE_PC, state_index) == 0xc);
STATIC_ASSERT(sizeof(CUT_RIGID_INSTANCE_PC) == 0x10);

typedef struct cut_character_instance_pc_s {
    void *model;
    u8 unknown_04[0x14];
} CUT_CHARACTER_INSTANCE_PC;
STATIC_ASSERT(sizeof(CUT_CHARACTER_INSTANCE_PC) == 0x18);

typedef struct cut_rigid_system_lifecycle_pc_s {
    CUT_RIGID_LIFECYCLE_PC *rigids;
    u16 count;
} CUT_RIGID_SYSTEM_LIFECYCLE_PC;
STATIC_ASSERT(offsetof(CUT_RIGID_SYSTEM_LIFECYCLE_PC, count) == 4);

typedef struct cut_character_system_lifecycle_pc_s {
    CUT_CHARACTER_GENERIC_PC *characters;
    u16 count;
} CUT_CHARACTER_SYSTEM_LIFECYCLE_PC;
STATIC_ASSERT(offsetof(CUT_CHARACTER_SYSTEM_LIFECYCLE_PC, count) == 4);

typedef struct cut_rigid_instance_system_pc_s {
    CUT_RIGID_INSTANCE_PC *rigids;
} CUT_RIGID_INSTANCE_SYSTEM_PC;
typedef struct cut_character_instance_system_pc_s {
    CUT_CHARACTER_INSTANCE_PC *characters;
} CUT_CHARACTER_INSTANCE_SYSTEM_PC;

typedef struct cut_locator_lifecycle_pc_s {
    CUT_LOCATOR_GENERIC_PC generic;
    u8 unknown_54[4];
    u8 flags;
    u8 type_index;
    u8 unknown_5a[10];
} CUT_LOCATOR_LIFECYCLE_PC;
STATIC_ASSERT(offsetof(CUT_LOCATOR_LIFECYCLE_PC, flags) == 0x58);
STATIC_ASSERT(offsetof(CUT_LOCATOR_LIFECYCLE_PC, type_index) == 0x59);
STATIC_ASSERT(sizeof(CUT_LOCATOR_LIFECYCLE_PC) == 0x64);

typedef struct cut_locator_type_lifecycle_pc_s {
    u8 unknown_00[4];
    u8 flags;
    u8 unknown_05[7];
} CUT_LOCATOR_TYPE_LIFECYCLE_PC;
STATIC_ASSERT(offsetof(CUT_LOCATOR_TYPE_LIFECYCLE_PC, flags) == 4);
STATIC_ASSERT(sizeof(CUT_LOCATOR_TYPE_LIFECYCLE_PC) == 0xc);

typedef struct cut_locator_system_lifecycle_pc_s {
    CUT_LOCATOR_LIFECYCLE_PC *locators;
    CUT_LOCATOR_TYPE_LIFECYCLE_PC *types;
    u8 count;
} CUT_LOCATOR_SYSTEM_LIFECYCLE_PC;
STATIC_ASSERT(offsetof(CUT_LOCATOR_SYSTEM_LIFECYCLE_PC, types) == 4);
STATIC_ASSERT(offsetof(CUT_LOCATOR_SYSTEM_LIFECYCLE_PC, count) == 8);

typedef struct cut_locator_instance_pc_s {
    f32 frame;
    i32 effect_handle;
} CUT_LOCATOR_INSTANCE_PC;
STATIC_ASSERT(offsetof(CUT_LOCATOR_INSTANCE_PC, effect_handle) == 4);
STATIC_ASSERT(sizeof(CUT_LOCATOR_INSTANCE_PC) == 8);
typedef struct cut_locator_instance_system_pc_s {
    CUT_LOCATOR_INSTANCE_PC *locators;
} CUT_LOCATOR_INSTANCE_SYSTEM_PC;

typedef struct cut_scene_lifecycle_pc_s {
    u8 unknown_00[8];
    f32 duration;
    u8 unknown_0c[8];
    CUT_RIGID_SYSTEM_LIFECYCLE_PC *rigid_system;
    CUT_CHARACTER_SYSTEM_LIFECYCLE_PC *character_system;
    CUT_LOCATOR_SYSTEM_LIFECYCLE_PC *locator_system;
} CUT_SCENE_LIFECYCLE_PC;
STATIC_ASSERT(offsetof(CUT_SCENE_LIFECYCLE_PC, duration) == 8);
STATIC_ASSERT(offsetof(CUT_SCENE_LIFECYCLE_PC, rigid_system) == 0x14);
STATIC_ASSERT(offsetof(CUT_SCENE_LIFECYCLE_PC, character_system) == 0x18);
STATIC_ASSERT(offsetof(CUT_SCENE_LIFECYCLE_PC, locator_system) == 0x1c);

typedef struct cut_camera_instance_lifecycle_pc_s {
    u8 unknown_00[9];
    i8 camera_index;
} CUT_CAMERA_INSTANCE_LIFECYCLE_PC;
STATIC_ASSERT(offsetof(CUT_CAMERA_INSTANCE_LIFECYCLE_PC, camera_index) == 9);

typedef struct cut_instance_lifecycle_pc_s {
    INSTNUGCUTSCENE *next;
    INSTNUGCUTSCENE *previous;
    u8 unknown_08[0x10];
    NUMTX matrix;
    CUT_SCENE_LIFECYCLE_PC *scene;
    CUT_SCENE_LIFECYCLE_PC *saved_scene;
    u8 unknown_60[0x28];
    u32 flags_88;
    u32 flags_8c;
    f32 current_frame;
    f32 render_frame;
    u8 unknown_98[4];
    CUT_CAMERA_INSTANCE_LIFECYCLE_PC *camera_instance;
    CUT_RIGID_INSTANCE_SYSTEM_PC *rigid_instance;
    CUT_CHARACTER_INSTANCE_SYSTEM_PC *character_instance;
    CUT_LOCATOR_INSTANCE_SYSTEM_PC *locator_instance;
} CUT_INSTANCE_LIFECYCLE_PC;
STATIC_ASSERT(offsetof(CUT_INSTANCE_LIFECYCLE_PC, matrix) == 0x18);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LIFECYCLE_PC, scene) == 0x58);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LIFECYCLE_PC, saved_scene) == 0x5c);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LIFECYCLE_PC, flags_88) == 0x88);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LIFECYCLE_PC, flags_8c) == 0x8c);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LIFECYCLE_PC, current_frame) == 0x90);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LIFECYCLE_PC, render_frame) == 0x94);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LIFECYCLE_PC, camera_instance) == 0x9c);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LIFECYCLE_PC, rigid_instance) == 0xa0);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LIFECYCLE_PC, character_instance) == 0xa4);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LIFECYCLE_PC, locator_instance) == 0xa8);

typedef struct cut_sound_voice_lifecycle_pc_s {
    u8 unknown_00[0x80];
    f32 first;
    f32 second;
} CUT_SOUND_VOICE_LIFECYCLE_PC;
STATIC_ASSERT(offsetof(CUT_SOUND_VOICE_LIFECYCLE_PC, first) == 0x80);
STATIC_ASSERT(offsetof(CUT_SOUND_VOICE_LIFECYCLE_PC, second) == 0x84);
typedef struct cut_sound_owner_lifecycle_pc_s {
    u8 unknown_00[0xc];
    CUT_SOUND_VOICE_LIFECYCLE_PC **voices;
} CUT_SOUND_OWNER_LIFECYCLE_PC;
STATIC_ASSERT(offsetof(CUT_SOUND_OWNER_LIFECYCLE_PC, voices) == 0xc);
typedef struct cut_sound_handle_lifecycle_pc_s {
    CUT_SOUND_OWNER_LIFECYCLE_PC *owner;
    u8 unknown_04[4];
    i16 active;
    u16 third_voice;
    u8 unknown_0c[2];
    u16 first_voice;
    u16 second_voice;
} CUT_SOUND_HANDLE_LIFECYCLE_PC;
STATIC_ASSERT(offsetof(CUT_SOUND_HANDLE_LIFECYCLE_PC, active) == 8);
STATIC_ASSERT(offsetof(CUT_SOUND_HANDLE_LIFECYCLE_PC, third_voice) == 0xa);
STATIC_ASSERT(offsetof(CUT_SOUND_HANDLE_LIFECYCLE_PC, first_voice) == 0xe);
STATIC_ASSERT(offsetof(CUT_SOUND_HANDLE_LIFECYCLE_PC, second_voice) == 0x10);

static void CutSceneDestroyCharactersPC(CUT_INSTANCE_LIFECYCLE_PC *instance) {
    CUT_CHARACTER_SYSTEM_LIFECYCLE_PC *system = instance->scene->character_system;
    CUT_CHARACTER_INSTANCE_SYSTEM_PC *characters = instance->character_instance;
    i32 i;
    if (system != NULL) {
        for (i = 0; i < system->count; ++i) {
            if ((system->characters[i].flags & 2) != 0 && NuCutSceneCharacterDestroyData != NULL) {
                NuCutSceneCharacterDestroyData(&system->characters[i], &characters->characters[i]);
            }
        }
    }
}

static void CutSceneEndCharactersPC(INSTNUGCUTSCENE *owner, f32 frame) {
    CUT_INSTANCE_LIFECYCLE_PC *instance = (CUT_INSTANCE_LIFECYCLE_PC *)owner;
    CUT_SCENE_LIFECYCLE_PC *scene = instance->scene;
    CUT_CHARACTER_SYSTEM_LIFECYCLE_PC *system = scene->character_system;
    CUT_CHARACTER_INSTANCE_SYSTEM_PC *characters = instance->character_instance;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_CHARACTER_INSTANCE_PC *character = &characters->characters[i];
        CUT_CHARACTER_GENERIC_PC *source = &system->characters[i];
        if (character->model != NULL) {
            if ((source->flags & 2) == 0 && NuCutSceneCharacterEval != NULL) {
                NuCutSceneCharacterEval(owner, scene, character, source, frame);
            }
            if (nu_current_thread_id == 0 && NuCutSceneCharacterRelease != NULL) {
                NuCutSceneCharacterRelease(character, source);
            }
        }
    }
}

static void CutSceneEndRigidsPC(INSTNUGCUTSCENE *owner, f32 frame) {
    CUT_INSTANCE_LIFECYCLE_PC *instance = (CUT_INSTANCE_LIFECYCLE_PC *)owner;
    CUT_RIGID_SYSTEM_LIFECYCLE_PC *system = instance->scene->rigid_system;
    CUT_RIGID_INSTANCE_SYSTEM_PC *objects = instance->rigid_instance;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_RIGID_LIFECYCLE_PC *source = &system->rigids[i];
        CUT_RIGID_INSTANCE_PC *object = &objects->rigids[i];
        if ((source->flags & 4) != 0 && (source->flags & 2) == 0) {
            NUMTX matrix;
            if (source->state_animation != NULL) {
                u8 visible;
                if (StateAnimEvaluate(source->state_animation, &object->state_index, &visible, frame) != 0) {
                    NuSpecialSetVisibility(&object->special, visible != 0);
                }
            }
            if (NuSpecialGetVisibilityFn(&object->special) == 0) {
                continue;
            }
            NuGCutRigidCalcMtx(&source->generic, frame, &matrix);
            if ((instance->flags_88 & 0x80) != 0) {
                NuMtxMul(&matrix, &matrix, &instance->matrix);
            }
            NuSpecialSetDrawMtx(&object->special, &matrix);
        }
    }
}

static void CutSceneEndLocatorsPC(CUT_LOCATOR_INSTANCE_SYSTEM_PC *instance, CUT_LOCATOR_SYSTEM_LIFECYCLE_PC *system) {
    u32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOCATOR_INSTANCE_PC *state = &instance->locators[i];
        CUT_LOCATOR_LIFECYCLE_PC *locator = &system->locators[i];
        CUT_LOCATOR_TYPE_LIFECYCLE_PC *type = &system->types[locator->type_index];
        state->frame = 0.0f;
        if ((type->flags & 8) != 0) {
            CUT_SOUND_HANDLE_LIFECYCLE_PC *sound = (CUT_SOUND_HANDLE_LIFECYCLE_PC *)(u32)state->effect_handle;
            if (sound->active != 0 && sound->owner != NULL) {
                CUT_SOUND_VOICE_LIFECYCLE_PC *voice;
                if (sound->first_voice != 0) {
                    voice = sound->owner->voices[sound->first_voice - 1];
                    voice->first = 0.0f;
                    voice->second = 0.0f;
                }
                if (sound->second_voice != 0) {
                    voice = sound->owner->voices[sound->second_voice - 1];
                    voice->first = 0.0f;
                    voice->second = 0.0f;
                }
                if (sound->third_voice != 0) {
                    voice = sound->owner->voices[sound->third_voice - 1];
                    voice->first = 0.0f;
                    voice->second = 0.0f;
                }
            }
        } else if ((type->flags & 1) != 0) {
            if ((locator->flags & 0x20) != 0 && state->effect_handle >= 0) {
                DebFreeInstantly(&state->effect_handle);
            }
        } else if ((type->flags & 0x10) != 0) {
            if (ReleaseLocatorVfxFn != NULL) {
                ReleaseLocatorVfxFn(state->effect_handle);
            }
            state->effect_handle = -1;
            state->frame = 0.0f;
        }
    }
}

void __cdecl NuGCutSceneDestroy(void *owner) {
    CUT_SCENE_LIFECYCLE_PC *scene = (CUT_SCENE_LIFECYCLE_PC *)owner;
    if (scene->character_system != NULL && NuCutSceneDestroyCharacters != NULL) {
        NuCutSceneDestroyCharacters(scene);
    }
}

void __cdecl instNuGCutSceneEnd(INSTNUGCUTSCENE *owner) {
    CUT_INSTANCE_LIFECYCLE_PC *instance = (CUT_INSTANCE_LIFECYCLE_PC *)owner;
    CUT_SCENE_LIFECYCLE_PC *scene = instance->scene;
    instNuGCutSceneEndButNotSystems(owner);
    instance->current_frame = scene->duration;
    instance->flags_88 = (instance->flags_88 & ~2U) | 0x1000;
    instance->flags_8c &= ~0x40U;
    ForcePlayEndFrame = 1;
    if ((instance->flags_88 & 0x40000) != 0) {
        if (scene->rigid_system != NULL) {
            CutSceneEndRigidsPC(owner, instance->scene->duration - instance->current_frame);
        }
        if (instance->character_instance != NULL) {
            CutSceneEndCharactersPC(owner, instance->scene->duration - instance->current_frame);
        }
    } else {
        if (scene->rigid_system != NULL) {
            CutSceneEndRigidsPC(owner, instance->current_frame);
        }
        if (instance->character_instance != NULL) {
            CutSceneEndCharactersPC(owner, instance->current_frame);
        }
    }
    if (instance->locator_instance != NULL) {
        CutSceneEndLocatorsPC(instance->locator_instance, scene->locator_system);
    }
    ForcePlayEndFrame = 0;
    if (instance->camera_instance != NULL && instance->camera_instance->camera_index >= 0) {
        CutSceneCameraCTRL = 0;
    }
}

void __cdecl instNuGCutSceneDestroy(INSTNUGCUTSCENE *owner) {
    CUT_INSTANCE_LIFECYCLE_PC *instance = (CUT_INSTANCE_LIFECYCLE_PC *)owner;
    instance->scene = instance->saved_scene;
    if ((instance->flags_88 & 2) != 0) {
        instNuGCutSceneEnd(owner);
    }
    if (instance->character_instance != NULL) {
        CutSceneDestroyCharactersPC(instance);
    }
    if (instance->next != NULL) {
        ((CUT_INSTANCE_LIFECYCLE_PC *)instance->next)->previous = instance->previous;
    }
    if (instance->previous != NULL) {
        ((CUT_INSTANCE_LIFECYCLE_PC *)instance->previous)->next = instance->next;
    } else {
        active_cutscene_instances = instance->next;
    }
}
