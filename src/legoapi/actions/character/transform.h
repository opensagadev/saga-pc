#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/numtx.h"

#ifdef __cplusplus
extern "C" {
#endif
    void QuatInterpolateRotationMatrix(NUMTX *, NUMTX *, NUMTX *, f32);
    GAMEOBJECT *Transform_TargettedByObj(void *);
#ifdef __cplusplus
}
#endif
