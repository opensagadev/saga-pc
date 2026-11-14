#ifndef SAGA_NU2API_NUCORE_NUMAINFILTERGEN_H
#define SAGA_NU2API_NUCORE_NUMAINFILTERGEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuPostFilter.h"
#include "nu2api/numath/numtx.h"

#ifdef __cplusplus

class NuMainFilterGen : public NuPostFilter {
  public:
    NuMainFilterGen();
    virtual void initResources();
    virtual void destroyResources();
    virtual void initTextureResources(i32, i32);
    virtual void destroyTextureResources();
    virtual void render();
    virtual void reset();
    virtual bool isEnabled();
    void preprocessBlurTextures(GRAPHICS_RENDER_TEXTURE_PC *color, GRAPHICS_RENDER_TEXTURE_PC *normal);
    void preprocessDofMotionBlur(GRAPHICS_RENDER_TEXTURE_PC *depth);
    void fun_005c2310(i32 filter, bool enable);
    bool fun_005c2360(i32 filter);
    void fun_005c2370(const NU_POST_DOF_PARAMETERS_PC *parameters);
    void fun_005c23a0(const NU_POST_BLOOM_PARAMETERS_PC *parameters);
    void fun_005c23b0(const NUMTX *previous, const NUMTX *current, f32 scale, f32 maximum, f32 falloff);
    void fun_005c2400(i32 frames, f32 blend, f32 mode);
    void *programs[18];
    GRAPHICS_RENDER_TEXTURE_PC *blur_texture;
    PC_FRAMEBUFFER_PC *blur_fbo;
    GRAPHICS_RENDER_TEXTURE_PC *downsample_texture;
    f32 blur_gain;
    i32 downsample_lod;
    const NU_POST_BLOOM_PARAMETERS_PC *bloom;
    f32 blur_radius;
    f32 dof_strength;
    f32 dof_near;
    f32 dof_far;
    f32 dof_blur;
    i32 dof_mode;
    i32 dof_bias;
    bool dof_enabled;
    bool bloom_enabled;
    bool motion_blur_enabled;
    u8 unknown_08b;
    i32 active_filter_count;
    NUMTX motion_previous;
    NUMTX motion_current;
    f32 motion_scale;
    f32 motion_maximum;
    f32 motion_falloff;
    i32 accumulation_frames;
    f32 accumulation_blend;
    f32 accumulation_mode_pc;
};

STATIC_ASSERT(sizeof(NuMainFilterGen) == 0x128);
STATIC_ASSERT(offsetof(NuMainFilterGen, bloom) == 0x68);
STATIC_ASSERT(offsetof(NuMainFilterGen, dof_enabled) == 0x88);
STATIC_ASSERT(offsetof(NuMainFilterGen, bloom_enabled) == 0x89);
STATIC_ASSERT(offsetof(NuMainFilterGen, active_filter_count) == 0x8c);
#endif

#endif
