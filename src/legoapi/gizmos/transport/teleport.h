#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 Teleport_UpdateHints(HINT *);
    void Teleports_Configure(WORLDINFO *, char *);
    typedef struct TELEPORT_MOTION TELEPORT_MOTION;
    TELEPORT_MOTION *Teleport_Find(GAMEOBJECT *, f32);
    void Teleport_MoveCode(GAMEOBJECT *, i32);
    void Teleports_Reset(WORLDINFO *);
    void Teleports_UpdateBeforeGameObjects(WORLDINFO *);
    void Teleports_UpdateAfterGameObjects(WORLDINFO *);
    void Teleport_NetMoveCode(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif
