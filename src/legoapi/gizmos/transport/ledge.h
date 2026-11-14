#ifndef SAGA_LEGOAPI_GIZMOS_TRANSPORT_LEDGE_H
#define SAGA_LEGOAPI_GIZMOS_TRANSPORT_LEDGE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/world/world.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void Ledge_MoveCode(WORLDINFO *, GAMEOBJECT *);
#ifdef __cplusplus
}
#endif

#endif
