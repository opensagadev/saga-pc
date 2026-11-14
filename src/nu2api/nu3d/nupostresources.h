#ifndef SAGA_NU2API_NU3D_NUPOSTRESOURCES_H
#define SAGA_NU2API_NU3D_NUPOSTRESOURCES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    PC_FRAMEBUFFER_PC *NuFramebufferGetObject(i32 index);
    i32 NuFramebufferGetWidth(PC_FRAMEBUFFER_PC *framebuffer);
    i32 NuFramebufferGetHeight(PC_FRAMEBUFFER_PC *framebuffer);
    void NuEffectTexLockVP(VARIPTR *buffer, VARIPTR end);
    i32 NuEffectTexUnlockVP(void);
#ifdef __cplusplus
    GRAPHICS_RENDER_TEXTURE_PC *NuFramebufferGetAttachedTex(PC_FRAMEBUFFER_PC *, i32, i32 *, i32 *);
#ifdef __cplusplus
}
#endif
#endif

#endif
