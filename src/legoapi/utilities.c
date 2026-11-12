#include "saga.h"

/* Retail unit 0x54f960-0x553700 (legoapi/utilities.c): definitions in retail address order. */
#include <stdlib.h>

extern char **TexAnimScriptTablePc;
void __cdecl NuTexAnimScriptMissingPc(char *path);
#define SPLINE_POINT(spline, index) ((NUVEC *)((u8 *)(spline)->pts + (index) * (i16)(spline)->pt_size))
extern const f64 GameObjectNearFloorNoFloorHeight;
extern i32 VehicleArea;
extern u16 temp_xrot;
extern u16 temp_zrot;
/* Source-private in gameobjects.c. Forward pointer uses struct tag in saga.h.
 * Tail[1] exposes actual counted traversal; it is NOT owner/allocation count.
 */
typedef struct shadow_platform_entry_pc_s {
    GAMEOBJECT *object;
    u8 unknown_04_14[0x10];
} SHADOW_PLATFORM_ENTRY_PC;
struct shadow_platform_list_pc_s {
    u8 unknown_00_04[4];
    i32 count;
    u8 unknown_08_18[0x10];
    SHADOW_PLATFORM_ENTRY_PC entries[1];
};
i32 __cdecl NuSpecialDrawAtAlpha(NUSPECIALHANDLE *, NUMTX *, f32);
extern CUT_DRAW_SETTINGS_PC *CutSceneSys;
extern i32 dco_id, dco_wearinghat, dco_reflectaxis;
extern PLAYERCHARACTERCONFIG_PC *dco_gcdata;
extern CHARACTER_MODEL_LAYERS_PC *dco_cmodel;
extern f32 dco_reflectcoord;
extern i32 dco_locatorposonly;
extern u16 dco_prerotatez;
extern void(__cdecl *DisguiseAdjustFn)(i32, i32, NUVEC *, NUVEC *);
typedef struct USEFUL_MATERIAL_PC {
    u8 unknown_00[0x40];
    u32 attributes;
} USEFUL_MATERIAL_PC;
extern NUMTL *SolidMtl;
extern i32(__cdecl *MatrixReflection_CanOverrideFn)(void);

static __inline f32 UtilitiesFabsPc(f32 value);

static __declspec(noinline) void ReflectionCopyMatrixPc(NUMTX *result, NUMTX *source) {
    result->m00 = source->m00;
    result->m01 = source->m01;
    result->m02 = source->m02;
    result->m03 = source->m03;
    result->m10 = source->m10;
    result->m11 = source->m11;
    result->m12 = source->m12;
    result->m13 = source->m13;
    result->m20 = source->m20;
    result->m21 = source->m21;
    result->m22 = source->m22;
    result->m23 = source->m23;
    result->m30 = source->m30;
    result->m31 = source->m31;
    result->m32 = source->m32;
    result->m33 = source->m33;
}
i32 qseed = 0x3039;

i32 qrand(void) {
    qseed = qseed * 0x24cd + 1 & 0xffff;

    return qseed;
}

i32 getqseed(void) {
    return qseed;
}

void ResetSeeds(void) {
    // The PC runtime uses srand where the Android version calls srand48.
    srand(0);
    qseed = 0x3039;
    GAMERAND = 0x1f3ad27f;
    NuRandSeed(0);
    DebrisSetSeed(0);
    NuRandSetSeed(NULL, 0);
    NuPartSetSeed(0);
}

f32 __cdecl SeekLinearF(f32 current, f32 target, f32 step) {
    if (current > target) {
        current -= step;
        if (current < target)
            current = target;
    } else if (current < target) {
        current += step;
        if (current > target)
            current = target;
    }
    return current;
}

f32 __cdecl SeekValF(f32 current, f32 target, f32 rate) {
    f32 blend;

    blend = rate * FRAMETIME;
    if (blend > 1.0f)
        blend = 1.0f;
    return current + (target - current) * blend;
}

void __cdecl SeekVec(NUVEC *result, NUVEC *current, NUVEC *target, f32 rate) {
    f32 blend;

    blend = rate * FRAMETIME;
    if (blend > 1.0f)
        blend = 1.0f;
    result->x = current->x + (target->x - current->x) * blend;
    result->y = current->y + (target->y - current->y) * blend;
    result->z = current->z + (target->z - current->z) * blend;
}

void __cdecl CapVec(NUVEC *input, f32 maximum, NUVEC *output) {
    f32 length_squared = input->x * input->x + input->y * input->y + input->z * input->z;
    if (length_squared > maximum * maximum) {
        f32 factor = maximum / NuFsqrt(length_squared);
        output->x *= factor;
        output->y *= factor;
        output->z *= factor;
    }
}

i32 __cdecl SphereSphereOverlap(NUVEC *a, f32 radius_a, NUVEC *b, f32 radius_b) {
    f32 x = b->x - a->x;
    f32 y = b->y - a->y;
    f32 z = b->z - a->z;
    f32 radius = radius_a + radius_b;
    return x * x + y * y + z * z <= radius * radius;
}

i32 __cdecl SphereSphereOverlapScaleY(NUVEC *position_a, f32 radius_a, f32 y_radius_a, NUVEC *position_b, f32 radius_b,
                                      f32 y_radius_b) {
    f32 dx = position_b->x - position_a->x;
    f32 dy = position_b->y - position_a->y;
    f32 dz = position_b->z - position_a->z;
    f32 radius;
    if (radius_b != y_radius_b || radius_a != y_radius_a) {
        dy *= (radius_b + radius_a) / (y_radius_b + y_radius_a);
    }
    radius = radius_b + radius_a;
    return dx * dx + dy * dy + dz * dz <= radius * radius;
}

i32 __cdecl OnOrInsidePlane(NUVEC *point, NUVEC *plane_point, NUVEC *plane_normal, NUVEC *corrected_point,
                            f32 normal_offset, f32 *distance_out) {
    NUVEC test_point;
    f32 distance, correction;
    if (normal_offset == 0.0f)
        test_point = *point;
    else {
        NuVecScale(&test_point, plane_normal, normal_offset);
        NuVecAdd(&test_point, &test_point, point);
    }
    distance = (f32)(((f64)test_point.x - (f64)plane_point->x) * (f64)plane_normal->x +
                     ((f64)test_point.y - (f64)plane_point->y) * (f64)plane_normal->y +
                     ((f64)test_point.z - (f64)plane_point->z) * (f64)plane_normal->z);
    if (distance_out != NULL)
        *distance_out = distance;
    if (distance <= 0.0f) {
        if (corrected_point != NULL) {
            correction = (f32)(-((f64)distance - 0.5));
            NuVecScale(corrected_point, plane_normal, correction);
            NuVecAdd(corrected_point, corrected_point, point);
        }
        return 1;
    }
    return 0;
}

i32 __cdecl OnOrOutsidePlane(NUVEC *point, NUVEC *plane_point, NUVEC *normal) {
    return (point->y - plane_point->y) * normal->y + (point->x - plane_point->x) * normal->x +
               (point->z - plane_point->z) * normal->z >=
           0.0;
}

__declspec(noinline) void __cdecl FindAnglesZX(NUVEC *normal, u16 *x_rotation, u16 *z_rotation) {
    u16 x_angle = (u16)NuAtan2D(normal->z, normal->y);
    NUVEC rotated;
    u16 z_angle;

    if (x_rotation != NULL) {
        *x_rotation = x_angle;
    }
    temp_zrot = x_angle;
    NuVecRotateX(&rotated, normal, (NUANG)(0 - x_angle));
    z_angle = (u16)NuAtan2D(rotated.x, rotated.y);
    if (z_rotation != NULL) {
        *z_rotation = (u16)-z_angle;
    }
    temp_xrot = (u16)-z_angle;
}

void __cdecl FindAnglesXY(NUVEC *direction, u16 *x_rotation, u16 *y_rotation) {
    f32 horizontal_squared;
    temp_yrot = (u16)NuAtan2D(direction->x, direction->z);
    if (y_rotation)
        *y_rotation = temp_yrot;
    horizontal_squared = direction->x * direction->x + direction->z * direction->z;
    FindAnglesXFullTempPc = -NuAtan2D(direction->y, NuFsqrt(horizontal_squared));
    temp_zrot = (u16)FindAnglesXFullTempPc;
    if (x_rotation)
        *x_rotation = temp_zrot;
}

void __cdecl GetRotationAngles(NUVEC *direction, u16 *x_rotation, u16 *y_rotation) {
    NUVEC rotated, copy = *direction;
    u16 y_angle = (u16)-NuAtan2D(copy.z, copy.x);
    NuVecRotateY(&rotated, &copy, -(i32)y_angle);
    *x_rotation = (u16)-NuAtan2D(rotated.x, rotated.y);
    *y_rotation = y_angle;
}

i32 __cdecl RotDiff(u16 current, u16 target) {
    i32 difference = (i32)target - (i32)current;

    if (difference > 0x8000) {
        difference -= 0x10000;
    } else if (difference < -0x8000) {
        difference += 0x10000;
    }
    return difference;
}

u16 __cdecl SeekRot(u16 current, u16 target, f32 rate) {
    f32 blend;
    i32 difference;
    i32 angle;

    blend = rate * FRAMETIME;
    if (blend > 1.0f)
        blend = 1.0f;
    difference = (i32)target - (i32)current;
    if (difference > 0x8000)
        difference -= 0x10000;
    else if (difference < -0x8000)
        difference += 0x10000;
    angle = (i32)((f32)current + (f32)difference * blend);
    return (u16)angle;
}

u16 __cdecl TurnRot(u16 current, u16 target, i32 speed, i32 *difference_out) {
    i32 step;
    i32 difference;
    if (target == current)
        return target;
    step = (i32)(speed * FRAMETIME);
    difference = (i32)target - (i32)current;
    if (difference > 0x8000)
        difference -= 0x10000;
    else if (difference < -0x8000)
        difference += 0x10000;
    if (difference_out != NULL)
        *difference_out = difference;
    if (difference > 0) {
        if (difference <= step)
            return target;
        return (u16)(current + step);
    }
    if (difference >= -step)
        return target;
    return (u16)(current - step);
}

__declspec(noinline) f32 __cdecl PushingTowardsAngle(u16 input_angle, u16 direction) {
    NUVEC input, target;
    NuVecRotateY(&input, &v001, input_angle);
    NuVecRotateY(&target, &v001, direction);
    return input.x * target.x + input.z * target.z;
}

void __cdecl AnglesBetweenPoints(NUVEC *from, NUVEC *to, u16 *vertical, u16 *horizontal) {
    f32 x = to->x - from->x;
    f32 y = to->y - from->y;
    f32 z = to->z - from->z;
    *vertical = NuAtan2D(y, NuFsqrt(x * x + z * z));
    *horizontal = NuAtan2D(x, z);
}

void __cdecl CalculateInterceptVector(NUVEC *origin, NUVEC *target, NUVEC *velocity, f32 speed, NUVEC *direction,
                                      NUVEC *intercept) {
    NUVEC prediction = *target;
    i32 i;
    for (i = 0; i < 3; ++i) {
        f32 distance, time;
        NuVecSub(direction, &prediction, origin);
        distance = NuVecMag(direction);
        if (distance == 0.0f || speed == 0.0f)
            time = 0.0f;
        else
            time = distance / speed;
        NuVecAddScale(&prediction, target, velocity, time);
    }
    NuVecSub(direction, &prediction, origin);
    if (intercept != NULL)
        *intercept = prediction;
}

void __cdecl RotateGameMatrix(NUMTX *matrix, i32 order, u16 x, u16 y, u16 z) {
    switch (order) {
        case 0:
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            break;
        case 1:
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            break;
        case 2:
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            break;
        case 3:
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            break;
    }
}

i32 __cdecl MatrixReflection(NUMTX *matrix, i32 axis, f32 plane, f32 override_plane, NUMTX *result) {
    switch (axis) {
        case 1:
            *result = *matrix;
            result->m00 = -result->m00;
            result->m10 = -result->m10;
            result->m20 = -result->m20;
            result->m30 = plane - (result->m30 - plane);
            return 1;
        case 2:
            if ((f64)override_plane != 2000000.0 &&
                (MatrixReflection_CanOverrideFn == NULL || MatrixReflection_CanOverrideFn() != 0))
                plane = override_plane;
            *result = *matrix;
            result->m01 = -result->m01;
            result->m11 = -result->m11;
            result->m21 = -result->m21;
            result->m31 = plane - (result->m31 - plane);
            return 1;
        case 3:
            *result = *matrix;
            result->m02 = -result->m02;
            result->m12 = -result->m12;
            result->m22 = -result->m22;
            result->m32 = plane - (result->m32 - plane);
            return 1;
        default:
            return 0;
    }
}

i32 __cdecl MatrixReflectionVU0_AXISY(NUMTX *matrix, f32 plane, f32 override_plane, NUMTX *result) {
    if ((f64)override_plane != 2000000.0 &&
        (MatrixReflection_CanOverrideFn == NULL || MatrixReflection_CanOverrideFn() != 0))
        plane = override_plane;
    ReflectionCopyMatrixPc(result, matrix);
    result->m01 = -result->m01;
    result->m11 = -result->m11;
    result->m21 = -result->m21;
    result->m31 = plane - (result->m31 - plane);
    return 1;
}

void InterpolateRotationMatrix(NUMTX *result, NUMTX *first, NUMTX *second, float fraction) {
    SeekVec(NUMTX_GET_ROW_VEC(result, 0), NUMTX_GET_ROW_VEC(first, 0), NUMTX_GET_ROW_VEC(second, 0), fraction);
    SeekVec(NUMTX_GET_ROW_VEC(result, 1), NUMTX_GET_ROW_VEC(first, 1), NUMTX_GET_ROW_VEC(second, 1), fraction);
    SeekVec(NUMTX_GET_ROW_VEC(result, 2), NUMTX_GET_ROW_VEC(first, 2), NUMTX_GET_ROW_VEC(second, 2), fraction);
    NuVecNorm(NUMTX_GET_ROW_VEC(result, 0), NUMTX_GET_ROW_VEC(result, 0));
    NuVecNorm(NUMTX_GET_ROW_VEC(result, 1), NUMTX_GET_ROW_VEC(result, 1));
    NuVecNorm(NUMTX_GET_ROW_VEC(result, 2), NUMTX_GET_ROW_VEC(result, 2));
}

void __cdecl QuatInterpolateRotationMatrix(NUMTX *out, NUMTX *left, NUMTX *right, f32 fraction) {
    NUQUAT a, b, value;
    NuMtxToQuat(right, &b);
    NuMtxToQuat(left, &a);
    NuQuatSlerp(&value, &a, &b, fraction);
    NuQuatToMtx(&value, out);
}

__declspec(noinline) f32 __cdecl GameShadow(GAMEOBJECT *object, NUVEC *position, f32 probe, i32 mask) {
    i32 handles[16];
    i32 handle_count = 0;
    i32 i;
    f32 height;

    if (object != NULL) {
        WORLDINFO *world = WorldInfo_CurrentlyActive();
        if (object->level_object_index != -1)
            handles[handle_count++] = object->level_object_index;
        if (world != NULL && world->shadow_platform_list_pc != NULL && VehicleArea == 0 &&
            (object->flags_1fc & 0x80u) == 0) {
            struct shadow_platform_list_pc_s *list = world->shadow_platform_list_pc;
            for (i = 0; i < list->count; ++i) {
                if (list->entries[i].object != NULL)
                    handles[handle_count++] = list->entries[i].object->level_object_index;
            }
        }
        for (i = 0; i < handle_count; ++i)
            PlatOnOff(handles[i], 0);
        if (LAYER_HOVERIGNORE != 0xffffffffu && object->character_data->game_character->field_028 == 0.0f)
            mask &= (i32)~LAYER_HOVERIGNORE;
    }

    height = NewShadowEx(position, 0.0f, probe, probe, mask);
    if (object != NULL && object->level_object_index != -1)
        PlatOnOff(object->level_object_index, 1);
    for (i = 0; i < handle_count; ++i)
        PlatOnOff(handles[i], 1);
    return height;
}

f32 __cdecl BlobShadowFade(NUVEC *position, f32 fade_start, f32 fade_end, f32 alpha) {
    NUVEC delta;
    f32 distance_squared;
    if (alpha <= 0.0f)
        return 0.0f;
    NuVecSub(&delta, position, (NUVEC *)&pNuCam->mtx.m30);
    distance_squared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
    if (distance_squared < fade_end * fade_end) {
        if (distance_squared > fade_start * fade_start)
            return (1.0f - (NuFsqrt(distance_squared) - fade_start) / (fade_end - fade_start)) * alpha;
        return alpha;
    }
    return 0.0f;
}

static void SplinePointAngles(NUGSPLINE *spline, i32 index, i32 looping, u16 *pitch, u16 *angle) {
    NUVEC *current;
    NUVEC *previous;
    NUVEC *next;
    i32 previous_index;
    i32 next_index;
    f32 dx;
    f32 dy;
    f32 dz;

    if (pitch == NULL && angle == NULL) {
        return;
    }
    dx = 0.0f;
    dy = 0.0f;
    dz = 0.0f;
    current = &spline->pts[index];
    previous = NULL;
    previous_index = index - 1;
    if (previous_index < 0 && looping) {
        previous = &spline->pts[spline->length - 1];
    } else if (previous_index >= 0) {
        previous = &spline->pts[previous_index];
    }
    if (previous != NULL) {
        dx += current->x - previous->x;
        dy += current->y - previous->y;
        dz += current->z - previous->z;
    }
    next = NULL;
    next_index = index + 1;
    if (next_index >= spline->length && looping) {
        next = spline->pts;
    } else if (next_index < spline->length) {
        next = &spline->pts[next_index];
    }
    if (next != NULL) {
        dx += next->x - current->x;
        dy += next->y - current->y;
        dz += next->z - current->z;
    }
    if (pitch != NULL) {
        *pitch = NuAtan2D(dy, NuFsqrt(dx * dx + dz * dz));
    }
    if (angle != NULL) {
        *angle = NuAtan2D(dx, dz);
    }
}

void __cdecl PointAlongSpline(NUGSPLINE *spline, f32 along, NUVEC *position, u16 *angle, u16 *pitch, i32 looping) {
    u32 extent;
    u32 fixed;
    i32 index;
    i32 next_index;
    NUVEC *current;
    NUVEC *next;
    f32 fraction;
    u16 next_angle;
    u16 next_pitch;

    if (angle != NULL) {
        *angle = 0;
    }
    if (pitch != NULL) {
        *pitch = 0;
    }
    if (spline == NULL) {
        return;
    }
    if (along > 1.0f) {
        along = 1.0f;
    } else if (along < 0.0f) {
        along = 0.0f;
    }

    if (looping) {
        extent = (u32)spline->length << 16;
    } else {
        extent = (u32)(spline->length - 1) << 16;
    }
    fixed = (u32)(i32)((f32)extent * along);
    index = (i32)(fixed >> 16);
    current = &spline->pts[index];
    *position = *current;
    SplinePointAngles(spline, index, looping, pitch, angle);
    next_index = index + 1;
    if (next_index >= spline->length) {
        if (!looping) {
            return;
        }
        next_index = 0;
    } else if (next_index < 0) {
        return;
    }
    fixed &= 0xffff;
    if (fixed == 0) {
        return;
    }

    next = &spline->pts[next_index];
    fraction = fixed * (1.0f / 65536.0f);
    position->x += (next->x - current->x) * fraction;
    position->y += (next->y - current->y) * fraction;
    position->z += (next->z - current->z) * fraction;
    SplinePointAngles(spline, next_index, looping, pitch != NULL ? &next_pitch : NULL,
                      angle != NULL ? &next_angle : NULL);
    if (angle != NULL) {
        *angle += (i32)(RotDiff(*angle, next_angle) * fraction);
    }
    if (pitch != NULL) {
        *pitch += (i32)(RotDiff(*pitch, next_pitch) * fraction);
    }
}

f32 __cdecl SplineLength(NUGSPLINE *spline, i32 closed) {
    i32 segment_count;
    i32 segment;
    f32 length;
    NUVEC *previous;
    if (spline == NULL)
        return 0.0f;
    if (spline->length < 2)
        return 0.0f;
    segment_count = spline->length;
    if (closed == 0)
        --segment_count;
    length = 0.0f;
    previous = spline->pts;
    for (segment = 0; segment < segment_count; ++segment) {
        NUVEC *next = &spline->pts[segment == spline->length - 1 ? 0 : segment + 1];
        NUVEC difference;
        NuVecSub(&difference, next, previous);
        length += NuVecMag(&difference);
        previous = next;
    }
    return length;
}

i32 __cdecl StreamingInsideLineXZPc(f32 x, f32 z, f32 x0, f32 z0, f32 x1, f32 z1) {
    return (x - x0) * (z1 - z0) + (z - z0) * (x0 - x1) >= 0.0f;
}

i32 __cdecl LineCrossedXZ(f32 ax, f32 az, f32 bx, f32 bz, f32 cx, f32 cz, f32 dx, f32 dz) {
    i32 result = 0;
    if (!((bx - cx) * (dz - cz) + (bz - cz) * (cx - dx) >= 0.0f) &&
        (ax - cx) * (dz - cz) + (az - cz) * (cx - dx) >= 0.0f) {
        result = 1;
        if ((bx - ax) * (cz - az) + (bz - az) * (ax - cx) >= 0.0f &&
            (bx - dx) * (az - dz) + (bz - dz) * (dx - ax) >= 0.0f) {
            result = 2;
        }
    }
    return result;
}

f32 __cdecl RatioAlongLineXZ(NUVEC *point, NUVEC *start, NUVEC *end) {
    f32 dx = end->x - start->x;
    f32 dz = end->z - start->z;
    u16 angle = (u16)-NuAtan2D(dx, dz);
    f32 sine = NuTrigTable[angle >> 1];
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 along = (point->z - start->z) * cosine - (point->x - start->x) * sine;
    f32 length;
    if (along <= 0.0f)
        return 0.0f;
    length = dz * cosine - dx * sine;
    if (along >= length)
        return 1.0f;
    return along / length;
}

f32 __cdecl DistanceToLineXZ(NUVEC *position, NUVEC *first, NUVEC *second) {
    u16 angle;
    union {
        f32 value;
        u32 bits;
    } result, absolute;
    angle = (u16)-NuAtan2D(second->x - first->x, second->z - first->z);
    result.value = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * (position->x - first->x) +
                   NuTrigTable[angle >> 1] * (position->z - first->z);
    absolute.bits = result.bits & 0x7fffffffu;
    return absolute.value;
}

f32 __cdecl RatioBetweenEdgesXZ(NUVEC *point, NUVEC *edge_a0, NUVEC *edge_a1, NUVEC *edge_b0, NUVEC *edge_b1) {
    f32 distance_a = DistanceToLineXZ(point, edge_a0, edge_a1);
    f32 distance_b = DistanceToLineXZ(point, edge_b0, edge_b1);
    return distance_a / (distance_a + distance_b);
}

i32 __cdecl LineIntersectXY(NUVEC *a, NUVEC *b, NUVEC *c, NUVEC *d, NUVEC *first, NUVEC *second) {
    f32 t_numerator = (d->x - c->x) * (a->y - c->y) - (d->y - c->y) * (a->x - c->x);
    f32 t_denominator = (d->y - c->y) * (b->x - a->x) - (d->x - c->x) * (b->y - a->y);
    f32 u_numerator = (b->x - a->x) * (a->y - c->y) - (b->y - a->y) * (a->x - c->x);
    f32 u_denominator = (d->y - c->y) * (b->x - a->x) - (d->x - c->x) * (b->y - a->y);
    f32 t;
    f32 u;

    if (t_numerator == 0.0f && t_denominator == 0.0f && u_numerator == 0.0f && u_denominator == 0.0f)
        return 0;
    if (t_denominator == 0.0f || u_denominator == 0.0f)
        return 0;
    t = t_numerator / t_denominator;
    u = u_numerator / u_denominator;
    if (first != NULL) {
        first->x = (b->x - a->x) * t + a->x;
        first->y = (b->y - a->y) * t + a->y;
    }
    if (second != NULL) {
        second->x = (d->x - c->x) * u + c->x;
        second->y = (d->y - c->y) * u + c->y;
    }
    return t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f;
}

i32 XZLinesIntersect(NUVEC *a, NUVEC *b, NUVEC *c, NUVEC *d, float *first, float *second) {
    i32 angle;
    NUVEC ac;
    NUVEC bc;
    NUVEC ca;
    NUVEC da;
    NUVEC direction;
    NuVecSub(&direction, b, a);
    angle = NuAtan2D(direction.x, direction.z);
    NuVecSub(&ca, c, a);
    NuVecRotateY(&ca, &ca, -angle);
    NuVecSub(&da, d, a);
    NuVecRotateY(&da, &da, -angle);
    if (NuFsign(ca.x) != NuFsign(da.x)) {
        NuVecSub(&direction, d, c);
        angle = NuAtan2D(direction.x, direction.z);
        NuVecSub(&ac, a, c);
        NuVecRotateY(&ac, &ac, -angle);
        NuVecSub(&bc, b, c);
        NuVecRotateY(&bc, &bc, -angle);
        if (NuFsign(ac.x) != NuFsign(bc.x)) {
            if (first != NULL)
                *first = UtilitiesFabsPc(ac.x) / (UtilitiesFabsPc(ac.x) + UtilitiesFabsPc(bc.x));
            if (second != NULL)
                *second = UtilitiesFabsPc(ca.x) / (UtilitiesFabsPc(ca.x) + UtilitiesFabsPc(da.x));
            return 1;
        }
    }
    return 0;
}

/* Original source name unknown. XZ distance from a point, given in the segment's
   rotated frame, to the segment from start to end; also returns the fraction
   along the segment. */
static f32 XZPointToSegmentPc(NUVEC *point, f32 *fraction, NUVEC *start, NUVEC *end, f32 length) {
    if (point->z < 0.0f) {
        *fraction = 0.0f;
        return NuVecXZDist(start, point, NULL);
    }
    if (point->z > length) {
        *fraction = 1.0f;
        return NuVecXZDist(end, point, NULL);
    }
    *fraction = point->z / length;
    return UtilitiesFabsPc(point->x);
}

f32 XZLinesClosest(NUVEC *a, NUVEC *b, NUVEC *c, NUVEC *d, f32 *first, f32 *second) {
    i32 angle;
    f32 ab_length;
    f32 cd_length;
    f32 closest;
    f32 distance;
    f32 fraction;
    NUVEC direction;
    NUVEC ca;
    NUVEC da;
    NUVEC ac;
    NUVEC bc;
    closest = 1000000000.0f;
    NuVecSub(&direction, b, a);
    angle = NuAtan2D(direction.x, direction.z);
    NuVecSub(&ca, c, a);
    NuVecRotateY(&ca, &ca, -angle);
    NuVecSub(&da, d, a);
    NuVecRotateY(&da, &da, -angle);
    NuVecSub(&direction, d, c);
    angle = NuAtan2D(direction.x, direction.z);
    NuVecSub(&ac, a, c);
    NuVecRotateY(&ac, &ac, -angle);
    NuVecSub(&bc, b, c);
    NuVecRotateY(&bc, &bc, -angle);

    if (NuFsign(ca.x) != NuFsign(da.x) && NuFsign(ac.x) != NuFsign(bc.x)) {
        *first = UtilitiesFabsPc(ac.x) / (UtilitiesFabsPc(ac.x) + UtilitiesFabsPc(bc.x));
        *second = UtilitiesFabsPc(ca.x) / (UtilitiesFabsPc(ca.x) + UtilitiesFabsPc(da.x));
        return 0.0f;
    }

    ab_length = NuVecXZDist(b, a, NULL);
    cd_length = NuVecXZDist(d, c, NULL);

    distance = XZPointToSegmentPc(&ca, &fraction, a, b, ab_length);
    if (distance < closest) {
        closest = distance;
        *first = fraction;
        *second = 0.0f;
    }
    distance = XZPointToSegmentPc(&da, &fraction, a, b, ab_length);
    if (distance < closest) {
        closest = distance;
        *first = fraction;
        *second = 1.0f;
    }
    distance = XZPointToSegmentPc(&ac, &fraction, c, d, cd_length);
    if (distance < closest) {
        closest = distance;
        *second = fraction;
        *first = 0.0f;
    }
    distance = XZPointToSegmentPc(&bc, &fraction, c, d, cd_length);
    if (distance < closest) {
        closest = distance;
        *second = fraction;
        *first = 1.0f;
    }
    return closest;
}

i32 __cdecl LineIntersectSphere(NUVEC *origin, NUVEC *direction, NUVEC *center, f32 radius_squared,
                                f32 *distance_squared) {
    f32 x;
    f32 y;
    f32 z;
    f32 projection;
    f32 point_squared;
    f32 projection_squared;
    f32 distance;
    x = center->x - origin->x;
    y = center->y - origin->y;
    z = center->z - origin->z;
    projection = direction->x * x + direction->y * y + direction->z * z;
    if (projection < 0.0f)
        return 0;
    point_squared = x * x + y * y + z * z;
    projection_squared = projection * projection;
    distance = point_squared - projection_squared;
    if (!(radius_squared >= distance))
        return 0;
    if (distance_squared != NULL)
        *distance_squared = distance;
    return 1;
}

i32 __cdecl LineIntersectSphereXZPc(NUVEC *origin, NUVEC *direction, NUVEC *center, f32 radius_squared) {
    f32 x = center->x - origin->x;
    f32 z = center->z - origin->z;
    f32 projection = direction->x * x + direction->z * z;
    f32 point_squared;
    f32 projection_squared;
    f32 distance;
    if (projection < 0.0f)
        return 0;
    point_squared = x * x + z * z;
    projection_squared = projection * projection;
    distance = point_squared - projection_squared;
    if (!(radius_squared >= distance))
        return 0;
    return 1;
}

void __cdecl MoveSplinePosition(SPLINEPOS *position, f32 distance) {
    i32 points;
    NUVEC offset;
    NUVEC *current, *next;
    f32 fraction;

    if (position == NULL || position->spline == NULL || position->spline->length <= 1)
        return;
    if (position->looping)
        points = position->spline->length + 1;
    else
        points = position->spline->length;
    if (position->segment >= points - 1)
        return;

    if (distance >= 0.0f) {
        if (distance > 0.0f)
            do {
                position->segment_distance += distance;
                if (!(position->segment_distance >= position->segment_length)) {
                    current = SPLINE_POINT(position->spline, position->segment);
                    next = SPLINE_POINT(position->spline, (position->segment + 1) % position->spline->length);
                    NuVecSub(&offset, next, current);
                    fraction =
                        position->segment_length != 0.0f ? position->segment_distance / position->segment_length : 0.0f;
                    NuVecScale(&offset, &offset, fraction);
                    NuVecAdd(&position->position, current, &offset);
                    break;
                }
                distance = position->segment_distance - position->segment_length;
                position->segment++;
                if (position->segment >= points - 1) {
                    if (!position->looping) {
                        position->finished = 1;
                        position->position = *SPLINE_POINT(position->spline, position->segment);
                        position->segment--;
                        position->along = 1.0f;
                        position->segment_distance = position->segment_length;
                        return;
                    }
                    position->segment = 0;
                }
                current = SPLINE_POINT(position->spline, position->segment);
                next = SPLINE_POINT(position->spline, (position->segment + 1) % position->spline->length);
                position->segment_distance = 0.0f;
                position->segment_length = NuVecDist(next, current, &offset);
                if (distance == 0.0f)
                    position->position = *current;
            } while (distance > 0.0f);
    } else if (position->segment >= 0 && distance < 0.0f) {
        do {
            position->segment_distance += distance;
            if (!(position->segment_distance < 0.0f)) {
                current = SPLINE_POINT(position->spline, position->segment);
                next = SPLINE_POINT(position->spline, (position->segment + 1) % position->spline->length);
                NuVecSub(&offset, next, current);
                fraction =
                    position->segment_length != 0.0f ? position->segment_distance / position->segment_length : 0.0f;
                NuVecScale(&offset, &offset, fraction);
                NuVecAdd(&position->position, current, &offset);
                break;
            }
            distance = position->segment_distance;
            position->segment--;
            if (position->segment < 0) {
                if (!position->looping) {
                    position->finished = 1;
                    position->position = *SPLINE_POINT(position->spline, 0);
                    position->segment++;
                    position->along = 0.0f;
                    position->segment_distance = 0.0f;
                    return;
                }
                position->segment = (i16)(points - 2);
            }
            current = SPLINE_POINT(position->spline, position->segment);
            next = SPLINE_POINT(position->spline, (position->segment + 1) % position->spline->length);
            position->segment_length = NuVecDist(next, current, &offset);
            position->segment_distance = position->segment_length;
            if (distance == 0.0f)
                position->position = *next;
        } while (distance < 0.0f);
    }
    fraction = position->segment_distance / position->segment_length;
    position->along = (fraction + position->segment) / (points - 1);
}

void __cdecl GetNearestSplinePos(NUVEC *origin, SPLINEPOS *result, NUGSPLINE *spline, i32 looping, i16 first_point,
                                 i16 last_point) {
    f32 nearest = 1000000000.0f;
    i16 point_count;
    i32 logical_count, index;
    NUVEC *point, *current, *next;
    NUVEC offset;

    if (result == NULL)
        return;
    memset(result, 0, sizeof(*result));
    if (origin == NULL || spline == NULL || spline->length <= 1)
        return;
    result->spline = spline;
    result->looping = (i8)looping;
    point_count = spline->length;
    logical_count = result->looping ? point_count + 1 : point_count;
    if (first_point >= 0) {
        index = first_point;
        if (index >= logical_count)
            return;
    } else
        index = 0;
    if (last_point < 0 || last_point > point_count)
        last_point = point_count;
    point = SPLINE_POINT(spline, index);
    do {
        f32 distance = NuVecDistSqr(origin, point, &offset);
        if (distance < nearest) {
            nearest = distance;
            result->segment = (i16)index;
        }
        point = (NUVEC *)((u8 *)point + (i16)result->spline->pt_size);
        index++;
    } while (index < last_point);

    spline = result->spline;
    current = SPLINE_POINT(spline, result->segment);
    next = SPLINE_POINT(spline, (result->segment + 1) % spline->length);
    result->segment_distance = 0.0f;
    result->segment_length = NuVecDist(next, current, &offset);
    result->position = *current;
    nearest = result->segment_distance / result->segment_length;
    result->along = (nearest + result->segment) / (logical_count - 1);
}

void EvaluateSplineXZIntersection(NUGSPLINE *first, i32 first_looping, SPLINEPOS *first_position, NUGSPLINE *second,
                                  i32 second_looping, SPLINEPOS *second_position) {
    f32 closest = 1000000000.0f;
    i32 first_count, second_count, i, j, first_segment, second_segment;
    NUVEC *first_start, *first_end;
    f32 distance, first_fraction, second_fraction;
    NUVEC direction;

    memset(first_position, 0, sizeof(*first_position));
    memset(second_position, 0, sizeof(*second_position));
    first_position->spline = first;
    first_position->looping = (i8)first_looping;
    second_position->spline = second;
    second_position->looping = (i8)second_looping;
    if (first == NULL || second == NULL || first->length == 0 || second->length == 0)
        return;
    first_count = first->length;
    if (first_looping)
        first_count++;
    second_count = second->length;
    if (second_looping)
        second_count++;
    for (i = 0; i < first_count - 1; ++i) {
        first_segment = i % first->length;
        first_start = &first->pts[first_segment];
        first_end = &first->pts[(i + 1) % first->length];
        /* Retail includes the wraparound segment of the second spline even
         * without looping, and only stops this inner scan at an intersection. */
        for (j = 0; j < second_count; ++j) {
            second_segment = j % second->length;
            distance = XZLinesClosest(first_start, first_end, &second->pts[second_segment],
                                      &second->pts[(j + 1) % second->length], &first_fraction, &second_fraction);
            if (distance < closest) {
                closest = distance;
                first_position->segment = (i16)first_segment;
                first_position->segment_distance = first_fraction;
                second_position->segment = (i16)second_segment;
                second_position->segment_distance = second_fraction;
                if (distance == 0.0f)
                    break;
            }
        }
    }
    first_position->segment_length = NuVecDist(&first->pts[(first_position->segment + 1) % first->length],
                                               &first->pts[first_position->segment], &direction);
    first_position->segment_distance *= first_position->segment_length;
    MoveSplinePosition(first_position, 0.00001f);
    second_position->segment_length = NuVecDist(&second->pts[(second_position->segment + 1) % second->length],
                                                &second->pts[second_position->segment], &direction);
    second_position->segment_distance *= second_position->segment_length;
    MoveSplinePosition(second_position, 0.00001f);
}

i32 __cdecl OutSideSplineArea(NUVEC *position, NUGSPLINE *spline, NUVEC *edge_end, NUVEC *edge_start, i32 inside) {
    f32 best = 1000000000.0f;
    i32 closest_index, count, i;
    NUVEC closest;

    if (position == NULL || spline == NULL)
        return 0;
    {
        NUVEC ray_start, ray_end, end;
        f32 end_y;
        i32 intersections, odd;
        ray_end = *position;
        ray_start = *position;
        end_y = ray_start.y;
        ray_start.y = -ray_start.z;
        ray_start.z = end_y;
        ray_end.z = ray_end.y;
        ray_end.y = 100000.0f;
        end = spline->pts[0];
        end_y = end.y;
        end.y = -end.z;
        end.z = end_y;
        intersections = 0;
        for (i = 1; i < spline->length; ++i) {
            NUVEC start = end;
            end = spline->pts[i];
            end_y = end.y;
            end.y = -end.z;
            end.z = end_y;
            intersections += LineIntersectXY(&ray_start, &ray_end, &start, &end, NULL, NULL);
        }
        odd = intersections & 1;
        if (inside == 0) {
            if (odd != 0)
                return 0;
        } else if (odd == 0)
            return 0;
    }
    closest_index = 0;
    closest = spline->pts[0];
    count = spline->length;
    for (i = 0; i < count - 1; ++i) {
        NUVEC point = spline->pts[i];
        NUVEC delta;
        f32 distance;
        delta.x = point.x - position->x;
        delta.z = point.z - position->z;
        distance = delta.x * delta.x + delta.z * delta.z;
        if (distance < best) {
            closest_index = i;
            closest = point;
            best = distance;
        }
    }
    {
        NUVEC next_point, previous_point, end_point, start_point, a, b;
        i32 previous, next;
        if (closest_index == 0) {
            previous = count - 2;
            next = 1;
        } else if (closest_index == count - 2) {
            previous = closest_index - 1;
            next = 0;
        } else {
            previous = closest_index - 1;
            next = closest_index + 1;
        }
        next_point = spline->pts[next];
        previous_point = spline->pts[previous];
        a.x = next_point.x - position->x;
        a.z = next_point.z - position->z;
        b.x = previous_point.x - position->x;
        b.z = previous_point.z - position->z;
        if (b.x * b.x + b.z * b.z > a.x * a.x + a.z * a.z) {
            end_point = next_point;
            start_point = closest;
        } else {
            end_point = closest;
            start_point = previous_point;
        }
        if (edge_end != NULL)
            *edge_end = end_point;
        if (edge_start != NULL)
            *edge_start = start_point;
    }
    return 1;
}

void __cdecl VecRotateAxis(NUVEC *vector, u16 angle, NUVEC *axis) {
    f32 cosine, sine, partial;
    NUVEC result;
    f64 complement, tx, ty, tz, xy, xz, yz, sx, sy, sz;
    NuVecNorm(axis, axis);
    cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    sine = NuTrigTable[angle >> 1];
    complement = 1.0 - (f64)cosine;
    tx = complement * (f64)axis->x;
    ty = complement * (f64)axis->y;
    tz = complement * (f64)axis->z;
    xy = tx * (f64)axis->y;
    xz = tx * (f64)axis->z;
    yz = ty * (f64)axis->z;
    sx = (f64)sine * axis->x;
    sy = (f64)sine * axis->y;
    sz = (f64)sine * axis->z;
    partial = (f32)(((tx * (f64)axis->x + (f64)cosine) * (f64)vector->x) + 0.0);
    partial = (f32)((f64)partial + (xy - sz) * (f64)vector->y);
    result.x = (f32)((f64)partial + (xz + sy) * (f64)vector->z);
    partial = (f32)(((xy + sz) * (f64)vector->x) + 0.0);
    partial = (f32)((f64)partial + (ty * (f64)axis->y + (f64)cosine) * (f64)vector->y);
    result.y = (f32)((f64)partial + (yz - sx) * (f64)vector->z);
    partial = (f32)(((xz - sy) * (f64)vector->x) + 0.0);
    partial = (f32)((f64)partial + (sx + yz) * (f64)vector->y);
    result.z = (f32)((f64)partial + (tz * (f64)axis->z + (f64)cosine) * (f64)vector->z);
    vector->x = result.x;
    vector->y = result.y;
    vector->z = result.z;
}

/* PC animation constructors call the shared game allocator out of line. */
__declspec(noinline) void *__cdecl GameBufferAlloc(VARIPTR *buf, VARIPTR *end, i32 size) {
    void *ptr = NULL;

    if (buf != NULL && end != NULL && buf->addr + (u32)size < end->addr) {
        ptr = (void *)((buf->addr + 3) & ~3u);
        buf->addr = ((buf->addr + 3) & ~3u) + (u32)size;
    }
    if (ptr != NULL) {
        memset(ptr, 0, (u32)size);
    }
    return ptr;
}

void __cdecl EvalAnim(NUSPECIALHANDLE *special, f32 frame, NUMTX *matrix, i32 update) {
    NUINSTANIM *instance_animation;
    ANI3_HEADER_PC *animation;
    NUGSCN *scene;
    NUMTX *instance_matrix;

    if (special == NULL || matrix == NULL) {
        return;
    }
    instance_animation = (NUINSTANIM *)NuSpecialGetInstAnim(special);
    if (instance_animation != NULL) {
        scene = (NUGSCN *)special->scene;
        animation = (ANI3_HEADER_PC *)scene->instance_animation_data[instance_animation->animation_index];
        if (animation == NULL) {
            return;
        }
        NuAnimData2CalcMatrix(animation, 0, frame, matrix);
        if (update != 0) {
            instance_matrix = NuSpecialGetInstanceMtx(special);
            if (instance_matrix != NULL) {
                matrix->m30 += instance_matrix->m30;
                matrix->m31 += instance_matrix->m31;
                matrix->m32 += instance_matrix->m32;
            }
        }
    } else if (update != 0) {
        instance_matrix = NuSpecialGetInstanceMtx(special);
        if (instance_matrix != NULL) {
            *matrix = *instance_matrix;
        }
    }
}

void __cdecl EvalAnim2(NUSPECIALHANDLE *special, f32 frame) {
    NUINSTANIM *instance_animation;
    ANI3_HEADER_PC *animation;
    NUGSCN *scene;
    NUMTX *instance_matrix;
    i32 animation_index;

    instance_animation = (NUINSTANIM *)NuSpecialGetInstAnim(special);
    if (instance_animation == NULL) {
        return;
    }
    scene = (NUGSCN *)special->scene;
    animation = (ANI3_HEADER_PC *)scene->instance_animation_data[instance_animation->animation_index];
    if (animation == NULL || instance_animation->prev_eval_time == frame) {
        return;
    }
    instance_matrix = NuSpecialGetInstanceMtx(special);
    NuAnimData2CalcMatrix(animation, 0, frame, &instance_animation->matrix);
    instance_animation->prev_eval_time = frame;
    animation_index = (i32)(instance_animation - scene->instance_animations);
    scene->instance_animation_matrices[animation_index] = instance_animation->matrix;
    instance_animation->matrix.m30 += instance_matrix->m30;
    instance_animation->matrix.m31 += instance_matrix->m31;
    instance_animation->matrix.m32 += instance_matrix->m32;
    NuSpecialUpdate(special);
}

f32 NuSpecialGetAnimPos(nuhspecial_s *special) {
    f32 end_frame;
    f32 position;
    NUINSTANIM *animation = NuSpecialGetInstAnim(special);
    if (animation == NULL || special->scene->instance_animation_data[animation->animation_index] == NULL) {
        return 0.0f;
    }
    end_frame = NuSpecialGetAnimEndFrame(special);
    position = (animation->ltime - 1.0f) / (end_frame - 1.0f);
    if (position < 0.0f) {
        return 0.0f;
    }
    if (position > 1.0f)
        return 1.0f;
    return position;
}

__declspec(noinline) void __cdecl SaveSceneObjectAnimTFactors(NUGSCN *scene) {
    NUINSTANCE *instance;
    i32 count;

    if (scene == NULL || scene->display_list != NULL) {
        return;
    }
    count = scene->num_instances;
    instance = scene->instances + count;
    if (count == 0) {
        return;
    }
    do {
        NUINSTANIM *animation;
        --instance;
        --count;
        animation = instance->animation;
        if (animation != NULL)
            animation->fparam1 = animation->tfactor;
    } while (count != 0);
}

void __cdecl InitTexAnimScripts(char **table) {
    char path[0x40];
    char **entry;
    TexAnimScriptTablePc = table;
    if (table == NULL) {
        return;
    }
    for (entry = table; *entry != NULL; ++entry) {
        permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
        NuStrCpy(path, "stuff\\ats\\");
        NuStrCat(path, *entry);
        NuStrCat(path, ".ats");
        if (NuTexAnimProgReadScript(path, &permbuffer_ptr, permbuffer_end, (i32)DEFAULTFPS) == NULL) {
            NuTexAnimScriptMissingPc(path);
        }
    }
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 15) & ~15u;
}

void SetTexAnimSignals() {
    NuTexAnimSetSignals(texanimbits);
}

i32 __cdecl Draw3DObject(WORLDINFO *world, i32 object_index, NUVEC *position, u16 x_rotation, u16 y_rotation,
                         u16 z_rotation, float scale_x, float scale_y, float scale_z, i32 rotate_order) {
    if (object_index == -1) {
        return 1;
    }
    if (world == NULL) {
        world = WorldInfo_CurrentlyActive();
    }
    if (world == NULL) {
        return 1;
    }
    if (world->level_objects[object_index].active != 0 && (scale_x != 0.0f || scale_y != 0.0f || scale_z != 0.0f)) {
        NUVEC scale = {scale_x, scale_y, scale_z};
        NUMTX matrix;
        NuMtxSetScale(&matrix, &scale);
        RotateGameMatrix(&matrix, rotate_order, x_rotation, y_rotation, z_rotation);
        NuMtxTranslate(&matrix, position);
        return NuSpecialDrawAt(&world->level_objects[object_index].special, &matrix);
    }
    return 0;
}

i32 __cdecl Draw3DObjectMtx(WORLDINFO *world, i32 object_id, NUMTX *matrix) {
    if (object_id == -1)
        return 1;
    if (world == NULL) {
        world = WorldInfo_CurrentlyActive();
        if (world == NULL)
            return 1;
    }
    if (world->level_objects[object_id].active != 0)
        return NuSpecialDrawAt(&world->level_objects[object_id].special, matrix);
    return 0;
}

i32 __cdecl DrawPanel3DObject(f32 x, f32 y, f32 z, f32 scale_x, f32 scale_y, f32 scale_z, u16 rotate_x, u16 rotate_y,
                              u16 rotate_z, NUSPECIALHANDLE *special, i32 rotate_order, f32 alpha) {
    if (alpha > 0.0f && special != NULL && NuSpecialExistsFn(special) != 0 &&
        (scale_x != 0.0f || scale_y != 0.0f || scale_z != 0.0f)) {
        NUVEC scale = {scale_x, scale_y, scale_z};
        NUMTX matrix;
        NuMtxSetScale(&matrix, &scale);
        RotateGameMatrix(&matrix, rotate_order, rotate_x, rotate_y, rotate_z);
        matrix.translation.x = x * PANEL3DMULX;
        matrix.translation.y = y * PANEL3DMULY;
        matrix.translation.z = z;
        NuMtxMulVU0(&matrix, &matrix, NuCameraGetMtx());
        NuSpecialDrawAtAlpha(special, &matrix, alpha);
    }
    return 0;
}

/* Append to render.c. Native PC 552d10, 229 bytes, cdecl full stack ABI. */
i32 __cdecl DrawPanel3DObjectNoAlpha(f32 x, f32 y, f32 z, f32 scale_x, f32 scale_y, f32 scale_z, u16 rotate_x,
                                     u16 rotate_y, u16 rotate_z, NUSPECIALHANDLE *special, i32 rotate_order) {
    if (special != NULL && NuSpecialExistsFn(special) != 0 && (scale_x != 0.0f || scale_y != 0.0f || scale_z != 0.0f)) {
        NUVEC scale = {scale_x, scale_y, scale_z};
        NUMTX matrix;
        NuMtxSetScale(&matrix, &scale);
        RotateGameMatrix(&matrix, rotate_order, rotate_x, rotate_y, rotate_z);
        matrix.translation.x = x * PANEL3DMULX;
        matrix.translation.y = y * PANEL3DMULY;
        matrix.translation.z = z;
        NuMtxMulVU0(&matrix, &matrix, NuCameraGetMtx());
        NuSpecialDrawAt(special, &matrix);
    }
    return 0;
}

/* Add to render.c. Native PC 552e00/78; cdecl (special,matrix,alpha). */
i32 __cdecl DrawPanel3DObjectMtx(NUSPECIALHANDLE *special, NUMTX *matrix, f32 alpha) {
    if (!(alpha <= 0.0f) && special != NULL && NuSpecialExistsFn(special) != 0) {
        NuMtxMulVU0(matrix, matrix, NuCameraGetMtx());
        NuSpecialDrawAtAlpha(special, matrix, alpha);
    }
    return 0;
}

i32 DrawPanel3DObjectMtxNoAlpha(nuhspecial_s *special, NUMTX *matrix) {
    if (special == NULL || NuSpecialExistsFn(special) == 0)
        return 0;
    NuMtxMulVU0(matrix, matrix, NuCameraGetMtx());
    return NuSpecialDrawAt(special, matrix);
}

void DrawObjectOnCharacter(WORLDINFO *world, GAMEOBJECT *object, i32 object_id, NUSPECIALHANDLE *special, i32 locator,
                           i32 second_locator, NUMTX *joints, i32 reflect, u32 layers, NUMTX *rotation,
                           NUVEC *translation, f32 alpha, f32 scale) {
    i32 position_only = dco_locatorposonly;
    u16 rotate_z;
    i32 id, axis, hat, use_alpha;
    f32 plane;
    CHARACTER_MODEL_LAYERS_PC *model;
    NUMTX matrix, reflected;
    dco_locatorposonly = 0;
    rotate_z = dco_prerotatez;
    dco_prerotatez = 0;
    if (object != NULL) {
        id = object->id;
        model = object->character_model;
        axis = object->reflection_enabled;
        plane = object->reflection_height;
        hat = object->helmet;
    } else {
        id = dco_id;
        if (id == -1 || dco_gcdata == NULL || dco_cmodel == NULL)
            return;
        model = dco_cmodel;
        axis = dco_reflectaxis;
        plane = dco_reflectcoord;
        hat = dco_wearinghat;
    }
    dco_gcdata = NULL;
    dco_cmodel = NULL;
    dco_id = -1;
    dco_reflectaxis = 0;
    dco_reflectcoord = 2000000.0f;
    dco_wearinghat = 0;
    if (world == NULL)
        world = WorldInfo_CurrentlyActive();
    if (object_id != -1) {
        if (!world->level_objects[object_id].active)
            return;
    } else if (special == NULL || !NuSpecialExistsFn(special)) {
        return;
    }
    if (locator == -1 || model->points_of_interest[locator] == NULL)
        return;
    if (position_only)
        NuMtxSetTranslation(&matrix, (NUVEC *)&joints[locator].m30);
    else
        matrix = joints[locator];
    if (second_locator != -1 && model->points_of_interest[second_locator] != NULL) {
        NUVEC position = {matrix.m30, matrix.m31, matrix.m32};
        if (!position_only) {
            NUMTX first = matrix;
            NUMTX second = joints[second_locator];
            first.m30 = first.m31 = first.m32 = 0.0f;
            second.m30 = second.m31 = second.m32 = 0.0f;
            QuatInterpolateRotationMatrix(&matrix, &first, &second, 0.5f);
        }
        matrix.m30 = (f64)(position.x + joints[second_locator].m30) * 0.5;
        matrix.m31 = (f64)(position.y + joints[second_locator].m31) * 0.5;
        matrix.m32 = (f64)(position.z + joints[second_locator].m32) * 0.5;
    }
    if (rotate_z != 0)
        NuMtxPreRotateZ(&matrix, rotate_z);
    if (object_id != -1 && CutSceneSys->object_id == object_id && DisguiseAdjustFn != NULL) {
        NUVEC adjustment, offset;
        DisguiseAdjustFn(id, hat, &adjustment, &offset);
        if (adjustment.x != 1.0f || adjustment.y != 1.0f || adjustment.z != 1.0f)
            NuMtxPreScale(&matrix, &adjustment);
        if (offset.x != 0.0f || offset.y != 0.0f || offset.z != 0.0f)
            NuMtxPreTranslate(&matrix, &offset);
    }
    if (rotation != NULL) {
        NUVEC position = {matrix.m30, matrix.m31, matrix.m32};
        NuMtxMulR(&matrix, rotation, &matrix);
        matrix.m30 = position.x;
        matrix.m31 = position.y;
        matrix.m32 = position.z;
    }
    if (translation != NULL)
        NuMtxTranslate(&matrix, translation);
    use_alpha = 0;
    if (object_id != -1) {
        special = &world->level_objects[object_id].special;
        if (ObjTabList != NULL)
            use_alpha = ObjTabList[object_id].unknown_01;
    }
    if (scale != 1.0f)
        NuMtxPreScaleU(&matrix, scale);
    if (use_alpha)
        NuSpecialDrawAtAlpha(special, &matrix, alpha);
    else
        NuSpecialDrawAt(special, &matrix);
    if (reflect && MatrixReflection(&matrix, axis, plane, world->current_level->reflection_y, &reflected)) {
        NuRndrStartReflectionRender(0);
        if (use_alpha)
            NuSpecialDrawAtAlpha(special, &reflected, alpha);
        else
            NuSpecialDrawAt(special, &reflected);
        NuRndrEndReflectionRender();
    }
}

void __cdecl CreateUsefulMaterials(void) {
    SolidMtl = NuMtlCreate(1);
    ((USEFUL_MATERIAL_PC *)SolidMtl)->attributes &= 0xffff3fffu;
    ((USEFUL_MATERIAL_PC *)SolidMtl)->attributes &= 0xfffffff0u;
    ((USEFUL_MATERIAL_PC *)SolidMtl)->attributes |= 0x40000u;
    NuMtlUpdate(SolidMtl);
    ZipUpLineMaterialPc = NuMtlCreate3D(1);
    ((USEFUL_MATERIAL_PC *)ZipUpLineMaterialPc)->attributes &= 0xffff3fffu;
    ((USEFUL_MATERIAL_PC *)ZipUpLineMaterialPc)->attributes &= 0xfffffff0u;
    ((USEFUL_MATERIAL_PC *)ZipUpLineMaterialPc)->attributes |= 0x40000u;
    NuMtlUpdate((NUMTL *)ZipUpLineMaterialPc);
}

void __cdecl IncreaseScore(u32 *total, u64 amount, i32 apply_multiplier) {
    if (apply_multiplier != 0 && BonusArea == 0) {
        if (Cheats_CheckFlags(4) != 0)
            amount *= 2;
        if (Cheats_CheckFlags(8) != 0)
            amount *= 4;
        if (Cheats_CheckFlags(0x10) != 0)
            amount *= 6;
        if (Cheats_CheckFlags(0x20) != 0)
            amount *= 8;
        if (Cheats_CheckFlags(0x40) != 0)
            amount *= 10;
    }
    amount += *total;
    if (amount > 4000000000ULL)
        amount = 4000000000ULL;
    *total = (u32)amount;
}

f32 __cdecl FindReflectionNoPlatforms(NUVEC *position) {
    f32 height;
    i32 material;
    NewTerrPlatformsOff();
    height = NewShadowEx(position, 0.0f, 5.0f, 5.0f, -1);
    if (height != GameObjectNearFloorNoFloorHeight) {
        material = ShadowInfo();
        if ((u32)material <= 31u && (TerSurface[material].flags & 2u) != 0)
            return height;
    }
    return LevelConfigFarDistance;
}

void __cdecl InitSplinePosition(SPLINEPOS *position, NUGSPLINE *spline, f32 distance, i32 looping) {
    NUVEC offset;
    NUVEC *first;
    NUVEC *second;

    if (position == NULL) {
        return;
    }
    memset(position, 0, sizeof(*position));
    if (spline == NULL || spline->length <= 1) {
        return;
    }
    position->spline = spline;
    position->looping = (i8)looping;
    first = SPLINE_POINT(spline, 0);
    second = SPLINE_POINT(spline, 1);
    position->segment_length = NuVecDist(second, first, &offset);
    if (distance > 0.0f) {
        MoveSplinePosition(position, distance);
    } else {
        position->position = *first;
        position->along = 0.0f;
    }
}

void __cdecl SetAnimFrame(struct nuhspecial_s *special, f32 frame) {
    NUMTX matrix;
    NUINSTANIM *instance_animation;
    f32 **animation;
    NUMTX *instance_matrix;
    f32 end_frame;
    if (NuSpecialExistsFn((NUSPECIALHANDLE *)special) == 0)
        return;
    ReflectionCopyMatrixPc(&matrix, (NUMTX *)&ANI3_EVAL_IDENTITY_MATRIX);
    instance_animation = NuSpecialGetInstAnim((NUSPECIALHANDLE *)special);
    if (instance_animation == NULL)
        return;
    animation = (f32 **)&special->scene->instance_animation_data[instance_animation->animation_index];
    if (*animation == NULL)
        return;
    end_frame = **animation;
    if ((f64)frame == 1000000000.0)
        frame = end_frame;
    if (!(frame >= 1.0f && frame <= end_frame))
        return;
    if (*animation != NULL)
        NuAnimData2CalcMatrix((ANI3_HEADER_PC *)*animation, 0, frame, &matrix);
    instance_animation->matrix = matrix;
    instance_matrix = NuSpecialGetInstanceMtx((NUSPECIALHANDLE *)special);
    instance_animation->matrix.m30 = instance_matrix->m30;
    instance_animation->matrix.m31 = instance_matrix->m31;
    instance_animation->matrix.m32 = instance_matrix->m32;
    instance_animation->ltime = frame;
}

static __inline f32 UtilitiesFabsPc(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

void __cdecl MakeThrowVector(NUVEC *result, NUVEC *origin, NUVEC *target, NUVEC *target_velocity, f32 speed,
                             f32 gravity) {
    NUVEC predicted = *target;
    NUVEC horizontal_origin;
    NUVEC delta;
    f32 time;
    f32 vertical_delta;
    f32 vertical_speed;
    u16 angle;
    i32 i;
    for (i = 0; i < 3; ++i) {
        horizontal_origin.x = origin->x;
        horizontal_origin.y = 0.0f;
        horizontal_origin.z = origin->z;
        NuVecSub(&delta, &predicted, &horizontal_origin);
        time = NuVecMag(&delta) / speed;
        NuVecAddScale(&predicted, target, target_velocity, time);
    }
    time = NuVecXZDist(&predicted, origin, NULL) / speed;
    vertical_delta = predicted.y - origin->y;
    vertical_speed = (f32)((f64)vertical_delta / ((f64)FRAMETIME + (f64)time) - (f64)gravity * 0.5 * (f64)time);
    angle = (u16)NuAtan2D(predicted.x - origin->x, predicted.z - origin->z);
    result->x = speed * NuTrigTable[angle >> 1];
    result->y = vertical_speed;
    result->z = speed * NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
}
