#ifndef SAGA_LEGOAPI_ACTIONS_COMBAT_WHIP_H
#define SAGA_LEGOAPI_ACTIONS_COMBAT_WHIP_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void Whip_Release(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif

#endif
