/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizflow.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct trigger_system_pc_s TRIGGER_SYSTEM_PC;
    i32 GameObjectUsingLever(GAMEOBJECT *, void *);
    void AITriggerSetSysProcess(TRIGGER_SYSTEM_PC *);
#ifdef __cplusplus
}
#endif
typedef struct trigger_system_pc_s TRIGGER_SYSTEM_PC;
#ifdef __cplusplus
extern "C" {
#endif
    extern const char DeathStarEscapeBWindowCleanPc[12];
    extern const char DeathStarEscapeBWasher2Pc[9];
    extern const char DeathStarEscapeBWasher1Pc[9];
    void AITriggerSetSysReset(TRIGGER_SYSTEM_PC *);
    AITRIGGERSET_s *AITriggerSetCreate(AITRIGGERSETSYS_s *, FLOWBOX_PC *);
    void AITriggerSysAutoSetUp(WORLDINFO *, AITRIGGERSETSYS_s *);
#ifdef __cplusplus
}
#endif
