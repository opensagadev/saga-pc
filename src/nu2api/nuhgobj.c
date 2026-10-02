#include "saga.h"

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
