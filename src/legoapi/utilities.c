#include "saga.h"
#include <stdlib.h>

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

i32 __cdecl LineIntersectXY(NUVEC *a, NUVEC *b, NUVEC *c, NUVEC *d, NUVEC *first, NUVEC *second) {
    f32 ax = a->x;
    f32 ay = a->y;
    f32 dx = b->x - ax;
    f32 dy = b->y - ay;
    f32 ex = d->x - c->x;
    f32 ey = d->y - c->y;
    f32 denominator = ey * dx - ex * dy;
    f32 ox = ax - c->x;
    f32 oy = ay - c->y;
    f32 second_numerator = oy * dx - ox * dy;
    f32 t;
    f32 u;

    if (denominator == 0.0f) {
        return 0;
    }
    t = (ex * oy - ey * ox) / denominator;
    u = second_numerator / denominator;
    if (first != NULL) {
        first->x = dx * t + ax;
        first->y = dy * t + ay;
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
    if (NuFsign(ca.x) == NuFsign(da.x))
        return 0;

    NuVecSub(&direction, d, c);
    angle = NuAtan2D(direction.x, direction.z);
    NuVecSub(&ac, a, c);
    NuVecRotateY(&ac, &ac, -angle);
    NuVecSub(&bc, b, c);
    NuVecRotateY(&bc, &bc, -angle);
    if (NuFsign(ac.x) == NuFsign(bc.x))
        return 0;

    if (first != NULL)
        *first = fabsf(ac.x) / (fabsf(ac.x) + fabsf(bc.x));
    if (second != NULL)
        *second = fabsf(ca.x) / (fabsf(ca.x) + fabsf(da.x));
    return 1;
}

f32 XZLinesClosest(NUVEC *a, NUVEC *b, NUVEC *c, NUVEC *d, f32 *first, f32 *second) {
    i32 angle;
    f32 ab_length;
    f32 cd_length;
    f32 closest;
    f32 distance;
    f32 fraction;
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
    NuVecSub(&direction, d, c);
    angle = NuAtan2D(direction.x, direction.z);
    NuVecSub(&ac, a, c);
    NuVecRotateY(&ac, &ac, -angle);
    NuVecSub(&bc, b, c);
    NuVecRotateY(&bc, &bc, -angle);

    if (NuFsign(ca.x) != NuFsign(da.x) && NuFsign(ac.x) != NuFsign(bc.x)) {
        *first = fabsf(ac.x) / (fabsf(ac.x) + fabsf(bc.x));
        *second = fabsf(ca.x) / (fabsf(ca.x) + fabsf(da.x));
        return 0.0f;
    }

    ab_length = NuVecXZDist(b, a, NULL);
    cd_length = NuVecXZDist(d, c, NULL);
    closest = 1000000000.0f;

    if (ca.z < 0.0f) {
        distance = NuVecXZDist(a, &ca, NULL);
        fraction = 0.0f;
    } else if (ca.z > ab_length) {
        distance = NuVecXZDist(b, &ca, NULL);
        fraction = 1.0f;
    } else {
        fraction = ca.z / ab_length;
        distance = fabsf(ca.x);
    }
    if (distance < closest) {
        *first = fraction;
        *second = 0.0f;
        closest = distance;
    }
    if (da.z < 0.0f) {
        distance = NuVecXZDist(a, &da, NULL);
        fraction = 0.0f;
    } else if (da.z > ab_length) {
        distance = NuVecXZDist(b, &da, NULL);
        fraction = 1.0f;
    } else {
        fraction = da.z / ab_length;
        distance = fabsf(da.x);
    }
    if (distance < closest) {
        *first = fraction;
        *second = 1.0f;
        closest = distance;
    }
    if (!(ac.z < 0.0f)) {
        if (ac.z > cd_length) {
            distance = NuVecXZDist(d, &ac, NULL);
            fraction = 1.0f;
        } else {
            fraction = ac.z / cd_length;
            distance = fabsf(ac.x);
        }
    } else {
        distance = NuVecXZDist(c, &ac, NULL);
        fraction = 0.0f;
    }
    if (distance < closest) {
        *second = fraction;
        *first = 0.0f;
        closest = distance;
    }
    if (!(bc.z < 0.0f)) {
        if (bc.z > cd_length) {
            distance = NuVecXZDist(d, &bc, NULL);
            fraction = 1.0f;
        } else {
            fraction = bc.z / cd_length;
            distance = fabsf(bc.x);
        }
    } else {
        distance = NuVecXZDist(c, &bc, NULL);
        fraction = 0.0f;
    }
    if (distance < closest) {
        *second = fraction;
        *first = 1.0f;
        closest = distance;
    }
    return closest;
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
        NuVecAddScale(&predicted, target, target_velocity, NuVecMag(&delta) / speed);
    }
    time = NuVecXZDist(&predicted, origin, NULL) / speed;
    vertical_delta = predicted.y - origin->y;
    vertical_speed = (f32)((f64)vertical_delta / ((f64)FRAMETIME + (f64)time) - (f64)gravity * 0.5 * (f64)time);
    angle = (u16)NuAtan2D(predicted.x - origin->x, predicted.z - origin->z);
    result->x = speed * NuTrigTable[angle >> 1];
    result->y = vertical_speed;
    result->z = speed * NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
}

void __cdecl GetRotationAngles(NUVEC *direction, u16 *x_rotation, u16 *y_rotation) {
    NUVEC rotated, copy = *direction;
    u16 y_angle = (u16)-NuAtan2D(copy.z, copy.x);
    NuVecRotateY(&rotated, &copy, -(i32)y_angle);
    *x_rotation = (u16)-NuAtan2D(rotated.x, rotated.y);
    *y_rotation = y_angle;
}

extern i32(__cdecl *MatrixReflection_CanOverrideFn)(void);
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

void InterpolateRotationMatrix(NUMTX *result, NUMTX *first, NUMTX *second, float fraction) {
    SeekVec(NUMTX_GET_ROW_VEC(result, 0), NUMTX_GET_ROW_VEC(first, 0), NUMTX_GET_ROW_VEC(second, 0), fraction);
    SeekVec(NUMTX_GET_ROW_VEC(result, 1), NUMTX_GET_ROW_VEC(first, 1), NUMTX_GET_ROW_VEC(second, 1), fraction);
    SeekVec(NUMTX_GET_ROW_VEC(result, 2), NUMTX_GET_ROW_VEC(first, 2), NUMTX_GET_ROW_VEC(second, 2), fraction);
    NuVecNorm(NUMTX_GET_ROW_VEC(result, 0), NUMTX_GET_ROW_VEC(result, 0));
    NuVecNorm(NUMTX_GET_ROW_VEC(result, 1), NUMTX_GET_ROW_VEC(result, 1));
    NuVecNorm(NUMTX_GET_ROW_VEC(result, 2), NUMTX_GET_ROW_VEC(result, 2));
}

void __cdecl FindAnglesXY(NUVEC *direction, u16 *x_rotation, u16 *y_rotation) {
    f32 horizontal_squared;
    ConfigBuffer.fields.find_angles_y_rotation = (u16)NuAtan2D(direction->x, direction->z);
    if (y_rotation)
        *y_rotation = ConfigBuffer.fields.find_angles_y_rotation;
    horizontal_squared = direction->x * direction->x + direction->z * direction->z;
    FindAnglesXFullTempPc = -NuAtan2D(direction->y, NuFsqrt(horizontal_squared));
    temp_zrot = (u16)FindAnglesXFullTempPc;
    if (x_rotation)
        *x_rotation = temp_zrot;
}
i32 __cdecl SphereSphereOverlap(NUVEC *a, f32 radius_a, NUVEC *b, f32 radius_b) {
    f32 x = b->x - a->x;
    f32 y = b->y - a->y;
    f32 z = b->z - a->z;
    f32 radius = radius_a + radius_b;
    return x * x + y * y + z * z <= radius * radius;
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

void __cdecl VecRotateAxis(NUVEC *vector, u16 angle, NUVEC *axis) {
    f32 cosine, sine, partial, x_result, y_result, z_result;
    f64 complement, tx, ty, tz, xy, xz, yz, sx, sy, sz;
    NUVEC input;
    NuVecNorm(axis, axis);
    input = *vector;
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
    partial = (f32)(((tx * (f64)axis->x + (f64)cosine) * (f64)input.x) + 0.0);
    partial = (f32)((f64)partial + (xy - sz) * (f64)input.y);
    x_result = (f32)((f64)partial + (xz + sy) * (f64)input.z);
    partial = (f32)(((xy + sz) * (f64)input.x) + 0.0);
    partial = (f32)((f64)partial + (ty * (f64)axis->y + (f64)cosine) * (f64)input.y);
    y_result = (f32)((f64)partial + (yz - sx) * (f64)input.z);
    partial = (f32)(((xz - sy) * (f64)input.x) + 0.0);
    partial = (f32)((f64)partial + (sx + yz) * (f64)input.y);
    z_result = (f32)((f64)partial + (tz * (f64)axis->z + (f64)cosine) * (f64)input.z);
    vector->x = x_result;
    vector->y = y_result;
    vector->z = z_result;
}

i32 __cdecl OnOrInsidePlane(NUVEC *point, NUVEC *plane_point, NUVEC *plane_normal, NUVEC *corrected_point,
                            f32 normal_offset, f32 *distance_out) {
    NUVEC test_point;
    f32 distance, correction;
    if (normal_offset != 0.0f) {
        NuVecScale(&test_point, plane_normal, normal_offset);
        NuVecAdd(&test_point, &test_point, point);
    } else
        test_point = *point;
    distance = (f32)(((f64)test_point.x - (f64)plane_point->x) * (f64)plane_normal->x +
                     ((f64)test_point.y - (f64)plane_point->y) * (f64)plane_normal->y +
                     ((f64)test_point.z - (f64)plane_point->z) * (f64)plane_normal->z);
    if (distance_out != NULL)
        *distance_out = distance;
    if (!(distance <= 0.0f))
        return 0;
    if (corrected_point != NULL) {
        correction = (f32)(0.5 - (f64)distance);
        NuVecScale(corrected_point, plane_normal, correction);
        NuVecAdd(corrected_point, corrected_point, point);
    }
    return 1;
}
