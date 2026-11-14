#ifndef SAGA_LEGOAPI_GIZMO_BASE_GIZBLOWUPOBJECTINTERFACE_H
#define SAGA_LEGOAPI_GIZMO_BASE_GIZBLOWUPOBJECTINTERFACE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"
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
