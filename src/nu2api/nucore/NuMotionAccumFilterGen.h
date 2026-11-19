#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuPostFilter.h"

struct PC_FRAMEBUFFER;

#ifdef __cplusplus

class NuMotionAccumFilterGen : public NuPostFilter { // size 0x28
  public:
    NuMotionAccumFilterGen() : frames(0), blend(1.0f), weight_exponent(0.0f) {
    }

    void setEnabled(bool enable);
    void SetAccumulation(i32 frames, f32 blend, f32 mode);
    f32 GetTiming(i32 *last_frame);
    virtual void initResources();
    virtual void destroyResources();
    virtual void initTextureResources(i32, i32);
    virtual void destroyTextureResources();
    virtual void render();
    void *program;                                 // 0x0c
    PC_FRAMEBUFFER *accumulation_fbo;              // 0x10
    GRAPHICS_RENDER_TEXTURE *accumulation_texture; // 0x14
    i32 frames;                                    // 0x18
    f32 blend;                                     // 0x1c
    f32 weight_exponent;                           // 0x20
    i32 current_frame;                             // 0x24
};

#endif
