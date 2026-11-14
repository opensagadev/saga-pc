#ifndef SAGA_NU2API_NUMATH_NUVEC4_H
#define SAGA_NU2API_NUMATH_NUVEC4_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

struct numtx_s;

typedef struct NUVEC4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
#ifdef __cplusplus
    NUVEC operator-(const NUVEC4 &other) const {
        __declspec(align(16)) NUVEC result;
        result.x = x - other.x;
        result.y = y - other.y;
        result.z = z - other.z;
        return result;
    }

    void fun_00595250(f32 new_x, f32 new_y, f32 new_z, f32 new_w);
    void fun_00595270();
    NUVEC4 &fun_00595290(const NUVEC &v);
#endif
} NUVEC4;

STATIC_ASSERT(sizeof(NUVEC4) == 16);
STATIC_ASSERT(offsetof(NUVEC4, w) == 12);

typedef struct NUHALFVEC4 {
    i16 x;
    i16 y;
    i16 z;
    i16 w;
} NUHALFVEC4;

STATIC_ASSERT(sizeof(NUHALFVEC4) == 8);
STATIC_ASSERT(offsetof(NUHALFVEC4, x) == 0);
STATIC_ASSERT(offsetof(NUHALFVEC4, y) == 2);
STATIC_ASSERT(offsetof(NUHALFVEC4, z) == 4);
STATIC_ASSERT(offsetof(NUHALFVEC4, w) == 6);
#ifdef __cplusplus
extern "C" {
#endif
    void NuVec4MtxRotateVU0(NUVEC4 *, NUVEC4 *, struct numtx_s *);
    void NuVec4MtxTransformVU0x3(NUVEC4 *out, NUVEC4 *in, struct numtx_s *matrix);
    void NuVec4MtxTransform(NUVEC4 *, NUVEC *, struct numtx_s *);
    void NuVec4MtxTransformVU0(NUVEC4 *, NUVEC4 *, struct numtx_s *);
    void NuVec4MtxTransformH(NUVEC4 *, NUVEC4 *, struct numtx_s *);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(sizeof(NUVEC4) == 0x10);
STATIC_ASSERT(offsetof(NUVEC4, x) == 0);
STATIC_ASSERT(offsetof(NUVEC4, y) == 4);
STATIC_ASSERT(offsetof(NUVEC4, z) == 8);
STATIC_ASSERT(offsetof(NUVEC4, w) == 0xc);
#ifdef __cplusplus
extern "C" {
#endif
    void NuVec4Add(NUVEC4 *, NUVEC4 *, NUVEC4 *);
    f32 NuVec4Dot(NUVEC4 *v0, NUVEC4 *v1);
    void NuVec4ScaleAccum(NUVEC4 *out, NUVEC4 *v, f32 scale);
    void NuVec4Sub(NUVEC4 *out, NUVEC4 *a, NUVEC4 *b);
    void NuVec4Max(NUVEC4 *out, NUVEC4 *a, NUVEC4 *b);
    void NuVec4Min(NUVEC4 *out, NUVEC4 *a, NUVEC4 *b);
    f32 NuVec4Mag(NUVEC4 *v);
    f32 NuVec4MagSqr(NUVEC4 *v);
    void NuVec4Lerp(NUVEC4 *out, NUVEC4 *a, NUVEC4 *b, f32 t);
    void NuVec4ToNuHalfVec4(NUVEC4 *v, NUHALFVEC4 *out);
    void NuHalfVec4ToNuVec4(NUHALFVEC4 *v, NUVEC4 *out);
    void NuVec4MtxTransformHVU0(struct NUVEC4 *out, struct NUVEC4 *v, struct numtx_s *matrix);
#ifdef __cplusplus
}
#endif

#endif
