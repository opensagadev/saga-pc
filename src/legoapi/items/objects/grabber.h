/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void Grabber_StoreProgress(WORLDINFO *world, LEVEL_PROGRESS *progress);

    extern i16 GrabMachineId;
    extern i16 GrabMagnetId;
    extern i16 RobotBaseId;
    void Grabber_Configure(WORLDINFO *, char *);
#ifdef __cplusplus
}
#endif
