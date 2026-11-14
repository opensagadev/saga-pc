#ifndef SAGA_NU2API_NU3D_NUSPLINE_H
#define SAGA_NU2API_NU3D_NUSPLINE_H

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

struct NUGSCN_s;

typedef struct nugspline_s {
    i16 length;
    u16 pt_size;
    char *name;
    NUVEC *pts;
} NUGSPLINE;

STATIC_ASSERT(sizeof(NUGSPLINE) == 0xc);
STATIC_ASSERT(offsetof(NUGSPLINE, length) == 0);
STATIC_ASSERT(offsetof(NUGSPLINE, pt_size) == 2);
STATIC_ASSERT(offsetof(NUGSPLINE, name) == 4);
STATIC_ASSERT(offsetof(NUGSPLINE, pts) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    NUGSPLINE *NuSplineFind(struct NUGSCN_s *, char *);
    i32 NuSplineFindAllBeg(struct NUGSCN_s *scene, char *name, NUGSPLINE **results, i32 capacity);
    extern const char AITransferLiteral7577dc[8];
#ifdef __cplusplus
}
#endif

#endif
