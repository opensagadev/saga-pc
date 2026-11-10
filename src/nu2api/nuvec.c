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

/* Retail is a hand-written SSE block: four points per iteration, three outputs (x, y, z blocks) and one outcode
 * vector. Outcodes: 32 near, 16 far, 1/2 left/right (|x| against z*zx), 8/4 bottom/top (|y| against z*zy). */

void __cdecl NuVec4MtxTransformVU0(NUVEC4 *out, NUVEC4 *v, NUMTX *m) {
    f32 y = v->x * m->m01 + v->y * m->m11 + v->z * m->m21 + v->w * m->m31;
    f32 z = v->x * m->m02 + v->y * m->m12 + v->z * m->m22 + v->w * m->m32;
    f32 w = v->x * m->m03 + v->y * m->m13 + v->z * m->m23 + v->w * m->m33;
    out->x = v->x * m->m00 + v->y * m->m10 + v->z * m->m20 + v->w * m->m30;
    out->y = y;
    out->z = z;
    out->w = w;
}

void __cdecl NuVec4MtxTransformVU0x3(NUVEC4 *out, NUVEC4 *in, NUMTX *m) {
    i32 i;
    for (i = 0; i < 3; ++i) {
        f32 y = ((in->y * m->m11 + in->x * m->m01) + (in->z * m->m21)) + in->w * m->m31;
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
