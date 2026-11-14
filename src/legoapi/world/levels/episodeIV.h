/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 MosEisleyC_PastBarrier(GAMEOBJECT *);
    extern f32 VehicleLoopTime;
    extern i8 *DeathStarEscapeBNetPacket;
    extern const char DeathStarEscapeBSponge[7];
    extern const char DeathStarEscapeBLiftDown[10];
    extern const char DeathStarEscapeBLiftUp[8];
    i32 DeathStarShieldDown(void);
#ifdef __cplusplus
}
#endif
