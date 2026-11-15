#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

typedef union nuplane_s { // size 0x10

    struct {   // size 0x10
        f32 a; // 0x0
        f32 b; // 0x4
        f32 c; // 0x8
        f32 d; // 0xc
    };

    struct {          // size 0x10
        NUVEC normal; // 0x0
        f32 distance; // 0xc
    };
} NUPLANE;

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
