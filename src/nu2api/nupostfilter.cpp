#include "saga.h"
#include <string.h>

extern "C" PC_FRAMEBUFFER_PC *__cdecl PCGraphicsCreateFramebuffer(void);
extern "C" void __cdecl PCGraphicsAttachFramebuffer(PC_FRAMEBUFFER_PC *, i32, GRAPHICS_RENDER_TEXTURE_PC *, i32);

static f32 motionFactorPan = 0.01f;
static f32 motionFactorPull = -0.024f;
static f32 motionFactorPanClamp = 0.02f;

void __cdecl NuSpeedBlurSetMotionFactors(f32 pan, f32 pull, f32 clamp) {
    motionFactorPan = pan;
    motionFactorPull = pull;
    motionFactorPanClamp = clamp;
}

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

/* Private complete initializer/filter initialization bodies; merge existing TU. */
#include <new>
#include "saga.h"
struct POST_SHADER_PROGRAM_PC;
extern "C" {
    POST_SHADER_PROGRAM_PC *__cdecl PostShaderProgramCreatePc(i32, const DWORD *, const DWORD *);
    extern const DWORD PostShaderTokens7656f0[42];
    extern const DWORD PostShaderTokens765798[43];
    extern const DWORD PostShaderTokens765848[151];
    extern const DWORD PostShaderTokens765aa8[126];
    extern const DWORD PostShaderTokens765ca0[60];
    extern const DWORD PostShaderTokens765d90[43];
    extern const DWORD PostShaderTokens765e40[60];
    extern const DWORD PostShaderTokens765f30[104];
    extern const DWORD PostShaderTokens7660d0[130];
    extern const DWORD PostShaderTokens7662d8[168];
    extern const DWORD PostShaderTokens766578[94];
    extern const DWORD PostShaderTokens7666f0[75];
    extern const DWORD PostShaderTokens766820[261];
    extern const DWORD PostShaderTokens766c38[168];
    extern const DWORD PostShaderTokens766ed8[94];
    extern const DWORD PostShaderTokens767050[75];
    extern const DWORD PostShaderTokens767180[261];
    extern const DWORD PostShaderTokens767598[85];
    extern const DWORD PostShaderTokens7676f0[566];
    extern const DWORD PostShaderTokens767fc8[554];
    extern const DWORD PostShaderTokens768870[155];
    extern const DWORD PostShaderTokens768ae0[143];
    extern const DWORD PostShaderTokens768d20[48];
    extern const DWORD PostShaderTokens768de0[180];
    extern const DWORD PostShaderTokens7690b0[131];
    extern const DWORD PostShaderTokens7692c0[175];
    extern const DWORD PostShaderTokens769580[100];
}
extern "C" void PCGraphicsCreatePostEffectTextures(u32, u32);
extern "C" GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectWorkTexturePc;
extern "C" GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectBackBufferTexturePc;
extern "C" i32 PCGraphicsEffectTextureMipCount(GRAPHICS_RENDER_TEXTURE_PC *);
extern "C" void PCGraphicsEffectTextureSetLowNibbles(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32);
extern "C" void PCGraphicsEffectTextureSetField24(GRAPHICS_RENDER_TEXTURE_PC *, i32);
extern "C" void PCGraphicsEffectTextureSetFilterFields(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32);
extern "C" void PCGraphicsEffectTextureSetWrapFields(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32);
extern "C" void PostFilterInitProgramsPc(i32, i32);
extern "C" u8 NuPostFilterMemoryPc[0x800];
extern "C" VirtualStackAllocator NuPostFilterAllocatorPc;
extern "C" NuCopyFilter *NuPostCopyFilterPc;
extern "C" u32 NuPostShaderEngineFlagsPc;

NuPostFilterGen::~NuPostFilterGen() {
}

void NuPostFilterGen::initResources() {
    input_fbo = PCGraphicsCreateFramebuffer();
}
void NuPostFilterGen::initTextureResources(i32, i32) {
}
NuMainFilterGen::NuMainFilterGen() {
    dof_strength = 1.0f;
    dof_near = 1.0f;
    dof_far = 1.0f;
    dof_blur = 3.0f;
    dof_bias = 0;
    blur_radius = 5.0f;
    dof_mode = 3;
    bloom = NULL;
    downsample_lod = 0;
    blur_gain = 2.1f;
    motion_scale = 0.0f;
    motion_maximum = 0.0f;
    motion_falloff = 1.0f;
}
NuMainFilter::NuMainFilter() {
    platform_fbo = NULL;
}
NuDeferredFilterGen::NuDeferredFilterGen() {
    shadow_fbos[0] = NULL;
    shadow_fbos[1] = NULL;
    shadow_fbos[2] = NULL;
    shadow_fbos[3] = NULL;
    shadow_fbos[4] = NULL;
    shadow_fbos[5] = NULL;
    parameters[1] = 0.5f;
    light_fbo = NULL;
    sample_count = 4;
    parameters[0] = 0.0f;
}
void NuMainFilterGen::initResources() {
    input_fbo = PCGraphicsCreateFramebuffer();
    blur_fbo = PCGraphicsCreateFramebuffer();
    dof_enabled = false;
    bloom_enabled = false;
    motion_blur_enabled = false;
    active_filter_count = 0;
}
void NuMainFilter::initTextureResources(i32, i32) {
    texture_surface.Attach(PCGraphicsPostEffectBackBufferTexturePc, 0, false);
}
void NuMotionAccumFilterGen::initResources() {
    input_fbo = PCGraphicsCreateFramebuffer();
    accumulation_fbo = PCGraphicsCreateFramebuffer();
    PCGraphicsAttachFramebuffer(accumulation_fbo, 0, accumulation_texture, 0);
}
void NuMotionAccumFilter::initResources() {
    NuMotionAccumFilterGen::initResources();
    program = NULL;
}
void NuMotionAccumFilterGen::initTextureResources(i32 width, i32 height) {
    accumulation_texture = PCGraphicsCreateEffectTexture(width, height, 1, 1, 2);
}
void NuSpeedBlurFilter::initTextureResources(i32, i32) {
    texture_surface.Attach(PCGraphicsPostEffectBackBufferTexturePc, 0, false);
}
void NuDeferredFilterGen::initResources() {
    input_fbo = PCGraphicsCreateFramebuffer();
    for (i32 i = 0; i < 6; ++i)
        shadow_fbos[i] = PCGraphicsCreateFramebuffer();
    light_fbo = PCGraphicsCreateFramebuffer();
    PCGraphicsAttachFramebuffer(light_fbo, 0, textures[0], 0);
    dynamic_light_count = 0;
    deferred_geometry_count = 0;
    resetAll();
}
void NuDeferredFilterGen::initTextureResources(i32, i32) {
    textures[2] = PCGraphicsCreateEffectTexture(640, 2048, 1, 1, 4);
    PCGraphicsEffectTextureSetLowNibbles(textures[2], 0, 1);
    PCGraphicsEffectTextureSetWrapFields(textures[2], 3, 3);
    PCGraphicsEffectTextureSetField24(textures[2], -1);
    PCGraphicsEffectTextureSetFilterFields(textures[2], 2, 2, 0);
    for (i32 i = 0; i < 2; ++i) {
        i32 height = PCGraphicsEffectTextureHeight(textures[2]);
        i32 width = PCGraphicsEffectTextureWidth(textures[2]);
        textures[4 + i] = PCGraphicsCreateEffectTexture(width, height, 1, 1, 10);
    }
    textures[0] = PCGraphicsPostEffectBackBufferTexturePc;
    textures[1] = NULL;
    i32 old_sample_count = sample_count;
    if (old_sample_count < PCGraphicsEffectTextureMipCount(textures[0]))
        sample_count = old_sample_count;
    else
        sample_count = PCGraphicsEffectTextureMipCount(textures[0]);
}
void NuCopyFilter::initResources() {
    input_fbo = PCGraphicsCreateFramebuffer();
    copy_fbo = PCGraphicsCreateFramebuffer();
    PCGraphicsAttachFramebuffer(copy_fbo, 0, PCGraphicsPostEffectWorkTexturePc, 0);
    input_fbo = copy_fbo;
}

extern "C" u8 NuPostFilterMemoryPc[0x800];
u8 NuPostFilterMemoryPc[0x800] = {0};
VirtualStackAllocator NuPostFilterAllocatorPc(NuPostFilterMemoryPc, sizeof(NuPostFilterMemoryPc));
NuCopyFilter *NuPostCopyFilterPc = NULL;
u32 NuPostShaderEngineFlagsPc = 0;

extern "C" void __cdecl NuPostEffectInit(u32 flags, VARIPTR *buffer, VARIPTR end) {
    i32 width = PS2_REZ_W, height = PS2_REZ_H;
    u32 main_mask = flags & 0xc, motion_mask = flags & 0x10, accum_mask = flags & 0x40;
    u32 speed_mask = flags & 0x80, deferred_mask = flags & 0x20;
    void *object;
    NuPostEffectInitialisedFlagsPc = flags;
    if (main_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuMainFilter);
        NuPostMainFilterPc = object ? (NU_POST_MAIN_VIEW_PC *)new (object) NuMainFilter : NULL;
    }
    if (motion_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuMotionFilter);
        NuPostMotionFilterPc = object ? (NU_POST_MOTION_VIEW_PC *)new (object) NuMotionFilter : NULL;
        NuPostShaderEngineFlagsPc |= 0x20;
    }
    if (accum_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuMotionAccumFilter);
        NuPostAccumFilterPc = object ? (NU_POST_ACCUM_VIEW_PC *)new (object) NuMotionAccumFilter : NULL;
    }
    if (speed_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuSpeedBlurFilter);
        NuPostSpeedFilterPc = object ? (NU_POST_SPEED_VIEW_PC *)new (object) NuSpeedBlurFilter : NULL;
    }
    if (deferred_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuDeferredFilter);
        NuPostDeferredFilterPc = object ? (NU_POST_DEFERRED_VIEW_PC *)new (object) NuDeferredFilter : NULL;
        NuPostShaderEngineFlagsPc |= 0x10;
    }
    object = NuPostFilterAllocatorPc.cursor;
    NuPostFilterAllocatorPc.cursor += sizeof(NuCopyFilter);
    NuPostCopyFilterPc = object ? new (object) NuCopyFilter : NULL;
    NuEffectTexLockVP(buffer, end);
    PCGraphicsCreatePostEffectTextures(width, height);
    if (main_mask)
        ((NuPostFilterGen *)NuPostMainFilterPc)->initTextureResources(width, height);
    if (motion_mask)
        ((NuPostFilterGen *)NuPostMotionFilterPc)->initTextureResources(width, height);
    if (accum_mask)
        ((NuPostFilterGen *)NuPostAccumFilterPc)->initTextureResources(width, height);
    if (speed_mask)
        ((NuPostFilterGen *)NuPostSpeedFilterPc)->initTextureResources(width, height);
    if (deferred_mask)
        ((NuPostFilterGen *)NuPostDeferredFilterPc)->initTextureResources(width, height);
    if (flags & 1)
        NuPostEffectBackBufferTexturePc = PCGraphicsPostEffectBackBufferTexturePc;
    NuPostCopyFilterPc->initTextureResources(width, height);
    NuEffectTexUnlockVP();
    PostFilterInitProgramsPc(width, height);
    if (main_mask)
        ((NuPostFilterGen *)NuPostMainFilterPc)->initResources();
    if (motion_mask)
        ((NuPostFilterGen *)NuPostMotionFilterPc)->initResources();
    if (accum_mask)
        ((NuPostFilterGen *)NuPostAccumFilterPc)->initResources();
    if (speed_mask)
        ((NuPostFilterGen *)NuPostSpeedFilterPc)->initResources();
    if (deferred_mask)
        ((NuPostFilterGen *)NuPostDeferredFilterPc)->initResources();
    NuPostCopyFilterPc->initResources();
    NuPostEffectsRanPc = 0;
    NuPostLastFrameEffectPc = 0;
}

void NuMainFilter::initResources() {
    NuMainFilterGen::initResources();
    void *last_program;
    if (PCDeviceHardwareVertexProcessingPC > 1) {
        programs[6] = PostShaderProgramCreatePc(2, PostShaderTokens7662d8, PostShaderTokens766578);
        programs[8] = PostShaderProgramCreatePc(2, PostShaderTokens766820, PostShaderTokens766578);
        platform_programs[0] = PostShaderProgramCreatePc(2, PostShaderTokens7662d8, PostShaderTokens7666f0);
        last_program = PostShaderProgramCreatePc(2, PostShaderTokens766820, PostShaderTokens7666f0);
        platform_uniforms[0] = 0x82;
        platform_uniforms[1] = 0x88;
        platform_uniforms[2] = 0x89;
        platform_uniforms[3] = 0x8a;
    } else {
        programs[6] = PostShaderProgramCreatePc(2, PostShaderTokens766c38, PostShaderTokens766ed8);
        programs[8] = PostShaderProgramCreatePc(2, PostShaderTokens767180, PostShaderTokens766ed8);
        platform_programs[0] = PostShaderProgramCreatePc(2, PostShaderTokens766c38, PostShaderTokens767050);
        last_program = PostShaderProgramCreatePc(2, PostShaderTokens767180, PostShaderTokens767050);
        platform_uniforms[0] = 0;
        platform_uniforms[1] = 1;
        platform_uniforms[2] = 2;
        platform_uniforms[3] = 3;
    }
    platform_programs[1] = last_program;
    platform_fbo = PCGraphicsCreateFramebuffer();
}
void NuDeferredFilter::initResources() {
    NuDeferredFilterGen::initResources();
    programs[0] = PostShaderProgramCreatePc(2, PostShaderTokens767598, PostShaderTokens7676f0);
    programs[1] = PostShaderProgramCreatePc(2, PostShaderTokens767598, PostShaderTokens767fc8);
    programs[2] = PostShaderProgramCreatePc(2, PostShaderTokens768d20, PostShaderTokens768870);
    programs[3] = PostShaderProgramCreatePc(2, PostShaderTokens768d20, PostShaderTokens768ae0);
}
void NuSpeedBlurFilter::initResources() {
    NuPostFilterGen::initResources();
    void *program;
    if (PCDeviceHardwareVertexProcessingPC > 1) {
        program = PostShaderProgramCreatePc(2, PostShaderTokens768de0, PostShaderTokens7690b0);
        platform_uniform = 0x80;
    } else {
        program = PostShaderProgramCreatePc(2, PostShaderTokens7692c0, PostShaderTokens769580);
        platform_uniform = 0;
    }
    programs[0] = program;
}

void NuDeferredFilter::initTextureResources(i32 width, i32 height) {
    NuDeferredFilterGen::initTextureResources(width, height);
}

void NuMainFilterGen::reset() {
    enabled = false;
    dof_enabled = false;
    bloom_enabled = false;
    motion_blur_enabled = false;
    active_filter_count = 0;
}
void NuMainFilter::render() {
    if (bloom_enabled)
        RenderBloomPc();
}
extern "C" void __cdecl NuPostEffectEnd(void) {
    if (NuPostDeferredFilterPc != NULL)
        ((NuPostFilterGen *)NuPostDeferredFilterPc)->resetAll();
    if (NuPostMainFilterPc != NULL)
        ((NuPostFilterGen *)NuPostMainFilterPc)->resetAll();
    if (NuPostMotionFilterPc != NULL)
        ((NuPostFilterGen *)NuPostMotionFilterPc)->resetAll();
    if (NuPostAccumFilterPc != NULL)
        ((NuPostFilterGen *)NuPostAccumFilterPc)->resetAll();
    PostProxyBuffersPc[0].texture = NULL;
    PostProxyBuffersPc[0].kind = 0;
    PostProxyBuffersPc[0].resolved = false;
    PostProxyBuffersPc[1].texture = NULL;
    PostProxyBuffersPc[1].resolved = false;
    PostProxyBuffersPc[2].texture = NULL;
    PostProxyBuffersPc[2].resolved = false;
    PostProxyBuffersPc[4].texture = NULL;
    PostProxyBuffersPc[4].resolved = false;
    PostProxyBuffersPc[0].enabled = true;
    PostProxyBuffersPc[1].kind = 1;
    PostProxyBuffersPc[1].enabled = true;
    PostProxyBuffersPc[2].kind = 2;
    PostProxyBuffersPc[2].enabled = true;
    PostProxyBuffersPc[4].kind = 4;
    PostProxyBuffersPc[4].enabled = true;
}

extern "C" void __cdecl NuVec4Scale(NUVEC4 *, NUVEC4 *, f32);
void NuSpeedBlurFilterGen::computeSpeedBlur(NUVEC4 *result) {
    NUMTX previous_inverse, current_inverse;
    NUVEC4 motion;
    NuMtxInvH(&previous_inverse, const_cast<NUMTX *>(&parameters->previous));
    NuMtxInvH(&current_inverse, const_cast<NUMTX *>(&parameters->current));
    NuVec4MtxTransformH(&motion, reinterpret_cast<NUVEC4 *>(&previous_inverse) + 3,
                        const_cast<NUMTX *>(&parameters->current));
    if (motion.z < 0.0f) {
        NuVec4MtxTransformH(&motion, reinterpret_cast<NUVEC4 *>(&current_inverse) + 3,
                            const_cast<NUMTX *>(&parameters->previous));
        motion.x = (f32)((f64)motion.x * -1.0);
        motion.y = (f32)((f64)motion.y * -1.0);
        motion.z = (f32)((f64)motion.z * -1.0);
    }
    NuVec4Scale(&motion, &motion, 1.0f / motion.w);
    motion.x *= motionFactorPan;
    motion.y *= motionFactorPan;
    motion.z *= motionFactorPull;
    NuVec4Scale(&motion, &motion, parameters->scale);
    motion.x = motion.x < -motionFactorPanClamp ? -motionFactorPanClamp
                                                : (motion.x > motionFactorPanClamp ? motionFactorPanClamp : motion.x);
    motion.y = motion.y < -motionFactorPanClamp ? -motionFactorPanClamp
                                                : (motion.y > motionFactorPanClamp ? motionFactorPanClamp : motion.y);
    memcpy(result, &motion, sizeof(NUVEC));
    result->w = 0.0f;
}
void NuSpeedBlurFilter::destroyTextureResources() {
    texture_surface.Reset();
}

extern "C" void __cdecl PCGraphicsDestroyFramebuffer(PC_FRAMEBUFFER_PC *);
void NuPostFilterGen::destroyResources() {
    PCGraphicsDestroyFramebuffer(input_fbo);
    input_fbo = NULL;
}
PC_FRAMEBUFFER_PC *NuPostFilterGen::getInputFbo() {
    return input_fbo;
}
