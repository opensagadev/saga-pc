#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    GIZMOBLOWUP_PC *GizmoBlowUp_Target(GAMEOBJECT *, NUVEC *, NUVEC *, f32, f32, i32, i32, i32);
    GIZMOBLOWUP_PC *GizmoBlowUp_FindFromPlatID(WORLDINFO *, i32);
    GIZMOBLOWUP_PC *GizmoBlowUp_FindByName(WORLDINFO *, char *);
#ifdef __cplusplus
}
#endif
