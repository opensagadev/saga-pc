#include "saga.h"

extern i32 NuWindMaxMatsPC;

void __cdecl NuWindSetup(VARIPTR *buffer, VARIPTR end, i32 matrix_count, i32 group_count) {
    NuWindMaxMatsPC = matrix_count;
    NuWindMaxGroupsPC = group_count;
    buffer->addr = (buffer->addr + 15) & ~15u;
    NuWindGroup = (PC_NUWINDGROUP *)buffer->void_ptr;
    buffer->addr += group_count * sizeof(PC_NUWINDGROUP);
    (void)end;
}

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

void __cdecl NuWindUpdateArray(NUVEC **positions) {
    i32 instance, index, object, any_interaction, closest;
    f32 scale, contact_height, contact_radius, contact_x, contact_z;
    NUMTX *matrix;
    PC_NUWINDGROUP *group;
    f32 closest_squared, squared, target_x, target_z;
    NUVEC wind;
    i32 nearby[8];
    group = NuWindGroup;
    NuWindWave += 501;
    NuWindDir2 += 133;
    NuWindDir += 377;
    wind.x = NU_SIN_LUT((i32)(16384.0f + NU_SIN_LUT(NuWindDir2) * 8192.0f)) * 0.75f +
             NU_SIN_LUT((i32)((u32)NuWindDir + 0x4000)) * 0.15f;
    wind.z = NU_SIN_LUT((i32)(NU_SIN_LUT(NuWindDir2) * 8192.0f)) * 0.75f - NU_SIN_LUT(NuWindDir) * 0.15f;
    any_interaction = 0;
    for (index = 0; index < NuWindMaxGroupsPC; ++index, ++group) {
        if (group->in_use != 0) {
            if ((group->center.x - global_camera.mtx.m30) * (group->center.x - global_camera.mtx.m30) +
                    (group->center.y - global_camera.mtx.m31) * (group->center.y - global_camera.mtx.m31) +
                    (group->center.z - global_camera.mtx.m32) * (group->center.z - global_camera.mtx.m32) <
                group->extended_radius_squared) {
                group->visible = 1;
                if (group->drawn != 0 && group->value20 > 0.0f) {
                    matrix = group->matrices;
                    if (group->flags != 0) {
                        for (object = 0; object < 8; ++object) {
                            if (positions[object] == NULL) {
                                nearby[object] = 0;
                            } else if ((group->center.x - positions[object]->x) *
                                               (group->center.x - positions[object]->x) +
                                           (group->center.y - positions[object]->y) *
                                               (group->center.y - positions[object]->y) +
                                           (group->center.z - positions[object]->z) *
                                               (group->center.z - positions[object]->z) <
                                       group->radius_squared) {
                                nearby[object] = 1;
                                any_interaction = 1;
                            } else {
                                nearby[object] = 0;
                            }
                        }
                    }
                    if (group->flags == 0 || any_interaction == 0) {
                        for (instance = 0; instance < group->matrix_count; ++instance, ++matrix) {
                            target_x =
                                matrix->m33 * wind.x *
                                (NU_SIN_LUT((i32)((f32)NuWindWave + (matrix->m30 + matrix->m32) * 8192.0f)) * 0.5f +
                                 1.0f);
                            target_z = matrix->m33 * wind.z *
                                       (NU_SIN_LUT((i32)(16384.0f +
                                                         ((f32)NuWindWave + (matrix->m30 + matrix->m32) * 8192.0f))) *
                                            0.5f +
                                        1.0f);
                            matrix->m10 += (target_x - matrix->m10) * 0.2f;
                            matrix->m12 += (target_z - matrix->m12) * 0.2f;
                            scale = 1.0f - NuFsqrt(matrix->m10 * matrix->m10 + matrix->m12 * matrix->m12) * 0.35f;
                            if (scale < 0.05f)
                                scale = 0.05f;
                            matrix->m10 *= scale;
                            matrix->m12 *= scale;
                            matrix->m23 = 1.0f / scale;
                            if (scale < 0.19f)
                                scale = 0.19f;
                            matrix->m11 = (matrix->m33 / group->value20) * scale;
                        }
                    } else {
                        for (instance = 0; instance < group->matrix_count; ++instance, ++matrix) {
                            closest_squared = 1000000.0f;
                            closest = -1;
                            for (object = 0; object < 8; ++object) {
                                if (positions[object] != NULL) {
                                    squared =
                                        (matrix->m30 - positions[object]->x) * (matrix->m30 - positions[object]->x) +
                                        (matrix->m32 - positions[object]->z) * (matrix->m32 - positions[object]->z);
                                    if (squared < closest_squared) {
                                        closest_squared = squared;
                                        closest = object;
                                    }
                                }
                            }
                            if (closest == -1)
                                break;
                            if (positions[closest]->y - matrix->m31 > (group->value24 * matrix->m33) / group->value20) {
                                contact_radius = 0.0f;
                                contact_height = (matrix->m33 * 0.2f) / group->value20;
                            } else {
                                contact_radius = 0.45f;
                                if (0.0f > positions[closest]->y - matrix->m31) {
                                    contact_height = (matrix->m33 * 0.2f) / group->value20;
                                } else {
                                    contact_height =
                                        (((positions[closest]->y - matrix->m31) * 0.5f + 0.2f) * matrix->m33) /
                                        group->value20;
                                }
                            }
                            matrix->m10 *= matrix->m23;
                            matrix->m12 *= matrix->m23;
                            contact_x = (matrix->m10 * contact_height + matrix->m30) - positions[closest]->x;
                            contact_z = (matrix->m12 * contact_height + matrix->m32) - positions[closest]->z;
                            scale = contact_x * contact_x + contact_z * contact_z;
                            if (scale >= contact_radius * contact_radius) {
                                target_x =
                                    matrix->m33 * wind.x *
                                    (NU_SIN_LUT((i32)((f32)NuWindWave + (matrix->m30 + matrix->m32) * 8192.0f)) * 0.5f +
                                     1.0f);
                                target_z = matrix->m33 * wind.z *
                                           (NU_SIN_LUT((i32)(16384.0f + ((f32)NuWindWave +
                                                                         (matrix->m30 + matrix->m32) * 8192.0f))) *
                                                0.5f +
                                            1.0f);
                                contact_x = 0.2f * (target_x - matrix->m10);
                                contact_z = 0.2f * (target_z - matrix->m12);
                                scale = contact_x * contact_x + contact_z * contact_z;
                                if (scale > 0.000144f) {
                                    scale = 0.012f / NuFsqrt(scale);
                                    contact_x *= scale;
                                    contact_z *= scale;
                                }
                                matrix->m10 += contact_x;
                                matrix->m12 += contact_z;
                            } else {
                                if (scale == 0.0f) {
                                    contact_x = contact_radius;
                                    contact_z = 0.0f;
                                } else {
                                    scale = contact_radius / NuFsqrt(scale);
                                    contact_x *= scale;
                                    contact_z *= scale;
                                }
                                contact_x =
                                    (0.3f * ((positions[closest]->x + contact_x) - matrix->m30)) / contact_height;
                                contact_z =
                                    (0.3f * ((positions[closest]->z + contact_z) - matrix->m32)) / contact_height;
                                scale = contact_x * contact_x + contact_z * contact_z;
                                if (scale > 0.25f) {
                                    scale = 0.5f / NuFsqrt(scale);
                                    contact_x *= scale;
                                    contact_z *= scale;
                                }
                                contact_x = (contact_x - matrix->m10) * group->field_48;
                                contact_z = (contact_z - matrix->m12) * group->field_48;
                                scale = contact_x * contact_x + contact_z * contact_z;
                                if (group->field_44 < scale) {
                                    scale = group->field_40 / NuFsqrt(scale);
                                    contact_x *= scale;
                                    contact_z *= scale;
                                }
                                matrix->m10 += contact_x;
                                matrix->m12 += contact_z;
                                target_x =
                                    matrix->m33 * wind.x *
                                    (1.0f +
                                     (NU_SIN_LUT((i32)((f32)NuWindWave + (matrix->m30 + matrix->m32) * 8192.0f)) *
                                      0.5f));
                                target_z = matrix->m33 * wind.z *
                                           (NU_SIN_LUT((i32)(16384.0f + ((f32)NuWindWave +
                                                                         (matrix->m30 + matrix->m32) * 8192.0f))) *
                                                0.5f +
                                            1.0f);
                                matrix->m10 += (target_x - matrix->m10) * 0.05f;
                                matrix->m12 += (target_z - matrix->m12) * 0.05f;
                            }
                            scale = 1.0f - NuFsqrt(matrix->m10 * matrix->m10 + matrix->m12 * matrix->m12) * 0.35f;
                            if (scale < 0.05f)
                                scale = 0.05f;
                            matrix->m10 *= scale;
                            matrix->m12 *= scale;
                            matrix->m23 = 1.0f / scale;
                            if (scale < 0.19f)
                                scale = 0.19f;
                            matrix->m11 = (matrix->m33 / group->value20) * scale;
                        }
                    }
                }
            } else {
                group->visible = 0;
            }
        }
    }
}

void __cdecl NuWindUpdate(NUVEC *wind) {
    NUVEC *winds[8];
    i32 index;
    for (index = 1; index <= 7; ++index) {
        winds[index] = NULL;
    }
    winds[0] = wind;
    NuWindUpdateArray(winds);
}

void __cdecl NuWindDraw(void) {
    i32 instance, index;
    NUMTX *matrix;
    PC_NUWINDGROUP *group;
    f32 near_squared, far_squared, saved_m23, saved_m33, distance_squared;
    NUSPECIALHANDLE special;
    special.special = NULL;
    group = NuWindGroup;
    for (index = 0; index < NuWindMaxGroupsPC; ++index, ++group) {
        if (group->in_use != 0 && group->visible != 0) {
            NuWindCurGrp = group;
            group->drawn = 0;
            matrix = group->matrices;
            near_squared = group->near_distance * group->near_distance;
            far_squared = group->far_distance * group->far_distance;
            for (instance = 0; instance < group->matrix_count; ++instance, ++matrix) {
                saved_m23 = matrix->m23;
                saved_m33 = matrix->m33;
                matrix->m33 = 1.0f;
                distance_squared = (matrix->m30 - global_camera.mtx.m30) * (matrix->m30 - global_camera.mtx.m30) +
                                   (matrix->m31 - global_camera.mtx.m31) * (matrix->m31 - global_camera.mtx.m31) +
                                   (matrix->m32 - global_camera.mtx.m32) * (matrix->m32 - global_camera.mtx.m32);
                if (distance_squared < far_squared) {
                    if (near_squared > distance_squared) {
                        matrix->m23 = 0.0f;
                    } else {
                        matrix->m23 = 1.0e-11f;
                    }
                    special.scene = group->scene;
                    special.display_special = (NUMTX *)group->special;
                    if (NuSpecialDrawAt(&special, matrix) != 0)
                        group->drawn = 1;
                }
                matrix->m23 = saved_m23;
                matrix->m33 = saved_m33;
            }
        }
    }
}
