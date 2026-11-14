#ifndef SAGA_LEGOAPI_GIZMOS_FX_GUIDELINES_H
#define SAGA_LEGOAPI_GIZMOS_FX_GUIDELINES_H

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

struct WORLDINFO_s;

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
    INDY_GUIDELINE_PC *GuideLine_FindNearest(NUVEC *position, struct WORLDINFO_s *world, i32 *endpoint, f32 *distance);
#ifdef __cplusplus
}
#endif

#endif
