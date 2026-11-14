#ifndef SAGA_NU2API_NUCORE_NUHGOBJ_H
#define SAGA_NU2API_NUCORE_NUHGOBJ_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/cutscenes/gcutscn.h, nu2api/nu3d/nu3d_includes.h (not all of it earlier in
 * the shim). */

#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/cutscenes/gcutscn.h"
#include "nu2api/nu3d/nu3d_includes.h"
#endif

typedef void (*NUJOINTPROCANIMFN_PC)(ANI3_BUFFER_PC *, NUHGOBJ_EVAL_PC *, i32, NUHGOBJ_JOINT_OVERRIDE_PC *);
#ifdef __cplusplus
extern "C" {
#endif
    i32 NuHGobjRndrRandShadowSurfacePoints(NUHGOBJ_EVAL_PC *, NUMTX *, NUMTX *, i32, NUVEC *, i32);
    i32 NuHGobjRndrMtxDwa(NUHGOBJ_EVAL_PC *, NUMTX *, i32, i16 *, NUMTX *, DWA_WEIGHT_BLOCK_PC **, i32);
    void NuHGobjRestrictEvaluation(NUHGOBJ_EVAL_PC *);
    void NuHGobjRestoreEvaluation(void);
    i32 NuHGobjReversibleCharacters(i32 enabled);
    i32 NuHGobjForceShadowsOnCharacters(i32 enabled);
    NUHGOBJ_POI_PC *NuHGobjGetPOI(NUHGOBJ_EVAL_PC *object, u8 index);
    i32 NuHGobjGetLayerIndex(const char *, NUHGOBJ_EVAL_PC *);
    extern NUJOINTPROCANIMFN_PC JointProcAnimFn;
    void NuAnimBuffProceduralAnimation(ANI3_BUFFER_PC *, NUHGOBJ_EVAL_PC *, i32, NUHGOBJ_JOINT_OVERRIDE_PC *);
    void NuHGobjPOIMtx(NUHGOBJ_EVAL_PC *, u8, NUMTX *, NUMTX *, NUMTX *);
    void NuHGobjEval(NUHGOBJ_EVAL_PC *, i32, NUHGOBJ_JOINT_OVERRIDE_PC *, NUMTX *);
    void NuHGobjEvalAnimBlend2Root(NUHGOBJ_EVAL_PC *, ANI3_HEADER_PC *, f32, ANI3_HEADER_PC *, f32, f32, i32, void *,
                                   NUMTX *, ANI3_ROOT_CALLBACK_PC, void *);
    DWA_WEIGHT_BLOCK_PC **NuHGobjEvalDwa2(i32, i16 *, NUANIMDATA2_PC *, f32);
    DWA_WEIGHT_BLOCK_PC **NuHGobjEvalDwaBlend2(i32, i16 *, NUANIMDATA2_PC *, f32, NUANIMDATA2_PC *, f32, f32);
    void NuHGobjEvalAnim2Root_3(NUHGOBJ_EVAL_PC *, ANI3_HEADER_PC *, f32, i32, void *, NUMTX *, ANI3_ROOT_CALLBACK_PC,
                                void *);
    void NuHGobjEvalAnimBlend2Root_3(NUHGOBJ_EVAL_PC *, ANI3_HEADER_PC *, f32, ANI3_HEADER_PC *, f32, f32, i32, void *,
                                     NUMTX *, ANI3_ROOT_CALLBACK_PC, void *);
    void NuHGobjEvalAnimBlend2(NUHGOBJ_EVAL_PC *, ANI3_HEADER_PC *, f32, ANI3_HEADER_PC *, f32, f32, i32, void *,
                               NUMTX *);
    void NuHGobjEvalAnim2Root(NUHGOBJ_EVAL_PC *, ANI3_HEADER_PC *, f32, i32, NUHGOBJ_JOINT_OVERRIDE_PC *, NUMTX *,
                              ANI3_ROOT_CALLBACK_PC, void *);
    void NuHGobjEvalAnim2(NUHGOBJ_EVAL_PC *, ANI3_HEADER_PC *, f32, i32, NUHGOBJ_JOINT_OVERRIDE_PC *, NUMTX *);
    DWA_WEIGHT_BLOCK_PC **NuHGobjEvalDwaBlend(i32, i16 *, NUANIM_LEGACY_HEADER_PC *, f32, NUANIM_LEGACY_HEADER_PC *,
                                              f32, f32);
    void NuHGobjEvalAnimBlend(NUHGOBJ_EVAL_PC *, NUANIM_LEGACY_HEADER_PC *, f32, NUANIM_LEGACY_HEADER_PC *, f32, f32,
                              i32, NUHGOBJ_JOINT_OVERRIDE_PC *, NUMTX *);
    NUHGOBJ_EVAL_PC *NuGHGRead(char *, VARIPTR *, VARIPTR);
#ifdef __cplusplus
}
#endif

#endif
