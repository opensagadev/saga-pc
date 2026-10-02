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
