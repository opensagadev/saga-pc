#include "saga.h"
#include <string.h>

STATIC_ASSERT(offsetof(NUVEC4, x) == 0);
STATIC_ASSERT(offsetof(NUVEC4, y) == 4);
STATIC_ASSERT(offsetof(NUVEC4, z) == 8);

void __cdecl NuVec4MtxTransformH(NUVEC4 *out, NUVEC4 *input, NUMTX *matrix) {
    f32 y = input->x * matrix->m01 + input->y * matrix->m11 + input->z * matrix->m21 + input->w * matrix->m31;
    f32 z = input->x * matrix->m02 + input->y * matrix->m12 + input->z * matrix->m22 + input->w * matrix->m32;
    f32 w = input->x * matrix->m03 + input->y * matrix->m13 + input->z * matrix->m23 + input->w * matrix->m33;
    out->x = input->x * matrix->m00 + input->y * matrix->m10 + input->z * matrix->m20 + input->w * matrix->m30;
    out->y = y;
    out->z = z;
    out->w = w;
}

void __cdecl NuVec4Add(NUVEC4 *out, NUVEC4 *left, NUVEC4 *right) {
    out->w = left->w + right->w;
    out->x = left->x + right->x;
    out->y = left->y + right->y;
    out->z = left->z + right->z;
}

i16 __cdecl NuFloatToHalf(f32 value) {
    union {
        f32 value;
        u32 bits;
    } packed;
    i32 exponent, mantissa, sign;
    memcpy(&packed.bits, &value, 4);
    sign = packed.bits >> 31;
    exponent = (packed.bits >> 23) & 0xff;
    mantissa = packed.bits & 0x7fffff;
    exponent -= 112;
    if (exponent < 0)
        exponent = 0;
    if (exponent > 31)
        exponent = 31;
    return (i16)((sign << 15) | ((exponent & 31) << 10) | (mantissa >> 13));
}
f32 __cdecl NuHalfToFloat(i16 value) {
    u16 bits = (u16)value;
    union {
        u32 bits;
        f32 value;
    } packed;
    if (bits == 0)
        return 0.0f;
    packed.bits = ((u32)(bits >> 15) << 31) | ((u32)(((bits >> 10) & 31) + 112) << 23) | ((u32)(bits & 0x3ff) << 13);
    return packed.value;
}
f32 __cdecl NuVec4Dot(NUVEC4 *v0, NUVEC4 *v1) {
    return v0->w * v1->w + v0->x * v1->x + v0->y * v1->y + v0->z * v1->z;
}

void __cdecl NuVec4ScaleAccum(NUVEC4 *out, NUVEC4 *v, f32 scale) {
    out->x += v->x * scale;
    out->y += v->y * scale;
    out->z += v->z * scale;
    out->w += v->w * scale;
}

void __cdecl NuVec4Sub(NUVEC4 *out, NUVEC4 *a, NUVEC4 *b) {
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
    out->w = a->w - b->w;
}

void __cdecl NuVec4Max(NUVEC4 *out, NUVEC4 *a, NUVEC4 *b) {
    if (a->x > b->x)
        out->x = a->x;
    else
        out->x = b->x;
    if (a->y > b->y)
        out->y = a->y;
    else
        out->y = b->y;
    if (a->z > b->z)
        out->z = a->z;
    else
        out->z = b->z;
    if (a->w > b->w)
        out->w = a->w;
    else
        out->w = b->w;
}

void __cdecl NuVec4Min(NUVEC4 *out, NUVEC4 *a, NUVEC4 *b) {
    if (a->x < b->x)
        out->x = a->x;
    else
        out->x = b->x;
    if (a->y < b->y)
        out->y = a->y;
    else
        out->y = b->y;
    if (a->z < b->z)
        out->z = a->z;
    else
        out->z = b->z;
    if (a->w < b->w)
        out->w = a->w;
    else
        out->w = b->w;
}

f32 __cdecl NuVec4Mag(NUVEC4 *v) {
    return NuFsqrt(v->w * v->w + v->x * v->x + v->y * v->y + v->z * v->z);
}

f32 __cdecl NuVec4MagSqr(NUVEC4 *v) {
    return v->w * v->w + v->x * v->x + v->y * v->y + v->z * v->z;
}

void __cdecl NuVec4Lerp(NUVEC4 *out, NUVEC4 *a, NUVEC4 *b, f32 t) {
    out->x = b->x + (a->x - b->x) * t;
    out->y = b->y + (a->y - b->y) * t;
    out->z = b->z + (a->z - b->z) * t;
    out->w = b->w + (a->w - b->w) * t;
}

void __cdecl NuVec4ToNuHalfVec4(NUVEC4 *v, NUHALFVEC4 *out) {
    out->x = NuFloatToHalf(v->x);
    out->y = NuFloatToHalf(v->y);
    out->z = NuFloatToHalf(v->z);
    out->w = NuFloatToHalf(v->w);
}

void __cdecl NuHalfVec4ToNuVec4(NUHALFVEC4 *v, NUVEC4 *out) {
    out->x = NuHalfToFloat(v->x);
    out->y = NuHalfToFloat(v->y);
    out->z = NuHalfToFloat(v->z);
    out->w = NuHalfToFloat(v->w);
}
