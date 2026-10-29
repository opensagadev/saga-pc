#include "saga.h"
#include <string.h>
#include <math.h>

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

__declspec(noinline) void __cdecl NuMtxPreScale(NUMTX *m, NUVEC *s) {
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

    m->m01 = (m01 * cos_x) - (m->m02 * sin_x);
    m->m02 = (m01 * sin_x) + (m->m02 * cos_x);
    m->m11 = (m11 * cos_x) - (m->m12 * sin_x);
    m->m12 = (m11 * sin_x) + (m->m12 * cos_x);
    m->m21 = (m21 * cos_x) - (m->m22 * sin_x);
    m->m22 = (m21 * sin_x) + (m->m22 * cos_x);
    m->m31 = (m31 * cos_x) - (m->m32 * sin_x);
    m->m32 = (m31 * sin_x) + (m->m32 * cos_x);
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

    m->m00 = (m00 * cos_z) - (m->m01 * sin_z);
    m->m01 = (m00 * sin_z) + (m->m01 * cos_z);
    m->m10 = (m10 * cos_z) - (m->m11 * sin_z);
    m->m11 = (m10 * sin_z) + (m->m11 * cos_z);
    m->m20 = (m20 * cos_z) - (m->m21 * sin_z);
    m->m21 = (m20 * sin_z) + (m->m21 * cos_z);
    m->m30 = (m30 * cos_z) - (m->m31 * sin_z);
    m->m31 = (m30 * sin_z) + (m->m31 * cos_z);
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

typedef struct NuMtxIndexedViewPc {
    f32 elements[4][4];
} NuMtxIndexedViewPc;

STATIC_ASSERT(sizeof(NuMtxIndexedViewPc) == sizeof(NUMTX));
STATIC_ASSERT(offsetof(NuMtxIndexedViewPc, elements) == 0);
STATIC_ASSERT(sizeof(((NuMtxIndexedViewPc *)0)->elements[0]) == 0x10);
STATIC_ASSERT(sizeof(((NuMtxIndexedViewPc *)0)->elements[0]) + sizeof(f32) == offsetof(NUMTX, m11));
STATIC_ASSERT(2 * sizeof(((NuMtxIndexedViewPc *)0)->elements[0]) + 2 * sizeof(f32) == offsetof(NUMTX, m22));

void __cdecl NuMtxToQuat(NUMTX *m, NUQUAT *out) {
    NuMtxIndexedViewPc *indexed = (NuMtxIndexedViewPc *)m;
    i32 next[3];
    f32 q[4];
    f32 s;
    f32 trace;
    i32 i;
    i32 j;
    i32 k;

    next[0] = 1;
    next[1] = 2;
    next[2] = 0;
    trace = m->m00 + m->m11 + m->m22;

    if (trace > 0.0f) {
        s = NuFsqrt(trace + 1.0f);
        out->w = s * 0.5f;
        s = 0.5f / s;
        out->x = (m->m12 - m->m21) * s;
        out->y = (m->m20 - m->m02) * s;
        out->z = (m->m01 - m->m10) * s;
    } else {
        i = 0;
        if (m->m11 > m->m00) {
            i = 1;
        }
        if (m->m22 > indexed->elements[i][i]) {
            i = 2;
        }
        j = next[i];
        k = next[j];
        s = NuFsqrt((indexed->elements[i][i] - (indexed->elements[j][j] + indexed->elements[k][k])) + 1.0f);
        q[i] = s * 0.5f;
        if (s != 0.0f) {
            s = 0.5f / s;
        }
        q[3] = (indexed->elements[j][k] - indexed->elements[k][j]) * s;
        q[j] = (indexed->elements[i][j] + indexed->elements[j][i]) * s;
        q[k] = (indexed->elements[i][k] + indexed->elements[k][i]) * s;
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

void NuEulerXYZFromQuat(NUANG *x, NUANG *y, NUANG *z, NUQUAT *input) {
    NUQUAT q = *input;
    f32 sin_y = -2.0f * (q.x * q.z - q.w * q.y);
    if (sin_y < -1.0f) {
        sin_y = -1.0f;
    }
    if (sin_y > 1.0f) {
        sin_y = 1.0f;
    }

    *y = (i16)NuASin(sin_y);
    *x = NuAtan2D(2.0f * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
    *z = NuAtan2D(2.0f * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z);
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

void __cdecl NuMtxInvH(NUMTX *mi, NUMTX *m0) {
    i32 p[4];
    i32 j;
    i32 k;
    i32 i;
    f32 a[4][4];
    i32 n;
    f32 local_1c;
    f32 localm20;
    f32 localm28;
    f32 localm30;
    f32 pivot_magnitude;

    n = 4;

    a[0][0] = m0->m00;
    a[0][1] = m0->m01;
    a[0][2] = m0->m02;
    a[0][3] = m0->m03;
    a[1][0] = m0->m10;
    a[1][1] = m0->m11;
    a[1][2] = m0->m12;
    a[1][3] = m0->m13;
    a[2][0] = m0->m20;
    a[2][1] = m0->m21;
    a[2][2] = m0->m22;
    a[2][3] = m0->m23;
    a[3][0] = m0->m30;
    a[3][1] = m0->m31;
    a[3][2] = m0->m32;
    a[3][3] = m0->m33;

    for (i = 0; i < n; i++) {
        localm20 = 0.0f;
        p[i] = 0;

        for (j = i; j < n; j++) {
            local_1c = 0.0f;

            for (k = i; k < n; k++) {
                local_1c += fabsf(a[j][k]);
            }

            pivot_magnitude = fabsf(a[j][i]);
            localm28 = pivot_magnitude == 0.0f || local_1c == 0.0f ? 0.0f : pivot_magnitude / local_1c;

            if (localm20 < localm28) {
                localm20 = localm28;
                p[i] = j;
            }
        }

        if (localm20 == 0.0f) {
            return;
        }

        if (p[i] != i) {
            for (k = 0; k < n; k++) {
                f32 f;

                f = a[i][k];
                a[i][k] = a[p[i]][k];
                a[p[i]][k] = f;
            }
        }

        localm30 = a[i][i];

        for (k = 0; k < n; k++) {
            if (k != i) {
                a[i][k] = -a[i][k] / localm30;
                for (j = 0; j < n; j++) {
                    if (j != i) {
                        a[j][k] += a[j][i] * a[i][k];
                    }
                }
            }
        }

        for (j = 0; j < n; j++) {
            a[j][i] = a[j][i] / localm30;
        }

        a[i][i] = 1.0f / localm30;
    }

    for (i = n - 1; i >= 0; i--) {
        if (p[i] != i) {
            for (j = 0; j < n; j++) {
                f32 f;

                f = a[j][i];
                a[j][i] = a[j][p[i]];
                a[j][p[i]] = f;
            }
        }
    }

    mi->m00 = a[0][0];
    mi->m01 = a[0][1];
    mi->m02 = a[0][2];
    mi->m03 = a[0][3];
    mi->m10 = a[1][0];
    mi->m11 = a[1][1];
    mi->m12 = a[1][2];
    mi->m13 = a[1][3];
    mi->m20 = a[2][0];
    mi->m21 = a[2][1];
    mi->m22 = a[2][2];
    mi->m23 = a[2][3];
    mi->m30 = a[3][0];
    mi->m31 = a[3][1];
    mi->m32 = a[3][2];
    mi->m33 = a[3][3];
}

STATIC_ASSERT(offsetof(NUMTX, m00) == 0x00);
STATIC_ASSERT(offsetof(NUMTX, m10) == 0x10);
STATIC_ASSERT(offsetof(NUMTX, m20) == 0x20);

void __cdecl NuMtxAlignY(NUMTX *m, NUVEC *v) {
    f32 s;
    m->m10 = v->x;
    m->m11 = v->y;
    m->m12 = v->z;
    m->m00 = m->m11 * m->m22 - m->m12 * m->m21;
    m->m01 = m->m12 * m->m20 - m->m10 * m->m22;
    m->m02 = m->m10 * m->m21 - m->m11 * m->m20;
    s = NuFsqrt(m->m00 * m->m00 + m->m01 * m->m01 + m->m02 * m->m02);
    s = s == 0.0f ? 0.0f : 1.0f / s;
    m->m00 *= s;
    m->m01 *= s;
    m->m02 *= s;
    m->m20 = m->m01 * m->m12 - m->m02 * m->m11;
    m->m21 = m->m02 * m->m10 - m->m00 * m->m12;
    m->m22 = m->m00 * m->m11 - m->m01 * m->m10;
}
void __cdecl NuMtxAlignZ(NUMTX *m, NUVEC *v) {
    f32 x_squared = m->m00 * m->m00 + m->m01 * m->m01 + m->m02 * m->m02;
    f32 y_squared = m->m10 * m->m10 + m->m11 * m->m11 + m->m12 * m->m12;
    f32 z_squared = m->m20 * m->m20 + m->m21 * m->m21 + m->m22 * m->m22;
    f32 denominator = v->x * v->x + v->y * v->y + v->z * v->z;
    f32 quotient = z_squared == 0.0f || denominator == 0.0f ? 0.0f : z_squared / denominator;
    f32 len = NuFsqrt(quotient);
    f32 dot;
    m->m20 = v->x * len;
    m->m21 = v->y * len;
    m->m22 = v->z * len;
    dot = m->m10 * m->m20 + m->m11 * m->m21 + m->m12 * m->m22;
    if (fabsf(dot) > 0.86602540378f) {
        NuVecCross((NUVEC *)&m->m10, (NUVEC *)&m->m20, (NUVEC *)&m->m00);
        denominator = m->m10 * m->m10 + m->m11 * m->m11 + m->m12 * m->m12;
        quotient = y_squared == 0.0f || denominator == 0.0f ? 0.0f : y_squared / denominator;
        len = NuFsqrt(quotient);
        m->m10 *= len;
        m->m11 *= len;
        m->m12 *= len;
        NuVecCross((NUVEC *)&m->m00, (NUVEC *)&m->m10, (NUVEC *)&m->m20);
        denominator = m->m00 * m->m00 + m->m01 * m->m01 + m->m02 * m->m02;
        quotient = x_squared == 0.0f || denominator == 0.0f ? 0.0f : x_squared / denominator;
        len = NuFsqrt(quotient);
        m->m00 *= len;
        m->m01 *= len;
        m->m02 *= len;
    } else {
        NuVecCross((NUVEC *)&m->m00, (NUVEC *)&m->m10, (NUVEC *)&m->m20);
        denominator = m->m00 * m->m00 + m->m01 * m->m01 + m->m02 * m->m02;
        quotient = x_squared == 0.0f || denominator == 0.0f ? 0.0f : x_squared / denominator;
        len = NuFsqrt(quotient);
        m->m00 *= len;
        m->m01 *= len;
        m->m02 *= len;
        NuVecCross((NUVEC *)&m->m10, (NUVEC *)&m->m20, (NUVEC *)&m->m00);
        denominator = m->m10 * m->m10 + m->m11 * m->m11 + m->m12 * m->m12;
        quotient = y_squared == 0.0f || denominator == 0.0f ? 0.0f : y_squared / denominator;
        len = NuFsqrt(quotient);
        m->m10 *= len;
        m->m11 *= len;
        m->m12 *= len;
    }
}
void __cdecl NuMtxLookAtY(NUMTX *m, NUVEC *point) {
    NUVEC v;
    v.x = point->x - m->m30;
    v.y = point->y - m->m31;
    v.z = point->z - m->m32;
    NuVecNorm(&v, &v);
    NuMtxAlignY(m, &v);
}
void __cdecl NuMtxLookAtZ(NUMTX *m, NUVEC *point) {
    NUVEC v;
    v.x = point->x - m->m30;
    v.y = point->y - m->m31;
    v.z = point->z - m->m32;
    NuVecNorm(&v, &v);
    NuMtxAlignZ(m, &v);
}

void __cdecl PcGetGeometryPerspectivePc(NUMTX *matrix, f32 *fovy, f32 *aspect, f32 *z_near, f32 *z_far) {
    f32 q = matrix->m22;
    f32 near_value = (f32)(-(f64)matrix->m32 / q);
    f32 reciprocal_y, radians;
    *z_near = near_value;
    *z_far = (f32)(((f64)near_value * q) / ((f64)q - 1.0));
    *aspect = matrix->m11 / matrix->m00;
    reciprocal_y = 1.0f / matrix->m11;
    radians = (f32)((f64)(i32)NuAtan2D(reciprocal_y, 1.0f) * PCAtan2RadianScale);
    *fovy = (f32)((f64)radians * 360.0 / (f64)3.1415927410125732);
}

void __cdecl NuMtxInvRSSH(NUMTX *inv, NUMTX *T) {
    NUMTX gm;

    f32 tx = -T->m30;
    f32 ty = -T->m31;
    f32 tz = -T->m32;
    f32 det = T->m00 * (T->m11 * T->m22 - T->m12 * T->m21) - T->m01 * (T->m22 * T->m10 - T->m12 * T->m20) +
              T->m02 * (T->m10 * T->m21 - T->m11 * T->m20);
    f32 invdet = 1.0f / det;

    gm.m00 = (T->m11 * T->m22 - T->m12 * T->m21) * invdet;
    gm.m10 = (T->m22 * T->m10 - T->m12 * T->m20) * -invdet;
    gm.m20 = (T->m10 * T->m21 - T->m11 * T->m20) * invdet;
    gm.m01 = (T->m22 * T->m01 - T->m21 * T->m02) * -invdet;
    gm.m11 = (T->m00 * T->m22 - T->m20 * T->m02) * invdet;
    gm.m21 = (T->m00 * T->m21 - T->m01 * T->m20) * -invdet;
    gm.m02 = (T->m01 * T->m12 - T->m11 * T->m02) * invdet;
    gm.m12 = (T->m00 * T->m12 - T->m10 * T->m02) * -invdet;
    gm.m22 = (T->m00 * T->m11 - T->m01 * T->m10) * invdet;
    gm.m30 = gm.m00 * tx + gm.m10 * ty + gm.m20 * tz;
    gm.m31 = gm.m01 * tx + gm.m11 * ty + gm.m21 * tz;
    gm.m32 = gm.m02 * tx + gm.m12 * ty + gm.m22 * tz;
    gm.m03 = 0.0f;
    gm.m13 = 0.0f;
    gm.m23 = 0.0f;
    gm.m33 = 1.0f;
    *inv = gm;
}

void __cdecl NuMtxAlignX(NUMTX *m, NUVEC *v) {
    f32 s;
    m->m00 = v->x;
    m->m01 = v->y;
    m->m02 = v->z;
    m->m20 = m->m01 * m->m12 - m->m02 * m->m11;
    m->m21 = m->m02 * m->m10 - m->m00 * m->m12;
    m->m22 = m->m11 * m->m00 - m->m01 * m->m10;

    s = NuFsqrt(m->m20 * m->m20 + m->m21 * m->m21 + m->m22 * m->m22);
    s = s == 0.0f ? 0.0f : 1.0f / s;

    m->m20 = m->m20 * s;
    m->m21 = m->m21 * s;
    m->m22 = m->m22 * s;
    m->m10 = m->m21 * m->m02 - m->m22 * m->m01;
    m->m11 = m->m22 * m->m00 - m->m20 * m->m02;
    m->m12 = m->m20 * m->m01 - m->m21 * m->m00;
}

void __cdecl NuMtxLookAtX(NUMTX *m, NUVEC *pnt) {
    NUVEC v;

    v.x = pnt->x - m->m30;
    v.y = pnt->y - m->m31;
    v.z = pnt->z - m->m32;

    NuVecNorm(&v, &v);
    NuMtxAlignX(m, &v);
}

void __cdecl NuMtxInvLookAtX(NUMTX *m, NUVEC *pnt) {
    NUVEC v;

    v.x = m->m30 - pnt->x;
    v.y = m->m31 - pnt->y;
    v.z = m->m32 - pnt->z;

    NuVecNorm(&v, &v);
    NuMtxAlignX(m, &v);
}

void __cdecl NuMtxInvLookAtY(NUMTX *m, NUVEC *pnt) {
    NUVEC v;

    v.x = m->m30 - pnt->x;
    v.y = m->m31 - pnt->y;
    v.z = m->m32 - pnt->z;

    NuVecNorm(&v, &v);
    NuMtxAlignY(m, &v);
}

void __cdecl NuMtxInvLookAtZ(NUMTX *m, NUVEC *pnt) {
    NUVEC v;

    v.x = m->m30 - pnt->x;
    v.y = m->m31 - pnt->y;
    v.z = m->m32 - pnt->z;

    NuVecNorm(&v, &v);
    NuMtxAlignZ(m, &v);
}

void __cdecl NuMtxAddR(NUMTX *m, NUMTX *m0, NUMTX *m1) {
    m->m00 = m0->m00 + m1->m00;
    m->m01 = m0->m01 + m1->m01;
    m->m02 = m0->m02 + m1->m02;
    m->m03 = 0.0f;
    m->m10 = m0->m10 + m1->m10;
    m->m11 = m0->m11 + m1->m11;
    m->m12 = m0->m12 + m1->m12;
    m->m13 = 0.0f;
    m->m20 = m0->m20 + m1->m20;
    m->m21 = m0->m21 + m1->m21;
    m->m22 = m0->m22 + m1->m22;
    m->m23 = 0.0f;
    m->m30 = 0.0f;
    m->m31 = 0.0f;
    m->m32 = 0.0f;
    m->m33 = 1.0f;
}

void __cdecl NuMtxSubR(NUMTX *m, NUMTX *m0, NUMTX *m1) {
    m->m00 = m0->m00 - m1->m00;
    m->m01 = m0->m01 - m1->m01;
    m->m02 = m0->m02 - m1->m02;
    m->m03 = 0.0f;
    m->m10 = m0->m10 - m1->m10;
    m->m11 = m0->m11 - m1->m11;
    m->m12 = m0->m12 - m1->m12;
    m->m13 = 0.0f;
    m->m20 = m0->m20 - m1->m20;
    m->m21 = m0->m21 - m1->m21;
    m->m22 = m0->m22 - m1->m22;
    m->m23 = 0.0f;
    m->m30 = 0.0f;
    m->m31 = 0.0f;
    m->m32 = 0.0f;
    m->m33 = 1.0f;
}

void __cdecl NuMtxSkewSymmetric(NUMTX *m, NUVEC *v) {
    m->m00 = 0.0;
    m->m01 = -v->z;
    m->m02 = v->y;
    m->m03 = 0.0;
    m->m10 = v->z;
    m->m11 = 0.0;
    m->m12 = -v->x;
    m->m13 = 0.0;
    m->m20 = -v->y;
    m->m21 = v->x;
    m->m22 = 0.0;
    m->m23 = 0.0;
    m->m30 = 0.0;
    m->m31 = 0.0;
    m->m32 = 0.0;
    m->m33 = 1.0;
}

f32 __cdecl NuMtxDet3(NUMTX *m) {
    return m->m00 * (m->m11 * m->m22 - m->m12 * m->m21) - m->m01 * (m->m10 * m->m22 - m->m12 * m->m20) +
           m->m02 * (m->m10 * m->m21 - m->m11 * m->m20);
}

i32 __cdecl NuMtxCompare(NUMTX *a, NUMTX *b) {
    i32 i;
    f32 *aa = &a->m00;
    f32 *bb = &b->m00;

    for (i = 0; i < 16; i++) {
        if (*aa < *bb) {
            return -1;
        }
        if (*aa > *bb) {
            return 1;
        }
        aa += 1;
        bb += 1;
    }

    return 0;
}

void __cdecl NuMtxPreTranslateX(NUMTX *m, f32 tx) {
    m->m30 = m->m30 + m->m00 * tx;
    m->m31 = m->m31 + m->m01 * tx;
    m->m32 = m->m32 + m->m02 * tx;
}

void __cdecl NuMtxPreTranslateNeg(NUMTX *m, NUVEC *t) {
    m->m30 -= t->x * m->m00 + t->y * m->m10 + t->z * m->m20;
    m->m31 -= t->x * m->m01 + t->y * m->m11 + t->z * m->m21;
    m->m32 -= t->x * m->m02 + t->y * m->m12 + t->z * m->m22;
}

void __cdecl NuMtxPreSkewYX(NUMTX *Mtx, f32 SkewVal) {
    Mtx->m00 = Mtx->m00 + Mtx->m10 * SkewVal;
    Mtx->m01 = Mtx->m01 + Mtx->m11 * SkewVal;
    Mtx->m02 = Mtx->m02 + Mtx->m12 * SkewVal;
}

void __cdecl NuMtxPreRotateY180X(NUMTX *m, NUANG a) {
    f32 cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m10 = m->m10;
    f32 m11 = m->m11;
    f32 m12 = m->m12;

    m->m00 = -m->m00;
    m->m01 = -m->m01;
    m->m02 = -m->m02;
    m->m10 = cosx * m10 - m->m20 * sinx;
    m->m11 = cosx * m11 - m->m21 * sinx;
    m->m12 = cosx * m12 - m->m22 * sinx;
    m->m20 = -(m->m20 * cosx) - sinx * m10;
    m->m21 = -(m->m21 * cosx) - sinx * m11;
    m->m22 = -(m->m22 * cosx) - sinx * m12;
}

void __cdecl NuMtxPreRotateY180(NUMTX *m) {
    m->m00 = -m->m00;
    m->m01 = -m->m01;
    m->m02 = -m->m02;
    m->m20 = -m->m20;
    m->m21 = -m->m21;
    m->m22 = -m->m22;
}

void __cdecl NuMtxLookAtInverseD3D(NUMTX *mtx, NUVEC *eye, NUVEC *center, NUVEC *up) {
    NUVEC ax;
    NUVEC ay;
    NUVEC az;

    az.x = center->x - eye->x;
    az.y = center->y - eye->y;
    az.z = center->z - eye->z;
    NuVecNorm(&az, &az);
    ax.x = up->y * az.z - up->z * az.y;
    ax.y = up->z * az.x - up->x * az.z;
    ax.z = up->x * az.y - up->y * az.x;
    NuVecNorm(&ax, &ax);
    ay.x = az.y * ax.z - az.z * ax.y;
    ay.y = az.z * ax.x - az.x * ax.z;
    ay.z = az.x * ax.y - az.y * ax.x;
    NuVecNorm(&ay, &ay);

    mtx->m00 = ax.x;
    mtx->m01 = ax.y;
    mtx->m02 = ax.z;
    mtx->m03 = 0.0f;
    mtx->m10 = ay.x;
    mtx->m11 = ay.y;
    mtx->m12 = ay.z;
    mtx->m13 = 0.0f;
    mtx->m20 = az.x;
    mtx->m21 = az.y;
    mtx->m22 = az.z;
    mtx->m23 = 0.0f;
    mtx->m30 = eye->x;
    mtx->m31 = eye->y;
    mtx->m32 = eye->z;
    mtx->m33 = 1.0f;
}

void __cdecl NuMtxSetPerspectiveBlend(NUMTX *mtx, f32 fovy, f32 aspect, f32 zNear, f32 zFar) {
    i32 angle = (i32)((fovy / 2.0f) * (65536.0f / 360.0f));
    f32 tanFovy2 = NuTrigTable[(angle >> 1) & 0x7fff] / NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    mtx->m00 = 1.0f / (aspect * tanFovy2);
    mtx->m01 = 0.0f;
    mtx->m02 = 0.0f;
    mtx->m03 = 0.0f;
    mtx->m10 = 0.0f;
    mtx->m11 = 1.0f / tanFovy2;
    mtx->m12 = 0.0f;
    mtx->m13 = 0.0f;
    mtx->m20 = 0.0f;
    mtx->m21 = 0.0f;
    mtx->m22 = (zFar + zNear) / (zFar - zNear);
    mtx->m23 = 1.0f;
    mtx->m30 = 0.0f;
    mtx->m31 = 0.0f;
    mtx->m32 = (zFar * -2.0f * zNear) / (zFar - zNear);
    mtx->m33 = 0.0f;
}

void __cdecl NuMtxGetFrustumBlend(NUMTX *mtx, f32 *l, f32 *r, f32 *b, f32 *t, f32 *n, f32 *f) {
    f32 Q = (mtx->m22 + 1.0f) * 0.5f;
    f32 zNear = -(mtx->m32 * 0.5f) / Q;

    if (f != NULL) {
        *f = (Q * zNear) / (Q - 1.0f);
    }
    if (l != NULL) {
        *l = ((-1.0f - mtx->m20) * zNear) / mtx->m00;
    }
    if (r != NULL) {
        *r = ((1.0f - mtx->m20) * zNear) / mtx->m00;
    }
    if (b != NULL) {
        *b = ((-1.0f - mtx->m21) * zNear) / mtx->m11;
    }
    if (t != NULL) {
        *t = ((1.0f - mtx->m21) * zNear) / mtx->m11;
    }
    if (n != NULL) {
        *n = zNear;
    }
}

void __cdecl NuMtxTruncate24Bit(NUMTX *out, NUMTX *input) {
    union {
        f32 value;
        u32 bits;
    } packed;
    memcpy(&packed.bits, &input->m00, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m00, &packed.bits, 4);
    memcpy(&packed.bits, &input->m01, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m01, &packed.bits, 4);
    memcpy(&packed.bits, &input->m02, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m02, &packed.bits, 4);
    memcpy(&packed.bits, &input->m03, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m03, &packed.bits, 4);
    memcpy(&packed.bits, &input->m10, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m10, &packed.bits, 4);
    memcpy(&packed.bits, &input->m11, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m11, &packed.bits, 4);
    memcpy(&packed.bits, &input->m12, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m12, &packed.bits, 4);
    memcpy(&packed.bits, &input->m13, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m13, &packed.bits, 4);
    memcpy(&packed.bits, &input->m20, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m20, &packed.bits, 4);
    memcpy(&packed.bits, &input->m21, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m21, &packed.bits, 4);
    memcpy(&packed.bits, &input->m22, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m22, &packed.bits, 4);
    memcpy(&packed.bits, &input->m23, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m23, &packed.bits, 4);
    memcpy(&packed.bits, &input->m30, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m30, &packed.bits, 4);
    memcpy(&packed.bits, &input->m31, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m31, &packed.bits, 4);
    memcpy(&packed.bits, &input->m32, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m32, &packed.bits, 4);
    memcpy(&packed.bits, &input->m33, 4);
    packed.bits &= 0xffffff00u;
    memcpy(&out->m33, &packed.bits, 4);
}

void __cdecl NuMtx24BitCorrection(NUMTX *correction, NUMTX *matrix) {
    f32 best_error = 3.4028234663852886e+38f;
    i32 element, pass;
    f32 step = 1.0f;
    NUMTX truncated, product;
    NuMtxTruncate24Bit(&truncated, matrix);
    *correction = NuMtxSetIdentityTemplate;
    for (pass = 0; pass < 100; ++pass) {
        for (element = 0; element < 16; ++element) {
            f32 previous, error;
            f32 *value;
            union {
                f32 value;
                u32 bits;
            } packed;
            if (element % 4 == 3)
                continue;
            value = &correction->m00 + element;
            previous = *value;
            packed.value = *value + step;
            packed.bits &= 0xffffff00u;
            *value = packed.value;
            NuMtxMulH(&product, correction, &truncated);
            error = NuMtxSSE(&product, matrix);
            if (error >= best_error)
                *value = previous;
            else
                best_error = error;
            previous = *value;
            packed.value = *value - step;
            packed.bits &= 0xffffff00u;
            *value = packed.value;
            NuMtxMulH(&product, correction, &truncated);
            error = NuMtxSSE(&product, matrix);
            if (error >= best_error)
                *value = previous;
            else
                best_error = error;
        }
        step *= 0.8999999761581421;
    }
}
