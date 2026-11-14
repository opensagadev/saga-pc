#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODEIV_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODEIV_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 MosEisleyC_PastBarrier(GAMEOBJECT *);
    extern f32 VehicleLoopTimePc;
    extern i8 *DeathStarEscapeBNetPacketPc;
    extern const char DeathStarEscapeBSpongePc[7];
    extern const char DeathStarEscapeBLiftDownPc[10];
    extern const char DeathStarEscapeBLiftUpPc[8];
    i32 DeathStarShieldDown(void);
#ifdef __cplusplus
}
#endif

#endif
