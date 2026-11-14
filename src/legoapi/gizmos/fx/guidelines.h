/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

struct WORLDINFO_s;

typedef struct indy_guideline_s { // size 0x24
    char name[8];                 // 0x00
    NUVEC endpoints[2];           // 0x08
    u32 flags;                    // 0x20
} INDY_GUIDELINE_PC;

#ifdef __cplusplus
extern "C" {
#endif
    INDY_GUIDELINE_PC *GuideLine_FindNearest(NUVEC *position, struct WORLDINFO_s *world, i32 *endpoint, f32 *distance);
#ifdef __cplusplus
}
#endif
