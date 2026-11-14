/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/numtl.h"
#endif

typedef struct nugeom_s {  // size 0x8
    struct nugeom_s *next; // 0x0
    NUMTL *mtl;            // 0x4
} NUGEOM;
