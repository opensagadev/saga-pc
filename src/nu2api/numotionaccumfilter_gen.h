/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
#ifdef __cplusplus
extern "C" {
#endif
    void PostFilterBlitTexture(GRAPHICS_RENDER_TEXTURE *, i32, GRAPHICS_RENDER_TEXTURE *, i32, POST_SHADER_PROGRAM_PC *,
                               GRAPHICS_RENDER_TEXTURE *);
    void NuPostEffectResize(void);
    void PCGraphicsCreatePostEffectWorkTexture(i32 width, i32 height);
    void PostBlur7x7High(GRAPHICS_RENDER_TEXTURE *source, i32 source_lod, GRAPHICS_RENDER_TEXTURE *textures, i32 first_lod, i32 iterations,
                         i32 levels, bool mip_chain, f32 radius, POST_SHADER_PROGRAM_PC *program);
    void PostGaussianKernel(i32 width, i32 height, NUVEC4 *taps, f32 scale);
#ifdef __cplusplus
}
#endif
#endif
