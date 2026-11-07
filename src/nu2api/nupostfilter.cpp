#include "saga.h"
#include <string.h>

/* The native reset clears these three adjacent flags together. */
STATIC_ASSERT(offsetof(NuMainFilterGen, bloom_enabled) == offsetof(NuMainFilterGen, dof_enabled) + sizeof(bool));
STATIC_ASSERT(offsetof(NuMainFilterGen, motion_blur_enabled) ==
              offsetof(NuMainFilterGen, bloom_enabled) + sizeof(bool));

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
    NuPostEffectBackBufferCopyPc = NuPostEffectBackBufferTexturePc;
    if (NuPostEffectBackBufferFramePc != frame || frame == -1) {
        if (frame != -1)
            NuPostEffectBackBufferFramePc = frame;
        PC_FRAMEBUFFER_PC *bound = PCGraphicsGetBoundFramebufferPc();
        NuPostEffectBackBufferCopyPc = NuPostEffectBackBufferTexturePc;
        PCGraphicsEnableFramebufferGuardsPc(bound, 1);
        PCGraphicsCopyFramebufferTex2DPc(0, NuPostEffectBackBufferCopyPc, 0, 0, 0,
                                         PCGraphicsEffectTextureWidth(NuPostEffectBackBufferCopyPc),
                                         PCGraphicsEffectTextureHeight(NuPostEffectBackBufferCopyPc));
        PCGraphicsEnableFramebufferGuardsPc(bound, 0);
    }
    return NuPostEffectBackBufferCopyPc;
}

extern "C" void __cdecl PCGraphicsResizePostEffectTexturesPc(u32, u32);

/* Re-creates the size-dependent filter textures after a device reset. */
extern "C" void __cdecl NuPostEffectResizePc(void) {
    i32 width = PS2_REZ_W;
    i32 height = PS2_REZ_H;
    if (NuPostMainFilterPc != NULL)
        ((NuMainFilter *)NuPostMainFilterPc)->ResizeTexturesPc(width, height);
    if (NuPostSpeedFilterPc != NULL)
        ((NuSpeedBlurFilter *)NuPostSpeedFilterPc)->ResizeTexturesPc(width, height);
    if (NuPostDeferredFilterPc != NULL)
        ((NuDeferredFilter *)NuPostDeferredFilterPc)->ResizeTexturesPc(width, height);
    PCGraphicsResizePostEffectTexturesPc(width, height);
}

/* Original nupostfilter source family. This is an append proposal to existing
 * nupostfilter.c; includes/layout declarations supplied separately. */
void __cdecl NuPostEffectReset(void) {
    NuPostEffectsRanPc = 0;
    NuPostLastFrameEffectPc = 0;
}
void __cdecl NuPostEffectEnable(i32 mask) {
    u8 *flag;
    if ((NuPostEffectInitialisedFlagsPc & mask) == 0)
        return;
    switch (mask) {
        case 4:
            flag = &NuPostMainFilterPc->bloom_enabled;
            if (!*flag) {
                ++NuPostMainFilterPc->active_filter_count;
                *flag = 1;
            }
            break;
        case 8:
            flag = &NuPostMainFilterPc->dof_enabled;
            if (!*flag) {
                ++NuPostMainFilterPc->active_filter_count;
                *flag = 1;
            }
            break;
        case 0x20:
            NuPostDeferredFilterPc->enabled = 1;
            break;
        case 0x10:
            NuPostMotionFilterPc->enabled = 1;
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
    u8 *flag;
    switch (mask) {
        case 4:
            flag = &NuPostMainFilterPc->bloom_enabled;
            if (*flag) {
                --NuPostMainFilterPc->active_filter_count;
                *flag = 0;
            }
            break;
        case 8:
            flag = &NuPostMainFilterPc->dof_enabled;
            if (*flag) {
                --NuPostMainFilterPc->active_filter_count;
                *flag = 0;
            }
            break;
        case 0x20:
            NuPostDeferredFilterPc->enabled = 0;
            break;
        case 0x10:
            NuPostMotionFilterPc->enabled = 0;
            break;
        case 0x40:
            NuPostMotionFilterPc->enabled = 0;
            break;
        case 0x80:
            NuPostSpeedFilterPc->enabled = 0;
            break;
    }
}
void __cdecl NuPostEffectAddDynamicLight(NuDynamicLight *light) {
    if (NuPostDeferredFilterPc != NULL) {
        i32 &count = NuPostDeferredFilterPc->dynamic_light_count;
        NuPostDeferredFilterPc->dynamic_lights[count++] = light;
    }
}
i32 __cdecl NuPostEffectGetActiveDynamicLightCount(void) {
    return NuPostDeferredFilterPc != NULL ? NuPostDeferredFilterPc->dynamic_light_count : 0;
}
void __cdecl NuPostEffectDepthOfField(const NU_POST_DOF_PARAMETERS_PC *parameters) {
    NU_POST_MAIN_VIEW_PC *main = NuPostMainFilterPc;
    if (main == NULL)
        return;
    main->dof_strength = parameters->strength;
    main->dof_near = parameters->near_distance;
    main->dof_far = parameters->far_distance;
    main->dof_bias = parameters->bias;
    main->dof_mode = parameters->mode;
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
    NU_POST_MAIN_VIEW_PC *main = NuPostMainFilterPc;
    if (main == NULL)
        return;
    main->motion_previous = *previous;
    main->motion_current = *current;
    main->motion_scale = scale;
    main->motion_maximum = maximum;
    main->motion_falloff = falloff;
    NU_POST_MOTION_VIEW_PC *motion = NuPostMotionFilterPc;
    motion->previous = *previous;
    motion->current = *current;
    motion->scale = scale;
    motion->maximum = maximum;
    motion->falloff = falloff;
}
void __cdecl NuPostEffectAccumulationMotionBlur(i32 frames, f32 blend, f32 mode_pc) {
    NU_POST_MAIN_VIEW_PC *main = NuPostMainFilterPc;
    if (main == NULL)
        return;
    main->accumulation_frames = frames;
    main->accumulation_blend = blend;
    main->accumulation_mode_pc = mode_pc;
    NU_POST_ACCUM_VIEW_PC *accum = NuPostAccumFilterPc;
    if (accum != NULL) {
        accum->mode_pc = mode_pc;
        accum->frames = frames;
        accum->blend = blend;
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

inline NuPostFilterGen::~NuPostFilterGen() {
}

/* Native platform initializers call their generic implementations out of line. */
__declspec(noinline) void NuPostFilterGen::initResources() {
    input_fbo = PCGraphicsCreateFramebuffer();
}
void NuPostFilterGen::initTextureResources(i32, i32) {
}
void NuPostFilterGen::render() {
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
NuDeferredFilterGen::NuDeferredFilterGen() {
    memset(shadow_fbos, 0, sizeof(shadow_fbos));
    parameters[1] = 0.5f;
    light_fbo = NULL;
    enabled = false;
    sample_count = 4;
    memset(parameters, 0, sizeof(parameters[0]));
}
__declspec(noinline) void NuMainFilterGen::initResources() {
    input_fbo = PCGraphicsCreateFramebuffer();
    blur_fbo = PCGraphicsCreateFramebuffer();
    memset(&dof_enabled, 0, sizeof(dof_enabled) + sizeof(bloom_enabled) + sizeof(motion_blur_enabled));
    active_filter_count = 0;
}
void NuMainFilterGen::initTextureResources(i32 width, i32 height) {
    blur_texture = PCGraphicsCreateEffectTexture(width / 2, height / 2, 2, 1, 2);
    i32 w = width, h = height;
    downsample_lod = 0;
    for (i32 i = 0; i < 3; ++i) {
        if (w < 128 || h < 128)
            break;
        ++downsample_lod;
        w /= 2;
        h /= 2;
    }
    downsample_texture = PCGraphicsCreateEffectTexture(w, h, 1, 1, 2);
    if (height < 704) {
        dof_blur -= 1.0;
        blur_radius -= 0.85000002384185791;
        blur_gain -= 0.5;
    }
}
void NuMainFilter::initTextureResources(i32, i32) {
    texture_surface.Attach(PCGraphicsPostEffectBackBufferTexturePc, 0, false);
}
__declspec(noinline) void NuMotionAccumFilterGen::initResources() {
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
void NuSpeedBlurFilterGen::initTextureResources(i32 width, i32 height) {
    texture = PCGraphicsCreateEffectTexture(width / 2, height / 2, 1, 1, 2);
    PCGraphicsEffectTextureSetFilterFields(texture, 2, 2, 0);
    PCGraphicsEffectTextureSetWrapFields(texture, 2, 2);
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
        textures[4 + i] = PCGraphicsCreateEffectTexture(PCGraphicsEffectTextureWidth(textures[2]),
                                                        PCGraphicsEffectTextureHeight(textures[2]), 1, 1, 10);
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
    u32 main_mask, motion_mask, accum_mask, speed_mask, deferred_mask;
    void *object;
    NuPostEffectInitialisedFlagsPc = flags;
    main_mask = flags & 0xc;
    if (main_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuMainFilter);
        NuPostMainFilterPc = object ? (NU_POST_MAIN_VIEW_PC *)new (object) NuMainFilter : NULL;
    }
    motion_mask = flags & 0x10;
    if (motion_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuMotionFilter);
        NuPostMotionFilterPc = object ? (NU_POST_MOTION_VIEW_PC *)new (object) NuMotionFilter : NULL;
        NuPostShaderEngineFlagsPc |= 0x20;
    }
    accum_mask = flags & 0x40;
    if (accum_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuMotionAccumFilter);
        NuPostAccumFilterPc = object ? (NU_POST_ACCUM_VIEW_PC *)new (object) NuMotionAccumFilter : NULL;
    }
    speed_mask = flags & 0x80;
    if (speed_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuSpeedBlurFilter);
        NuPostSpeedFilterPc = object ? (NU_POST_SPEED_VIEW_PC *)new (object) NuSpeedBlurFilter : NULL;
    }
    deferred_mask = flags & 0x20;
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
    memset(&dof_enabled, 0, sizeof(dof_enabled) + sizeof(bloom_enabled) + sizeof(motion_blur_enabled));
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
    *result = motion;
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

STATIC_ASSERT(offsetof(NuMainFilterGen, blur_texture) == 0x54);
STATIC_ASSERT(offsetof(NuMainFilterGen, blur_fbo) == 0x58);
STATIC_ASSERT(offsetof(NuMainFilterGen, downsample_texture) == 0x5c);
STATIC_ASSERT(offsetof(NuMotionAccumFilterGen, accumulation_fbo) == 0x10);
STATIC_ASSERT(offsetof(NuMotionAccumFilterGen, accumulation_texture) == 0x14);
STATIC_ASSERT(offsetof(NuSpeedBlurFilterGen, texture) == 0x18);
STATIC_ASSERT(offsetof(NuCopyFilter, copy_fbo) == 0xc);
STATIC_ASSERT(offsetof(NuDeferredFilterGen, textures) == 0x28);

void NuPostFilterGen::destroyTextureResources() {
}
void NuMainFilterGen::destroyTextureResources() {
    PCGraphicsDestroyRenderTexture(blur_texture);
    PCGraphicsDestroyRenderTexture(downsample_texture);
}
void NuCopyFilter::reset() {
    input_fbo = copy_fbo;
    copy_texture = NULL;
}
void NuCopyFilter::destroyResources() {
    PCGraphicsDestroyFramebuffer(copy_fbo);
    NuPostFilterGen::destroyResources();
}
void NuCopyFilter::render(PC_FRAMEBUFFER_PC *output) {
    GRAPHICS_RENDER_TEXTURE_PC *color = PCGraphicsGetFramebufferAttachmentPc(input_fbo, 0, NULL, NULL);
    if (copy_texture != NULL)
        copy(color, copy_texture, output);
    else
        copy(color, output);
}
void NuMotionAccumFilterGen::destroyResources() {
    PCGraphicsDestroyFramebuffer(accumulation_fbo);
    accumulation_fbo = NULL;
    NuPostFilterGen::destroyResources();
}
void NuMotionAccumFilterGen::destroyTextureResources() {
    PCGraphicsDestroyRenderTexture(accumulation_texture);
}
void NuSpeedBlurFilterGen::destroyTextureResources() {
    PCGraphicsDestroyRenderTexture(texture);
}
void NuDeferredFilterGen::destroyResources() {
    for (i32 i = 0; i < 6; ++i)
        PCGraphicsDestroyFramebuffer(shadow_fbos[i]);
    PCGraphicsDestroyFramebuffer(light_fbo);
    NuPostFilterGen::destroyResources();
}
void NuDeferredFilterGen::destroyTextureResources() {
    for (i32 i = 0; i < 2; ++i) {
        PCGraphicsAttachFramebuffer(shadow_fbos[i], 0, NULL, 0);
        PCGraphicsAttachFramebuffer(shadow_fbos[i], 4, NULL, 0);
    }
    PCGraphicsAttachFramebuffer(light_fbo, 0, NULL, 0);
    PCGraphicsDestroyRenderTexture(textures[2]);
    PCGraphicsDestroyRenderTexture(textures[3]);
    textures[2] = NULL;
    textures[3] = NULL;
    textures[0] = NULL;
    PCGraphicsDestroyRenderTexture(textures[1]);
    textures[1] = NULL;
    for (i32 i = 4; i < 6; ++i) {
        PCGraphicsDestroyRenderTexture(textures[i]);
        textures[i] = NULL;
    }
}
void NuMainFilterGen::destroyResources() {
    PCGraphicsDestroyFramebuffer(blur_fbo);
    blur_fbo = NULL;
    NuPostFilterGen::destroyResources();
}
void NuMainFilter::destroyResources() {
    NuMainFilterGen::destroyResources();
    PCGraphicsDestroyFramebuffer(platform_fbo);
}
void NuMainFilter::destroyTextureResources() {
    texture_surface.Reset();
}

#define POST_FILTER_BIND_TEXTURE(stage, texture)                                                                       \
    do {                                                                                                               \
        GRAPHICS_RENDER_TEXTURE_PC *texture_pc = (texture);                                                            \
        if (texture_pc != NULL) {                                                                                      \
            u32 *tuple_pc = PCGraphicsEffectTextureSamplerPc(texture_pc);                                              \
            if ((u32)PCDeviceSamplerInvalidation[stage][0] != tuple_pc[0] ||                                           \
                (u32)PCDeviceSamplerInvalidation[stage][1] != tuple_pc[1])                                             \
                PCGraphicsSetSamplerTuplePc(stage, PCGraphicsEffectTextureSamplerPc(texture_pc));                      \
        }                                                                                                              \
        PCGraphicsBindEffectTexture(texture_pc, stage);                                                                \
    } while (0)

extern "C" f32 NuMotionFilterCentrePc;
f32 NuMotionFilterCentrePc = 0.5f;
void NuMotionFilterGen::render() {
    PC_FRAMEBUFFER_PC *output =
        (PC_FRAMEBUFFER_PC *)PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].data;
    PC_POST_BUFFER_PROXY_PC *color =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data;
    PC_POST_BUFFER_PROXY_PC *velocity =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[3].manager->entries[PostResourcePortsPc[3].index].data;
    if (!color->resolved) {
        PCGraphicsFramebufferBarrierPc(color->kind, color->enabled);
        color->resolved = true;
    }
    if (!velocity->resolved) {
        PCGraphicsFramebufferBarrierPc(velocity->kind, velocity->enabled);
        velocity->resolved = true;
    }
    GRAPHICS_RENDER_TEXTURE_PC *color_texture = color->texture;
    GRAPHICS_RENDER_TEXTURE_PC *velocity_texture = velocity->texture;
    GRAPHICS_RENDER_TEXTURE_PC *destination = PCGraphicsGetFramebufferAttachmentPc(output, 0, NULL, NULL);
    PostShaderProgramBindPc((POST_SHADER_PROGRAM_PC *)programs[0]);
    f32 ratio = maximum / scale;
    __declspec(align(16)) f32 params[4];
    __declspec(align(16)) f32 weights[7][4];
    for (i32 i = 0; i < 7; ++i) {
        weights[i][0] = (NuPow((f32)((i + 1.0) / 7.0), falloff) - 0.5) * ratio;
        weights[i][1] = weights[i][0];
        weights[i][3] = weights[i][2] = 0.0f;
    }
    params[0] = ratio;
    params[1] = NuMotionFilterCentrePc;
    params[3] = params[2] = 0.0f;
    PCDeviceSetPixelShaderConstantsF(0x8a, params, 1);
    PCDeviceSetPixelShaderConstantsF(0x8b, &weights[0][0], 7);
    PCGraphicsEffectTextureSetFilterFields(color_texture, 2, 2, 2);
    PCGraphicsEffectTextureSetWrapFields(color_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(0, color_texture);
    PCGraphicsEffectTextureSetFilterFields(velocity_texture, 2, 2, 0);
    PCGraphicsEffectTextureSetWrapFields(velocity_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(2, velocity_texture);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PCGraphicsBindFramebuffer(output);
    PCGraphicsDrawPostEffectQuadPc();
    color->texture = destination;
    color->kind = 4;
    color->enabled = true;
    color->resolved = false;
    PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data = color;
}

extern "C" {
    extern POST_SHADER_PROGRAM_PC *PostCopyTexProgramPc;
    extern PC_FRAMEBUFFER_PC *PostCopyFramebufferPc;
    f32 NuSpeedBlurDepthScalePc = 100000.0f;
}

extern "C" void __cdecl PostFilterBlitTexturePc(GRAPHICS_RENDER_TEXTURE_PC *destination, i32 destination_level,
                                                GRAPHICS_RENDER_TEXTURE_PC *source, i32 source_level,
                                                POST_SHADER_PROGRAM_PC *program, GRAPHICS_RENDER_TEXTURE_PC *extra) {
    i32 height, width;
    PCGraphicsEffectTextureDimensions(destination, destination_level, &width, &height);
    PCGraphicsAttachFramebuffer(PostCopyFramebufferPc, 0, destination, destination_level);
    PCGraphicsBindFramebuffer(PostCopyFramebufferPc);
    PCGraphicsSubmitViewportPc(0, 0, width, height);
    PostShaderProgramBindPc(program);
    PCGraphicsEffectTextureSetFilterFields(source, 2, 2, 1);
    PCGraphicsEffectTextureSetLowNibbles(source, source_level, source_level);
    POST_FILTER_BIND_TEXTURE(0, source);
    if (extra != NULL)
        POST_FILTER_BIND_TEXTURE(1, extra);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PCGraphicsDrawPostEffectQuadPc();
    PCGraphicsFramebufferBarrierPc(0, 1);
}

static __forceinline void PostFilterBindProgramPc(const POST_SHADER_PROGRAM_PC *program) {
    if (program != NULL) {
        if (NuDisplayHandlerVertexShaderCachePc != program->vertex_shader) {
            PCDeviceSetVertexShaderPc(program->vertex_shader);
            NuDisplayHandlerVertexShaderCachePc = program->vertex_shader;
        }
        if (NuDisplayHandlerPixelShaderCachePc != program->pixel_shader) {
            PCDeviceSetPixelShaderPc(program->pixel_shader);
            NuDisplayHandlerPixelShaderCachePc = program->pixel_shader;
        }
    } else {
        PCDeviceSetVertexShaderPc(NULL);
        PCDeviceSetPixelShaderPc(NULL);
        NuDisplayHandlerVertexShaderCachePc = NULL;
        NuDisplayHandlerPixelShaderCachePc = NULL;
    }
}

void NuMotionAccumFilterGen::render() {
    PC_FRAMEBUFFER_PC *output =
        (PC_FRAMEBUFFER_PC *)PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].data;
    PC_POST_BUFFER_PROXY_PC *color =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data;
    if (!color->resolved) {
        PCGraphicsFramebufferBarrierPc(color->kind, color->enabled);
        color->resolved = true;
    }
    GRAPHICS_RENDER_TEXTURE_PC *color_texture = color->texture;
    GRAPHICS_RENDER_TEXTURE_PC *destination = PCGraphicsGetFramebufferAttachmentPc(output, 0, NULL, NULL);
    f32 weights[256];
    if (frames > 256)
        AnimationDiagnosticNoOpPc();
    f32 sum = 0.0f;
    for (i32 i = 0; i < frames; ++i) {
        f32 position = (f32)(i + 1) / frames;
        weights[i] = NuPow(position, mode_pc);
        sum += weights[i];
    }
    for (i32 i = 0; i < frames; ++i)
        weights[i] *= 1.0f / sum;
    sum = 0.0f;
    for (i32 i = 0; i < frames; ++i) {
        sum += weights[i];
        weights[i] /= sum;
    }
    if (++current_frame == frames)
        current_frame = 0;
    PostFilterBindProgramPc((POST_SHADER_PROGRAM_PC *)program);
    f32 params[4];
    params[1] = 0.0f;
    params[2] = 0.0f;
    params[3] = 0.0f;
    params[0] = weights[current_frame];
    PCDeviceSetPixelShaderConstantsF(0x8a, params, 1);
    PCGraphicsEffectTextureSetFilterFields(color_texture, 1, 1, 0);
    PCGraphicsEffectTextureSetWrapFields(color_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(0, color_texture);
    PCGraphicsEffectTextureSetFilterFields(accumulation_texture, 1, 1, 0);
    PCGraphicsEffectTextureSetWrapFields(accumulation_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(1, accumulation_texture);
    i32 width, height;
    PCGraphicsEffectTextureDimensions(accumulation_texture, 0, &width, &height);
    PCGraphicsBindFramebuffer(accumulation_fbo);
    PCGraphicsSubmitViewportPc(0, 0, width, height);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PCGraphicsDrawPostEffectQuadPc();
    PCGraphicsFramebufferBarrierPc(0, 1);
    PostFilterBindProgramPc((POST_SHADER_PROGRAM_PC *)program);
    PCDeviceSetPixelShaderConstantsF(0x8a, params, 1);
    PCGraphicsEffectTextureSetFilterFields(color_texture, 1, 1, 0);
    PCGraphicsEffectTextureSetWrapFields(color_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(0, accumulation_texture);
    PCGraphicsEffectTextureSetFilterFields(accumulation_texture, 1, 1, 0);
    PCGraphicsEffectTextureSetWrapFields(accumulation_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(1, accumulation_texture);
    PCGraphicsBindFramebuffer(output);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PCGraphicsDrawPostEffectQuadPc();
    color->texture = destination;
    color->kind = 4;
    color->enabled = true;
    color->resolved = false;
    PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data = color;
}

void NuSpeedBlurFilterGen::render() {
    PC_FRAMEBUFFER_PC *output =
        (PC_FRAMEBUFFER_PC *)PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].data;
    PC_POST_BUFFER_PROXY_PC *color =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data;
    PC_POST_BUFFER_PROXY_PC *depth =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[5].manager->entries[PostResourcePortsPc[5].index].data;
    if (!color->resolved) {
        PCGraphicsFramebufferBarrierPc(color->kind, color->enabled);
        color->resolved = true;
    }
    if (!depth->resolved) {
        PCGraphicsFramebufferBarrierPc(depth->kind, depth->enabled);
        depth->resolved = true;
    }
    __declspec(align(16)) f32 values[8];
    values[0] = 0.0f;
    values[1] = parameters->scale * NuSpeedBlurDepthScalePc;
    computeSpeedBlur((NUVEC4 *)(values + 4));
    GRAPHICS_RENDER_TEXTURE_PC *color_texture = color->texture;
    GRAPHICS_RENDER_TEXTURE_PC *depth_texture = depth->texture;
    GRAPHICS_RENDER_TEXTURE_PC *destination = PCGraphicsGetFramebufferAttachmentPc(output, 0, NULL, NULL);
    if (texture != NULL)
        PostFilterCopyTexturePc(texture, 0, color_texture, 0, PostCopyTexProgramPc, NULL);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PostShaderProgramBindPc((POST_SHADER_PROGRAM_PC *)programs[0]);
    PCDeviceSetVertexShaderConstantsF(0x80, values, 2);
    PCGraphicsEffectTextureSetFilterFields(color_texture, 2, 2, 1);
    PCGraphicsEffectTextureSetLowNibbles(color_texture, 0, 2);
    PCGraphicsEffectTextureSetWrapFields(color_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(0, color_texture);
    PCGraphicsEffectTextureSetFilterFields(depth_texture, 2, 2, 1);
    PCGraphicsEffectTextureSetWrapFields(depth_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(4, depth_texture);
    POST_FILTER_BIND_TEXTURE(9, texture != NULL ? texture : color_texture);
    PCGraphicsBindFramebuffer(output);
    PCGraphicsDrawPostEffectIndexedPc();
    color->texture = destination;
    color->kind = 0;
    color->enabled = true;
    color->resolved = false;
    PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data = color;
}

extern "C" {
    extern POST_SHADER_PROGRAM_PC *PostBlur5x5ProgramPc;
    void __cdecl PostBlur7x7DispatchPc(GRAPHICS_RENDER_TEXTURE_PC *, i32, GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32,
                                       bool, f32, POST_SHADER_PROGRAM_PC *);
    void __cdecl PCGraphicsResolveFramebufferPc(i32);
    f32 NuMainFilterDofBlurScalePc = 1.5f;
    f32 NuMainFilterDofWeightPc = 1.0f;
}

void NuMainFilterGen::preprocessBlurTextures(GRAPHICS_RENDER_TEXTURE_PC *color, GRAPHICS_RENDER_TEXTURE_PC *normal) {
    (void)normal;
    i32 width = PCGraphicsEffectTextureWidth(color);
    i32 height = PCGraphicsEffectTextureHeight(color);
    if (!dof_enabled && !bloom_enabled)
        return;
    i32 levels;
    if (dof_enabled) {
        i32 smaller = width < height ? width : height;
        f32 scaled = RenderPostLogPc((f32)smaller) * -3.321928024291992;
        levels = -6 - (i32)scaled;
    } else {
        levels = downsample_lod;
    }
    PostBlur7x7DispatchPc(color, 1, color, 2, 1, levels, true, 1.0f, PostBlur5x5ProgramPc);
    if (bloom_enabled) {
        f32 threshold[4];
        threshold[0] = bloom->threshold < 0.0f ? 0.0f : (bloom->threshold > 1.0f ? 1.0f : bloom->threshold);
        threshold[1] = 0.0f;
        threshold[2] = 0.0f;
        threshold[3] = 0.0f;
        static f32 scaleBias[4] = {1.0f, 1.0f, 0.0f, 0.0f};
        PostShaderProgramBindPc((POST_SHADER_PROGRAM_PC *)programs[17]);
        PCDeviceSetVertexShaderConstantsF(0xae, threshold, 1);
        PCDeviceSetVertexShaderConstantsF(0xaf, scaleBias, 1);
        PostFilterCopyTexturePc(downsample_texture, 0, color, downsample_lod, (POST_SHADER_PROGRAM_PC *)programs[17],
                                NULL);
        i32 passes = (i32)bloom->blur_iterations;
        if (passes >= 1)
            PostBlur7x7DispatchPc(downsample_texture, 0, downsample_texture, 0, passes, 1, true, 1.0f,
                                  PostBlur5x5ProgramPc);
        f32 remainder = bloom->blur_iterations - passes;
        if (remainder > 0.0f)
            PostBlur7x7DispatchPc(downsample_texture, 0, downsample_texture, 0, 1, 1, true, remainder,
                                  PostBlur5x5ProgramPc);
    }
}

void NuMainFilterGen::preprocessDofMotionBlur(GRAPHICS_RENDER_TEXTURE_PC *depth) {
    POST_SHADER_PROGRAM_PC *program;
    if (dof_enabled) {
        if (dof_bias == 1)
            program = (POST_SHADER_PROGRAM_PC *)(motion_blur_enabled ? programs[4] : programs[3]);
        else
            program = (POST_SHADER_PROGRAM_PC *)(motion_blur_enabled ? programs[2] : programs[0]);
    } else {
        program = (POST_SHADER_PROGRAM_PC *)programs[1];
    }
    PostShaderProgramBindPc(program);
    if (dof_enabled) {
        f32 fov, aspect, near_z, far_z;
        PcGetGeometryPerspectivePc(&PcGeometryProjectionMatrixPc, &fov, &aspect, &near_z, &far_z);
        f32 projection_scale = far_z / (far_z - near_z);
        f32 strength_near = dof_strength * dof_near;
        f32 product = dof_far * strength_near;
        f32 denominator = near_z * projection_scale * (dof_far - dof_near);
        f32 values[4];
        values[0] = product / denominator;
        values[1] = strength_near / (dof_far - dof_near) - projection_scale * product / denominator;
        values[2] = dof_blur;
        values[3] = 0.0f;
        PCDeviceSetVertexShaderConstantsF(0x83, values, 1);
    }
    if (motion_blur_enabled) {
        NUMTX bias = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f};
        NUMTX inverse_bias, inverse_previous, transform;
        NuMtxInvH(&inverse_bias, &bias);
        NuMtxInvH(&inverse_previous, &motion_previous);
        NuMtxMulH(&transform, &inverse_bias, &inverse_previous);
        NuMtxMulH(&transform, &transform, &motion_current);
        NuMtxMulH(&transform, &transform, &bias);
        NuMtxTranspose(&transform, &transform);
        PCDeviceSetPixelShaderConstantsF(0x84, (f32 *)&transform, 4);
    }
    if (dof_enabled || motion_blur_enabled) {
        i32 width, height;
        PCGraphicsEffectTextureDimensions(blur_texture, 0, &width, &height);
        PCGraphicsEffectTextureSetFilterFields(depth, 2, 2, 0);
        POST_FILTER_BIND_TEXTURE(2, depth);
        PCGraphicsAttachFramebuffer(blur_fbo, 0, blur_texture, 0);
        PCGraphicsBindFramebuffer(blur_fbo);
        PCGraphicsSubmitViewportPc(0, 0, width, height);
        PCGraphicsDrawPostEffectQuadPc();
        PCGraphicsResolveFramebufferPc(1);
        if (dof_mode > 0)
            PostBlur7x7DispatchPc(blur_texture, 0, blur_texture, 1, dof_mode, 1, true, NuMainFilterDofBlurScalePc,
                                  (POST_SHADER_PROGRAM_PC *)programs[15]);
    }
}

/* Normalizes the xyz part and makes the result a homogeneous direction (w = 1). */
static __forceinline void PostVec4NormalizeDirection(NUVEC4 *vector) {
    NuVecNorm((NUVEC *)vector, (NUVEC *)vector);
    vector->w = 1.0f;
}

void NuMainFilterGen::render() {
    D3DPERF_BeginEvent((D3DCOLOR)-1, L"bloom/dof");
    PC_FRAMEBUFFER_PC *output =
        (PC_FRAMEBUFFER_PC *)PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].data;
    PC_POST_BUFFER_PROXY_PC *color =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data;
    PC_POST_BUFFER_PROXY_PC *normal =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[2].manager->entries[PostResourcePortsPc[2].index].data;
    PC_POST_BUFFER_PROXY_PC *depth =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[5].manager->entries[PostResourcePortsPc[5].index].data;
    if (!color->resolved) {
        PCGraphicsFramebufferBarrierPc(color->kind, color->enabled);
        color->resolved = true;
    }
    if (normal->texture != NULL && !normal->resolved) {
        PCGraphicsFramebufferBarrierPc(normal->kind, normal->enabled);
        normal->resolved = true;
    }
    GRAPHICS_RENDER_TEXTURE_PC *color_texture = color->texture;
    GRAPHICS_RENDER_TEXTURE_PC *depth_texture = depth->texture;
    GRAPHICS_RENDER_TEXTURE_PC *normal_texture = normal->texture;
    GRAPHICS_RENDER_TEXTURE_PC *destination = PCGraphicsGetFramebufferAttachmentPc(output, 0, NULL, NULL);
    GRAPHICS_RENDER_TEXTURE_PC *depth_destination = PCGraphicsGetFramebufferAttachmentPc(output, 4, NULL, NULL);
    if (bloom_enabled || dof_enabled)
        preprocessBlurTextures(color_texture, normal_texture);
    if (dof_enabled || motion_blur_enabled)
        preprocessDofMotionBlur(depth_texture);
    i32 selection = (bloom_enabled && bloom->directional ? 1 : 0) | (bloom_enabled && !bloom->directional ? 2 : 0) |
                    (dof_enabled ? 4 : 0);
    POST_SHADER_PROGRAM_PC *program = NULL;
    switch (selection) {
        case 1:
            program = (POST_SHADER_PROGRAM_PC *)programs[8];
            break;
        case 2:
            program = (POST_SHADER_PROGRAM_PC *)programs[6];
            break;
        case 4:
            program = (POST_SHADER_PROGRAM_PC *)programs[5];
            break;
        case 5:
            program = (POST_SHADER_PROGRAM_PC *)programs[9];
            break;
        case 6:
            program = (POST_SHADER_PROGRAM_PC *)programs[7];
            break;
        case 9:
            program = (POST_SHADER_PROGRAM_PC *)programs[13];
            break;
        case 10:
            program = (POST_SHADER_PROGRAM_PC *)programs[11];
            break;
        case 12:
            program = (POST_SHADER_PROGRAM_PC *)programs[10];
            break;
        case 13:
            program = (POST_SHADER_PROGRAM_PC *)programs[14];
            break;
        case 14:
            program = (POST_SHADER_PROGRAM_PC *)programs[12];
            break;
        default:
            AnimationDiagnosticNoOpPc();
            break;
    }
    PostShaderProgramBindPc(program);
    if (dof_enabled) {
        PCGraphicsEffectTextureWidth(color_texture);
        PCGraphicsEffectTextureHeight(color_texture);
        __declspec(align(16)) f32 params[4] = {dof_blur, NuMainFilterDofWeightPc, 0.0f, 0.0f};
        __declspec(align(16)) f32 jitter[4] = {(f32)(rand() / 32767.0), (f32)(rand() / 32767.0), 0.0f, 0.0f};
        PCDeviceSetVertexShaderConstantsF(0x80, params, 1);
        PCDeviceSetVertexShaderConstantsF(0x81, jitter, 1);
        PCGraphicsEffectTextureSetFilterFields(blur_texture, 2, 2, 2);
        PCGraphicsEffectTextureSetWrapFields(blur_texture, 2, 2);
        PCGraphicsEffectTextureSetLowNibbles(blur_texture, 0, 13);
        POST_FILTER_BIND_TEXTURE(1, blur_texture);
    }
    if (bloom_enabled) {
        f32 blend = bloom->blend;
        __declspec(align(16)) f32 params[4] = {bloom->intensity * blur_gain, blend, 0.0f, 0.0f};
        PCDeviceSetVertexShaderConstantsF(0x82, params, 1);
        f32 scale_range = (bloom->far_scale - bloom->near_scale) * bloom->intensity;
        f32 angle_start = bloom->near_angle / 180.0;
        f32 angle_range = 180.0 / (bloom->far_angle - bloom->near_angle);
        __declspec(align(16)) f32 angle[4] = {bloom->near_scale * bloom->intensity, scale_range, angle_start,
                                              angle_range};
        PCDeviceSetVertexShaderConstantsF(0x88, angle, 1);
        if (bloom->directional) {
            f32 direction_scale_range = (bloom->direction_far_scale - bloom->direction_near_scale) * bloom->intensity;
            f32 direction_angle_start = bloom->direction_near_angle * 3.1415927410125732 / 180.0;
            f32 direction_angle_range =
                180.0 / ((bloom->direction_far_angle - bloom->direction_near_angle) * 3.1415927410125732);
            __declspec(align(16)) f32 directional[4] = {bloom->direction_near_scale * bloom->intensity,
                                                        direction_scale_range, direction_angle_start,
                                                        direction_angle_range};
            PCDeviceSetVertexShaderConstantsF(0x89, directional, 1);
            f32 direction_y = bloom->direction.y;
            f32 direction_z = bloom->direction.z;
            __declspec(align(16)) NUVEC4 direction = {bloom->direction.x, direction_y, direction_z, 1.0f};
            PostVec4NormalizeDirection(&direction);
            direction.w = bloom->direction_bias;
            PCDeviceSetVertexShaderConstantsF(0x8a, &direction.x, 1);
        }
        PCGraphicsEffectTextureSetFilterFields(downsample_texture, 2, 2, 0);
        PCGraphicsEffectTextureSetWrapFields(downsample_texture, 2, 2);
        POST_FILTER_BIND_TEXTURE(3, downsample_texture);
    }
    PCGraphicsEffectTextureSetFilterFields(color_texture, 2, 2, 2);
    PCGraphicsEffectTextureSetWrapFields(color_texture, 2, 2);
    PCGraphicsEffectTextureSetLowNibbles(color_texture, 0, 13);
    POST_FILTER_BIND_TEXTURE(0, color_texture);
    if (depth_destination != NULL) {
        PCGraphicsEffectTextureSetFilterFields(depth_texture, 2, 2, 0);
        POST_FILTER_BIND_TEXTURE(2, depth_texture);
        PCDeviceSetRenderState(22, 1);
        PCDeviceSetRenderState(15, 0);
        PCDeviceSetRenderState(27, 0);
        PCDeviceSetRenderState(7, 1);
        PCDeviceSetRenderState(14, 1);
        PCDeviceSetRenderState(23, 8);
    } else {
        PCDeviceSetRenderState(22, 1);
        PCDeviceSetRenderState(15, 0);
        PCDeviceSetRenderState(27, 0);
        PCDeviceSetRenderState(7, 0);
        PCDeviceSetRenderState(168, 15);
    }
    PCGraphicsBindFramebuffer(output);
    PCGraphicsSubmitViewportPc(0, 0, PCGraphicsEffectTextureWidth(destination),
                               PCGraphicsEffectTextureHeight(destination));
    PCGraphicsDrawPostEffectIndexedPc();
    color->texture = destination;
    color->kind = 0;
    color->enabled = true;
    color->resolved = false;
    PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data = color;
    if (depth_destination != NULL) {
        depth->texture = depth_destination;
        depth->kind = 4;
        depth->enabled = true;
        depth->resolved = false;
        PostResourcePortsPc[5].manager->entries[PostResourcePortsPc[5].index].data = depth;
    }
    D3DPERF_EndEvent();
}

extern "C" {
    extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectAuxTexturePc;
}

/* Drops this module's references to the shared post ports and frees the auxiliary and work targets. */
extern "C" void __cdecl NuPostEffectReleasePortsPc(void) {
    {
        PC_POST_DATA_PORT_ENTRY *entry = &PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index];
        entry->references--;
    }
    {
        PC_POST_DATA_PORT_ENTRY *entry = &PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index];
        entry->references--;
    }
    {
        PC_POST_DATA_PORT_ENTRY *entry = &PostResourcePortsPc[2].manager->entries[PostResourcePortsPc[2].index];
        entry->references--;
    }
    {
        PC_POST_DATA_PORT_ENTRY *entry = &PostResourcePortsPc[4].manager->entries[PostResourcePortsPc[4].index];
        entry->references--;
    }
    {
        PC_POST_DATA_PORT_ENTRY *entry = &PostResourcePortsPc[5].manager->entries[PostResourcePortsPc[5].index];
        entry->references--;
    }
    if (PCGraphicsPostEffectAuxTexturePc != NULL) {
        PCGraphicsDestroyRenderTexture(PCGraphicsPostEffectAuxTexturePc);
        PCGraphicsPostEffectAuxTexturePc = NULL;
    }
    if (PCGraphicsPostEffectWorkTexturePc != NULL) {
        PCGraphicsDestroyRenderTexture(PCGraphicsPostEffectWorkTexturePc);
        PCGraphicsPostEffectWorkTexturePc = NULL;
    }
}

extern "C" {
    extern POST_SHADER_PROGRAM_PC *PostBlendTexProgramPc;
    extern PC_FRAMEBUFFER_PC *PostBlurFramebufferPc;
    extern f32 PostGaussianTexelBiasPc;
    void __cdecl PostGaussianKernelPc(i32 width, i32 height, NUVEC4 *taps, f32 scale);
}

/* Blurs a texture chain with the 13-tap Gaussian kernel, one level at a time. */
extern "C" void __cdecl PostBlurGaussianChainPc(GRAPHICS_RENDER_TEXTURE_PC *source, i32 source_lod,
                                                GRAPHICS_RENDER_TEXTURE_PC *textures, i32 first_lod, i32 iterations,
                                                i32 levels, bool mip_chain) {
    PostShaderProgramBindPc(PostBlendTexProgramPc);
    for (i32 lod = 0; lod < levels; ++lod) {
        GRAPHICS_RENDER_TEXTURE_PC *input;
        i32 input_lod;
        if (lod == 0) {
            input = source;
            input_lod = source_lod;
        } else if (mip_chain) {
            input = textures;
            input_lod = first_lod + lod - 1;
        } else {
            input = textures + first_lod + lod - 1;
            input_lod = 0;
        }
        GRAPHICS_RENDER_TEXTURE_PC *output = mip_chain ? textures : textures + first_lod + lod;
        i32 output_lod = mip_chain ? first_lod + lod : 0;
        for (i32 pass = 0; pass < iterations; ++pass) {
            i32 width, height, output_width, output_height;
            __declspec(align(16)) NUVEC4 taps[13];
            PCGraphicsEffectTextureDimensions(input, (u16)input_lod, &width, &height);
            PCGraphicsEffectTextureDimensions(output, (u16)output_lod, &output_width, &output_height);
            PostGaussianKernelPc(width, height, taps, 1.0f);
            f32 bias[4] = {1.0f, 1.0f, PostGaussianTexelBiasPc / (f32)width, PostGaussianTexelBiasPc / (f32)height};
            PCGraphicsAttachFramebuffer(PostBlurFramebufferPc, 0, output, output_lod);
            PCGraphicsBindFramebuffer(PostBlurFramebufferPc);
            PCGraphicsSubmitViewportPc(0, 0, output_width, output_height);
            PCGraphicsEffectTextureSetWrapFields(input, 2, 2);
            if (width <= output_width && height <= output_height)
                PCGraphicsEffectTextureSetFilterFields(input, 1, 1, 1);
            else
                PCGraphicsEffectTextureSetFilterFields(input, 2, 2, 1);
            PCGraphicsEffectTextureSetLowNibbles(input, input_lod, input_lod);
            POST_FILTER_BIND_TEXTURE(0, input);
            PCDeviceSetVertexShaderConstantsF(0xa0, &taps[0].x, 13);
            PCDeviceSetVertexShaderConstantsF(0xad, bias, 1);
            PCGraphicsDrawPostEffectQuadPc();
            PCGraphicsFramebufferBarrierPc(0, 1);
            input = output;
            input_lod = output_lod;
        }
    }
}

extern "C" void __cdecl PCGraphicsDestroyPostEffectTexturesPc(void) {
    NuPostEffectReleasePortsPc();
    PCGraphicsDestroyRenderTexture(PCGraphicsPostEffectBackBufferTexturePc);
}

#define POST_FILTER_RELEASE(filter)                                                                                    \
    do {                                                                                                               \
        if ((filter) != NULL) {                                                                                        \
            NuPostFilterGen *released = (NuPostFilterGen *)(filter);                                                   \
            released->destroyTextureResources();                                                                       \
            released->destroyResources();                                                                              \
            (filter) = NULL;                                                                                           \
        }                                                                                                              \
    } while (0)

/* Tears every post filter down, frees the shared targets and rewinds the filter arena. */
extern "C" void __cdecl NuPostEffectReleasePc(void) {
    POST_FILTER_RELEASE(NuPostMainFilterPc);
    POST_FILTER_RELEASE(NuPostMotionFilterPc);
    POST_FILTER_RELEASE(NuPostAccumFilterPc);
    POST_FILTER_RELEASE(NuPostSpeedFilterPc);
    POST_FILTER_RELEASE(NuPostDeferredFilterPc);
    POST_FILTER_RELEASE(NuPostCopyFilterPc);
    PCGraphicsDestroyPostEffectTexturesPc();
    NuPostFilterAllocatorPc.cursor = NuPostFilterAllocatorPc.base;
}
