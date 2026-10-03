#include "saga.h"

void __cdecl NuWindInit(void) {
    i32 index;
    NuWindWave = 0;
    NuWindDir = 0;
    NuWindDir2 = 0;
    for (index = 0; index < NuWindMaxGroupsPC; ++index)
        NuWindGroup[index].in_use = 0;
}

PC_NUWINDGROUP *__cdecl NuWindAllocateGrp(void) {
    i32 index;
    for (index = 0; index < NuWindMaxGroupsPC; ++index) {
        if (NuWindGroup[index].in_use == 0) {
            NuWindGroup[index].in_use = 1;
            return &NuWindGroup[index];
        }
    }
    return NULL;
}

PC_NUWINDGROUP *NuWindCreateMtx(NUSPECIALHANDLE *source, NUMTX *matrices, i16 count, f32 value20, f32 value24,
                                i32 flags, f32 near_distance, f32 far_distance) {
    PC_NUWINDGROUP *group = NuWindAllocateGrp();
    NUMTX *matrix = matrices;
    i32 index;
    f32 min_x, min_y, min_z, max_x, max_y, max_z;
    if (group == NULL || matrix == NULL) {
        if (group != NULL)
            group->in_use = 0;
        return NULL;
    }
    group->matrices = matrices;
    group->scene = (NUGSCN *)source->scene;
    group->special = (NUDISPLAY_SPECIAL_PC *)source->display_special;
    group->matrix_count = count;
    group->value20 = value20;
    group->value24 = value24;
    group->flags = (u16)flags;
    group->near_distance = near_distance;
    group->far_distance = far_distance;
    group->extended_radius_squared = far_distance * far_distance;
    group->distance_range = group->far_distance - group->near_distance;
    group->field_48 = 0.2f;
    group->field_40 = 0.025f;
    group->field_44 = 0.0006250000442378223f;

    min_x = min_y = min_z = 10000000.0f;
    max_x = max_y = max_z = -10000000.0f;
    for (index = 0; index < count; ++index, ++matrix) {
        if (min_x > matrix->m30)
            min_x = matrix->m30;
        if (min_y > matrix->m31)
            min_y = matrix->m31;
        if (min_z > matrix->m32)
            min_z = matrix->m32;
        if (matrix->m30 > max_x)
            max_x = matrix->m30;
        if (matrix->m31 > max_y && matrix->m31 < 2000000.0)
            max_y = matrix->m31;
        if (matrix->m32 > max_z)
            max_z = matrix->m32;
    }
    group->center.x = 0.5 * (max_x + min_x);
    group->center.y = 0.5 * (max_y + min_y);
    group->center.z = 0.5 * (max_z + min_z);
    group->radius_squared =
        1.5 + ((0.5 * (max_x - min_x)) * ((max_x - min_x) * 0.5) + ((max_y - min_y) * 0.5) * (0.5 * (max_y - min_y)) +
               ((max_z - min_z) * 0.5) * (0.5 * (max_z - min_z)));
    group->extended_radius_squared += group->radius_squared;
    group->radius = NuFsqrt(group->radius_squared);
    return group;
}
