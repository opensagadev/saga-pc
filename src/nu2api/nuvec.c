#include "saga.h"
#include <xmmintrin.h>

STATIC_ASSERT(offsetof(NUVEC, x) == 0x00);
STATIC_ASSERT(offsetof(NUVEC, y) == 0x04);
STATIC_ASSERT(offsetof(NUVEC, z) == 0x08);
STATIC_ASSERT(offsetof(NUMTX, m00) == 0x00);
STATIC_ASSERT(offsetof(NUMTX, m01) == 0x04);
STATIC_ASSERT(offsetof(NUMTX, m02) == 0x08);
STATIC_ASSERT(offsetof(NUMTX, m10) == 0x10);
STATIC_ASSERT(offsetof(NUMTX, m11) == 0x14);
STATIC_ASSERT(offsetof(NUMTX, m12) == 0x18);
STATIC_ASSERT(offsetof(NUMTX, m20) == 0x20);
STATIC_ASSERT(offsetof(NUMTX, m21) == 0x24);
STATIC_ASSERT(offsetof(NUMTX, m22) == 0x28);
STATIC_ASSERT(offsetof(NUMTX, m30) == 0x30);
STATIC_ASSERT(offsetof(NUMTX, m31) == 0x34);
STATIC_ASSERT(offsetof(NUMTX, m32) == 0x38);

void __cdecl NuVecAdd(NUVEC *out, NUVEC *left, NUVEC *right) {
    out->x = left->x + right->x;
    out->y = left->y + right->y;
    out->z = left->z + right->z;
}

void __cdecl NuVecSub(NUVEC *out, NUVEC *left, NUVEC *right) {
    out->x = left->x - right->x;
    out->y = left->y - right->y;
    out->z = left->z - right->z;
}

void __cdecl NuVecScale(NUVEC *out, NUVEC *vector, f32 scale) {
    out->x = vector->x * scale;
    out->y = vector->y * scale;
    out->z = vector->z * scale;
}

void __cdecl NuVecAddScale(NUVEC *out, NUVEC *left, NUVEC *right, f32 scale) {
    out->x = left->x + right->x * scale;
    out->y = left->y + right->y * scale;
    out->z = left->z + right->z * scale;
}

void __cdecl NuVecScaleAccum(NUVEC *out, NUVEC *vector, f32 scale) {
    out->x += vector->x * scale;
    out->y += vector->y * scale;
    out->z += vector->z * scale;
}

void __cdecl NuVecInvScale(NUVEC *out, NUVEC *vector, f32 scale) {
    f32 inverse_scale;

    if (scale == 0.0f) {
        inverse_scale = 0.0f;
    } else {
        inverse_scale = 1.0f / scale;
    }

    out->x = vector->x * inverse_scale;
    out->y = vector->y * inverse_scale;
    out->z = vector->z * inverse_scale;
}

void __cdecl NuVecRotateX(NUVEC *out, NUVEC *vector, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 y = vector->y;

    out->x = vector->x;
    out->y = y * cosine - vector->z * sine;
    out->z = y * sine + vector->z * cosine;
}

void __cdecl NuVecRotateY(NUVEC *out, NUVEC *vector, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 x = vector->x;

    out->x = x * cosine + vector->z * sine;
    out->y = vector->y;
    out->z = vector->z * cosine - x * sine;
}

void __cdecl NuVecRotateZ(NUVEC *out, NUVEC *vector, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 x = vector->x;

    out->x = x * cosine - vector->y * sine;
    out->y = x * sine + vector->y * cosine;
    out->z = vector->z;
}

void __cdecl NuVecRotateYValZ(NUVEC *out, f32 magnitude, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];

    out->x = magnitude * sine;
    out->y = 0.0f;
    out->z = magnitude * cosine;
}

f32 __cdecl NuVecDot(NUVEC *left, NUVEC *right) {
    return left->x * right->x + left->y * right->y + left->z * right->z;
}

void __cdecl NuVecNeg(NUVEC *out, NUVEC *vector) {
    out->x = -vector->x;
    out->y = -vector->y;
    out->z = -vector->z;
}

void __cdecl NuVecCross(NUVEC *out, NUVEC *left, NUVEC *right) {
    f32 y, z;

    y = left->z * right->x - left->x * right->z;
    z = left->x * right->y - left->y * right->x;
    out->x = left->y * right->z - left->z * right->y;
    out->y = y;
    out->z = z;
}

void __cdecl NuVecCrossRel(NUVEC *out, NUVEC *base, NUVEC *p0, NUVEC *p1) {
    NUVEC edge0;
    NUVEC edge1;

    edge0.x = p0->x - base->x;
    edge0.y = p0->y - base->y;
    edge0.z = p0->z - base->z;
    edge1.x = p1->x - base->x;
    edge1.y = p1->y - base->y;
    edge1.z = p1->z - base->z;
    NuVecCross(out, &edge0, &edge1);
}

void __cdecl NuVecMax(NUVEC *out, NUVEC *left, NUVEC *right) {
    if (left->x > right->x) {
        out->x = left->x;
    } else {
        out->x = right->x;
    }

    if (left->y > right->y) {
        out->y = left->y;
    } else {
        out->y = right->y;
    }

    if (left->z > right->z) {
        out->z = left->z;
    } else {
        out->z = right->z;
    }
}

void __cdecl NuVecMin(NUVEC *out, NUVEC *left, NUVEC *right) {
    if (left->x < right->x) {
        out->x = left->x;
    } else {
        out->x = right->x;
    }

    if (left->y < right->y) {
        out->y = left->y;
    } else {
        out->y = right->y;
    }

    if (left->z < right->z) {
        out->z = left->z;
    } else {
        out->z = right->z;
    }
}

f32 __cdecl NuVecMagSqr(NUVEC *vector) {
    return vector->x * vector->x + vector->y * vector->y + vector->z * vector->z;
}

f32 __cdecl NuVecMagXZ(NUVEC *vector) {
    return NuFsqrt(vector->x * vector->x + vector->z * vector->z);
}

f32 __cdecl NuVecNorm(NUVEC *out, NUVEC *vector) {
    f32 length = NuFsqrt(vector->x * vector->x + vector->y * vector->y + vector->z * vector->z);
    f32 inverse_length;

    if (length > 0.0f) {
        inverse_length = 1.0f / length;
    } else {
        inverse_length = 0.0f;
    }

    out->x = vector->x * inverse_length;
    out->y = vector->y * inverse_length;
    out->z = vector->z * inverse_length;
    return length;
}

void __cdecl NuVecSurfaceNormal(NUVEC *out, NUVEC *p0, NUVEC *p1, NUVEC *p2) {
    NUVEC edge0;
    NUVEC edge1;

    edge0.x = p0->x - p1->x;
    edge0.y = p0->y - p1->y;
    edge0.z = p0->z - p1->z;
    edge1.x = p0->x - p2->x;
    edge1.y = p0->y - p2->y;
    edge1.z = p0->z - p2->z;

    NuVecCross(out, &edge1, &edge0);
    NuVecNorm(out, out);
}

f32 __cdecl NuVecDistSqr(NUVEC *v0, NUVEC *v1, NUVEC *delta) {
    NUVEC local_delta;

    if (delta == NULL) {
        delta = &local_delta;
    }

    delta->x = v0->x - v1->x;
    delta->y = v0->y - v1->y;
    delta->z = v0->z - v1->z;
    return delta->x * delta->x + delta->y * delta->y + delta->z * delta->z;
}

f32 __cdecl NuVecXZDistSqr(NUVEC *v0, NUVEC *v1, NUVEC *delta) {
    NUVEC local_delta;

    if (delta == NULL) {
        delta = &local_delta;
    }

    delta->x = v0->x - v1->x;
    delta->y = 0.0f;
    delta->z = v0->z - v1->z;
    return delta->x * delta->x + delta->z * delta->z;
}

void __cdecl NuVecLerp(NUVEC *out, NUVEC *left, NUVEC *right, f32 t) {
    out->x = ((left->x - right->x) * t) + right->x;
    out->y = ((left->y - right->y) * t) + right->y;
    out->z = ((left->z - right->z) * t) + right->z;
}

void __cdecl NuVecMtxRotate(NUVEC *out, NUVEC *v, NUMTX *m) {
    f32 y = v->x * m->m01 + v->y * m->m11 + v->z * m->m21;
    f32 z = v->x * m->m02 + v->y * m->m12 + v->z * m->m22;

    out->x = v->x * m->m00 + v->y * m->m10 + v->z * m->m20;
    out->y = y;
    out->z = z;
}

void __cdecl NuVecMtxTransform(NUVEC *out, NUVEC *v, NUMTX *m) {
    f32 y = v->x * m->m01 + v->y * m->m11 + v->z * m->m21 + m->m31;
    f32 z = v->x * m->m02 + v->y * m->m12 + v->z * m->m22 + m->m32;

    out->x = v->x * m->m00 + v->y * m->m10 + v->z * m->m20 + m->m30;
    out->y = y;
    out->z = z;
}

void __cdecl NuVecMtxTransformBlock(NUVEC *out, NUVEC *vectors, NUMTX *matrix, i32 count) {
    i32 index;

    for (index = 0; index < count; ++index) {
        NUVEC *vector = &vectors[index];
        NUVEC *transformed = &out[index];

        transformed->x = vector->x * matrix->m00 + vector->y * matrix->m10 + vector->z * matrix->m20 + matrix->m30;
        transformed->y = vector->x * matrix->m01 + vector->y * matrix->m11 + vector->z * matrix->m21 + matrix->m31;
        transformed->z = vector->x * matrix->m02 + vector->y * matrix->m12 + vector->z * matrix->m22 + matrix->m32;
    }
}

void __cdecl PCVecMtxTransformClipBlocks(NUVEC4BLOCK_PC *out, NUVEC4BLOCK_PC *in, NUMTX *matrix, i32 count,
                                         u32 *outcodes) {
    i32 block_count = (count + 3) >> 2;
    i32 block;
    __m128 m00, m01, m02;
    __m128 m10, m11, m12;
    __m128 m20, m21, m22;
    __m128 m30, m31, m32;
    __m128 near_clip, far_clip, minus_one;
    __m128 zx_scale, zy_scale;
    __m128 mask32, mask16, mask1, mask2, mask8, mask4;

    if (block_count <= 0) {
        return;
    }

    m00 = _mm_set1_ps(matrix->m00);
    m01 = _mm_set1_ps(matrix->m01);
    m02 = _mm_set1_ps(matrix->m02);
    m10 = _mm_set1_ps(matrix->m10);
    m11 = _mm_set1_ps(matrix->m11);
    m12 = _mm_set1_ps(matrix->m12);
    m20 = _mm_set1_ps(matrix->m20);
    m21 = _mm_set1_ps(matrix->m21);
    m22 = _mm_set1_ps(matrix->m22);
    m30 = _mm_set1_ps(matrix->m30);
    m31 = _mm_set1_ps(matrix->m31);
    m32 = _mm_set1_ps(matrix->m32);
    near_clip = _mm_set1_ps(global_camera.near_clip);
    far_clip = _mm_load_ps(ClipFarLimits);
    minus_one = _mm_load_ps(ClipOutcodeMinusOne);
    zx_scale = _mm_set1_ps(zx);
    zy_scale = _mm_set1_ps(zy);
    mask32 = _mm_load_ps((const f32 *)ClipOutcodeMask32);
    mask16 = _mm_load_ps((const f32 *)ClipOutcodeMask16);
    mask1 = _mm_load_ps((const f32 *)ClipOutcodeMask1);
    mask2 = _mm_load_ps((const f32 *)ClipOutcodeMask2);
    mask8 = _mm_load_ps((const f32 *)ClipOutcodeMask8);
    mask4 = _mm_load_ps((const f32 *)ClipOutcodeMask4);

    for (block = 0; block < block_count; ++block) {
        NUVEC4BLOCK_PC *source = &in[block];
        NUVEC4BLOCK_PC *destination = &out[block];
        __m128 x = _mm_load_ps(source->x);
        __m128 y = _mm_load_ps(source->y);
        __m128 z = _mm_load_ps(source->z);
        __m128 transformed_x =
            _mm_add_ps(_mm_add_ps(_mm_add_ps(m30, _mm_mul_ps(x, m00)), _mm_mul_ps(y, m10)), _mm_mul_ps(z, m20));
        __m128 transformed_y =
            _mm_add_ps(_mm_add_ps(_mm_add_ps(m31, _mm_mul_ps(x, m01)), _mm_mul_ps(y, m11)), _mm_mul_ps(z, m21));
        __m128 transformed_z =
            _mm_add_ps(_mm_add_ps(_mm_add_ps(m32, _mm_mul_ps(x, m02)), _mm_mul_ps(y, m12)), _mm_mul_ps(z, m22));
        __m128 z_times_zx = _mm_mul_ps(transformed_z, zx_scale);
        __m128 z_times_zy = _mm_mul_ps(transformed_z, zy_scale);
        __m128 code;

        _mm_store_ps(destination->x, transformed_x);
        _mm_store_ps(destination->y, transformed_y);
        _mm_store_ps(destination->z, transformed_z);

        code = _mm_and_ps(_mm_cmplt_ps(transformed_z, near_clip), mask32);
        code = _mm_or_ps(code, _mm_and_ps(_mm_cmpnle_ps(transformed_z, far_clip), mask16));
        code = _mm_or_ps(code, _mm_and_ps(_mm_cmplt_ps(transformed_x, _mm_mul_ps(z_times_zx, minus_one)), mask1));
        code = _mm_or_ps(code, _mm_and_ps(_mm_cmpnle_ps(transformed_x, z_times_zx), mask2));
        code = _mm_or_ps(code, _mm_and_ps(_mm_cmplt_ps(transformed_y, _mm_mul_ps(z_times_zy, minus_one)), mask8));
        code = _mm_or_ps(code, _mm_and_ps(_mm_cmpnle_ps(transformed_y, z_times_zy), mask4));
        _mm_store_ps((f32 *)&outcodes[block * 4], code);
    }
}

void __cdecl NuVecInvMtxRotate(NUVEC *out, NUVEC *v, NUMTX *m) {
    f32 y = v->x * m->m10 + v->y * m->m11 + v->z * m->m12;
    f32 z = v->x * m->m20 + v->y * m->m21 + v->z * m->m22;

    out->x = v->x * m->m00 + v->y * m->m01 + v->z * m->m02;
    out->y = y;
    out->z = z;
}

void __cdecl NuVecInvMtxTransform(NUVEC *out, NUVEC *v, NUMTX *m) {
    f32 x = v->x - m->m30;
    f32 y = v->y - m->m31;
    f32 z = v->z - m->m32;

    out->x = x * m->m00 + y * m->m01 + z * m->m02;
    out->y = x * m->m10 + y * m->m11 + z * m->m12;
    out->z = x * m->m20 + y * m->m21 + z * m->m22;
}

f32 NuVecMag(NUVEC *vec) {
    return NuFsqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
}

f32 NuVecDist(NUVEC *v0, NUVEC *v1, NUVEC *dist) {
    NUVEC delta;

    if (dist != NULL) {
        dist->x = v0->x - v1->x;
        dist->y = v0->y - v1->y;
        dist->z = v0->z - v1->z;
        return NuFsqrt(dist->x * dist->x + dist->y * dist->y + dist->z * dist->z);
    }
    delta.x = v0->x - v1->x;
    delta.y = v0->y - v1->y;
    delta.z = v0->z - v1->z;
    return NuFsqrt(delta.y * delta.y + delta.x * delta.x + delta.z * delta.z);
}

f32 __cdecl NuVecXZDist(NUVEC *left, NUVEC *right, NUVEC *delta) {
    NUVEC local;
    if (delta == NULL)
        delta = &local;
    delta->x = left->x - right->x;
    delta->y = 0.0f;
    delta->z = left->z - right->z;
    return NuFsqrt(delta->x * delta->x + delta->y * delta->y + delta->z * delta->z);
}

void __cdecl NuVec4MtxTransformVU0(NUVEC4 *out, NUVEC4 *v, NUMTX *m) {
    f32 y = v->x * m->m01 + v->y * m->m11 + v->z * m->m21 + v->w * m->m31;
    f32 z = v->x * m->m02 + v->y * m->m12 + v->z * m->m22 + v->w * m->m32;
    f32 w = v->x * m->m03 + v->y * m->m13 + v->z * m->m23 + v->w * m->m33;
    out->x = v->x * m->m00 + v->y * m->m10 + v->z * m->m20 + v->w * m->m30;
    out->y = y;
    out->z = z;
    out->w = w;
}
