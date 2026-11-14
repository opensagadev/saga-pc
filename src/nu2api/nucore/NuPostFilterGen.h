/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
/* Extend/replace existing C++ accessed-field classes; private proposal only. */
class NuPostFilterGen { // size 0xc
  public:
    NuPostFilterGen() : enabled(false), input_fbo(NULL) {
    }

    virtual ~NuPostFilterGen();
    virtual void initResources();
    virtual void destroyResources();
    virtual void initTextureResources(i32, i32);
    virtual void destroyTextureResources();
    virtual void render();
    virtual void reset();
    virtual void resetAll();
    virtual bool isEnabled();
    virtual PC_FRAMEBUFFER_PC *getInputFbo();
    void fun_005c22b0();
    static void copy(GRAPHICS_RENDER_TEXTURE_PC *, PC_FRAMEBUFFER_PC *);
    static void copy(GRAPHICS_RENDER_TEXTURE_PC *, GRAPHICS_RENDER_TEXTURE_PC *, PC_FRAMEBUFFER_PC *);
    bool enabled;                 // 0x4
    u8 unknown_005[3];            // 0x5
    PC_FRAMEBUFFER_PC *input_fbo; // 0x8
};

#endif
