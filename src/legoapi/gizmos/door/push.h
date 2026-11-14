/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

typedef struct PUSHPROGRESS PUSHPROGRESS;

struct PUSHPROGRESS {           // size 0x24c
    u32 visible_mask[1];        // 0x000
    u32 state_mask[1];          // 0x004
    u32 position_mask[1];       // 0x008
    NUVEC positions[16];        // 0x00c
    NUVEC end_positions[2][16]; // 0x0cc
};
