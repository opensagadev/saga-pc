/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nustencilshadows_PC.h"
#endif

struct nugobj_s;

#ifdef __cplusplus
extern "C" {
#endif
    void PCGraphicsInitializeFramebuffers(u32, i32, VARIPTR *, VARIPTR);
    void PCGraphicsCopyFramebufferTex2D(i32, GRAPHICS_RENDER_TEXTURE *, i32, i32, i32, i32, i32);
    void NuGobjReleaseChild(PC_GOBJ_REGISTRY_CHILD_PC *);
    void PCGraphicsClearFrameTextures(void);
    void fun_006030d0(struct nugobj_s *gobj);
    void fun_006030f0(struct nugobj_s *gobj, i32 first, i32 second);
#ifdef __cplusplus
}
#endif
