#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/nuvec4.h"

#ifdef __cplusplus
extern "C" {
#endif
    GRAPHICS_RENDER_TEXTURE *PCGraphicsCreateColourTarget(i32, i32);
    GRAPHICS_RENDER_TEXTURE *PCGraphicsCreateColourTargetFormat(i32, i32, i32);
    void PCGraphicsInitRenderTexturePool(VARIPTR *buffer, VARIPTR end);
    void NuRndrGeometryBuffersInit(void);
    void NuRndrBufferDescriptorsInit(VARIPTR *, VARIPTR);

    GRAPHICS_RENDER_TEXTURE *PCGraphicsCreateEffectTexture(i32, i32, i32, i32, i32);
#ifdef __cplusplus
}
#endif
// NOLINTNEXTLINE(readability-identifier-naming)
#define PcBootSecondaryFramebuffer (PcBootFramebuffers.fields.secondary_framebuffer)
#ifdef __cplusplus
extern "C" {
#endif
    extern NUVEC4 NuDisplayHandlerTint;
    extern i32 NuDisplayHandlerReflection;

    extern f32 NuDisplayHandlerOpacity;
    extern i32 NuDisplayHandlerZMode;
    extern i32 PCGraphicsFrameState;
#ifdef __cplusplus
}
#endif
struct POST_SHADER_PROGRAM;
#ifdef __cplusplus
extern "C" {
#endif
    extern void *NuDisplayMaterialLast;
    extern NUVEC4 NuDisplayHandlerDefaultTint;
    GRAPHICS_RENDER_TEXTURE *PCGraphicsFrameRenderTexture(i32 texture_id);
    PC_FRAMEBUFFER *PCGraphicsCreateFramebuffer(void);
    extern f32 PostBlurDirectInputBias;
    extern f32 PostBlurDirectOutputBias;
    extern f32 PostBlurInputBias;
    void PCGraphicsAttachFramebuffer(PC_FRAMEBUFFER *, i32, GRAPHICS_RENDER_TEXTURE *, i32);
    void PCGraphicsCreatePostEffectTextures(u32, u32);
    void PCGraphicsDestroyFramebuffer(PC_FRAMEBUFFER *);
    void PCGraphicsReleaseFramebuffers(void);
    void PCGraphicsRestoreFramebuffers(void);
#ifdef __cplusplus
}
#endif
