#ifndef SAGA_NU2API_NU3D_ANDROID_NUPOSTFILTER_H
#define SAGA_NU2API_NU3D_ANDROID_NUPOSTFILTER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h, nu2api/nucore/NuDynamicLight.h (not all of it earlier
 * in the shim). */

#include "nu2api/numath/numtx.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuDynamicLight.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuPostEffectInit(u32, VARIPTR *, VARIPTR);
    i32 NuPostEffectIsInitialised(u32 mask);
    GRAPHICS_RENDER_TEXTURE_PC *NuPostEffectGetBackBuffer(i32);
    void NuPostEffectRender(PC_FRAMEBUFFER_PC *);
    void NuPostEffectReset(void);
    void NuPostEffectEnable(i32);
    void NuPostEffectDisable(u32);
    void NuPostEffectAddDynamicLight(NuDynamicLight *);
    i32 NuPostEffectGetActiveDynamicLightCount(void);
    void NuPostEffectDepthOfField(const NU_POST_DOF_PARAMETERS_PC *);
    void NuPostEffectDeferredShading(const f32 *);
    void NuPostEffectBloom(const NU_POST_BLOOM_PARAMETERS_PC *);
    void NuPostEffectMotionBlur(const NUMTX *, const NUMTX *, f32, f32, f32);
    void NuPostEffectAccumulationMotionBlur(i32, f32, f32);
    void NuPostEffectSpeedBlur(const NU_POST_SPEED_PARAMETERS_PC *);
#ifdef __cplusplus
    void NuPostEffectEnd(void);
#ifdef __cplusplus
}
#endif
#endif

#endif
