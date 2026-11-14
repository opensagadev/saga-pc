#ifndef SAGA_NU2API_NUCORE_NUPOSTFILTERGEN_H
#define SAGA_NU2API_NUCORE_NUPOSTFILTERGEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
/* Extend/replace existing C++ accessed-field classes; private proposal only. */
class NuPostFilterGen {
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
    bool enabled;
    u8 unknown_005[3];
    PC_FRAMEBUFFER_PC *input_fbo;
};

STATIC_ASSERT(sizeof(NuPostFilterGen) == 0xc);

STATIC_ASSERT(offsetof(NuPostFilterGen, enabled) == 4);
STATIC_ASSERT(offsetof(NuPostFilterGen, input_fbo) == 8);
#endif

#endif
