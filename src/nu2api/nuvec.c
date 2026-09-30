#include "saga.h"

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
