#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODES_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/lib/hub.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

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

#endif
