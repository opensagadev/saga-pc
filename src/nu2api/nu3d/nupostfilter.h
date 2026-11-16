#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuDynamicLight.h"
#include "nu2api/numath/numtx.h"

struct NU_POST_BLOOM_PARAMETERS;
struct NU_POST_SPEED_PARAMETERS;
struct NuDynamicLight;
struct PC_FRAMEBUFFER;

/* NuPostEffectInit flags and the NuPostEffectEnable/Disable/IsEnabled masks: the filter each bit creates */
#define NUPOSTEFFECT_BLOOM 0x4         /* NuMainFilter bloom */
#define NUPOSTEFFECT_DOF 0x8           /* NuMainFilter depth of field */
#define NUPOSTEFFECT_MOTION 0x10       /* NuMotionFilter */
#define NUPOSTEFFECT_DEFERRED 0x20     /* NuDeferredFilter */
#define NUPOSTEFFECT_MOTION_ACCUM 0x40 /* NuMotionAccumFilter */
#define NUPOSTEFFECT_SPEED_BLUR 0x80   /* NuSpeedBlurFilter */

#ifdef __cplusplus
extern "C" {
#endif
    void NuPostEffectInit(u32, VARIPTR *, VARIPTR);
    i32 NuPostEffectIsInitialised(u32 mask);
    GRAPHICS_RENDER_TEXTURE *NuPostEffectGetBackBuffer(i32);
    void NuPostEffectRender(PC_FRAMEBUFFER *);
    void NuPostEffectReset(void);
    void NuPostEffectEnable(i32);
    void NuPostEffectDisable(u32);
    void NuPostEffectAddDynamicLight(NuDynamicLight *);
    i32 NuPostEffectGetActiveDynamicLightCount(void);
    void NuPostEffectDepthOfField(const NU_POST_DOF_PARAMETERS *);
    void NuPostEffectDeferredShading(const f32 *);
    void NuPostEffectBloom(const NU_POST_BLOOM_PARAMETERS *);
    void NuPostEffectMotionBlur(const NUMTX *, const NUMTX *, f32, f32, f32);
    void NuPostEffectAccumulationMotionBlur(i32, f32, f32);
    void NuPostEffectSpeedBlur(const NU_POST_SPEED_PARAMETERS *);
#ifdef __cplusplus
    void NuPostEffectEnd(void);
#ifdef __cplusplus
}
#endif
#endif
