#ifndef SAGA_LEGOAPI_MISC_UTILITIES_H
#define SAGA_LEGOAPI_MISC_UTILITIES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/characters/motion/move.h, legoapi/items/base/apiobject.h (not all of it
 * earlier in the shim). */

#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void GetRotationAngles(NUVEC *, u16 *, u16 *);
    extern i32 dco_id;
    extern f32 dco_reflectcoord;
    extern PLAYERCHARACTERCONFIG_PC *dco_gcdata;
    extern CHARACTER_MODEL_LAYERS_PC *dco_cmodel;
    extern i32 dco_wearinghat;
    extern i32 dco_reflectaxis;
    extern u16 dco_prerotatez;
    i32 MatrixReflection(NUMTX *, i32, f32, f32, NUMTX *);

    void IncreaseScore(u32 *score, u64 amount, i32 apply_multiplier);
    f32 GameShadow(GAMEOBJECT *object, NUVEC *position, f32 probe, i32 mask);
    f32 FindReflectionNoPlatforms(NUVEC *position);
    void FindAnglesZX(NUVEC *, u16 *, u16 *);
    extern i32 i_temp_xrot;
    i32 LineIntersectSphere(NUVEC *, NUVEC *, NUVEC *, f32, f32 *);
    i32 LineIntersectCircle(NUVEC *, NUVEC *, NUVEC *, f32);
    i32 SphereSphereOverlapScaleY(NUVEC *, f32, f32, NUVEC *, f32, f32);
    void FindAnglesXY(NUVEC *, u16 *, u16 *);
    i32 SphereSphereOverlap(NUVEC *, f32, NUVEC *, f32);
    i32 LineIntersectXY(NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUVEC *);
    void ResetSeeds(void);
    void RotateGameMatrix(NUMTX *matrix, i32 order, u16 x, u16 y, u16 z);
    i32 RotDiff(u16 current, u16 target);
    void MakeThrowVector(NUVEC *, NUVEC *, NUVEC *, NUVEC *, f32, f32);

    extern i32 qseed;
    void CalculateInterceptVector(NUVEC *, NUVEC *, NUVEC *, f32, NUVEC *, NUVEC *);
    void VecRotateAxis(NUVEC *, u16, NUVEC *);

    i32 OnOrInsidePlane(NUVEC *, NUVEC *, NUVEC *, NUVEC *, f32, f32 *);
    void SetAnimFrame(struct nuhspecial_s *special, f32 frame);
    i32 DrawPanel3DObjectMtxNoAlpha(nuhspecial_s *, NUMTX *);
    f32 XZLinesClosest(struct nuvec_s *first_start, struct nuvec_s *first_end, struct nuvec_s *second_start,
                       struct nuvec_s *second_end, f32 *first_fraction, f32 *second_fraction);
#ifdef __cplusplus
}
#endif

#endif
