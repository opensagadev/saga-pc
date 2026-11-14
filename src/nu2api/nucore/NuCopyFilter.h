/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuPostFilter.h"
#endif

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
