#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nucore/NuMotionAccumFilterGen.h"
#include "nu2api/numath/numtx.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuSpecialDrawAtAlpha(nuhspecial_s *, NUMTX *, f32);
    void RenderPostViewportSet(f32 source_x, f32 source_y, f32 source_right, f32 source_bottom, f32 dest_x, f32 dest_y, f32 dest_right,
                               f32 dest_bottom);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

class NuMotionAccumFilter : public NuMotionAccumFilterGen { // size 0x28
  public:
    virtual void initResources();
};

#endif
