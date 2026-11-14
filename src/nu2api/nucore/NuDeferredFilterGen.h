/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuDynamicLight.h"
#include "nu2api/nucore/NuPostFilter.h"
#endif

#ifdef __cplusplus

class NuDeferredFilterGen : public NuPostFilter { // size 0x2f8
  public:
    NuDeferredFilterGen();
    void fun_005c30a0(bool enable);
    void fun_005c30b0(NuDynamicLight *light);
    i32 fun_005c30d0();
    void fun_005c30e0(const f32 *parameters);
    virtual void initResources();
    virtual void destroyResources();
    virtual void initTextureResources(i32, i32);
    virtual void destroyTextureResources();
    virtual void render();
    virtual void resetAll();
    virtual bool isEnabled();
    i32 sample_count;                        // 0x00c
    void *programs[6];                       // 0x010
    GRAPHICS_RENDER_TEXTURE_PC *textures[6]; // 0x028
    u32 unknown_040;                         // 0x040
    f32 parameters[4];                       // 0x044
    i32 dynamic_light_count;                 // 0x054
    NuDynamicLight *dynamic_lights[32];      // 0x058
    i32 deferred_geometry_count;             // 0x0d8
    void *geometry[128];                     // 0x0dc
    PC_FRAMEBUFFER_PC *shadow_fbos[6];       // 0x2dc
    PC_FRAMEBUFFER_PC *light_fbo;            // 0x2f4
};

#endif
