#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuDynamicLight.h"
#include "nu2api/nucore/NuPostFilter.h"

struct NuDynamicLight;
struct PC_FRAMEBUFFER;

#ifdef __cplusplus

class NuDeferredFilterGen : public NuPostFilter { // size 0x2f8
  public:
    NuDeferredFilterGen();
    void setEnabled(bool enable);
    void fun_005c30b0(NuDynamicLight *light);
    i32 getDynamicLightCount();
    void fun_005c30e0(const f32 *parameters);
    virtual void initResources();
    virtual void destroyResources();
    virtual void initTextureResources(i32, i32);
    virtual void destroyTextureResources();
    virtual void render();
    virtual void resetAll();
    virtual bool isEnabled();
    i32 sample_count;                     // 0x00c
    POST_SHADER_PROGRAM *programs[6];     // 0x010
    GRAPHICS_RENDER_TEXTURE *textures[6]; // 0x028
    u32 unknown_040;                      // 0x040
    f32 parameters[4];                    // 0x044
    i32 dynamic_light_count;              // 0x054
    NuDynamicLight *dynamic_lights[32];   // 0x058
    i32 deferred_geometry_count;          // 0x0d8
    void *geometry[128];                  // 0x0dc
    PC_FRAMEBUFFER *shadow_fbos[6];       // 0x2dc
    PC_FRAMEBUFFER *light_fbo;            // 0x2f4
};

#endif
