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
    CUT_LOAD_LOCATOR *locator;
    u32 unknown_58;
    u8 flags, unknown_5d, uses_locator, unknown_5f, locator_index, unknown_61[3];
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
typedef struct cut_load_camera_pc_s {
    NUMTX matrix;
    u8 flags, unknown_41, animation_node, focus_flags;
    u8 unknown_44[12];
} CUT_LOAD_CAMERA_PC;
STATIC_ASSERT(sizeof(CUT_LOAD_CAMERA_PC) == 0x50);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_PC, animation_node) == 0x42);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_PC, focus_flags) == 0x43);
typedef struct cut_load_camera_system_s {
    i32 count;
    CUT_LOAD_CAMERA_PC *cameras;
    void *animation;
    STATE_ANIM_PC *state_animation;
    i8 starting_camera;
    i8 target_camera_map[11];
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

typedef struct cut_load_camera_target_pc_s {
    NUVEC *target;
    f32 start_frame, duration;
    i8 target_index;
    u8 unknown_0d[3];
} CUT_LOAD_CAMERA_TARGET_PC;
STATIC_ASSERT(sizeof(CUT_LOAD_CAMERA_TARGET_PC) == 16);
typedef struct cut_load_camera_state_pc_s {
    u8 flags, event_index, unknown_02[2];
} CUT_LOAD_CAMERA_STATE_PC;
STATIC_ASSERT(sizeof(CUT_LOAD_CAMERA_STATE_PC) == 4);
typedef struct cut_load_camera_instance_s {
    CUT_LOAD_CAMERA_TARGET_PC *targets;
    CUT_LOAD_CAMERA_STATE_PC *states;
    u8 state_index;
    i8 camera_index;
    u8 next_target_index, unknown_0b, target_count, focus_state_index;
    i8 focus_index;
    u8 unknown_0f;
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
    f32 frame;
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
typedef struct cut_load_trigger_flag_pc_s {
    u16 unknown_00;
    u8 flags, unknown_03;
} CUT_LOAD_TRIGGER_FLAG_PC;
STATIC_ASSERT(sizeof(CUT_LOAD_TRIGGER_FLAG_PC) == 4);
typedef struct cut_load_trigger_owner_s {
    u8 unknown_00[8];
    i32 count;
    union {
        CUT_LOAD_TRIGGER_ENTRY *entries;
        CUT_LOAD_TRIGGER_FLAG_PC *trigger_states;
    };
} CUT_LOAD_TRIGGER_OWNER;
typedef struct cut_load_trigger_state_pc_s {
    u8 state_index, unknown_01[3];
} CUT_LOAD_TRIGGER_STATE_PC;
STATIC_ASSERT(sizeof(CUT_LOAD_TRIGGER_STATE_PC) == 4);
typedef struct cut_load_trigger_instance_s {
    CUT_LOAD_TRIGGER_OWNER *owner;
    CUT_LOAD_TRIGGER_STATE_PC *states;
} CUT_LOAD_TRIGGER_INSTANCE;
typedef struct cut_instance_load_pc_s {
    INSTNUGCUTSCENE *next, *previous;
    char name[16];
    NUMTX matrix;
    CUT_SCENE_LOAD_PC *scene, *saved_scene;
    NUVEC center;
    NUVEC bounds_min, bounds_max;
    f32 radius_squared;
    u32 flags_88, flags_8c;
    f32 current_frame, render_frame, rate;
    CUT_LOAD_CAMERA_INSTANCE *camera_instance;
    CUT_LOAD_RIGID_INSTANCE_SYSTEM *rigid_instance;
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *character_instance;
    CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *locator_instance;
    CUT_LOAD_TRIGGER_INSTANCE *trigger_instance;
    INSTNUGCUTSCENE *chained_instance;
    void(__cdecl *end_callback)(INSTNUGCUTSCENE *);
    u32 unknown_b8;
    void *stream_buffer_0, *stream_buffer_1;
    NUGSCN *scene_owner;
    CUT_LOAD_TRIGGER_OWNER *extra_scene_owner;
    u8 unknown_cc[12];
    u16 skip_countdown;
    u8 unknown_da[5];
    u8 stream_index;
    f32 accumulated_duration, elapsed, alpha;
    void *pending_stream;
    u32 allocation_size;
    INSTNUGCUTSCENE *queued_stream_instance;
} CUT_INSTANCE_LOAD_PC;
STATIC_ASSERT(sizeof(CUT_INSTANCE_LOAD_PC) == 0xf8);
STATIC_ASSERT(offsetof(CUT_LOAD_RIGID_INSTANCE, state_index) == 0xc);
STATIC_ASSERT(offsetof(CUT_LOAD_RIGID_INSTANCE, visible) == 0xd);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_SYSTEM, count) == 0x0);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_SYSTEM, starting_camera) == 0x10);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, rate) == 0x98);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, rigid_instance) == 0xa0);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, character_instance) == 0xa4);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, matrix) == 0x18);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, flags_8c) == 0x8c);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, current_frame) == 0x90);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, render_frame) == 0x94);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, saved_scene) == 0x5c);
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
    u32 unknown_04;
    void(__cdecl *reset)(INSTNUGCUTSCENE *, CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *, CUT_LOAD_LOCATOR *);
    u8 unknown_0c[8];
};
STATIC_ASSERT(offsetof(CUT_LOCATOR_FUNCTION_ENTRY_PC, reset) == 8);
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
    u32 i;
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
        instance->states = (CUT_LOAD_CAMERA_STATE_PC *)buffer->void_ptr;
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
    instance->states = (CUT_LOAD_TRIGGER_STATE_PC *)buffer->void_ptr;
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

typedef void(__cdecl *CUT_DRAW_RENDER_CALLBACK_PC)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32);
typedef void(__cdecl *CUT_DRAW_EVAL_CALLBACK_PC)(INSTNUGCUTSCENE *, void *, void *, void *, f32);
typedef void(__cdecl *CUT_DRAW_RIGID_CALLBACK_PC)(void *, NUSPECIALHANDLE *, NUMTX *);
extern CUT_DRAW_RENDER_CALLBACK_PC NuCutSceneCharacterRender;
extern void(__cdecl *NuCutSceneResetCharacters)(INSTNUGCUTSCENE *);
extern CUT_DRAW_RIGID_CALLBACK_PC NuCutSceneRigidPostRender;
void __cdecl NuGCutSceneSysInit(CUT_LOCATOR_FUNCTION_ENTRY_PC *functions) {
    CutSceneBackgroundInstancesPc = NULL;
    active_cutscene_instances = NULL;
    CutSceneLocatorFunctionsPc = functions;
}
void __cdecl NuSetCutSceneCharacterRenderFn(CUT_DRAW_RENDER_CALLBACK_PC callback) {
    NuCutSceneCharacterRender = callback;
}
void __cdecl NuSetCutSceneCharacterEvalFn(CUT_DRAW_EVAL_CALLBACK_PC callback) {
    NuCutSceneCharacterEval = callback;
}
void __cdecl NuSetCutSceneFindCharactersFn(void(__cdecl *callback)(CUT_SCENE_LOAD_PC *)) {
    NuCutSceneFindCharacters = callback;
}
void __cdecl NuSetCutSceneResetCharactersFn(void(__cdecl *callback)(INSTNUGCUTSCENE *)) {
    NuCutSceneResetCharacters = callback;
}
void __cdecl NuSetCutSceneCharacterCreateDataFn(void(__cdecl *callback)(CUT_LOAD_CHARACTER *,
                                                                        CUT_LOAD_CHARACTER_INSTANCE *, VARIPTR *)) {
    NuCutSceneCharacterCreateData = callback;
}
void __cdecl NuSetCutSceneRigidPostRenderFn(CUT_DRAW_RIGID_CALLBACK_PC callback) {
    NuCutSceneRigidPostRender = callback;
}

void __cdecl instNuGCutSceneEndFirstFrame(INSTNUGCUTSCENE *owner) {
    CUT_INSTANCE_LIFECYCLE_PC *instance = (CUT_INSTANCE_LIFECYCLE_PC *)owner;
    CUT_SCENE_LIFECYCLE_PC *scene = instance->scene;
    instance->current_frame = 1.0f;
    instance->render_frame = 1.0f;
    instance->flags_88 &= ~0x1002U;
    instance->flags_8c &= ~0x40U;
    if ((instance->flags_88 & 0x40000) != 0) {
        if (scene->rigid_system != NULL)
            CutSceneEndRigidsPC(owner, scene->duration - 1.0f);
        if (instance->character_instance != NULL)
            CutSceneEndCharactersPC(owner, instance->scene->duration - instance->current_frame);
    } else {
        if (scene->rigid_system != NULL)
            CutSceneEndRigidsPC(owner, 1.0f);
        if (instance->character_instance != NULL)
            CutSceneEndCharactersPC(owner, instance->current_frame);
    }
    if (instance->locator_instance != NULL)
        CutSceneEndLocatorsPC(instance->locator_instance, scene->locator_system);
    if (instance->camera_instance != NULL && instance->camera_instance->camera_index >= 0)
        CutSceneCameraCTRL = 0;
}

extern void(__cdecl *CutSceneRigidRenderCallbackPc)(CUT_LOAD_RIGID *, NUMTX *);
extern i32 CutSceneRenderRigidsPc, CutSceneRenderCharactersPc, CutSceneRenderLocatorsPc;

static void CutSceneRigidSysRenderPc(INSTNUGCUTSCENE *owner, f32 frame, i32 paused) {
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    CUT_SCENE_LOAD_PC *scene = instance->scene;
    CUT_LOAD_RIGID_SYSTEM *system = scene->rigid_system;
    CUT_LOAD_RIGID_INSTANCE_SYSTEM *objects = instance->rigid_instance;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_RIGID *rigid = &system->rigids[i];
        CUT_LOAD_RIGID_INSTANCE *object = &objects->rigids[i];
        NUMTX matrix;
        u8 visible;
        if ((rigid->flags & 6) != 6)
            continue;
        if (rigid->state_animation != NULL &&
            StateAnimEvaluate(rigid->state_animation, &object->state_index, &visible, frame) != 0)
            object->visible = visible != 0;
        if (object->visible == 0)
            continue;
        NuGCutRigidCalcMtx((CUT_RIGID_GENERIC_PC *)rigid, frame, &matrix);
        if ((instance->flags_88 & 0x80) != 0)
            NuMtxMul(&matrix, &matrix, &instance->matrix);
        if (instance->alpha == 1.0f)
            NuSpecialDrawAt(&object->special, &matrix);
        else
            NuSpecialDrawAtAlpha(&object->special, &matrix, instance->alpha);
        if (NuCutSceneRigidPostRender != NULL && (rigid->flags & 0x18) != 0)
            NuCutSceneRigidPostRender(rigid, &object->special, &matrix);
        if (CutSceneRigidRenderCallbackPc != NULL)
            CutSceneRigidRenderCallbackPc(rigid, &matrix);
        if (rigid->locator_index != 0xff) {
            CUT_LOAD_LOCATOR_SYSTEM *locators = scene->locator_system;
            CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *states = instance->locator_instance;
            i32 j;
            for (j = 0; j < rigid->uses_locator; ++j) {
                i32 index = rigid->locator_index + j;
                instNuGCutLocatorUpdate(owner, locators, &states->locators[index], &locators->locators[index], frame,
                                        &matrix, paused);
            }
        }
    }
}

static void CutSceneLocatorSysUpdatePc(INSTNUGCUTSCENE *owner, f32 frame, i32 paused) {
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    CUT_LOAD_LOCATOR_SYSTEM *system = instance->scene->locator_system;
    CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *states = instance->locator_instance;
    u32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_LOCATOR *locator = &system->locators[i];
        if ((locator->flags & 3) == 0)
            instNuGCutLocatorUpdate(owner, system, &states->locators[i], locator, frame,
                                    (instance->flags_88 & 0x80) != 0 ? &instance->matrix : NULL, paused);
    }
}

static void CutSceneCharacterSysRenderPc(INSTNUGCUTSCENE *owner, f32 frame, i32 paused) {
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    CUT_SCENE_LOAD_PC *scene = instance->scene;
    CUT_LOAD_CHARACTER_SYSTEM *system = scene->character_system;
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *states = instance->character_instance;
    i32 i;
    if (NuCutSceneCharacterRender != NULL) {
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_CHARACTER_INSTANCE *state = &states->characters[i];
            if (state->model != NULL)
                NuCutSceneCharacterRender(owner, scene, state, &system->characters[i], frame, paused);
        }
    }
}

static void CutSceneInstanceRenderPc(CUT_INSTANCE_LOAD_PC *instance, i32 paused) {
    INSTNUGCUTSCENE *owner = (INSTNUGCUTSCENE *)instance;
    if ((instance->flags_88 & 0x800) != 0 || (instance->flags_88 & 2) == 0 || (instance->flags_88 & 0x400) == 0)
        return;
    if ((instance->flags_88 & 0x40000) != 0) {
        if (CutSceneRenderRigidsPc != 0 && instance->rigid_instance != NULL)
            CutSceneRigidSysRenderPc(owner, instance->scene->duration - instance->render_frame, paused);
        if (CutSceneRenderCharactersPc != 0 && instance->character_instance != NULL)
            CutSceneCharacterSysRenderPc(owner, instance->scene->duration - instance->render_frame, paused);
        if ((instance->flags_8c & 0x40) != 0 && instance->current_frame == instance->scene->duration - 1.0f)
            return;
        if ((instance->flags_88 & 0x40000000) != 0)
            return;
        if (CutSceneRenderLocatorsPc != 0 && instance->locator_instance != NULL)
            CutSceneLocatorSysUpdatePc(owner, instance->scene->duration - instance->render_frame, paused);
    } else {
        if (CutSceneRenderRigidsPc != 0 && instance->rigid_instance != NULL)
            CutSceneRigidSysRenderPc(owner, instance->render_frame, paused);
        if (CutSceneRenderCharactersPc != 0 && instance->character_instance != NULL)
            CutSceneCharacterSysRenderPc(owner, instance->render_frame, paused);
        if ((instance->flags_8c & 0x40) != 0 && instance->current_frame == instance->scene->duration - 1.0f)
            return;
        if ((instance->flags_88 & 0x40000000) != 0)
            return;
        if (CutSceneRenderLocatorsPc != 0 && instance->locator_instance != NULL)
            CutSceneLocatorSysUpdatePc(owner, instance->render_frame, paused);
    }
}

void __cdecl NuGCutSceneSysRender(i32 paused) {
    INSTNUGCUTSCENE *owner;
    for (owner = active_cutscene_instances; owner != NULL; owner = ((CUT_INSTANCE_LOAD_PC *)owner)->next)
        CutSceneInstanceRenderPc((CUT_INSTANCE_LOAD_PC *)owner, paused);
}

extern NUMTX CutSceneCameraMatrixPc;
extern NUVEC CutSceneFocusLocationPc;
extern u8 CutSceneUseFocusLocationPc, CutSceneCameraMatrixSetPc;
extern f32 CutSceneFocusDistancePc, CutSceneFStopPc;
extern i32 CutSceneCameraChangedPc, CutSceneCameraDofModePc;

static void CutSceneCameraSysUpdatePc(INSTNUGCUTSCENE *owner, f32 frame) {
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    CUT_LOAD_CAMERA_SYSTEM *system = instance->scene->camera_system;
    CUT_LOAD_CAMERA_INSTANCE *state = instance->camera_instance;
    CUT_LOAD_CAMERA_PC *camera;
    CUT_LOAD_CAMERA_STATE_PC *camera_state;
    CUT_LOAD_CAMERA_TARGET_PC *target;
    u8 value;
    i32 index;
    f32 duration, blend;
    i32 reverse;
    NUMTX look_at;
    NUVEC translation;
    NUQUAT target_rotation, camera_rotation, blended_rotation;
    CutSceneCameraFocalLengthPc = 0.0f;
    CutSceneFocusDistancePc = 0.0f;
    CutSceneUseFocusLocationPc = 0;
    CutSceneFStopPc = 0.0f;
    if ((instance->scene->version & 0xff) >= 5 && system->focus_state_animation != NULL &&
        StateAnimEvaluate(system->focus_state_animation, &state->focus_state_index, &value, frame) != 0)
        state->focus_index = value == 0xff ? -1 : (i8)instance->scene->focus_indices[value];
    if (system->state_animation != NULL) {
        if (StateAnimEvaluate(system->state_animation, &state->state_index, &value, frame) != 0) {
            state->camera_index = (i8)value;
            instance->flags_8c |= 0x1000;
            CutSceneCameraChangedPc = 1;
        }
        if (state->state_index < system->state_animation->count) {
            f32 next_frame = system->state_animation->times[state->state_index];
            if (next_frame - frame < 1.0f &&
                system->state_animation->values[state->state_index] != state->camera_index) {
                f32 render_frame = next_frame - 1.0f;
                if ((instance->flags_88 & 0x40000) != 0)
                    render_frame = instance->scene->duration - render_frame;
                instance->render_frame = render_frame;
                if (instance->render_frame < 1.0f)
                    instance->render_frame = 1.0f;
            }
        }
    }
    while (state->next_target_index < state->target_count &&
           frame >= state->targets[state->next_target_index].start_frame) {
        index = system->target_camera_map[state->targets[state->next_target_index].target_index];
        state->states[index].flags |= 2;
        state->states[index].event_index = state->next_target_index++;
    }
    while (state->next_target_index != 0 && frame < state->targets[state->next_target_index - 1].start_frame) {
        --state->next_target_index;
        index = system->target_camera_map[state->targets[state->next_target_index].target_index];
        state->states[index].flags |= 2;
        state->states[index].event_index = state->next_target_index;
    }
    if (state->camera_index < 0) {
        CutSceneCameraCTRL = 0;
        return;
    }
    CutSceneCameraCTRL = 1;
    CutSceneCameraDofModePc = 2;
    index = state->camera_index;
    if ((u32)index >= (u32)system->count)
        ((void(__cdecl *)(void))CutSceneLoadDiagnosticPc)();
    camera = &system->cameras[index];
    camera_state = &state->states[index];
    if ((camera->flags & 1) == 0 || camera->animation_node >= NuAnimNumNodes((NUANIMDATA2_PC *)system->animation))
        CutSceneCameraMatrixPc = camera->matrix;
    else {
        ANI3_HEADER_PC *animation = (ANI3_HEADER_PC *)system->animation;
        ANI3_HEADER_PC *focus = (ANI3_HEADER_PC *)system->focus_animation;
        if ((instance->scene->version & 0xff) >= 5 && focus != NULL && (i32)focus->magic >= 0x414e4934 &&
            (i32)focus->magic <= 0x414e4935) {
            f32 *values =
                NuAnimCurveExtractAllNodeCurves_3(focus, camera->animation_node, instance->render_frame, NULL);
            CutSceneFStopPc = values[2];
            CutSceneCameraFocalLengthPc = (f32)(values[0] * (f64)1.3f);
            if ((camera->focus_flags & 2) != 0)
                CutSceneFocusDistancePc = values[1];
            else if (system->focus_state_animation != NULL && state->focus_index >= 0) {
                CUT_LOAD_LOCATOR *locator = &instance->scene->locator_system->locators[state->focus_index];
                if (locator->animation != NULL) {
                    NUMTX focus_matrix;
                    NuGCutLocatorCalcMtx((CUT_LOCATOR_GENERIC_PC *)locator, frame, &focus_matrix, NULL);
                    if ((locator->flags & 4) != 0)
                        NuMtxPreTranslate(&focus_matrix, &locator->pivot);
                    if ((instance->flags_88 & 0x80) != 0)
                        NuMtxMul(&focus_matrix, &focus_matrix, &instance->matrix);
                    NuMtxGetTranslation(&focus_matrix, &CutSceneFocusLocationPc);
                    CutSceneUseFocusLocationPc = 1;
                }
            }
        }
        if ((i32)animation->magic >= 0x414e4934 && (i32)animation->magic <= 0x414e4935)
            NuAnimCurve2SetApplyToMatrix_3(animation, camera->animation_node, instance->render_frame,
                                           &CutSceneCameraMatrixPc);
        else
            ((void(__cdecl *)(void))CutSceneLoadDiagnosticPc)();
    }
    if ((instance->flags_88 & 0x80) != 0)
        NuMtxMul(&CutSceneCameraMatrixPc, &CutSceneCameraMatrixPc, &instance->matrix);
    CutSceneCameraMatrixSetPc = 1;
    if ((camera_state->flags & 2) == 0)
        return;
    target = &state->targets[camera_state->event_index];
    duration = fabsf(target->duration);
    reverse = NuFsign(target->duration) < 0.0f;
    if (reverse && target->start_frame + duration <= frame) {
        camera_state->flags &= ~2;
        return;
    }
    translation.x = CutSceneCameraMatrixPc.m30;
    translation.y = CutSceneCameraMatrixPc.m31;
    translation.z = CutSceneCameraMatrixPc.m32;
    look_at = CutSceneCameraMatrixPc;
    NuMtxLookAtZ(&look_at, target->target);
    if (duration > 0.01f) {
        blend = (frame - target->start_frame) / duration;
        if (blend > 1.0f || (!(blend < 0.0f) && reverse))
            blend = 1.0f - blend;
    } else
        blend = reverse ? 0.0f : 1.0f;
    NuMtxToQuat(&look_at, &target_rotation);
    NuMtxToQuat(&CutSceneCameraMatrixPc, &camera_rotation);
    NuQuatSlerp(&blended_rotation, &camera_rotation, &target_rotation, blend);
    NuQuatToMtx(&blended_rotation, &CutSceneCameraMatrixPc);
    CutSceneCameraMatrixPc.m30 = translation.x;
    CutSceneCameraMatrixPc.m31 = translation.y;
    CutSceneCameraMatrixPc.m32 = translation.z;
}

STATIC_ASSERT(offsetof(CUT_LOAD_TRIGGER_FLAG_PC, flags) == 2);
STATIC_ASSERT(offsetof(CUT_LOAD_TRIGGER_OWNER, trigger_states) == 0xc);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_INSTANCE, state_index) == 8);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_INSTANCE, camera_index) == 9);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_INSTANCE, next_target_index) == 0xa);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_INSTANCE, target_count) == 0xc);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_INSTANCE, focus_state_index) == 0xd);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_INSTANCE, focus_index) == 0xe);
STATIC_ASSERT(sizeof(CUT_LOAD_CAMERA_INSTANCE) == 16);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, bounds_min) == 0x6c);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, bounds_max) == 0x78);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, radius_squared) == 0x84);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, chained_instance) == 0xb0);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, end_callback) == 0xb4);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, skip_countdown) == 0xd8);

STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, center) == 0x60);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, accumulated_duration) == 0xe0);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, elapsed) == 0xe4);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, pending_stream) == 0xec);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, queued_stream_instance) == 0xf4);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_TARGET_PC, target) == 0x0);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_TARGET_PC, start_frame) == 0x4);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_TARGET_PC, duration) == 0x8);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_TARGET_PC, target_index) == 0xc);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_STATE_PC, flags) == 0x0);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_STATE_PC, event_index) == 0x1);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_SYSTEM, target_camera_map) == 0x11);
STATIC_ASSERT(offsetof(CUT_LOAD_CAMERA_PC, flags) == 0x40);
STATIC_ASSERT(offsetof(CUT_LOAD_BOUNDS, min) == 0x0);
STATIC_ASSERT(offsetof(CUT_LOAD_BOUNDS, max) == 0xc);

static void CutSceneRigidSysUpdatePc(INSTNUGCUTSCENE *owner, f32 frame, i32 paused) {
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    CUT_SCENE_LOAD_PC *scene = instance->scene;
    CUT_LOAD_RIGID_SYSTEM *system = scene->rigid_system;
    CUT_LOAD_RIGID_INSTANCE_SYSTEM *objects = instance->rigid_instance;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_RIGID *rigid = &system->rigids[i];
        CUT_LOAD_RIGID_INSTANCE *object = &objects->rigids[i];
        NUMTX matrix;
        u8 visible;
        if ((rigid->flags & 2) != 0 || (rigid->flags & 4) == 0)
            continue;
        if (rigid->state_animation != NULL &&
            StateAnimEvaluate(rigid->state_animation, &object->state_index, &visible, frame) != 0)
            NuSpecialSetVisibility(&object->special, visible != 0);
        if (NuSpecialGetVisibilityFn(&object->special) == 0)
            continue;
        NuGCutRigidCalcMtx((CUT_RIGID_GENERIC_PC *)rigid, frame, &matrix);
        if ((instance->flags_88 & 0x80) != 0)
            NuMtxMul(&matrix, &matrix, &instance->matrix);
        NuSpecialSetDrawMtx(&object->special, &matrix);
        if (NuSpecialGetVisibilityFn(&object->special) != 0 && rigid->locator_index != 0xff) {
            CUT_LOAD_LOCATOR_SYSTEM *locators = scene->locator_system;
            CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *states = instance->locator_instance;
            i32 j;
            for (j = 0; j < rigid->uses_locator; ++j) {
                i32 index = rigid->locator_index + j;
                instNuGCutLocatorUpdate(owner, locators, &states->locators[index], &locators->locators[index], frame,
                                        &matrix, paused);
            }
        }
    }
}

static void CutSceneTriggerSysUpdatePc(CUT_INSTANCE_LOAD_PC *instance, f32 frame) {
    CUT_LOAD_TRIGGER_SYSTEM *system = instance->scene->trigger_system;
    CUT_LOAD_TRIGGER_INSTANCE *states = instance->trigger_instance;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_TRIGGER_EVENT *event = &system->events[i];
        u8 value;
        if (event->state_animation != NULL &&
            StateAnimEvaluate(event->state_animation, &states->states[i].state_index, &value, frame) != 0) {
            CUT_LOAD_TRIGGER_FLAG_PC *target = &states->owner->trigger_states[(i16)event->index];
            if (value != 0)
                target->flags |= 1;
            else
                target->flags &= ~1;
        }
    }
}

static void CutSceneRigidSysStartPc(CUT_LOAD_RIGID_INSTANCE_SYSTEM *states, CUT_LOAD_RIGID_SYSTEM *system) {
    u32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_RIGID *rigid = &system->rigids[i];
        CUT_LOAD_RIGID_INSTANCE *state = &states->rigids[i];
        state->state_index = 0;
        if ((rigid->flags & 6) == 6)
            state->visible = rigid->flags & 1;
    }
}

static void CutSceneLocatorSysStartPc(CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *states, CUT_LOAD_LOCATOR_SYSTEM *system) {
    u32 i;
    for (i = 0; i < system->count; ++i) {
        states->locators[i].frame = 0.0f;
        if ((system->types[system->locators[i].type_index].flags & 0x10) != 0)
            states->locators[i].effect.index = -1;
    }
}

void __cdecl instNuGCutSceneStart(INSTNUGCUTSCENE *owner) {
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    instance->current_frame = 1.0f;
    instance->render_frame = 1.0f;
    instance->flags_8c &= ~0x1000U;
    instance->flags_88 = (instance->flags_88 & ~0x1001U) | 2;
    if (instance->rate < 0.0f)
        instance->rate = -instance->rate;
    if (instance->camera_instance != NULL) {
        CUT_LOAD_CAMERA_INSTANCE *state = instance->camera_instance;
        CUT_LOAD_CAMERA_SYSTEM *system = instance->scene->camera_system;
        u32 i;
        state->state_index = 0;
        state->camera_index = system->starting_camera;
        state->next_target_index = 0;
        for (i = 0; i < (u32)system->count; ++i) {
            state->states[i].flags &= ~2;
            state->states[i].event_index = 0;
        }
    }
    if (instance->rigid_instance != NULL)
        CutSceneRigidSysStartPc(instance->rigid_instance, instance->scene->rigid_system);
    if (instance->locator_instance != NULL)
        CutSceneLocatorSysStartPc(instance->locator_instance, instance->scene->locator_system);
    if (instance->trigger_instance != NULL) {
        i32 i;
        for (i = 0; i < instance->scene->trigger_system->count; ++i)
            instance->trigger_instance->states[i].state_index = 0;
    }
    if (instance->skip_countdown != 0 && instance->stream_index == 0) {
        instance->flags_88 |= 0x40000000;
        instance->flags_8c = (instance->flags_8c & ~5U) | 0xa;
    }
}

static void CutSceneRigidSysResetPc(CUT_LOAD_RIGID_INSTANCE_SYSTEM *states, CUT_LOAD_RIGID_SYSTEM *system) {
    u32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_RIGID *rigid = &system->rigids[i];
        CUT_LOAD_RIGID_INSTANCE *state = &states->rigids[i];
        state->state_index = 0;
        if ((rigid->flags & 4) != 0) {
            if ((rigid->flags & 2) != 0)
                state->visible = rigid->flags & 1;
            else {
                NuSpecialSetVisibility(&state->special, (rigid->flags & 1) != 0);
                NuSpecialSetDrawMtx(&state->special, &rigid->matrix);
            }
        }
    }
}

static void CutSceneLocatorSysResetPc(INSTNUGCUTSCENE *owner) {
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    CUT_LOAD_LOCATOR_SYSTEM *system = instance->scene->locator_system;
    CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *states = instance->locator_instance;
    u32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_LOCATOR *locator = &system->locators[i];
        CUT_LOAD_LOCATOR_INSTANCE *state = &states->locators[i];
        CUT_LOAD_LOCATOR_TYPE *type = &system->types[locator->type_index];
        if ((type->flags & 2) != 0 && type->function_index != 0xffff &&
            CutSceneLocatorFunctionsPc[type->function_index].reset != NULL)
            CutSceneLocatorFunctionsPc[type->function_index].reset(owner, system, state, locator);
        if ((type->flags & 0x10) != 0 && type->function_index != 0xffff)
            state->effect.index = -1;
    }
}

static void CutSceneTriggerSysResetPc(CUT_INSTANCE_LOAD_PC *instance) {
    CUT_LOAD_TRIGGER_SYSTEM *system = instance->scene->trigger_system;
    CUT_LOAD_TRIGGER_INSTANCE *states = instance->trigger_instance;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_TRIGGER_EVENT *event = &system->events[i];
        u8 value;
        states->states[i].state_index = 0;
        if (StateAnimEvaluate(event->state_animation, &states->states[i].state_index, &value, 1.0f) != 0) {
            CUT_LOAD_TRIGGER_FLAG_PC *target = &states->owner->trigger_states[(i16)event->index];
            if (value != 0)
                target->flags |= 1;
            else
                target->flags &= ~1;
        }
    }
}

void __cdecl instNuGCutSceneReset(INSTNUGCUTSCENE *owner) {
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    if (instance == NULL)
        return;
    instance->flags_88 &= ~0x1007U;
    instance->flags_8c &= ~0x1040U;
    instance->current_frame = 1.0f;
    instance->render_frame = 1.0f;
    instance->chained_instance = NULL;
    instance->end_callback = NULL;
    if (instance->rate < 0.0f)
        instance->rate = -instance->rate;
    instance->scene = instance->saved_scene;
    if (instance->rigid_instance != NULL)
        CutSceneRigidSysResetPc(instance->rigid_instance, instance->scene->rigid_system);
    if (instance->locator_instance != NULL)
        CutSceneLocatorSysResetPc(owner);
    if (instance->trigger_instance != NULL)
        CutSceneTriggerSysResetPc(instance);
    if (instance->character_instance != NULL && NuCutSceneResetCharacters != NULL)
        NuCutSceneResetCharacters(owner);
    if (instance->camera_instance != NULL && instance->camera_instance->camera_index >= 0)
        CutSceneCameraCTRL = 0;
}

f32 __cdecl NuCameraDistSqr(NUVEC *);
void __cdecl CutSceneStreamDiagnosticPc(char *);

i32 CutSceneRenderRigidsPc = 1;
i32 CutSceneRenderCharactersPc = 1;
i32 CutSceneRenderLocatorsPc = 1;
i32 CutSceneUpdateCamerasPc = 1;

extern INSTNUGCUTSCENE *CutSceneLoadingInstancePc;

static void CutSceneCopyStreamAnimationsPc(CUT_SCENE_LOAD_PC *target, CUT_SCENE_LOAD_PC *source) {
    i32 i;
    if ((target->version & 0xff) >= 5) {
        target->camera_system->focus_animation = source->camera_system->focus_animation;
        target->camera_system->focus_state_animation = source->camera_system->focus_state_animation;
    }
    if (source->camera_system->animation != NULL)
        target->camera_system->animation = source->camera_system->animation;
    target->camera_system->state_animation = source->camera_system->state_animation;
    if (source->locator_system != NULL && source->locator_system->locators != NULL) {
        for (i = 0; i < source->locator_system->count; ++i) {
            if (source->locator_system->locators[i].animation != NULL)
                target->locator_system->locators[i].animation = source->locator_system->locators[i].animation;
        }
    }
    if (source->rigid_system != NULL && source->rigid_system->rigids != NULL) {
        for (i = 0; i < source->rigid_system->count; ++i) {
            if (source->rigid_system->rigids[i].animation != NULL)
                target->rigid_system->rigids[i].animation = source->rigid_system->rigids[i].animation;
            target->rigid_system->rigids[i].state_animation = source->rigid_system->rigids[i].state_animation;
        }
    }
    if (source->character_system != NULL && source->character_system->characters != NULL) {
        for (i = 0; i < source->character_system->count; ++i) {
            CUT_LOAD_CHARACTER *from = &source->character_system->characters[i];
            CUT_LOAD_CHARACTER *to = &target->character_system->characters[i];
            if (from->animation != NULL)
                to->animation = from->animation;
            if (from->face_animation != NULL)
                to->face_animation = from->face_animation;
            if (from->extra_animation != NULL)
                to->extra_animation = from->extra_animation;
        }
    }
    if (source->trigger_system != NULL && source->trigger_system->events != NULL) {
        for (i = 0; i < source->trigger_system->count; ++i)
            target->trigger_system->events[i].state_animation = source->trigger_system->events[i].state_animation;
    }
    target->duration = source->duration;
}

static void CutSceneSwapStreamAssetPc(CUT_INSTANCE_LOAD_PC *instance) {
    CUT_SCENE_LOAD_PC *saved = instance->saved_scene;
    CUT_SCENE_LOAD_PC *scene =
        (CUT_SCENE_LOAD_PC *)((instance->flags_88 & 0x10000000) != 0 ? instance->stream_buffer_1
                                                                     : instance->stream_buffer_0);
    i32 i;
    instance->scene = scene;
    scene->filename = saved->filename;
    scene->stream_buffer_0 = saved->stream_buffer_0;
    scene->stream_buffer_1 = saved->stream_buffer_1;
    scene->flags = (scene->flags & ~2U) | (saved->flags & 2);
    if (scene->locator_system != NULL) {
        for (i = 0; i < scene->locator_system->count; ++i) {
            CUT_LOAD_LOCATOR *to = &scene->locator_system->locators[i];
            void *animation = to->animation;
            *to = saved->locator_system->locators[i];
            to->animation = animation;
        }
        scene->locator_system->types = saved->locator_system->types;
    }
    if (scene->rigid_system != NULL && scene->rigid_system->rigids != NULL) {
        for (i = 0; i < scene->rigid_system->count; ++i) {
            CUT_LOAD_RIGID *to = &scene->rigid_system->rigids[i];
            void *animation = to->animation;
            STATE_ANIM_PC *state_animation = to->state_animation;
            *to = saved->rigid_system->rigids[i];
            to->animation = animation;
            to->state_animation = state_animation;
            if (to->locator.pointer != NULL && to->uses_locator != 0 && to->locator_index != 0xff)
                to->locator.pointer = &scene->locator_system->locators[to->locator_index];
        }
    }
    if (scene->character_system != NULL) {
        for (i = 0; i < scene->character_system->count; ++i) {
            CUT_LOAD_CHARACTER *to = &scene->character_system->characters[i];
            void *animation = to->animation, *face = to->face_animation, *extra = to->extra_animation;
            CUT_LOAD_LOCATOR *locator = to->locator;
            *to = saved->character_system->characters[i];
            to->animation = animation;
            to->face_animation = face;
            to->extra_animation = extra;
            if (locator != NULL && to->uses_locator != 0 && to->locator_index != 0xff)
                to->locator = &scene->locator_system->locators[to->locator_index];
        }
    }
    if (scene->trigger_system != NULL) {
        for (i = 0; i < scene->trigger_system->count; ++i) {
            CUT_LOAD_TRIGGER_EVENT *to = &scene->trigger_system->events[i];
            STATE_ANIM_PC *animation = to->state_animation;
            *to = saved->trigger_system->events[i];
            to->state_animation = animation;
        }
    }
}

static void CutSceneClipTestPc(CUT_INSTANCE_LOAD_PC *instance) {
    CUT_SCENE_LOAD_PC *scene = instance->scene;
    instance->flags_88 |= 0x400;
    if (scene->bounds == NULL)
        return;
    if ((instance->flags_88 & 0x200) != 0 && NuCameraDistSqr(&instance->center) > instance->radius_squared) {
        instance->flags_88 &= ~0x400U;
        return;
    }
    if ((instance->flags_88 & 0x100) != 0 &&
        NuCameraClipTestExtents(&scene->bounds->min, &scene->bounds->max,
                                (instance->flags_88 & 0x80) != 0 ? &instance->matrix : &NuMtxSetIdentityTemplate, 0.0f,
                                0) == 0)
        instance->flags_88 &= ~0x400U;
}

static void CutSceneStreamSkipPc(CUT_INSTANCE_LOAD_PC *instance, f32 elapsed) {
    u8 last_stream = (u8)instance->saved_scene->unknown_40;
    if (instance->stream_index == last_stream)
        instance->current_frame = instance->scene->duration - 1.0f;
    instance->current_frame -= instance->rate * elapsed;
    if (CutSceneLoadingInstancePc != NULL)
        return;
    if ((i32)instance->stream_index == (i32)last_stream - 1) {
        if ((instance->flags_8c & 0x80) != 0 || instance->pending_stream != NULL)
            CutSceneStreamDiagnosticPc(instance->scene->filename);
        else {
            if ((instance->flags_88 & 0x10000000) != 0) {
                instance->flags_88 &= ~0x10000000U;
                instance->pending_stream = instance->stream_buffer_1;
            } else {
                instance->flags_88 |= 0x10000000;
                instance->pending_stream = instance->stream_buffer_0;
            }
            CutSceneStreamDiagnosticPc(instance->scene->filename);
        }
        if (CutSceneSingleStreamBufferPc < 2)
            CutSceneCopyStreamAnimationsPc(instance->scene, (CUT_SCENE_LOAD_PC *)((instance->flags_88 & 0x10000000) != 0
                                                                                      ? instance->stream_buffer_1
                                                                                      : instance->stream_buffer_0));
        else
            CutSceneSwapStreamAssetPc(instance);
        instance->stream_index = (u8)instance->scene->unknown_40;
        instNuGCutSceneStart((INSTNUGCUTSCENE *)instance);
        instance->current_frame = instance->scene->duration - 1.0f;
    } else {
        instance->stream_index = (u8)(instance->scene->unknown_40 - 1);
        instance->pending_stream =
            (instance->flags_88 & 0x10000000) != 0 ? instance->stream_buffer_0 : instance->stream_buffer_1;
        CutSceneLoadingInstancePc = (INSTNUGCUTSCENE *)instance;
    }
}

static i32 CutSceneSynchroCounterPc = 0;
static i32 CutSceneQuitPromptPc = 0;
static i32 CutSceneHasQuitPc = 0;
static i32 CutSceneLockPlayerPc = 0;
static i32 CutSceneTerminateStreamPc = 0;
extern i32 CutSceneUpdateCamerasPc;
extern void(__cdecl *CutSceneInstanceSfxRequestPc)(INSTNUGCUTSCENE *);
extern void(__cdecl *NuCutSceneCharacterProcess)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32);

static void CutSceneCharacterSysUpdatePc(INSTNUGCUTSCENE *owner, f32 frame, i32 paused) {
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    CUT_SCENE_LOAD_PC *scene = instance->scene;
    CUT_LOAD_CHARACTER_SYSTEM *system = scene->character_system;
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *states = instance->character_instance;
    i32 i;
    if (NuCutSceneCharacterProcess != NULL) {
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_CHARACTER_INSTANCE *state = &states->characters[i];
            if (state->model != NULL)
                NuCutSceneCharacterProcess(owner, scene, state, &system->characters[i], frame, paused);
        }
    }
}

static void CutSceneStreamHoldFramePc(CUT_INSTANCE_LOAD_PC *instance) {
    instance->current_frame = instance->scene->duration - 1.0f;
    CutSceneMissingObjectDiagnosticPc(instance->scene->filename);
}

static void CutSceneInstanceUpdatePc(INSTNUGCUTSCENE *owner, i32 paused, i32 skip, f32 elapsed) {
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    CUT_SCENE_LOAD_PC *scene = instance->scene;
    f32 overflow;
    i32 repeat_count;
    instance->flags_8c &= ~0x1000U;
    instance->flags_88 = (instance->flags_88 & ~0x7f80000U) | (((u32)CutSceneSynchroCounterPc << 19) & 0x7f80000);
    if ((instance->flags_88 & 0x800) != 0 || (scene->flags & 8) != 0)
        return;
    instance->elapsed = elapsed;
    if (CutSceneInstanceSfxRequestPc != NULL && (scene->flags & 4) != 0 && (instance->flags_8c & 0x20) != 0) {
        NUMTX *camera = NuCameraGetMtx();
        if (camera != NULL && camera->m30 < instance->bounds_max.x && camera->m30 > instance->bounds_min.x &&
            camera->m31 < instance->bounds_max.y && camera->m31 > instance->bounds_min.y &&
            camera->m32 < instance->bounds_max.z && camera->m32 > instance->bounds_min.z)
            CutSceneInstanceSfxRequestPc(owner);
    }
    instance->flags_88 &= ~0x40000000U;
    if (instance->skip_countdown != 0 && (instance->flags_88 & 1) == 0 && (instance->flags_8c & 0xf) != 0) {
        instance->flags_8c = (instance->flags_8c & ~0xfU) | ((instance->flags_8c - 1) & 0xf);
        instance->flags_88 |= 0x40000000;
        paused = 1;
    }
    if ((instance->flags_88 & 2) == 0) {
        if ((instance->flags_88 & 1) != 0 || (instance->flags_88 & 0x40) == 0)
            return;
        instNuGCutSceneStart(owner);
    }
    if ((scene->flags & 8) != 0 && (instance->flags_88 & 0x8000000) == 0)
        return;
    if (instance->scene->version >= 2) {
        if ((instance->flags_88 & 0x20000000) != 0 && CutSceneLoadingInstancePc == NULL &&
            (instance->flags_88 & 0x8000000) == 0 && instance->stream_index != (u8)instance->saved_scene->unknown_40)
            CutSceneLoadingInstancePc = owner;
        else if (instance->queued_stream_instance != NULL && CutSceneLoadingInstancePc == NULL &&
                 instance->stream_index == (u8)instance->saved_scene->unknown_40) {
            CutSceneLoadingInstancePc = instance->queued_stream_instance;
            instance->queued_stream_instance = NULL;
        }
    }
    CutSceneClipTestPc(instance);
    if ((instance->flags_88 & 0x400) == 0)
        return;
    if ((instance->flags_88 & 4) != 0)
        paused = 1;
    if (CutSceneTerminateStreamPc != 0 || (instance->flags_8c & 0x10) != 0) {
        if (scene->version >= 2 && (scene->flags & 1) != 0) {
            if (skip != 0)
                instance->flags_8c |= 0x400;
            else
                CutSceneQuitPromptPc = 1;
        } else if ((instance->flags_8c & 0x10) != 0) {
            if (skip != 0) {
                instance->current_frame = scene->duration - 1.0f - instance->rate * elapsed;
                CutSceneHasQuitPc = 1;
            } else
                CutSceneQuitPromptPc = 1;
        }
    }
    if ((instance->flags_8c & 0x200) != 0)
        CutSceneLockPlayerPc = 1;
    if ((instance->flags_88 & 1) == 0) {
        if (paused == 0)
            instance->flags_88 |= 1;
    } else if (paused == 0) {
        instance->current_frame += instance->rate * elapsed;
        if ((instance->flags_8c & 0x400) != 0)
            CutSceneStreamSkipPc(instance, elapsed);
        if (instance->current_frame < 1.0f) {
            if ((instance->flags_88 & 0x10) != 0) {
                if ((instance->flags_88 & 8) != 0) {
                    if (instance->rate < 0.0f)
                        instance->rate = -instance->rate;
                    instance->current_frame = 2.0f - instance->current_frame;
                } else {
                    repeat_count = (i32)(instance->flags_88 << 14) >> 27;
                    if (repeat_count > 0) {
                        overflow = 1.0f - instance->current_frame;
                        instance->flags_88 =
                            (instance->flags_88 & ~0x3e000U) | (((u32)(repeat_count - 1) << 13) & 0x3e000);
                        instNuGCutSceneStart(owner);
                        instance->flags_88 |= 1;
                        instance->current_frame += overflow;
                    } else if (instance->chained_instance != NULL) {
                        CUT_INSTANCE_LOAD_PC *next;
                        overflow = 1.0f - instance->current_frame;
                        instNuGCutSceneEndFirstFrame(owner);
                        instNuGCutSceneStart(instance->chained_instance);
                        next = (CUT_INSTANCE_LOAD_PC *)instance->chained_instance;
                        next->current_frame += overflow;
                        CutSceneClipTestPc(next);
                        CutSceneInstanceUpdatePc(instance->chained_instance, 0, skip, elapsed);
                        instance->chained_instance = NULL;
                        return;
                    } else {
                        instNuGCutSceneEndFirstFrame(owner);
                        return;
                    }
                }
            } else if ((instance->flags_88 & 0x20) != 0)
                instance->current_frame = 1.0f;
            else
                instNuGCutSceneEndFirstFrame(owner);
        } else if (instance->current_frame >= ((f64)scene->duration - 1.0)) {
            CUT_SCENE_LOAD_PC *current = instance->scene;
            if (current->version >= 2 && (current->flags & 1) != 0) {
                if (instance->stream_index >= (u8)instance->saved_scene->unknown_40) {
                    if ((instance->flags_8c & 0x40) != 0) {
                        instance->flags_88 |= 0x1000;
                        instance->current_frame = ((f64)scene->duration - 1.0);
                    } else {
                        instNuGCutSceneEnd(owner);
                        instance->accumulated_duration = 0.0f;
                    }
                } else if ((instance->flags_88 & 0x8000000) == 0)
                    CutSceneStreamHoldFramePc(instance);
                else {
                    overflow = instance->current_frame - ((f64)current->duration - 1.0);
                    if (overflow > current->duration)
                        overflow = 0.0f;
                    ++instance->stream_index;
                    instance->accumulated_duration += current->duration;
                    if ((instance->flags_8c & 0x80) != 0 || instance->pending_stream != NULL) {
                        CutSceneStreamDiagnosticPc(instance->scene->filename);
                        CutSceneStreamHoldFramePc(instance);
                        --instance->stream_index;
                    } else {
                        if ((instance->flags_88 & 0x10000000) != 0) {
                            instance->flags_88 &= ~0x10000000U;
                            instance->pending_stream = instance->stream_buffer_1;
                        } else {
                            instance->flags_88 |= 0x10000000;
                            instance->pending_stream = instance->stream_buffer_0;
                        }
                        CutSceneStreamDiagnosticPc(instance->scene->filename);
                        if (CutSceneSingleStreamBufferPc < 2)
                            CutSceneCopyStreamAnimationsPc(instance->scene,
                                                           (CUT_SCENE_LOAD_PC *)((instance->flags_88 & 0x10000000) != 0
                                                                                     ? instance->stream_buffer_1
                                                                                     : instance->stream_buffer_0));
                        else
                            CutSceneSwapStreamAssetPc(instance);
                        instance->flags_88 &= ~0x8000000U;
                        instNuGCutSceneStart(owner);
                        instance->flags_88 |= 1;
                        instance->current_frame += overflow;
                    }
                }
            } else if ((instance->flags_88 & 0x10) != 0) {
                overflow = instance->current_frame - ((f64)scene->duration - 1.0);
                instance->current_frame = ((f64)scene->duration - 1.0) - overflow;
                if (instance->rate > 0.0f)
                    instance->rate = -instance->rate;
            } else if ((instance->flags_8c & 0x40) != 0) {
                instance->flags_88 |= 0x1000;
                instance->current_frame = ((f64)scene->duration - 1.0);
            } else {
                if (instance->end_callback != NULL) {
                    void(__cdecl * callback)(INSTNUGCUTSCENE *) = instance->end_callback;
                    instance->end_callback = NULL;
                    callback(owner);
                }
                repeat_count = (i32)(instance->flags_88 << 14) >> 27;
                if (repeat_count > 0) {
                    instance->flags_88 = (instance->flags_88 & ~0x3e000U) | (((u32)(repeat_count - 1) << 13) & 0x3e000);
                    overflow = instance->current_frame - ((f64)scene->duration - 1.0);
                    if (overflow > scene->duration)
                        overflow = 0.0f;
                    instNuGCutSceneStart(owner);
                    instance->flags_88 |= 1;
                    instance->current_frame += overflow;
                } else if (instance->chained_instance != NULL) {
                    CUT_INSTANCE_LOAD_PC *next;
                    overflow = instance->current_frame - ((f64)scene->duration - 1.0);
                    if (overflow > scene->duration)
                        overflow = 0.0f;
                    instNuGCutSceneEnd(owner);
                    instNuGCutSceneStart(instance->chained_instance);
                    next = (CUT_INSTANCE_LOAD_PC *)instance->chained_instance;
                    next->current_frame += overflow;
                    CutSceneClipTestPc(next);
                    CutSceneInstanceUpdatePc(instance->chained_instance, 0, skip, elapsed);
                    instance->chained_instance = NULL;
                    return;
                } else if ((instance->flags_88 & 8) != 0) {
                    overflow = instance->current_frame - ((f64)scene->duration - 1.0);
                    if (overflow > scene->duration)
                        overflow = 0.0f;
                    instNuGCutSceneStart(owner);
                    instance->flags_88 |= 1;
                    instance->current_frame += overflow;
                } else
                    instNuGCutSceneEnd(owner);
            }
        }
    }
    if ((instance->flags_88 & 2) == 0)
        return;
    instance->render_frame = instance->current_frame;
    if ((instance->flags_88 & 0x40000) != 0) {
        if (CutSceneRenderCharactersPc != 0 && instance->character_instance != NULL)
            CutSceneCharacterSysUpdatePc(owner, scene->duration - instance->current_frame, paused);
        if (CutSceneUpdateCamerasPc != 0 && instance->camera_instance != NULL)
            CutSceneCameraSysUpdatePc(owner, scene->duration - instance->current_frame);
        if (instance->trigger_instance != NULL)
            CutSceneTriggerSysUpdatePc(instance, scene->duration - instance->render_frame);
        if (CutSceneRenderRigidsPc != 0 && instance->rigid_instance != NULL)
            CutSceneRigidSysUpdatePc(owner, scene->duration - instance->render_frame, paused);
    } else {
        if (CutSceneRenderCharactersPc != 0 && instance->character_instance != NULL)
            CutSceneCharacterSysUpdatePc(owner, instance->current_frame, paused);
        if (CutSceneUpdateCamerasPc != 0 && instance->camera_instance != NULL)
            CutSceneCameraSysUpdatePc(owner, instance->current_frame);
        if (instance->trigger_instance != NULL)
            CutSceneTriggerSysUpdatePc(instance, instance->render_frame);
        if (CutSceneRenderRigidsPc != 0 && instance->rigid_instance != NULL)
            CutSceneRigidSysUpdatePc(owner, instance->render_frame, paused);
    }
}

void __cdecl NuGCutSceneSysUpdate(i32 paused, i32 skip, f32 elapsed) {
    INSTNUGCUTSCENE *owner;
    if (++CutSceneSynchroCounterPc > 0xff)
        CutSceneSynchroCounterPc = 1;
    CutSceneQuitPromptPc = 0;
    CutSceneHasQuitPc = 0;
    CutSceneLockPlayerPc = 0;
    CutSceneCameraChangedPc = 0;
    if (paused != 0)
        skip = 0;
    for (owner = active_cutscene_instances; owner != NULL; owner = ((CUT_INSTANCE_LOAD_PC *)owner)->next) {
        CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
        if (((i32)(instance->flags_88 << 5) >> 24) != CutSceneSynchroCounterPc)
            CutSceneInstanceUpdatePc(owner, paused, skip, elapsed);
    }
}

STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, filename) == 0x2c);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, stream_buffer_1) == 0x34);
STATIC_ASSERT(offsetof(CUT_SCENE_LOAD_PC, stream_buffer_size) == 0x3c);
STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, stream_buffer_1) == 0xc0);
STATIC_ASSERT(offsetof(CUT_CHARACTER_SYSTEM_LIFECYCLE_PC, characters) == 0);
STATIC_ASSERT(offsetof(CUT_CHARACTER_INSTANCE_SYSTEM_PC, characters) == 0);
STATIC_ASSERT(offsetof(CUT_CHARACTER_INSTANCE_PC, model) == 0);

char *__cdecl NuStrStr(char *text, const char *needle);
extern i32 CutSceneStreamLoadGuardPc;

static void CutSceneReleaseInstanceCharactersPc(CUT_INSTANCE_LIFECYCLE_PC *instance) {
    CUT_CHARACTER_SYSTEM_LIFECYCLE_PC *system = instance->scene->character_system;
    CUT_CHARACTER_INSTANCE_SYSTEM_PC *states = instance->character_instance;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_CHARACTER_INSTANCE_PC *state = &states->characters[i];
        if (state->model != NULL && NuCutSceneCharacterRelease != NULL)
            NuCutSceneCharacterRelease(state, &system->characters[i]);
    }
}

void __cdecl instNuGCutSceneStop(INSTNUGCUTSCENE *owner) {
    CUT_INSTANCE_LIFECYCLE_PC *instance = (CUT_INSTANCE_LIFECYCLE_PC *)owner;
    instance->flags_88 &= ~0x1006U;
    if (instance->character_instance != NULL)
        CutSceneReleaseInstanceCharactersPc(instance);
    if (instance->camera_instance != NULL && instance->camera_instance->camera_index >= 0)
        CutSceneCameraCTRL = 0;
}

i32 __cdecl instNuGCutSceneIsFinished(INSTNUGCUTSCENE *owner) {
    return ((CUT_INSTANCE_LIFECYCLE_PC *)owner)->flags_88 & 0x1000 ? -1 : 0;
}

void __cdecl instNuGCutScenePause(INSTNUGCUTSCENE *owner, i32 paused) {
    CUT_INSTANCE_LIFECYCLE_PC *instance = (CUT_INSTANCE_LIFECYCLE_PC *)owner;
    instance->flags_88 ^= ((u32)paused * 4U ^ instance->flags_88) & 4U;
}

void __cdecl NuGCutSceneSysPostBackgroundLoad(void) {
    INSTNUGCUTSCENE *owner = CutSceneBackgroundInstancesPc;
    INSTNUGCUTSCENE *active = active_cutscene_instances;
    if (owner == NULL)
        return;
    do {
        CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
        INSTNUGCUTSCENE *next = instance->next;
        instance->next = active;
        if (active != NULL)
            ((CUT_INSTANCE_LOAD_PC *)active)->previous = owner;
        instance->previous = NULL;
        active = owner;
        owner = next;
    } while (owner != NULL);
    active_cutscene_instances = active;
    CutSceneBackgroundInstancesPc = NULL;
}

i32 __cdecl instNuGCutSceneSwapBuffers(INSTNUGCUTSCENE *owner, i32 force) {
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    if (((instance->flags_8c & 0x80) != 0 && force == 0) || instance->pending_stream != NULL) {
        CutSceneStreamDiagnosticPc(instance->scene->filename);
        return 0;
    }
    if ((instance->flags_88 & 0x10000000) != 0) {
        instance->flags_88 &= ~0x10000000U;
        instance->pending_stream = instance->stream_buffer_1;
    } else {
        instance->flags_88 |= 0x10000000;
        instance->pending_stream = instance->stream_buffer_0;
    }
    CutSceneStreamDiagnosticPc(instance->scene->filename);
    return 1;
}

void __cdecl instNuGCutSceneServiceLoad(void) {
    char path[256];
    char extension[8];
    char *dot;
    i32 tens, units;
    VARIPTR buffer, buffer_end;
    CUT_INSTANCE_LOAD_PC *instance;
    if (CutSceneLoadingInstancePc == NULL || CutSceneStreamLoadGuardPc != 0 ||
        ((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc)->scene == NULL)
        return;
    NuStrCpy(path, ((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc)->scene->filename);
    dot = NuStrStr(path, ".");
    NuStrCpy(extension, dot);
    tens = (((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc)->stream_index + 1) / 10;
    units = (((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc)->stream_index + 1) % 10;
    if (tens != 0)
        *dot++ = (char)('0' + tens);
    *dot++ = (char)('0' + units);
    NuStrCpy(dot, extension);
    instance = (CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc;
    buffer.void_ptr = instance->pending_stream;
    instance->pending_stream = NULL;
    if (buffer.void_ptr == NULL)
        return;
    ((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc)->flags_8c |= 0x80;
    buffer_end.addr = buffer.addr + ((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc)->scene->stream_buffer_size;
    ((void(__cdecl *)(char *, ...))CutSceneMissingObjectDiagnosticPc)(path, buffer.void_ptr);
    CutSceneStreamDiagnosticPc(path);
    NuGCutSceneLoad(path, &buffer, &buffer_end, 1);
    CutSceneStreamDiagnosticPc(path);
    ((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc)->flags_88 |= 0x08000000;
    instance = (CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc;
    if (instance->stream_index == 0 && (instance->scene->flags & 8) != 0) {
        instNuGCutSceneSwapBuffers(CutSceneLoadingInstancePc, 1);
        instance = (CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc;
        if (CutSceneSingleStreamBufferPc < 2) {
            CutSceneCopyStreamAnimationsPc(instance->scene, (CUT_SCENE_LOAD_PC *)instance->scene->stream_buffer_1);
        } else {
            ((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc)->flags_88 |= 0x10000000;
            CutSceneSwapStreamAssetPc((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc);
        }
        ((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc)->scene->flags &= ~8U;
        ((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc)->flags_88 &= ~0x08000000U;
        ((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc)->stream_index = 1;
    }
    ((CUT_INSTANCE_LOAD_PC *)CutSceneLoadingInstancePc)->flags_8c &= ~0x80U;
    CutSceneLoadingInstancePc = NULL;
}
