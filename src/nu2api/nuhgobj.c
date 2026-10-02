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
