/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuplane.h"
#endif

typedef struct pc_frustum_group_s { // size 0x380
    u8 unknown_0000[0x1a0];         // 0x000
    NUPLANE planes[12];             // 0x1a0
    i32 plane_count;                // 0x260
    u8 unknown_0264[0x11c];         // 0x264
} PC_FRUSTUM_GROUP;
