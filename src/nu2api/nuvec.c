#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numaths.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuquat.h"
#include "nu2api/numath/nuvec4.h"
#include <xmmintrin.h>

/* Retail is a hand-written SSE block: four points per iteration, three outputs (x, y, z blocks) and one outcode
 * vector. Outcodes: 32 near, 16 far, 1/2 left/right (|x| against z*zx), 8/4 bottom/top (|y| against z*zy). */

/* Retail 0x5c1880 takes parent in ECX, rotation in EDX and the output on the
 * stack (caller cleanup); __fastcall is the closest non-LTCG convention. */
void __fastcall NuQuatMulAligned(const NUQUAT *parent, const NUQUAT *rotation, NUQUAT *out) {
    __m128 q = _mm_load_ps(&rotation->x);
    __m128 p = _mm_load_ps(&parent->x);
    __m128 a = q;
    __m128 b;
    __m128 c;
    __m128 d;
    __m128 lo0;
    __m128 lo1;
    __m128 hi1;
    __m128 sum;
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
