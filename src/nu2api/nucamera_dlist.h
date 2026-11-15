#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"

struct GRAPHICS_RENDER_TEXTURE;
struct PC_TEXTURE_RESOURCE;

#ifdef __cplusplus
extern "C" {
#endif
    HRESULT PCDeviceCreateVertexShader(const DWORD *, struct PC_TEXTURE_RESOURCE **);
    HRESULT PCDeviceCreatePixelShader(const DWORD *, struct PC_TEXTURE_RESOURCE **);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 RenderFrameShaderTimePc;
    extern void (*RenderFrameCallback)(void);
    HRESULT PCDeviceEndScenePc(void);
    i32 NuTexRegisterRenderTarget(struct GRAPHICS_RENDER_TEXTURE *target);
    void NuGobjRegistryCleanupPc(void);
    void NuGobjRegistryInitPc(void);
#ifdef __cplusplus
    void PostFilterCopyTexture(GRAPHICS_RENDER_TEXTURE *, i32, GRAPHICS_RENDER_TEXTURE *, i32, POST_SHADER_PROGRAM *,
                               GRAPHICS_RENDER_TEXTURE *);
    POST_SHADER_PROGRAM *PostShaderProgramCreatePc(i32, const DWORD *, const DWORD *);
    extern POST_SHADER_PROGRAM *PostBlendTexProgram;
    extern POST_SHADER_PROGRAM *PostBlur5x5Program;
    extern POST_SHADER_PROGRAM *PostBlur7x7Program;
    extern POST_SHADER_PROGRAM *PostBlurAuxProgram;
    extern POST_SHADER_PROGRAM *PostCopyAuxProgram;
    extern POST_SHADER_PROGRAM *PostCopyTexProgram;
    void PostFilterInitPrograms(i32, i32);
#ifdef __cplusplus
}
#endif
#endif
