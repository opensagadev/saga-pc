#ifndef SAGA_NU2API_NUCORE_NUMOTIONACCUMFILTERGEN_H
#define SAGA_NU2API_NUCORE_NUMOTIONACCUMFILTERGEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuPostFilter.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus

class NuMotionAccumFilterGen : public NuPostFilter {
  public:
    NuMotionAccumFilterGen() : frames(0), blend(1.0f), mode_pc(0.0f) {
    }

    void fun_005c3100(bool enable);
    void fun_005c3110(i32 frames, f32 blend, f32 mode);
    f32 GetTiming(i32 *last_frame);
    virtual void initResources();
    virtual void destroyResources();
    virtual void initTextureResources(i32, i32);
    virtual void destroyTextureResources();
    virtual void render();
    void *program;
    PC_FRAMEBUFFER_PC *accumulation_fbo;
    GRAPHICS_RENDER_TEXTURE_PC *accumulation_texture;
    i32 frames;
    f32 blend;
    f32 mode_pc;
    i32 current_frame;
};

STATIC_ASSERT(sizeof(NuMotionAccumFilterGen) == 0x28);
#endif

#endif
