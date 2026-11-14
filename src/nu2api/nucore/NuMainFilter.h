/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuMainFilterGen.h"
#include "nu2api/nustencilshadows_PC.h"
#endif

#ifdef __cplusplus

class NuMainFilter : public NuMainFilterGen {
  public:
    NuMainFilter();
    void RenderBloomPc();
    void ResizeTexturesPc(i32 width, i32 height);
    virtual void initResources();
    virtual void initTextureResources(i32, i32);
    virtual void destroyResources();
    virtual void destroyTextureResources();
    virtual void render();
    PC_EFFECT_SURFACE_PC texture_surface;
    PC_FRAMEBUFFER_PC *platform_fbo;
    void *platform_programs[2];
    i32 platform_uniforms[4];
};

STATIC_ASSERT(sizeof(NuMainFilter) == 0x160);
STATIC_ASSERT(offsetof(NuMainFilter, texture_surface) == 0x128);
STATIC_ASSERT(offsetof(NuMainFilter, platform_fbo) == 0x144);
STATIC_ASSERT(offsetof(NuMainFilter, platform_programs) == 0x148);
STATIC_ASSERT(offsetof(NuMainFilter, platform_uniforms) == 0x150);
#endif
