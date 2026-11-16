#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizflow.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct trigger_system_s TRIGGER_SYSTEM;
    i32 GameObjectUsingLever(GAMEOBJECT *, void *);
    void AITriggerSetSysProcess(TRIGGER_SYSTEM *);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern const char DeathStarEscapeBWindowClean[12];
    extern const char DeathStarEscapeBWasher2[9];
    extern const char DeathStarEscapeBWasher1[9];
    void AITriggerSetSysReset(TRIGGER_SYSTEM *);
    AITRIGGERSET_s *AITriggerSetCreate(AITRIGGERSETSYS_s *, FLOWBOX *);
    void AITriggerSysAutoSetUp(WORLDINFO *, AITRIGGERSETSYS_s *);
#ifdef __cplusplus
}
#endif
