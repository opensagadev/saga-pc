#ifndef SAGA_NU2API_NUGOBJ_H
#define SAGA_NU2API_NUGOBJ_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h, nu2api/nustencilshadows_PC.h (not all of it earlier in
 * the shim). */

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nustencilshadows_PC.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void PCGraphicsInitializeFramebuffers(u32, i32, VARIPTR *, VARIPTR);
    void PCGraphicsCopyFramebufferTex2DPc(i32, GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32, i32, i32);
    void NuGobjReleaseChildPc(PC_GOBJ_REGISTRY_CHILD_PC *);
#ifdef __cplusplus
}
#endif

#endif
