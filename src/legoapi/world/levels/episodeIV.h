#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODEIV_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODEIV_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

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
