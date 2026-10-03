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

f32 __cdecl NuMtxSSE(NUMTX *a, NUMTX *b) {
    f32 sse;

    sse = (a->m00 - b->m00) * (a->m00 - b->m00);
    sse += (a->m01 - b->m01) * (a->m01 - b->m01);
    sse += (a->m02 - b->m02) * (a->m02 - b->m02);
    sse += (a->m03 - b->m03) * (a->m03 - b->m03);
    sse += (a->m10 - b->m10) * (a->m10 - b->m10);
    sse += (a->m11 - b->m11) * (a->m11 - b->m11);
    sse += (a->m12 - b->m12) * (a->m12 - b->m12);
    sse += (a->m13 - b->m13) * (a->m13 - b->m13);
    sse += (a->m20 - b->m20) * (a->m20 - b->m20);
    sse += (a->m21 - b->m21) * (a->m21 - b->m21);
    sse += (a->m22 - b->m22) * (a->m22 - b->m22);
    sse += (a->m23 - b->m23) * (a->m23 - b->m23);
    sse += (a->m30 - b->m30) * (a->m30 - b->m30);
    sse += (a->m31 - b->m31) * (a->m31 - b->m31);
    sse += (a->m32 - b->m32) * (a->m32 - b->m32);
    sse += (a->m33 - b->m33) * (a->m33 - b->m33);

    return sse;
}

void __cdecl NuMtxOrth(NUMTX *m) {
    f32 magnitude;

    magnitude = 1.0f / NuFsqrt(m->m00 * m->m00 + m->m01 * m->m01 + m->m02 * m->m02);
    m->m00 = m->m00 * magnitude;
    m->m01 = m->m01 * magnitude;
    m->m02 = m->m02 * magnitude;

    magnitude = 1.0f / NuFsqrt(m->m10 * m->m10 + m->m11 * m->m11 + m->m12 * m->m12);
    m->m10 = m->m10 * magnitude;
    m->m11 = m->m11 * magnitude;
    m->m12 = m->m12 * magnitude;

    m->m20 = m->m01 * m->m12 - m->m02 * m->m11;
    m->m21 = m->m02 * m->m10 - m->m00 * m->m12;
    m->m22 = m->m00 * m->m11 - m->m01 * m->m10;
    m->m10 = m->m21 * m->m02 - m->m22 * m->m01;
    m->m11 = m->m22 * m->m00 - m->m20 * m->m02;
    m->m12 = m->m20 * m->m01 - m->m21 * m->m00;
}

void __cdecl NuMtxInvRSS(NUMTX *inverse, NUMTX *matrix) {
    NUMTX result;
    f32 determinant;
    f32 inverse_determinant;

    determinant = matrix->m00 * (matrix->m11 * matrix->m22 - matrix->m12 * matrix->m21) -
                  matrix->m01 * (matrix->m10 * matrix->m22 - matrix->m12 * matrix->m20) +
                  matrix->m02 * (matrix->m10 * matrix->m21 - matrix->m11 * matrix->m20);
    inverse_determinant = determinant == 0.0f ? 0.0f : 1.0f / determinant;

    result.m00 = (matrix->m11 * matrix->m22 - matrix->m12 * matrix->m21) * inverse_determinant;
    result.m10 = (matrix->m10 * matrix->m22 - matrix->m12 * matrix->m20) * -inverse_determinant;
    result.m20 = (matrix->m10 * matrix->m21 - matrix->m11 * matrix->m20) * inverse_determinant;
    result.m01 = (matrix->m01 * matrix->m22 - matrix->m02 * matrix->m21) * -inverse_determinant;
    result.m11 = (matrix->m00 * matrix->m22 - matrix->m02 * matrix->m20) * inverse_determinant;
    result.m21 = (matrix->m00 * matrix->m21 - matrix->m01 * matrix->m20) * -inverse_determinant;
    result.m02 = (matrix->m01 * matrix->m12 - matrix->m02 * matrix->m11) * inverse_determinant;
    result.m12 = (matrix->m00 * matrix->m12 - matrix->m02 * matrix->m10) * -inverse_determinant;
    result.m22 = (matrix->m00 * matrix->m11 - matrix->m01 * matrix->m10) * inverse_determinant;
    result.m03 = 0.0f;
    result.m13 = 0.0f;
    result.m23 = 0.0f;
    result.m33 = 1.0f;
    result.m30 = 0.0f;
    result.m31 = 0.0f;
    result.m32 = 0.0f;
    *inverse = result;
}

void __cdecl NuMtxMulVU0(NUMTX *result, NUMTX *left, NUMTX *right) {
    D3DXMatrixMultiply((D3DXMATRIX *)result, (D3DXMATRIX *)left, (D3DXMATRIX *)right);
}

void __cdecl NuMtxLookAtD3D(NUMTX *matrix, NUVEC *eye, NUVEC *center, NUVEC *up) {
    NUVEC x_axis;
    NUVEC y_axis;
    NUVEC z_axis;

    NuVecSub(&z_axis, center, eye);
    NuVecNorm(&z_axis, &z_axis);
    NuVecCross(&x_axis, up, &z_axis);
    NuVecNorm(&x_axis, &x_axis);
    NuVecCross(&y_axis, &z_axis, &x_axis);
    NuVecNorm(&y_axis, &y_axis);

    matrix->m00 = x_axis.x;
    matrix->m01 = y_axis.x;
    matrix->m02 = z_axis.x;
    matrix->m03 = 0.0f;
    matrix->m10 = x_axis.y;
    matrix->m11 = y_axis.y;
    matrix->m12 = z_axis.y;
    matrix->m13 = 0.0f;
    matrix->m20 = x_axis.z;
    matrix->m21 = y_axis.z;
    matrix->m22 = z_axis.z;
    matrix->m23 = 0.0f;
    matrix->m30 = (-eye->x * x_axis.x - eye->y * x_axis.y) - eye->z * x_axis.z;
    matrix->m31 = (-eye->x * y_axis.x - eye->y * y_axis.y) - eye->z * y_axis.z;
    matrix->m32 = (-eye->x * z_axis.x - eye->y * z_axis.y) - eye->z * z_axis.z;
    matrix->m33 = 1.0f;
}

void __cdecl NuMtxSetPerspectiveD3D(NUMTX *matrix, f32 fovy, f32 aspect, f32 z_near, f32 z_far) {
    i32 half_angle = (i32)((fovy / 2.0f) * (65536.0f / 360.0f));
    f32 tangent = NuTrigTable[(half_angle >> 1) & 0x7fff] / NuTrigTable[((half_angle + 0x4000) >> 1) & 0x7fff];

    matrix->m00 = 1.0f / (aspect * tangent);
    matrix->m01 = 0.0f;
    matrix->m02 = 0.0f;
    matrix->m03 = 0.0f;
    matrix->m10 = 0.0f;
    matrix->m11 = 1.0f / tangent;
    matrix->m12 = 0.0f;
    matrix->m13 = 0.0f;
    matrix->m20 = 0.0f;
    matrix->m21 = 0.0f;
    matrix->m22 = z_far / (z_far - z_near);
    matrix->m23 = 1.0f;
    matrix->m30 = 0.0f;
    matrix->m31 = 0.0f;
    matrix->m32 = (-z_far * z_near) / (z_far - z_near);
    matrix->m33 = 0.0f;
}

void __cdecl NuMtxSetFrustumD3D(NUMTX *matrix, f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far) {
    matrix->m00 = (z_near + z_near) / (right - left);
    matrix->m01 = 0.0f;
    matrix->m02 = 0.0f;
    matrix->m03 = 0.0f;
    matrix->m10 = 0.0f;
    matrix->m11 = (z_near + z_near) / (top - bottom);
    matrix->m12 = 0.0f;
    matrix->m13 = 0.0f;
    matrix->m20 = (right + left) / (left - right);
    matrix->m21 = (top + bottom) / (bottom - top);
    matrix->m22 = z_far / (z_far - z_near);
    matrix->m23 = 1.0f;
    matrix->m30 = 0.0f;
    matrix->m31 = 0.0f;
    matrix->m32 = (-z_far * z_near) / (z_far - z_near);
    matrix->m33 = 0.0f;
}

void __cdecl NuMtxSetFrustumBlend(NUMTX *matrix, f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far) {
    matrix->m00 = (z_near + z_near) / (right - left);
    matrix->m01 = 0.0f;
    matrix->m02 = 0.0f;
    matrix->m03 = 0.0f;
    matrix->m10 = 0.0f;
    matrix->m11 = (z_near + z_near) / (top - bottom);
    matrix->m12 = 0.0f;
    matrix->m13 = 0.0f;
    matrix->m20 = (right + left) / (left - right);
    matrix->m21 = (top + bottom) / (bottom - top);
    matrix->m22 = (z_far + z_near) / (z_far - z_near);
    matrix->m23 = 1.0f;
    matrix->m30 = 0.0f;
    matrix->m31 = 0.0f;
    matrix->m32 = (z_far * -2.0f * z_near) / (z_far - z_near);
    matrix->m33 = 0.0f;
}

void __cdecl NuMtxGetFrustumD3D(NUMTX *matrix, f32 *left, f32 *right, f32 *bottom, f32 *top, f32 *z_near_out,
                                f32 *z_far_out) {
    f32 q = matrix->m22;
    f32 z_near = -matrix->m32 / q;

    if (z_far_out != NULL) {
        *z_far_out = (q * z_near) / (q - 1.0f);
    }
    if (left != NULL) {
        *left = ((-1.0f - matrix->m20) * z_near) / matrix->m00;
    }
    if (right != NULL) {
        *right = ((1.0f - matrix->m20) * z_near) / matrix->m00;
    }
    if (bottom != NULL) {
        *bottom = ((-1.0f - matrix->m21) * z_near) / matrix->m11;
    }
    if (top != NULL) {
        *top = ((1.0f - matrix->m21) * z_near) / matrix->m11;
    }
    if (z_near_out != NULL) {
        *z_near_out = z_near;
    }
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

void __cdecl NuMtxPreRotateX(NUMTX *m, NUANG angle) {
    f32 cos_x = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_x = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m10 = m->m10;
    f32 m11 = m->m11;
    f32 m12 = m->m12;

    m->m10 = cos_x * m10 + m->m20 * sin_x;
    m->m11 = cos_x * m11 + m->m21 * sin_x;
    m->m12 = cos_x * m12 + m->m22 * sin_x;
    m->m20 = m->m20 * cos_x - sin_x * m10;
    m->m21 = m->m21 * cos_x - sin_x * m11;
    m->m22 = m->m22 * cos_x - sin_x * m12;
}

void __cdecl NuMtxPreRotateY(NUMTX *m, NUANG angle) {
    f32 cos_y = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_y = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m01 = m->m01;
    f32 m02 = m->m02;

    m->m00 = cos_y * m00 - m->m20 * sin_y;
    m->m01 = cos_y * m01 - m->m21 * sin_y;
    m->m02 = cos_y * m02 - m->m22 * sin_y;
    m->m20 = sin_y * m00 + m->m20 * cos_y;
    m->m21 = sin_y * m01 + m->m21 * cos_y;
    m->m22 = sin_y * m02 + m->m22 * cos_y;
}

void __cdecl NuMtxPreRotateZ(NUMTX *m, NUANG angle) {
    f32 cos_z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_z = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m01 = m->m01;
    f32 m02 = m->m02;

    m->m00 = cos_z * m00 + m->m10 * sin_z;
    m->m01 = cos_z * m01 + m->m11 * sin_z;
    m->m02 = cos_z * m02 + m->m12 * sin_z;
    m->m10 = m->m10 * cos_z - sin_z * m00;
    m->m11 = m->m11 * cos_z - sin_z * m01;
    m->m12 = m->m12 * cos_z - sin_z * m02;
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

void __cdecl NuMtxToQuat(NUMTX *m, NUQUAT *out) {
    f32 elements[3][3];
    i32 next[3];
    f32 q[4];
    f32 s;
    f32 trace;
    i32 i;
    i32 j;
    i32 k;

    elements[0][0] = m->m00;
    elements[0][1] = m->m01;
    elements[0][2] = m->m02;
    elements[1][0] = m->m10;
    elements[1][1] = m->m11;
    elements[1][2] = m->m12;
    elements[2][0] = m->m20;
    elements[2][1] = m->m21;
    elements[2][2] = m->m22;
    next[0] = 1;
    next[1] = 2;
    next[2] = 0;
    trace = elements[0][0] + elements[1][1] + elements[2][2];

    if (trace > 0.0f) {
        s = NuFsqrt(trace + 1.0f);
        out->w = s * 0.5f;
        s = 0.5f / s;
        out->x = (elements[1][2] - elements[2][1]) * s;
        out->y = (elements[2][0] - elements[0][2]) * s;
        out->z = (elements[0][1] - elements[1][0]) * s;
    } else {
        i = 0;
        if (elements[1][1] > elements[0][0]) {
            i = 1;
        }
        if (elements[2][2] > elements[i][i]) {
            i = 2;
        }
        j = next[i];
        k = next[j];
        s = NuFsqrt((elements[i][i] - (elements[j][j] + elements[k][k])) + 1.0f);
        q[i] = s * 0.5f;
        if (s != 0.0f) {
            s = 0.5f / s;
        }
        q[3] = (elements[j][k] - elements[k][j]) * s;
        q[j] = (elements[i][j] + elements[j][i]) * s;
        q[k] = (elements[i][k] + elements[k][i]) * s;
        out->x = q[0];
        out->y = q[1];
        out->z = q[2];
        out->w = q[3];
    }
}

#define NU_SIN_PC(angle) NuTrigTable[((angle) >> 1) & 0x7fff]
#define NU_COS_PC(angle) NuTrigTable[(((angle) + 0x4000) >> 1) & 0x7fff]

void __cdecl NuMtxSetRotateXYZ(NUMTX *m, NUANGVEC *a) {
    f32 cosx = NU_COS_PC(a->x);
    f32 sinx = NU_SIN_PC(a->x);
    f32 cosy = NU_COS_PC(a->y);
    f32 siny = NU_SIN_PC(a->y);
    f32 cosz = NU_COS_PC(a->z);
    f32 sinz = NU_SIN_PC(a->z);

    m->m00 = cosy * cosz;
    m->m01 = cosy * sinz;
    m->m02 = -siny;
    m->m03 = 0.0f;
    m->m10 = sinx * siny * cosz - cosx * sinz;
    m->m11 = sinx * siny * sinz + cosx * cosz;
    m->m12 = sinx * cosy;
    m->m13 = 0.0f;
    m->m20 = cosx * siny * cosz + sinx * sinz;
    m->m21 = cosx * siny * sinz - sinx * cosz;
    m->m22 = cosx * cosy;
    m->m23 = 0.0f;
    m->m30 = 0.0f;
    m->m31 = 0.0f;
    m->m32 = 0.0f;
    m->m33 = 1.0f;
}

void __cdecl NuMtxSetRotateXYZVU0(NUMTX *matrix, NUANGVEC *angles) {
    NuMtxSetRotateXYZ(matrix, angles);
}

void __cdecl NuMtxPreScaleVU0(NUMTX *matrix, NUVEC *scale) {
    NuMtxPreScale(matrix, scale);
}

void __cdecl NuMtxMul(NUMTX *m, NUMTX *m0, NUMTX *m1) {
    NUMTX gm;

    if ((m == m0) || (m == m1)) {
        gm.m00 = m0->m00 * m1->m00 + m0->m01 * m1->m10 + m0->m02 * m1->m20;
        gm.m01 = m0->m00 * m1->m01 + m0->m01 * m1->m11 + m0->m02 * m1->m21;
        gm.m02 = m0->m00 * m1->m02 + m0->m01 * m1->m12 + m0->m02 * m1->m22;
        gm.m03 = 0.0f;
        gm.m10 = m0->m10 * m1->m00 + m0->m11 * m1->m10 + m0->m12 * m1->m20;
        gm.m11 = m0->m10 * m1->m01 + m0->m11 * m1->m11 + m0->m12 * m1->m21;
        gm.m12 = m0->m10 * m1->m02 + m0->m11 * m1->m12 + m0->m12 * m1->m22;
        gm.m13 = 0.0f;
        gm.m20 = m0->m20 * m1->m00 + m0->m21 * m1->m10 + m0->m22 * m1->m20;
        gm.m21 = m0->m20 * m1->m01 + m0->m21 * m1->m11 + m0->m22 * m1->m21;
        gm.m22 = m0->m20 * m1->m02 + m0->m21 * m1->m12 + m0->m22 * m1->m22;
        gm.m23 = 0.0f;
        gm.m30 = m0->m30 * m1->m00 + m0->m31 * m1->m10 + m0->m32 * m1->m20 + m1->m30;
        gm.m31 = m0->m30 * m1->m01 + m0->m31 * m1->m11 + m0->m32 * m1->m21 + m1->m31;
        gm.m32 = m0->m30 * m1->m02 + m0->m31 * m1->m12 + m0->m32 * m1->m22 + m1->m32;
        gm.m33 = 1.0f;
        *m = gm;
    } else {
        m->m00 = m0->m00 * m1->m00 + m0->m01 * m1->m10 + m0->m02 * m1->m20;
        m->m01 = m0->m00 * m1->m01 + m0->m01 * m1->m11 + m0->m02 * m1->m21;
        m->m02 = m0->m00 * m1->m02 + m0->m01 * m1->m12 + m0->m02 * m1->m22;
        m->m03 = 0.0f;
        m->m10 = m0->m10 * m1->m00 + m0->m11 * m1->m10 + m0->m12 * m1->m20;
        m->m11 = m0->m10 * m1->m01 + m0->m11 * m1->m11 + m0->m12 * m1->m21;
        m->m12 = m0->m10 * m1->m02 + m0->m11 * m1->m12 + m0->m12 * m1->m22;
        m->m13 = 0.0f;
        m->m20 = m0->m20 * m1->m00 + m0->m21 * m1->m10 + m0->m22 * m1->m20;
        m->m21 = m0->m20 * m1->m01 + m0->m21 * m1->m11 + m0->m22 * m1->m21;
        m->m22 = m0->m20 * m1->m02 + m0->m21 * m1->m12 + m0->m22 * m1->m22;
        m->m23 = 0.0f;
        m->m30 = m0->m30 * m1->m00 + m0->m31 * m1->m10 + m0->m32 * m1->m20 + m1->m30;
        m->m31 = m0->m30 * m1->m01 + m0->m31 * m1->m11 + m0->m32 * m1->m21 + m1->m31;
        m->m32 = m0->m30 * m1->m02 + m0->m31 * m1->m12 + m0->m32 * m1->m22 + m1->m32;
        m->m33 = 1.0f;
    }
}

void __cdecl NuMtxMulR(NUMTX *m, NUMTX *m0, NUMTX *m1) {
    NUMTX gm;

    if ((m == m0) || (m == m1)) {
        gm.m00 = m0->m00 * m1->m00 + m0->m01 * m1->m10 + m0->m02 * m1->m20;
        gm.m01 = m0->m00 * m1->m01 + m0->m01 * m1->m11 + m0->m02 * m1->m21;
        gm.m02 = m0->m00 * m1->m02 + m0->m01 * m1->m12 + m0->m02 * m1->m22;
        gm.m03 = 0.0;
        gm.m10 = m0->m10 * m1->m00 + m0->m11 * m1->m10 + m0->m12 * m1->m20;
        gm.m11 = m0->m10 * m1->m01 + m0->m11 * m1->m11 + m0->m12 * m1->m21;
        gm.m12 = m0->m10 * m1->m02 + m0->m11 * m1->m12 + m0->m12 * m1->m22;
        gm.m13 = 0.0;
        gm.m20 = m0->m20 * m1->m00 + m0->m21 * m1->m10 + m0->m22 * m1->m20;
        gm.m21 = m0->m20 * m1->m01 + m0->m21 * m1->m11 + m0->m22 * m1->m21;
        gm.m22 = m0->m20 * m1->m02 + m0->m21 * m1->m12 + m0->m22 * m1->m22;
        gm.m23 = 0.0;
        gm.m30 = gm.m31 = gm.m32 = 0.0f;
        gm.m33 = 1.0;
        *m = gm;
    } else {
        m->m00 = m0->m00 * m1->m00 + m0->m01 * m1->m10 + m0->m02 * m1->m20;
        m->m01 = m0->m00 * m1->m01 + m0->m01 * m1->m11 + m0->m02 * m1->m21;
        m->m02 = m0->m00 * m1->m02 + m0->m01 * m1->m12 + m0->m02 * m1->m22;
        m->m03 = 0.0;
        m->m10 = m0->m10 * m1->m00 + m0->m11 * m1->m10 + m0->m12 * m1->m20;
        m->m11 = m0->m10 * m1->m01 + m0->m11 * m1->m11 + m0->m12 * m1->m21;
        m->m12 = m0->m10 * m1->m02 + m0->m11 * m1->m12 + m0->m12 * m1->m22;
        m->m13 = 0.0;
        m->m20 = m0->m20 * m1->m00 + m0->m21 * m1->m10 + m0->m22 * m1->m20;
        m->m21 = m0->m20 * m1->m01 + m0->m21 * m1->m11 + m0->m22 * m1->m21;
        m->m22 = m0->m20 * m1->m02 + m0->m21 * m1->m12 + m0->m22 * m1->m22;
        m->m23 = 0.0;
        m->m30 = m->m31 = m->m32 = 0.0f;
        m->m33 = 1.0;
    }
}

NUVEC *__cdecl NuMtxGetScale(NUVEC *out, NUMTX *matrix) {
    out->x = NuFsqrt(matrix->m00 * matrix->m00 + matrix->m01 * matrix->m01 + matrix->m02 * matrix->m02);
    out->y = NuFsqrt(matrix->m10 * matrix->m10 + matrix->m11 * matrix->m11 + matrix->m12 * matrix->m12);
    out->z = NuFsqrt(matrix->m20 * matrix->m20 + matrix->m21 * matrix->m21 + matrix->m22 * matrix->m22);
    return out;
}

void __cdecl NuMtxVecToEulerXYZ(NUVEC *x_vector, NUVEC *z_vector, NUANG *x, NUANG *y, NUANG *z) {
    NUVEC local_x;
    NUVEC local_z;
    NUANG negative_z;
    *z = NuAtan2D(x_vector->y, x_vector->x);
    negative_z = -*z;
    NuMtxRotateAng(negative_z, x_vector->x, x_vector->y, &local_x.x, &local_x.y);
    local_x.z = x_vector->z;
    NuMtxRotateAng(negative_z, z_vector->x, z_vector->y, &local_z.x, &local_z.y);
    local_z.z = z_vector->z;
    *y = -NuAtan2D(local_x.z, local_x.x);
    NuMtxRotateAng(*y, local_z.x, local_z.z, &local_z.x, &local_z.z);
    *x = -NuAtan2D(local_z.y, local_z.z);
}
void __cdecl NuMtxGetEulerXYZ(NUMTX *matrix, NUANG *x, NUANG *y, NUANG *z) {
    NUVEC x_vector;
    NUVEC z_vector;
    x_vector.x = matrix->m00;
    x_vector.y = matrix->m01;
    x_vector.z = matrix->m02;
    z_vector.x = matrix->m20;
    z_vector.y = matrix->m21;
    z_vector.z = matrix->m22;
    NuMtxVecToEulerXYZ(&x_vector, &z_vector, x, y, z);
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

void __cdecl NuMtxScaleVU0(NUMTX *matrix, NUVEC *scale) {
    NuMtxScale(matrix, scale);
}

void __cdecl NuMtxMulH(NUMTX *result, NUMTX *left, NUMTX *right) {
    D3DXMatrixMultiply((D3DXMATRIX *)result, (D3DXMATRIX *)left, (D3DXMATRIX *)right);
}

void __cdecl NuMtxMulRVU0(NUMTX *result, NUMTX *left, NUMTX *right) {
    NuMtxMulR(result, left, right);
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
    matrix->m30 = matrix->m31 = matrix->m32 = 0.0f;
    matrix->m33 = 1.0f;
}
