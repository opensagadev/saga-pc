#include "saga.h"
#include <xmmintrin.h>

i32 __cdecl NuLineLineIntersect(NUVEC *origin_a, NUVEC *direction_a, NUVEC *origin_b, NUVEC *direction_b,
                                f32 *parameter_a, f32 *parameter_b) {
    NUVEC cross;
    NUVEC difference;
    f32 length;
    f32 inverse;
    f32 determinant;

    cross.x = direction_a->y * direction_b->z - direction_a->z * direction_b->y;
    cross.y = direction_a->z * direction_b->x - direction_a->x * direction_b->z;
    cross.z = direction_a->x * direction_b->y - direction_a->y * direction_b->x;
    length = NuFsqrt(cross.x * cross.x + cross.y * cross.y + cross.z * cross.z);
    inverse = length == 0.0f ? 0.0f : 1.0f / length;
    inverse *= inverse;
    difference.x = origin_b->x - origin_a->x;
    difference.y = origin_b->y - origin_a->y;
    difference.z = origin_b->z - origin_a->z;
    determinant = difference.x * (direction_b->y * cross.z - direction_b->z * cross.y) -
                  difference.y * (direction_b->x * cross.z - direction_b->z * cross.x) +
                  difference.z * (direction_b->x * cross.y - direction_b->y * cross.x);
    *parameter_a = determinant * inverse;
    determinant = difference.x * (direction_a->y * cross.z - direction_a->z * cross.y) -
                  difference.y * (direction_a->x * cross.z - direction_a->z * cross.x) +
                  difference.z * (direction_a->x * cross.y - direction_a->y * cross.x);
    *parameter_b = determinant * inverse;
    return inverse != 0.0f;
}

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

void __cdecl NuVec4MtxTransform(NUVEC4 *out, NUVEC *v, NUMTX *m) {
    f32 y = v->x * m->m01 + v->y * m->m11 + v->z * m->m21 + m->m31;
    f32 z = v->x * m->m02 + v->y * m->m12 + v->z * m->m22 + m->m32;
    f32 w = v->x * m->m03 + v->y * m->m13 + v->z * m->m23 + m->m33;
    out->x = v->x * m->m00 + v->y * m->m10 + v->z * m->m20 + m->m30;
    out->y = y;
    out->z = z;
    out->w = w;
}

void __cdecl NuVec4MtxTransformVU0x3(NUVEC4 *out, NUVEC4 *in, NUMTX *m) {
    i32 i;
    for (i = 0; i < 3; ++i) {
        f32 y = ((in->y * m->m11 + in->x * m->m01) + in->z * m->m21) + in->w * m->m31;
        f32 z = ((in->y * m->m12 + in->x * m->m02) + in->z * m->m22) + in->w * m->m32;
        f32 w = ((in->x * m->m03 + in->y * m->m13) + in->z * m->m23) + in->w * m->m33;
        f32 x = ((in->x * m->m00 + in->y * m->m10) + in->z * m->m20) + in->w * m->m30;
        out->x = x;
        out->y = y;
        out->z = z;
        out->w = w;
        ++in;
        ++out;
    }
}

void __cdecl NuVec4MtxRotateVU0(NUVEC4 *out, NUVEC4 *input, NUMTX *matrix) {
    f32 y = matrix->m01 * input->x + matrix->m11 * input->y + matrix->m21 * input->z;
    f32 z = matrix->m02 * input->x + matrix->m12 * input->y + matrix->m22 * input->z;
    out->x = matrix->m10 * input->y + input->x * matrix->m00 + matrix->m20 * input->z;
    out->y = y;
    out->z = z;
    out->w = input->w;
}

i32 __cdecl NuPointRelToBoundingBox(NUVEC *point, NUVEC *maximum, NUVEC *minimum) {
    i32 result = 0;
    if (point->x >= maximum->x)
        result |= 1;
    else if (point->x <= minimum->x)
        result |= 8;
    if (point->y >= maximum->y)
        result |= 2;
    else if (point->y <= minimum->y)
        result |= 16;
    if (point->z >= maximum->z)
        result |= 4;
    else if (point->z <= minimum->z)
        result |= 32;
    return result;
}
void __cdecl NuClipXPlane(NUVEC *out, NUVEC *point, NUVEC *direction, f32 *plane) {
    f32 delta;
    out->x = plane ? *plane : 0.0f;
    delta = out->x - point->x;
    out->y = point->y + (direction->y * delta) / direction->x;
    out->z = point->z + (direction->z * delta) / direction->x;
}
void __cdecl NuClipYPlane(NUVEC *out, NUVEC *point, NUVEC *direction, f32 *plane) {
    f32 delta;
    out->y = plane ? *plane : 0.0f;
    delta = out->y - point->y;
    out->x = point->x + (direction->x * delta) / direction->y;
    out->z = point->z + (direction->z * delta) / direction->y;
}
void __cdecl NuClipZPlane(NUVEC *out, NUVEC *point, NUVEC *direction, f32 *plane) {
    f32 delta;
    out->z = plane ? *plane : 0.0f;
    delta = out->z - point->z;
    out->x = point->x + (direction->x * delta) / direction->z;
    out->y = point->y + (direction->y * delta) / direction->z;
}
i32 __cdecl BoundingBoxToLine(NUVEC *minimum, NUVEC *maximum, NUMTX *matrix, NUVEC *start, NUVEC *end, f32 expansion,
                              NUVEC *intersection) {
    NUVEC a, b, direction, hit, lo, hi;
    i32 ca, cb;
    lo.x = minimum->x - expansion;
    lo.y = minimum->y - expansion;
    lo.z = minimum->z - expansion;
    hi.x = maximum->x + expansion;
    hi.y = maximum->y + expansion;
    hi.z = maximum->z + expansion;
    NuVecInvMtxTransform(&a, start, matrix);
    ca = NuPointRelToBoundingBox(&a, &hi, &lo);
    if (!ca && !intersection)
        return 1;
    NuVecInvMtxTransform(&b, end, matrix);
    cb = NuPointRelToBoundingBox(&b, &hi, &lo);
    if (!cb && !intersection)
        return 1;
    if (!ca && !cb) {
        if (intersection)
            intersection->z = intersection->y = intersection->x = 0.0f;
        return 1;
    }
    if (ca & cb)
        return 0;
    NuVecSub(&direction, &b, &a);
    NuVecNorm(&direction, &direction);
    if (ca & 1) {
        NuClipXPlane(&hit, &a, &direction, &hi.x);
        if (hit.y < hi.y && hit.y > lo.y && hit.z < hi.z && hit.z > lo.z) {
            if (intersection)
                *intersection = hit;
            return 1;
        }
    } else if (ca & 8) {
        NuClipXPlane(&hit, &a, &direction, &lo.x);
        if (hit.y < hi.y && hit.y > lo.y && hit.z < hi.z && hit.z > lo.z) {
            if (intersection)
                *intersection = hit;
            return 1;
        }
    }
    if (ca & 2) {
        NuClipYPlane(&hit, &a, &direction, &hi.y);
        if (hit.x < hi.x && hit.x > lo.x && hit.z < hi.z && hit.z > lo.z) {
            if (intersection)
                *intersection = hit;
            return 1;
        }
    } else if (ca & 16) {
        NuClipYPlane(&hit, &a, &direction, &lo.y);
        if (hit.x < hi.x && hit.x > lo.x && hit.z < hi.z && hit.z > lo.z) {
            if (intersection)
                *intersection = hit;
            return 1;
        }
    }
    if (ca & 4) {
        NuClipZPlane(&hit, &a, &direction, &hi.z);
        if (hit.x < hi.x && hit.x > lo.x && hit.y < hi.y && hit.y > lo.y) {
            if (intersection)
                *intersection = hit;
            return 1;
        }
    } else if (ca & 32) {
        NuClipZPlane(&hit, &a, &direction, &lo.z);
        if (hit.x < hi.x && hit.x > lo.x && hit.y < hi.y && hit.y > lo.y) {
            if (intersection)
                *intersection = hit;
            return 1;
        }
    }
    return 0;
}

f32 __cdecl NuVecMtxTransformH(NUVEC *out, NUVEC *v, NUMTX *m) {
    f32 w = v->x * m->m03 + v->y * m->m13 + v->z * m->m23 + m->m33;
    f32 reciprocal_w = w == 0.0f ? 0.0f : 1.0f / w;
    f32 y = (v->x * m->m01 + v->y * m->m11 + v->z * m->m21 + m->m31) * reciprocal_w;
    f32 z = (v->x * m->m02 + v->y * m->m12 + v->z * m->m22 + m->m32) * reciprocal_w;
    out->x = (v->x * m->m00 + v->y * m->m10 + v->z * m->m20 + m->m30) * reciprocal_w;
    out->y = y;
    out->z = z;
    return reciprocal_w;
}

void __cdecl NuVecMtxTranslate(NUVEC *out, NUVEC *v, NUMTX *m) {
    out->x = v->x + m->m30;
    out->y = v->y + m->m31;
    out->z = v->z + m->m32;
}

void __cdecl NuVecMtxRotateValX(NUVEC *out, f32 value, NUMTX *matrix) {
    out->x = matrix->m00 * value;
    out->y = matrix->m01 * value;
    out->z = matrix->m02 * value;
}

void __cdecl NuVecMtxRotateValY(NUVEC *out, f32 value, NUMTX *matrix) {
    out->x = matrix->m10 * value;
    out->y = matrix->m11 * value;
    out->z = matrix->m12 * value;
}

void __cdecl NuVecMtxRotateValZ(NUVEC *out, f32 value, NUMTX *matrix) {
    out->x = matrix->m20 * value;
    out->y = matrix->m21 * value;
    out->z = matrix->m22 * value;
}

void __cdecl NuVecMtxRotateH(NUVEC *out, NUVEC *v, NUMTX *m) {
    f32 reciprocal_w = 1.0f / (v->x * m->m03 + v->y * m->m13 + v->z * m->m23);
    f32 y = (v->x * m->m01 + v->y * m->m11 + v->z * m->m21) * reciprocal_w;
    f32 z = (v->x * m->m02 + v->y * m->m12 + v->z * m->m22) * reciprocal_w;
    out->x = (v->x * m->m00 + v->y * m->m10 + v->z * m->m20) * reciprocal_w;
    out->y = y;
    out->z = z;
}

void __cdecl NuVecMtxScale(NUVEC *out, NUVEC *v, NUMTX *m) {
    out->x = v->x * m->m00;
    out->y = v->y * m->m11;
    out->z = v->z * m->m22;
}

void __cdecl NuVecInvMtxTranslate(NUVEC *out, NUVEC *v, NUMTX *matrix) {
    out->x = v->x - matrix->m30;
    out->y = v->y - matrix->m31;
    out->z = v->z - matrix->m32;
}

void __cdecl NuVecInvMtxRotateValX(NUVEC *out, f32 unused, NUMTX *matrix) {
    (void)unused;
    out->x = out->x * matrix->m00;
    out->y = out->x * matrix->m10;
    out->z = out->x * matrix->m20;
}

void __cdecl NuVecInvMtxRotateValY(NUVEC *out, f32 unused, NUMTX *matrix) {
    (void)unused;
    out->x = out->y * matrix->m01;
    out->y = out->y * matrix->m11;
    out->z = out->y * matrix->m21;
}

void __cdecl NuVecInvMtxRotateValZ(NUVEC *out, f32 unused, NUMTX *matrix) {
    (void)unused;
    out->x = out->z * matrix->m02;
    out->y = out->z * matrix->m12;
    out->z = out->z * matrix->m22;
}

void __cdecl NuVecInvMtxScale(NUVEC *out, NUVEC *v, NUMTX *matrix) {
    out->x = v->x / matrix->m00;
    out->y = v->y / matrix->m11;
    out->z = v->z / matrix->m22;
}

void __cdecl NuVecRotateYValX(NUVEC *out, f32 value, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    out->x = value * cosine;
    out->y = 0.0f;
    out->z = -value * sine;
}

void __cdecl NuVec4MtxTransformHVU0(NUVEC4 *out, NUVEC4 *v, NUMTX *matrix) {
    NuVec4MtxTransformVU0(out, v, matrix);
}

void __cdecl AnimationDiagnosticNoOpPc(void) {
}

/* Retail 0x5c1880 takes parent in ECX, rotation in EDX and the output on the
 * stack (caller cleanup); __fastcall is the closest non-LTCG convention. */
void __fastcall NuQuatMulAlignedPc(const NUQUAT *parent, const NUQUAT *rotation, NUQUAT *out) {
    __m128 q = _mm_load_ps(&rotation->x);
    __m128 p = _mm_load_ps(&parent->x);
    __m128 a = q;
    __m128 b, c, d, lo0, lo1, hi1, sum;
    q = _mm_shuffle_ps(q, q, 0x1b);
    b = q;
    q = _mm_shuffle_ps(q, q, 0xb1);
    c = q;
    a = _mm_sub_ps(_mm_setzero_ps(), _mm_mul_ps(a, p));
    b = _mm_mul_ps(b, p);
    c = _mm_mul_ps(c, p);
    q = _mm_shuffle_ps(q, q, 0x1b);
    d = _mm_mul_ps(q, p);
    c = _mm_shuffle_ps(c, c, 0x36);
    d = _mm_shuffle_ps(d, d, 0x6c);
    b = _mm_shuffle_ps(b, b, 0xb4);
    lo0 = _mm_unpacklo_ps(d, a);
    d = _mm_unpackhi_ps(d, a);
    lo1 = _mm_unpacklo_ps(b, c);
    hi1 = _mm_unpackhi_ps(b, c);
    sum = _mm_add_ps(_mm_add_ps(_mm_movelh_ps(lo1, lo0), _mm_movehl_ps(lo0, lo1)), _mm_movelh_ps(hi1, d));
    _mm_store_ps(&out->x, _mm_sub_ps(sum, _mm_movehl_ps(d, hi1)));
}
