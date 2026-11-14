#ifndef SAGA_LEGOAPI_ACTIONS_COMBAT_HITS_H
#define SAGA_LEGOAPI_ACTIONS_COMBAT_HITS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 ObjHitObj(GAMEOBJECT *, GAMEOBJECT *, i32, u16, i32, i32);
    i32 NoLayerKill(GAMEOBJECT *);
    u16 ObjHitObj_Flags(GAMEOBJECT *);
    i32 CannotKill(GAMEOBJECT *);
    void RegenerateHearts(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif

#endif
