/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "MechInputTouch/MechInputTouch_types.h"
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus

/* The per-frame object interface ThingManager dispatches (Android BaseThing).
 * flags holds the THING_FLAG_SKIP_* bits; profiling brackets the call with
 * the time bar. */
class BaseThing {
  public:
    BaseThing();
    virtual ~BaseThing();
    virtual char const *GetName() = 0;

    virtual i32 RemoveDependancies(ThingRemoveData *) {
        return 1;
    }

    virtual void EnterLevel(ThingLevelData *) {
    }

    virtual void ExitLevel(ThingLevelData *) {
    }

    virtual void Reset(ThingResetData *) {
    }

    virtual void Process(ThingProcessData *) {
    }

    virtual void ProcessEvenWhenPaused(ThingProcessData *);

    virtual void ProcessOnlyWhenPaused(ThingProcessData *) {
    }

    virtual void Render(ThingRenderData *) {
    }

    virtual void Display(ThingRenderData *);

    virtual void Effects(ThingRenderData *) {
    }

    i32 fun_00633400(i32 action);

    u32 id;
    u32 flags;
    i32 profiling;
};
#endif
