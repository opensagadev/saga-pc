#ifndef SAGA_LEGOAPI_GIZMOS_FX_GUIDELINES_H
#define SAGA_LEGOAPI_GIZMOS_FX_GUIDELINES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

typedef struct indy_guideline_pc_s {
    char name[8];
    NUVEC endpoints[2];
    u32 flags;
} INDY_GUIDELINE_PC;

STATIC_ASSERT(sizeof(INDY_GUIDELINE_PC) == 0x24);
STATIC_ASSERT(offsetof(INDY_GUIDELINE_PC, endpoints) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    INDY_GUIDELINE_PC *GuideLine_FindNearest(NUVEC *position, WORLDINFO *world, i32 *endpoint, f32 *distance);
#ifdef __cplusplus
}
#endif

#endif
