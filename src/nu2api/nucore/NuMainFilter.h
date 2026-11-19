#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuMainFilterGen.h"

struct PC_FRAMEBUFFER;

#ifdef __cplusplus

class NuMainFilter : public NuMainFilterGen { // size 0x160
  public:
    NuMainFilter();
    void RenderBloom();
    void ResizeTextures(i32 width, i32 height);
    virtual void initResources();
    virtual void initTextureResources(i32, i32);
    virtual void destroyResources();
    virtual void destroyTextureResources();
    virtual void render();
    PC_EFFECT_SURFACE texture_surface;                // 0x128
    PC_FRAMEBUFFER *platform_fbo;                     // 0x144
    struct POST_SHADER_PROGRAM *platform_programs[2]; // 0x148
    i32 platform_uniforms[4];                         // 0x150
};

#endif
