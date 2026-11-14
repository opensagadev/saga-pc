#ifndef SAGA_NU2API_NU3D_NURNDR_H
#define SAGA_NU2API_NU3D_NURNDR_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#include "nu2api/nustencilshadows_PC.h"
#endif

struct DWA_WEIGHT_BLOCK_PC;
struct NUVEC4;
struct numtl_s;
struct numtx_s;

typedef struct nurnd_vertex3d_pc_s {
    NUVEC position;
    NUVEC normal;
    u32 colour;
    f32 u;
    f32 v;
} NURND_VERTEX3D_PC;

STATIC_ASSERT(sizeof(NURND_VERTEX3D_PC) == 0x24);
STATIC_ASSERT(offsetof(NURND_VERTEX3D_PC, position) == 0);
STATIC_ASSERT(offsetof(NURND_VERTEX3D_PC, colour) == 0x18);
STATIC_ASSERT(offsetof(NURND_VERTEX3D_PC, u) == 0x1c);
STATIC_ASSERT(offsetof(NURND_VERTEX3D_PC, v) == 0x20);
#ifdef __cplusplus
extern "C" {
#endif
    void WindShear(struct numtx_s *, struct numtx_s *, i32, i32);
    extern f32 global_windscale;
    extern f32 global_windspeed;
    void NuRndrShadowDirCol(const NUVEC *direction, u32 colour, f32 near_distance, f32 far_distance);
    void NuRndrStartReflectionRender(i32);
    void NuRndrEndReflectionRender(void);
    void NuRndrCalcRandEllipsePos(struct NUVEC4 *, struct numtx_s *, NUVEC *);
    void NuRndrCalcRandCylinderPos(struct NUVEC4 *, struct numtx_s *, NUVEC *);
    extern i32 NuRndrShadowCnt;
    extern PC_SHADOW_QUEUE_RECORD NuRndrShadPolDat[128];
    STATIC_ASSERT(sizeof(NURND_VERTEX3D_PC[4]) == 0x90);
    STATIC_ASSERT(offsetof(NURND_VERTEX3D_PC, normal) == 0xc);
    void NuRndrShadPolys(struct numtl_s *);
    i32 NuRndrSetDirectionalLightsPS(const NUVEC *, const NUVEC *, const NUVEC *, const NUVEC *, const NUVEC *,
                                     const NUVEC *);
    void NuRndrAddShadow(NUVEC *, f32, i32, i32, i32, i32);
    void NuRndrAxisArrowsMtx(struct numtx_s *, f32, struct numtl_s *);
    i32 NuRndrSetAmbientLightPS(const NUVEC *colour);
    void NuRndrEndScene(void);
    i32 NuRndrBeginScene(i32 begin_flags);
    void NuRndrFx(i32 paused, void *context);
    void NuRndrClear(i32 clear_flags, i32 bg_colour, f32 alpha);
    void NuRndrGradClear(i32 clear_flags, i32 top_colour, i32 bottom_colour, f32 alpha);
    extern i32 NuRndrStopUpdate;
    extern NUVEC NuRndrDebBase;
    extern NUVEC NuRndrDebRange;
    extern NUVEC NuRndrDebRangeInv;
    void NuRndrSetDebBox(NUVEC *);
    /* Current shared TYPE100/INSTANCE114/TERRAIN_SET views suffice; no new layout slots. */
    i32 NuRndrLine3d(NURND_VERTEX3D_PC *, struct numtl_s *, struct numtx_s *);
    i32 NuRndrStrip3d(NURND_VERTEX3D_PC *, struct numtl_s *, struct numtx_s *, i32);
    i32 NuRndrTriStrip3dClip(NURND_VERTEX3D_PC *, i32, struct numtx_s *, struct numtl_s *);
    void NuRndrLine3dDbg(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, i32 colour);
    extern i32 PS2_REZ_W;
    extern i32 PS2_REZ_H;
    extern i32 rndr_blend_shape_deformer_wt_cnt;
    extern i32 rndr_blend_shape_deformer_wt_ptrs_cnt;
    struct DWA_WEIGHT_BLOCK_PC *NuRndrCreateBlendShapeDeformerWeightsArray(i32);
    struct DWA_WEIGHT_BLOCK_PC **NuRndrCreateBlendShapeDWAPointers(i32);
    i32 NuRndrGobj(void *object, struct numtx_s *matrix);
    i32 NuRndrSetSpecularLightPS(const NUVEC *, const struct NUVEC4 *);
    i32 NuRndrSetFxMtx(struct numtx_s *);
    void NuRndrStateSetSpecularLightEx(const NUVEC *, const struct numtx_s *, const NUVEC *);
    i32 NuRndrStateUpdateCameraState(void);
    void NuRndrStreamInit(i32 size, VARIPTR *buffer);
    void NuRndrSwapStreamBuffers(void);
    /* Private helper remains in its genuine caller TU, with local scene ingestion views. */

    void NuRndrInitEx(i32 stream_size, VARIPTR *buffer, VARIPTR end);
    void NuRndrInitGeneric(void);
    i32 NuRndrSetAmbientLightSpecular(const struct NUVEC4 *);

    i32 NuRndrSwapScreen(i32 mode);
    i32 NuRndrSwapScreenEx(i32 mode, void (*callback)(void));
    void NuRndrEndSceneEx(i32 flags);
    void NuRndrRectUV2di(i32 x, i32 y, i32 width, i32 height, f32 u0, f32 v0, f32 u1, f32 v1, i32 colour,
                         struct numtl_s *material);
    void NuRndrGradRectUV2di(i32 x, i32 y, i32 width, i32 height, f32 u0, f32 v0, f32 u1, f32 v1, u32 *colours,
                             void *material);
#ifdef __cplusplus
}
#endif

#endif
