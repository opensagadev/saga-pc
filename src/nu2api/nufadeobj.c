#include "saga.h"

void __cdecl NuFadeObjSetup(VARIPTR *buffer, VARIPTR end, i32 matrix_count, i32 group_count) {
    buffer->addr = (buffer->addr + 15) & ~15u;
    NuFadeObjGroup = (PC_NUFADEGROUP *)buffer->void_ptr;
    NuFadeMaxGroupsPC = group_count;
    buffer->addr += group_count * sizeof(PC_NUFADEGROUP);
    buffer->addr = (buffer->addr + 15) & ~15u;
    NuFadeMaxDataPC = matrix_count;
    NuFadeData = (i16 *)buffer->void_ptr;
    buffer->addr += matrix_count * sizeof(i16);
    (void)end;
}

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
    NUMTX *matrix = matrices;
    i16 *platform;
    i32 i;
    f32 min_x, max_x, min_y, max_y, min_z, max_z;
    group = NuFadeObjAllocateGrp();
    if (NuFadeObjDataIndex + count < NuFadeMaxDataPC) {
        NuFadeObjDataIndex += count;
        platform = &NuFadeData[NuFadeObjDataIndex - count];
    } else {
        platform = NULL;
    }
    if (group != NULL && matrices != NULL && platform != NULL) {
        group->matrices = matrices;
        group->platforms = platform;
        group->scene = (NUGSCN *)source->scene;
        group->special = (NUDISPLAY_SPECIAL_PC *)source->display_special;
        group->instance_index = (i16)((NUDISPLAY_SPECIAL_PC *)source->display_special)->instance_ix;
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

void __cdecl NuFadeObjAngleTerrain(NUMTX *matrix) {
    NUVEC4 position;
    NUVEC4 angles;
    matrix->m31 = NewShadow(&matrix->translation, 0.0f, 5.0f, 0);
    angles.x = (f32)NuAtan2D(ShadNorm.z, ShadNorm.y);
    angles.z = (f32)-NuAtan2D(ShadNorm.x, NuFsqrt(ShadNorm.y * ShadNorm.y + ShadNorm.z * ShadNorm.z));
    position.x = matrix->m30;
    position.y = matrix->m31;
    position.z = matrix->m32;
    matrix->m30 = matrix->m31 = matrix->m32 = 0.0f;
    NuMtxRotateZ(matrix, (NUANG)angles.z);
    NuMtxRotateX(matrix, (NUANG)angles.x);
    matrix->m30 = position.x;
    matrix->m31 = position.y;
    matrix->m32 = position.z;
}

void __cdecl NuFadeObjUpdateArray(NUVEC **positions) {
    i32 i, g, p, nearby;
    NUMTX *matrix;
    i16 *platform;
    PC_NUFADEGROUP *group = NuFadeObjGroup;
    void *terrain;
    NuFadeObjWave += 501;
    NuFadeObjDir += 133;
    NuFadeObjDir2 += 377;
    if (NuFadeObjEnabledPc == 0)
        return;
    terrain = TerrainGetCur();
    for (g = 0; g < NuFadeMaxGroupsPC; ++g, ++group) {
        if (group->active) {
            if ((group->centre.x - global_camera.mtx.m30) * (group->centre.x - global_camera.mtx.m30) +
                    (group->centre.y - global_camera.mtx.m31) * (group->centre.y - global_camera.mtx.m31) +
                    (group->centre.z - global_camera.mtx.m32) * (group->centre.z - global_camera.mtx.m32) <
                group->cull_distance_squared) {
                group->in_range = 1;
                if (group->flags) {
                    TerrainSetCur(group->terrain);
                    matrix = group->matrices;
                    platform = group->platforms;
                    for (i = 0; i < group->count; ++i, ++matrix, ++platform) {
                        nearby = 0;
                        for (p = 0; p < 8; ++p) {
                            if (positions[p]) {
                                if ((matrix->m30 - positions[p]->x) * (matrix->m30 - positions[p]->x) +
                                        (matrix->m31 - positions[p]->y) * (matrix->m31 - positions[p]->y) +
                                        (matrix->m32 - positions[p]->z) * (matrix->m32 - positions[p]->z) <
                                    4.0) {
                                    nearby = 1;
                                    break;
                                }
                            }
                        }
                        if (nearby) {
                            if (*platform == -1) {
                                *platform = (i16)NewPlatInst(matrix, group->instance_index);
                                PlatInstRotate(*platform, 1);
                            }
                        } else if (*platform != -1) {
                            DeletePlatinst(*platform);
                            *platform = -1;
                        }
                    }
                }
            } else {
                group->in_range = 0;
            }
        }
    }
    TerrainSetCur(terrain);
}

void __cdecl NuFadeObjUpdate(NUVEC *position) {
    i32 i;
    NUVEC *positions[8];
    for (i = 1; i < 8; ++i)
        positions[i] = NULL;
    positions[0] = position;
    NuFadeObjUpdateArray(positions);
}

void __cdecl NuFadeObjDraw(void) {
    i32 i, g;
    NUMTX *matrix;
    PC_NUFADEGROUP *group;
    f32 near_squared, far_squared, distance_squared;
    NUSPECIALHANDLE special;
    special.special = NULL;
    if (NuFadeObjEnabledPc == 0)
        return;
    group = NuFadeObjGroup;
    for (g = 0; g < NuFadeMaxGroupsPC; ++g, ++group) {
        if (group->active && group->in_range) {
            NuFadeObjCurGrp = group;
            group->drawn = 0;
            matrix = group->matrices;
            near_squared = group->near_distance * group->near_distance;
            far_squared = group->far_distance * group->far_distance;
            if (matrix == NULL)
                continue;
            for (i = 0; i < group->count; ++i, ++matrix) {
                distance_squared = (matrix->m30 - global_camera.mtx.m30) * (matrix->m30 - global_camera.mtx.m30) +
                                   (matrix->m31 - global_camera.mtx.m31) * (matrix->m31 - global_camera.mtx.m31) +
                                   (matrix->m32 - global_camera.mtx.m32) * (matrix->m32 - global_camera.mtx.m32);
                if (distance_squared < far_squared) {
                    if (distance_squared < near_squared)
                        matrix->m23 = 0.0f;
                    else
                        matrix->m23 = 1.0e-11f;
                    special.scene = group->scene;
                    special.display_special = (NUMTX *)group->special;
                    if (NuSpecialDrawAt(&special, matrix))
                        group->drawn = 1;
                }
            }
        }
    }
}
