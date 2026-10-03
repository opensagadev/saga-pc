#include "saga.h"

NUVEC __cdecl NuFadeObjGetAngleTerrainValues(NUVEC *position) {
    NUVEC values;
    values.y = NewShadow(position, 0.0f, 5.0f, 0);
    values.x = (f32)NuAtan2D(ShadNorm.z, ShadNorm.y);
    values.z = (f32)-NuAtan2D(ShadNorm.x, NuFsqrt(ShadNorm.y * ShadNorm.y + ShadNorm.z * ShadNorm.z));
    return values;
}

void __cdecl NuFadeObjInit(void) {
    i32 group, i;
    i16 *platform;
    void *terrain;
    NuFadeObjDir = 0;
    NuFadeObjDir2 = 0;
    NuFadeObjWave = 0;
    NuFadeObjDataIndex = 0;
    terrain = TerrainGetCur();
    for (group = 0; group < NuFadeMaxGroupsPC; ++group) {
        if (NuFadeObjGroup[group].active) {
            TerrainSetCur(NuFadeObjGroup[group].terrain);
            platform = NuFadeObjGroup[group].platforms;
            for (i = 0; i < NuFadeObjGroup[group].count; ++i, ++platform) {
                if (*platform != -1)
                    DeletePlatinst(*platform);
            }
            NuFadeObjGroup[group].active = 0;
        }
    }
    TerrainSetCur(terrain);
}

PC_NUFADEGROUP *__cdecl NuFadeObjAllocateGrp(void) {
    i32 index;
    for (index = 0; index < NuFadeMaxGroupsPC; ++index) {
        if (!NuFadeObjGroup[index].active) {
            NuFadeObjGroup[index].active = 1;
            return &NuFadeObjGroup[index];
        }
    }
    return NULL;
}

PC_NUFADEGROUP *__cdecl NuFadeObjCreateMtx(NUSPECIALHANDLE *source, NUMTX *matrices, i16 count, f32 near_distance,
                                           f32 far_distance, i32 flags) {
    PC_NUFADEGROUP *group;
    NUDISPLAY_SPECIAL_PC *special;
    NUMTX *matrix = matrices;
    i16 *platform = NULL;
    i32 i;
    f32 min_x, max_x, min_y, max_y, min_z, max_z;
    group = NuFadeObjAllocateGrp();
    if (NuFadeObjDataIndex + count < NuFadeMaxDataPC) {
        platform = &NuFadeData[NuFadeObjDataIndex];
        NuFadeObjDataIndex += count;
    }
    if (group != NULL && matrices != NULL && platform != NULL) {
        group->matrices = matrices;
        group->platforms = platform;
        group->scene = (NUGSCN *)source->scene;
        special = (NUDISPLAY_SPECIAL_PC *)source->display_special;
        group->special = special;
        group->instance_index = (i16)special->instance_ix;
        group->terrain = TerrainGetCur();
        group->count = count;
        group->flags = (i16)flags;
        group->near_distance = near_distance;
        group->far_distance = far_distance;
        group->cull_distance_squared = far_distance * far_distance;
        group->fade_range = group->far_distance - group->near_distance;
        min_x = min_y = min_z = 10000000.0f;
        max_x = max_y = max_z = -10000000.0f;
        for (i = 0; i < count; ++i, ++matrix, ++platform) {
            *platform = -1;
            if (matrix->m30 < min_x)
                min_x = matrix->m30;
            if (matrix->m31 < min_y)
                min_y = matrix->m31;
            if (matrix->m32 < min_z)
                min_z = matrix->m32;
            if (matrix->m30 > max_x)
                max_x = matrix->m30;
            if (matrix->m31 > max_y && matrix->m31 < 2000000.0)
                max_y = matrix->m31;
            if (matrix->m32 > max_z)
                max_z = matrix->m32;
        }
        group->centre.x = (max_x + min_x) * 0.5;
        group->centre.y = (max_y + min_y) * 0.5;
        group->centre.z = (max_z + min_z) * 0.5;
        group->radius_squared = ((max_x - min_x) * 0.5) * ((max_x - min_x) * 0.5) +
                                ((max_y - min_y) * 0.5) * ((max_y - min_y) * 0.5) +
                                ((max_z - min_z) * 0.5) * ((max_z - min_z) * 0.5) + 1.0;
        group->cull_distance_squared += group->radius_squared;
        return group;
    }
    group->active = 0;
    return NULL;
}
