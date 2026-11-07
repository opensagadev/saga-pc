#include "saga.h"

/* nu3d's two texture helpers that live outside the platform unit; the rest of nutex_pc
 * is in nu3d_platform_pc.cpp. */

extern "C" {
    extern u32 PCGraphicsPostEffectWidthPc;
    extern u32 PCGraphicsPostEffectHeightPc;
    extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectWorkTexturePc;
}

extern "C" void __cdecl PCGraphicsCreatePostEffectWorkTexture(i32 width, i32 height) {
    (void)width;
    (void)height;
    PCGraphicsPostEffectWorkTexturePc =
        PCGraphicsCreateEffectTexture(PCGraphicsPostEffectWidthPc, PCGraphicsPostEffectHeightPc, 1, 0x11, 2);
}

extern "C" i32 __cdecl NuTexCreateFramebufferCopy(VARIPTR *buffer, VARIPTR end) {
    NuEffectTexLockVP(buffer, end);
    PC_FRAMEBUFFER_PC *framebuffer = NuFramebufferGetObject(1);
    i32 width = NuFramebufferGetWidth(framebuffer);
    i32 height = NuFramebufferGetHeight(framebuffer);
    GRAPHICS_RENDER_TEXTURE_PC *texture = PCGraphicsCreateEffectTexture(width, height, 1, 1, 1);
    NuEffectTexUnlockVP();
    return NuTexRegisterRenderTargetPc(texture);
}
