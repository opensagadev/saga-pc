/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

typedef struct TERRAIN_LAST_IMPACT { // size 0x10
    NUVEC position;                  // 0x0
    f32 hit_type;                    // 0xc
} TERRAIN_LAST_IMPACT;

#ifdef __cplusplus
extern "C" {
#endif
    extern TERRAIN_LAST_IMPACT TerrLastImpact;
    extern i32 WallSplinesOnly;
#ifdef __cplusplus
}
#endif
