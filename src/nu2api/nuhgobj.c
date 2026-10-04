#include "saga.h"

#include <string.h>

i32 __cdecl NuHGobjGetLayerIndex(const char *name, NUHGOBJ_EVAL_PC *object) {
    i32 i;
    for (i = 0; i < object->render_count; ++i) {
        if (NuStrICmp(name, object->render_parts[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

i32 __cdecl NuHGobjReversibleCharacters(i32 enabled) {
    i32 previous = nuapi.reversible_characters;
    nuapi.reversible_characters = enabled;
    return previous;
}

i32 __cdecl NuHGobjForceShadowsOnCharacters(i32 enabled) {
    i32 previous = nuapi.force_shadows_on_characters;
    nuapi.force_shadows_on_characters = enabled;
    return previous;
}

void __cdecl NuHGobjPOIMtx(NUHGOBJ_EVAL_PC *object, u8 index, NUMTX *world_matrix, NUMTX *joint_matrices,
                           NUMTX *result) {
    NUHGOBJ_POI_PC *point = &object->points[object->point_index_map[index]];
    NuMtxMulVU0(result, &point->matrix, &joint_matrices[point->joint_index]);
    NuMtxMulVU0(result, result, world_matrix);
}

void __cdecl NuHGobjEval(NUHGOBJ_EVAL_PC *object, i32 override_count, NUHGOBJ_JOINT_OVERRIDE_PC *overrides,
                         NUMTX *matrices) {
    NUHGOBJ_JOINT_OVERRIDE_PC *override_by_joint[256];
    u8 i;
    if (override_count != 0) {
        memset(override_by_joint, 0, object->joint_count * sizeof(override_by_joint[0]));
        for (i = 0; i < override_count; ++i) {
            u8 index = overrides[i].joint_index;
            if (index < object->joint_override_map_count) {
                u8 joint = object->joint_override_map[index];
                if (joint != 0xff)
                    override_by_joint[joint] = &overrides[i];
            }
        }
    }
    for (i = 0; i < object->joint_count; ++i) {
        NUMTX *local;
        NUMTX transformed, rotation;
        if (override_count != 0 && override_by_joint[i] != NULL) {
            NUHGOBJ_JOINT_OVERRIDE_PC *item = override_by_joint[i];
            NUANGVEC angles;
            transformed = object->bind_matrices[i];
            angles.x = (i32)(item->rotation.x * AIPathAngularScale);
            angles.y = (i32)(item->rotation.y * AIPathAngularScale);
            angles.z = (i32)(item->rotation.z * AIPathAngularScale);
            NuMtxSetRotateXYZVU0(&rotation, &angles);
            NuMtxTranslate(&rotation, &item->translation);
            NuMtxMulVU0(&transformed, &rotation, &transformed);
            local = &transformed;
        } else {
            local = &object->bind_matrices[i];
        }
        if (object->joints[i].parent_index == 0xff)
            matrices[i] = *local;
        else
            NuMtxMulVU0(&matrices[i], local, &matrices[object->joints[i].parent_index]);
    }
}

typedef void(__cdecl *ANI3_OVERRIDE_CALLBACK_WORKING_PC)(ANI3_BUFFER_PC *, NUHGOBJ_EVAL_PC *, i32, void *);

void __cdecl NuHGobjEvalAnim2Root_3(NUHGOBJ_EVAL_PC *object, ANI3_HEADER_PC *animation, f32 time, i32 override_count,
                                    void *overrides, NUMTX *matrices, ANI3_ROOT_CALLBACK_PC root_fn, void *root_data) {
    ANI3_BUFFER_PC buffer;
    NUVEC root_translation = {0.0f, 0.0f, 0.0f};
    volatile f32 adjusted_time;
    ANI3_OVERRIDE_CALLBACK_WORKING_PC callback;
    buffer.joint_count = 0;
    buffer.max_joints = globalbuffer->scratch_joint_capacity;
    buffer.joints = (ANI3_JOINT_PC *)(((u32)NuScratchAlloc32((buffer.max_joints + 1) * 48) + 15u) & ~15u);
    buffer.joint_flags = (u8 *)NuScratchAlloc32(buffer.max_joints);
    buffer.joint_count = object->joint_count;
    adjusted_time = time - 1.0;
    ANI_SimpleAni3PlayerV4Joint(animation, adjusted_time, &buffer, object->joint_count, 0);
    callback = (ANI3_OVERRIDE_CALLBACK_WORKING_PC)nuapi_endframe_callbackfn;
    if (override_count != 0 && callback != NULL)
        callback(&buffer, object, override_count, overrides);
    NuAnimBuffEvaluate_3(&buffer, object, matrices, animation, root_fn, &root_translation, root_data);
    NuScratchRelease();
    NuScratchRelease();
}

void __cdecl NuHGobjEvalAnimBlend2Root_3(NUHGOBJ_EVAL_PC *object, ANI3_HEADER_PC *animation_a, f32 time_a,
                                         ANI3_HEADER_PC *animation_b, f32 time_b, f32 blend, i32 override_count,
                                         void *overrides, NUMTX *matrices, ANI3_ROOT_CALLBACK_PC root_fn,
                                         void *root_data) {
    ANI3_BUFFER_PC buffer;
    NUVEC root_translation = {0.0f, 0.0f, 0.0f};
    NUVEC blend_translation;
    volatile f32 adjusted_time_a, adjusted_time_b;
    ANI3_OVERRIDE_CALLBACK_WORKING_PC callback;
    u8 quaternion_mode;
    buffer.joint_count = 0;
    buffer.max_joints = globalbuffer->scratch_joint_capacity;
    buffer.joints = (ANI3_JOINT_PC *)(((u32)NuScratchAlloc32((buffer.max_joints + 1) * 48) + 15u) & ~15u);
    buffer.joint_flags = (u8 *)NuScratchAlloc32(buffer.max_joints);
    quaternion_mode = ForceEulerToQuat | ((animation_a->format_flags | animation_b->format_flags) & 1);
    if (AnimQuatDepthPc < 4) {
        AnimQuatSavedPc[AnimQuatDepthPc++] = ForceEulerToQuat;
        ForceEulerToQuat = quaternion_mode;
    } else {
        AnimationDiagnosticNoOpPc();
    }
    buffer.joint_count = object->joint_count;
    adjusted_time_a = time_a - 1.0;
    ANI_SimpleAni3PlayerV4Joint(animation_a, adjusted_time_a, &buffer, object->joint_count, 0);
    buffer.joint_count = object->joint_count;
    adjusted_time_b = time_b - 1.0;
    ANI_SimpleAni3PlayerV4Joint_Blend(animation_b, adjusted_time_b, &buffer, blend, object->joint_count, 0,
                                      &blend_translation);
    callback = (ANI3_OVERRIDE_CALLBACK_WORKING_PC)nuapi_endframe_callbackfn;
    if (override_count != 0 && callback != NULL)
        callback(&buffer, object, override_count, overrides);
    NuAnimBuffEvaluate_3(&buffer, object, matrices, animation_a, root_fn, &root_translation, root_data);
    NuScratchRelease();
    NuScratchRelease();
    if (AnimQuatDepthPc != 0)
        ForceEulerToQuat = AnimQuatSavedPc[--AnimQuatDepthPc];
    else
        AnimationDiagnosticNoOpPc();
}

void __cdecl NuHGobjEvalAnimBlend2(NUHGOBJ_EVAL_PC *object, ANI3_HEADER_PC *animation_a, f32 time_a,
                                   ANI3_HEADER_PC *animation_b, f32 time_b, f32 blend, i32 override_count,
                                   void *overrides, NUMTX *matrices) {
    if ((i32)animation_a->magic < 0x414e4934 || (i32)animation_a->magic > 0x414e4935) {
        AnimationDiagnosticNoOpPc();
        return;
    }
    NuHGobjEvalAnimBlend2Root_3(object, animation_a, time_a, animation_b, time_b, blend, override_count, overrides,
                                matrices, NULL, NULL);
}

DWA_WEIGHT_BLOCK_PC **__cdecl NuHGobjEvalDwa2(i32 count, i16 *indices, NUANIMDATA2_PC *animation, f32 frame) {
    NUANIM_TIME2_PC time;
    DWA_WEIGHT_BLOCK_PC **result;
    i32 render;
    if (animation == NULL || count == 0)
        return NULL;
    NuAnimData2CalcTime(animation, frame, &time);
    if (indices == NULL)
        count = 1;
    result = NuRndrCreateBlendShapeDWAPointers(count);
    memset(result, 0, ((u32)count * 4 + 15u) >> 4);
    if (result == NULL)
        return NULL;
    for (render = 0; render < count; ++render) {
        i32 node = indices == NULL ? 0 : indices[render];
        i32 curve_count, curve;
        DWA_WEIGHT_BLOCK_PC *weights;
        DWA_CURVE_CELL_PC *cells;
        i8 *types;
        if (node < 0)
            continue;
        if (node >= NuAnimNumNodes(animation)) {
            result[render] = NuRndrCreateBlendShapeDeformerWeightsArray(0);
            continue;
        }
        curve_count = animation->curve_count;
        weights = NuRndrCreateBlendShapeDeformerWeightsArray(curve_count);
        result[render] = weights;
        cells = &animation->curves[curve_count * node];
        types = &animation->curve_types[curve_count * node];
        if (weights == NULL)
            continue;
        for (curve = 0; curve < curve_count; ++curve) {
            volatile f32 value =
                types[curve] == 0 ? cells[curve].constant : NuAnimCurve2CalcValEx(&cells[curve], &time, types[curve]);
            weights->values[curve] = value;
        }
    }
    return result;
}

DWA_WEIGHT_BLOCK_PC **__cdecl NuHGobjEvalDwaBlend2(i32 count, i16 *indices, NUANIMDATA2_PC *animation_a, f32 frame_a,
                                                   NUANIMDATA2_PC *animation_b, f32 frame_b, f32 blend) {
    NUANIM_TIME2_PC time_a, time_b;
    DWA_WEIGHT_BLOCK_PC **result;
    i32 render;
    if ((animation_a == NULL && animation_b == NULL) || count == 0)
        return NULL;
    if (animation_a != NULL)
        NuAnimData2CalcTime(animation_a, frame_a, &time_a);
    if (animation_b != NULL)
        NuAnimData2CalcTime(animation_b, frame_b, &time_b);
    if (indices == NULL)
        count = 1;
    result = NuRndrCreateBlendShapeDWAPointers(count);
    memset(result, 0, ((u32)count * 4 + 15u) >> 4);
    if (result == NULL)
        return NULL;
    for (render = 0; render < count; ++render) {
        i32 node = indices == NULL ? 0 : indices[render];
        i32 count_a = 0, count_b = 0, curves, curve;
        DWA_CURVE_CELL_PC *cells_a = NULL, *cells_b = NULL;
        i8 *types_a = NULL, *types_b = NULL;
        DWA_WEIGHT_BLOCK_PC *weights;
        volatile f64 inverse;
        if (node < 0)
            continue;
        if (animation_a != NULL && node < NuAnimNumNodes(animation_a)) {
            count_a = animation_a->curve_count;
            cells_a = &animation_a->curves[count_a * node];
            types_a = &animation_a->curve_types[count_a * node];
        }
        if (animation_b != NULL && node < NuAnimNumNodes(animation_b)) {
            count_b = animation_b->curve_count;
            cells_b = &animation_b->curves[count_b * node];
            types_b = &animation_b->curve_types[count_b * node];
        }
        curves = count_a > count_b ? count_a : count_b;
        weights = NuRndrCreateBlendShapeDeformerWeightsArray(curves);
        result[render] = weights;
        if (weights == NULL || curves <= 0)
            continue;
        inverse = 1.0 - (f64)blend;
        for (curve = 0; curve < curves; ++curve) {
            volatile f32 value_a = 0.0f, value_b = 0.0f;
            if (curve < count_a)
                value_a = types_a[curve] == 0 ? cells_a[curve].constant
                                              : NuAnimCurve2CalcValEx(&cells_a[curve], &time_a, types_a[curve]);
            if (curve < count_b)
                value_b = types_b[curve] == 0 ? cells_b[curve].constant
                                              : NuAnimCurve2CalcValEx(&cells_b[curve], &time_b, types_b[curve]);
            weights->values[curve] = (f64)value_b * blend + (f64)value_a * inverse;
        }
    }
    return result;
}

void __cdecl NuHGobjEvalAnimBlend2Root(NUHGOBJ_EVAL_PC *object, ANI3_HEADER_PC *animation_a, f32 time_a,
                                       ANI3_HEADER_PC *animation_b, f32 time_b, f32 blend, i32 override_count,
                                       void *overrides, NUMTX *matrices, ANI3_ROOT_CALLBACK_PC root_fn,
                                       void *root_data) {
    if ((i32)animation_a->magic < 0x414e4934 || (i32)animation_a->magic > 0x414e4935) {
        AnimationDiagnosticNoOpPc();
        return;
    }
    NuHGobjEvalAnimBlend2Root_3(object, animation_a, time_a, animation_b, time_b, blend, override_count, overrides,
                                matrices, root_fn, root_data);
}

/* Proposal only. Normal finite skeleton/override counts 0..255, readable
 * indexed payloads and hierarchy, writable outputs; optional installed root
 * callback obeys the existing six-word normal engine contract. Legacy duration1
 * leaves time_offset undefined: format4 requires a separately valid offset.
 * Missing-node scales remain undefined; later scale-dependent children require
 * a valid prior parent node or the initialized parent255 sentinel. */
void __cdecl NuHGobjEvalAnim2Root(NUHGOBJ_EVAL_PC *object, ANI3_HEADER_PC *animation, f32 time, i32 override_count,
                                  NUHGOBJ_JOINT_OVERRIDE_PC *overrides, NUMTX *matrices, ANI3_ROOT_CALLBACK_PC root_fn,
                                  void *root_data) {
    NUANIMDATA2_SKELETAL_PC *legacy;
    NUANIM_TIME2_PC anim_time;
    NUVEC scales[256];
    NUHGOBJ_JOINT_OVERRIDE_PC *joint_overrides[256];
    NUMTX local_matrix;
    NUVEC translation, locator_translation;
    u8 i;
    if ((i32)animation->magic >= (i32)0x414e4934 && (i32)animation->magic <= (i32)0x414e4935) {
        NuHGobjEvalAnim2Root_3(object, animation, time, override_count, overrides, matrices, root_fn, root_data);
        return;
    }
    if (object->joint_count > DdMaxJointsPc)
        DdMaxJointsPc = object->joint_count;
    legacy = (NUANIMDATA2_SKELETAL_PC *)animation;
    scales[255].x = scales[255].y = scales[255].z = 1.0f;
    NuAnimData2CalcTime(&legacy->prefix, time, &anim_time);
    if (override_count != 0) {
        memset(joint_overrides, 0, object->joint_count * 4);
        for (i = 0; (i32)i < override_count; ++i) {
            if ((i32)overrides[i].joint_index < object->joint_override_map_count) {
                u8 index = object->joint_override_map[overrides[i].joint_index];
                if (index != 255)
                    joint_overrides[index] = &overrides[i];
            }
        }
    }
    for (i = 0; (i32)i < object->joint_count; ++i) {
        NUMTX *output = &matrices[i];
        NUHGOBJ_JOINT_OVERRIDE_PC *override_anim;
        NUHGOBJ_EVAL_JOINT_PC *joint;
        DWA_CURVE_CELL_PC *curves;
        i8 *types;
        i8 flags;
        if ((i32)i >= legacy->prefix.node_count.legacy) {
            *output = NuMtxSetIdentityTemplate;
            continue;
        }
        override_anim = override_count != 0 ? joint_overrides[i] : NULL;
        curves = &legacy->prefix.curves[i * legacy->prefix.curve_count];
        types = &legacy->prefix.curve_types[i * legacy->prefix.curve_count];
        flags = legacy->node_flags[i];
        joint = &object->joints[i];
        PCBuildLegacySkeletalJoint(curves, types, flags, &anim_time, joint, &scales[i], &scales[joint->parent_index],
                                   &local_matrix, override_anim, i == 0 ? &translation : NULL,
                                   i == 0 ? &locator_translation : NULL);
        if (i == 0 && root_fn != NULL)
            root_fn(&local_matrix, root_data, &translation, NULL, &locator_translation, 0.0f);
        if (joint->parent_index == 255) {
            if (flags & 0x40)
                scales[i].x = scales[i].y = scales[i].z = 1.0f;
            *output = local_matrix;
        } else {
            NuMtxMulVU0(output, &local_matrix, &matrices[joint->parent_index]);
        }
    }
}

void __cdecl NuHGobjEvalAnim2(NUHGOBJ_EVAL_PC *object, ANI3_HEADER_PC *animation, f32 time, i32 override_count,
                              NUHGOBJ_JOINT_OVERRIDE_PC *overrides, NUMTX *matrices) {
    NuHGobjEvalAnim2Root(object, animation, time, override_count, overrides, matrices, NULL, NULL);
}

NUHGOBJ_POI_PC *__cdecl NuHGobjGetPOI(NUHGOBJ_EVAL_PC *object, u8 index) {
    if ((i32)index < object->point_index_count) {
        index = object->point_index_map[index];
        if (index != 0xff)
            return &object->points[index];
    }
    return NULL;
}

typedef struct PC_HG_SHADOW_PACKET PC_HG_SHADOW_PACKET;
typedef struct PC_HG_SHADOW_STATE_VIEW PC_HG_SHADOW_STATE_VIEW;
typedef struct PC_HG_SHADOW_SHAPE48 PC_HG_SHADOW_SHAPE48;
typedef struct PC_HG_SHADOW_SHAPE64 PC_HG_SHADOW_SHAPE64;
typedef struct PC_HG_SHADOW_GROUP PC_HG_SHADOW_GROUP;
typedef struct PC_HG_RENDER_PART PC_HG_RENDER_PART;
typedef struct PC_HG_RENDER_VIEW PC_HG_RENDER_VIEW;
typedef struct PC_HG_SHADOW_PACKET PC_HG_SHADOW_PACKET;
typedef struct PC_HG_SHADOW_STATE_VIEW PC_HG_SHADOW_STATE_VIEW;

struct PC_HG_SHADOW_SHAPE48 {
    union {
        NUVEC axes[4];
        struct {
            f32 center_x;
            u32 flags;
            u8 rest[40];
        } filter;
    };
};
struct PC_HG_SHADOW_SHAPE64 {
    PC_HG_SHADOW_SHAPE48 position;
    u8 unknown_30[16];
};
struct PC_HG_SHADOW_GROUP {
    PC_HG_SHADOW_SHAPE48 *ellipses;
    PC_HG_SHADOW_SHAPE64 *cylinders;
    void *third_shape;
    u8 ellipse_count, cylinder_count, third_count, joint_index;
};
struct PC_HG_RENDER_PART {
    char *name;
    NUSPECIALHANDLE **rigid;
    NUSPECIALHANDLE *smooth;
    NUSPECIALHANDLE **alternate_rigid;
    NUSPECIALHANDLE *alternate_smooth;
};
struct PC_HG_RENDER_VIEW {
    u8 unknown_000[0x168];
    i32 joint_count;
    NUHGOBJ_EVAL_JOINT_PC *joints;
    NUMTX *bind_matrices;
    NUMTX *inverse_bind;
    i32 joint_override_map_count;
    u8 *joint_override_map;
    u8 unknown_180[4];
    NUHGOBJ_POI_PC *points;
    i32 point_index_count;
    u8 *point_index_map;
    i32 render_count;
    PC_HG_RENDER_PART *render_parts;
    u8 unknown_198[4];
    PC_HG_SHADOW_GROUP *shadow_groups;
    u8 suppress_shadow_points;
    u8 unknown_1a1[11];
    NUVEC bounds_min, bounds_max;
};
STATIC_ASSERT(sizeof(PC_HG_SHADOW_SHAPE48) == 48);
STATIC_ASSERT(sizeof(PC_HG_SHADOW_SHAPE64) == 64);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_SHAPE48, filter.flags) == 4);
STATIC_ASSERT(sizeof(PC_HG_SHADOW_GROUP) == 16);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, ellipses) == 0);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, cylinders) == 4);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, third_shape) == 8);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, ellipse_count) == 12);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, cylinder_count) == 13);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, third_count) == 14);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, joint_index) == 15);
STATIC_ASSERT(sizeof(PC_HG_RENDER_PART) == 20);
STATIC_ASSERT(offsetof(PC_HG_RENDER_PART, rigid) == 4);
STATIC_ASSERT(offsetof(PC_HG_RENDER_PART, smooth) == 8);
STATIC_ASSERT(offsetof(PC_HG_RENDER_PART, alternate_rigid) == 12);
STATIC_ASSERT(offsetof(PC_HG_RENDER_PART, alternate_smooth) == 16);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, joint_count) == 0x168);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, joints) == 0x16c);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, bind_matrices) == 0x170);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, inverse_bind) == 0x174);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, joint_override_map_count) == 0x178);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, joint_override_map) == 0x17c);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, points) == 0x184);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, point_index_count) == 0x188);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, point_index_map) == 0x18c);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, render_count) == 0x190);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, render_parts) == 0x194);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, shadow_groups) == 0x19c);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, suppress_shadow_points) == 0x1a0);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, bounds_min) == 0x1ac);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, bounds_max) == 0x1b8);
STATIC_ASSERT(sizeof(PC_HG_SHADOW_PACKET) == 0x68);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, ellipse) == 0x40);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, cylinder) == 0x44);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, third_shape) == 0x48);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, state0) == 0x4c);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, state1) == 0x50);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, state2) == 0x54);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, state_byte) == 0x58);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, near_value) == 0x5c);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, far_value) == 0x60);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, ellipse_count) == 0x64);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, cylinder_count) == 0x65);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, third_count) == 0x66);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, sequence) == 0x67);
STATIC_ASSERT(sizeof(PC_HG_SHADOW_STATE_VIEW) == 16);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_STATE_VIEW, state_byte) == 15);

i32 NuHGobjRndrRandShadowSurfacePoints(NUHGOBJ_EVAL_PC *hierarchy, NUMTX *world, NUMTX *joints, i32 count,
                                       NUVEC *positions, i32 exclusion_mask) {
    i32 i;
    PC_HG_RENDER_VIEW *object;
    PC_HG_SHADOW_GROUP *groups;
    i32 total;
    PC_HG_SHADOW_GROUP *group;
    i16 counts[256];
    i32 index;
    NUMTX combined;
    NUVEC4 point;

    object = ((PC_HG_RENDER_VIEW *)(hierarchy));
    if (PCShadowSurfaceSamplingEnabled == 0 ||
        NuCameraClipTestExtents(&object->bounds_min, &object->bounds_max, world, 0.0f, 0) == 0)
        return 0;
    groups = object->shadow_groups;
    if (groups == NULL || object->suppress_shadow_points != 0)
        return 1;
    total = 0;

    for (group = groups; group->joint_index != 255; ++group) {
        i32 i;

        for (i = 0; i < group->ellipse_count; ++i)
            if ((group->ellipses[i].filter.flags & 1) == 0)
                ++total;
        for (i = 0; i < group->cylinder_count; ++i)
            if ((group->cylinders[i].position.filter.flags & 1) == 0)
                ++total;
    }
    if (total == 0)
        return 1;

    for (i = 0; i < total; ++i)
        counts[i] = 0;
    for (i = 0; i < count; ++i)
        ++counts[((i32)NuWindRand() >> 8) % total];
    index = 0;

    for (group = groups; group->joint_index != 255; ++group) {
        i32 i;
        i32 matrix_ready;

        if (count == 0)
            continue;
        matrix_ready = 0;
        for (i = 0; i < group->ellipse_count; ++i) {
            i32 n;

            if (exclusion_mask != 0 && (group->ellipses[i].filter.flags & exclusion_mask) != 0)
                continue;
            n = counts[index];
            if (n != 0) {
                if (matrix_ready == 0)
                    NuMtxMulVU0(&combined, &joints[group->joint_index], world);
                if (n > 0) {
                    i32 j;

                    count -= n;
                    for (j = n - 1; j >= 0; --j) {
                        NuRndrCalcRandEllipsePos(&point, &combined, group->ellipses[i].axes);
                        positions[count + j].x = point.x;
                        positions[count + j].y = point.y;
                        positions[count + j].z = point.z;
                    }
                }
                matrix_ready = 1;
            }
            ++index;
        }
        for (i = 0; i < group->cylinder_count; ++i) {
            i32 n;

            if (exclusion_mask != 0 && (group->ellipses[i].filter.flags & exclusion_mask) != 0)
                continue;
            n = counts[index];
            if (n != 0) {
                if (matrix_ready == 0)
                    NuMtxMulVU0(&combined, &joints[group->joint_index], world);
                if (n > 0) {
                    i32 j;

                    count -= n;
                    for (j = n - 1; j >= 0; --j) {
                        NuRndrCalcRandCylinderPos(&point, &combined, group->cylinders[i].position.axes);
                        positions[count + j].x = point.x;
                        positions[count + j].y = point.y;
                        positions[count + j].z = point.z;
                    }
                }
                matrix_ready = 1;
            }
            ++index;
        }
    }
    return 1;
}

i32 NuCameraClipHGobj(NUHGOBJ_EVAL_PC *hierarchy, NUMTX *world, NUMTX *root) {
    PC_HG_RENDER_VIEW *object;
    NUVEC half;
    NUVEC min;
    NUMTX matrix;

    object = ((PC_HG_RENDER_VIEW *)(hierarchy));
    if (CutSceneBoundingBoxTrackRoot == 0)
        return NuCameraClipTestExtents(&object->bounds_min, &object->bounds_max, world, 0.0f, 0);

    NuVecSub(&half, &object->bounds_max, &object->bounds_min);
    NuVecScale(&half, &half, 0.5f);
    NuVecNeg(&min, &half);
    matrix = *world;
    NuMtxPreTranslate(&matrix, &root->translation);
    return NuCameraClipTestExtents(&min, &half, &matrix, 0.0f, 0);
}

void NuMtxMulHArrayPc(NUMTX *out, NUMTX *left, NUMTX *right, i32 count) {
    while (count != 0) {
        D3DXMatrixMultiply(((D3DXMATRIX *)(out)), ((D3DXMATRIX *)(left)), ((D3DXMATRIX *)(right)));
        ++out;
        ++left;
        ++right;
        --count;
    }
}

void PCEnqueueHierarchyShadowGroup(NUMTX *matrix, void *ellipse, i32 ellipse_count, void *cylinder, i32 cylinder_count,
                                   void *third, i32 third_count) {
    PC_HG_SHADOW_PACKET *packet;

    if (PCShadowPacketCount >= 280)
        return;
    packet = &PCShadowPackets[PCShadowPacketCount];
    packet->near_value = PCShadowNearValue;
    packet->far_value = PCShadowFarValue;
    packet->state0 = PCShadowState.state0;
    packet->state1 = PCShadowState.state1;
    packet->state2 = PCShadowState.state2;
    packet->state_byte = PCShadowState.state_byte;
    packet->matrix = *matrix;
    packet->ellipse = ellipse;
    packet->ellipse_count = (u8)ellipse_count;
    packet->cylinder = cylinder;
    packet->cylinder_count = (u8)cylinder_count;
    packet->third_shape = third;
    packet->third_count = (u8)third_count;
    packet->sequence = (u8)PCShadowPacketSequence;
    ++PCShadowPacketCount;
    ++PCShadowPacketSequence;
}

void NuHGobjSubmitShadowGroupsPc(NUHGOBJ_EVAL_PC *hierarchy, NUMTX *world, NUMTX *joints) {
    PC_HG_SHADOW_GROUP *group;
    PC_HG_RENDER_VIEW *object;

    object = ((PC_HG_RENDER_VIEW *)(hierarchy));
    if (PCShadowDrawEnabled == 0 || PCShadowDrawMode != 1 || object->shadow_groups == NULL ||
        object->suppress_shadow_points != 0)
        return;
    for (group = object->shadow_groups; group->joint_index != 255; ++group) {
        if (PCShadowPacketCount < 280 && PCShadowSubmissionEnabled != 0) {
            NUMTX combined;

            NuMtxMulVU0(&combined, &joints[group->joint_index], world);
            PCEnqueueHierarchyShadowGroup(&combined, group->ellipses, group->ellipse_count, group->cylinders,
                                          group->cylinder_count, group->third_shape, group->third_count);
            currentScene.clear_flags = 1;
        }
    }
}

i32 NuHGobjRndrMtxDwa(NUHGOBJ_EVAL_PC *hierarchy, NUMTX *world, i32 render_count, i16 *render_indices, NUMTX *joints,
                      DWA_WEIGHT_BLOCK_PC **weights, i32 render_flags) {
    i32 index;
    PC_HG_RENDER_VIEW *object;
    i32 clip;
    i32 shadow;
    i32 old_clip;
    i32 drawn;

    object = ((PC_HG_RENDER_VIEW *)(hierarchy));
    clip = PCSpecialClipState;
    if (clip == -1)
        clip = NuCameraClipHGobj(hierarchy, world, joints);
    shadow = 0;
    if (PCSpecialActiveShadowLights > 0) {
        NUVEC min;
        NUVEC max;

        NuVecMtxTransform(&min, &object->bounds_min, world);
        NuVecMtxTransform(&max, &object->bounds_max, world);
        shadow = NuSpecialClipTestShadowLights(&min, &max, 1);
    }
    if (clip == 0 && shadow == 0) {
        PCSpecialShadowClipResultsPresent = 0;
        return 0;
    }
    old_clip = PCSpecialClipState;
    PCSpecialClipState = clip;
    NuHGobjSubmitShadowGroupsPc(hierarchy, world, joints);
    drawn = 0;
    for (index = 0; index < render_count; ++index) {
        i32 pass;
        i32 layer;
        PC_HG_RENDER_PART *part;
        NUMTX *skin;
        DWA_WEIGHT_BLOCK_PC *blend;
        NUSPECIALHANDLE **lists[2];

        if (render_indices != NULL) {
            layer = render_indices[index];
            if (layer < 0 || layer >= object->render_count)
                continue;
        } else {
            render_count = 1;
            layer = 0;
        }
        part = &object->render_parts[layer];
        skin = NULL;
        if (part->smooth != NULL || part->alternate_smooth != NULL) {
            NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
            NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 127) & ~127u;
            skin = ((NUMTX *)(NuDisplayListStreamCursor->addr));
            NuDisplayListStreamCursor->addr += object->joint_count * sizeof(NUMTX);
        }
        blend = weights != NULL ? weights[index] : NULL;
        lists[0] = part->rigid;
        lists[1] = part->alternate_rigid;
        for (pass = 0; pass < 2; ++pass)
            if (lists[pass] != NULL) {
                i32 joint;

                for (joint = 0; joint < object->joint_count; ++joint)
                    if (lists[pass][joint] != NULL) {
                        NUMTX matrix;

                        NuMtxMulVU0(&matrix, &joints[joint], world);
                        if (lists[pass][joint]->scene != NULL && lists[pass][joint]->display_special != NULL)
                            drawn |= NuDisplayListRndrSpecial(lists[pass][joint], &matrix, 0, NULL, NULL);
                    }
            }
        if (part->smooth != NULL || part->alternate_smooth != NULL) {
            NuMtxMulHArrayPc(skin, object->inverse_bind, joints, object->joint_count);
            if (part->smooth != NULL && part->smooth->scene != NULL && part->smooth->display_special != NULL)
                drawn |= NuDisplayListRndrSpecial(part->smooth, world, 2, skin, blend);
            if (part->alternate_smooth != NULL && part->alternate_smooth->scene != NULL &&
                part->alternate_smooth->display_special != NULL)
                drawn |= NuDisplayListRndrSpecial(part->alternate_smooth, world, 2, skin, blend);
        }
    }
    PCSpecialClipState = old_clip;
    PCSpecialShadowClipResultsPresent = 0;
    (void)render_flags;
    return drawn;
}

void NuSpecialForceMaterials(i32 count, NUMTL **materials) {
    if (count == 0) {
        PCSpecialForcedMaterialTable = NULL;
        nuspecial_draw_state &= ~4;
    } else {
        nuspecial_draw_state |= 4;
        PCSpecialForcedMaterialTable = materials;
        PCSpecialForceMaterialCount = count;
    }
}

void __cdecl NuHGobjEvalAnimBlend(NUHGOBJ_EVAL_PC *object, NUANIM_LEGACY_HEADER_PC *animation_a, f32 frame_a,
                                  NUANIM_LEGACY_HEADER_PC *animation_b, f32 frame_b, f32 blend, i32 override_count,
                                  NUHGOBJ_JOINT_OVERRIDE_PC *overrides, NUMTX *matrices) {
    NUVEC scales[256];
    NUHGOBJ_JOINT_OVERRIDE_PC *joint_overrides[256];
    NUMTX local_matrix;
    NUANIM_TIME2_PC time_a, time_b;
    NUANIM_LEGACY_CHUNK_PC *chunk_a, *chunk_b;
    u8 index;
    if (object->joint_count > DdMaxJointsPc)
        DdMaxJointsPc = object->joint_count;
    scales[255].x = scales[255].y = scales[255].z = 1.0f;
    NuAnimDataCalcTime(animation_a, frame_a, &time_a);
    chunk_a = (NUANIM_LEGACY_CHUNK_PC *)animation_a->chunks[time_a.chunk];
    NuAnimDataCalcTime(animation_b, frame_b, &time_b);
    chunk_b = (NUANIM_LEGACY_CHUNK_PC *)animation_b->chunks[time_b.chunk];
    if (override_count != 0) {
        memset(joint_overrides, 0, object->joint_count * sizeof(joint_overrides[0]));
        for (index = 0; index < override_count; ++index) {
            u8 override_index = overrides[index].joint_index;
            if (override_index < object->joint_override_map_count) {
                u8 joint_index = object->joint_override_map[override_index];
                if (joint_index != 0xff)
                    joint_overrides[joint_index] = &overrides[index];
            }
        }
    }
    for (index = 0; index < object->joint_count; ++index) {
        NUMTX *output = &matrices[index];
        NUHGOBJ_JOINT_OVERRIDE_PC *joint_override = override_count != 0 ? joint_overrides[index] : NULL;
        NUANIM_LEGACY_SET_PC *first = chunk_a->curve_sets[index];
        NUANIM_LEGACY_SET_PC *second = chunk_b->curve_sets[index];
        NUMTX *local = &local_matrix;
        u8 parent_index;
        if (first != NULL && second != NULL) {
            NUHGOBJ_EVAL_JOINT_PC *joint = &object->joints[index];
            NuAnimCurveSetApplyBlendToJoint2(first, &time_a, second, &time_b, blend, joint, &scales[index],
                                             &scales[joint->parent_index], &local_matrix, joint_override);
        } else {
            local = &object->bind_matrices[index];
        }
        parent_index = object->joints[index].parent_index;
        if (parent_index == 0xff)
            *output = *local;
        else
            NuMtxMulVU0(output, local, &matrices[parent_index]);
    }
}

DWA_WEIGHT_BLOCK_PC **__cdecl NuHGobjEvalDwa(i32 render_count, i16 *render_indices, NUANIM_LEGACY_HEADER_PC *animation,
                                             f32 frame) {
    NUANIM_TIME2_PC time;
    DWA_WEIGHT_BLOCK_PC **weights_by_render;
    i32 render;
    if (animation == NULL || render_count == 0)
        return NULL;
    NuAnimDataCalcTime(animation, frame, &time);
    if (render_indices == NULL)
        render_count = 1;
    weights_by_render = NuRndrCreateBlendShapeDWAPointers(render_count);
    memset(weights_by_render, 0, ((u32)render_count * sizeof(void *) + 15) >> 4);
    if (weights_by_render == NULL)
        return NULL;
    for (render = 0; render < render_count; ++render) {
        i32 node = render_indices == NULL ? 0 : render_indices[render];
        NUANIM_LEGACY_CHUNK_PC *chunk;
        NUANIM_LEGACY_SET_PC *set;
        DWA_WEIGHT_BLOCK_PC *weights;
        i32 curve_count, curve;
        if (node < 0)
            continue;
        chunk = (NUANIM_LEGACY_CHUNK_PC *)animation->chunks[time.chunk];
        if (render_indices != NULL && node >= chunk->node_count) {
            weights_by_render[render] = NuRndrCreateBlendShapeDeformerWeightsArray(0);
            continue;
        }
        set = chunk->curve_sets[node];
        if (set == NULL) {
            weights_by_render[render] = NuRndrCreateBlendShapeDeformerWeightsArray(0);
            continue;
        }
        curve_count = set->curve_count;
        weights = NuRndrCreateBlendShapeDeformerWeightsArray(curve_count);
        weights_by_render[render] = weights;
        if (weights == NULL || curve_count <= 0)
            continue;
        for (curve = 0; curve < curve_count; ++curve) {
            NUANIM_LEGACY_SET_PC *curve_set =
                ((NUANIM_LEGACY_CHUNK_PC *)animation->chunks[time.chunk])->curve_sets[node];
            weights->values[curve] = curve_set->curves[curve] != NULL
                                         ? NuAnimCurveCalcVal2(curve_set->curves[curve], &time)
                                         : curve_set->constants[curve];
        }
    }
    return weights_by_render;
}

DWA_WEIGHT_BLOCK_PC **__cdecl NuHGobjEvalDwaBlend(i32 render_count, i16 *render_indices,
                                                  NUANIM_LEGACY_HEADER_PC *animation_a, f32 frame_a,
                                                  NUANIM_LEGACY_HEADER_PC *animation_b, f32 frame_b, f32 blend) {
    NUANIM_TIME2_PC time_a, time_b;
    DWA_WEIGHT_BLOCK_PC **weights_by_render;
    i32 render;
    if ((animation_a == NULL && animation_b == NULL) || render_count == 0)
        return NULL;
    if (animation_a != NULL)
        NuAnimDataCalcTime(animation_a, frame_a, &time_a);
    if (animation_b != NULL)
        NuAnimDataCalcTime(animation_b, frame_b, &time_b);
    if (render_indices == NULL)
        render_count = 1;
    weights_by_render = NuRndrCreateBlendShapeDWAPointers(render_count);
    memset(weights_by_render, 0, ((u32)render_count * sizeof(void *) + 15) >> 4);
    if (weights_by_render == NULL)
        return NULL;
    for (render = 0; render < render_count; ++render) {
        i32 node = render_indices == NULL ? 0 : render_indices[render];
        i32 count_a = 0, count_b = 0, curve_count, curve;
        DWA_WEIGHT_BLOCK_PC *weights;
        f64 inverse_blend;
        if (node < 0)
            continue;
        if (animation_a != NULL) {
            NUANIM_LEGACY_CHUNK_PC *chunk = (NUANIM_LEGACY_CHUNK_PC *)animation_a->chunks[time_a.chunk];
            if (node < chunk->node_count && chunk->curve_sets[node] != NULL)
                count_a = chunk->curve_sets[node]->curve_count;
        }
        if (animation_b != NULL) {
            NUANIM_LEGACY_CHUNK_PC *chunk = (NUANIM_LEGACY_CHUNK_PC *)animation_b->chunks[time_b.chunk];
            if (node < chunk->node_count && chunk->curve_sets[node] != NULL)
                count_b = chunk->curve_sets[node]->curve_count;
        }
        curve_count = count_a < count_b ? count_b : count_a;
        weights = NuRndrCreateBlendShapeDeformerWeightsArray(curve_count);
        weights_by_render[render] = weights;
        if (weights == NULL || curve_count <= 0)
            continue;
        inverse_blend = 1.0 - blend;
        for (curve = 0; curve < curve_count; ++curve) {
            f32 value_a = 0.0f;
            f64 value_b = 0.0;
            if (curve < count_a) {
                NUANIM_LEGACY_SET_PC *set =
                    ((NUANIM_LEGACY_CHUNK_PC *)animation_a->chunks[time_a.chunk])->curve_sets[node];
                value_a = set->curves[curve] != NULL ? NuAnimCurveCalcVal2(set->curves[curve], &time_a)
                                                     : set->constants[curve];
            }
            if (curve < count_b) {
                NUANIM_LEGACY_SET_PC *set =
                    ((NUANIM_LEGACY_CHUNK_PC *)animation_b->chunks[time_b.chunk])->curve_sets[node];
                value_b = set->curves[curve] != NULL ? NuAnimCurveCalcVal2(set->curves[curve], &time_b)
                                                     : set->constants[curve];
            }
            weights->values[curve] = value_b * blend + inverse_blend * value_a;
        }
    }
    return weights_by_render;
}
