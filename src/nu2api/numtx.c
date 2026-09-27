#include "saga.h"

void __cdecl NuMtxSetIdentity(NUMTX *matrix) {
    *matrix = NuMtxSetIdentityTemplate;
}

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

void __cdecl NuMtxSetTranslation(NUMTX *m, NUVEC *t) {
    m->m30 = t->x;
    m->m31 = t->y;
    m->m32 = t->z;
    m->m01 = m->m02 = m->m03 = m->m10 = m->m12 = m->m13 = m->m20 = m->m21 = m->m23 = 0.0f;
    m->m00 = m->m11 = m->m22 = m->m33 = 1.0f;
}

void __cdecl NuMtxSetTranslationNeg(NUMTX *m, NUVEC *t) {
    m->m30 = -t->x;
    m->m31 = -t->y;
    m->m32 = -t->z;
    m->m01 = m->m02 = m->m03 = m->m10 = m->m12 = m->m13 = m->m20 = m->m21 = m->m23 = 0.0f;
    m->m00 = m->m11 = m->m22 = m->m33 = 1.0f;
}

void __cdecl NuMtxSetScale(NUMTX *m, NUVEC *s) {
    m->m00 = s->x;
    m->m11 = s->y;
    m->m22 = s->z;
    m->m01 = m->m02 = m->m03 = m->m10 = m->m12 = m->m13 = m->m20 = m->m21 = m->m23 = m->m30 = m->m31 = m->m32 = 0.0f;
    m->m33 = 1.0f;
}

void __cdecl NuMtxSetRotationX(NUMTX *m, NUANG angle) {
    m->m11 = m->m22 = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    m->m12 = NuTrigTable[(angle >> 1) & 0x7fff];
    m->m21 = -m->m12;
    m->m00 = 1.0f;
    m->m01 = m->m02 = m->m03 = m->m23 = m->m10 = m->m20 = m->m13 = m->m30 = m->m31 = m->m32 = 0.0f;
    m->m33 = 1.0f;
}

void __cdecl NuMtxSetRotationY(NUMTX *m, NUANG angle) {
    m->m00 = m->m22 = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    m->m20 = NuTrigTable[(angle >> 1) & 0x7fff];
    m->m02 = -m->m20;
    m->m11 = 1.0f;
    m->m01 = m->m10 = m->m03 = m->m23 = m->m12 = m->m21 = m->m13 = m->m30 = m->m31 = m->m32 = 0.0f;
    m->m33 = 1.0f;
}

void __cdecl NuMtxSetRotationZ(NUMTX *m, NUANG angle) {
    m->m00 = m->m11 = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    m->m01 = NuTrigTable[(angle >> 1) & 0x7fff];
    m->m10 = -m->m01;
    m->m22 = 1.0f;
    m->m02 = m->m12 = m->m03 = m->m23 = m->m20 = m->m21 = m->m13 = m->m30 = m->m31 = m->m32 = 0.0f;
    m->m33 = 1.0f;
}

void __cdecl NuMtxSetRotationAxis(NUMTX *m, NUANG angle, NUVEC *axis) {
    NUANG half_angle = angle >> 1;
    f32 sine = NuTrigTable[(half_angle >> 1) & 0x7fff];
    f32 cosine = NuTrigTable[((half_angle + 0x4000) >> 1) & 0x7fff];
    f32 a = axis->x * sine;
    f32 two_a = a + a;
    f32 b = axis->y * sine;
    f32 two_b = b + b;
    f32 c = axis->z * sine;
    f32 two_c = c + c;
    f32 two_sa = cosine * two_a;
    f32 two_aa = a * two_a;
    f32 two_sb = cosine * two_b;
    f32 two_ab = a * two_b;
    f32 two_bb = b * two_b;
    f32 two_sc = cosine * two_c;
    f32 two_ac = a * two_c;
    f32 two_bc = b * two_c;
    f32 two_cc = c * two_c;

    m->m00 = 1.0f - (two_bb + two_cc);
    m->m11 = 1.0f - (two_aa + two_cc);
    m->m22 = 1.0f - (two_aa + two_bb);
    m->m33 = 1.0f;
    m->m01 = two_ab + two_sc;
    m->m02 = two_ac - two_sb;
    m->m03 = 0.0f;
    m->m10 = two_ab - two_sc;
    m->m12 = two_bc + two_sa;
    m->m13 = 0.0f;
    m->m20 = two_ac + two_sb;
    m->m21 = two_bc - two_sa;
    m->m23 = 0.0f;
    m->m30 = m->m31 = m->m32 = 0.0f;
}

void __cdecl NuMtxRotateAng(NUANG angle, f32 x, f32 z, f32 *rx, f32 *rz) {
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];

    *rx = x * cosine - z * sine;
    *rz = x * sine + z * cosine;
}

void __cdecl NuMtxTranslate(NUMTX *m, NUVEC *t) {
    m->m30 += t->x;
    m->m31 += t->y;
    m->m32 += t->z;
}

void __cdecl NuMtxTranslateNeg(NUMTX *m, NUVEC *t) {
    m->m30 -= t->x;
    m->m31 -= t->y;
    m->m32 -= t->z;
}

void __cdecl NuMtxPreTranslate(NUMTX *m, NUVEC *t) {
    m->m30 += t->x * m->m00 + t->y * m->m10 + t->z * m->m20;
    m->m31 += t->x * m->m01 + t->y * m->m11 + t->z * m->m21;
    m->m32 += t->x * m->m02 + t->y * m->m12 + t->z * m->m22;
}

void __cdecl NuMtxScale(NUMTX *m, NUVEC *s) {
    m->m00 *= s->x;
    m->m01 *= s->y;
    m->m02 *= s->z;
    m->m10 *= s->x;
    m->m11 *= s->y;
    m->m12 *= s->z;
    m->m20 *= s->x;
    m->m21 *= s->y;
    m->m22 *= s->z;
    m->m30 *= s->x;
    m->m31 *= s->y;
    m->m32 *= s->z;
}

void __cdecl NuMtxScaleU(NUMTX *m, f32 s) {
    m->m00 *= s;
    m->m01 *= s;
    m->m02 *= s;
    m->m10 *= s;
    m->m11 *= s;
    m->m12 *= s;
    m->m20 *= s;
    m->m21 *= s;
    m->m22 *= s;
    m->m30 *= s;
    m->m31 *= s;
    m->m32 *= s;
}

void __cdecl NuMtxPreScaleU(NUMTX *m, f32 s) {
    m->m00 *= s;
    m->m01 *= s;
    m->m02 *= s;
    m->m10 *= s;
    m->m11 *= s;
    m->m12 *= s;
    m->m20 *= s;
    m->m21 *= s;
    m->m22 *= s;
}

void __cdecl NuMtxPreScale(NUMTX *m, NUVEC *s) {
    m->m00 *= s->x;
    m->m01 *= s->x;
    m->m02 *= s->x;
    m->m10 *= s->y;
    m->m11 *= s->y;
    m->m12 *= s->y;
    m->m20 *= s->z;
    m->m21 *= s->z;
    m->m22 *= s->z;
}

void __cdecl NuMtxPreScaleX(NUMTX *m, f32 scale_x) {
    m->m00 *= scale_x;
    m->m01 *= scale_x;
    m->m02 *= scale_x;
}

void __cdecl NuMtxGetXAxis(NUMTX *m, NUVEC *x) {
    x->x = m->m00;
    x->y = m->m01;
    x->z = m->m02;
}

void __cdecl NuMtxGetYAxis(NUMTX *m, NUVEC *y) {
    y->x = m->m10;
    y->y = m->m11;
    y->z = m->m12;
}

void __cdecl NuMtxGetZAxis(NUMTX *m, NUVEC *z) {
    z->x = m->m20;
    z->y = m->m21;
    z->z = m->m22;
}

void __cdecl NuMtxGetTranslation(NUMTX *m, NUVEC *translation) {
    translation->x = m->m30;
    translation->y = m->m31;
    translation->z = m->m32;
}

void __cdecl NuMtxRotateX(NUMTX *m, NUANG angle) {
    f32 cos_x = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_x = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m01 = m->m01;
    f32 m11 = m->m11;
    f32 m21 = m->m21;
    f32 m31 = m->m31;

    m->m01 = m01 * cos_x - m->m02 * sin_x;
    m->m02 = m01 * sin_x + m->m02 * cos_x;
    m->m11 = m11 * cos_x - m->m12 * sin_x;
    m->m12 = m11 * sin_x + m->m12 * cos_x;
    m->m21 = m21 * cos_x - m->m22 * sin_x;
    m->m22 = m21 * sin_x + m->m22 * cos_x;
    m->m31 = m31 * cos_x - m->m32 * sin_x;
    m->m32 = m31 * sin_x + m->m32 * cos_x;
}

void __cdecl NuMtxRotateY(NUMTX *m, NUANG angle) {
    f32 cos_y = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_y = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m10 = m->m10;
    f32 m20 = m->m20;
    f32 m30 = m->m30;

    m->m00 = m00 * cos_y + m->m02 * sin_y;
    m->m02 = m->m02 * cos_y - m00 * sin_y;
    m->m10 = m10 * cos_y + m->m12 * sin_y;
    m->m12 = m->m12 * cos_y - m10 * sin_y;
    m->m20 = m20 * cos_y + m->m22 * sin_y;
    m->m22 = m->m22 * cos_y - m20 * sin_y;
    m->m30 = m30 * cos_y + m->m32 * sin_y;
    m->m32 = m->m32 * cos_y - m30 * sin_y;
}

void __cdecl NuMtxRotateZ(NUMTX *m, NUANG angle) {
    f32 cos_z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_z = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m10 = m->m10;
    f32 m20 = m->m20;
    f32 m30 = m->m30;

    m->m00 = m00 * cos_z - m->m01 * sin_z;
    m->m01 = m00 * sin_z + m->m01 * cos_z;
    m->m10 = m10 * cos_z - m->m11 * sin_z;
    m->m11 = m10 * sin_z + m->m11 * cos_z;
    m->m20 = m20 * cos_z - m->m21 * sin_z;
    m->m21 = m20 * sin_z + m->m21 * cos_z;
    m->m30 = m30 * cos_z - m->m31 * sin_z;
    m->m31 = m30 * sin_z + m->m31 * cos_z;
}

void __cdecl NuMtxTransposeR(NUMTX *m, NUMTX *source) {
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
    m->m30 = source->m30;
    m->m31 = source->m31;
    m->m32 = source->m32;
    m->m33 = source->m33;
}

void __cdecl NuMtxTranspose(NUMTX *m, NUMTX *source) {
    f32 t;

    t = source->m01;
    m->m01 = source->m10;
    m->m10 = t;
    t = source->m02;
    m->m02 = source->m20;
    m->m20 = t;
    t = source->m03;
    m->m03 = source->m30;
    m->m30 = t;
    t = source->m12;
    m->m12 = source->m21;
    m->m21 = t;
    t = source->m13;
    m->m13 = source->m31;
    m->m31 = t;
    t = source->m23;
    m->m23 = source->m32;
    m->m32 = t;
    m->m00 = source->m00;
    m->m11 = source->m11;
    m->m22 = source->m22;
    m->m33 = source->m33;
}

void __cdecl NuMtxInv(NUMTX *m, NUMTX *source) {
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

void __cdecl NuMtxInvR(NUMTX *m, NUMTX *source) {
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
    m->m23 = 0.0f;
    m->m13 = m->m23;
    m->m03 = m->m13;
    m->m32 = 0.0f;
    m->m31 = m->m32;
    m->m30 = m->m31;
    m->m33 = 1.0f;
}
