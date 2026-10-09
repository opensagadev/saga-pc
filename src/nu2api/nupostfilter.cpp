#include "saga.h"
#include <string.h>

i32 __cdecl NuPostEffectIsInitialised(u32 mask) {
    return (NuPostEffectInitialisedFlagsPc & mask) != 0;
}

extern i32 NuPostEffectBackBufferFramePc;
extern GRAPHICS_RENDER_TEXTURE_PC *NuPostEffectBackBufferTexturePc;
extern GRAPHICS_RENDER_TEXTURE_PC *NuPostEffectBackBufferCopyPc;
PC_FRAMEBUFFER_PC *__cdecl PCGraphicsGetBoundFramebufferPc(void);
void __cdecl PCGraphicsEnableFramebufferGuardsPc(PC_FRAMEBUFFER_PC *, i32);
void __cdecl PCGraphicsCopyFramebufferTex2DPc(i32, GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32, i32, i32);

GRAPHICS_RENDER_TEXTURE_PC *__cdecl NuPostEffectGetBackBuffer(i32 frame) {
    PC_FRAMEBUFFER_PC *bound;
    i32 width, height;
    NuPostEffectBackBufferCopyPc = NuPostEffectBackBufferTexturePc;
    if (NuPostEffectBackBufferFramePc == frame && frame != -1)
        return NuPostEffectBackBufferCopyPc;
    if (frame != -1)
        NuPostEffectBackBufferFramePc = frame;
    bound = PCGraphicsGetBoundFramebufferPc();
    NuPostEffectBackBufferCopyPc = NuPostEffectBackBufferTexturePc;
    PCGraphicsEnableFramebufferGuardsPc(bound, 1);
    height = PCGraphicsEffectTextureHeight(NuPostEffectBackBufferCopyPc);
    width = PCGraphicsEffectTextureWidth(NuPostEffectBackBufferCopyPc);
    PCGraphicsCopyFramebufferTex2DPc(0, NuPostEffectBackBufferCopyPc, 0, 0, 0, width, height);
    PCGraphicsEnableFramebufferGuardsPc(bound, 0);
    return NuPostEffectBackBufferCopyPc;
}

/* Original nupostfilter source family. This is an append proposal to existing
 * nupostfilter.c; includes/layout declarations supplied separately. */
void __cdecl NuPostEffectReset(void) {
    NuPostEffectsRanPc = 0;
    NuPostLastFrameEffectPc = 0;
}
void __cdecl NuPostEffectEnable(i32 mask) {
    if ((NuPostEffectInitialisedFlagsPc & mask) == 0)
        return;
    switch (mask) {
        case 4:
            if (!NuPostMainFilterPc->bloom_enabled) {
                ++NuPostMainFilterPc->active_filter_count;
                NuPostMainFilterPc->bloom_enabled = 1;
            }
            break;
        case 8:
            if (!NuPostMainFilterPc->dof_enabled) {
                ++NuPostMainFilterPc->active_filter_count;
                NuPostMainFilterPc->dof_enabled = 1;
            }
            break;
        case 0x10:
            NuPostMotionFilterPc->enabled = 1;
            break;
        case 0x20:
            NuPostDeferredFilterPc->enabled = 1;
            break;
        case 0x40:
            NuPostAccumFilterPc->enabled = 1;
            break;
        case 0x80:
            NuPostSpeedFilterPc->enabled = 1;
            break;
    }
}
void __cdecl NuPostEffectDisable(u32 mask) {
    switch (mask) {
        case 4:
            if (NuPostMainFilterPc->bloom_enabled) {
                --NuPostMainFilterPc->active_filter_count;
                NuPostMainFilterPc->bloom_enabled = 0;
            }
            break;
        case 8:
            if (NuPostMainFilterPc->dof_enabled) {
                --NuPostMainFilterPc->active_filter_count;
                NuPostMainFilterPc->dof_enabled = 0;
            }
            break;
        case 0x10:
        case 0x40:
            NuPostMotionFilterPc->enabled = 0;
            break;
        case 0x20:
            NuPostDeferredFilterPc->enabled = 0;
            break;
        case 0x80:
            NuPostSpeedFilterPc->enabled = 0;
            break;
    }
}
void __cdecl NuPostEffectAddDynamicLight(NuDynamicLight *light) {
    i32 count;
    if (NuPostDeferredFilterPc == NULL)
        return;
    count = NuPostDeferredFilterPc->dynamic_light_count;
    NuPostDeferredFilterPc->dynamic_lights[count] = light;
    NuPostDeferredFilterPc->dynamic_light_count = count + 1;
}
i32 __cdecl NuPostEffectGetActiveDynamicLightCount(void) {
    return NuPostDeferredFilterPc != NULL ? NuPostDeferredFilterPc->dynamic_light_count : 0;
}
void __cdecl NuPostEffectDepthOfField(const NU_POST_DOF_PARAMETERS_PC *parameters) {
    if (NuPostMainFilterPc == NULL)
        return;
    NuPostMainFilterPc->dof_strength = parameters->strength;
    NuPostMainFilterPc->dof_near = parameters->near_distance;
    NuPostMainFilterPc->dof_far = parameters->far_distance;
    NuPostMainFilterPc->dof_bias = parameters->bias;
    NuPostMainFilterPc->dof_mode = parameters->mode;
}
void __cdecl NuPostEffectDeferredShading(const f32 *parameters) {
    if (NuPostDeferredFilterPc == NULL)
        return;
    memcpy(NuPostDeferredFilterPc->parameters, parameters, sizeof(NuPostDeferredFilterPc->parameters));
}
void __cdecl NuPostEffectBloom(const NU_POST_BLOOM_PARAMETERS_PC *parameters) {
    if (NuPostMainFilterPc != NULL)
        NuPostMainFilterPc->bloom = parameters;
}
void __cdecl NuPostEffectMotionBlur(const NUMTX *previous, const NUMTX *current, f32 scale, f32 maximum, f32 falloff) {
    if (NuPostMainFilterPc == NULL)
        return;
    NuPostMainFilterPc->motion_previous = *previous;
    NuPostMainFilterPc->motion_current = *current;
    NuPostMainFilterPc->motion_scale = scale;
    NuPostMainFilterPc->motion_maximum = maximum;
    NuPostMainFilterPc->motion_falloff = falloff;
    NuPostMotionFilterPc->previous = *previous;
    NuPostMotionFilterPc->current = *current;
    NuPostMotionFilterPc->scale = scale;
    NuPostMotionFilterPc->maximum = maximum;
    NuPostMotionFilterPc->falloff = falloff;
}
void __cdecl NuPostEffectAccumulationMotionBlur(i32 frames, f32 blend, f32 mode_pc) {
    if (NuPostMainFilterPc == NULL)
        return;
    NuPostMainFilterPc->accumulation_frames = frames;
    NuPostMainFilterPc->accumulation_blend = blend;
    NuPostMainFilterPc->accumulation_mode_pc = mode_pc;
    if (NuPostAccumFilterPc != NULL) {
        NuPostAccumFilterPc->frames = frames;
        NuPostAccumFilterPc->blend = blend;
        NuPostAccumFilterPc->mode_pc = mode_pc;
    }
}
void __cdecl NuPostEffectSpeedBlur(const NU_POST_SPEED_PARAMETERS_PC *parameters) {
    if (NuPostSpeedFilterPc != NULL)
        NuPostSpeedFilterPc->parameters = parameters;
}

void NuPostFilterGen::resetAll() {
}

bool NuPostFilterGen::isEnabled() {
    return enabled;
}

bool NuMainFilterGen::isEnabled() {
    return active_filter_count != 0;
}
