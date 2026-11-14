/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/lib/hub.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 EpCompleteCount;
    extern i32 EpCompleteTotal;
    extern i32 EpBuildUpCount;
    extern i32 EpBuildUpTotal;
    extern i32 EpGoldBrickCount;
    extern i32 EpGoldBrickTotal;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
i32 Episode_CountOpenAreas(i32, i32, COLLECTION_AREA_SAVE_PC *);
#endif
