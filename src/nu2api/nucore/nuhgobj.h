#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/nuvec.h"

struct NUANIMDATA2;
struct numtx_s;

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuHGobjRndrRandShadowSurfacePoints(NUHGOBJ_EVAL *, struct numtx_s *, struct numtx_s *, i32, NUVEC *, i32);
    i32 NuHGobjRndrMtxDwa(NUHGOBJ_EVAL *, struct numtx_s *, i32, i16 *, struct numtx_s *, DWA_WEIGHT_BLOCK **, i32);
    void NuHGobjRestrictEvaluation(NUHGOBJ_EVAL *);
    void NuHGobjRestoreEvaluation(void);
    i32 NuHGobjReversibleCharacters(i32 enabled);
    i32 NuHGobjForceShadowsOnCharacters(i32 enabled);
    NUHGOBJ_POI *NuHGobjGetPOI(NUHGOBJ_EVAL *object, u8 index);
    i32 NuHGobjGetLayerIndex(const char *, NUHGOBJ_EVAL *);
    extern NUJOINTPROCANIMFN JointProcAnimFn;
    void NuAnimBuffProceduralAnimation(ANI3_BUFFER *, NUHGOBJ_EVAL *, i32, NUHGOBJ_JOINT_OVERRIDE *);
    void NuHGobjPOIMtx(NUHGOBJ_EVAL *, u8, struct numtx_s *, struct numtx_s *, struct numtx_s *);
    void NuHGobjEval(NUHGOBJ_EVAL *, i32, NUHGOBJ_JOINT_OVERRIDE *, struct numtx_s *);
    void NuHGobjEvalAnimBlend2Root(NUHGOBJ_EVAL *, ANI3_HEADER *, f32, ANI3_HEADER *, f32, f32, i32, void *, struct numtx_s *,
                                   ANI3_ROOT_CALLBACK, void *);
    DWA_WEIGHT_BLOCK **NuHGobjEvalDwa2(i32, i16 *, struct NUANIMDATA2 *, f32);
    DWA_WEIGHT_BLOCK **NuHGobjEvalDwaBlend2(i32, i16 *, struct NUANIMDATA2 *, f32, struct NUANIMDATA2 *, f32, f32);
    void NuHGobjEvalAnim2Root_3(NUHGOBJ_EVAL *, ANI3_HEADER *, f32, i32, void *, struct numtx_s *, ANI3_ROOT_CALLBACK, void *);
    void NuHGobjEvalAnimBlend2Root_3(NUHGOBJ_EVAL *, ANI3_HEADER *, f32, ANI3_HEADER *, f32, f32, i32, void *, struct numtx_s *,
                                     ANI3_ROOT_CALLBACK, void *);
    void NuHGobjEvalAnimBlend2(NUHGOBJ_EVAL *, ANI3_HEADER *, f32, ANI3_HEADER *, f32, f32, i32, void *, struct numtx_s *);
    void NuHGobjEvalAnim2Root(NUHGOBJ_EVAL *, ANI3_HEADER *, f32, i32, NUHGOBJ_JOINT_OVERRIDE *, struct numtx_s *, ANI3_ROOT_CALLBACK,
                              void *);
    void NuHGobjEvalAnim2(NUHGOBJ_EVAL *, ANI3_HEADER *, f32, i32, NUHGOBJ_JOINT_OVERRIDE *, struct numtx_s *);
    DWA_WEIGHT_BLOCK **NuHGobjEvalDwaBlend(i32, i16 *, NUANIM_LEGACY_HEADER *, f32, NUANIM_LEGACY_HEADER *, f32, f32);
    void NuHGobjEvalAnimBlend(NUHGOBJ_EVAL *, NUANIM_LEGACY_HEADER *, f32, NUANIM_LEGACY_HEADER *, f32, f32, i32, NUHGOBJ_JOINT_OVERRIDE *,
                              struct numtx_s *);
    NUHGOBJ_EVAL *NuGHGRead(char *, VARIPTR *, VARIPTR);
    extern void NuHGobjDestroy(NUHGOBJ_EVAL *object);
#ifdef __cplusplus
}
#endif
