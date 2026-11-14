/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

struct NUGSCN_s;

typedef struct nugspline_s { // size 0xc
    i16 length;              // 0x0
    u16 pt_size;             // 0x2
    char *name;              // 0x4
    NUVEC *pts;              // 0x8
} NUGSPLINE;

#ifdef __cplusplus
extern "C" {
#endif
    NUGSPLINE *NuSplineFind(struct NUGSCN_s *, char *);
    i32 NuSplineFindAllBeg(struct NUGSCN_s *scene, char *name, NUGSPLINE **results, i32 capacity);
    extern const char AITransferLiteral7577dc[8];
#ifdef __cplusplus
}
#endif
