#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"

struct GRAPHICS_RENDER_TEXTURE;
struct PC_TEXTURE_RESOURCE;

#ifdef __cplusplus
extern "C" {
#endif
    /* Compiled Direct3D 9 shader token streams of the post-effect and display-handler programs. */
    extern const DWORD dat_007656f0[42];
    extern const DWORD dat_00765798[43];
    extern const DWORD dat_00765848[151];
    extern const DWORD dat_00765aa8[126];
    extern const DWORD dat_00765ca0[60];
    extern const DWORD dat_00765d90[43];
    extern const DWORD dat_00765e40[60];
    extern const DWORD dat_00765f30[104];
    extern const DWORD dat_007660d0[130];
    extern const DWORD dat_007662d8[168];
    extern const DWORD dat_00766578[94];
    extern const DWORD dat_007666f0[75];
    extern const DWORD dat_00766820[261];
    extern const DWORD dat_00766c38[168];
    extern const DWORD dat_00766ed8[94];
    extern const DWORD dat_00767050[75];
    extern const DWORD dat_00767180[261];
    extern const DWORD dat_00767598[85];
    extern const DWORD dat_007676f0[566];
    extern const DWORD dat_00767fc8[554];
    extern const DWORD dat_00768870[155];
    extern const DWORD dat_00768ae0[143];
    extern const DWORD dat_00768d20[48];
    extern const DWORD dat_00768de0[180];
    extern const DWORD dat_007690b0[131];
    extern const DWORD dat_007692c0[175];
    extern const DWORD dat_00769580[100];
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
