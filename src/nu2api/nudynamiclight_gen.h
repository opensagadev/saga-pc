#ifndef SAGA_NU2API_NUDYNAMICLIGHT_GEN_H
#define SAGA_NU2API_NUDYNAMICLIGHT_GEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuplane.h"

typedef struct pc_frustum_group_pc_s {
    u8 unknown_0000[0x1a0];
    NUPLANE planes[12];
    i32 plane_count;
    u8 unknown_0264[0x11c];
} PC_FRUSTUM_GROUP_PC;

STATIC_ASSERT(offsetof(PC_FRUSTUM_GROUP_PC, planes) == 0x1a0);
STATIC_ASSERT(offsetof(PC_FRUSTUM_GROUP_PC, plane_count) == 0x260);
STATIC_ASSERT(sizeof(PC_FRUSTUM_GROUP_PC) == 0x380);

#endif
