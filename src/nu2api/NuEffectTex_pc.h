/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void PCGraphicsDestroyRenderTexture(GRAPHICS_RENDER_TEXTURE *texture);
    void PCGraphicsInitRenderTexture(GRAPHICS_RENDER_TEXTURE *, i32, i32, i32, i32, i32, i32);
    void PCGraphicsResetRenderTexture(GRAPHICS_RENDER_TEXTURE *texture);
    void PCGraphicsResizeEffectTexture(GRAPHICS_RENDER_TEXTURE *, i32, i32, i32);
    void PCGraphicsReleaseEffectTextures(void);
    void PCGraphicsRestoreEffectTextures(i32);
    void PCGraphicsBindEffectTexture(GRAPHICS_RENDER_TEXTURE *, u32);
    i32 PCGraphicsEffectTextureWidth(GRAPHICS_RENDER_TEXTURE *);
    i32 PCGraphicsEffectTextureHeight(GRAPHICS_RENDER_TEXTURE *);
    void PCGraphicsEffectTextureDimensions(GRAPHICS_RENDER_TEXTURE *, u16, i32 *, i32 *);
    void PCGraphicsBindFramebuffer(PC_FRAMEBUFFER *framebuffer);
    PC_FRAMEBUFFER *PCGraphicsGetBoundFramebuffer(void);
    void PCGraphicsEnableFramebufferGuards(PC_FRAMEBUFFER *, i32);
#ifdef __cplusplus
    u32 *PCGraphicsEffectTextureSampler(GRAPHICS_RENDER_TEXTURE *);
    void PCGraphicsFramebufferBarrier(i32, i32);
    void PCGraphicsFramebufferAction(i32);
#ifdef __cplusplus
}
#endif
#endif
