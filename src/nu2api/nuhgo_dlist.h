#ifndef SAGA_NU2API_NUHGO_DLIST_H
#define SAGA_NU2API_NUHGO_DLIST_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/nuvec4.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsCreateColourTarget(i32, i32);
    GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsCreateColourTargetFormat(i32, i32, i32);
    void PCGraphicsInitRenderTexturePool(VARIPTR *buffer, VARIPTR end);
    void NuRndrGeometryBuffersInitPc(void);
    void NuRndrBufferDescriptorsInitPc(VARIPTR *, VARIPTR);

    GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsCreateEffectTexture(i32, i32, i32, i32, i32);
#ifdef __cplusplus
}
#endif
// NOLINTNEXTLINE(readability-identifier-naming)
#define PcBootSecondaryFramebuffer (PcBootFramebuffersPc.fields.secondary_framebuffer)
#ifdef __cplusplus
extern "C" {
#endif
    extern NUVEC4 NuDisplayHandlerTintPc;
    extern i32 NuDisplayHandlerReflectionPc;

    extern f32 NuDisplayHandlerOpacityPc;
    extern i32 NuDisplayHandlerZModePc;
    extern i32 PCGraphicsFrameState;
#ifdef __cplusplus
}
#endif
struct PC_DL_SHADER_PROGRAM_PAIR_PC;
#ifdef __cplusplus
extern "C" {
#endif
    extern void *NuDisplayMaterialLastPc;
    extern NUVEC4 NuDisplayHandlerDefaultTintPc;
#ifdef __cplusplus
}
#endif

#endif
