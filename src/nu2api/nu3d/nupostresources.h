/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

struct PC_FRAMEBUFFER;

#ifdef __cplusplus
extern "C" {
#endif
    PC_FRAMEBUFFER *NuFramebufferGetObject(i32 index);
    i32 NuFramebufferGetWidth(PC_FRAMEBUFFER *framebuffer);
    i32 NuFramebufferGetHeight(PC_FRAMEBUFFER *framebuffer);
    void NuEffectTexLockVP(VARIPTR *buffer, VARIPTR end);
    i32 NuEffectTexUnlockVP(void);
#ifdef __cplusplus
    GRAPHICS_RENDER_TEXTURE *NuFramebufferGetAttachedTex(PC_FRAMEBUFFER *, i32, i32 *, i32 *);
#ifdef __cplusplus
}
#endif
#endif
