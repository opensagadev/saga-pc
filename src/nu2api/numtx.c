#include "saga.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

STATIC_ASSERT(offsetof(NUVEC, x) == 0x00);
STATIC_ASSERT(offsetof(NUVEC, y) == 0x04);
STATIC_ASSERT(offsetof(NUVEC, z) == 0x08);
STATIC_ASSERT(offsetof(NUMTX, m00) == 0x00);
STATIC_ASSERT(offsetof(NUMTX, m01) == 0x04);
STATIC_ASSERT(offsetof(NUMTX, m02) == 0x08);
STATIC_ASSERT(offsetof(NUMTX, m03) == 0x0c);
STATIC_ASSERT(offsetof(NUMTX, m10) == 0x10);
STATIC_ASSERT(offsetof(NUMTX, m11) == 0x14);
STATIC_ASSERT(offsetof(NUMTX, m12) == 0x18);
STATIC_ASSERT(offsetof(NUMTX, m13) == 0x1c);
STATIC_ASSERT(offsetof(NUMTX, m20) == 0x20);
STATIC_ASSERT(offsetof(NUMTX, m21) == 0x24);
STATIC_ASSERT(offsetof(NUMTX, m22) == 0x28);
STATIC_ASSERT(offsetof(NUMTX, m23) == 0x2c);
STATIC_ASSERT(offsetof(NUMTX, m30) == 0x30);
STATIC_ASSERT(offsetof(NUMTX, m31) == 0x34);
STATIC_ASSERT(offsetof(NUMTX, m32) == 0x38);
STATIC_ASSERT(offsetof(NUMTX, m33) == 0x3c);

void __cdecl NuMtxMulVU0(NUMTX *result, NUMTX *left, NUMTX *right) {
    D3DXMatrixMultiply((D3DXMATRIX *)result, (D3DXMATRIX *)left, (D3DXMATRIX *)right);
}

void __cdecl NuVecConvertToIntVU0(void) {
}

i32 __cdecl NuPlnLineVU0(NUPLANE *plane, NUVEC *start, NUVEC *end, NUVEC *out) {
    return NuPlnLine(plane, start, end, out);
}

/* The PS2 VU0 clip test has no PC body; every point reports as unclipped. */
i32 __cdecl NuVecClipTestPointVU0(NUVEC *point, NUMTX *matrix) {
    (void)point;
    (void)matrix;
    return 0;
}

void __cdecl NuMtxSetRotateXYZVU0(NUMTX *matrix, NUANGVEC *angles) {
    NuMtxSetRotateXYZ(matrix, angles);
}

void __cdecl NuMtxPreScaleVU0(NUMTX *matrix, NUVEC *scale) {
    NuMtxPreScale(matrix, scale);
}

void __cdecl NuMtxInvVU0(NUMTX *m, NUMTX *source) {
    f32 tx = -source->m30;
    f32 ty = -source->m31;
    f32 tz = -source->m32;
    f32 t;

    t = source->m01;
    m->m01 = source->m10;
    m->m10 = t;
    t = source->m02;
    m->m02 = source->m20;
    m->m20 = t;
    t = source->m12;
    m->m12 = source->m21;
    m->m21 = t;
    m->m00 = source->m00;
    m->m11 = source->m11;
    m->m22 = source->m22;
    m->m30 = m->m00 * tx + m->m10 * ty + m->m20 * tz;
    m->m31 = m->m01 * tx + m->m11 * ty + m->m21 * tz;
    m->m32 = m->m02 * tx + m->m12 * ty + m->m22 * tz;
    m->m23 = 0.0f;
    m->m13 = m->m23;
    m->m03 = m->m13;
    m->m33 = 1.0f;
}

f32 __cdecl NuVecMagVU0(NUVEC *v) {
    return (f32)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

void __cdecl NuMtxScaleVU0(NUMTX *matrix, NUVEC *scale) {
    NuMtxScale(matrix, scale);
}

void __cdecl NuMtxMulH(NUMTX *result, NUMTX *left, NUMTX *right) {
    D3DXMatrixMultiply((D3DXMATRIX *)result, (D3DXMATRIX *)left, (D3DXMATRIX *)right);
}

void __cdecl NuMtxMulRVU0(NUMTX *result, NUMTX *left, NUMTX *right) {
    NuMtxMulR(result, left, right);
}

void __cdecl NuMtxPreScaleUVU0(NUMTX *matrix, f32 scale) {
    matrix->m00 *= scale;
    matrix->m01 *= scale;
    matrix->m02 *= scale;
    matrix->m10 *= scale;
    matrix->m11 *= scale;
    matrix->m12 *= scale;
    matrix->m20 *= scale;
    matrix->m21 *= scale;
    matrix->m22 *= scale;
}

void __cdecl NuMtxSetRotationXYVU0(NUMTX *matrix, NUANGVEC *angles) {
    f32 cx = NuTrigTable[((angles->x + 0x4000) >> 1) & 0x7fff];
    f32 sx = NuTrigTable[(angles->x >> 1) & 0x7fff];
    f32 cy = NuTrigTable[((angles->y + 0x4000) >> 1) & 0x7fff];
    f32 sy = NuTrigTable[(angles->y >> 1) & 0x7fff];
    f32 cz = NuTrigTable[0x2000], sz = NuTrigTable[0];
    matrix->m00 = cy * cz;
    matrix->m01 = cy * sz;
    matrix->m02 = -sy;
    matrix->m03 = 0.0f;
    matrix->m10 = sx * sy * cz - cx * sz;
    matrix->m11 = sx * sy * sz + cx * cz;
    matrix->m12 = sx * cy;
    matrix->m13 = 0.0f;
    matrix->m20 = cx * sy * cz + sx * sz;
    matrix->m21 = cx * sy * sz - sx * cz;
    matrix->m22 = cx * cy;
    matrix->m23 = 0.0f;
    matrix->m30 = 0.0f;
    matrix->m31 = 0.0f;
    matrix->m32 = 0.0f;
    matrix->m33 = 1.0f;
}

void __cdecl NuMtxMulnVU0(NUMTX *out, NUMTX *left, NUMTX **right) {
    NuMtxMul(out, left, *right);
}

f32 __cdecl randyfloat(void) {
    return rand() / 32767.0;
}

void __cdecl NuVecInvMtxTransformVU0(NUVEC *out, NUVEC *v, NUMTX *m) {
    f32 x = v->x - m->m30;
    f32 y = v->y - m->m31;
    f32 z = v->z - m->m32;
    out->x = m->m00 * x + m->m01 * y + m->m02 * z;
    out->y = m->m10 * x + m->m11 * y + m->m12 * z;
    out->z = m->m20 * x + m->m21 * y + m->m22 * z;
}

static NUVEC tempvec = {0};
static NUVEC4 tempvec4 = {0};

NUVEC *__cdecl makenuvec(f32 x, f32 y, f32 z) {
    tempvec.x = x;
    tempvec.y = y;
    tempvec.z = z;
    return &tempvec;
}

NUVEC4 *__cdecl makenuvec4(f32 x, f32 y, f32 z, f32 w) {
    tempvec4.x = x;
    tempvec4.y = y;
    tempvec4.z = z;
    tempvec4.w = w;
    return &tempvec4;
}

u32 __cdecl NuVecToRGBA(NUVEC *v, f32 alpha) {
    u32 red = (v->x + 1.0f) * 127.5f;
    u32 green = (v->y + 1.0f) * 127.5f;
    u32 blue = (v->z + 1.0f) * 127.5f;
    u32 opacity = alpha * 255.0f;
    return (opacity << 24) + (red << 16) + (green << 8) + blue;
}

f32 __cdecl NuVecNormVU0(NUVEC *out, NUVEC *v) {
    f32 magnitude = (f32)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    NUVEC normalized;
    if (magnitude != 0.0f) {
        normalized.x = v->x / magnitude;
        normalized.y = v->y / magnitude;
        normalized.z = v->z / magnitude;
    } else {
        normalized.x = 0.0f;
        normalized.y = 0.0f;
        normalized.z = 0.0f;
    }
    out->x = normalized.x;
    out->y = normalized.y;
    out->z = normalized.z;
    return magnitude;
}

void __cdecl NuVec4MtxInvTransformVU0(NUVEC4 *out, NUVEC4 *v, NUMTX *matrix) {
    NUMTX inverse;
    NuMtxInv(&inverse, matrix);
    NuVec4MtxTransformVU0(out, v, &inverse);
}

void __cdecl NuVec4MtxInvRotVU0(NUVEC4 *out, NUVEC4 *v, NUMTX *m) {
    f32 y = v->x * m->m10 + v->y * m->m11 + v->z * m->m12;
    f32 z = v->x * m->m20 + v->y * m->m21 + v->z * m->m22;
    f32 w = v->x * m->m30 + v->y * m->m31 + v->z * m->m32;
    out->x = v->x * m->m00 + v->y * m->m01 + v->z * m->m02;
    out->y = y;
    out->z = z;
    out->w = w;
}

f32 __cdecl NuVecDiffVU0(NUVEC *a, NUVEC *b) {
    f32 x = a->x - b->x;
    f32 y = a->y - b->y;
    f32 z = a->z - b->z;
    f32 squared = x * x + y * y + z * z;
    return NuFsqrt(squared);
}

f32 __cdecl NuVecDiffSqrVU0(NUVEC *a, NUVEC *b) {
    f32 x = a->x - b->x;
    f32 y = a->y - b->y;
    f32 z = a->z - b->z;
    f32 squared = x * x + y * y + z * z;
    return squared;
}

f32 __cdecl NuASin_Accurate(f32 value) {
    if (value >= 0.0f) {
        if (value >= 1.0f) {
            return 1.57079637f;
        }
        return 1.5707963705062866 - NuFsqrt(1.0f - value) * (1.5707287788391113 - 0.2121143937110901 * value +
                                                             0.07426100224256516 * value * value -
                                                             0.018729299306869507 * value * value * value);
    }
    if (value <= -1.0f) {
        return -1.57079637f;
    }
    return NuFsqrt(value + 1.0) * (1.5707287788391113 + 0.2121143937110901 * value +
                                   0.07426100224256516 * value * value + 0.018729299306869507 * value * value * value) -
           1.5707963705062866;
}

f32 __cdecl NuSin_Accurate(f32 angle) {
    i32 index;
    f32 low;
    f32 high;
    f32 fraction;
    if (angle < 0.0f) {
        angle += 6.2831854820251465;
    }
    angle *= 5215.18896484375;
    index = (i32)angle;
    low = NuTrigTable[index];
    high = NuTrigTable[(index + 1) & 0x7fff];
    fraction = angle - index;
    return low + (high - low) * fraction;
}

void __cdecl NuQuatLerp2(NUQUAT *out, NUQUAT *from, NUQUAT *to, f32 t) {
    f32 dot = to->x * from->x + to->y * from->y + to->z * from->z + to->w * from->w;
    if (dot < 0.0f) {
        out->x = (to->x + from->x) * t - from->x;
        out->y = (to->y + from->y) * t - from->y;
        out->z = (to->z + from->z) * t - from->z;
        out->w = (to->w + from->w) * t - from->w;
    } else {
        out->x = (to->x - from->x) * t + from->x;
        out->y = (to->y - from->y) * t + from->y;
        out->z = (to->z - from->z) * t + from->z;
        out->w = (to->w - from->w) * t + from->w;
    }
}

void __cdecl NuQuatSlerp_Accurate(NUQUAT *out, NUQUAT *from, NUQUAT *to, f32 t) {
    f32 scale;
    f32 from_factor;
    f32 to_factor;
    f32 omega;
    f32 sin_omega;
    NUQUAT to_prime;
    scale = from->x * to->x + from->y * to->y + from->z * to->z + from->w * to->w;
    if (scale < 0.0f) {
        scale = -scale;
        to_prime.x = -to->x;
        to_prime.y = -to->y;
        to_prime.z = -to->z;
        to_prime.w = -to->w;
    } else {
        to_prime.x = to->x;
        to_prime.y = to->y;
        to_prime.z = to->z;
        to_prime.w = to->w;
    }
    if (1.0f - scale > 0.0f) {
        omega = 1.5707963705062866 - NuASin_Accurate(scale);
        sin_omega = NuSin_Accurate(omega);
        from_factor = NuSin_Accurate((1.0f - t) * omega) / sin_omega;
        to_factor = NuSin_Accurate(t * omega) / sin_omega;
    } else {
        from_factor = 1.0f - t;
        to_factor = t;
    }
    out->x = from->x * from_factor + to_prime.x * to_factor;
    out->y = from->y * from_factor + to_prime.y * to_factor;
    out->z = from->z * from_factor + to_prime.z * to_factor;
    out->w = from->w * from_factor + to_prime.w * to_factor;
}
