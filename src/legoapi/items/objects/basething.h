/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

struct ThingProcessData;

#ifdef __cplusplus

/* The per-frame object interface ThingManager dispatches.
 * flags holds the THING_FLAG_SKIP_* bits; profiling brackets the call with
 * the time bar. */
struct ThingLevelData;
struct ThingRemoveData;
struct ThingRenderData;
struct ThingResetData;

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

    virtual void Process(struct ThingProcessData *) {
    }

    virtual void ProcessEvenWhenPaused(struct ThingProcessData *);

    virtual void ProcessOnlyWhenPaused(struct ThingProcessData *) {
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
