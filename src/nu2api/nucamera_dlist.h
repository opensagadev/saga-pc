/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nustencilshadows_PC.h"
#endif

struct PC_TEXTURE_RESOURCE;

#ifdef __cplusplus
extern "C" {
#endif
    HRESULT PCDeviceCreateVertexShader(const DWORD *, struct PC_TEXTURE_RESOURCE **);
    HRESULT PCDeviceCreatePixelShader(const DWORD *, struct PC_TEXTURE_RESOURCE **);
#ifdef __cplusplus
}
#endif

typedef struct FRAME_NATIVE_TEXTURE_POOL { // size 0x308
    FRAME_NATIVE_TEXTURE records[16];      // 0x000
    i8 occupied[2];                        // 0x300
    u8 unknown_302[2];                     // 0x302
    i32 cursor;                            // 0x304
#ifdef __cplusplus
    FRAME_NATIVE_TEXTURE *Allocate();
    FRAME_NATIVE_TEXTURE_POOL *fun_00605150();
    i32 fun_00605170(FRAME_NATIVE_TEXTURE *record);
    void fun_00605300(i32 index, bool set);
    i32 fun_00605460(i32 index);
    void fun_0060a010(FRAME_NATIVE_TEXTURE *record);
#endif
} FRAME_NATIVE_TEXTURE_POOL;

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 RenderFrameShaderTimePc;
    extern void (*RenderFrameCallback)(void);
    HRESULT PCDeviceEndScenePc(void);
    i32 NuTexRegisterRenderTarget(GRAPHICS_RENDER_TEXTURE *target);
    void NuGobjRegistryCleanupPc(void);
    void NuGobjRegistryInitPc(void);
#ifdef __cplusplus
    void PostFilterCopyTexture(GRAPHICS_RENDER_TEXTURE *, i32, GRAPHICS_RENDER_TEXTURE *, i32, POST_SHADER_PROGRAM_PC *,
                               GRAPHICS_RENDER_TEXTURE *);
#ifdef __cplusplus
}
#endif
#endif
