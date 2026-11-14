#ifndef SAGA_NU2API_NUCORE_NUCOPYFILTER_H
#define SAGA_NU2API_NUCORE_NUCOPYFILTER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuPostFilter.h"

#ifdef __cplusplus

class NuCopyFilter : public NuPostFilter {
  public:
    virtual void initResources();
    virtual void destroyResources();
    virtual void reset();
    virtual void render(PC_FRAMEBUFFER_PC *);
    PC_FRAMEBUFFER_PC *copy_fbo;
    GRAPHICS_RENDER_TEXTURE_PC *copy_texture;
};

STATIC_ASSERT(sizeof(NuCopyFilter) == 0x14);
#endif

#endif
