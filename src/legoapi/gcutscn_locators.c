#include "gameapi/edptl.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/text_tables.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/nuanimdata.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/trigger.h"

typedef struct CUT_LOOKAT_TEXTURE { // size 0x20
    char *name;                     // 0x00
    u32 unknown_04;                 // 0x04
    i16 *frames;                    // 0x08
    i16 frame_count;                // 0x0c
    i16 unknown_0e;                 // 0x0e
    NUMTL *material;                // 0x10
    u8 unknown_14[12];              // 0x14
} CUT_LOOKAT_TEXTURE;

typedef struct CUT_LOOKAT_HIERARCHY { // size 0x68
    u8 unknown_00[12];                // 0x00
    NUMTL **materials;                // 0x0c
    i32 material_count;               // 0x10
    u8 unknown_14[0x4c];              // 0x14
    i32 texture_count;                // 0x60
    CUT_LOOKAT_TEXTURE *textures;     // 0x64
} CUT_LOOKAT_HIERARCHY;

typedef struct CUT_LOOKAT_MATERIAL { // size 0x9b
    u8 unknown_00[0x9a];             // 0x00
    u8 type;                         // 0x9a
} CUT_LOOKAT_MATERIAL;

typedef struct CUT_LOOKAT_LOCATOR_INFO { // size 0x60
    u8 unknown_00[0x5b];                 // 0x00
    u8 character_id;                     // 0x5b
    u8 joint;                            // 0x5c
    u8 material;                         // 0x5d
    u8 secondary_material;               // 0x5e
    u8 tertiary_material;                // 0x5f
} CUT_LOOKAT_LOCATOR_INFO;

typedef struct CUT_LOOKAT_STATE {     // size 0x18
    NUHGOBJ_EVAL *hierarchy;          // 0x00
    f32 blend;                        // 0x04
    u16 character;                    // 0x08
    u16 material;                     // 0x0a
    u16 texture;                      // 0x0c
    u16 secondary_material;           // 0x0e
    u16 tertiary_material;            // 0x10
    u16 joint;                        // 0x12
    CUT_LOOKAT_LOCATOR_INFO *locator; // 0x14
} CUT_LOOKAT_STATE;

i32 FindMtlInHGObj(NUHGOBJ_EVAL *hierarchy, i32 material_type) {
    CUT_LOOKAT_HIERARCHY *view = (CUT_LOOKAT_HIERARCHY *)hierarchy;
    i32 index;
    for (index = 0; index < view->material_count; ++index) {
        CUT_LOOKAT_MATERIAL *material = (CUT_LOOKAT_MATERIAL *)view->materials[index];
        if (material->type == material_type) {
            return index + 1;
        }
    }
    return 0;
}

i32 FindTexAnimFromMtl(NUHGOBJ_EVAL *hierarchy, NUMTL *material) {
    CUT_LOOKAT_HIERARCHY *view = (CUT_LOOKAT_HIERARCHY *)hierarchy;
    i32 index;
    for (index = 0; index < view->texture_count; ++index) {
        if (view->textures[index].material == material) {
            return index + 1;
        }
    }
    return 0;
}

void instGetLookAtLocatorInfo(INSTNUGCUTSCENE *owner, CUT_LOOKAT_STATE *state) {
    CUT_LOOKAT_LOCATOR_INFO *locator = state->locator;
    CUT_INSTANCE_LOAD_PC *instance = (CUT_INSTANCE_LOAD_PC *)owner;
    CUT_SCENE_LOAD_PC *scene;
    CUT_LOAD_CHARACTER_ANIMATION *animations;
    NUHGOBJ_EVAL *hierarchy;
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
        state->texture = (u16)FindTexAnimFromMtl(hierarchy, ((CUT_LOOKAT_HIERARCHY *)hierarchy)->materials[state->material - 1]);
    }
    state->secondary_material = (u16)FindMtlInHGObj(hierarchy, locator->secondary_material);
    state->tertiary_material = (u16)FindMtlInHGObj(hierarchy, locator->tertiary_material);
}

void CutGetJointOverrideMatrix(NUHGOBJ_EVAL *hierarchy, i32 index, NUMTX *matrix) {
    NUHGOBJ_EVAL_JOINT *joint = &hierarchy->joints[index];
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

typedef struct CUT_UPDATE_LOCATOR_MATRIX { // size 0x64
    NUMTX matrix;                          // 0x00
    NUVEC pivot;                           // 0x40
    f32 scale;                             // 0x4c
    NUANIMDATA2 *animation;                // 0x50
    u32 unknown_54;                        // 0x54
    u8 flags;                              // 0x58
    u8 type;                               // 0x59
    u8 joint;                              // 0x5a
    u8 self;                               // 0x5b
    u8 unknown_5c[8];                      // 0x5c
} CUT_UPDATE_LOCATOR_MATRIX;

typedef struct CUT_UPDATE_ENTRY { // size 0x14
    char *name;                   // 0x00
    u32 unknown_04;               // 0x04
    u32 unknown_08;               // 0x08
    u32 unknown_0c;               // 0x0c
    void (*update)(INSTNUGCUTSCENE *, CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *, CUT_LOAD_LOCATOR *, f32, NUMTX *,
                   i32); // 0x10
} CUT_UPDATE_ENTRY;

f32 CutSceneDebrisAxisSize = 0;
i32 CutSceneDebrisProcessMode = 0;

static void CutSceneLocatorDebrisUpdate(CUT_LOAD_LOCATOR *locator, CUT_LOAD_LOCATOR_SYSTEM *system, CUT_LOAD_LOCATOR_INSTANCE *instance,
                                        f32 frame, NUMTX *parent) {
    u16 effect = system->types[locator->type_index].function_index;
    NUANIM_TIME2 time;
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
    if (NuGCutLocatorIsVisble((CUT_LOCATOR_GENERIC *)locator, frame, &time, &scale, &rate) != 0) {
        if (scale < 0.0f && (locator->flags & 0x10) == 0) {
            // MATCH-EXCEPTION: the original negates with fldz/fsubrp (0 - scale), unary minus gives fchs.
            scale = (f32)debtab[effect]->frequency * (0.0f - scale);
        }
        if ((f64)scale > 0.10000000149011612) {
            if ((locator->flags & 0x10) != 0 && instance->effect.index != 0) {
                return;
            }
            NuGCutLocatorCalcMtx((CUT_LOCATOR_GENERIC *)locator, frame, &matrix, &time);
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
                    if (CutSceneDebrisProcessMode == 1) {
                        DebrisEmitterPos(instance->effect.index, position->x, position->y, position->z);
                    } else if (CutSceneDebrisProcessMode == 2) {
                        DebrisOrientationMtx(instance->effect.index, &matrix);
                    } else {
                        DebrisPosOrientationMtx(instance->effect.index, &matrix);
                    }
                }
                if (CutSceneDebrisAxisSize > 0.0f) {
                    NuRndrAxisArrowsMtx(&matrix, CutSceneDebrisAxisSize, 0);
                }
            } else {
                AddScaledVariableShotDebrisEffect2(effect, position, (i32)scale, debris_time_increment, &matrix, 0, rate);
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

static void CutMatrixAssignPrivate(NUMTX *destination, NUMTX *source) {
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

static void CutSceneLocatorVfxUpdate(CUT_LOAD_LOCATOR *locator, CUT_LOAD_LOCATOR_SYSTEM *system, CUT_LOAD_LOCATOR_INSTANCE *instance,
                                     f32 frame, NUMTX *parent) {
    i32 effect = system->types[locator->type_index].function_index;
    NUANIM_TIME2 time;
    f32 scale = 0.0f;
    NUMTX matrix;
    __declspec(align(16)) NUMTX aligned_matrix;
    if (effect < 0 || TriggerLocatorVfxFn == NULL || ReleaseLocatorVfxFn == NULL) {
        return;
    }
    if (locator->animation != NULL) {
        NuAnimData2CalcTime(locator->animation, frame, &time);
    }
    if (NuGCutLocatorIsVisble((CUT_LOCATOR_GENERIC *)locator, frame, &time, &scale, NULL) != 0 && (f64)scale > 0.10000000149011612) {
        NuGCutLocatorCalcMtx((CUT_LOCATOR_GENERIC *)locator, frame, &matrix, &time);
        if ((locator->flags & 4) != 0) {
            NuMtxPreTranslate(&matrix, &locator->pivot);
        }
        if (parent != NULL) {
            NuMtxMul(&matrix, &matrix, parent);
        }
        CutMatrixAssignPrivate(&aligned_matrix, &matrix);
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

typedef struct CUT_LOOKAT_MATERIAL_VALUES { // size 0x88
    u8 unknown_00[0x80];                    // 0x00
    f32 look_x;                             // 0x80
    f32 look_y;                             // 0x84
} CUT_LOOKAT_MATERIAL_VALUES;

static void CutSceneLocatorLookAtUpdate(CUT_LOAD_LOCATOR_INSTANCE *instance, CUT_LOAD_LOCATOR *locator, f32 frame, NUMTX *parent,
                                        INSTNUGCUTSCENE *owner) {
    CUT_LOOKAT_STATE *state;
    CUT_INSTANCE_LOAD_PC *owner_view = (CUT_INSTANCE_LOAD_PC *)owner;
    NUHGOBJ_EVAL *hierarchy;
    CUT_LOOKAT_HIERARCHY *hierarchy_view;
    CUT_LOOKAT_MATERIAL_VALUES *eye_material;
    CUT_LOAD_CHARACTER *character;
    CUT_LOAD_CHARACTER_INSTANCE *character_instance;
    CUT_LOOKAT_TEXTURE *texture;
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
    u32 magic = ((NUANIMDATA2 *)locator->animation)->header.magic;
    if ((i32)magic < 0x414e4934 || (i32)magic > 0x414e4935) {
        return;
    }
    state = (CUT_LOOKAT_STATE *)instance->effect.index;
    instGetLookAtLocatorInfo(owner, state);
    if (state->character == 0 || state->hierarchy == NULL) {
        return;
    }
    hierarchy = state->hierarchy;
    hierarchy_view = (CUT_LOOKAT_HIERARCHY *)hierarchy;
    values = NuGCutLocatorCalcMtx((CUT_LOCATOR_GENERIC *)locator, frame, &locator_matrix, NULL);
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
            NuGCutCharAnimProcess((CUT_CHARACTER_GENERIC *)character, character_instance->frame_a, &character_matrix, &visible,
                                  &animation_index, &animation_rate, &blend, &start, &layers);
            if (visible == 0) {
                return;
            }
            NuHGobjEvalAnim2(hierarchy, character->face_animation, character_instance->frame_a, 0, NULL, joints);
            CutGetJointOverrideMatrix(hierarchy, state->joint - 1, &joint_offset);
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
        eye_material = (CUT_LOOKAT_MATERIAL_VALUES *)hierarchy_view->materials[state->secondary_material - 1];
        if ((f64)x > 0.23999999463558197) {
            x = 0.24f;
        }
        if ((f64)x < -0.23999999463558197) {
            x = -0.24f;
        }
        amount = x < 0.0f ? -x : x;
        selected_frame = (i32)(((f64)amount / -0.23999999463558197) * 8192.0);
        limit = (f32)(((f64)NuTrigTable[((0x4000 - selected_frame) >> 1) & 0x7fff] - 0.699999988079071) * 0.18000000715255737 /
                      0.30000001192092896);
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

void instNuGCutLocatorUpdate(INSTNUGCUTSCENE *owner, CUT_LOAD_LOCATOR_SYSTEM *system, CUT_LOAD_LOCATOR_INSTANCE *instance,
                             CUT_LOAD_LOCATOR *locator, f32 frame, NUMTX *parent, i32 paused) {
    CUT_LOAD_LOCATOR_TYPE *type = &system->types[locator->type_index];
    if ((type->flags & 1) != 0) {
        if (paused == 0) {
            CutSceneLocatorDebrisUpdate(locator, system, instance, frame, parent);
        }
    } else if ((type->flags & 2) != 0) {
        if (type->function_index != 0xffff) {
            CUT_UPDATE_ENTRY *entry = &((CUT_UPDATE_ENTRY *)CutSceneLocatorFunctions)[type->function_index];
            if (entry->update != NULL) {
                entry->update(owner, system, instance, locator, frame, parent, paused);
            }
        }
    } else if ((type->flags & 0x10) != 0) {
        if (paused == 0) {
            CutSceneLocatorVfxUpdate(locator, system, instance, frame, parent);
        }
    } else if ((type->flags & 4) != 0) {
        if (NuCutSceneSFXUpdate != NULL && ((CUT_INSTANCE_LOAD_PC *)owner)->rate != 0.0f) {
            NuCutSceneSFXUpdate(system, instance, locator, frame, parent, paused);
        }
    } else if ((type->flags & 8) != 0) {
        if (paused == 0) {
            CutSceneLocatorLookAtUpdate(instance, locator, frame, parent, owner);
        }
    }
}
