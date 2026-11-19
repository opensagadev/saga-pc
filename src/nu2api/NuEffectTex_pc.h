#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"

struct PC_FRAMEBUFFER;

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
    i32 PCGraphicsEffectTextureMipCount(GRAPHICS_RENDER_TEXTURE *);
    void PCGraphicsClearFramebuffer(u32, i32);
    void PCGraphicsEffectTextureSetSamplerExtra(GRAPHICS_RENDER_TEXTURE *, i32);
    void PCGraphicsEffectTextureSetFilterFields(GRAPHICS_RENDER_TEXTURE *, i32, i32, i32);
    void PCGraphicsEffectTextureSetLowNibbles(GRAPHICS_RENDER_TEXTURE *, i32, i32);
    void PCGraphicsEffectTextureSetWrapFields(GRAPHICS_RENDER_TEXTURE *, i32, i32);
    void PCGraphicsResolveFramebuffer(i32);
    void fun_005f83a0(GRAPHICS_RENDER_TEXTURE *texture, i32 texture_id);
#ifdef __cplusplus
}
#endif
#endif
