/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/numtx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void QuatInterpolateRotationMatrix(NUMTX *, NUMTX *, NUMTX *, f32);
    GAMEOBJECT *Transform_TargettedByObj(void *);
#ifdef __cplusplus
}
#endif
