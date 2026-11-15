/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/menus/core/hint.h"
#endif

struct GIZTURRETSYS_s;

#ifdef __cplusplus
extern "C" {
#endif
    i32 GizTurrets_UpdateHint(HINT *);
    void GizTurrets_OpponentSelection(struct GIZTURRETSYS_s *, i32, APIOBJECT **, i32, APIOBJECT **);
    extern i32 gizturret_test_ang;
    extern f32 gizturret_rapid_fire_rate;
#ifdef __cplusplus
}
#endif
