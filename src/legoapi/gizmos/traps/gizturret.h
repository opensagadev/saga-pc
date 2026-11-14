/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/hint.h"
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
