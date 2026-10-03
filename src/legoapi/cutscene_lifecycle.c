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

typedef struct cut_load_locator_s {
    NUMTX matrix;
    NUVEC pivot;
    f32 scale;
    void *animation;
    u32 unknown_54;
    u8 flags, type_index, unknown_5a, tracks_self;
    u8 unknown_5c[8];
} CUT_LOAD_LOCATOR;
STATIC_ASSERT(sizeof(CUT_LOAD_LOCATOR) == 0x64);
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR, animation) == 0x50);
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR, flags) == 0x58);
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR, type_index) == 0x59);
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR, tracks_self) == 0x5b);
typedef struct cut_load_locator_type_s {
    char *name;
    u8 flags, unknown_05;
    u16 function_index;
    u32 unknown_08;
} CUT_LOAD_LOCATOR_TYPE;
STATIC_ASSERT(sizeof(CUT_LOAD_LOCATOR_TYPE) == 12);
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR_TYPE, function_index) == 6);
typedef struct cut_load_locator_system_s {
    CUT_LOAD_LOCATOR *locators;
    CUT_LOAD_LOCATOR_TYPE *types;
    u8 count, type_count;
    u8 unknown_0a[2];
} CUT_LOAD_LOCATOR_SYSTEM;
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR_SYSTEM, count) == 8);
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR_SYSTEM, type_count) == 9);
typedef struct cut_load_rigid_s {
    NUMTX matrix;
    char *name;
    NUGSCN *scene;
    void *special;
    void *animation;
    STATE_ANIM_PC *state_animation;
    union {
        i32 index;
        CUT_LOAD_LOCATOR *pointer;
    } locator;
    u8 flags, uses_locator, locator_index, unknown_5b;
} CUT_LOAD_RIGID;
STATIC_ASSERT(sizeof(CUT_LOAD_RIGID) == 0x5c);
STATIC_ASSERT(offsetof(CUT_LOAD_RIGID, name) == 0x40);
STATIC_ASSERT(offsetof(CUT_LOAD_RIGID, animation) == 0x4c);
STATIC_ASSERT(offsetof(CUT_LOAD_RIGID, state_animation) == 0x50);
STATIC_ASSERT(offsetof(CUT_LOAD_RIGID, locator) == 0x54);
STATIC_ASSERT(offsetof(CUT_LOAD_RIGID, flags) == 0x58);
STATIC_ASSERT(offsetof(CUT_LOAD_RIGID, uses_locator) == 0x59);
STATIC_ASSERT(offsetof(CUT_LOAD_RIGID, locator_index) == 0x5a);
typedef struct cut_load_rigid_system_s {
    CUT_LOAD_RIGID *rigids;
    u16 count, unused;
} CUT_LOAD_RIGID_SYSTEM;
struct cut_load_character_s {
    NUMTX matrix;
    char *name;
    void *animation, *face_animation, *extra_animation;
    void *model;
    u8 unknown_54[8];
    u8 flags, unknown_5d[7];
};
STATIC_ASSERT(sizeof(CUT_LOAD_CHARACTER) == 0x64);
STATIC_ASSERT(offsetof(CUT_LOAD_CHARACTER, name) == 0x40);
STATIC_ASSERT(offsetof(CUT_LOAD_CHARACTER, model) == 0x50);
STATIC_ASSERT(offsetof(CUT_LOAD_CHARACTER, flags) == 0x5c);
typedef struct cut_load_character_system_s {
    CUT_LOAD_CHARACTER *characters;
    u16 count;
} CUT_LOAD_CHARACTER_SYSTEM;
typedef struct cut_load_character_animation_s {
    u32 unknown_00;
    void *animation;
    u8 unknown_08[8];
} CUT_LOAD_CHARACTER_ANIMATION;
STATIC_ASSERT(sizeof(CUT_LOAD_CHARACTER_ANIMATION) == 16);
typedef struct cut_load_camera_system_s {
    i32 count;
    void *cameras;
    void *animation;
    STATE_ANIM_PC *state_animation;
    u8 unknown_10[12];
    void *focus_animation;
    STATE_ANIM_PC *focus_state_animation;
} CUT_LOAD_CAMERA_SYSTEM;
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_SYSTEM, focus_animation) == 0x1c);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_SYSTEM, focus_state_animation) == 0x20);
typedef struct cut_load_trigger_event_s {
    u16 index, unknown_02;
    char *name;
    STATE_ANIM_PC *state_animation;
} CUT_LOAD_TRIGGER_EVENT;
STATIC_ASSERT(sizeof(CUT_LOAD_TRIGGER_EVENT) == 12);
typedef struct cut_load_trigger_system_s {
    i32 count;
    CUT_LOAD_TRIGGER_EVENT *events;
} CUT_LOAD_TRIGGER_SYSTEM;
typedef struct cut_load_bounds_s {
    NUVEC min, max;
} CUT_LOAD_BOUNDS;
struct cut_scene_load_pc_s {
    i32 version, string_delta;
    f32 duration;
    char *strings;
    CUT_LOAD_CAMERA_SYSTEM *camera_system;
    CUT_LOAD_RIGID_SYSTEM *rigid_system;
    CUT_LOAD_CHARACTER_SYSTEM *character_system;
    CUT_LOAD_LOCATOR_SYSTEM *locator_system;
    CUT_LOAD_BOUNDS *bounds;
    CUT_LOAD_TRIGGER_SYSTEM *trigger_system;
    i32 relocation_delta;
    char *filename;
    void *stream_buffer_0, *stream_buffer_1;
    u32 flags;
    i32 stream_buffer_size;
    u32 unknown_40;
    NUGSCN *scene;
    void *extra_scene;
    u32 stream_buffer_offset;
    CUT_LOAD_CHARACTER_ANIMATION *character_animations;
    u16 *focus_indices;
    i32 loaded_size;
};
STATIC_ASSERT(sizeof(CUT_SCENE_LOAD_PC) == 0x5c);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, string_delta) == 4);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, camera_system) == 0x10);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, rigid_system) == 0x14);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, character_system) == 0x18);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, locator_system) == 0x1c);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, bounds) == 0x20);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, trigger_system) == 0x24);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, relocation_delta) == 0x28);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, flags) == 0x38);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, scene) == 0x44);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, extra_scene) == 0x48);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, character_animations) == 0x50);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, focus_indices) == 0x54);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, loaded_size) == 0x58);

typedef struct cut_load_camera_instance_s {
    u32 unknown_00;
    u32 *states;
    u8 unknown_08[8];
} CUT_LOAD_CAMERA_INSTANCE;
typedef struct cut_load_rigid_instance_s {
    NUSPECIALHANDLE special;
    u8 state_index, visible, unknown_0e[2];
} CUT_LOAD_RIGID_INSTANCE;
STATIC_ASSERT(sizeof(CUT_LOAD_RIGID_INSTANCE) == 16);
typedef struct cut_load_rigid_instance_system_s {
    CUT_LOAD_RIGID_INSTANCE *rigids;
} CUT_LOAD_RIGID_INSTANCE_SYSTEM;
struct cut_load_character_instance_s {
    void *model;
    u8 unknown_04[0x11];
    u8 animation_a, animation_b, unknown_17;
};
STATIC_ASSERT(sizeof(CUT_LOAD_CHARACTER_INSTANCE) == 0x18);
STATIC_ASSERT(offsetof(CUT_LOAD_CHARACTER_INSTANCE, animation_a) == 0x15);
STATIC_ASSERT(offsetof(CUT_LOAD_CHARACTER_INSTANCE, animation_b) == 0x16);
typedef struct cut_load_character_instance_system_s {
    CUT_LOAD_CHARACTER_INSTANCE *characters;
} CUT_LOAD_CHARACTER_INSTANCE_SYSTEM;
typedef struct cut_load_focus_target_s {
    u32 unknown_00[5];
    CUT_LOAD_LOCATOR *self;
} CUT_LOAD_FOCUS_TARGET;
STATIC_ASSERT(sizeof(CUT_LOAD_FOCUS_TARGET) == 0x18);
typedef struct cut_load_sound_state_s {
    u32 unknown_00[4];
} CUT_LOAD_SOUND_STATE;
typedef struct cut_load_locator_instance_s {
    u32 unknown_00;
    union {
        i32 index;
        CUT_LOAD_FOCUS_TARGET *focus;
        CUT_LOAD_SOUND_STATE *sound;
    } effect;
} CUT_LOAD_LOCATOR_INSTANCE;
STATIC_ASSERT(sizeof(CUT_LOAD_LOCATOR_INSTANCE) == 8);
typedef struct cut_load_locator_instance_system_s {
    CUT_LOAD_LOCATOR_INSTANCE *locators;
} CUT_LOAD_LOCATOR_INSTANCE_SYSTEM;
typedef struct cut_load_trigger_entry_s {
    char *name;
    u8 unknown_04[0x30];
} CUT_LOAD_TRIGGER_ENTRY;
STATIC_ASSERT(sizeof(CUT_LOAD_TRIGGER_ENTRY) == 0x34);
typedef struct cut_load_trigger_owner_s {
    u8 unknown_00[8];
    i32 count;
    CUT_LOAD_TRIGGER_ENTRY *entries;
} CUT_LOAD_TRIGGER_OWNER;
typedef struct cut_load_trigger_instance_s {
    CUT_LOAD_TRIGGER_OWNER *owner;
    u32 *states;
} CUT_LOAD_TRIGGER_INSTANCE;
typedef struct cut_instance_load_pc_s {
    INSTNUGCUTSCENE *next, *previous;
    char name[16];
    NUMTX matrix;
    CUT_SCENE_LOAD_PC *scene, *saved_scene;
    NUVEC center;
    u8 unknown_6c[0x1c];
    u32 flags_88, flags_8c;
    f32 current_frame, render_frame, rate;
    CUT_LOAD_CAMERA_INSTANCE *camera_instance;
    CUT_LOAD_RIGID_INSTANCE_SYSTEM *rigid_instance;
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *character_instance;
    CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *locator_instance;
    CUT_LOAD_TRIGGER_INSTANCE *trigger_instance;
    u8 unknown_b0[12];
    void *stream_buffer_0, *stream_buffer_1;
    NUGSCN *scene_owner;
    CUT_LOAD_TRIGGER_OWNER *extra_scene_owner;
    u8 unknown_cc[19];
    u8 stream_index;
    f32 accumulated_duration, elapsed, alpha;
    void *pending_stream;
    u32 allocation_size, unknown_f4;
} CUT_INSTANCE_LOAD_PC;
STATIC_ASSERT(sizeof(CUT_INSTANCE_LOAD_PC) == 0xf8);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, flags_88) == 0x88);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, scene) == 0x58);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, camera_instance) == 0x9c);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, locator_instance) == 0xa8);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, trigger_instance) == 0xac);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, stream_buffer_0) == 0xbc);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, scene_owner) == 0xc4);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, extra_scene_owner) == 0xc8);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, stream_index) == 0xdf);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, alpha) == 0xe8);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, allocation_size) == 0xf0);
struct cut_locator_function_entry_pc_s {
    char *name;
    u8 unknown_04[16];
};
STATIC_ASSERT(sizeof(CUT_LOCATOR_FUNCTION_ENTRY_PC) == 20);

static __declspec(noinline) void CutCameraFixPtrsPc(CUT_SCENE_LOAD_PC *scene, i32 string_delta, i32 relocation_delta,
                                                    i32 mode) {
    CUT_LOAD_CAMERA_SYSTEM *system = scene->camera_system;
    if (system != NULL) {
        system = (CUT_LOAD_CAMERA_SYSTEM *)((u32)system + string_delta);
        scene->camera_system = system;
        system->cameras = system->cameras ? (void *)((u32)system->cameras + string_delta) : NULL;
        if (relocation_delta != 0) {
            if ((scene->version & 0xff) >= 5) {
                if (system->focus_animation != NULL)
                    system->focus_animation = NuAnimData2FixPtrs(system->focus_animation, relocation_delta, mode, 0);
                system->focus_state_animation = StateAnimFixPtrs(system->focus_state_animation, relocation_delta);
            }
            if (system->animation != NULL)
                system->animation = NuAnimData2FixPtrs(system->animation, relocation_delta, mode, 0);
            system->state_animation = StateAnimFixPtrs(system->state_animation, relocation_delta);
        }
    }
}

static __declspec(noinline) void CutLocatorFixPtrsPc(CUT_SCENE_LOAD_PC *scene, i32 string_delta, i32 relocation_delta,
                                                     i32 mode) {
    CUT_LOAD_LOCATOR_SYSTEM *system = scene->locator_system;
    i32 i;
    if (system != NULL) {
        system = (CUT_LOAD_LOCATOR_SYSTEM *)((u32)system + string_delta);
        scene->locator_system = system;
        system->locators = system->locators ? (CUT_LOAD_LOCATOR *)((u32)system->locators + string_delta) : NULL;
        if (system->locators != NULL && relocation_delta != 0)
            for (i = 0; i < system->count; ++i)
                if (system->locators[i].animation != NULL)
                    system->locators[i].animation =
                        NuAnimData2FixPtrs(system->locators[i].animation, relocation_delta, mode, 0);
        system->types = system->types ? (CUT_LOAD_LOCATOR_TYPE *)((u32)system->types + string_delta) : NULL;
        if (system->types != NULL)
            for (i = 0; i < system->type_count; ++i) {
                CUT_LOAD_LOCATOR_TYPE *type = &system->types[i];
                if (type->name != NULL)
                    type->name =
                        mode == 0 ? scene->strings + (u32)type->name - 1 : (char *)((u32)type->name + string_delta);
            }
    }
}

static __declspec(noinline) void CutRigidFixPtrsPc(CUT_SCENE_LOAD_PC *scene, i32 string_delta, i32 relocation_delta,
                                                   i32 mode) {
    CUT_LOAD_RIGID_SYSTEM *system = scene->rigid_system;
    i32 i;
    if (system != NULL) {
        system = (CUT_LOAD_RIGID_SYSTEM *)((u32)system + string_delta);
        scene->rigid_system = system;
        system->rigids = system->rigids ? (CUT_LOAD_RIGID *)((u32)system->rigids + string_delta) : NULL;
        if (system->rigids != NULL)
            for (i = 0; i < system->count; ++i) {
                CUT_LOAD_RIGID *rigid = &system->rigids[i];
                if (rigid->name != NULL)
                    rigid->name =
                        mode == 0 ? scene->strings + (u32)rigid->name - 1 : (char *)((u32)rigid->name + string_delta);
                if (relocation_delta != 0) {
                    if (rigid->animation != NULL)
                        rigid->animation = NuAnimData2FixPtrs(rigid->animation, relocation_delta, mode, 0);
                    rigid->state_animation = StateAnimFixPtrs(rigid->state_animation, relocation_delta);
                }
            }
    }
}

static __declspec(noinline) void CutCharacterFixPtrsPc(CUT_SCENE_LOAD_PC *scene, i32 string_delta, i32 relocation_delta,
                                                       i32 mode) {
    CUT_LOAD_CHARACTER_SYSTEM *system = scene->character_system;
    i32 i;
    if (system != NULL) {
        system = (CUT_LOAD_CHARACTER_SYSTEM *)((u32)system + string_delta);
        scene->character_system = system;
        system->characters = system->characters ? (CUT_LOAD_CHARACTER *)((u32)system->characters + string_delta) : NULL;
        if (system->characters != NULL)
            for (i = 0; i < system->count; ++i) {
                CUT_LOAD_CHARACTER *character = &system->characters[i];
                if (character->name != NULL)
                    character->name = mode == 0 ? scene->strings + (u32)character->name - 1
                                                : (char *)((u32)character->name + string_delta);
                if (relocation_delta != 0) {
                    if (character->animation != NULL)
                        character->animation = NuAnimData2FixPtrs(character->animation, relocation_delta, mode, 0);
                    if (character->face_animation != NULL)
                        character->face_animation =
                            NuAnimData2FixPtrs(character->face_animation, relocation_delta, mode, 0);
                    if (character->extra_animation != NULL)
                        character->extra_animation =
                            NuAnimData2FixPtrs(character->extra_animation, relocation_delta, mode, 0);
                }
            }
        if ((scene->version & 0xff) >= 4) {
            scene->character_animations =
                scene->character_animations
                    ? (CUT_LOAD_CHARACTER_ANIMATION *)((u32)scene->character_animations + string_delta)
                    : NULL;
            if (system->characters != NULL && scene->character_animations != NULL)
                for (i = 0; i < system->count; ++i)
                    if (scene->character_animations[i].animation != NULL)
                        scene->character_animations[i].animation =
                            NuAnimData2FixPtrs(scene->character_animations[i].animation, relocation_delta, mode, 0);
        }
    }
}

static __declspec(noinline) void CutTriggerFixPtrsPc(CUT_SCENE_LOAD_PC *scene, i32 string_delta) {
    CUT_LOAD_TRIGGER_SYSTEM *system = scene->trigger_system;
    i32 i;
    if (system != NULL) {
        system = (CUT_LOAD_TRIGGER_SYSTEM *)((u32)system + string_delta);
        scene->trigger_system = system;
        system->events = system->events ? (CUT_LOAD_TRIGGER_EVENT *)((u32)system->events + string_delta) : NULL;
        if (system->events != NULL)
            for (i = 0; i < system->count; ++i) {
                CUT_LOAD_TRIGGER_EVENT *event = &system->events[i];
                event->name = event->name ? (char *)((u32)event->name + string_delta) : NULL;
                event->state_animation = StateAnimFixPtrs(event->state_animation, string_delta);
            }
    }
}

static __declspec(noinline) void CutSceneFixPtrsPc(CUT_SCENE_LOAD_PC *scene, i32 relocation_delta, i32 mode) {
    scene->strings = scene->strings ? (char *)((u32)scene->strings + scene->string_delta) : NULL;
    if (scene->camera_system != NULL)
        CutCameraFixPtrsPc(scene, scene->string_delta, relocation_delta, mode);
    if (scene->locator_system != NULL)
        CutLocatorFixPtrsPc(scene, scene->string_delta, relocation_delta, mode);
    if (scene->rigid_system != NULL)
        CutRigidFixPtrsPc(scene, scene->string_delta, relocation_delta, mode);
    if (scene->character_system != NULL)
        CutCharacterFixPtrsPc(scene, scene->string_delta, relocation_delta, mode);
    if (scene->trigger_system != NULL)
        CutTriggerFixPtrsPc(scene, scene->string_delta);
    scene->bounds = scene->bounds ? (CUT_LOAD_BOUNDS *)((u32)scene->bounds + scene->string_delta) : NULL;
}

static __declspec(noinline) void CutSceneRemapFocusPc(CUT_SCENE_LOAD_PC *scene, VARIPTR *buffer) {
    i32 i;
    if ((scene->version & 0xff) < 5 || scene->camera_system == NULL ||
        scene->camera_system->focus_state_animation == NULL || scene->locator_system == NULL)
        return;
    buffer->addr = (buffer->addr + 1) & ~1;
    scene->focus_indices = (u16 *)buffer->addr;
    for (i = 0; i < scene->locator_system->count; ++i)
        if ((scene->locator_system->types[scene->locator_system->locators[i].type_index].flags & 8) != 0) {
            u16 *index = (u16 *)buffer->addr;
            buffer->addr += 2;
            *index = (u16)i;
        }
}

void *__cdecl NuGCutSceneLoad(char *path, VARIPTR *buffer, VARIPTR *end, i32 no_stream_buffers) {
    char filename[1024];
    i32 capacity = end->addr - buffer->addr;
    i32 size;
    CUT_SCENE_LOAD_PC *scene;
    char *src = path, *dst = filename;
    do {
        *dst++ = *src;
    } while (*src++ != 0);
    buffer->addr = (buffer->addr + 15) & ~15;
    size = NuFileLoadBuffer(filename, buffer->void_ptr, capacity);
    if (size == 0)
        return NULL;
    scene = (CUT_SCENE_LOAD_PC *)buffer->void_ptr;
    if (scene->version >= 10) {
        scene->string_delta = (i32)scene - scene->string_delta;
        scene->relocation_delta = (i32)scene - scene->relocation_delta;
        scene->loaded_size = size;
        if ((scene->flags & 8) != 0) {
            CutSceneFixPtrsPc(scene, 0, 0);
            buffer->addr += size;
            CutSceneRemapFocusPc(scene, buffer);
        } else {
            CutSceneFixPtrsPc(scene, scene->relocation_delta, 0);
            if ((scene->flags & 1) == 0 || no_stream_buffers != 0) {
                buffer->addr += size;
                CutSceneRemapFocusPc(scene, buffer);
            } else {
                buffer->addr = (buffer->addr + scene->stream_buffer_offset + 15) & ~15;
                scene->stream_buffer_0 = buffer->void_ptr;
                buffer->addr += scene->stream_buffer_size + 0x400;
                if (CutSceneSingleStreamBufferPc == 0) {
                    buffer->addr = (buffer->addr + 15) & ~15;
                    scene->stream_buffer_1 = buffer->void_ptr;
                    buffer->addr += scene->stream_buffer_size + 0x400;
                }
                scene->filename = (char *)buffer->void_ptr;
                buffer->addr += NuStrLen(path) + 4;
                NuStrCpy(scene->filename, path);
                CutSceneRemapFocusPc(scene, buffer);
            }
        }
    } else {
        void *first_animation;
        i32 relocation_delta;
        if (scene->version > 2)
            CutSceneLoadDiagnosticPc(path, scene->version);
        scene->string_delta = (i32)scene - scene->string_delta;
        relocation_delta = scene->string_delta;
        if (scene->version >= 2) {
            scene->relocation_delta = (i32)scene - scene->relocation_delta;
            relocation_delta = scene->relocation_delta;
        }
        if (scene->version >= 2 && (scene->flags & 8) != 0) {
            CutSceneFixPtrsPc(scene, 0, 0);
            buffer->addr += size;
        } else {
            CutSceneFixPtrsPc(scene, relocation_delta, 0);
            if (scene->version >= 2 && (scene->flags & 1) != 0 && no_stream_buffers == 0) {
                first_animation = NULL;
                if (scene->camera_system != NULL && scene->camera_system->animation != NULL)
                    first_animation = scene->camera_system->animation;
                else if (scene->rigid_system != NULL && scene->rigid_system->rigids->animation != NULL)
                    first_animation = scene->rigid_system->rigids->animation;
                else if (scene->character_system != NULL) {
                    if (scene->character_system->characters->animation != NULL)
                        first_animation = scene->character_system->characters->animation;
                    else if (scene->character_system->characters->face_animation != NULL)
                        first_animation = scene->character_system->characters->face_animation;
                }
                if (scene->stream_buffer_offset != 0)
                    first_animation = (void *)(buffer->addr + scene->stream_buffer_offset);
                buffer->addr = ((u32)first_animation + 15) & ~15;
                scene->stream_buffer_0 = buffer->void_ptr;
                buffer->addr += scene->stream_buffer_size + 0x400;
                if (CutSceneSingleStreamBufferPc == 0) {
                    buffer->addr = (buffer->addr + 15) & ~15;
                    scene->stream_buffer_1 = buffer->void_ptr;
                    buffer->addr += scene->stream_buffer_size + 0x400;
                }
                scene->filename = (char *)buffer->void_ptr;
                buffer->addr += NuStrLen(path) + 4;
                NuStrCpy(scene->filename, path);
            } else
                buffer->addr += size;
        }
    }
    if (buffer->addr > end->addr)
        CutSceneLoadDiagnosticPc(path, buffer->addr - end->addr);
    return scene;
}

static __declspec(noinline) void CutSceneRigidFixUpPc(CUT_SCENE_LOAD_PC *scene, NUGSCN *gscene) {
    CUT_LOAD_RIGID_SYSTEM *system = scene->rigid_system;
    CUT_LOAD_LOCATOR_SYSTEM *locators = scene->locator_system;
    NUSPECIALHANDLE handle;
    i32 i;
    if (gscene != NULL)
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_RIGID *rigid = &system->rigids[i];
            if (NuSpecialFind(gscene, &handle, rigid->name, 1) != 0) {
                rigid->flags |= 4;
                rigid->scene = handle.scene;
                rigid->special = handle.display_special ? (void *)handle.display_special : (void *)handle.special;
            } else
                CutSceneMissingObjectDiagnosticPc(rigid->name);
            if (rigid->uses_locator != 0 && rigid->locator.index < 255) {
                rigid->locator_index = (u8)rigid->locator.index;
                rigid->locator.pointer = &locators->locators[rigid->locator.index];
            } else
                rigid->locator_index = 255;
        }
}

static __declspec(noinline) i32 CutFindLocatorFunctionPc(char *name) {
    i32 i;
    if (CutSceneLocatorFunctionsPc != NULL)
        for (i = 0; CutSceneLocatorFunctionsPc[i].name != NULL; ++i)
            if (NuStrICmp(name, CutSceneLocatorFunctionsPc[i].name) == 0)
                return i;
    return -1;
}

static __declspec(noinline) void CutSceneLocatorFixUpPc(CUT_SCENE_LOAD_PC *scene, i32 page) {
    CUT_LOAD_LOCATOR_SYSTEM *system = scene->locator_system;
    i32 i;
    for (i = 0; i < system->type_count; ++i) {
        CUT_LOAD_LOCATOR_TYPE *type = &system->types[i];
        if ((type->flags & 1) != 0) {
            if (CutSceneLookupAllDebrisPagesPc != 0)
                type->function_index = (u16)LookupDebrisEffectPage(type->name, (i8)page);
            else
                type->function_index = (u16)LookupDebrisEffectPageOnly(type->name, (i8)page);
        } else if ((type->flags & 2) != 0)
            type->function_index = (u16)CutFindLocatorFunctionPc(type->name);
        else if ((type->flags & 16) != 0) {
            if (LookupLocatorVfxFn != NULL) {
                char *underscore = NuStrRChr(type->name, '_');
                if (underscore != NULL && underscore[1] >= '0' && underscore[1] <= '9')
                    *underscore = 0;
                type->function_index = (u16)LookupLocatorVfxFn(type->name);
            } else
                type->function_index = 65535;
        } else if ((type->flags & 4) != 0 && NuCutSceneRequestSFX != NULL) {
            type->function_index = (u16)NuCutSceneRequestSFX(type->name);
            if (type->function_index != 65535)
                scene->flags |= 4;
        }
        if (type->function_index == 65535)
            CutSceneMissingObjectDiagnosticPc(type->name);
    }
}

static __declspec(noinline) void CutSceneTriggerFixUpPc(CUT_SCENE_LOAD_PC *scene, CUT_LOAD_TRIGGER_OWNER *owner) {
    CUT_LOAD_TRIGGER_SYSTEM *system = scene->trigger_system;
    i32 i, j;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_TRIGGER_EVENT *event = &system->events[i];
        for (j = 0; j < owner->count; ++j) {
            event->index = 65535;
            if (NuStrICmp(event->name, owner->entries[j].name) == 0) {
                event->index = (u16)j;
                break;
            }
        }
        if (event->index == 65535)
            CutSceneLoadDiagnosticPc(event->name);
    }
}

void __cdecl NuGCutSceneFixUp(void *asset, NUGSCN *gscene, void *extra_scene, i32 page) {
    CUT_SCENE_LOAD_PC *scene = (CUT_SCENE_LOAD_PC *)asset;
    if (scene != NULL) {
        if (scene->version >= 2) {
            scene->scene = gscene;
            scene->extra_scene = extra_scene;
        }
        if (scene->rigid_system != NULL)
            CutSceneRigidFixUpPc(scene, gscene);
        if (scene->character_system != NULL && NuCutSceneFindCharacters != NULL)
            NuCutSceneFindCharacters(scene);
        if (scene->locator_system != NULL)
            CutSceneLocatorFixUpPc(scene, page);
        if (scene->trigger_system != NULL && extra_scene != NULL)
            CutSceneTriggerFixUpPc(scene, (CUT_LOAD_TRIGGER_OWNER *)extra_scene);
    }
}

static __declspec(noinline) void CutSceneRigidFixUpExtraPc(CUT_SCENE_LOAD_PC *scene, NUGSCN *gscene) {
    CUT_LOAD_RIGID_SYSTEM *system = scene->rigid_system;
    NUSPECIALHANDLE handle;
    i32 i;
    if (gscene != NULL)
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_RIGID *rigid = &system->rigids[i];
            if ((rigid->flags & 4) == 0) {
                if (NuSpecialFind(gscene, &handle, rigid->name, 1) != 0) {
                    rigid->flags |= 4;
                    rigid->scene = handle.scene;
                    rigid->special = handle.display_special ? (void *)handle.display_special : (void *)handle.special;
                } else
                    CutSceneMissingObjectDiagnosticPc(rigid->name);
            }
        }
}

void __cdecl NuGCutSceneFixUpExtra(void *asset, NUGSCN *gscene) {
    CUT_SCENE_LOAD_PC *scene = (CUT_SCENE_LOAD_PC *)asset;
    if (scene != NULL && scene->rigid_system != NULL)
        CutSceneRigidFixUpExtraPc(scene, gscene);
}

static __declspec(noinline) CUT_LOAD_CAMERA_INSTANCE *CutCameraInstanceCreatePc(CUT_LOAD_CAMERA_SYSTEM *system,
                                                                                VARIPTR *buffer) {
    CUT_LOAD_CAMERA_INSTANCE *instance = NULL;
    if (system != NULL && system->count != 0 && buffer != NULL) {
        buffer->addr = (buffer->addr + 15) & ~15;
        instance = (CUT_LOAD_CAMERA_INSTANCE *)buffer->void_ptr;
        buffer->addr += 16;
        memset(instance, 0, 16);
        buffer->addr = (buffer->addr + 15) & ~15;
        instance->states = (u32 *)buffer->void_ptr;
        buffer->addr += system->count * 4;
        memset(instance->states, 0, system->count * 4);
    }
    return instance;
}

static __declspec(noinline) CUT_LOAD_RIGID_INSTANCE_SYSTEM *CutRigidInstanceCreatePc(CUT_SCENE_LOAD_PC *scene,
                                                                                     VARIPTR *buffer, NUGSCN *gscene) {
    CUT_LOAD_RIGID_SYSTEM *system = scene->rigid_system;
    CUT_LOAD_RIGID_INSTANCE_SYSTEM *instance = NULL;
    i32 i;
    if (system != NULL && system->count != 0 && buffer != NULL) {
        buffer->addr = (buffer->addr + 15) & ~15;
        instance = (CUT_LOAD_RIGID_INSTANCE_SYSTEM *)buffer->void_ptr;
        buffer->addr += 4;
        instance->rigids = NULL;
        buffer->addr = (buffer->addr + 15) & ~15;
        instance->rigids = (CUT_LOAD_RIGID_INSTANCE *)buffer->void_ptr;
        buffer->addr += system->count * 16;
        memset(instance->rigids, 0, system->count * 16);
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_RIGID *rigid = &system->rigids[i];
            CUT_LOAD_RIGID_INSTANCE *target = &instance->rigids[i];
            if (gscene != NULL && gscene != rigid->scene) {
                if (rigid->special != NULL) {
                    target->special.scene = gscene;
                    if (gscene->display_list != NULL) {
                        i32 index = (i32)(((i32)rigid->special - (i32)rigid->scene->display_list->specials) / 0xd0);
                        target->special.special = NULL;
                        target->special.display_special = (NUMTX *)&gscene->display_list->specials[index];
                    } else {
                        i32 index = (i32)(((i32)rigid->special - (i32)rigid->scene->specials) / 0x4c);
                        target->special.display_special = NULL;
                        target->special.special = (NUMTX *)&gscene->specials[index];
                    }
                    if ((rigid->flags & 2) != 0)
                        target->visible = rigid->flags & 1;
                } else
                    CutSceneMissingObjectDiagnosticPc(rigid->name);
            } else {
                target->special.scene = rigid->scene;
                if (rigid->scene != NULL && rigid->scene->display_list != NULL) {
                    target->special.special = NULL;
                    target->special.display_special = (NUMTX *)rigid->special;
                } else {
                    target->special.special = (NUMTX *)rigid->special;
                    target->special.display_special = NULL;
                }
            }
        }
    }
    return instance;
}

static __declspec(noinline) CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *CutCharacterInstanceCreatePc(CUT_SCENE_LOAD_PC *scene,
                                                                                             VARIPTR *buffer) {
    CUT_LOAD_CHARACTER_SYSTEM *system = scene->character_system;
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *instance = NULL;
    i32 i;
    if (system != NULL && system->count != 0 && buffer != NULL) {
        buffer->addr = (buffer->addr + 15) & ~15;
        instance = (CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *)buffer->void_ptr;
        buffer->addr += 4;
        instance->characters = NULL;
        buffer->addr = (buffer->addr + 15) & ~15;
        instance->characters = (CUT_LOAD_CHARACTER_INSTANCE *)buffer->void_ptr;
        buffer->addr += system->count * 24;
        memset(instance->characters, 0, system->count * 24);
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_CHARACTER *character = &system->characters[i];
            CUT_LOAD_CHARACTER_INSTANCE *target = &instance->characters[i];
            target->animation_b = 255;
            target->animation_a = 255;
            if ((character->flags & 2) != 0) {
                if (NuCutSceneCharacterCreateData != NULL)
                    NuCutSceneCharacterCreateData(character, target, buffer);
            } else
                target->model = character->model;
        }
    }
    return instance;
}

static __declspec(noinline) CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *
CutLocatorInstanceCreatePc(CUT_LOAD_LOCATOR_SYSTEM *system, VARIPTR *buffer) {
    CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *instance = NULL;
    i32 i;
    if (system != NULL && system->count != 0 && buffer != NULL) {
        buffer->addr = (buffer->addr + 15) & ~15;
        instance = (CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *)buffer->void_ptr;
        buffer->addr += 4;
        instance->locators = NULL;
        buffer->addr = (buffer->addr + 15) & ~15;
        instance->locators = (CUT_LOAD_LOCATOR_INSTANCE *)buffer->void_ptr;
        buffer->addr += system->count * 8;
        memset(instance->locators, 0, system->count * 8);
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_LOCATOR *locator = &system->locators[i];
            CUT_LOAD_LOCATOR_INSTANCE *target = &instance->locators[i];
            u8 flags = system->types[locator->type_index].flags;
            if ((flags & 1) != 0) {
                if ((locator->flags & 0x20) != 0)
                    target->effect.index = -1;
            } else if ((flags & 2) != 0) {
                /* This type has no per-instance allocation. */
            } else if ((flags & 16) != 0)
                target->effect.index = -1;
            else if ((flags & 8) != 0) {
                buffer->addr = (buffer->addr + 3) & ~3;
                target->effect.focus = (CUT_LOAD_FOCUS_TARGET *)buffer->void_ptr;
                buffer->addr += 24;
                memset(target->effect.focus, 0, 24);
                if (locator->tracks_self != 0)
                    target->effect.focus->self = locator;
            } else if ((flags & 4) != 0) {
                buffer->addr = (buffer->addr + 15) & ~15;
                target->effect.sound = (CUT_LOAD_SOUND_STATE *)buffer->void_ptr;
                buffer->addr += 16;
                memset(target->effect.sound, 0, 16);
            }
        }
    }
    return instance;
}

static __declspec(noinline) CUT_LOAD_TRIGGER_INSTANCE *
CutTriggerInstanceCreatePc(VARIPTR *buffer, CUT_SCENE_LOAD_PC *scene, CUT_LOAD_TRIGGER_OWNER *owner) {
    i32 count = scene->trigger_system->count;
    CUT_LOAD_TRIGGER_INSTANCE *instance;
    buffer->addr = (buffer->addr + 15) & ~15;
    instance = (CUT_LOAD_TRIGGER_INSTANCE *)buffer->void_ptr;
    buffer->addr += 8;
    instance->states = NULL;
    instance->owner = owner;
    instance->states = (u32 *)buffer->void_ptr;
    buffer->addr += count * 4;
    memset(instance->states, 0, count * 4);
    return instance;
}

static __declspec(noinline) void CutSceneCreateContentsPc(VARIPTR *buffer, char *name, CUT_INSTANCE_LOAD_PC *instance,
                                                          CUT_SCENE_LOAD_PC *scene, NUGSCN *gscene,
                                                          CUT_LOAD_TRIGGER_OWNER *extra_scene, VARIPTR *end) {
    if (scene != NULL) {
        instance->scene = scene;
        instance->saved_scene = scene;
        if (name != NULL)
            sprintf(instance->name, name);
        if (scene->camera_system != NULL)
            instance->camera_instance = CutCameraInstanceCreatePc(scene->camera_system, buffer);
        if (scene->rigid_system != NULL)
            instance->rigid_instance = CutRigidInstanceCreatePc(scene, buffer, gscene);
        if (scene->character_system != NULL)
            instance->character_instance = CutCharacterInstanceCreatePc(scene, buffer);
        if (scene->locator_system != NULL)
            instance->locator_instance = CutLocatorInstanceCreatePc(scene->locator_system, buffer);
        if (scene->trigger_system != NULL && extra_scene != NULL)
            instance->trigger_instance = CutTriggerInstanceCreatePc(buffer, scene, extra_scene);
        if (instance->scene->bounds != NULL) {
            instance->center.x = (f32)((instance->scene->bounds->min.x + instance->scene->bounds->max.x) * 0.5);
            instance->center.y = (f32)((instance->scene->bounds->min.y + instance->scene->bounds->max.y) * 0.5);
            instance->center.z = (f32)((instance->scene->bounds->min.z + instance->scene->bounds->max.z) * 0.5);
        } else
            instance->center.x = instance->center.y = instance->center.z = 0.0f;
        instance->rate = 1.0f;
        if (end != NULL && buffer->addr > end->addr)
            CutSceneLoadDiagnosticPc(buffer);
    }
}

INSTNUGCUTSCENE *__cdecl instNuGCutSceneCreate(void *asset, NUGSCN *gscene, void *extra_scene, char *name,
                                               VARIPTR *buffer, i32 background) {
    CUT_SCENE_LOAD_PC *scene = (CUT_SCENE_LOAD_PC *)asset;
    CUT_INSTANCE_LOAD_PC *instance = NULL;
    if (scene != NULL) {
        buffer->addr = (buffer->addr + 15) & ~15;
        instance = (CUT_INSTANCE_LOAD_PC *)buffer->void_ptr;
        buffer->addr += 0xf8;
        if (instance == NULL)
            return NULL;
        memset(instance, 0, 0xf8);
        instance->alpha = 1.0f;
        instance->scene_owner = gscene;
        instance->extra_scene_owner = (CUT_LOAD_TRIGGER_OWNER *)extra_scene;
        instance->stream_index = 0;
        if (scene->version >= 2 && (scene->flags & 1) != 0) {
            instance->stream_buffer_0 = scene->stream_buffer_0;
            instance->stream_buffer_1 = scene->stream_buffer_1;
            instance->pending_stream = scene->stream_buffer_1;
            instance->flags_88 &= ~0x10000000;
            instance->stream_index = 0;
        }
        if (scene->version >= 2 && (scene->flags & 9) != 0)
            instance->flags_88 |= 0x20000000;
        else
            instance->flags_88 &= ~0x20000000;
        CutSceneCreateContentsPc(buffer, name, instance, scene, gscene, (CUT_LOAD_TRIGGER_OWNER *)extra_scene, NULL);
        NuThreadDisableThreadSwap();
        if (background != 0) {
            instance->next = CutSceneBackgroundInstancesPc;
            if (instance->next != NULL)
                ((CUT_INSTANCE_LOAD_PC *)instance->next)->previous = (INSTNUGCUTSCENE *)instance;
            CutSceneBackgroundInstancesPc = (INSTNUGCUTSCENE *)instance;
        } else {
            instance->next = active_cutscene_instances;
            if (instance->next != NULL)
                ((CUT_INSTANCE_LOAD_PC *)instance->next)->previous = (INSTNUGCUTSCENE *)instance;
            active_cutscene_instances = (INSTNUGCUTSCENE *)instance;
        }
        instance->allocation_size = buffer->addr - (u32)instance;
        NuThreadEnableThreadSwap();
    }
    return (INSTNUGCUTSCENE *)instance;
}
