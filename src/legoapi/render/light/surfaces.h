/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void Surfaces_Reset(void);
    extern u16 SURFACEBITS_NODUST;
    i32 UnderWater(GAMEOBJECT *object);
    i32 IntersectWater(GAMEOBJECT *object);
    void Surface_Deflect(NUVEC *normal, NUVEC *movement, NUVEC *result, i32 mode);
    void InitSurfaceInfo(GAMEOBJECT *object);
    i32 SetObjOnSurface(GAMEOBJECT *object, i32 mode);
    void GetSurfaceInfo(GAMEOBJECT *object, i32 update_fields, f32 floor_height);
    extern void (*SurfaceInfo_ExtraReflectFn)(GAMEOBJECT *object);
    extern u16 SURFACEBITS_DUST;
    extern TERRAIN_SURFACE_PC TerSurface[32];
#ifdef __cplusplus
}
#endif
