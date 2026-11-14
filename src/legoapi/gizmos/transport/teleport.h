#ifndef SAGA_LEGOAPI_GIZMOS_TRANSPORT_TELEPORT_H
#define SAGA_LEGOAPI_GIZMOS_TRANSPORT_TELEPORT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 Teleport_UpdateHints(HINT_PC *);
    void Teleports_Configure(WORLDINFO *, char *);
    typedef struct TELEPORT_MOTION_PC TELEPORT_MOTION_PC;
    TELEPORT_MOTION_PC *Teleport_Find(GAMEOBJECT *, f32);
    void Teleport_MoveCode(GAMEOBJECT *, i32);
    void Teleports_Reset(WORLDINFO *);
    void Teleports_UpdateBeforeGameObjects(WORLDINFO *);
    void Teleports_UpdateAfterGameObjects(WORLDINFO *);
    void Teleport_NetMoveCode(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif

#endif
