#include "globals.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/lib/camera.h"
#include "legoapi/render/fx.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "nu2api/cutscene_include.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nuanimdata.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuquat.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/trigger.h"
#include <stdio.h>
#include <float.h>
NUMTX CutSceneCameraMatrix = {0};
static INSTNUGCUTSCENE *CutSceneBackgroundInstances = 0;
static INSTNUGCUTSCENE *active_cutscene_instances = 0;
i32 CutSceneLookupAllDebrisPages = 0;

typedef struct cut_sound_voice_lifecycle_s { // size 0x88
    u8 reserved_00[0x80];                    // 0x00
    f32 first;                               // 0x80
    f32 second;                              // 0x84
} CUT_SOUND_VOICE_LIFECYCLE;

typedef struct cut_sound_owner_lifecycle_s { // size 0x10
    u8 reserved_00[0xc];                     // 0x0
    CUT_SOUND_VOICE_LIFECYCLE **voices;      // 0xc
} CUT_SOUND_OWNER_LIFECYCLE;

typedef struct cut_sound_handle_lifecycle_s { // size 0x14
    CUT_SOUND_OWNER_LIFECYCLE *owner;         // 0x00
    u8 reserved_04[4];                        // 0x04
    i16 active;                               // 0x08
    u16 third_voice;                          // 0x0a
    u8 reserved_0c[2];                        // 0x0c
    u16 first_voice;                          // 0x0e
    u16 second_voice;                         // 0x10
} CUT_SOUND_HANDLE_LIFECYCLE;

static void CutSceneDestroyCharactersPC(INSTNUGCUTSCENE *instance) {
    CUT_LOAD_CHARACTER_SYSTEM *system = instance->scene->character_system;
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *characters = instance->character_instance;
    i32 i;
    if (system != NULL && system->count != 0) {
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_CHARACTER_INSTANCE *state = &characters->characters[i];
            CUT_LOAD_CHARACTER *character = &system->characters[i];
            if ((character->flags & 2) != 0 && NuCutSceneCharacterDestroyData != NULL) {
                NuCutSceneCharacterDestroyData(character, state);
            }
        }
    }
}

static void CutSceneEndCharactersPC(INSTNUGCUTSCENE *owner, f32 frame) {
    INSTNUGCUTSCENE *instance = owner;
    CUT_SCENE_LOAD *scene = instance->scene;
    CUT_LOAD_CHARACTER_SYSTEM *system = scene->character_system;
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *characters = instance->character_instance;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_CHARACTER *source = &system->characters[i];
        CUT_LOAD_CHARACTER_INSTANCE *character = &characters->characters[i];
        if (character->model != NULL) {
            if ((source->flags & CUT_CHARACTER_MODEL_ONLY) == 0 && NuCutSceneCharacterEval != NULL) {
                NuCutSceneCharacterEval(owner, scene, character, source, frame);
            }
            if (nu_current_thread_id == 0 && NuCutSceneCharacterRelease != NULL) {
                NuCutSceneCharacterRelease(character, source);
            }
        }
    }
}

static void CutSceneEndRigidsPC(INSTNUGCUTSCENE *owner, f32 frame) {
    INSTNUGCUTSCENE *instance = owner;
    CUT_LOAD_RIGID_SYSTEM *system = instance->scene->rigid_system;
    CUT_LOAD_RIGID_INSTANCE_SYSTEM *objects = instance->rigid_instance;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_RIGID *source = &system->rigids[i];
        CUT_LOAD_RIGID_INSTANCE *object = &objects->rigids[i];
        if ((source->flags & CUTRIGID_FOUND) != 0 && (source->flags & CUTRIGID_INSTANCED) == 0) {
            NUMTX matrix;
            if (source->state_animation != NULL) {
                u8 visible;
                if (StateAnimEvaluate(source->state_animation, &object->state_index, &visible, frame) != 0) {
                    if (visible != 0) {
                        NuSpecialSetVisibility(&object->special, 1);
                    } else {
                        NuSpecialSetVisibility(&object->special, 0);
                    }
                }
            }
            if (NuSpecialGetVisibilityFn(&object->special) == 0) {
                continue;
            }
            NuGCutRigidCalcMtx((CUT_RIGID_GENERIC *)source, frame, &matrix);
            if ((instance->flags_88 & NUGCUTSCENE_INST_OWN_MATRIX) != 0) {
                NuMtxMul(&matrix, &matrix, &instance->matrix);
            }
            NuSpecialSetDrawMtx(&object->special, &matrix);
        }
    }
}

i32 instNuGCutGetNextRigidInfo(INSTNUGCUTSCENE *owner, f32 frame, i32 index, NUMTX *matrix, nuhspecial_s *special) {
    INSTNUGCUTSCENE *instance = owner;
    CUT_LOAD_RIGID_SYSTEM *system = instance->scene->rigid_system;
    CUT_LOAD_RIGID *rigid;
    CUT_LOAD_RIGID_INSTANCE *object;
    if (system == NULL) {
        return 0;
    }
    if (index >= system->count) {
        return 0;
    }
    rigid = &system->rigids[index];
    object = &instance->rigid_instance->rigids[index];
    if ((rigid->flags & (CUTRIGID_INSTANCED | CUTRIGID_FOUND)) != 0) {
        NuGCutRigidCalcMtx((CUT_RIGID_GENERIC *)rigid, frame, matrix);
        if ((instance->flags_88 & NUGCUTSCENE_INST_OWN_MATRIX) != 0) {
            NuMtxMul(matrix, matrix, &instance->matrix);
        }
        *special = object->special;
    }
    return index + 1;
}

static void CutSceneEndLocatorsPC(CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *instance, CUT_LOAD_LOCATOR_SYSTEM *system) {
    u32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_LOCATOR_INSTANCE *state = &instance->locators[i];
        CUT_LOAD_LOCATOR *locator;
        CUT_LOAD_LOCATOR_TYPE *type;
        state->frame = 0.0f;
        locator = &system->locators[i];
        type = &system->types[locator->type_index];
        if ((type->flags & CUTLOCATOR_LOOK_AT) != 0) {
            CUT_SOUND_HANDLE_LIFECYCLE *sound = (CUT_SOUND_HANDLE_LIFECYCLE *)state->effect.sound;
            CUT_SOUND_OWNER_LIFECYCLE *owner;
            if (sound->active != 0 && (owner = sound->owner) != NULL) {
                CUT_SOUND_VOICE_LIFECYCLE *voice;
                if (sound->first_voice != 0) {
                    voice = owner->voices[sound->first_voice - 1];
                    voice->first = 0.0f;
                    voice->second = 0.0f;
                }
                if (sound->second_voice != 0) {
                    voice = owner->voices[sound->second_voice - 1];
                    voice->first = 0.0f;
                    voice->second = 0.0f;
                }
                if (sound->third_voice != 0) {
                    voice = owner->voices[sound->third_voice - 1];
                    voice->first = 0.0f;
                    voice->second = 0.0f;
                }
            }
        } else if ((type->flags & CUTLOCATOR_DEBRIS) != 0) {
            if ((locator->flags & CUTLOC_PERSISTENT) != 0 && state->effect.index > -1) {
                DebFreeInstantly(&state->effect.index);
            }
        } else if ((type->flags & CUTLOCATOR_VFX) != 0) {
            if (ReleaseLocatorVfxFn != NULL) {
                ReleaseLocatorVfxFn(state->effect.index);
            }
            state->effect.index = -1;
            state->frame = 0.0f;
        }
    }
}

void CalculateLocatorDirection(i32 direction, NUMTX *matrix, NUVEC *out) {
    switch (direction) {
        case 0:
        case 1:
            out->z = 0.0f;
            out->x = 0.0f;
            out->y = 1.0f;
            return;
        case 2:
            out->z = 0.0f;
            out->x = 0.0f;
            out->y = -1.0f;
            return;
        case 3:
            out->x = -1.0f;
            out->y = 0.0f;
            out->z = 0.0f;
            break;
        case 4:
            out->x = 1.0f;
            out->y = 0.0f;
            out->z = 0.0f;
            break;
        case 5:
            out->x = 0.0f;
            out->y = -1.0f;
            out->z = 0.0f;
            break;
        case 6:
            out->x = 0.0f;
            out->y = 1.0f;
            out->z = 0.0f;
            break;
        case 7:
            out->x = 0.0f;
            out->y = 0.0f;
            out->z = 1.0f;
            break;
        case 8:
            out->x = 0.0f;
            out->y = 0.0f;
            out->z = -1.0f;
            break;
        default:
            return;
    }
    NuVecMtxRotate(out, out, matrix);
}

static void CutSceneDestroyCharacterSys(CUT_SCENE_LOAD *scene) {
    if (NuCutSceneDestroyCharacters != NULL) {
        NuCutSceneDestroyCharacters(scene);
    }
}

void NuGCutSceneDestroy(void *owner) {
    CUT_SCENE_LOAD *scene = owner;
    if (scene->character_system != NULL) {
        CutSceneDestroyCharacterSys(scene);
    }
}

void instNuGCutSceneEnd(INSTNUGCUTSCENE *owner) {
    INSTNUGCUTSCENE *instance = owner;
    CUT_SCENE_LOAD *scene = instance->scene;
    instNuGCutSceneEndButNotSystems(owner);
    instance->current_frame = scene->duration;
    instance->flags_88 = (instance->flags_88 & ~NUGCUTSCENE_INST_PLAYING) | NUGCUTSCENE_INST_ENDED;
    instance->flags_8c &= ~NUGCUTSCENE_PLAY_HOLD_LAST_FRAME;
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

void instNuGCutSceneDestroy(INSTNUGCUTSCENE *owner) {
    INSTNUGCUTSCENE *instance = owner;
    instance->scene = instance->saved_scene;
    if ((instance->flags_88 & NUGCUTSCENE_INST_PLAYING) != 0) {
        instNuGCutSceneEnd(owner);
    }
    if (instance->character_instance != NULL) {
        CutSceneDestroyCharactersPC(instance);
    }
    if (instance->next != NULL) {
        (instance->next)->previous = instance->previous;
    }
    if (instance->previous != NULL) {
        (instance->previous)->next = instance->next;
    } else {
        active_cutscene_instances = instance->next;
    }
}

static void CutCameraFixPtrs(CUT_SCENE_LOAD *scene, i32 string_delta, i32 relocation_delta, i32 mode) {
    CUT_LOAD_CAMERA_SYSTEM *system;
    if (scene->camera_system != NULL) {
        system = (CUT_LOAD_CAMERA_SYSTEM *)((u32)scene->camera_system + string_delta);
        scene->camera_system = system;
        system->cameras = system->cameras ? (void *)((u32)system->cameras + string_delta) : NULL;
        if (relocation_delta != 0) {
            if ((scene->version & 0xff) >= 5) {
                if (system->focus_animation != NULL) {
                    system->focus_animation = NuAnimData2FixPtrs(system->focus_animation, relocation_delta, mode, 0);
                }
                system->focus_state_animation = StateAnimFixPtrs(system->focus_state_animation, relocation_delta);
            }
            if (system->animation != NULL) {
                system->animation = NuAnimData2FixPtrs(system->animation, relocation_delta, mode, 0);
            }
            system->state_animation = StateAnimFixPtrs(system->state_animation, relocation_delta);
        }
    }
}

static void CutLocatorFixPtrs(CUT_SCENE_LOAD *scene, i32 string_delta, i32 relocation_delta, i32 mode) {
    CUT_LOAD_LOCATOR_SYSTEM *system;
    i32 i;
    if (scene->locator_system != NULL) {
        system = (CUT_LOAD_LOCATOR_SYSTEM *)((u32)scene->locator_system + string_delta);
        scene->locator_system = system;
        system->locators = system->locators ? (CUT_LOAD_LOCATOR *)((u32)system->locators + string_delta) : NULL;
        if (system->locators != NULL && relocation_delta != 0) {
            for (i = 0; i < system->count; ++i) {
                CUT_LOAD_LOCATOR *locator = &system->locators[i];
                if (locator->animation != NULL) {
                    locator->animation = NuAnimData2FixPtrs(locator->animation, relocation_delta, mode, 0);
                }
            }
        }
        system->types = system->types ? (CUT_LOAD_LOCATOR_TYPE *)((u32)system->types + string_delta) : NULL;
        if (system->types != NULL) {
            for (i = 0; i < system->type_count; ++i) {
                CUT_LOAD_LOCATOR_TYPE *type = &system->types[i];
                if (type->name != NULL) {
                    type->name = mode == 0 ? scene->strings + (u32)type->name - 1 : (char *)((u32)type->name + string_delta);
                }
            }
        }
    }
}

static void CutRigidFixPtrs(CUT_SCENE_LOAD *scene, i32 string_delta, i32 relocation_delta, i32 mode) {
    CUT_LOAD_RIGID_SYSTEM *system;
    i32 i;
    if (scene->rigid_system != NULL) {
        system = (CUT_LOAD_RIGID_SYSTEM *)((u32)scene->rigid_system + string_delta);
        scene->rigid_system = system;
        system->rigids = system->rigids ? (CUT_LOAD_RIGID *)((u32)system->rigids + string_delta) : NULL;
        if (system->rigids != NULL) {
            for (i = 0; i < system->count; ++i) {
                CUT_LOAD_RIGID *rigid = &system->rigids[i];
                if (rigid->name != NULL) {
                    rigid->name = mode == 0 ? scene->strings + (u32)rigid->name - 1 : (char *)((u32)rigid->name + string_delta);
                }
                if (relocation_delta != 0) {
                    if (rigid->animation != NULL) {
                        rigid->animation = NuAnimData2FixPtrs(rigid->animation, relocation_delta, mode, 0);
                    }
                    rigid->state_animation = StateAnimFixPtrs(rigid->state_animation, relocation_delta);
                }
            }
        }
    }
}

static void CutCharacterFixPtrs(CUT_SCENE_LOAD *scene, i32 string_delta, i32 relocation_delta, i32 mode) {
    CUT_LOAD_CHARACTER_SYSTEM *system;
    i32 i;
    if (scene->character_system != NULL) {
        system = (CUT_LOAD_CHARACTER_SYSTEM *)((u32)scene->character_system + string_delta);
        scene->character_system = system;
        system->characters = system->characters ? (CUT_LOAD_CHARACTER *)((u32)system->characters + string_delta) : NULL;
        if (system->characters != NULL) {
            for (i = 0; i < system->count; ++i) {
                CUT_LOAD_CHARACTER *character = &system->characters[i];
                if (character->name != NULL) {
                    character->name = mode == 0 ? scene->strings + (u32)character->name - 1 : (char *)((u32)character->name + string_delta);
                }
                if (relocation_delta != 0) {
                    if (character->animation != NULL) {
                        character->animation = NuAnimData2FixPtrs(character->animation, relocation_delta, mode, 0);
                    }
                    if (character->face_animation != NULL) {
                        character->face_animation = NuAnimData2FixPtrs(character->face_animation, relocation_delta, mode, 0);
                    }
                    if (character->extra_animation != NULL) {
                        character->extra_animation = NuAnimData2FixPtrs(character->extra_animation, relocation_delta, mode, 0);
                    }
                }
            }
        }
        if ((scene->version & 0xff) >= 4) {
            scene->character_animations =
                scene->character_animations ? (CUT_LOAD_CHARACTER_ANIMATION *)((u32)scene->character_animations + string_delta) : NULL;
            if (system->characters != NULL && scene->character_animations != NULL) {
                for (i = 0; i < system->count; ++i) {
                    if (scene->character_animations[i].animation != NULL) {
                        scene->character_animations[i].animation =
                            NuAnimData2FixPtrs(scene->character_animations[i].animation, relocation_delta, mode, 0);
                    }
                }
            }
        }
    }
}

static void CutTriggerFixPtrs(CUT_SCENE_LOAD *scene, i32 string_delta) {
    CUT_LOAD_TRIGGER_SYSTEM *system = scene->trigger_system;
    i32 i;
    if (system != NULL) {
        system = (CUT_LOAD_TRIGGER_SYSTEM *)((u32)system + string_delta);
        scene->trigger_system = system;
        system->events = system->events ? (CUT_LOAD_TRIGGER_EVENT *)((u32)system->events + string_delta) : NULL;
        if (system->events != NULL) {
            for (i = 0; i < system->count; ++i) {
                CUT_LOAD_TRIGGER_EVENT *event = &system->events[i];
                event->name = event->name ? (char *)((u32)event->name + string_delta) : NULL;
                event->state_animation = StateAnimFixPtrs(event->state_animation, string_delta);
            }
        }
    }
}

static void NuGCutSceneFixPtrs(CUT_SCENE_LOAD *scene, i32 relocation_delta, i32 mode) {
    scene->strings = scene->strings ? (char *)((u32)scene->strings + scene->string_delta) : NULL;
    if (scene->camera_system != NULL) {
        CutCameraFixPtrs(scene, scene->string_delta, relocation_delta, mode);
    }
    if (scene->locator_system != NULL) {
        CutLocatorFixPtrs(scene, scene->string_delta, relocation_delta, mode);
    }
    if (scene->rigid_system != NULL) {
        CutRigidFixPtrs(scene, scene->string_delta, relocation_delta, mode);
    }
    if (scene->character_system != NULL) {
        CutCharacterFixPtrs(scene, scene->string_delta, relocation_delta, mode);
    }
    if (scene->trigger_system != NULL) {
        CutTriggerFixPtrs(scene, scene->string_delta);
    }
    scene->bounds = scene->bounds ? (CUT_LOAD_BOUNDS *)((u32)scene->bounds + scene->string_delta) : NULL;
}

void CutSceneRemapFocus(CUT_SCENE_LOAD *scene, VARIPTR *buffer) {
    CUT_LOAD_LOCATOR *locator;
    i32 i;
    if ((scene->version & 0xff) < 5 || scene->camera_system == NULL || scene->camera_system->focus_state_animation == NULL ||
        scene->locator_system == NULL) {
        return;
    }
    locator = scene->locator_system->locators;
    buffer->addr = (buffer->addr + 1) & ~1;
    scene->focus_indices = (u16 *)buffer->addr;
    for (i = 0; i < scene->locator_system->count; ++i, ++locator) {
        if ((scene->locator_system->types[locator->type_index].flags & CUTLOCATOR_LOOK_AT) != 0) {
            u16 *index = (u16 *)buffer->addr;
            buffer->addr += 2;
            *index = (u16)i;
        }
    }
}

void *NuGCutSceneLoad(char *path, VARIPTR *buffer, VARIPTR *end, i32 no_stream_buffers) {
    char filename[1024];
    i32 capacity = end->addr - buffer->addr;
    i32 size;
    i32 filename_length;
    char *filename_destination;
    CUT_SCENE_LOAD *scene;
    u32 addr;
    char *src = path;
    char *dst = filename;
    do {
        *dst++ = *src;
    } while (*src++ != 0);
    buffer->addr = (buffer->addr + 15) & ~15;
    size = NuFileLoadBuffer(filename, buffer->void_ptr, capacity);
    if (size == 0) {
        return NULL;
    }
    scene = buffer->void_ptr;
    if (scene->version >= 10) {
        scene->loaded_size = size;
        scene->string_delta = (i32)scene - scene->string_delta;
        scene->relocation_delta = (i32)scene - scene->relocation_delta;
        if ((scene->flags & CUTSCENE_STREAM_PENDING) == 0) {
            NuGCutSceneFixPtrs(scene, scene->relocation_delta, 0);
            if ((scene->flags & CUTSCENE_STREAMED) == 0 || no_stream_buffers != 0) {
                buffer->addr += size;
                CutSceneRemapFocus(scene, buffer);
            } else {
                addr = scene->stream_buffer_offset + buffer->addr;
                scene->stream_buffer_0 = (void *)((addr + 15) & ~15);
                addr = ((addr + 15) & ~15) + scene->stream_buffer_size + 0x400;
                if (CutSceneSingleStreamBuffer == 0) {
                    addr += 15;
                    scene->stream_buffer_1 = (void *)(addr & ~15);
                    addr = (addr & ~15) + scene->stream_buffer_size + 0x400;
                }
                buffer->addr = addr;
                filename_length = NuStrLen(path);
                filename_destination = buffer->void_ptr;
                buffer->addr += filename_length + 4;
                NuStrCpy(filename_destination, path);
                scene->filename = filename_destination;
                CutSceneRemapFocus(scene, buffer);
            }
        } else {
            NuGCutSceneFixPtrs(scene, 0, 0);
            buffer->addr += size;
            CutSceneRemapFocus(scene, buffer);
        }
    } else {
        void *first_animation;
        i32 relocation_delta;
        i32 version;
        if (scene->version > 2) {
            CutSceneLoadDiagnostic(path, scene->version);
        }
        version = scene->version;
        scene->string_delta = (i32)scene - scene->string_delta;
        relocation_delta = scene->string_delta;
        if (version >= 2) {
            scene->relocation_delta = (i32)scene - scene->relocation_delta;
            relocation_delta = scene->relocation_delta;
        }
        if (version >= 2 && (scene->flags & CUTSCENE_STREAM_PENDING) != 0) {
            NuGCutSceneFixPtrs(scene, 0, 0);
            buffer->addr += size;
        } else {
            NuGCutSceneFixPtrs(scene, relocation_delta, 0);
            if (scene->version >= 2 && (scene->flags & CUTSCENE_STREAMED) != 0 && no_stream_buffers == 0) {
                first_animation = NULL;
                if (scene->camera_system != NULL && scene->camera_system->animation != NULL) {
                    first_animation = scene->camera_system->animation;
                } else if (scene->rigid_system != NULL && scene->rigid_system->rigids->animation != NULL) {
                    first_animation = scene->rigid_system->rigids->animation;
                } else if (scene->character_system != NULL && scene->character_system->characters->animation != NULL) {
                    first_animation = scene->character_system->characters->animation;
                } else if (scene->character_system != NULL && scene->character_system->characters->face_animation != NULL) {
                    first_animation = scene->character_system->characters->face_animation;
                }
                if (scene->stream_buffer_offset != 0) {
                    first_animation = (void *)(buffer->addr + scene->stream_buffer_offset);
                }
                addr = (u32)first_animation + 15;
                scene->stream_buffer_0 = (void *)(addr & ~15);
                addr = (addr & ~15) + scene->stream_buffer_size + 0x400;
                if (CutSceneSingleStreamBuffer == 0) {
                    addr += 15;
                    scene->stream_buffer_1 = (void *)(addr & ~15);
                    addr = (addr & ~15) + scene->stream_buffer_size + 0x400;
                }
                buffer->addr = addr;
                filename_length = NuStrLen(path);
                filename_destination = buffer->void_ptr;
                buffer->addr += filename_length + 4;
                NuStrCpy(filename_destination, path);
                scene->filename = filename_destination;
            } else {
                buffer->addr += size;
            }
        }
    }
    if (buffer->addr > end->addr) {
        CutSceneLoadDiagnostic(path, buffer->addr - end->addr);
    }
    return scene;
}

/* Relocates a cutscene image that is already in memory instead of loading it from disk. */
void *NuGCutSceneLoadAddr(char *path, CUT_SCENE_LOAD *scene, i32 loaded_size, VARIPTR *buffer, VARIPTR *end, i32 no_stream_buffers) {
    if (scene->version >= 10) {
        if (scene->string_delta == 0) {
            ((void (*)(void))CutSceneLoadDiagnostic)();
        }
        scene->loaded_size = loaded_size;
        scene->string_delta = (i32)scene - scene->string_delta;
        scene->relocation_delta = (i32)scene - scene->relocation_delta;
        if ((scene->flags & CUTSCENE_STREAM_PENDING) == 0) {
            NuGCutSceneFixPtrs(scene, scene->relocation_delta, 0);
            if ((scene->flags & CUTSCENE_STREAMED) != 0 && no_stream_buffers == 0) {
                ((void (*)(void))CutSceneLoadDiagnostic)();
            }
        } else {
            NuGCutSceneFixPtrs(scene, 0, 0);
        }
        CutSceneRemapFocus(scene, buffer);
        if (buffer->addr > end->addr) {
            CutSceneLoadDiagnostic(path, buffer->addr - end->addr);
        }
        scene->string_delta = 0;
        return scene;
    }
    ((void (*)(void))CutSceneLoadDiagnostic)();
    return NULL;
}

static void CutSceneRigidFixUp(CUT_SCENE_LOAD *scene, NUGSCN *gscene) {
    CUT_LOAD_RIGID_SYSTEM *system = scene->rigid_system;
    CUT_LOAD_LOCATOR_SYSTEM *locators = scene->locator_system;
    nuhspecial_s handle;
    i32 i;
    if (gscene != NULL) {
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_RIGID *rigid = &system->rigids[i];
            if (NuSpecialFind(gscene, &handle, rigid->name, 1) != 0) {
                rigid->flags |= CUTRIGID_FOUND;
                rigid->scene = handle.scene;
                if (handle.display_special != NULL) {
                    rigid->special = handle.display_special;
                } else {
                    rigid->special = handle.special;
                }
            } else {
                CutSceneMissingObjectDiagnostic(rigid->name);
            }
            if (rigid->uses_locator != 0 && rigid->locator.index < 255) {
                rigid->locator_index = (u8)rigid->locator.index;
                rigid->locator.pointer = &locators->locators[rigid->locator.index];
            } else {
                rigid->locator_index = 255;
            }
        }
    }
}

static i32 CutFindLocatorFunction(char *name) {
    i32 i = 0;
    if (CutSceneLocatorFunctions != NULL) {
        for (; CutSceneLocatorFunctions[i].name != NULL; ++i) {
            if (NuStrICmp(name, CutSceneLocatorFunctions[i].name) == 0) {
                return i;
            }
        }
    }
    return -1;
}

static void CutSceneLocatorFixUp(CUT_SCENE_LOAD *scene, i32 page) {
    CUT_LOAD_LOCATOR_SYSTEM *system = scene->locator_system;
    u32 i;
    for (i = 0; i < system->type_count; ++i) {
        CUT_LOAD_LOCATOR_TYPE *type = &system->types[i];
        if ((type->flags & CUTLOCATOR_DEBRIS) != 0) {
            if (CutSceneLookupAllDebrisPages != 0) {
                type->function_index = (u16)LookupDebrisEffectPage(type->name, (i8)page);
            } else {
                type->function_index = (u16)LookupDebrisEffectPageOnly(type->name, (i8)page);
            }
        } else if ((type->flags & CUTLOCATOR_FUNCTION) != 0) {
            type->function_index = (u16)CutFindLocatorFunction(type->name);
        } else if ((type->flags & CUTLOCATOR_VFX) != 0) {
            if (LookupLocatorVfxFn != NULL) {
                char *underscore = NuStrRChr(type->name, '_');
                if (underscore != NULL && underscore[1] >= '0' && underscore[1] <= '9') {
                    *underscore = 0;
                }
                type->function_index = (u16)LookupLocatorVfxFn(type->name);
            } else {
                type->function_index = 65535;
            }
        } else if ((type->flags & CUTLOCATOR_SFX) != 0 && NuCutSceneRequestSFX != NULL) {
            type->function_index = (u16)NuCutSceneRequestSFX(type->name);
            if (type->function_index != 65535) {
                scene->flags |= CUTSCENE_HAS_SFX;
            }
        }
        if (type->function_index == 65535) {
            CutSceneMissingObjectDiagnostic(type->name);
        }
    }
}

static void CutSceneTriggerFixUp(CUT_SCENE_LOAD *scene, CUT_LOAD_TRIGGER_OWNER *owner) {
    CUT_LOAD_TRIGGER_SYSTEM *system = scene->trigger_system;
    i32 i;
    i32 j;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_TRIGGER_EVENT *event = &system->events[i];
        for (j = 0; j < owner->count; ++j) {
            event->index = -1;
            if (NuStrICmp(event->name, owner->entries[j].name) == 0) {
                event->index = (i16)j;
                break;
            }
        }
        if (event->index == -1) {
            CutSceneLoadDiagnostic(event->name);
        }
    }
}

static void CutSceneFindCharacters(CUT_SCENE_LOAD *scene) {
    if (NuCutSceneFindCharacters != NULL) {
        NuCutSceneFindCharacters(scene);
    }
}

void NuGCutSceneFixUp(void *asset, NUGSCN *gscene, void *extra_scene, i32 page) {
    CUT_SCENE_LOAD *scene = asset;
    if (scene != NULL) {
        if (scene->version >= 2) {
            scene->scene = gscene;
            scene->extra_scene = extra_scene;
        }
        if (scene->rigid_system != NULL) {
            CutSceneRigidFixUp(scene, gscene);
        }
        if (scene->character_system != NULL) {
            CutSceneFindCharacters(scene);
        }
        if (scene->locator_system != NULL) {
            CutSceneLocatorFixUp(scene, page);
        }
        if (scene->trigger_system != NULL && extra_scene != NULL) {
            CutSceneTriggerFixUp(scene, extra_scene);
        }
    }
}

static void CutSceneRigidFixUpExtra(CUT_SCENE_LOAD *scene, NUGSCN *gscene) {
    CUT_LOAD_RIGID_SYSTEM *system = scene->rigid_system;
    nuhspecial_s handle;
    i32 i;
    if (gscene != NULL) {
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_RIGID *rigid = &system->rigids[i];
            if ((rigid->flags & CUTRIGID_FOUND) == 0) {
                if (NuSpecialFind(gscene, &handle, rigid->name, 1) != 0) {
                    rigid->flags |= CUTRIGID_FOUND;
                    rigid->scene = handle.scene;
                    if (handle.display_special != NULL) {
                        rigid->special = handle.display_special;
                    } else {
                        rigid->special = handle.special;
                    }
                } else {
                    CutSceneMissingObjectDiagnostic(rigid->name);
                }
            }
        }
    }
}

void NuGCutSceneFixUpExtra(void *asset, NUGSCN *gscene) {
    CUT_SCENE_LOAD *scene = asset;
    if (scene != NULL && scene->rigid_system != NULL) {
        CutSceneRigidFixUpExtra(scene, gscene);
    }
}

void NuGCutRigidForceInstanced(void *asset) {
    CUT_LOAD_RIGID_SYSTEM *system = ((CUT_SCENE_LOAD *)asset)->rigid_system;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_RIGID *rigid = &system->rigids[i];
        rigid->flags |= CUTRIGID_INSTANCED;
    }
}

/* Places the instance's bounding-box centre in world space for its matrix. */
static void NuGCutSceneUpdateMtx(INSTNUGCUTSCENE *instance, NUMTX *matrix) {
    CUT_SCENE_LOAD *scene = instance->scene;
    if (scene->bounds != NULL) {
        instance->center.x = (scene->bounds->max.x + scene->bounds->min.x) * 0.5;
        instance->center.y = (scene->bounds->max.y + scene->bounds->min.y) * 0.5;
        instance->center.z = (scene->bounds->max.z + scene->bounds->min.z) * 0.5;
    } else {
        instance->center.x = 0.0f;
        instance->center.y = 0.0f;
        instance->center.z = 0.0f;
    }
    if (matrix != NULL) {
        NuVecMtxTransform(&instance->center, &instance->center, matrix);
    }
}

static CUT_LOAD_CAMERA_INSTANCE *CutCameraInstanceCreate(CUT_LOAD_CAMERA_SYSTEM *system, VARIPTR *buffer) {
    CUT_LOAD_CAMERA_INSTANCE *instance = NULL;
    if (system != NULL && system->count != 0 && buffer != NULL) {
        buffer->addr = (buffer->addr + 15) & ~15;
        instance = buffer->void_ptr;
        buffer->addr += 16;
        memset(instance, 0, sizeof(*instance));
        buffer->addr = (buffer->addr + 15) & ~15;
        instance->states = buffer->void_ptr;
        buffer->addr += system->count * 4;
        memset(instance->states, 0, system->count * 4);
    }
    return instance;
}

static CUT_LOAD_RIGID_INSTANCE_SYSTEM *CutRigidInstanceCreate(CUT_SCENE_LOAD *scene, VARIPTR *buffer, NUGSCN *gscene) {
    CUT_LOAD_RIGID_SYSTEM *system = scene->rigid_system;
    CUT_LOAD_RIGID_INSTANCE_SYSTEM *instance = NULL;
    i32 i;
    if (system != NULL && system->count != 0 && buffer != NULL) {
        buffer->addr = (buffer->addr + 15) & ~15;
        instance = buffer->void_ptr;
        buffer->addr += 4;
        instance->rigids = NULL;
        buffer->addr = (buffer->addr + 15) & ~15;
        instance->rigids = buffer->void_ptr;
        buffer->addr += system->count * 16;
        memset(instance->rigids, 0, system->count * 16);
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_RIGID *rigid = &system->rigids[i];
            CUT_LOAD_RIGID_INSTANCE *target = &instance->rigids[i];
            if (gscene != NULL && gscene != rigid->scene) {
                if (rigid->special != NULL) {
                    NUDISPLAY_LIST *display_list;
                    target->special.scene = gscene;
                    display_list = gscene->display_list;
                    if (display_list != NULL) {
                        i32 index = ((i32)rigid->special - (i32)rigid->scene->display_list->specials) / 0xd0;
                        target->special.display_special = &display_list->specials[index];
                        target->special.special = NULL;
                    } else {
                        i32 index = ((i32)rigid->special - (i32)rigid->scene->specials) / 0x4c;
                        target->special.special = (NUMTX *)&gscene->specials[index];
                        target->special.display_special = NULL;
                    }
                    if ((rigid->flags & CUTRIGID_INSTANCED) != 0) {
                        target->visible = rigid->flags & CUTRIGID_VISIBLE;
                    }
                } else {
                    CutSceneMissingObjectDiagnostic(rigid->name);
                }
            } else {
                NUGSCN *owner = rigid->scene;
                target->special.scene = owner;
                if (owner != NULL && owner->display_list != NULL) {
                    target->special.special = NULL;
                    target->special.display_special = rigid->special;
                } else {
                    target->special.special = rigid->special;
                    target->special.display_special = NULL;
                }
            }
        }
    }
    return instance;
}

static CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *CutCharacterInstanceCreate(CUT_SCENE_LOAD *scene, VARIPTR *buffer) {
    CUT_LOAD_CHARACTER_SYSTEM *system = scene->character_system;
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *instance = NULL;
    i32 i;
    if (system != NULL && system->count != 0 && buffer != NULL) {
        buffer->addr = (buffer->addr + 15) & ~15;
        instance = buffer->void_ptr;
        buffer->addr += 4;
        instance->characters = NULL;
        buffer->addr = (buffer->addr + 15) & ~15;
        instance->characters = buffer->void_ptr;
        buffer->addr += system->count * 24;
        memset(instance->characters, 0, system->count * 24);
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_CHARACTER *character = &system->characters[i];
            CUT_LOAD_CHARACTER_INSTANCE *target = &instance->characters[i];
            target->animation_b = 255;
            target->animation_a = 255;
            if ((character->flags & 2) != 0) {
                if (NuCutSceneCharacterCreateData != NULL) {
                    NuCutSceneCharacterCreateData(character, target, buffer);
                }
            } else {
                target->model = character->model;
            }
        }
    }
    return instance;
}

static CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *CutLocatorInstanceCreate(CUT_LOAD_LOCATOR_SYSTEM *system, VARIPTR *buffer) {
    CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *instance = NULL;
    i32 i;
    if (system != NULL && system->count != 0 && buffer != NULL) {
        buffer->addr = (buffer->addr + 15) & ~15;
        instance = buffer->void_ptr;
        buffer->addr += 4;
        instance->locators = NULL;
        buffer->addr = (buffer->addr + 15) & ~15;
        instance->locators = buffer->void_ptr;
        buffer->addr += system->count * 8;
        memset(instance->locators, 0, system->count * 8);
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_LOCATOR *locator = &system->locators[i];
            CUT_LOAD_LOCATOR_INSTANCE *target = &instance->locators[i];
            u8 flags = system->types[locator->type_index].flags;
            if ((flags & CUTLOCATOR_DEBRIS) != 0) {
                if ((locator->flags & CUTLOC_PERSISTENT) != 0) {
                    target->effect.index = -1;
                }
            } else if ((flags & CUTLOCATOR_FUNCTION) != 0) {
                /* This type has no per-instance allocation. */
            } else if ((flags & CUTLOCATOR_VFX) != 0) {
                target->effect.index = -1;
            } else if ((flags & CUTLOCATOR_LOOK_AT) != 0) {
                buffer->addr = (buffer->addr + 3) & ~3;
                target->effect.focus = buffer->void_ptr;
                buffer->addr += 24;
                memset(target->effect.focus, 0, sizeof(*target->effect.focus));
                if (locator->tracks_self != 0) {
                    target->effect.focus->self = locator;
                }
            } else if ((flags & CUTLOCATOR_SFX) != 0) {
                buffer->addr = (buffer->addr + 15) & ~15;
                target->effect.sound = buffer->void_ptr;
                buffer->addr += 16;
                memset(target->effect.sound, 0, sizeof(*target->effect.sound));
            }
        }
    }
    return instance;
}

static CUT_LOAD_TRIGGER_INSTANCE *CutTriggerInstanceCreate(VARIPTR *buffer, CUT_SCENE_LOAD *scene, CUT_LOAD_TRIGGER_OWNER *owner) {
    i32 count = scene->trigger_system->count;
    CUT_LOAD_TRIGGER_INSTANCE *instance;
    buffer->addr = (buffer->addr + 15) & ~15;
    instance = buffer->void_ptr;
    buffer->addr += 8;
    memset(instance, 0, sizeof(*instance));
    instance->owner = owner;
    instance->states = buffer->void_ptr;
    buffer->addr += count * sizeof(CUT_LOAD_TRIGGER_STATE);
    memset(instance->states, 0, count * sizeof(CUT_LOAD_TRIGGER_STATE));
    return instance;
}

static void CutSceneCreateContents(VARIPTR *buffer, char *name, INSTNUGCUTSCENE *instance, CUT_SCENE_LOAD *scene, NUGSCN *gscene,
                                   CUT_LOAD_TRIGGER_OWNER *extra_scene, VARIPTR *end) {
    if (scene != NULL) {
        instance->scene = scene;
        instance->saved_scene = scene;
        if (name != NULL) {
            sprintf(instance->name, name);
        }
        if (scene->camera_system != NULL) {
            instance->camera_instance = CutCameraInstanceCreate(scene->camera_system, buffer);
        }
        if (scene->rigid_system != NULL) {
            instance->rigid_instance = CutRigidInstanceCreate(scene, buffer, gscene);
        }
        if (scene->character_system != NULL) {
            instance->character_instance = CutCharacterInstanceCreate(scene, buffer);
        }
        if (scene->locator_system != NULL) {
            instance->locator_instance = CutLocatorInstanceCreate(scene->locator_system, buffer);
        }
        if (scene->trigger_system != NULL && extra_scene != NULL) {
            instance->trigger_instance = CutTriggerInstanceCreate(buffer, scene, extra_scene);
        }
        if (instance->scene->bounds != NULL) {
            instance->center.x = (f32)((instance->scene->bounds->min.x + instance->scene->bounds->max.x) * 0.5);
            instance->center.y = (f32)((instance->scene->bounds->min.y + instance->scene->bounds->max.y) * 0.5);
            instance->center.z = (f32)((instance->scene->bounds->min.z + instance->scene->bounds->max.z) * 0.5);
        } else {
            instance->center.x = 0.0f;
            instance->center.y = 0.0f;
            instance->center.z = 0.0f;
        }
        instance->rate = 1.0f;
        if (end != NULL && buffer->addr > end->addr) {
            ((void (*)(void))CutSceneLoadDiagnostic)();
        }
    }
}

INSTNUGCUTSCENE *instNuGCutSceneCreate(void *asset, NUGSCN *gscene, void *extra_scene, char *name, VARIPTR *buffer, i32 background) {
    CUT_SCENE_LOAD *scene = asset;
    INSTNUGCUTSCENE *instance = NULL;
    if (scene != NULL) {
        buffer->addr = (buffer->addr + 15) & ~15;
        instance = buffer->void_ptr;
        buffer->addr += 0xf8;
        if (instance == NULL) {
            return NULL;
        }
        memset(instance, 0, sizeof(*instance));
        instance->alpha = 1.0f;
        instance->scene_owner = gscene;
        instance->extra_scene_owner = extra_scene;
        instance->stream_index = 0;
        if (scene->version >= 2 && (scene->flags & CUTSCENE_STREAMED) != 0) {
            instance->stream_buffer_0 = scene->stream_buffer_0;
            instance->stream_buffer_1 = scene->stream_buffer_1;
            instance->pending_stream = scene->stream_buffer_1;
            instance->flags_88 &= ~NUGCUTSCENE_INST_STREAM_BUFFER_1;
            instance->stream_index = 0;
        }
        if (scene->version >= 2 && (scene->flags & (CUTSCENE_STREAMED | CUTSCENE_STREAM_PENDING)) != 0) {
            instance->flags_88 |= NUGCUTSCENE_INST_STREAMED;
        } else {
            instance->flags_88 &= ~NUGCUTSCENE_INST_STREAMED;
        }
        CutSceneCreateContents(buffer, name, instance, scene, gscene, extra_scene, NULL);
        NuThreadDisableThreadSwap();
        if (background != 0) {
            instance->next = CutSceneBackgroundInstances;
            if (instance->next != NULL) {
                instance->next->previous = instance;
            }
            CutSceneBackgroundInstances = instance;
        } else {
            instance->next = active_cutscene_instances;
            if (instance->next != NULL) {
                instance->next->previous = instance;
            }
            active_cutscene_instances = instance;
        }
        instance->allocation_size = buffer->addr - (u32)instance;
        NuThreadEnableThreadSwap();
    }
    return instance;
}

void fun_005000c0(void) {
    CutSceneBackgroundInstances = NULL;
}

void NuGCutSceneSysInit(CUT_LOCATOR_FUNCTION_ENTRY *functions) {
    CutSceneBackgroundInstances = NULL;
    active_cutscene_instances = NULL;
    CutSceneLocatorFunctions = functions;
}

void NuGCutSceneSysInitVfx(i32 (*lookup)(char *), i32 (*trigger)(i32, NUMTX *), void (*release)(i32), void (*update)(i32, NUMTX *)) {
    LookupLocatorVfxFn = lookup;
    TriggerLocatorVfxFn = trigger;
    ReleaseLocatorVfxFn = release;
    UpdateLocatorVfxFn = update;
}

void instNuGCutSceneResetCamLock(INSTNUGCUTSCENE *owner) {
    INSTNUGCUTSCENE *instance = owner;
    if (instance != NULL && instance->camera_instance != NULL && instance->camera_instance->camera_index >= 0) {
        CutSceneCameraCTRL = 0;
    }
}

void NuSetCutSceneGetHGObjFn(NUHGOBJ_EVAL *(*callback)(INSTNUGCUTSCENE *, i32)) {
    NuCutSceneGetHGObj = callback;
}

void NuSetCutSceneCharacterProcessFn(void (*callback)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32)) {
    NuCutSceneCharacterProcess = callback;
}

void NuSetCutSceneCharacterRenderFn(CUT_DRAW_RENDER_CALLBACK callback) {
    NuCutSceneCharacterRender = callback;
}

void NuSetCutSceneCharacterEvalFn(CUT_DRAW_EVAL_CALLBACK callback) {
    NuCutSceneCharacterEval = callback;
}

void NuSetCutSceneCharacterReleaseFn(void (*callback)(void *, void *)) {
    NuCutSceneCharacterRelease = callback;
}

void NuSetCutSceneFindCharactersFn(void (*callback)(CUT_SCENE_LOAD *)) {
    NuCutSceneFindCharacters = callback;
}

void NuSetCutSceneDestroyCharactersFn(void (*callback)(void *)) {
    NuCutSceneDestroyCharacters = callback;
}

void NuSetCutSceneResetCharactersFn(void (*callback)(INSTNUGCUTSCENE *)) {
    NuCutSceneResetCharacters = callback;
}

void NuSetCutSceneCharacterCreateDataFn(void (*callback)(CUT_LOAD_CHARACTER *, CUT_LOAD_CHARACTER_INSTANCE *, VARIPTR *)) {
    NuCutSceneCharacterCreateData = callback;
}

void NuSetCutSceneCharacterDestroyDataFn(void (*callback)(void *, void *)) {
    NuCutSceneCharacterDestroyData = callback;
}

void NuSetCutSceneRigidCollisionCheckFn(void (*callback)(CUT_LOAD_RIGID *, NUMTX *)) {
    NuCutSceneRigidCollisionCheck = callback;
}

void NuSetCutSceneRequestSFXFn(i32 (*callback)(char *)) {
    NuCutSceneRequestSFX = callback;
}

void NuSetCutSceneSFXUpdateFn(void (*callback)(CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *, CUT_LOAD_LOCATOR *, f32, NUMTX *,
                                               i32)) {
    NuCutSceneSFXUpdate = callback;
}

void NuSetCutSceneSFXFixUpFn(void (*callback)(INSTNUGCUTSCENE *)) {
    NuCutSceneSFXFixUp = callback;
}

void NuSetCutSceneRigidPostRenderFn(CUT_DRAW_RIGID_CALLBACK callback) {
    NuCutSceneRigidPostRender = callback;
}

void instNuGCutSceneEndFirstFrame(INSTNUGCUTSCENE *owner) {
    INSTNUGCUTSCENE *instance = owner;
    CUT_SCENE_LOAD *scene = instance->scene;
    instance->current_frame = 1.0f;
    instance->render_frame = 1.0f;
    instance->flags_88 &= ~(NUGCUTSCENE_INST_PLAYING | NUGCUTSCENE_INST_ENDED);
    instance->flags_8c &= ~NUGCUTSCENE_PLAY_HOLD_LAST_FRAME;
    if ((instance->flags_88 & 0x40000) != 0) {
        if (scene->rigid_system != NULL) {
            CutSceneEndRigidsPC(owner, scene->duration - 1.0f);
        }
        if (instance->character_instance != NULL) {
            CutSceneEndCharactersPC(owner, instance->scene->duration - instance->current_frame);
        }
    } else {
        if (scene->rigid_system != NULL) {
            CutSceneEndRigidsPC(owner, 1.0f);
        }
        if (instance->character_instance != NULL) {
            CutSceneEndCharactersPC(owner, instance->current_frame);
        }
    }
    if (instance->locator_instance != NULL) {
        CutSceneEndLocatorsPC(instance->locator_instance, scene->locator_system);
    }
    if (instance->camera_instance != NULL && instance->camera_instance->camera_index >= 0) {
        CutSceneCameraCTRL = 0;
    }
}

/* Jumps an instance to its last frame and settles the rigid and locator systems there. */
static void CutSceneJumpToEnd(INSTNUGCUTSCENE *instance, u16 flag) {
    CUT_SCENE_LOAD *scene;
    u32 flags = (instance->flags_88 & ~NUGCUTSCENE_INST_PLAYING) | ((flag & 1) << 1) | NUGCUTSCENE_INST_ENDED | NUGCUTSCENE_INST_ADVANCING;
    scene = instance->scene;
    instance->current_frame = scene->duration;
    instance->flags_88 = flags;
    instance->flags_8c = (instance->flags_8c & ~NUGCUTSCENE_PLAY_HOLD_LAST_FRAME) | ((flag & 1) << 6);
    ForcePlayEndFrame = 1;
    if ((instance->flags_88 & 0x40000) != 0) {
        if (scene->rigid_system != NULL) {
            CutSceneEndRigidsPC(instance, instance->scene->duration - instance->current_frame);
        }
    } else {
        if (scene->rigid_system != NULL) {
            CutSceneEndRigidsPC(instance, instance->current_frame);
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

/* Unreferenced; named after what they do. */
void instNuGCutSceneJumpToEnd(INSTNUGCUTSCENE *owner) {
    CutSceneJumpToEnd(owner, 0);
}

void instNuGCutSceneJumpToEndFlagged(INSTNUGCUTSCENE *owner) {
    CutSceneJumpToEnd(owner, 1);
}

void instNuGCutSceneSetTranslation(INSTNUGCUTSCENE *instance, NUVEC *translation) {
    instance->flags_88 |= NUGCUTSCENE_INST_OWN_MATRIX;
    NuMtxSetTranslation(&instance->matrix, translation);
    NuGCutSceneUpdateMtx(instance, &instance->matrix);
}

void instNuGCutSceneTranslate(INSTNUGCUTSCENE *instance, NUVEC *translation) {
    if ((instance->flags_88 & NUGCUTSCENE_INST_OWN_MATRIX) == 0) {
        instance->flags_88 |= NUGCUTSCENE_INST_OWN_MATRIX;
        NuMtxSetIdentity(&instance->matrix);
    }
    NuMtxTranslate(&instance->matrix, translation);
    NuGCutSceneUpdateMtx(instance, &instance->matrix);
}

void instNuGCutSceneSetMtx(INSTNUGCUTSCENE *instance, NUMTX *mtx) {
    instance->flags_88 |= NUGCUTSCENE_INST_OWN_MATRIX;
    instance->matrix = *mtx;
    NuGCutSceneUpdateMtx(instance, &instance->matrix);
}

void instNuGCutSceneRotateY(INSTNUGCUTSCENE *instance, NUANG angle) {
    instance->flags_88 |= NUGCUTSCENE_INST_OWN_MATRIX;
    NuMtxRotateY(&instance->matrix, angle);
    NuGCutSceneUpdateMtx(instance, &instance->matrix);
}

static void CutSceneRigidSysRender(INSTNUGCUTSCENE *instance, f32 frame, i32 paused) {
    CUT_SCENE_LOAD *scene = instance->scene;
    CUT_LOAD_RIGID_SYSTEM *system = scene->rigid_system;
    CUT_LOAD_RIGID_INSTANCE_SYSTEM *objects = instance->rigid_instance;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_RIGID *rigid = &system->rigids[i];
        CUT_LOAD_RIGID_INSTANCE *object = &objects->rigids[i];
        NUMTX matrix;
        u8 visible;
        if ((rigid->flags & CUTRIGID_INSTANCED) == 0 || (rigid->flags & CUTRIGID_FOUND) == 0) {
            continue;
        }
        if (rigid->state_animation != NULL && StateAnimEvaluate(rigid->state_animation, &object->state_index, &visible, frame) != 0) {
            object->visible = visible != 0;
        }
        if (object->visible == 0) {
            continue;
        }
        NuGCutRigidCalcMtx((CUT_RIGID_GENERIC *)rigid, frame, &matrix);
        if ((instance->flags_88 & NUGCUTSCENE_INST_OWN_MATRIX) != 0) {
            NuMtxMul(&matrix, &matrix, &instance->matrix);
        }
        if (instance->alpha == 1.0f) {
            NuSpecialDrawAt(&object->special, &matrix);
        } else {
            NuSpecialDrawAtAlpha(&object->special, &matrix, instance->alpha);
        }
        if (NuCutSceneRigidPostRender != NULL && (rigid->flags & 0x18) != 0) {
            NuCutSceneRigidPostRender(rigid, &object->special, &matrix);
        }
        if (NuCutSceneRigidCollisionCheck != NULL) {
            NuCutSceneRigidCollisionCheck(rigid, &matrix);
        }
        if (rigid->locator_index != 0xff) {
            CUT_LOAD_LOCATOR_SYSTEM *locators = scene->locator_system;
            CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *states = instance->locator_instance;
            i32 j;
            for (j = 0; j < rigid->uses_locator; ++j) {
                i32 index = rigid->locator_index + j;
                instNuGCutLocatorUpdate(instance, locators, &states->locators[index], &locators->locators[index], frame, &matrix, paused);
            }
        }
    }
}

static void CutSceneLocatorSysUpdate(INSTNUGCUTSCENE *instance, f32 frame, i32 paused) {
    CUT_LOAD_LOCATOR_SYSTEM *system = instance->scene->locator_system;
    CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *states = instance->locator_instance;
    u32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_LOCATOR *locator = &system->locators[i];
        if ((locator->flags & 3) == 0) {
            if ((instance->flags_88 & NUGCUTSCENE_INST_OWN_MATRIX) != 0) {
                instNuGCutLocatorUpdate(instance, system, &states->locators[i], locator, frame, &instance->matrix, paused);
            } else {
                instNuGCutLocatorUpdate(instance, system, &states->locators[i], locator, frame, NULL, paused);
            }
        }
    }
}

static void CutSceneCharacterSysRender(INSTNUGCUTSCENE *instance, f32 frame, i32 paused) {
    CUT_SCENE_LOAD *scene = instance->scene;
    CUT_LOAD_CHARACTER_SYSTEM *system = scene->character_system;
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *states = instance->character_instance;
    i32 i;
    if (NuCutSceneCharacterRender != NULL) {
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_CHARACTER_INSTANCE *state = &states->characters[i];
            if (state->model != NULL) {
                NuCutSceneCharacterRender(instance, scene, state, &system->characters[i], frame, paused);
            }
        }
    }
}

static void CutSceneInstanceRender(INSTNUGCUTSCENE *instance, i32 paused) {
    INSTNUGCUTSCENE *owner = instance;

    if ((instance->flags_88 & NUGCUTSCENE_INST_SUSPENDED) != 0 || (instance->flags_88 & NUGCUTSCENE_INST_PLAYING) == 0 ||
        (instance->flags_88 & NUGCUTSCENE_INST_VISIBLE) == 0) {
        return;
    }
    if ((instance->flags_88 & 0x40000) != 0) {
        if (CutSceneRenderRigids != 0 && instance->rigid_instance != NULL) {
            CutSceneRigidSysRender(owner, instance->scene->duration - instance->render_frame, paused);
        }
        if (CutSceneRenderCharacters != 0 && instance->character_instance != NULL) {
            CutSceneCharacterSysRender(owner, instance->scene->duration - instance->render_frame, paused);
        }
        if ((instance->flags_8c & NUGCUTSCENE_PLAY_HOLD_LAST_FRAME) != 0 && instance->current_frame == instance->scene->duration - 1.0f) {
            return;
        }
        if ((instance->flags_88 & NUGCUTSCENE_INST_SKIPPING) != 0) {
            return;
        }
        if (CutSceneRenderLocators != 0 && instance->locator_instance != NULL) {
            CutSceneLocatorSysUpdate(owner, instance->scene->duration - instance->render_frame, paused);
        }
    } else {
        if (CutSceneRenderRigids != 0 && instance->rigid_instance != NULL) {
            CutSceneRigidSysRender(owner, instance->render_frame, paused);
        }
        if (CutSceneRenderCharacters != 0 && instance->character_instance != NULL) {
            CutSceneCharacterSysRender(owner, instance->render_frame, paused);
        }
        if ((instance->flags_8c & NUGCUTSCENE_PLAY_HOLD_LAST_FRAME) != 0 && instance->current_frame == instance->scene->duration - 1.0f) {
            return;
        }
        if ((instance->flags_88 & NUGCUTSCENE_INST_SKIPPING) != 0) {
            return;
        }
        if (CutSceneRenderLocators != 0 && instance->locator_instance != NULL) {
            CutSceneLocatorSysUpdate(owner, instance->render_frame, paused);
        }
    }
}

void NuGCutSceneSysRender(i32 paused) {
    INSTNUGCUTSCENE *owner;
    for (owner = active_cutscene_instances; owner != NULL; owner = owner->next) {
        CutSceneInstanceRender(owner, paused);
    }
}

void fun_00508f30(void) {
}

static void CutSceneCameraSysUpdate(INSTNUGCUTSCENE *instance, f32 frame) {
    CUT_LOAD_CAMERA_SYSTEM *system = instance->scene->camera_system;
    CUT_LOAD_CAMERA_INSTANCE *state = instance->camera_instance;
    CUT_LOAD_CAMERA *camera;
    CUT_LOAD_CAMERA_STATE *camera_state;
    CUT_LOAD_CAMERA_TARGET *target;
    u8 value;
    u8 focus_value;
    i32 index;
    f32 signed_duration;
    f32 duration;
    f32 blend;
    i32 reverse;
    NUMTX look_at;
    NUVEC translation;
    NUQUAT target_rotation;
    NUQUAT camera_rotation;
    NUQUAT blended_rotation;
    CutSceneCameraFocalLength = 0.0f;
    CutSceneFocusDistance = 0.0f;
    CutSceneUseFocusLocation = 0;
    CutSceneFStop = 0.0f;
    if ((instance->scene->version & 0xff) >= 5 && system->focus_state_animation != NULL &&
        StateAnimEvaluate(system->focus_state_animation, &state->focus_state_index, &focus_value, frame) != 0) {
        if (focus_value != 0xff) {
            state->focus_index = (i8)instance->scene->focus_indices[focus_value];
        } else {
            state->focus_index = -1;
        }
    }
    if (system->state_animation != NULL) {
        if (StateAnimEvaluate(system->state_animation, &state->state_index, &value, frame) != 0) {
            state->camera_index = (i8)value;
            instance->flags_8c |= NUGCUTSCENE_PLAY_CAMERA_CHANGED;
            CutSceneCameraChanged = 1;
        }
        if (state->state_index < system->state_animation->count) {
            if (system->state_animation->times[state->state_index] - frame < 1.0f &&
                system->state_animation->values[state->state_index] != state->camera_index) {
                f32 render_frame = system->state_animation->times[state->state_index] - 1.0f;
                instance->render_frame = render_frame;
                if ((instance->flags_88 & 0x40000) != 0) {
                    instance->render_frame = instance->scene->duration - render_frame;
                }
                if (instance->render_frame < 1.0f) {
                    instance->render_frame = 1.0f;
                }
            }
        }
    }
    while (state->next_target_index < state->target_count && frame >= state->targets[state->next_target_index].start_frame) {
        camera_state = &state->states[system->target_camera_map[state->targets[state->next_target_index].target_index]];
        camera_state->flags |= 2;
        camera_state->event_index = state->next_target_index;
        ++state->next_target_index;
    }
    while (state->next_target_index > 0 && frame < state->targets[state->next_target_index - 1].start_frame) {
        --state->next_target_index;
        camera_state = &state->states[system->target_camera_map[state->targets[state->next_target_index].target_index]];
        camera_state->flags |= 2;
        camera_state->event_index = state->next_target_index;
    }
    if (state->camera_index >= 0) {
        CutSceneCameraCTRL = 1;
        CutSceneCameraDofMode = 2;
    } else {
        CutSceneCameraCTRL = 0;
    }
    if (state->camera_index < 0) {
        return;
    }
    index = state->camera_index;
    if ((u32)index >= (u32)system->count) {
        ((void (*)(void))CutSceneLoadDiagnostic)();
    }
    camera = &system->cameras[index];
    camera_state = &state->states[index];
    if ((camera->flags & 1) == 0 || camera->animation_node >= NuAnimNumNodes(system->animation)) {
        CutSceneCameraMatrix = camera->matrix;
    } else {
        ANI3_HEADER *animation;
        ANI3_HEADER *focus = system->focus_animation;
        if ((instance->scene->version & 0xff) >= 5 && focus != NULL && focus->magic >= 0x414e4934 && focus->magic <= 0x414e4935) {
            f32 *values = NuAnimCurveExtractAllNodeCurves_3(focus, camera->animation_node, instance->render_frame, NULL);
            CutSceneFStop = values[2];
            CutSceneCameraFocalLength = (f32)(values[0] * (f64)1.3f);
            if ((camera->focus_flags & 2) != 0) {
                CutSceneFocusDistance = values[1];
            } else if (system->focus_state_animation != NULL && state->focus_index >= 0) {
                CUT_LOAD_LOCATOR *locator = &instance->scene->locator_system->locators[state->focus_index];
                if (locator->animation != NULL) {
                    NUMTX focus_matrix;
                    NuGCutLocatorCalcMtx((CUT_LOCATOR_GENERIC *)locator, frame, &focus_matrix, NULL);
                    if ((locator->flags & CUTLOC_PIVOT) != 0) {
                        NuMtxPreTranslate(&focus_matrix, &locator->pivot);
                    }
                    if ((instance->flags_88 & NUGCUTSCENE_INST_OWN_MATRIX) != 0) {
                        NuMtxMul(&focus_matrix, &focus_matrix, &instance->matrix);
                    }
                    NuMtxGetTranslation(&focus_matrix, &CutSceneFocusLocation);
                    CutSceneUseFocusLocation = 1;
                }
            }
        }
        animation = system->animation;
        if (animation->magic >= 0x414e4934 && animation->magic <= 0x414e4935) {
            NuAnimCurve2SetApplyToMatrix_3(animation, camera->animation_node, instance->render_frame, &CutSceneCameraMatrix);
        } else {
            ((void (*)(void))CutSceneLoadDiagnostic)();
        }
    }
    if ((instance->flags_88 & NUGCUTSCENE_INST_OWN_MATRIX) != 0) {
        NuMtxMul(&CutSceneCameraMatrix, &CutSceneCameraMatrix, &instance->matrix);
    }
    CutSceneCameraMatrixSet = 1;
    if ((camera_state->flags & 2) == 0) {
        return;
    }
    target = &state->targets[camera_state->event_index];
    // MATCH-EXCEPTION: the original takes the absolute value by clearing the sign bit in an integer register.
    signed_duration = target->duration;
    *(u32 *)&duration = *(u32 *)&signed_duration & 0x7fffffff;
    reverse = NuFsign(target->duration) < 0.0f;
    if (reverse && frame >= target->start_frame + duration) {
        camera_state->flags &= ~2;
        return;
    }
    translation = CutSceneCameraMatrix.translation;
    look_at = CutSceneCameraMatrix;
    NuMtxLookAtZ(&look_at, target->target);
    blend = 1.0f;
    if (duration > 0.01f) {
        blend = (frame - target->start_frame) / duration;
    }
    if (blend > 1.0f || (!(blend < 0.0f) && reverse)) {
        blend = 1.0f - blend;
    }
    NuMtxToQuat(&look_at, &target_rotation);
    NuMtxToQuat(&CutSceneCameraMatrix, &camera_rotation);
    NuQuatSlerp(&blended_rotation, &camera_rotation, &target_rotation, blend);
    NuQuatToMtx(&blended_rotation, &CutSceneCameraMatrix);
    CutSceneCameraMatrix.translation = translation;
}

static void CutSceneRigidSysUpdate(INSTNUGCUTSCENE *instance, f32 frame, i32 paused) {
    CUT_SCENE_LOAD *scene = instance->scene;
    CUT_LOAD_RIGID_SYSTEM *system = scene->rigid_system;
    CUT_LOAD_RIGID_INSTANCE_SYSTEM *objects = instance->rigid_instance;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_RIGID *rigid = &system->rigids[i];
        CUT_LOAD_RIGID_INSTANCE *object = &objects->rigids[i];
        NUMTX matrix;
        u8 visible;
        if ((rigid->flags & CUTRIGID_INSTANCED) != 0 || (rigid->flags & CUTRIGID_FOUND) == 0) {
            continue;
        }
        if (rigid->state_animation != NULL && StateAnimEvaluate(rigid->state_animation, &object->state_index, &visible, frame) != 0) {
            if (visible != 0) {
                NuSpecialSetVisibility(&object->special, 1);
            } else {
                NuSpecialSetVisibility(&object->special, 0);
            }
        }
        if (NuSpecialGetVisibilityFn(&object->special) == 0) {
            continue;
        }
        NuGCutRigidCalcMtx((CUT_RIGID_GENERIC *)rigid, frame, &matrix);
        if ((instance->flags_88 & NUGCUTSCENE_INST_OWN_MATRIX) != 0) {
            NuMtxMul(&matrix, &matrix, &instance->matrix);
        }
        NuSpecialSetDrawMtx(&object->special, &matrix);
        if (NuSpecialGetVisibilityFn(&object->special) != 0 && rigid->locator_index != 0xff) {
            CUT_LOAD_LOCATOR_SYSTEM *locators = scene->locator_system;
            CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *states = instance->locator_instance;
            i32 j;
            for (j = 0; j < rigid->uses_locator; ++j) {
                i32 index = rigid->locator_index + j;
                instNuGCutLocatorUpdate(instance, locators, &states->locators[index], &locators->locators[index], frame, &matrix, paused);
            }
        }
    }
}

static void CutSceneTriggerSysUpdate(INSTNUGCUTSCENE *instance, f32 frame) {
    CUT_LOAD_TRIGGER_INSTANCE *states = instance->trigger_instance;
    CUT_LOAD_TRIGGER_SYSTEM *system = instance->scene->trigger_system;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_TRIGGER_EVENT *event = &system->events[i];
        u8 value;
        if (event->state_animation != NULL) {
            CUT_LOAD_TRIGGER_STATE *state = &states->states[i];
            if (StateAnimEvaluate(event->state_animation, &state->state_index, &value, frame) != 0) {
                if (value != 0) {
                    states->owner->trigger_states[event->index].flags |= 1;
                } else {
                    states->owner->trigger_states[event->index].flags &= ~1;
                }
            }
        }
    }
}

static void CutCameraSysStart(CUT_LOAD_CAMERA_INSTANCE *state, CUT_LOAD_CAMERA_SYSTEM *system) {
    u32 i;
    state->state_index = 0;
    state->camera_index = system->starting_camera;
    state->next_target_index = 0;
    for (i = 0; i < (u32)system->count; ++i) {
        CUT_LOAD_CAMERA_STATE *camera_state = &state->states[i];
        camera_state->flags &= ~2;
        camera_state->event_index = 0;
    }
}

static void CutSceneRigidSysStart(CUT_LOAD_RIGID_INSTANCE_SYSTEM *states, CUT_LOAD_RIGID_SYSTEM *system) {
    u32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_RIGID *rigid = &system->rigids[i];
        CUT_LOAD_RIGID_INSTANCE *state = &states->rigids[i];
        state->state_index = 0;
        if ((rigid->flags & CUTRIGID_FOUND) != 0 && (rigid->flags & CUTRIGID_INSTANCED) != 0) {
            state->visible = rigid->flags & CUTRIGID_VISIBLE;
        }
    }
}

static void CutSceneLocatorSysStart(CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *states, CUT_LOAD_LOCATOR_SYSTEM *system) {
    u32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_LOCATOR_INSTANCE *state = &states->locators[i];
        state->frame = 0.0f;
        if ((system->types[system->locators[i].type_index].flags & CUTLOCATOR_VFX) != 0) {
            state->effect.index = -1;
        }
    }
}

static void CutTriggerSysStart(INSTNUGCUTSCENE *instance) {
    CUT_LOAD_TRIGGER_INSTANCE *states = instance->trigger_instance;
    CUT_LOAD_TRIGGER_SYSTEM *system = instance->scene->trigger_system;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        states->states[i].state_index = 0;
    }
}

void instNuGCutSceneStart(INSTNUGCUTSCENE *instance) {
    instance->current_frame = 1.0f;
    instance->render_frame = 1.0f;
    instance->flags_8c &= ~NUGCUTSCENE_PLAY_CAMERA_CHANGED;
    instance->flags_88 = (instance->flags_88 & ~(NUGCUTSCENE_INST_ADVANCING | NUGCUTSCENE_INST_ENDED)) | NUGCUTSCENE_INST_PLAYING;
    if (instance->rate < 0.0f) {
        instance->rate = -instance->rate;
    }
    if (instance->camera_instance != NULL) {
        CutCameraSysStart(instance->camera_instance, instance->scene->camera_system);
    }
    if (instance->rigid_instance != NULL) {
        CutSceneRigidSysStart(instance->rigid_instance, instance->scene->rigid_system);
    }
    if (instance->locator_instance != NULL) {
        CutSceneLocatorSysStart(instance->locator_instance, instance->scene->locator_system);
    }
    if (instance->trigger_instance != NULL) {
        CutTriggerSysStart(instance);
    }
    if (instance->skip_countdown != 0 && instance->stream_index == 0) {
        instance->flags_88 |= NUGCUTSCENE_INST_SKIPPING;
        instance->flags_8c = (instance->flags_8c & ~5U) | 0xa;
    }
}

i32 fun_00502620(INSTNUGCUTSCENE *instance) {
    CUT_LOAD_RIGID_SYSTEM *system = instance->scene->rigid_system;
    i32 count = system->count;
    i32 i;
    for (i = 0; i < count; ++i) {
        if ((system->rigids[i].flags & CUTRIGID_FOUND) != 0 && (system->rigids[i].flags & CUTRIGID_INSTANCED) != 0) {
            return 1;
        }
    }
    return 0;
}

static void CutSceneRigidSysReset(CUT_LOAD_RIGID_INSTANCE_SYSTEM *states, CUT_LOAD_RIGID_SYSTEM *system) {
    u32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_RIGID *rigid = &system->rigids[i];
        CUT_LOAD_RIGID_INSTANCE *state = &states->rigids[i];
        state->state_index = 0;
        if ((rigid->flags & CUTRIGID_FOUND) != 0) {
            if ((rigid->flags & CUTRIGID_INSTANCED) != 0) {
                state->visible = rigid->flags & CUTRIGID_VISIBLE;
            } else {
                if ((rigid->flags & CUTRIGID_VISIBLE) != 0) {
                    NuSpecialSetVisibility(&state->special, 1);
                } else {
                    NuSpecialSetVisibility(&state->special, 0);
                }
                NuSpecialSetDrawMtx(&state->special, &rigid->matrix);
            }
        }
    }
}

static void CutSceneLocatorSysReset(INSTNUGCUTSCENE *instance) {
    CUT_LOAD_LOCATOR_SYSTEM *system = instance->scene->locator_system;
    CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *states = instance->locator_instance;
    u32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_LOCATOR_INSTANCE *state = &states->locators[i];
        CUT_LOAD_LOCATOR *locator = &system->locators[i];
        CUT_LOAD_LOCATOR_TYPE *type = &system->types[locator->type_index];
        if ((type->flags & CUTLOCATOR_FUNCTION) != 0 && type->function_index != 0xffff &&
            CutSceneLocatorFunctions[type->function_index].reset != NULL) {
            CutSceneLocatorFunctions[type->function_index].reset(instance, system, state, locator);
        }
        if ((type->flags & CUTLOCATOR_VFX) != 0 && type->function_index != 0xffff) {
            state->effect.index = -1;
        }
    }
}

static void CutSceneTriggerSysReset(INSTNUGCUTSCENE *instance) {
    CUT_LOAD_TRIGGER_INSTANCE *states = instance->trigger_instance;
    CUT_LOAD_TRIGGER_SYSTEM *system = instance->scene->trigger_system;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_TRIGGER_EVENT *event = &system->events[i];
        u8 value;
        CUT_LOAD_TRIGGER_STATE *state = &states->states[i];
        state->state_index = 0;
        if (StateAnimEvaluate(event->state_animation, &state->state_index, &value, 1.0f) != 0) {
            if (value != 0) {
                states->owner->trigger_states[event->index].flags |= 1;
            } else {
                states->owner->trigger_states[event->index].flags &= ~1;
            }
        }
    }
}

void instNuGCutSceneReset(INSTNUGCUTSCENE *instance) {
    if (instance == NULL) {
        return;
    }
    instance->flags_88 &= ~0x1007U;
    instance->flags_8c &= ~0x1040U;
    instance->current_frame = 1.0f;
    instance->render_frame = 1.0f;
    instance->chained_instance = NULL;
    instance->end_callback = NULL;
    if (instance->rate < 0.0f) {
        instance->rate = -instance->rate;
    }
    instance->scene = instance->saved_scene;
    if (instance->rigid_instance != NULL) {
        CutSceneRigidSysReset(instance->rigid_instance, instance->scene->rigid_system);
    }
    if (instance->locator_instance != NULL) {
        CutSceneLocatorSysReset(instance);
    }
    if (instance->trigger_instance != NULL) {
        CutSceneTriggerSysReset(instance);
    }
    if (instance->character_instance != NULL && NuCutSceneResetCharacters != NULL) {
        NuCutSceneResetCharacters(instance);
    }
    if (instance->camera_instance != NULL && instance->camera_instance->camera_index >= 0) {
        CutSceneCameraCTRL = 0;
    }
}

i32 CutSceneRenderRigids = 1;
i32 CutSceneRenderCharacters = 1;
i32 CutSceneRenderLocators = 1;
i32 CutSceneSingleStreamBuffer = 2;
i32 CutSceneUpdateCameras = 1;

static void copyAnims(CUT_SCENE_LOAD *target, CUT_SCENE_LOAD *source) {
    i32 i;
    CUT_LOAD_CAMERA_SYSTEM *source_camera = source->camera_system;
    CUT_LOAD_CAMERA_SYSTEM *target_camera = target->camera_system;
    CUT_LOAD_LOCATOR_SYSTEM *source_locators;
    CUT_LOAD_LOCATOR_SYSTEM *target_locators;
    CUT_LOAD_RIGID_SYSTEM *source_rigids;
    CUT_LOAD_RIGID_SYSTEM *target_rigids;
    CUT_LOAD_CHARACTER_SYSTEM *source_characters;
    CUT_LOAD_CHARACTER_SYSTEM *target_characters;
    CUT_LOAD_TRIGGER_SYSTEM *source_triggers;
    CUT_LOAD_TRIGGER_SYSTEM *target_triggers;

    if ((target->version & 0xff) >= 5) {
        target_camera->focus_animation = source_camera->focus_animation;
        target_camera->focus_state_animation = source_camera->focus_state_animation;
    }
    if (source_camera->animation != NULL) {
        target_camera->animation = source_camera->animation;
    }
    target_camera->state_animation = source_camera->state_animation;
    source_locators = source->locator_system;
    if (source_locators != NULL) {
        target_locators = target->locator_system;
        if (source_locators->locators != NULL) {
            for (i = 0; i < source_locators->count; ++i) {
                CUT_LOAD_LOCATOR *from = &source_locators->locators[i];
                if (from->animation != NULL) {
                    target_locators->locators[i].animation = from->animation;
                }
            }
        }
    }
    source_rigids = source->rigid_system;
    if (source_rigids != NULL) {
        target_rigids = target->rigid_system;
        if (source_rigids->rigids != NULL) {
            for (i = 0; i < source_rigids->count; ++i) {
                CUT_LOAD_RIGID *from = &source_rigids->rigids[i];
                CUT_LOAD_RIGID *to = &target_rigids->rigids[i];
                if (from->animation != NULL) {
                    to->animation = from->animation;
                }
                to->state_animation = from->state_animation;
            }
        }
    }
    source_characters = source->character_system;
    if (source_characters != NULL) {
        target_characters = target->character_system;
        if (source_characters->characters != NULL) {
            for (i = 0; i < source_characters->count; ++i) {
                CUT_LOAD_CHARACTER *from = &source_characters->characters[i];
                CUT_LOAD_CHARACTER *to = &target_characters->characters[i];
                if (from->animation != NULL) {
                    to->animation = from->animation;
                }
                if (from->face_animation != NULL) {
                    to->face_animation = from->face_animation;
                }
                if (from->extra_animation != NULL) {
                    to->extra_animation = from->extra_animation;
                }
            }
        }
    }
    source_triggers = source->trigger_system;
    if (source_triggers != NULL) {
        target_triggers = target->trigger_system;
        if (source_triggers->events != NULL) {
            for (i = 0; i < source_triggers->count; ++i) {
                target_triggers->events[i].state_animation = source_triggers->events[i].state_animation;
            }
        }
    }
    target->duration = source->duration;
}

void CutSceneSwapStreamAsset(INSTNUGCUTSCENE *instance) {
    CUT_SCENE_LOAD *saved = instance->saved_scene;
    // MATCH-EXCEPTION: the original reads the buffer select as a signed one-bit field and masks it to index the pair.
    CUT_SCENE_LOAD *scene = instance->stream_buffers[instance->active_stream_buffer & 1];
    CUT_LOAD_LOCATOR_SYSTEM *locators;
    CUT_LOAD_LOCATOR_SYSTEM *saved_locators;
    CUT_LOAD_RIGID_SYSTEM *rigids;
    CUT_LOAD_RIGID_SYSTEM *saved_rigids;
    CUT_LOAD_CHARACTER_SYSTEM *characters;
    CUT_LOAD_CHARACTER_SYSTEM *saved_characters;
    CUT_LOAD_TRIGGER_SYSTEM *triggers;
    i32 i;
    instance->scene = scene;
    scene->filename = saved->filename;
    scene->stream_buffer_0 = saved->stream_buffer_0;
    scene->stream_buffer_1 = saved->stream_buffer_1;
    scene->flags = (scene->flags & ~2U) | (saved->flags & 2);
    saved_locators = saved->locator_system;
    locators = scene->locator_system;
    if (locators != NULL) {
        for (i = 0; i < locators->count; ++i) {
            void *animation = locators->locators[i].animation;
            locators->locators[i] = saved_locators->locators[i];
            locators->locators[i].animation = animation;
        }
        locators->types = saved_locators->types;
    }
    saved_rigids = saved->rigid_system;
    rigids = scene->rigid_system;
    if (rigids != NULL && rigids->rigids != NULL) {
        for (i = 0; i < rigids->count; ++i) {
            void *animation = rigids->rigids[i].animation;
            STATE_ANIM *state_animation = rigids->rigids[i].state_animation;
            rigids->rigids[i] = saved_rigids->rigids[i];
            rigids->rigids[i].animation = animation;
            rigids->rigids[i].state_animation = state_animation;
            if (rigids->rigids[i].locator.pointer != NULL && rigids->rigids[i].uses_locator != 0 &&
                rigids->rigids[i].locator_index != 0xff) {
                rigids->rigids[i].locator.pointer = &scene->locator_system->locators[rigids->rigids[i].locator_index];
            }
        }
    }
    saved_characters = saved->character_system;
    characters = scene->character_system;
    if (characters != NULL) {
        for (i = 0; i < characters->count; ++i) {
            void *animation = characters->characters[i].animation;
            void *face = characters->characters[i].face_animation;
            void *extra = characters->characters[i].extra_animation;
            CUT_LOAD_LOCATOR *locator = characters->characters[i].locator;
            characters->characters[i] = saved_characters->characters[i];
            characters->characters[i].animation = animation;
            characters->characters[i].face_animation = face;
            characters->characters[i].extra_animation = extra;
            if (locator != NULL && characters->characters[i].uses_locator != 0 && characters->characters[i].locator_index != 0xff) {
                characters->characters[i].locator = &scene->locator_system->locators[characters->characters[i].locator_index];
            }
        }
    }
    triggers = scene->trigger_system;
    if (triggers != NULL) {
        CUT_LOAD_TRIGGER_EVENT *from = saved->trigger_system->events;
        CUT_LOAD_TRIGGER_EVENT *to = triggers->events;
        for (i = 0; i < triggers->count; ++i, ++from, ++to) {
            STATE_ANIM *animation = to->state_animation;
            *to = *from;
            to->state_animation = animation;
        }
    }
}

static void instNuGCutSceneClipTest(INSTNUGCUTSCENE *instance) {
    CUT_SCENE_LOAD *scene = instance->scene;
    instance->flags_88 |= NUGCUTSCENE_INST_VISIBLE;
    if (scene->bounds == NULL) {
        return;
    }
    if ((instance->flags_88 & NUGCUTSCENE_INST_CLIP_RADIUS) != 0) {
        f32 distance = NuCameraDistSqr(&instance->center);
        if (distance > instance->radius_squared) {
            instance->flags_88 &= ~NUGCUTSCENE_INST_VISIBLE;
            return;
        }
    }
    if ((instance->flags_88 & NUGCUTSCENE_INST_CLIP_BOUNDS) != 0) {
        CUT_LOAD_BOUNDS *bounds = scene->bounds;
        i32 visible;
        if ((instance->flags_88 & NUGCUTSCENE_INST_OWN_MATRIX) != 0) {
            visible = NuCameraClipTestExtents(&bounds->min, &bounds->max, &instance->matrix, 0.0f, 0);
        } else {
            visible = NuCameraClipTestExtents(&bounds->min, &bounds->max, &NuMtxSetIdentityTemplate, 0.0f, 0);
        }
        if (visible == 0) {
            instance->flags_88 &= ~NUGCUTSCENE_INST_VISIBLE;
        }
    }
}

static void CutSceneStreamSkip(INSTNUGCUTSCENE *instance, f32 elapsed) {
    if (instance->stream_index == instance->saved_scene->last_stream) {
        instance->current_frame = instance->scene->duration - 1.0f;
    }
    instance->current_frame -= instance->rate * elapsed;
    if (CutSceneLoadingInstance != NULL) {
        return;
    }
    if (instance->stream_index == instance->saved_scene->last_stream - 1) {
        if ((instance->flags_8c & NUGCUTSCENE_PLAY_STREAM_LOADING) != 0) {
            CutSceneStreamDiagnostic(instance->scene->filename);
        } else if (instance->pending_stream != NULL) {
            CutSceneStreamDiagnostic(instance->scene->filename);
        } else {
            if ((instance->flags_88 & NUGCUTSCENE_INST_STREAM_BUFFER_1) != 0) {
                instance->pending_stream = instance->stream_buffer_1;
                instance->flags_88 &= ~NUGCUTSCENE_INST_STREAM_BUFFER_1;
            } else {
                instance->pending_stream = instance->stream_buffer_0;
                instance->flags_88 |= NUGCUTSCENE_INST_STREAM_BUFFER_1;
            }
            CutSceneStreamDiagnostic(instance->scene->filename);
        }
        if (CutSceneSingleStreamBuffer < 2) {
            copyAnims(instance->scene, instance->stream_buffers[instance->active_stream_buffer & 1]);
        } else {
            CutSceneSwapStreamAsset(instance);
        }
        instance->stream_index = instance->scene->last_stream;
        instNuGCutSceneStart(instance);
        instance->current_frame = instance->scene->duration - 1.0f;
    } else {
        instance->stream_index = (u8)(instance->scene->last_stream - 1);
        instance->pending_stream =
            (instance->flags_88 & NUGCUTSCENE_INST_STREAM_BUFFER_1) != 0 ? instance->stream_buffer_0 : instance->stream_buffer_1;
        CutSceneLoadingInstance = instance;
    }
}

static i32 cutscene_synchro_counter = 0;
u8 CutSceneCameraMatrixSet = 0;
u8 CutSceneCameraCTRL = 0;
u8 CutSceneUseFocusLocation = 0;
i32 CutSceneCameraDofMode = 0;
f32 CutSceneCameraFocalLength = 0;
f32 CutSceneFocusDistance = 0.0f;
f32 CutSceneFStop = 0.0f;
i32 CutSceneCameraChanged = 0;
static i32 CutSceneQuitPrompt = 0;
static i32 CutSceneHasQuit = 0;
INSTNUGCUTSCENE *CutSceneLoadingInstance = NULL;
void *cutscene_load_instance = 0;
static i32 CutSceneLockPlayer = 0;
static i32 CutSceneTerminateStream = 0;
NUHGOBJ_EVAL *(*NuCutSceneGetHGObj)(INSTNUGCUTSCENE *, i32) = 0;
void (*NuCutSceneCharacterProcess)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32) = NULL;
void (*NuCutSceneCharacterRender)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32) = 0;
void (*NuCutSceneCharacterRelease)(void *, void *) = 0;
void (*NuCutSceneCharacterEval)(INSTNUGCUTSCENE *, void *, void *, void *, f32) = 0;
void (*NuCutSceneFindCharacters)(CUT_SCENE_LOAD *) = 0;
void (*NuCutSceneDestroyCharacters)(void *) = 0;
void (*NuCutSceneResetCharacters)(INSTNUGCUTSCENE *) = 0;
void (*NuCutSceneCharacterCreateData)(CUT_LOAD_CHARACTER *, CUT_LOAD_CHARACTER_INSTANCE *, VARIPTR *) = 0;
void (*NuCutSceneCharacterDestroyData)(void *, void *) = 0;
void (*NuCutSceneRigidCollisionCheck)(struct cut_load_rigid_s *, NUMTX *) = NULL;
void (*NuCutSceneRigidPostRender)(void *, nuhspecial_s *, NUMTX *) = 0;
i32 (*NuCutSceneRequestSFX)(char *) = 0;
void (*NuCutSceneSFXUpdate)(CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *, CUT_LOAD_LOCATOR *, f32, NUMTX *, i32) = 0;
void (*NuCutSceneSFXFixUp)(INSTNUGCUTSCENE *) = NULL;
CUT_LOCATOR_FUNCTION_ENTRY *CutSceneLocatorFunctions = 0;
i32 CutSceneStreamLoadGuard = 0;

static void CutSceneCharacterSysUpdate(INSTNUGCUTSCENE *instance, f32 frame, i32 paused) {
    CUT_SCENE_LOAD *scene = instance->scene;
    CUT_LOAD_CHARACTER_SYSTEM *system = scene->character_system;
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *states = instance->character_instance;
    i32 i;
    if (NuCutSceneCharacterProcess != NULL) {
        for (i = 0; i < system->count; ++i) {
            CUT_LOAD_CHARACTER_INSTANCE *state = &states->characters[i];
            if (state->model != NULL) {
                NuCutSceneCharacterProcess(instance, scene, state, &system->characters[i], frame, paused);
            }
        }
    }
}

static void CutSceneStreamHoldFrame(INSTNUGCUTSCENE *instance) {
    instance->current_frame = instance->scene->duration - 1.0f;
    CutSceneMissingObjectDiagnostic(instance->scene->filename);
}

static void instNuGCutSceneUpdate(INSTNUGCUTSCENE *instance, i32 paused, i32 skip, f32 elapsed) {
    CUT_SCENE_LOAD *scene = instance->scene;
    f32 overflow;
    i32 repeat_count;
    instance->flags_8c &= ~NUGCUTSCENE_PLAY_CAMERA_CHANGED;
    instance->synchro = cutscene_synchro_counter;
    if ((instance->flags_88 & NUGCUTSCENE_INST_SUSPENDED) != 0 || (scene->flags & CUTSCENE_STREAM_PENDING) != 0) {
        return;
    }
    instance->elapsed = elapsed;
    if (NuCutSceneSFXFixUp != NULL && (scene->flags & CUTSCENE_HAS_SFX) != 0 && (instance->flags_8c & NUGCUTSCENE_PLAY_SFX_FIXUP) != 0) {
        NUMTX *camera = NuCameraGetMtx();
        if (camera != NULL && camera->m30 < instance->bounds_max.x && camera->m30 > instance->bounds_min.x &&
            camera->m31 < instance->bounds_max.y && camera->m31 > instance->bounds_min.y && camera->m32 < instance->bounds_max.z &&
            camera->m32 > instance->bounds_min.z) {
            NuCutSceneSFXFixUp(instance);
        }
    }
    instance->flags_88 &= ~NUGCUTSCENE_INST_SKIPPING;
    if (instance->skip_countdown != 0 && (instance->flags_88 & NUGCUTSCENE_INST_ADVANCING) == 0 && instance->skip_frames != 0) {
        instance->skip_frames--;
        instance->flags_88 |= NUGCUTSCENE_INST_SKIPPING;
        paused = 1;
    }
    if ((instance->flags_88 & NUGCUTSCENE_INST_PLAYING) == 0) {
        if ((instance->flags_88 & NUGCUTSCENE_INST_ADVANCING) != 0 || (instance->flags_88 & NUGCUTSCENE_INST_AUTO_START) == 0) {
            return;
        }
        instNuGCutSceneStart(instance);
    }
    if ((scene->flags & CUTSCENE_STREAM_PENDING) != 0 && (instance->flags_88 & NUGCUTSCENE_INST_NEXT_STREAM_READY) == 0) {
        return;
    }
    if (instance->scene->version >= 2) {
        if ((instance->flags_88 & NUGCUTSCENE_INST_STREAMED) != 0 && CutSceneLoadingInstance == NULL &&
            (instance->flags_88 & NUGCUTSCENE_INST_NEXT_STREAM_READY) == 0 &&
            instance->stream_index != instance->saved_scene->last_stream) {
            CutSceneLoadingInstance = instance;
        } else if (instance->queued_stream_instance != NULL && CutSceneLoadingInstance == NULL &&
                   instance->stream_index == instance->saved_scene->last_stream) {
            CutSceneLoadingInstance = instance->queued_stream_instance;
            instance->queued_stream_instance = NULL;
        }
    }
    instNuGCutSceneClipTest(instance);
    if ((instance->flags_88 & NUGCUTSCENE_INST_VISIBLE) == 0) {
        return;
    }
    if ((instance->flags_88 & NUGCUTSCENE_INST_PAUSED) != 0) {
        paused = 1;
    }
    if (CutSceneTerminateStream != 0 || (instance->flags_8c & NUGCUTSCENE_PLAY_SKIPPABLE) != 0) {
        if (scene->version >= 2 && (scene->flags & CUTSCENE_STREAMED) != 0) {
            if (skip != 0) {
                instance->flags_8c |= NUGCUTSCENE_PLAY_STREAM_SKIP;
            } else {
                CutSceneQuitPrompt = 1;
            }
        } else if ((instance->flags_8c & NUGCUTSCENE_PLAY_SKIPPABLE) != 0) {
            if (skip != 0) {
                instance->current_frame = scene->duration - 1.0f - instance->rate * elapsed;
                CutSceneHasQuit = 1;
            } else {
                CutSceneQuitPrompt = 1;
            }
        }
    }
    if ((instance->flags_8c & NUGCUTSCENE_PLAY_LOCK_PLAYER) != 0) {
        CutSceneLockPlayer = 1;
    }
    if ((instance->flags_88 & NUGCUTSCENE_INST_ADVANCING) != 0) {
        if (paused == 0) {
            instance->current_frame += instance->rate * elapsed;
            if ((instance->flags_8c & NUGCUTSCENE_PLAY_STREAM_SKIP) != 0) {
                CutSceneStreamSkip(instance, elapsed);
            }
            if (instance->current_frame < 1.0f) {
                if ((instance->flags_88 & NUGCUTSCENE_INST_PING_PONG) != 0) {
                    if ((instance->flags_88 & NUGCUTSCENE_INST_LOOP) != 0) {
                        if (instance->rate < 0.0f) {
                            instance->rate = -instance->rate;
                        }
                        instance->current_frame = 2.0f - instance->current_frame;
                    } else {
                        repeat_count = instance->repeat_count;
                        if (repeat_count > 0) {
                            overflow = 1.0f - instance->current_frame;
                            instance->repeat_count = repeat_count - 1;
                            instNuGCutSceneStart(instance);
                            instance->flags_88 |= NUGCUTSCENE_INST_ADVANCING;
                            instance->current_frame += overflow;
                        } else if (instance->chained_instance != NULL) {
                            INSTNUGCUTSCENE *next;
                            overflow = 1.0f - instance->current_frame;
                            instNuGCutSceneEndFirstFrame(instance);
                            instNuGCutSceneStart(instance->chained_instance);
                            next = instance->chained_instance;
                            next->current_frame += overflow;
                            instNuGCutSceneClipTest(next);
                            instNuGCutSceneUpdate(instance->chained_instance, 0, skip, elapsed);
                            instance->chained_instance = NULL;
                            return;
                        } else {
                            instNuGCutSceneEndFirstFrame(instance);
                            return;
                        }
                    }
                } else if ((instance->flags_88 & NUGCUTSCENE_INST_HOLD_FIRST_FRAME) != 0) {
                    instance->current_frame = 1.0f;
                } else {
                    instNuGCutSceneEndFirstFrame(instance);
                }
            } else if (!(instance->current_frame < ((f64)scene->duration - 1.0))) {
                CUT_SCENE_LOAD *current = instance->scene;
                if (current->version >= 2 && (current->flags & CUTSCENE_STREAMED) != 0) {
                    if (instance->stream_index >= instance->saved_scene->last_stream) {
                        if ((instance->flags_8c & NUGCUTSCENE_PLAY_HOLD_LAST_FRAME) != 0) {
                            instance->flags_88 |= NUGCUTSCENE_INST_ENDED;
                            instance->current_frame = ((f64)scene->duration - 1.0);
                        } else {
                            instNuGCutSceneEnd(instance);
                            instance->accumulated_duration = 0.0f;
                        }
                    } else if ((instance->flags_88 & NUGCUTSCENE_INST_NEXT_STREAM_READY) == 0) {
                        CutSceneStreamHoldFrame(instance);
                    } else {
                        overflow = instance->current_frame - ((f64)current->duration - 1.0);
                        if (overflow > current->duration) {
                            overflow = 0.0f;
                        }
                        ++instance->stream_index;
                        instance->accumulated_duration += current->duration;
                        if ((instance->flags_8c & NUGCUTSCENE_PLAY_STREAM_LOADING) != 0 || instance->pending_stream != NULL) {
                            CutSceneStreamDiagnostic(instance->scene->filename);
                            CutSceneStreamHoldFrame(instance);
                            --instance->stream_index;
                        } else {
                            if ((instance->flags_88 & NUGCUTSCENE_INST_STREAM_BUFFER_1) != 0) {
                                instance->flags_88 &= ~NUGCUTSCENE_INST_STREAM_BUFFER_1;
                                instance->pending_stream = instance->stream_buffer_1;
                            } else {
                                instance->flags_88 |= NUGCUTSCENE_INST_STREAM_BUFFER_1;
                                instance->pending_stream = instance->stream_buffer_0;
                            }
                            CutSceneStreamDiagnostic(instance->scene->filename);
                            if (CutSceneSingleStreamBuffer < 2) {
                                copyAnims(instance->scene, (instance->flags_88 & NUGCUTSCENE_INST_STREAM_BUFFER_1) != 0
                                                               ? instance->stream_buffer_1
                                                               : instance->stream_buffer_0);
                            } else {
                                CutSceneSwapStreamAsset(instance);
                            }
                            instance->flags_88 &= ~NUGCUTSCENE_INST_NEXT_STREAM_READY;
                            instNuGCutSceneStart(instance);
                            instance->flags_88 |= NUGCUTSCENE_INST_ADVANCING;
                            instance->current_frame += overflow;
                        }
                    }
                } else if ((instance->flags_88 & NUGCUTSCENE_INST_PING_PONG) != 0) {
                    overflow = instance->current_frame - ((f64)scene->duration - 1.0);
                    instance->current_frame = ((f64)scene->duration - 1.0) - overflow;
                    if (instance->rate > 0.0f) {
                        instance->rate = -instance->rate;
                    }
                } else if ((instance->flags_8c & NUGCUTSCENE_PLAY_HOLD_LAST_FRAME) != 0) {
                    instance->flags_88 |= NUGCUTSCENE_INST_ENDED;
                    instance->current_frame = ((f64)scene->duration - 1.0);
                } else {
                    if (instance->end_callback != NULL) {
                        void (*callback)(INSTNUGCUTSCENE *) = instance->end_callback;
                        instance->end_callback = NULL;
                        callback(instance);
                    }
                    repeat_count = instance->repeat_count;
                    if (repeat_count > 0) {
                        instance->repeat_count = repeat_count - 1;
                        overflow = instance->current_frame - ((f64)scene->duration - 1.0);
                        if (overflow > scene->duration) {
                            overflow = 0.0f;
                        }
                        instNuGCutSceneStart(instance);
                        instance->flags_88 |= NUGCUTSCENE_INST_ADVANCING;
                        instance->current_frame += overflow;
                    } else if (instance->chained_instance != NULL) {
                        INSTNUGCUTSCENE *next;
                        overflow = instance->current_frame - ((f64)scene->duration - 1.0);
                        if (overflow > scene->duration) {
                            overflow = 0.0f;
                        }
                        instNuGCutSceneEnd(instance);
                        instNuGCutSceneStart(instance->chained_instance);
                        next = instance->chained_instance;
                        next->current_frame += overflow;
                        instNuGCutSceneClipTest(next);
                        instNuGCutSceneUpdate(instance->chained_instance, 0, skip, elapsed);
                        instance->chained_instance = NULL;
                        return;
                    } else if ((instance->flags_88 & NUGCUTSCENE_INST_LOOP) != 0) {
                        overflow = instance->current_frame - ((f64)scene->duration - 1.0);
                        if (overflow > scene->duration) {
                            overflow = 0.0f;
                        }
                        instNuGCutSceneStart(instance);
                        instance->flags_88 |= NUGCUTSCENE_INST_ADVANCING;
                        instance->current_frame += overflow;
                    } else {
                        instNuGCutSceneEnd(instance);
                    }
                }
            }
        }
    } else if (paused == 0) {
        instance->flags_88 |= NUGCUTSCENE_INST_ADVANCING;
    }
    if ((instance->flags_88 & NUGCUTSCENE_INST_PLAYING) == 0) {
        return;
    }
    instance->render_frame = instance->current_frame;
    if ((instance->flags_88 & 0x40000) != 0) {
        if (CutSceneRenderCharacters != 0 && instance->character_instance != NULL) {
            CutSceneCharacterSysUpdate(instance, scene->duration - instance->current_frame, paused);
        }
        if (CutSceneUpdateCameras != 0 && instance->camera_instance != NULL) {
            CutSceneCameraSysUpdate(instance, scene->duration - instance->current_frame);
        }
        if (instance->trigger_instance != NULL) {
            CutSceneTriggerSysUpdate(instance, scene->duration - instance->render_frame);
        }
        if (CutSceneRenderRigids != 0 && instance->rigid_instance != NULL) {
            CutSceneRigidSysUpdate(instance, scene->duration - instance->render_frame, paused);
        }
    } else {
        if (CutSceneRenderCharacters != 0 && instance->character_instance != NULL) {
            CutSceneCharacterSysUpdate(instance, instance->current_frame, paused);
        }
        if (CutSceneUpdateCameras != 0 && instance->camera_instance != NULL) {
            CutSceneCameraSysUpdate(instance, instance->current_frame);
        }
        if (instance->trigger_instance != NULL) {
            CutSceneTriggerSysUpdate(instance, instance->render_frame);
        }
        if (CutSceneRenderRigids != 0 && instance->rigid_instance != NULL) {
            CutSceneRigidSysUpdate(instance, instance->render_frame, paused);
        }
    }
}

void NuGCutSceneSysUpdate(i32 paused, i32 skip, f32 elapsed) {
    INSTNUGCUTSCENE *owner = active_cutscene_instances;
    if (++cutscene_synchro_counter > 0xff) {
        cutscene_synchro_counter = 1;
    }
    CutSceneQuitPrompt = 0;
    CutSceneHasQuit = 0;
    CutSceneLockPlayer = 0;
    CutSceneCameraChanged = 0;
    if (paused != 0) {
        skip = 0;
    }
    for (; owner != NULL; owner = owner->next) {
        INSTNUGCUTSCENE *instance = owner;
        if (((i32)(instance->flags_88 << 5) >> 24) != cutscene_synchro_counter) {
            instNuGCutSceneUpdate(instance, paused, skip, elapsed);
        }
    }
}

static void CutSceneReleaseInstanceCharacters(INSTNUGCUTSCENE *instance) {
    CUT_LOAD_CHARACTER_SYSTEM *system = instance->scene->character_system;
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *states = instance->character_instance;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_CHARACTER_INSTANCE *state = &states->characters[i];
        if (state->model != NULL && NuCutSceneCharacterRelease != NULL) {
            NuCutSceneCharacterRelease(state, &system->characters[i]);
        }
    }
}

void instNuGCutSceneStop(INSTNUGCUTSCENE *owner) {
    INSTNUGCUTSCENE *instance = owner;
    instance->flags_88 &= ~0x1006U;
    if (instance->character_instance != NULL) {
        CutSceneReleaseInstanceCharacters(instance);
    }
    if (instance->camera_instance != NULL && instance->camera_instance->camera_index >= 0) {
        CutSceneCameraCTRL = 0;
    }
}

void fun_00500200(void) {
    active_cutscene_instances = NULL;
}

i32 instNuGCutSceneIsFinished(INSTNUGCUTSCENE *owner) {
    /* Sign-extends bit 12 of the flags word. */
    return (i32)(owner->flags_88 << 19) >> 31;
}

static f32 CutSceneFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

f32 instNuGCutSceneTimeLeft(INSTNUGCUTSCENE *owner) {
    INSTNUGCUTSCENE *instance = owner;
    CUT_SCENE_LOAD *scene;
    f32 frames_left;
    f32 time_left = 0.0f;

    if (instance != NULL && instance->scene != NULL) {
        scene = instance->scene;
        if (scene->version >= 10) {
            frames_left = scene->total_stream_frames - instance->current_frame;
        } else {
            if (scene->version >= 2 && (scene->flags & 1) != 0 && scene->last_stream != 0) {
                return FLT_MAX;
            }
            frames_left = scene->duration - 1.0 - instance->current_frame;
        }
        if (frames_left > time_left) {
            if (instance->rate == 0.0f) {
                return FLT_MAX;
            }
            return CutSceneFdiv(frames_left, instance->rate);
        }
    }
    return time_left;
}

f32 instCutSceneTimeElapsed(INSTNUGCUTSCENE *owner) {
    INSTNUGCUTSCENE *instance = owner;

    if (instance == NULL || instance->scene == NULL) {
        return 0.0f;
    }
    if (instance->rate == 0.0f) {
        return 0.0f;
    }
    return CutSceneFdiv(instance->accumulated_duration + instance->current_frame - 1.0, instance->rate);
}

f32 instNuGCutSceneTotalTime(INSTNUGCUTSCENE *owner) {
    INSTNUGCUTSCENE *instance = owner;

    if (instance == NULL || instance->scene == NULL || instance->scene->version < 10 || instance->rate == 0.0f) {
        return 0.0f;
    }
    return CutSceneFdiv((f32)instance->saved_scene->total_stream_frames, instance->rate);
}

i32 gcutSoundVol = 100;
i32 gcutSoundMusVol = 30;
i32 dat_008008b0 = -1;

void gcutSetSoundVol(i32 sound_volume, i32 music_volume) {
    gcutSoundVol = sound_volume;
    gcutSoundMusVol = music_volume;
}

void instNuGCutScenePause(INSTNUGCUTSCENE *owner, i32 paused) {
    INSTNUGCUTSCENE *instance = owner;
    instance->paused = paused;
}

void NuGCutSceneSysPostBackgroundLoad(void) {
    INSTNUGCUTSCENE *next = CutSceneBackgroundInstances;
    INSTNUGCUTSCENE *active;
    if (next != NULL) {
        active = active_cutscene_instances;
        while (next != NULL) {
            INSTNUGCUTSCENE *owner = next;
            next = owner->next;
            owner->next = active;
            if (active != NULL) {
                active->previous = owner;
            }
            owner->previous = NULL;
            active = owner;
        }
        active_cutscene_instances = active;
        CutSceneBackgroundInstances = NULL;
    }
}

i32 NuGCutSceneIsBackgroundLoading(void) {
    return 1;
}

i32 instNuGCutScenePreload(INSTNUGCUTSCENE *owner) {
    if (CutSceneLoadingInstance != NULL) {
        return 0;
    }
    CutSceneLoadingInstance = owner;
    return 1;
}

void fun_00500a60(i32 value) {
    dat_008008b0 = value;
}

i32 instNuGCutSceneSwapBuffers(INSTNUGCUTSCENE *instance, i32 force) {
    if (((instance->flags_8c & NUGCUTSCENE_PLAY_STREAM_LOADING) != 0 && force == 0) || instance->pending_stream != NULL) {
        CutSceneStreamDiagnostic(instance->scene->filename);
        return 0;
    }
    if ((instance->flags_88 & NUGCUTSCENE_INST_STREAM_BUFFER_1) != 0) {
        instance->flags_88 &= ~NUGCUTSCENE_INST_STREAM_BUFFER_1;
        instance->pending_stream = instance->stream_buffer_1;
    } else {
        instance->flags_88 |= NUGCUTSCENE_INST_STREAM_BUFFER_1;
        instance->pending_stream = instance->stream_buffer_0;
    }
    CutSceneStreamDiagnostic(instance->scene->filename);
    return 1;
}

void instNuGCutSceneServiceLoad(void) {
    char path[256];
    char extension[8];
    char *dot;
    i32 tens;
    i32 units;
    VARIPTR buffer;
    VARIPTR buffer_end;
    INSTNUGCUTSCENE *instance;
    if (CutSceneLoadingInstance == NULL || CutSceneStreamLoadGuard != 0 || CutSceneLoadingInstance->scene == NULL) {
        return;
    }
    NuStrCpy(path, CutSceneLoadingInstance->scene->filename);
    dot = NuStrStr(path, ".");
    NuStrCpy(extension, dot);
    tens = (CutSceneLoadingInstance->stream_index + 1) / 10;
    units = (CutSceneLoadingInstance->stream_index + 1) % 10;
    if (tens != 0) {
        *dot++ = (char)('0' + tens);
    }
    *dot++ = (char)('0' + units);
    NuStrCpy(dot, extension);
    instance = CutSceneLoadingInstance;
    buffer.void_ptr = instance->pending_stream;
    instance->pending_stream = NULL;
    if (buffer.void_ptr == NULL) {
        return;
    }
    CutSceneLoadingInstance->flags_8c = CutSceneLoadingInstance->flags_8c | NUGCUTSCENE_PLAY_STREAM_LOADING;
    buffer_end.addr = buffer.addr + CutSceneLoadingInstance->scene->stream_buffer_size;
    CutSceneMissingObjectDiagnostic(path, buffer.void_ptr);
    CutSceneStreamDiagnostic(path);
    NuGCutSceneLoad(path, &buffer, &buffer_end, 1);
    CutSceneStreamDiagnostic(path);
    CutSceneLoadingInstance->flags_88 = CutSceneLoadingInstance->flags_88 | NUGCUTSCENE_INST_NEXT_STREAM_READY;
    if (CutSceneLoadingInstance->stream_index == 0 && (CutSceneLoadingInstance->scene->flags & CUTSCENE_STREAM_PENDING) != 0) {
        instNuGCutSceneSwapBuffers(CutSceneLoadingInstance, 1);
        if (CutSceneSingleStreamBuffer < 2) {
            copyAnims(CutSceneLoadingInstance->scene, CutSceneLoadingInstance->scene->stream_buffer_1);
        } else {
            CutSceneLoadingInstance->flags_88 = CutSceneLoadingInstance->flags_88 | NUGCUTSCENE_INST_STREAM_BUFFER_1;
            CutSceneSwapStreamAsset(CutSceneLoadingInstance);
        }
        CutSceneLoadingInstance->scene->flags &= ~CUTSCENE_STREAM_PENDING;
        CutSceneLoadingInstance->flags_88 = CutSceneLoadingInstance->flags_88 & ~NUGCUTSCENE_INST_NEXT_STREAM_READY;
        CutSceneLoadingInstance->stream_index = 1;
    }
    CutSceneLoadingInstance->flags_8c = CutSceneLoadingInstance->flags_8c & ~NUGCUTSCENE_PLAY_STREAM_LOADING;
    CutSceneLoadingInstance = NULL;
}

INSTNUGCUTSCENE *instNuGCutSceneFindActive(char *name) {
    INSTNUGCUTSCENE *instance;
    for (instance = active_cutscene_instances; instance != NULL; instance = instance->next) {
        if (NuStrICmp(name, instance->name) == 0) {
            return instance;
        }
    }
    CutSceneMissingObjectDiagnostic(name);
    return NULL;
}

i32 instNuGCutSceneAddCamTgt(INSTNUGCUTSCENE *owner, NUVEC *target, f32 start_frame, f32 duration, i8 target_index) {
    CUT_LOAD_CAMERA_INSTANCE *camera = owner->camera_instance;
    CUT_LOAD_CAMERA_TARGET *entry;
    if (camera == NULL || camera->target_count >= camera->target_capacity) {
        return 0;
    }
    entry = &camera->targets[camera->target_count++];
    entry->target = target;
    entry->start_frame = start_frame;
    entry->duration = duration;
    entry->target_index = target_index;
    return 1;
}

i32 instNuGCutSceneCharGetStartMtx(INSTNUGCUTSCENE *owner, char *name, NUMTX *out) {
    CUT_LOAD_CHARACTER_SYSTEM *system = owner->scene->character_system;
    i32 i;
    for (i = 0; i < system->count; ++i) {
        CUT_LOAD_CHARACTER *character = &system->characters[i];
        if (NuStrICmp(name, character->name) == 0) {
            *out = character->matrix;
            return 1;
        }
    }
    return 0;
}

void instNuGCutScenePlay(INSTNUGCUTSCENE *instance, i32 forward) {
    if ((instance->flags_88 & NUGCUTSCENE_INST_PLAYING) != 0) {
        if (forward != 0) {
            if (instance->rate < 0.0f) {
                instance->rate = -instance->rate;
            }
        } else if (instance->rate > 0.0f) {
            instance->rate = -instance->rate;
        }
    } else if ((instance->flags_88 & NUGCUTSCENE_INST_ENDED) != 0) {
        if (forward == 0) {
            instance->flags_88 = (instance->flags_88 & ~NUGCUTSCENE_INST_ENDED) | NUGCUTSCENE_INST_PLAYING;
            instance->current_frame = (f32)(instance->scene->duration - 1.0);
            if (instance->rate > 0.0f) {
                instance->rate = -instance->rate;
            }
        }
    } else if (forward != 0) {
        if (instance->rate < 0.0f) {
            instance->rate = -instance->rate;
        }
        instNuGCutSceneStart(instance);
    }
}

CUT_SCENE_LOAD *RelocateCutScene(CUT_SCENE_LOAD *source, VARIPTR *buffer) {
    CUT_SCENE_LOAD *scene;
    i32 delta;
    buffer->addr = (buffer->addr + 3) & ~3;
    scene = buffer->void_ptr;
    delta = (i32)scene - (i32)source;
    if (delta != 0) {
        source->string_delta = delta;
        source->relocation_delta = delta;
        PCByteCopy(scene, source, source->loaded_size);
        NuGCutSceneFixPtrs(scene, delta, 1);
    }
    buffer->addr += scene->loaded_size;
    return scene;
}

/* Destroys the listed instances, compacts the surviving cutscene images to the front of the cutscene
   pool and recreates their instances; returns the first free byte. */
void *instNuGCutSceneCleanUp(void) {
    CUT_CLEANUP_ENTRY *end = DefragCutSceneListBase + DefragCutSceneListSize;
    CUT_CLEANUP_ENTRY *first = NULL;
    CUT_CLEANUP_ENTRY *entry;
    CUT_CLEANUP_ENTRY *other;
    VARIPTR cursor;

    for (entry = DefragCutSceneListBase; entry < end; ++entry) {
        CUT_SCENE_LOAD *scene;
        if ((entry->flags & 1) != 0) {
            continue;
        }
        for (other = entry; other < end; ++other) {
            if ((other->flags & 1) != 0 && entry->scene == other->scene) {
                break;
            }
        }
        if (other < end) {
            for (other = entry; other < end; ++other) {
                if ((other->flags & 1) != 0 && entry->scene == other->scene) {
                    other->flags |= 1;
                }
            }
            continue;
        }
        scene = entry->scene;
        if (*(u32 *)scene != 0x44454144) {
            NuGCutSceneDestroy(scene);
            *(u32 *)entry->scene = 0x44454144;
        }
    }

    for (entry = DefragCutSceneListBase; entry < end; ++entry) {
        instNuGCutSceneDestroy(DefragGetInstFn(entry->handle));
    }

    cursor.void_ptr = DefragCutSceneBaseMem;
    for (;;) {
        u32 lowest = 0xffffffff;
        CUT_SCENE_LOAD *source;
        CUT_SCENE_LOAD *relocated;
        for (entry = DefragCutSceneListBase; entry < end; ++entry) {
            if ((entry->flags & 1) != 0 && (u32)entry->scene < lowest) {
                lowest = (u32)entry->scene;
                first = entry;
            }
        }
        if (lowest == 0xffffffff) {
            break;
        }
        source = (CUT_SCENE_LOAD *)lowest;
        relocated = RelocateCutScene(source, &cursor);
        for (entry = first; entry < end; ++entry) {
            if ((entry->flags & 1) != 0 && entry->scene == source) {
                entry->scene = relocated;
                entry->flags ^= 1;
            }
        }
    }
    if (DefragInstBaseMem != NULL) {
        cursor.void_ptr = DefragInstBaseMem;
    }
    for (entry = DefragCutSceneListBase; entry < end; ++entry) {
        if (entry->flags != 0) {
            DefragCreateInstFn(entry->handle, entry->scene, &cursor)->accumulated_duration = entry->accumulated_duration;
        } else {
            DefragInstDestroyedFn(entry->handle);
        }
    }
    if (CutSceneBackgroundInstances != NULL) {
        active_cutscene_instances = CutSceneBackgroundInstances;
        CutSceneBackgroundInstances = NULL;
    }
    return cursor.void_ptr;
}
