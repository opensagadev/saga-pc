#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuSpecialForceMaterials(i32, NUMTL **);
    void ShadowMapEnable(struct PC_SPECIAL_SHADOW_LIGHT *);
    void NuSpecialClearShadowLights(void);
    i32 NuSpecialCompare(nuhspecial_s *left, nuhspecial_s *right);
    void NuSpecialClear(nuhspecial_s *special);
    void NuSpecialGetBounds(nuhspecial_s *, NUVEC *, NUVEC *);
    i32 NuSpecialFindDisplay(NUGSCN *, nuhspecial_s *, char *, i32);
    void NuSpecialGetRadius(nuhspecial_s *special, NUVEC *centre, f32 *radius);
    NUMTX *NuSpecialGetMtx(nuhspecial_s *special);
    void NuSpecialSetMtx(nuhspecial_s *special, const NUMTX *matrix);
    i32 NuSpecialExistsFn(nuhspecial_s *special);
    i32 NuSpecialTestAnim(nuhspecial_s *special);
    NUVEC *NuSpecialGetPos(nuhspecial_s *special);
    NUVEC *NuSpecialGetDrawPos(nuhspecial_s *special);
    char *NuSpecialGetName(nuhspecial_s *special);
    NUMTX *NuSpecialGetDrawMtx(nuhspecial_s *special);
    NUMTX *NuSpecialGetInstanceMtx(nuhspecial_s *special);
    void *NuSpecialGetInstAnim(nuhspecial_s *special);
    void NuSpecialSetCollision(nuhspecial_s *special, i32 enabled);
    i32 NuSpecialGetCollision(nuhspecial_s *special);
    void NuSpecialSetNoVisiTest(nuhspecial_s *special, i32 enabled);
    i32 NuSpecialGetNoVisiTestFn(nuhspecial_s *special);
    void NuSpecialConstAlpha(i32 enabled, f32 alpha);
    void NuSpecialConstTint(i32 enabled, const NUVEC *tint);
    void NuSpecialGetNext(nuhspecial_s *special);
    i32 NuSpecialGetFirst(NUGSCN *scene, nuhspecial_s *special, i32 flags);
    i32 NuSpecialGetNumSpecials(NUGSCN *scene);
    i32 NuSpecialGetOnScreenFn(nuhspecial_s *special);
    i32 NuSpecialGetVisibilityFn(nuhspecial_s *special);
    i32 NuSpecialGetInstanceix(nuhspecial_s *);
    void PCBuildLegacySkeletalJoint(DWA_CURVE_CELL *, i8 *, u8, NUANIM_TIME2 *, NUHGOBJ_EVAL_JOINT *, NUVEC *, NUVEC *, NUMTX *,
                                    NUHGOBJ_JOINT_OVERRIDE *, NUVEC *, NUVEC *);
    void NuSpecialSetVisibility(nuhspecial_s *, i32);
    f32 NuSpecialGetOriginRadius(nuhspecial_s *special);
    void NuSpecialList(struct NUGSCN_s *scene);

    i32 NuSpecialFindMulti(NUGSCN *, nuhspecial_s *, char *, i32, i32);
    i32 NuSpecialSetClipping(i32, i32);
    void NuSpecialClearShadowClipTestResults(void);
    NUMTL *NuSpecialGetMtl(nuhspecial_s *special, i32 index);
    i32 NuSpecialNumMtls(nuhspecial_s *special);
#ifdef __cplusplus
}
#endif
