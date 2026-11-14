#ifndef SAGA_NU2API_NU3D_NU2API_NU3D_TYPES_H
#define SAGA_NU2API_NU3D_NU2API_NU3D_TYPES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nugeom.h"
#include "nu2api/numath/nuvec.h"

typedef struct nugobj_s {
    u8 unknown_00[0xc];
    NUGEOM *geom;
    u8 unknown_10[4];
    f32 origin_radius;
    u8 unknown_18[4];
    NUVEC bounds_min;
    NUVEC bounds_max;
    NUVEC center;
    f32 radius;
    u8 unknown_44[8];
    struct nugobj_s *next;
} NUGOBJ;

STATIC_ASSERT(offsetof(NUGOBJ, geom) == 0xc);
STATIC_ASSERT(offsetof(NUGOBJ, next) == 0x4c);

#endif
