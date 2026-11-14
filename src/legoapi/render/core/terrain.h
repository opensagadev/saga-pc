#ifndef SAGA_LEGOAPI_RENDER_CORE_TERRAIN_H
#define SAGA_LEGOAPI_RENDER_CORE_TERRAIN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void noterraininit(void);
    void TerrSetPlatScanDist(f32 distance);
    void TerrainSetWallDeflectYScale(f32 scale);
    void TerrainPlatformOldUpdate(void);
    void TerrainPlatformNewUpdate(void);
    i32 NewRayCastScaleYMask(NUVEC *, NUVEC *, f32, f32, i32, u32);
    void RayImpact(NUVEC *);
    i32 TerrainPlatId(void);
    f32 NewRayCastGetTOFI(void);
    i32 NewRayCastGetImpactTerrainType(void);

    void NewScan(NUVEC *position, i32 terrain_mask, i32 scan_platforms);
    void ScanTerrainHandel(i32 terrain_mask, i16 *handle);
    i32 HitTerrPoly(TERRAIN_POLYGON_INFO_PC *surface, i32 group_index);
    void TerrainSetImpactData(void *records, i32 *count, i32 max_records);
    void TerrainSetPlatConnectTol(f32 tolerance);
    void DerotateMovementVector(void);
    void RotateVec(NUVEC *, NUVEC *);
    void DeRotateTerrain(TERRAIN_POLYGON_INFO_PC *);
    void CubeImpact(NUMTX *, NUMTX *, NUVEC *, f32, NUVEC *);
    void DeRotatePoint(NUVEC *);
    i32 CheckCylinder(i32, i32, i32 *, i32);
    i32 CheckSphere(i32);
    i32 CheckSphereTer(NUVEC *, f32);
    i32 HitPoly(f32, f32, f32, f32, TERRAIN_POLYGON_INFO_PC *);
    void NewTerrainScaleYMask(NUVEC *, NUVEC *, u8 *, i32, f32, f32, f32, i32, i32, i32);
    i32 HitTerrain(void);
    TERRAIN_TRACK_SLOT_PC *AllocTerrId(void);
    void FullDeflect(NUVEC *, NUVEC *, NUVEC *);
    void FullDeflectSmallY(NUVEC *, NUVEC *, NUVEC *);
    void PlatformConnect(char *, NUVEC *, NUVEC *, i32);
    void NewTerrStoreAnyInfo(void);
    void TerrainPlatGetMtx(i32, NUMTX **, NUMTX **);
    extern i32 IgnoreWallSplines;
    void *TerrainInitEx(i32 level_num, void *buf, void *buf_end, i32 options, char *path, void *gscn, i32 scene_id,
                        u32 max_group_indices, u32 max_groups, u32 max_platforms);
    f32 NewShadowEx(NUVEC *position, f32 unused, f32 height_above, f32 roof_range, i32 mask);
    void PlatOnOff(i32 platform_index, i32 enabled);
    void NewTerrPlatformsOff(void);
    i32 ShadowInfo(void);
    i32 EShadowInfo(void);
    f32 NewShadow(NUVEC *, f32, f32, i32);
    void NewRayCastGetImpactNormal(NUVEC *);
    extern i32 SkinFlipTab[8];
    void *TerrainGetCur(void);
    void TerrainSetCur(void *);
    i32 NewPlatInst(void *, i32);
    i32 DeletePlatinst(i32);
    void PlatInstRotate(i32, i32);
    i32 FindPlatInst(i32);
    void FullReflect(NUVEC *normal, NUVEC *movement, NUVEC *result);

    f32 NewRayCastGetEmbedDist(void);
    i32 NewRayCastHitWallSpline(void);
    i32 NewRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_flags);
    i16 *NewScanHandel(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_type, i16 *subset);

    i32 NewRayCastMask(NUVEC *, NUVEC *, f32, i32, i32);
    i32 NewRayCastSet(NUVEC *, NUVEC *, f32, f32, f32, i32, i32);
    i32 NewRayCastSetMask(NUVEC *, NUVEC *, f32, f32, f32, i32, i32, i32);

    i32 NewRayCastSetHandel(NUVEC *, NUVEC *, f32, f32, f32, i16 *, i32);
    i32 NewRayCastPlatForm(NUVEC *, NUVEC *, f32, f32, i32, i32);
    void DrawWallSpline(f32);
    i32 NewPlatPickupInst(void *, i32);
#ifdef __cplusplus
}
#endif

#endif
