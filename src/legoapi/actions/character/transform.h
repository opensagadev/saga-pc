#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/numtx.h"

#ifdef __cplusplus
extern "C" {
#endif
    void QuatInterpolateRotationMatrix(NUMTX *, NUMTX *, NUMTX *, f32);
    GAMEOBJECT *Transform_TargettedByObj(void *);
    void TransformDrawTarget(NUVEC *, f32, f32);
    void GizmoBlowup_TransformDraw_Game(struct GIZMOBLOWUP *target);
    void Transform_MoveCode(struct WORLDINFO_s *, struct gameobject_s *);
#ifdef __cplusplus
}
#endif
