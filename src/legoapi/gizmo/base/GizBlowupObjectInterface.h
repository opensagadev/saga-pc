#ifndef SAGA_LEGOAPI_GIZMO_BASE_GIZBLOWUPOBJECTINTERFACE_H
#define SAGA_LEGOAPI_GIZMO_BASE_GIZBLOWUPOBJECTINTERFACE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/gizmo/object/gizmoblowups.h, legoapi/items/base/apiobject.h (not all of it
 * earlier in the shim). */

#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    GIZMOBLOWUP_PC *GizmoBlowUp_Target(GAMEOBJECT *, NUVEC *, NUVEC *, f32, f32, i32, i32, i32);
    GIZMOBLOWUP_PC *GizmoBlowUp_FindFromPlatID(WORLDINFO *, i32);
    GIZMOBLOWUP_PC *GizmoBlowUp_FindByName(WORLDINFO *, char *);
#ifdef __cplusplus
}
#endif

#endif
