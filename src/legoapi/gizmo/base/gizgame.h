#ifndef SAGA_LEGOAPI_GIZMO_BASE_GIZGAME_H
#define SAGA_LEGOAPI_GIZMO_BASE_GIZGAME_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/collect/bolts.h (not all of it earlier in the shim). */

#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/collect/bolts.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 GizmoSys_BoltHit(void *, WORLDINFO *, BOLT_PC *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *);
#ifdef __cplusplus
}
#endif

#endif
