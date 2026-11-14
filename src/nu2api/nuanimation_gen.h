#ifndef SAGA_NU2API_NUANIMATION_GEN_H
#define SAGA_NU2API_NUANIMATION_GEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuSpecialForceMaterials(i32, NUMTL **);
    void ShadowMapEnablePc(struct PC_SPECIAL_SHADOW_LIGHT *);
    void NuSpecialClearShadowLights(void);
    i32 NuSpecialCompare(NUSPECIALHANDLE *left, NUSPECIALHANDLE *right);
    void NuSpecialClear(NUSPECIALHANDLE *special);
    void NuSpecialGetBounds(NUSPECIALHANDLE *, NUVEC *, NUVEC *);
    i32 NuSpecialFindDisplay(NUGSCN *, NUSPECIALHANDLE *, char *, i32);
    void NuSpecialGetRadius(NUSPECIALHANDLE *special, NUVEC *centre, f32 *radius);
    NUMTX *NuSpecialGetMtx(NUSPECIALHANDLE *special);
    void NuSpecialSetMtx(NUSPECIALHANDLE *special, const NUMTX *matrix);
    i32 NuSpecialExistsFn(NUSPECIALHANDLE *special);
    i32 NuSpecialTestAnim(NUSPECIALHANDLE *special);
    NUVEC *NuSpecialGetPos(NUSPECIALHANDLE *special);
    NUVEC *NuSpecialGetDrawPos(NUSPECIALHANDLE *special);
    char *NuSpecialGetName(NUSPECIALHANDLE *special);
    NUMTX *NuSpecialGetDrawMtx(NUSPECIALHANDLE *special);
    NUMTX *NuSpecialGetInstanceMtx(NUSPECIALHANDLE *special);
    void *NuSpecialGetInstAnim(NUSPECIALHANDLE *special);
    void NuSpecialSetCollision(NUSPECIALHANDLE *special, i32 enabled);
    i32 NuSpecialGetCollision(NUSPECIALHANDLE *special);
    void NuSpecialSetNoVisiTest(NUSPECIALHANDLE *special, i32 enabled);
    i32 NuSpecialGetNoVisiTestFn(NUSPECIALHANDLE *special);
    void NuSpecialConstAlpha(i32 enabled, f32 alpha);
    void NuSpecialConstTint(i32 enabled, const NUVEC *tint);
    void NuSpecialGetNext(NUSPECIALHANDLE *special);
    i32 NuSpecialGetFirst(NUGSCN *scene, NUSPECIALHANDLE *special, i32 flags);
    i32 NuSpecialGetNumSpecials(NUGSCN *scene);
    i32 NuSpecialGetOnScreenFn(NUSPECIALHANDLE *special);
    i32 NuSpecialGetVisibilityFn(NUSPECIALHANDLE *special);
    i32 NuSpecialGetInstanceix(NUSPECIALHANDLE *);
    void PCBuildLegacySkeletalJoint(DWA_CURVE_CELL_PC *, i8 *, u8, NUANIM_TIME2_PC *, NUHGOBJ_EVAL_JOINT_PC *, NUVEC *,
                                    NUVEC *, NUMTX *, NUHGOBJ_JOINT_OVERRIDE_PC *, NUVEC *, NUVEC *);
    void NuSpecialSetVisibility(NUSPECIALHANDLE *, i32);
    f32 NuSpecialGetOriginRadius(NUSPECIALHANDLE *special);
    void NuSpecialList(struct NUGSCN_s *scene);

    i32 NuSpecialFindMulti(NUGSCN *, NUSPECIALHANDLE *, char *, i32, i32);
    i32 NuSpecialSetClipping(i32, i32);
    void NuSpecialClearShadowClipTestResults(void);
#ifdef __cplusplus
}
#endif

#endif
