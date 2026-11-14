#include "saga.h"

typedef struct cut_load_locator_s {
    NUMTX matrix;     // 0x00
    NUVEC pivot;      // 0x40
    f32 scale;        // 0x4c
    void *animation;  // 0x50
    u32 unknown_54;   // 0x54
    u8 flags;         // 0x58
    u8 type_index;    // 0x59
    u8 unknown_5a;    // 0x5a
    u8 tracks_self;   // 0x5b
    u8 unknown_5c[8]; // 0x5c
} CUT_LOAD_LOCATOR;   // size 0x64

typedef struct cut_load_locator_type_s {
    char *name;          // 0x0
    u8 flags;            // 0x4
    u8 unknown_05;       // 0x5
    u16 function_index;  // 0x6
    u32 unknown_08;      // 0x8
} CUT_LOAD_LOCATOR_TYPE; // size 0xc

typedef struct cut_load_locator_system_s {
    CUT_LOAD_LOCATOR *locators;   // 0x0
    CUT_LOAD_LOCATOR_TYPE *types; // 0x4
    u8 count;                     // 0x8
    u8 type_count;                // 0x9
    u8 unknown_0a[2];             // 0xa
} CUT_LOAD_LOCATOR_SYSTEM;        // size 0xc

typedef struct cut_load_rigid_s {
    NUMTX matrix;                   // 0x00
    char *name;                     // 0x40
    NUGSCN *scene;                  // 0x44
    void *special;                  // 0x48
    void *animation;                // 0x4c
    STATE_ANIM_PC *state_animation; // 0x50

    union {                        // 0x54
        i32 index;                 // 0x54
        CUT_LOAD_LOCATOR *pointer; // 0x54
    } locator;

    u8 flags;         // 0x58
    u8 uses_locator;  // 0x59
    u8 locator_index; // 0x5a
    u8 unknown_5b;    // 0x5b
} CUT_LOAD_RIGID;     // size 0x5c

typedef struct cut_load_rigid_system_s {
    CUT_LOAD_RIGID *rigids;
    u16 count;
    u16 unused;
} CUT_LOAD_RIGID_SYSTEM;

struct cut_load_character_s {
    NUMTX matrix;          // 0x00
    char *name;            // 0x40
    void *animation;       // 0x44
    void *face_animation;  // 0x48
    void *extra_animation; // 0x4c
    void *model;           // 0x50
    u8 unknown_54[8];      // 0x54
    u8 flags;              // 0x5c
    u8 unknown_5d[7];      // 0x5d
}; // size 0x64

typedef struct cut_load_character_system_s {
    CUT_LOAD_CHARACTER *characters;
    u16 count;
} CUT_LOAD_CHARACTER_SYSTEM;

typedef struct cut_load_character_animation_s {
    u32 unknown_00;             // 0x0
    void *animation;            // 0x4
    u8 unknown_08[8];           // 0x8
} CUT_LOAD_CHARACTER_ANIMATION; // size 0x10

typedef struct cut_load_camera_system_s {
    i32 count;                            // 0x00
    void *cameras;                        // 0x04
    void *animation;                      // 0x08
    STATE_ANIM_PC *state_animation;       // 0x0c
    u8 unknown_10[12];                    // 0x10
    void *focus_animation;                // 0x1c
    STATE_ANIM_PC *focus_state_animation; // 0x20
} CUT_LOAD_CAMERA_SYSTEM;                 // size 0x24

typedef struct cut_load_trigger_event_s {
    u16 index;                      // 0x0
    u16 unknown_02;                 // 0x2
    char *name;                     // 0x4
    STATE_ANIM_PC *state_animation; // 0x8
} CUT_LOAD_TRIGGER_EVENT;           // size 0xc

typedef struct cut_load_trigger_system_s {
    i32 count;
    CUT_LOAD_TRIGGER_EVENT *events;
} CUT_LOAD_TRIGGER_SYSTEM;

typedef struct cut_load_bounds_s {
    NUVEC min;
    NUVEC max;
} CUT_LOAD_BOUNDS;

struct cut_scene_load_pc_s {
    i32 version;                                        // 0x00
    i32 string_delta;                                   // 0x04
    f32 duration;                                       // 0x08
    char *strings;                                      // 0x0c
    CUT_LOAD_CAMERA_SYSTEM *camera_system;              // 0x10
    CUT_LOAD_RIGID_SYSTEM *rigid_system;                // 0x14
    CUT_LOAD_CHARACTER_SYSTEM *character_system;        // 0x18
    CUT_LOAD_LOCATOR_SYSTEM *locator_system;            // 0x1c
    CUT_LOAD_BOUNDS *bounds;                            // 0x20
    CUT_LOAD_TRIGGER_SYSTEM *trigger_system;            // 0x24
    i32 relocation_delta;                               // 0x28
    char *filename;                                     // 0x2c
    void *stream_buffer_0;                              // 0x30
    void *stream_buffer_1;                              // 0x34
    u32 flags;                                          // 0x38
    i32 stream_buffer_size;                             // 0x3c
    u32 unknown_40;                                     // 0x40
    NUGSCN *scene;                                      // 0x44
    void *extra_scene;                                  // 0x48
    u32 stream_buffer_offset;                           // 0x4c
    CUT_LOAD_CHARACTER_ANIMATION *character_animations; // 0x50
    u16 *focus_indices;                                 // 0x54
    i32 loaded_size;                                    // 0x58
}; // size 0x5c

typedef struct cut_load_camera_instance_s {
    u32 unknown_00;
    u32 *states;
    u8 unknown_08[8];
} CUT_LOAD_CAMERA_INSTANCE;

typedef struct cut_load_rigid_instance_s {
    NUSPECIALHANDLE special; // 0x0
    u8 state_index;          // 0xc
    u8 visible;              // 0xd
    u8 unknown_0e[2];        // 0xe
} CUT_LOAD_RIGID_INSTANCE;   // size 0x10

typedef struct cut_load_rigid_instance_system_s {
    CUT_LOAD_RIGID_INSTANCE *rigids;
} CUT_LOAD_RIGID_INSTANCE_SYSTEM;

struct cut_load_character_instance_s {
    void *model;      // 0x00
    u8 unknown_04[8]; // 0x04
    f32 frame_a;      // 0x0c
    u8 unknown_10[5]; // 0x10
    u8 animation_a;   // 0x15
    u8 animation_b;   // 0x16
    u8 unknown_17;    // 0x17
}; // size 0x18

typedef struct cut_load_character_instance_system_s {
    CUT_LOAD_CHARACTER_INSTANCE *characters;
} CUT_LOAD_CHARACTER_INSTANCE_SYSTEM;

typedef struct cut_load_focus_target_s {
    u32 unknown_00[5];      // 0x00
    CUT_LOAD_LOCATOR *self; // 0x14
} CUT_LOAD_FOCUS_TARGET;    // size 0x18

typedef struct cut_load_sound_state_s {
    u32 unknown_00[4];
} CUT_LOAD_SOUND_STATE;

typedef struct cut_load_locator_instance_s {
    f32 frame; // 0x0

    union {                           // 0x4
        i32 index;                    // 0x4
        CUT_LOAD_FOCUS_TARGET *focus; // 0x4
        CUT_LOAD_SOUND_STATE *sound;  // 0x4
    } effect;
} CUT_LOAD_LOCATOR_INSTANCE; // size 0x8

typedef struct cut_load_locator_instance_system_s {
    CUT_LOAD_LOCATOR_INSTANCE *locators;
} CUT_LOAD_LOCATOR_INSTANCE_SYSTEM;

typedef struct cut_load_trigger_entry_s {
    char *name;           // 0x00
    u8 unknown_04[0x30];  // 0x04
} CUT_LOAD_TRIGGER_ENTRY; // size 0x34

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
    INSTNUGCUTSCENE *next;                                  // 0x00
    INSTNUGCUTSCENE *previous;                              // 0x04
    char name[16];                                          // 0x08
    NUMTX matrix;                                           // 0x18
    CUT_SCENE_LOAD_PC *scene;                               // 0x58
    CUT_SCENE_LOAD_PC *saved_scene;                         // 0x5c
    NUVEC center;                                           // 0x60
    u8 unknown_6c[0x1c];                                    // 0x6c
    u32 flags_88;                                           // 0x88
    u32 flags_8c;                                           // 0x8c
    f32 current_frame;                                      // 0x90
    f32 render_frame;                                       // 0x94
    f32 rate;                                               // 0x98
    CUT_LOAD_CAMERA_INSTANCE *camera_instance;              // 0x9c
    CUT_LOAD_RIGID_INSTANCE_SYSTEM *rigid_instance;         // 0xa0
    CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *character_instance; // 0xa4
    CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *locator_instance;     // 0xa8
    CUT_LOAD_TRIGGER_INSTANCE *trigger_instance;            // 0xac
    u8 unknown_b0[12];                                      // 0xb0
    void *stream_buffer_0;                                  // 0xbc
    void *stream_buffer_1;                                  // 0xc0
    NUGSCN *scene_owner;                                    // 0xc4
    CUT_LOAD_TRIGGER_OWNER *extra_scene_owner;              // 0xc8
    u8 unknown_cc[19];                                      // 0xcc
    u8 stream_index;                                        // 0xdf
    f32 accumulated_duration;                               // 0xe0
    f32 elapsed;                                            // 0xe4
    f32 alpha;                                              // 0xe8
    void *pending_stream;                                   // 0xec
    u32 allocation_size;                                    // 0xf0
    u32 unknown_f4;                                         // 0xf4
} CUT_INSTANCE_LOAD_PC;                                     // size 0xf8

struct cut_locator_function_entry_pc_s {
    char *name;        // 0x00
    u8 unknown_04[16]; // 0x04
}; // size 0x14

typedef struct CUT_LOOKAT_TEXTURE_PC {
    char *name;          // 0x00
    u32 unknown_04;      // 0x04
    i16 *frames;         // 0x08
    i16 frame_count;     // 0x0c
    i16 unknown_0e;      // 0x0e
    NUMTL *material;     // 0x10
    u8 unknown_14[12];   // 0x14
} CUT_LOOKAT_TEXTURE_PC; // size 0x20

typedef struct CUT_LOOKAT_HIERARCHY_PC {
    u8 unknown_00[12];               // 0x00
    NUMTL **materials;               // 0x0c
    i32 material_count;              // 0x10
    u8 unknown_14[0x4c];             // 0x14
    i32 texture_count;               // 0x60
    CUT_LOOKAT_TEXTURE_PC *textures; // 0x64
} CUT_LOOKAT_HIERARCHY_PC;           // size 0x68

typedef struct CUT_LOOKAT_MATERIAL_PC {
    u8 unknown_00[0x9a];  // 0x00
    u8 type;              // 0x9a
} CUT_LOOKAT_MATERIAL_PC; // size 0x9b

typedef struct CUT_LOOKAT_LOCATOR_INFO_PC {
    u8 unknown_00[0x5b];      // 0x00
    u8 character_id;          // 0x5b
    u8 joint;                 // 0x5c
    u8 material;              // 0x5d
    u8 secondary_material;    // 0x5e
    u8 tertiary_material;     // 0x5f
} CUT_LOOKAT_LOCATOR_INFO_PC; // size 0x60

typedef struct CUT_LOOKAT_STATE_PC {
    NUHGOBJ_EVAL_PC *hierarchy;          // 0x00
    f32 blend;                           // 0x04
    u16 character;                       // 0x08
    u16 material;                        // 0x0a
    u16 texture;                         // 0x0c
    u16 secondary_material;              // 0x0e
    u16 tertiary_material;               // 0x10
    u16 joint;                           // 0x12
    CUT_LOOKAT_LOCATOR_INFO_PC *locator; // 0x14
} CUT_LOOKAT_STATE_PC;                   // size 0x18

extern NUHGOBJ_EVAL_PC *(*NuCutSceneGetHGObj)(INSTNUGCUTSCENE *, i32);

i32 FindMtlInHGObj(NUHGOBJ_EVAL_PC *hierarchy, i32 material_type) {
    CUT_LOOKAT_HIERARCHY_PC *view = (CUT_LOOKAT_HIERARCHY_PC *)hierarchy;
    i32 index;
    for (index = 0; index < view->material_count; ++index) {
        CUT_LOOKAT_MATERIAL_PC *material = (CUT_LOOKAT_MATERIAL_PC *)view->materials[index];
        if (material->type == material_type) {
            return index + 1;
        }
    }
    return 0;
}

i32 FindTexAnimFromMtl(NUHGOBJ_EVAL_PC *hierarchy, NUMTL *material) {
    CUT_LOOKAT_HIERARCHY_PC *view = (CUT_LOOKAT_HIERARCHY_PC *)hierarchy;
    i32 index;
    for (index = 0; index < view->texture_count; ++index) {
        if (view->textures[index].material == material) {
            return index + 1;
        }
    }
    return 0;
}

void instGetLookAtLocatorInfo(INSTNUGCUTSCENE *owner, CUT_LOOKAT_STATE_PC *state) {
    CUT_LOOKAT_LOCATOR_INFO_PC *locator = state->locator;
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    CUT_SCENE_LOAD_PC *scene;
    CUT_LOAD_CHARACTER_ANIMATION *animations;
    NUHGOBJ_EVAL_PC *hierarchy;
    i32 i;
    i32 character_id;
    i32 joint;
    if (locator == NULL || NuCutSceneGetHGObj == NULL) {
        return;
    }
    character_id = locator->character_id;
    state->locator = NULL;
    scene = instance->scene;
    animations = scene->character_animations;
    for (i = 0; i < scene->character_system->count; ++i) {
        if ((u8)animations[i].unknown_00 == character_id) {
            state->character = (u16)(i + 1);
            break;
        }
    }
    if (state->character == 0) {
        return;
    }
    hierarchy = NuCutSceneGetHGObj(owner, state->character - 1);
    state->hierarchy = hierarchy;
    if (hierarchy == NULL) {
        return;
    }
    joint = locator->joint;
    if (joint >= 0 && joint < hierarchy->joint_override_map_count) {
        state->joint = (u16)(hierarchy->joint_override_map[joint] + 1);
    }
    state->material = (u16)FindMtlInHGObj(hierarchy, locator->material);
    if (state->material != 0) {
        state->texture =
            (u16)FindTexAnimFromMtl(hierarchy, ((CUT_LOOKAT_HIERARCHY_PC *)hierarchy)->materials[state->material - 1]);
    }
    state->secondary_material = (u16)FindMtlInHGObj(hierarchy, locator->secondary_material);
    state->tertiary_material = (u16)FindMtlInHGObj(hierarchy, locator->tertiary_material);
}

void CutGetJointOverrideMatrixPc(NUHGOBJ_EVAL_PC *hierarchy, i32 index, NUMTX *matrix) {
    NUHGOBJ_EVAL_JOINT_PC *joint = &hierarchy->joints[index];
    *matrix = NuMtxSetIdentityTemplate;
    for (;;) {
        if (joint->bind_matrix.m00 != 1.0f || joint->bind_matrix.m11 != 1.0f || joint->bind_matrix.m22 != 1.0f) {
            NuMtxMulVU0(matrix, &joint->bind_matrix, matrix);
        }
        if (joint->parent_index == 0xff) {
            break;
        }
        joint = &hierarchy->joints[joint->parent_index];
    }
    matrix->m02 = -matrix->m02;
    matrix->m12 = -matrix->m12;
    matrix->m20 = -matrix->m20;
    matrix->m21 = -matrix->m21;
    matrix->m23 = -matrix->m23;
    matrix->m32 = -matrix->m32;
    NuMtxTransposeR(matrix, matrix);
}

typedef struct CUT_UPDATE_LOCATOR_MATRIX_PC {
    NUMTX matrix;               // 0x00
    NUVEC pivot;                // 0x40
    f32 scale;                  // 0x4c
    NUANIMDATA2_PC *animation;  // 0x50
    u32 unknown_54;             // 0x54
    u8 flags;                   // 0x58
    u8 type;                    // 0x59
    u8 joint;                   // 0x5a
    u8 self;                    // 0x5b
    u8 unknown_5c[8];           // 0x5c
} CUT_UPDATE_LOCATOR_MATRIX_PC; // size 0x64

typedef struct CUT_UPDATE_ENTRY_PC {
    char *name;     // 0x00
    u32 unknown_04; // 0x04
    u32 unknown_08; // 0x08
    u32 unknown_0c; // 0x0c
    void (*update)(INSTNUGCUTSCENE *, CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *, CUT_LOAD_LOCATOR *, f32,
                   NUMTX *,
                   i32); // 0x10
} CUT_UPDATE_ENTRY_PC;   // size 0x14

extern i32 (*TriggerLocatorVfxFn)(i32, NUMTX *);
extern void (*UpdateLocatorVfxFn)(i32, NUMTX *);
extern void (*NuCutSceneSFXUpdate)(CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *, CUT_LOAD_LOCATOR *, f32,
                                   NUMTX *, i32);

static void CutSceneLocatorDebrisUpdatePc(CUT_LOAD_LOCATOR *locator, CUT_LOAD_LOCATOR_SYSTEM *system,
                                          CUT_LOAD_LOCATOR_INSTANCE *instance, f32 frame, NUMTX *parent) {
    u16 effect = system->types[locator->type_index].function_index;
    NUANIM_TIME2_PC time;
    f32 scale;
    f32 rate;
    NUMTX matrix;
    NUVEC *position;
    i32 finite;
    if (effect == 0xffff || debtab[effect] == NULL) {
        return;
    }
    if (locator->animation != NULL) {
        NuAnimData2CalcTime(locator->animation, frame, &time);
    }
    if (NuGCutLocatorIsVisble((CUT_LOCATOR_GENERIC_PC *)locator, frame, &time, &scale, &rate) != 0) {
        if (scale < 0.0f && (locator->flags & 0x10) == 0) {
            // MATCH-EXCEPTION: the original negates with fldz/fsubrp (0 - scale), unary minus gives fchs.
            scale = (f32)debtab[effect]->frequency * (0.0f - scale);
        }
        if ((f64)scale > 0.10000000149011612) {
            if ((locator->flags & 0x10) != 0 && instance->effect.index != 0) {
                return;
            }
            NuGCutLocatorCalcMtx((CUT_LOCATOR_GENERIC_PC *)locator, frame, &matrix, &time);
            if ((locator->flags & 4) != 0) {
                NuMtxPreTranslate(&matrix, &locator->pivot);
            }
            if (parent != NULL) {
                NuMtxMul(&matrix, &matrix, parent);
            }
            position = (NUVEC *)&matrix.m30;
            if ((locator->flags & 0x10) != 0) {
                finite = -1;
                AddScaledFiniteShotDebrisEffect(&finite, effect, position, 0, 0, 1, rate);
                DebrisEmitterOrientationMtx(finite, &matrix);
                instance->effect.index = 1;
            } else if ((locator->flags & 0x20) != 0) {
                if (instance->effect.index == -1) {
                    AddDebrisEffect(&instance->effect.index, effect, position->x, position->y, position->z);
                    DebrisPopulateInstance(instance->effect.index, 0.0f);
                }
                if (instance->effect.index > -1) {
                    if (CutSceneDebrisProcessModePc == 1) {
                        DebrisEmitterPos(instance->effect.index, position->x, position->y, position->z);
                    } else if (CutSceneDebrisProcessModePc == 2) {
                        DebrisOrientationMtx(instance->effect.index, &matrix);
                    } else {
                        DebrisPosOrientationMtx(instance->effect.index, &matrix);
                    }
                }
                if (CutSceneDebrisAxisSizePc > 0.0f) {
                    NuRndrAxisArrowsMtx(&matrix, CutSceneDebrisAxisSizePc, 0);
                }
            } else {
                AddScaledVariableShotDebrisEffect2(effect, position, (i32)scale, debris_time_increment, &matrix, 0,
                                                   rate);
            }
            return;
        }
    }
    if ((locator->flags & 0x20) != 0) {
        if (instance->effect.index > -1) {
            DebFreeInstantly(&instance->effect.index);
        }
    } else {
        instance->effect.index = 0;
    }
    instance->frame = 0.0f;
}

static void CutMatrixAssignPrivatePc(NUMTX *destination, NUMTX *source) {
    destination->m00 = source->m00;
    destination->m01 = source->m01;
    destination->m02 = source->m02;
    destination->m03 = source->m03;
    destination->m10 = source->m10;
    destination->m11 = source->m11;
    destination->m12 = source->m12;
    destination->m13 = source->m13;
    destination->m20 = source->m20;
    destination->m21 = source->m21;
    destination->m22 = source->m22;
    destination->m23 = source->m23;
    destination->m30 = source->m30;
    destination->m31 = source->m31;
    destination->m32 = source->m32;
    destination->m33 = source->m33;
}

static void CutSceneLocatorVfxUpdatePc(CUT_LOAD_LOCATOR *locator, CUT_LOAD_LOCATOR_SYSTEM *system,
                                       CUT_LOAD_LOCATOR_INSTANCE *instance, f32 frame, NUMTX *parent) {
    i32 effect = system->types[locator->type_index].function_index;
    NUANIM_TIME2_PC time;
    f32 scale = 0.0f;
    NUMTX matrix;
    __declspec(align(16)) NUMTX aligned_matrix;
    if (effect < 0 || TriggerLocatorVfxFn == NULL || ReleaseLocatorVfxFn == NULL) {
        return;
    }
    if (locator->animation != NULL) {
        NuAnimData2CalcTime(locator->animation, frame, &time);
    }
    if (NuGCutLocatorIsVisble((CUT_LOCATOR_GENERIC_PC *)locator, frame, &time, &scale, NULL) != 0 &&
        (f64)scale > 0.10000000149011612) {
        NuGCutLocatorCalcMtx((CUT_LOCATOR_GENERIC_PC *)locator, frame, &matrix, &time);
        if ((locator->flags & 4) != 0) {
            NuMtxPreTranslate(&matrix, &locator->pivot);
        }
        if (parent != NULL) {
            NuMtxMul(&matrix, &matrix, parent);
        }
        CutMatrixAssignPrivatePc(&aligned_matrix, &matrix);
        if (instance->effect.index >= 0 && UpdateLocatorVfxFn != NULL) {
            UpdateLocatorVfxFn(instance->effect.index, &aligned_matrix);
        } else {
            instance->effect.index = TriggerLocatorVfxFn(effect, &aligned_matrix);
        }
    } else {
        if (instance->effect.index >= 0) {
            ReleaseLocatorVfxFn(instance->effect.index);
        }
        instance->effect.index = -1;
    }
}

typedef struct CUT_LOOKAT_MATERIAL_VALUES_PC {
    u8 unknown_00[0x80];         // 0x00
    f32 look_x;                  // 0x80
    f32 look_y;                  // 0x84
} CUT_LOOKAT_MATERIAL_VALUES_PC; // size 0x88

static void CutSceneLocatorLookAtUpdatePc(CUT_LOAD_LOCATOR_INSTANCE *instance, CUT_LOAD_LOCATOR *locator, f32 frame,
                                          NUMTX *parent, INSTNUGCUTSCENE *owner) {
    CUT_LOOKAT_STATE_PC *state;
    CUT_INSTANCE_LOAD_PC *owner_view = (CUT_INSTANCE_LOAD_PC *)owner;
    NUHGOBJ_EVAL_PC *hierarchy;
    CUT_LOOKAT_HIERARCHY_PC *hierarchy_view;
    CUT_LOOKAT_MATERIAL_VALUES_PC *eye_material;
    CUT_LOAD_CHARACTER *character;
    CUT_LOAD_CHARACTER_INSTANCE *character_instance;
    CUT_LOOKAT_TEXTURE_PC *texture;
    NUMTL *material;
    NUMTX locator_matrix;
    NUMTX character_matrix;
    NUMTX joint_offset;
    NUMTX joint_world;
    NUMTX inverse;
    NUMTX joints[256];
    NUVEC4 direction;
    NUVEC4 local_direction;
    f32 *values;
    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 amount;
    f32 limit;
    f32 dx;
    f32 dy;
    i32 visible;
    i32 first;
    i32 second;
    i32 minimum_frame = 0;
    i32 selected_frame;
    u32 animation_index;
    f32 animation_rate;
    f32 blend;
    f32 start;
    i32 layers;
    u32 magic = ((NUANIMDATA2_PC *)locator->animation)->header.magic;
    if ((i32)magic < 0x414e4934 || (i32)magic > 0x414e4935) {
        return;
    }
    state = (CUT_LOOKAT_STATE_PC *)instance->effect.index;
    instGetLookAtLocatorInfo(owner, state);
    if (state->character == 0 || state->hierarchy == NULL) {
        return;
    }
    hierarchy = state->hierarchy;
    hierarchy_view = (CUT_LOOKAT_HIERARCHY_PC *)hierarchy;
    values = NuGCutLocatorCalcMtx((CUT_LOCATOR_GENERIC_PC *)locator, frame, &locator_matrix, NULL);
    if ((locator->flags & 4) != 0) {
        NuMtxPreTranslate(&locator_matrix, &locator->pivot);
    }
    if (parent != NULL) {
        NuMtxMul(&locator_matrix, &locator_matrix, parent);
    }
    first = (i32)(values[3] < 0.0f ? (f64)values[3] - 0.5 : (f64)values[3] + 0.5);
    second = (i32)(values[4] < 0.0f ? (f64)values[4] - 0.5 : (f64)values[4] + 0.5);
    if (state->secondary_material != 0) {
        if (first != 0) {
            character = &owner_view->scene->character_system->characters[state->character - 1];
            character_instance = &owner_view->character_instance->characters[state->character - 1];
            NuGCutCharAnimProcess((CUT_CHARACTER_GENERIC_PC *)character, character_instance->frame_a, &character_matrix,
                                  &visible, &animation_index, &animation_rate, &blend, &start, &layers);
            if (visible == 0) {
                return;
            }
            NuHGobjEvalAnim2(hierarchy, character->face_animation, character_instance->frame_a, 0, NULL, joints);
            CutGetJointOverrideMatrixPc(hierarchy, state->joint - 1, &joint_offset);
            NuMtxMulVU0(&joint_world, &joint_offset, &joints[state->joint - 1]);
            NuMtxMulVU0(&joint_world, &joint_world, &character_matrix);
            direction.x = joint_world.m30 - locator_matrix.m30;
            direction.y = joint_world.m31 - locator_matrix.m31;
            direction.z = joint_world.m32 - locator_matrix.m32;
            direction.w = 0.0f;
            NuMtxTranspose(&inverse, &joint_world);
            NuVec4MtxRotateVU0(&local_direction, &direction, &inverse);
            NuVecNorm((NUVEC *)&local_direction, (NUVEC *)&local_direction);
            if ((f64)local_direction.z > 0.10000000149011612) {
                x = (f32)((f64)local_direction.x * 0.5);
                y = (f32)((f64)local_direction.y * 0.5);
            }
        }
        eye_material = (CUT_LOOKAT_MATERIAL_VALUES_PC *)hierarchy_view->materials[state->secondary_material - 1];
        if ((f64)x > 0.23999999463558197) {
            x = 0.24f;
        }
        if ((f64)x < -0.23999999463558197) {
            x = -0.24f;
        }
        amount = x < 0.0f ? -x : x;
        selected_frame = (i32)(((f64)amount / -0.23999999463558197) * 8192.0);
        limit = (f32)(((f64)NuTrigTable[((0x4000 - selected_frame) >> 1) & 0x7fff] - 0.699999988079071) *
                      0.18000000715255737 / 0.30000001192092896);
        if (y > limit) {
            y = limit;
        }
        if (y < -limit) {
            y = -limit;
        }
        dx = (f32)(((f64)x - eye_material->look_x) * 0.125);
        eye_material->look_x += dx;
        dy = (f32)((-(f64)y - eye_material->look_y) * 0.125);
        eye_material->look_y += dy;
        if (eye_material->look_y < -0.15f) {
            minimum_frame = 1;
        }
    }
    if (state->texture == 0) {
        return;
    }
    if (second != 0) {
        state->blend = (f32)((f64)owner_view->elapsed * 30.0 + state->blend);
        if (state->blend > 3.0f) {
            state->blend = 3.0f;
        }
    } else {
        state->blend = (f32)((f64)state->blend - (f64)owner_view->elapsed * 10.0);
        if (state->blend < 0.0f) {
            state->blend = 0.0f;
        }
    }
    selected_frame = (i32)((f64)state->blend + 0.5);
    if (selected_frame < minimum_frame) {
        selected_frame = minimum_frame;
    }
    material = hierarchy_view->materials[state->material - 1];
    texture = &hierarchy_view->textures[state->texture - 1];
    if (selected_frame < texture->frame_count) {
        material->texture_id = texture->frames[selected_frame];
        NuMtlUpdate(material);
    }
}

void instNuGCutLocatorUpdate(INSTNUGCUTSCENE *owner, CUT_LOAD_LOCATOR_SYSTEM *system,
                             CUT_LOAD_LOCATOR_INSTANCE *instance, CUT_LOAD_LOCATOR *locator, f32 frame, NUMTX *parent,
                             i32 paused) {
    CUT_LOAD_LOCATOR_TYPE *type = &system->types[locator->type_index];
    if ((type->flags & 1) != 0) {
        if (paused == 0) {
            CutSceneLocatorDebrisUpdatePc(locator, system, instance, frame, parent);
        }
    } else if ((type->flags & 2) != 0) {
        if (type->function_index != 0xffff) {
            CUT_UPDATE_ENTRY_PC *entry = &((CUT_UPDATE_ENTRY_PC *)CutSceneLocatorFunctionsPc)[type->function_index];
            if (entry->update != NULL) {
                entry->update(owner, system, instance, locator, frame, parent, paused);
            }
        }
    } else if ((type->flags & 0x10) != 0) {
        if (paused == 0) {
            CutSceneLocatorVfxUpdatePc(locator, system, instance, frame, parent);
        }
    } else if ((type->flags & 4) != 0) {
        if (NuCutSceneSFXUpdate != NULL && ((CUT_INSTANCE_LOAD_PC *)owner)->rate != 0.0f) {
            NuCutSceneSFXUpdate(system, instance, locator, frame, parent, paused);
        }
    } else if ((type->flags & 8) != 0) {
        if (paused == 0) {
            CutSceneLocatorLookAtUpdatePc(instance, locator, frame, parent, owner);
        }
    }
}
