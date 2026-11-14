#ifndef SAGA_LEGOAPI_GIZMOS_DOOR_PUSH_H
#define SAGA_LEGOAPI_GIZMOS_DOOR_PUSH_H

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

typedef struct PUSHPROGRESS PUSHPROGRESS;

struct PUSHPROGRESS {
    u32 visible_mask[1];
    u32 state_mask[1];
    u32 position_mask[1];
    NUVEC positions[16];
    NUVEC end_positions[2][16];
};

STATIC_ASSERT(sizeof(struct PUSHPROGRESS) == 588);

#endif
