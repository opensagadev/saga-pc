#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuSpeedBlurFilterGen.h"

struct PC_DYNAMIC_GEOMETRY;
struct PC_GEOMETRY_MESH;
struct PC_GEOMETRY_OUTPUT;
struct PC_GEOMETRY_SKIN;
struct PC_GEOMETRY_SORT_ITEM;

#ifdef __cplusplus
extern "C" {
#endif
    void NuDisplayListResetStateCache(NURNDRLIBSTATE *state);
    i32 PcInitializeDynamicGeometryBuffers(struct PC_DYNAMIC_GEOMETRY *, i32);
    i32 PcUpdateGeometryDistances(struct PC_GEOMETRY_OUTPUT *, struct PC_GEOMETRY_SORT_ITEM *, i32);
    u8 PcTransformDynamicGeometry(struct PC_GEOMETRY_SKIN *, struct PC_GEOMETRY_MESH *, u32 *);
    i32 PcGeometryBackendAvailable(void);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

class NuSpeedBlurFilter : public NuSpeedBlurFilterGen { // size 0x40
  public:
    void ResizeTextures(i32 width, i32 height);
    virtual void initResources();
    virtual void initTextureResources(i32, i32);
    virtual void destroyTextureResources();
    virtual void render();
    PC_EFFECT_SURFACE texture_surface; // 0x20
    i32 platform_uniform;              // 0x3c
};

#endif
