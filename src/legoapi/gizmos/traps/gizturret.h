#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/menus/core/hint.h"

struct GIZTURRETSYS_s;

#ifdef __cplusplus
extern "C" {
#endif
    i32 GizTurrets_UpdateHint(HINT *);
    void GizTurrets_OpponentSelection(struct GIZTURRETSYS_s *, i32, APIOBJECT **, i32, APIOBJECT **);
    extern i32 gizturret_test_ang;
    extern f32 gizturret_rapid_fire_rate;
    ADDGIZMOTYPE *GizTurrets_RegisterGizmo(i32);
    u32 GizTurrets_TotalScore(WORLDINFO *world);
#ifdef __cplusplus
}
#endif
