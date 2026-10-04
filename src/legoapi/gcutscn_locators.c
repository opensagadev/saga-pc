#include "saga.h"
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
    u8 unknown_04[8];
    f32 frame_a;
    u8 unknown_10[5];
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

STATIC_ASSERT(offsetof(CUT_INSTANCE_LOAD_PC, elapsed) == 0xe4);
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR, pivot) == 0x40);
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR, scale) == 0x4c);
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR_TYPE, flags) == 4);
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR_SYSTEM, locators) == 0);
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR_SYSTEM, types) == 4);
STATIC_ASSERT(offsetof(CUT_LOAD_LOCATOR_INSTANCE, effect) == 4);
STATIC_ASSERT(offsetof(CUT_LOAD_CHARACTER_INSTANCE, frame_a) == 0xc);

typedef struct CUT_LOOKAT_TEXTURE_PC {
    char *name;
    u32 unknown_04;
    i16 *frames;
    i16 frame_count, unknown_0e;
    NUMTL *material;
    u8 unknown_14[12];
} CUT_LOOKAT_TEXTURE_PC;
STATIC_ASSERT(sizeof(CUT_LOOKAT_TEXTURE_PC) == 0x20);
typedef struct CUT_LOOKAT_HIERARCHY_PC {
    u8 unknown_00[12];
    NUMTL **materials;
    i32 material_count;
    u8 unknown_14[0x4c];
    i32 texture_count;
    CUT_LOOKAT_TEXTURE_PC *textures;
} CUT_LOOKAT_HIERARCHY_PC;
STATIC_ASSERT(offsetof(CUT_LOOKAT_HIERARCHY_PC, materials) == 0xc);
STATIC_ASSERT(offsetof(CUT_LOOKAT_HIERARCHY_PC, material_count) == 0x10);
STATIC_ASSERT(offsetof(CUT_LOOKAT_HIERARCHY_PC, texture_count) == 0x60);
STATIC_ASSERT(offsetof(CUT_LOOKAT_HIERARCHY_PC, textures) == 0x64);
typedef struct CUT_LOOKAT_MATERIAL_PC {
    u8 unknown_00[0x9a];
    u8 type;
} CUT_LOOKAT_MATERIAL_PC;
STATIC_ASSERT(offsetof(CUT_LOOKAT_MATERIAL_PC, type) == 0x9a);
typedef struct CUT_LOOKAT_LOCATOR_INFO_PC {
    u8 unknown_00[0x5b];
    u8 character_id;
    u8 joint, material, secondary_material, tertiary_material;
} CUT_LOOKAT_LOCATOR_INFO_PC;
STATIC_ASSERT(offsetof(CUT_LOOKAT_LOCATOR_INFO_PC, character_id) == 0x5b);
STATIC_ASSERT(offsetof(CUT_LOOKAT_LOCATOR_INFO_PC, joint) == 0x5c);
typedef struct CUT_LOOKAT_STATE_PC {
    NUHGOBJ_EVAL_PC *hierarchy;
    f32 blend;
    u16 character, material, texture, secondary_material, tertiary_material, joint;
    CUT_LOOKAT_LOCATOR_INFO_PC *locator;
} CUT_LOOKAT_STATE_PC;
STATIC_ASSERT(sizeof(CUT_LOOKAT_STATE_PC) == 0x18);
STATIC_ASSERT(offsetof(CUT_LOOKAT_STATE_PC, character) == 8);
STATIC_ASSERT(offsetof(CUT_LOOKAT_STATE_PC, joint) == 0x12);
STATIC_ASSERT(offsetof(CUT_LOOKAT_STATE_PC, locator) == 0x14);
STATIC_ASSERT(offsetof(CUT_LOOKAT_TEXTURE_PC, frames) == 8);
STATIC_ASSERT(offsetof(CUT_LOOKAT_TEXTURE_PC, frame_count) == 0xc);
STATIC_ASSERT(offsetof(CUT_LOOKAT_TEXTURE_PC, material) == 0x10);
STATIC_ASSERT(offsetof(CUT_LOOKAT_LOCATOR_INFO_PC, material) == 0x5d);
STATIC_ASSERT(offsetof(CUT_LOOKAT_LOCATOR_INFO_PC, secondary_material) == 0x5e);
STATIC_ASSERT(offsetof(CUT_LOOKAT_LOCATOR_INFO_PC, tertiary_material) == 0x5f);
STATIC_ASSERT(offsetof(CUT_LOOKAT_STATE_PC, hierarchy) == 0);
STATIC_ASSERT(offsetof(CUT_LOOKAT_STATE_PC, blend) == 4);
STATIC_ASSERT(offsetof(CUT_LOOKAT_STATE_PC, material) == 0xa);
STATIC_ASSERT(offsetof(CUT_LOOKAT_STATE_PC, texture) == 0xc);
STATIC_ASSERT(offsetof(CUT_LOOKAT_STATE_PC, secondary_material) == 0xe);
STATIC_ASSERT(offsetof(CUT_LOOKAT_STATE_PC, tertiary_material) == 0x10);
extern NUHGOBJ_EVAL_PC *(__cdecl *NuCutSceneGetHGObj)(INSTNUGCUTSCENE *, i32);

i32 __cdecl FindMtlInHGObj(NUHGOBJ_EVAL_PC *hierarchy, i32 material_type) {
    CUT_LOOKAT_HIERARCHY_PC *view = (CUT_LOOKAT_HIERARCHY_PC *)hierarchy;
    i32 index;
    for (index = 0; index < view->material_count; ++index) {
        CUT_LOOKAT_MATERIAL_PC *material = (CUT_LOOKAT_MATERIAL_PC *)view->materials[index];
        if (material->type == material_type)
            return index + 1;
    }
    return 0;
}
i32 __cdecl FindTexAnimFromMtl(NUHGOBJ_EVAL_PC *hierarchy, NUMTL *material) {
    CUT_LOOKAT_HIERARCHY_PC *view = (CUT_LOOKAT_HIERARCHY_PC *)hierarchy;
    i32 index;
    for (index = 0; index < view->texture_count; ++index)
        if (view->textures[index].material == material)
            return index + 1;
    return 0;
}
void __cdecl instGetLookAtLocatorInfo(INSTNUGCUTSCENE *owner, CUT_LOOKAT_STATE_PC *state) {
    CUT_LOOKAT_LOCATOR_INFO_PC *locator = state->locator;
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    CUT_SCENE_LOAD_PC *scene;
    NUHGOBJ_EVAL_PC *hierarchy;
    i32 i, character_id;
    if (locator == NULL || NuCutSceneGetHGObj == NULL)
        return;
    character_id = locator->character_id;
    state->locator = NULL;
    scene = instance->scene;
    for (i = 0; i < scene->character_system->count; ++i) {
        if ((u8)scene->character_animations[i].unknown_00 == character_id) {
            state->character = (u16)(i + 1);
            break;
        }
    }
    if (state->character == 0)
        return;
    hierarchy = NuCutSceneGetHGObj(owner, state->character - 1);
    state->hierarchy = hierarchy;
    if (hierarchy == NULL)
        return;
    if (locator->joint < hierarchy->joint_override_map_count)
        state->joint = (u16)(hierarchy->joint_override_map[locator->joint] + 1);
    state->material = (u16)FindMtlInHGObj(hierarchy, locator->material);
    if (state->material != 0)
        state->texture =
            (u16)FindTexAnimFromMtl(hierarchy, ((CUT_LOOKAT_HIERARCHY_PC *)hierarchy)->materials[state->material - 1]);
    state->secondary_material = (u16)FindMtlInHGObj(hierarchy, locator->secondary_material);
    state->tertiary_material = (u16)FindMtlInHGObj(hierarchy, locator->tertiary_material);
}
void __cdecl CutGetJointOverrideMatrixPc(NUHGOBJ_EVAL_PC *hierarchy, i32 index, NUMTX *matrix) {
    NUHGOBJ_EVAL_JOINT_PC *joint = &hierarchy->joints[index];
    *matrix = NuMtxSetIdentityTemplate;
    for (;;) {
        if (joint->bind_matrix.m00 != 1.0f || joint->bind_matrix.m11 != 1.0f || joint->bind_matrix.m22 != 1.0f)
            NuMtxMulVU0(matrix, &joint->bind_matrix, matrix);
        if (joint->parent_index == 0xff)
            break;
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
    NUMTX matrix;
    NUVEC pivot;
    f32 scale;
    NUANIMDATA2_PC *animation;
    u32 unknown_54;
    u8 flags, type, joint, self;
    u8 unknown_5c[8];
} CUT_UPDATE_LOCATOR_MATRIX_PC;
STATIC_ASSERT(sizeof(CUT_UPDATE_LOCATOR_MATRIX_PC) == 0x64);
STATIC_ASSERT(offsetof(CUT_UPDATE_LOCATOR_MATRIX_PC, flags) == 0x58);
typedef struct CUT_UPDATE_ENTRY_PC {
    char *name;
    u32 unknown_04, unknown_08, unknown_0c;
    void(__cdecl *update)(INSTNUGCUTSCENE *, CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *, CUT_LOAD_LOCATOR *,
                          f32, NUMTX *, i32);
} CUT_UPDATE_ENTRY_PC;
STATIC_ASSERT(sizeof(CUT_UPDATE_ENTRY_PC) == 20);
extern i32(__cdecl *TriggerLocatorVfxFn)(i32, NUMTX *);
extern void(__cdecl *UpdateLocatorVfxFn)(i32, NUMTX *);
extern void(__cdecl *NuCutSceneSFXUpdate)(CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *, CUT_LOAD_LOCATOR *,
                                          f32, NUMTX *, i32);
extern i32 CutSceneDebrisProcessModePc;
extern f32 CutSceneDebrisAxisSizePc;

static void CutSceneLocatorDebrisUpdatePc(CUT_LOAD_LOCATOR *locator, CUT_LOAD_LOCATOR_SYSTEM *system,
                                          CUT_LOAD_LOCATOR_INSTANCE *instance, f32 frame, NUMTX *parent) {
    i32 effect = system->types[locator->type_index].function_index;
    NUANIM_TIME2_PC time;
    f32 scale, rate;
    i32 visible;
    NUMTX matrix;
    NUVEC *position;
    i32 finite;
    if (effect == 0xffff || debtab[effect] == NULL)
        return;
    if (locator->animation != NULL)
        NuAnimData2CalcTime((NUANIMDATA2_PC *)locator->animation, frame, &time);
    visible = NuGCutLocatorIsVisble((CUT_LOCATOR_GENERIC_PC *)locator, frame, &time, &scale, &rate);
    if (visible != 0) {
        if (scale < 0.0f && (locator->flags & 0x10) == 0)
            scale = (f32)debtab[effect]->frequency * -scale;
        if ((f64)scale > 0.10000000149011612) {
            if ((locator->flags & 0x10) != 0 && instance->effect.index != 0)
                return;
            NuGCutLocatorCalcMtx((CUT_LOCATOR_GENERIC_PC *)locator, frame, &matrix, &time);
            if ((locator->flags & 4) != 0)
                NuMtxPreTranslate(&matrix, &locator->pivot);
            if (parent != NULL)
                NuMtxMul(&matrix, &matrix, parent);
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
                    if (CutSceneDebrisProcessModePc == 1)
                        DebrisEmitterPos(instance->effect.index, position->x, position->y, position->z);
                    else if (CutSceneDebrisProcessModePc == 2)
                        DebrisOrientationMtx(instance->effect.index, &matrix);
                    else
                        DebrisPosOrientationMtx(instance->effect.index, &matrix);
                }
                if (CutSceneDebrisAxisSizePc > 0.0f)
                    NuRndrAxisArrowsMtx(&matrix, CutSceneDebrisAxisSizePc, 0);
            } else
                AddScaledVariableShotDebrisEffect2(effect, position, (i32)scale, debris_time_increment, &matrix, 0,
                                                   rate);
            return;
        }
    }
    if ((locator->flags & 0x20) != 0) {
        if (instance->effect.index > -1)
            DebFreeInstantly(&instance->effect.index);
    } else
        instance->effect.index = 0;
    instance->unknown_00 = 0;
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
    if (effect < 0 || TriggerLocatorVfxFn == NULL || ReleaseLocatorVfxFn == NULL)
        return;
    if (locator->animation != NULL)
        NuAnimData2CalcTime((NUANIMDATA2_PC *)locator->animation, frame, &time);
    if (NuGCutLocatorIsVisble((CUT_LOCATOR_GENERIC_PC *)locator, frame, &time, &scale, NULL) != 0 &&
        (f64)scale > 0.10000000149011612) {
        NuGCutLocatorCalcMtx((CUT_LOCATOR_GENERIC_PC *)locator, frame, &matrix, &time);
        if ((locator->flags & 4) != 0)
            NuMtxPreTranslate(&matrix, &locator->pivot);
        if (parent != NULL)
            NuMtxMul(&matrix, &matrix, parent);
        CutMatrixAssignPrivatePc(&aligned_matrix, &matrix);
        if (instance->effect.index >= 0 && UpdateLocatorVfxFn != NULL)
            UpdateLocatorVfxFn(instance->effect.index, &aligned_matrix);
        else
            instance->effect.index = TriggerLocatorVfxFn(effect, &aligned_matrix);
    } else {
        if (instance->effect.index >= 0)
            ReleaseLocatorVfxFn(instance->effect.index);
        instance->effect.index = -1;
    }
}

typedef struct CUT_LOOKAT_MATERIAL_VALUES_PC {
    u8 unknown_00[0x80];
    f32 look_x, look_y;
} CUT_LOOKAT_MATERIAL_VALUES_PC;
STATIC_ASSERT(offsetof(CUT_LOOKAT_MATERIAL_VALUES_PC, look_x) == 0x80);
STATIC_ASSERT(offsetof(CUT_LOOKAT_MATERIAL_VALUES_PC, look_y) == 0x84);
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
    NUMTX locator_matrix, character_matrix, joint_offset, joint_world, inverse;
    NUMTX joints[256];
    NUVEC4 direction, local_direction;
    f32 *values, x = 0.0f, y = 0.0f;
    f32 amount, limit, dx, dy;
    i32 visible, first, second, minimum_frame = 0, selected_frame;
    u32 animation_index;
    f32 animation_rate, blend, start;
    i32 layers;
    u32 magic = ((NUANIMDATA2_PC *)locator->animation)->header.magic;
    if ((i32)magic < (i32)0x414e4934 || (i32)magic > (i32)0x414e4935)
        return;
    state = (CUT_LOOKAT_STATE_PC *)instance->effect.index;
    instGetLookAtLocatorInfo(owner, state);
    if (state->character == 0 || state->hierarchy == NULL)
        return;
    hierarchy = state->hierarchy;
    hierarchy_view = (CUT_LOOKAT_HIERARCHY_PC *)hierarchy;
    values = NuGCutLocatorCalcMtx((CUT_LOCATOR_GENERIC_PC *)locator, frame, &locator_matrix, NULL);
    if ((locator->flags & 4) != 0)
        NuMtxPreTranslate(&locator_matrix, &locator->pivot);
    if (parent != NULL)
        NuMtxMul(&locator_matrix, &locator_matrix, parent);
    first = (i32)(values[3] < 0.0f ? (f64)values[3] - 0.5 : (f64)values[3] + 0.5);
    second = (i32)(values[4] < 0.0f ? (f64)values[4] - 0.5 : (f64)values[4] + 0.5);
    if (state->secondary_material != 0) {
        if (first != 0) {
            character = &owner_view->scene->character_system->characters[state->character - 1];
            character_instance = &owner_view->character_instance->characters[state->character - 1];
            NuGCutCharAnimProcess((CUT_CHARACTER_GENERIC_PC *)character, character_instance->frame_a, &character_matrix,
                                  &visible, &animation_index, &animation_rate, &blend, &start, &layers);
            if (visible == 0)
                return;
            NuHGobjEvalAnim2(hierarchy, (ANI3_HEADER_PC *)character->face_animation, character_instance->frame_a, 0,
                             NULL, joints);
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
        if ((f64)x > 0.23999999463558197)
            x = 0.24f;
        if ((f64)x < -0.23999999463558197)
            x = -0.24f;
        amount = x < 0.0f ? -x : x;
        selected_frame = (i32)(((f64)amount / -0.23999999463558197) * 8192.0);
        limit = (f32)(((f64)NuTrigTable[((0x4000 - selected_frame) >> 1) & 0x7fff] - 0.699999988079071) *
                      0.18000000715255737 / 0.30000001192092896);
        if (y > limit)
            y = limit;
        if (y < -limit)
            y = -limit;
        dx = (f32)(((f64)x - eye_material->look_x) * 0.125);
        eye_material->look_x += dx;
        dy = (f32)((-(f64)y - eye_material->look_y) * 0.125);
        eye_material->look_y += dy;
        if (eye_material->look_y < -0.15f)
            minimum_frame = 1;
    }
    if (state->texture == 0)
        return;
    if (second != 0) {
        state->blend = (f32)((f64)owner_view->elapsed * 30.0 + state->blend);
        if (state->blend > 3.0f)
            state->blend = 3.0f;
    } else {
        state->blend = (f32)((f64)state->blend - (f64)owner_view->elapsed * 10.0);
        if (state->blend < 0.0f)
            state->blend = 0.0f;
    }
    selected_frame = (i32)((f64)state->blend + 0.5);
    if (selected_frame < minimum_frame)
        selected_frame = minimum_frame;
    material = hierarchy_view->materials[state->material - 1];
    texture = &hierarchy_view->textures[state->texture - 1];
    if (selected_frame < texture->frame_count) {
        material->texture_id = texture->frames[selected_frame];
        NuMtlUpdate(material);
    }
}

void __cdecl instNuGCutLocatorUpdate(INSTNUGCUTSCENE *owner, CUT_LOAD_LOCATOR_SYSTEM *system,
                                     CUT_LOAD_LOCATOR_INSTANCE *instance, CUT_LOAD_LOCATOR *locator, f32 frame,
                                     NUMTX *parent, i32 paused) {
    CUT_LOAD_LOCATOR_TYPE *type = &system->types[locator->type_index];
    if ((type->flags & 1) != 0) {
        if (paused == 0)
            CutSceneLocatorDebrisUpdatePc(locator, system, instance, frame, parent);
    } else if ((type->flags & 2) != 0) {
        if (type->function_index != 0xffff) {
            CUT_UPDATE_ENTRY_PC *entry = &((CUT_UPDATE_ENTRY_PC *)CutSceneLocatorFunctionsPc)[type->function_index];
            if (entry->update != NULL)
                entry->update(owner, system, instance, locator, frame, parent, paused);
        }
    } else if ((type->flags & 0x10) != 0) {
        if (paused == 0)
            CutSceneLocatorVfxUpdatePc(locator, system, instance, frame, parent);
    } else if ((type->flags & 4) != 0) {
        if (NuCutSceneSFXUpdate != NULL && ((CUT_INSTANCE_LOAD_PC *)owner)->rate != 0.0f)
            NuCutSceneSFXUpdate(system, instance, locator, frame, parent, paused);
    } else if ((type->flags & 8) != 0) {
        if (paused == 0)
            CutSceneLocatorLookAtUpdatePc(instance, locator, frame, parent, owner);
    }
}
