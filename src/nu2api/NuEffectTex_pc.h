#ifndef SAGA_NU2API_NUEFFECTTEX_PC_H
#define SAGA_NU2API_NUEFFECTTEX_PC_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void PCGraphicsDestroyRenderTexture(GRAPHICS_RENDER_TEXTURE_PC *texture);
    void PCGraphicsInitRenderTexture(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32, i32, i32, i32);
    void PCGraphicsResetRenderTexture(GRAPHICS_RENDER_TEXTURE_PC *texture);
    void PCGraphicsResizeEffectTexture(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32);
    void PCGraphicsReleaseEffectTextures(void);
    void PCGraphicsRestoreEffectTextures(i32);
    void PCGraphicsBindEffectTexture(GRAPHICS_RENDER_TEXTURE_PC *, u32);
    i32 PCGraphicsEffectTextureWidth(GRAPHICS_RENDER_TEXTURE_PC *);
    i32 PCGraphicsEffectTextureHeight(GRAPHICS_RENDER_TEXTURE_PC *);
    void PCGraphicsEffectTextureDimensions(GRAPHICS_RENDER_TEXTURE_PC *, u16, i32 *, i32 *);
    void PCGraphicsBindFramebuffer(PC_FRAMEBUFFER_PC *framebuffer);
    PC_FRAMEBUFFER_PC *PCGraphicsGetBoundFramebufferPc(void);
    void PCGraphicsEnableFramebufferGuardsPc(PC_FRAMEBUFFER_PC *, i32);
#ifdef __cplusplus
    u32 *PCGraphicsEffectTextureSamplerPc(GRAPHICS_RENDER_TEXTURE_PC *);
    void PCGraphicsFramebufferBarrierPc(i32, i32);
    void PCGraphicsFramebufferActionPc(i32);
#ifdef __cplusplus
}
#endif
#endif

#endif
