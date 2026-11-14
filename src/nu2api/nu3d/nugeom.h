#ifndef SAGA_NU2API_NU3D_NUGEOM_H
#define SAGA_NU2API_NU3D_NUGEOM_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/numtl.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/numtl.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/numtl.h"
#endif

typedef struct nugeom_s {
    struct nugeom_s *next;
    NUMTL *mtl;
} NUGEOM;

STATIC_ASSERT(offsetof(NUGEOM, mtl) == 4);

#endif
