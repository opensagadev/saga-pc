#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuPostFilter.h"

struct PC_FRAMEBUFFER;

#ifdef __cplusplus

class NuCopyFilter : public NuPostFilter { // size 0x14
  public:
    virtual void initResources();
    virtual void destroyResources();
    virtual void reset();
    virtual void render(PC_FRAMEBUFFER *);
    PC_FRAMEBUFFER *copy_fbo;              // 0x0c
    GRAPHICS_RENDER_TEXTURE *copy_texture; // 0x10
};

#endif
