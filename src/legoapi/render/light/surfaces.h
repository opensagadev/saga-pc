#ifndef SAGA_LEGOAPI_RENDER_LIGHT_SURFACES_H
#define SAGA_LEGOAPI_RENDER_LIGHT_SURFACES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/legoapi_types.h (not all of it earlier in
 * the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
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

#endif
