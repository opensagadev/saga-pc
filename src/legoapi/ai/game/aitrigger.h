#ifndef SAGA_LEGOAPI_AI_GAME_AITRIGGER_H
#define SAGA_LEGOAPI_AI_GAME_AITRIGGER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/legoapi_types.h (not all of it earlier in
 * the shim). */

#include "legoapi/gizmo/base/gizflow.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
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

#endif
