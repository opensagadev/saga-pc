#ifndef SAGA_NU2API_NUMATH_NUPLANE_H
#define SAGA_NU2API_NUMATH_NUPLANE_H

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

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

typedef union nuplane_s {
    struct {
        f32 a;
        f32 b;
        f32 c;
        f32 d;
    };

    struct {
        NUVEC normal;
        f32 distance;
    };
} NUPLANE;

STATIC_ASSERT(sizeof(NUPLANE) == 0x10);
STATIC_ASSERT(offsetof(NUPLANE, a) == 0);
STATIC_ASSERT(offsetof(NUPLANE, b) == 4);
STATIC_ASSERT(offsetof(NUPLANE, c) == 8);
STATIC_ASSERT(offsetof(NUPLANE, d) == 0xc);
STATIC_ASSERT(offsetof(NUPLANE, normal) == 0);
#ifdef __cplusplus
extern "C" {
#endif
    f32 NuInfiniteLineToPointDistSqrEx(NUVEC *, NUVEC *, NUVEC *, NUVEC *);
    f32 NuInfiniteLineToPointDistSqr(NUVEC *, NUVEC *, NUVEC *);
    f32 NuLineToPointDistSqrEx(NUVEC *, NUVEC *, NUVEC *, NUVEC *);
    void NuPlnEqn(NUPLANE *out, NUVEC *pnt0, NUVEC *pnt1, NUVEC *pnt2);
    i32 NuPlnPlnIntersect(NUPLANE *first, NUPLANE *second, NUVEC *point, NUVEC *direction);
    i32 NuPlnLine(NUPLANE *plane, NUVEC *s, NUVEC *e, NUVEC *out);
    f32 NuPlnLine3(NUPLANE *plane, NUVEC *pnt, NUVEC *v, NUVEC *out);
    i32 NuPtInPolyXY(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2);
    i32 NuPtInPolyYX(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2);
    i32 NuPtInPolyXZ(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2);
    i32 NuPtInPolyZX(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2);
    i32 NuPtInPolyYZ(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2);
    i32 NuPtInPolyZY(NUVEC *pnt, NUVEC *t0, NUVEC *t1, NUVEC *t2);
    f32 NuPlnDist2(NUPLANE *plane, NUVEC *a, NUVEC *b);
#ifdef __cplusplus
}
#endif

#endif
