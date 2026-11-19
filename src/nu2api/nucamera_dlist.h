#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"

struct GRAPHICS_RENDER_TEXTURE;
struct PC_TEXTURE_RESOURCE;

#ifdef __cplusplus
extern "C" {
#endif
    /* Compiled Direct3D 9 shader token streams of the post-effect and display-handler programs. */
    extern const DWORD PostCopyTexVertexShader[42];
    extern const DWORD PostCopyTexPixelShader[43];
    extern const DWORD PostBlur5x5VertexShader[151];
    extern const DWORD PostBlur5x5PixelShader[126];
    extern const DWORD PostBlur7x7VertexShader[60];
    extern const DWORD PostBlurPixelShader[43];
    extern const DWORD PostBlurAuxVertexShader[60];
    extern const DWORD PostCopyAuxVertexShader[104];
    extern const DWORD PostCopyAuxPixelShader[130];
    extern const DWORD NuMainFilterVertexShader[168];
    extern const DWORD NuMainFilterPixelShader[94];
    extern const DWORD NuMainFilterPlatformPixelShader[75];
    extern const DWORD NuMainFilterVertexShader2[261];
    extern const DWORD NuMainFilterVertexShaderSw[168];
    extern const DWORD NuMainFilterPixelShaderSw[94];
    extern const DWORD NuMainFilterPlatformPixelShaderSw[75];
    extern const DWORD NuMainFilterVertexShader2Sw[261];
    extern const DWORD NuDeferredFilterVertexShader[85];
    extern const DWORD NuDeferredFilterPixelShader0[566];
    extern const DWORD NuDeferredFilterPixelShader1[554];
    extern const DWORD NuDeferredFilterPixelShader2[155];
    extern const DWORD NuDeferredFilterPixelShader3[143];
    extern const DWORD NuDeferredFilterVertexShader2[48];
    extern const DWORD NuSpeedBlurFilterVertexShader[180];
    extern const DWORD NuSpeedBlurFilterPixelShader[131];
    extern const DWORD NuSpeedBlurFilterVertexShaderSw[175];
    extern const DWORD NuSpeedBlurFilterPixelShaderSw[100];
    HRESULT PCDeviceCreateVertexShader(const DWORD *, struct PC_TEXTURE_RESOURCE **);
    HRESULT PCDeviceCreatePixelShader(const DWORD *, struct PC_TEXTURE_RESOURCE **);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 RenderFrameShaderTime;
    extern void (*RenderFrameCallback)(void);
    HRESULT PCDeviceEndScene(void);
    i32 NuTexRegisterRenderTarget(struct GRAPHICS_RENDER_TEXTURE *target);
    void NuGobjRegistryCleanup(void);
    void NuGobjRegistryInit(void);
#ifdef __cplusplus
    void PostFilterCopyTexture(GRAPHICS_RENDER_TEXTURE *, i32, GRAPHICS_RENDER_TEXTURE *, i32, POST_SHADER_PROGRAM *,
                               GRAPHICS_RENDER_TEXTURE *);
    POST_SHADER_PROGRAM *PostShaderProgramCreate(i32, const DWORD *, const DWORD *);
    extern POST_SHADER_PROGRAM *PostBlendTexProgram;
    extern POST_SHADER_PROGRAM *PostBlur5x5Program;
    extern POST_SHADER_PROGRAM *PostBlur7x7Program;
    extern POST_SHADER_PROGRAM *PostBlurAuxProgram;
    extern POST_SHADER_PROGRAM *PostCopyAuxProgram;
    extern POST_SHADER_PROGRAM *PostCopyTexProgram;
    void PostFilterInitPrograms(i32, i32);
    void PostBlur7x7Dispatch(GRAPHICS_RENDER_TEXTURE *source, i32 source_lod, GRAPHICS_RENDER_TEXTURE *textures, i32 first_lod,
                             i32 iterations, i32 levels, bool mip_chain, f32 radius, POST_SHADER_PROGRAM *program);
#ifdef __cplusplus
}
#endif
#endif
