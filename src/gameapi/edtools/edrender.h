#ifndef SAGA_GAMEAPI_EDTOOLS_EDRENDER_H
#define SAGA_GAMEAPI_EDTOOLS_EDRENDER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void edbitsDrawDiagonalCross(f32 x, f32 y, f32 z, f32 radius, i32 colour, struct PC_SCENE_MTL_VIEW *material);
    void edbitsDrawCross(f32 x, f32 y, f32 z, f32 radius, i32 colour, struct PC_SCENE_MTL_VIEW *material);
#ifdef __cplusplus
}
#endif

#endif
