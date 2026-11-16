#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuPostFilter.h"
#include "nu2api/numath/numtx.h"

struct NU_POST_BLOOM_PARAMETERS;
struct PC_FRAMEBUFFER;

#ifdef __cplusplus

class NuMainFilterGen : public NuPostFilter { // size 0x128
  public:
    NuMainFilterGen();
    virtual void initResources();
    virtual void destroyResources();
    virtual void initTextureResources(i32, i32);
    virtual void destroyTextureResources();
    virtual void render();
    virtual void reset();
    virtual bool isEnabled();
    void preprocessBlurTextures(GRAPHICS_RENDER_TEXTURE *color, GRAPHICS_RENDER_TEXTURE *normal);
    void preprocessDofMotionBlur(GRAPHICS_RENDER_TEXTURE *depth);
    void fun_005c2310(i32 filter, bool enable);
    bool fun_005c2360(i32 filter);
    void fun_005c2370(const NU_POST_DOF_PARAMETERS *parameters);
    void fun_005c23a0(const NU_POST_BLOOM_PARAMETERS *parameters);
    void fun_005c23b0(const NUMTX *previous, const NUMTX *current, f32 scale, f32 maximum, f32 falloff);
    void fun_005c2400(i32 frames, f32 blend, f32 mode);
    POST_SHADER_PROGRAM *programs[18];           // 0x00c
    GRAPHICS_RENDER_TEXTURE *blur_texture;       // 0x054
    PC_FRAMEBUFFER *blur_fbo;                    // 0x058
    GRAPHICS_RENDER_TEXTURE *downsample_texture; // 0x05c
    f32 blur_gain;                               // 0x060
    i32 downsample_lod;                          // 0x064
    const NU_POST_BLOOM_PARAMETERS *bloom;       // 0x068
    f32 blur_radius;                             // 0x06c
    f32 dof_strength;                            // 0x070
    f32 dof_near;                                // 0x074
    f32 dof_far;                                 // 0x078
    f32 dof_blur;                                // 0x07c
    i32 dof_mode;                                // 0x080
    i32 dof_bias;                                // 0x084
    bool dof_enabled;                            // 0x088
    bool bloom_enabled;                          // 0x089
    bool motion_blur_enabled;                    // 0x08a
    u8 unknown_08b;                              // 0x08b
    i32 active_filter_count;                     // 0x08c
    NUMTX motion_previous;                       // 0x090
    NUMTX motion_current;                        // 0x0d0
    f32 motion_scale;                            // 0x110
    f32 motion_maximum;                          // 0x114
    f32 motion_falloff;                          // 0x118
    i32 accumulation_frames;                     // 0x11c
    f32 accumulation_blend;                      // 0x120
    f32 accumulation_weight_exponent;            // 0x124
};

#endif
