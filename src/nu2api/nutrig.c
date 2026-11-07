#include "saga.h"

#include <float.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PI_OVER_4 0.785398f
#define NEG_1_OVER_6 PCPathArcSineCoefficient3
#define NEG_3_OVER_40 PCPathArcSineCoefficient5
#define NEG_5_OVER_112 PCPathArcSineCoefficient7
#define NEG_35_OVER_1152 PCPathArcSineCoefficient9
#define MAX_SHORT_OVER_PI 10430.4f
#define POW2(x) ((x) * (x))
#define POW3(x) ((x) * POW2(x))
#define POW4(x) (POW2(x) * POW2(x))
#define POW5(x) (POW2(x) * POW3(x))

i16 __cdecl PCPathArcSine(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 unknown_a;
    f32 unknown_b;
    f32 unknown_c;
    f32 unknown_d;

    union {
        f32 value;
        u32 bits;
    } magnitude, absolute;
    magnitude.value = sin;
    absolute.bits = magnitude.bits & 0x7fffffff;
    abs = absolute.value;
    sqrt = NuFsqrt(1.0f - sin * sin);

    unknown_a = MIN(sqrt, abs);

    unknown_b = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    unknown_c = MIN(sin * 3.40282e+38f, 1.0f);
    unknown_c = MAX(unknown_c, -1.0f);

    unknown_d = unknown_b * unknown_c + unknown_c;

    return (NEG_3_OVER_40 * POW2(unknown_b * unknown_c * unknown_a) * POW3(unknown_b * unknown_c * unknown_a) +
            (unknown_d * PI_OVER_4 - (unknown_b * unknown_c * unknown_a) +
             NEG_1_OVER_6 * (unknown_b * unknown_c * unknown_a) * POW2(unknown_b * unknown_c * unknown_a)) +
            NEG_5_OVER_112 * POW3(unknown_b * unknown_c * unknown_a) * POW4(unknown_b * unknown_c * unknown_a) +
            NEG_35_OVER_1152 * POW4(unknown_b * unknown_c * unknown_a) * POW5(unknown_b * unknown_c * unknown_a)) *
           MAX_SHORT_OVER_PI;
}

void AiRndrLine3d(NURND_VERTEX3D_PC *vertices, NUMTL *unused1, NUMTX *matrix) {
    NUVEC first = vertices[0].position;
    NUVEC second = vertices[1].position;
    NUMTX matrix_copy;
    if (matrix != NULL) {
        if (((size_t)(matrix) & 15) != 0) {
            matrix_copy = *matrix;
            matrix = &matrix_copy;
        }
        NuVecMtxTransform(&first, &first, matrix);
        NuVecMtxTransform(&second, &second, matrix);
    }
    if (AiRndrCullDistance > 0.0f) {
        f32 dx;
        f32 dy;
        f32 dz;
        f32 limit;
        f32 dx2;
        f32 dy2;
        f32 dz2;
        __declspec(align(16)) NUVEC position;
        if (aieditor && (aieditor->flags & 2))
            position = aieditor->camera_position;
        else
            position = *(NUVEC *)&global_camera.mtx.m30;
        dx = position.x - first.x;
        dy = position.y - first.y;
        dz = position.z - first.z;
        limit = AiRndrCullDistance * AiRndrCullDistance;
        dx2 = position.x - second.x;
        dy2 = position.y - second.y;
        dz2 = position.z - second.z;
        if (dx * dx + dy * dy + dz * dz > limit && dx2 * dx2 + dy2 * dy2 + dz2 * dz2 > limit)
            return;
    }
    NuRndrLine3dDbg(first.x, first.y, first.z, second.x, second.y, second.z, vertices[0].colour);
}

void AiRndrLine3dDbg(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, u32 colour) {
    if (AiRndrCullDistance > 0.0f) {
        f32 dx;
        f32 dy;
        f32 dz;
        f32 limit;
        __declspec(align(16)) NUVEC position;
        if (aieditor && (aieditor->flags & 2))
            position = aieditor->camera_position;
        else
            position = *(NUVEC *)&global_camera.mtx.m30;
        dx = position.x - x0;
        dy = position.y - y0;
        dz = position.z - z0;
        limit = AiRndrCullDistance * AiRndrCullDistance;
        if (!(dx * dx + dy * dy + dz * dz < limit)) {
            dx = position.x - x1;
            dy = position.y - y1;
            dz = position.z - z1;
            if (!(dx * dx + dy * dy + dz * dz < limit))
                return;
        }
    }
    NuRndrLine3dDbg(x0, y0, z0, x1, y1, z1, colour);
}

i32 __cdecl RenderPostASinPc(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 unknown_a;
    f32 unknown_b;
    f32 unknown_c;
    f32 unknown_d;

    union {
        f32 value;
        u32 bits;
    } magnitude, absolute;
    magnitude.value = sin;
    absolute.bits = magnitude.bits & 0x7fffffff;
    abs = absolute.value;
    sqrt = NuFsqrt(1.0f - sin * sin);

    unknown_a = MIN(sqrt, abs);

    unknown_b = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    unknown_c = MIN(sin * 3.40282e+38f, 1.0f);
    unknown_c = MAX(unknown_c, -1.0f);

    unknown_d = unknown_b * unknown_c + unknown_c;

    return (NEG_3_OVER_40 * POW2(unknown_b * unknown_c * unknown_a) * POW3(unknown_b * unknown_c * unknown_a) +
            (unknown_d * PI_OVER_4 - (unknown_b * unknown_c * unknown_a) +
             NEG_1_OVER_6 * (unknown_b * unknown_c * unknown_a) * POW2(unknown_b * unknown_c * unknown_a)) +
            NEG_5_OVER_112 * POW3(unknown_b * unknown_c * unknown_a) * POW4(unknown_b * unknown_c * unknown_a) +
            NEG_35_OVER_1152 * POW4(unknown_b * unknown_c * unknown_a) * POW5(unknown_b * unknown_c * unknown_a)) *
           MAX_SHORT_OVER_PI;
}

#undef MIN
#undef MAX
#undef PI_OVER_4
#undef NEG_1_OVER_6
#undef NEG_3_OVER_40
#undef NEG_5_OVER_112
#undef NEG_35_OVER_1152
#undef MAX_SHORT_OVER_PI
#undef POW2
#undef POW3
#undef POW4
#undef POW5
