#ifndef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZTURRET_H
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZTURRET_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/legoapi_types.h (not all of it earlier in
 * the shim). */

#include "legoapi/menus/core/hint.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 GizTurrets_UpdateHint(HINT_PC *);
    void GizTurrets_OpponentSelection(struct GIZTURRETSYS_s *, i32, APIOBJECT_PC **, i32, APIOBJECT_PC **);
    extern i32 gizturret_test_ang;
    extern f32 gizturret_rapid_fire_rate;
#ifdef __cplusplus
}
#endif

#endif
