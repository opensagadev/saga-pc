#include "saga.h"

i32 __cdecl NuPostEffectIsInitialised(u32 mask) {
    return (NuPostEffectInitialisedFlagsPc & mask) != 0;
}

extern i32 NuPostEffectBackBufferFramePc;
extern GRAPHICS_RENDER_TEXTURE_PC *NuPostEffectBackBufferTexturePc;
extern GRAPHICS_RENDER_TEXTURE_PC *NuPostEffectBackBufferCopyPc;
PC_FRAMEBUFFER_PC *__cdecl PCGraphicsGetBoundFramebufferPc(void);
void __cdecl PCGraphicsEnableFramebufferGuardsPc(PC_FRAMEBUFFER_PC *, i32);
void __cdecl PCGraphicsCopyFramebufferTex2DPc(i32, GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32, i32, i32);

GRAPHICS_RENDER_TEXTURE_PC *__cdecl NuPostEffectGetBackBuffer(i32 frame) {
    PC_FRAMEBUFFER_PC *bound;
    i32 width, height;
    NuPostEffectBackBufferCopyPc = NuPostEffectBackBufferTexturePc;
    if (NuPostEffectBackBufferFramePc == frame && frame != -1)
        return NuPostEffectBackBufferCopyPc;
    if (frame != -1)
        NuPostEffectBackBufferFramePc = frame;
    bound = PCGraphicsGetBoundFramebufferPc();
    NuPostEffectBackBufferCopyPc = NuPostEffectBackBufferTexturePc;
    PCGraphicsEnableFramebufferGuardsPc(bound, 1);
    height = PCGraphicsEffectTextureHeight(NuPostEffectBackBufferCopyPc);
    width = PCGraphicsEffectTextureWidth(NuPostEffectBackBufferCopyPc);
    PCGraphicsCopyFramebufferTex2DPc(0, NuPostEffectBackBufferCopyPc, 0, 0, 0, width, height);
    PCGraphicsEnableFramebufferGuardsPc(bound, 0);
    return NuPostEffectBackBufferCopyPc;
}
