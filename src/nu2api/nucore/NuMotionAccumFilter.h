/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuMotionAccumFilterGen.h"
#include "nu2api/numath/numtx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuSpecialDrawAtAlpha(NUSPECIALHANDLE *, NUMTX *, f32);
    void RenderPostViewportSetPc(f32 source_x, f32 source_y, f32 source_right, f32 source_bottom, f32 dest_x,
                                 f32 dest_y, f32 dest_right, f32 dest_bottom);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

class NuMotionAccumFilter : public NuMotionAccumFilterGen {
  public:
    virtual void initResources();
};

STATIC_ASSERT(sizeof(NuMotionAccumFilter) == 0x28);
#endif
