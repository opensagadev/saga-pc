#ifndef SAGA_NU2API_NUCAMERA_DLIST_H
#define SAGA_NU2API_NUCAMERA_DLIST_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h, nu2api/nustencilshadows_PC.h (not all of it earlier in
 * the shim). */

#include "nu2api/nuwin32/d3dCore.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nustencilshadows_PC.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    HRESULT PCDeviceCreateVertexShader(const DWORD *, PC_TEXTURE_RESOURCE_PC **);
    HRESULT PCDeviceCreatePixelShader(const DWORD *, PC_TEXTURE_RESOURCE_PC **);
#ifdef __cplusplus
}
#endif

typedef struct FRAME_NATIVE_TEXTURE_POOL_PC {
    FRAME_NATIVE_TEXTURE_PC records[16];
    i8 occupied[2];
    u8 unknown_302[2];
    i32 cursor;
#ifdef __cplusplus
    FRAME_NATIVE_TEXTURE_PC *Allocate();
    FRAME_NATIVE_TEXTURE_POOL_PC *fun_00605150();
    i32 fun_00605170(FRAME_NATIVE_TEXTURE_PC *record);
    void fun_00605300(i32 index, bool set);
    i32 fun_00605460(i32 index);
    void fun_0060a010(FRAME_NATIVE_TEXTURE_PC *record);
#endif
} FRAME_NATIVE_TEXTURE_POOL_PC;

STATIC_ASSERT(offsetof(FRAME_NATIVE_TEXTURE_POOL_PC, records) == 0);
STATIC_ASSERT(offsetof(FRAME_NATIVE_TEXTURE_POOL_PC, occupied) == 0x300);
STATIC_ASSERT(offsetof(FRAME_NATIVE_TEXTURE_POOL_PC, unknown_302) == 0x302);
STATIC_ASSERT(offsetof(FRAME_NATIVE_TEXTURE_POOL_PC, cursor) == 0x304);
STATIC_ASSERT(sizeof(FRAME_NATIVE_TEXTURE_POOL_PC) == 0x308);
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 RenderFrameShaderTimePc;
    extern void (*RenderFrameCallbackPc)(void);
    HRESULT PCDeviceEndScenePc(void);
    i32 NuTexRegisterRenderTargetPc(GRAPHICS_RENDER_TEXTURE_PC *target);
    void NuGobjRegistryCleanupPc(void);
    void NuGobjRegistryInitPc(void);
#ifdef __cplusplus
    void PostFilterCopyTexturePc(GRAPHICS_RENDER_TEXTURE_PC *, i32, GRAPHICS_RENDER_TEXTURE_PC *, i32,
                                 POST_SHADER_PROGRAM_PC *, GRAPHICS_RENDER_TEXTURE_PC *);
#ifdef __cplusplus
}
#endif
#endif

#endif
