#ifndef SAGA_NU2API_NUCORE_NUDEFERREDFILTERGEN_H
#define SAGA_NU2API_NUCORE_NUDEFERREDFILTERGEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuDynamicLight.h"
#include "nu2api/nucore/NuPostFilter.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuDynamicLight.h"
#include "nu2api/nucore/NuPostFilter.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuDynamicLight.h"
#include "nu2api/nucore/NuPostFilter.h"
#endif

#ifdef __cplusplus

class NuDeferredFilterGen : public NuPostFilter {
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
    i32 sample_count;
    void *programs[6];
    GRAPHICS_RENDER_TEXTURE_PC *textures[6];
    u32 unknown_040;
    f32 parameters[4];
    i32 dynamic_light_count;
    NuDynamicLight *dynamic_lights[32];
    i32 deferred_geometry_count;
    void *geometry[128];
    PC_FRAMEBUFFER_PC *shadow_fbos[6];
    PC_FRAMEBUFFER_PC *light_fbo;
};

STATIC_ASSERT(sizeof(NuDeferredFilterGen) == 0x2f8);
STATIC_ASSERT(offsetof(NuDeferredFilterGen, shadow_fbos) == 0x2dc);
STATIC_ASSERT(offsetof(NuDeferredFilterGen, light_fbo) == 0x2f4);
STATIC_ASSERT(offsetof(NuDeferredFilterGen, parameters) == 0x44);
STATIC_ASSERT(offsetof(NuDeferredFilterGen, dynamic_light_count) == 0x54);
STATIC_ASSERT(offsetof(NuDeferredFilterGen, dynamic_lights) == 0x58);
STATIC_ASSERT(offsetof(NuDeferredFilterGen, deferred_geometry_count) == 0xd8);
#endif

#endif
