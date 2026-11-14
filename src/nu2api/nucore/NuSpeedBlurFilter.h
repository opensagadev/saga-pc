#ifndef SAGA_NU2API_NUCORE_NUSPEEDBLURFILTER_H
#define SAGA_NU2API_NUCORE_NUSPEEDBLURFILTER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuSpeedBlurFilterGen.h"
#include "nu2api/nustencilshadows_PC.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuDisplayListResetStateCache(NURNDRLIBSTATE *state);
    i32 PcInitializeDynamicGeometryBuffersPc(struct PC_DYNAMIC_GEOMETRY_PC *, i32);
    i32 PcUpdateGeometryDistancesPc(struct PC_GEOMETRY_OUTPUT_PC *, struct PC_GEOMETRY_SORT_ITEM_PC *, i32);
    u8 PcTransformDynamicGeometryPc(struct PC_GEOMETRY_SKIN_PC *, struct PC_GEOMETRY_MESH_PC *, u32 *);
    i32 PcGeometryBackendAvailablePc(void);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

class NuSpeedBlurFilter : public NuSpeedBlurFilterGen {
  public:
    void ResizeTexturesPc(i32 width, i32 height);
    virtual void initResources();
    virtual void initTextureResources(i32, i32);
    virtual void destroyTextureResources();
    virtual void render();
    PC_EFFECT_SURFACE_PC texture_surface;
    i32 platform_uniform;
};

STATIC_ASSERT(sizeof(NuSpeedBlurFilter) == 0x40);
STATIC_ASSERT(offsetof(NuSpeedBlurFilter, texture_surface) == 0x20);
STATIC_ASSERT(offsetof(NuSpeedBlurFilter, platform_uniform) == 0x3c);
#endif

#endif
