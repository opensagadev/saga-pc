/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    f32 NuAnimEndFrame(void *);
    void buildBitCountTable(void);
    void *NuAnimCurveSetCreate(i32);
    void *NuAnimDataRead(NUFILE);
    extern const char AIScriptKeywordInclude[8];
    extern const char AIScriptKeywordDeriveFromScript[17];
    extern const char AIScriptKeywordActionMacro[12];
    extern const char AIScriptKeywordConditionMacro[15];
    extern ANI3_EVAL_CALLBACK AnimBuffEvalCB;
    extern void **AnimBuffEvalData;
    extern i32 *AnimBuffEvalJoint;
    f32 NuAnimEndFrameOld(void *animation_data);
    extern ANI3_GLOBALBUFFER *globalbuffer;
    f32 CalcValue1648(char *, i32, i32, f32, ANI3_SCALEMIN *);
    void CalcValue1648Get2Values(char *, i32, i32, ANI3_SCALEMIN *, f32 *, f32 *);
    void ANI_Ani3ExtractAllNodeCurves(ANI3_HEADER *, f32, f32 *, i32, char *);
    f32 *NuAnimCurveExtractAllNodeCurves_3(ANI3_HEADER *, i32, f32, char *);
    void NuAnimCurve2SetApplyToMatrix_3(ANI3_HEADER *, i32, f32, NUMTX *);
    void NuAnimData2CalcMatrix(ANI3_HEADER *, i32, f32, NUMTX *);
    i32 ANI_SimpleAni3PlayerV4Joint(ANI3_HEADER *, f32, ANI3_BUFFER *, i32, i32);
    i32 ANI_SimpleAni3PlayerV4Joint_Blend(ANI3_HEADER *, f32, ANI3_BUFFER *, f32, i32, i32, NUVEC *);
    void NuAnimRelocatePtrsANI3(ANI3_HEADER *, i32);
    void ANI_FixUpAddrs(ANI3_HEADER *, i32, i32);
    void *NuAnimData2FixPtrs(void *, i32, i32, i32);
    void *NuAnimData2Fixup(i32, void **);
    void *NuAnimData2LoadBuffFromPAK(void *, i32);
    void *NuAnimData2LoadBuffEx(char *, VARIPTR *, VARIPTR *, void **);
    void *NuAnimData2LoadBuff(char *, VARIPTR *, VARIPTR *);
    void NuAnimBuffEvaluateCallback(ANI3_EVAL_CALLBACK callback, void **data, i32 *joints);
    void NuAnimBuffEvaluate_3_QuatB(NUMTX *, ANI3_BUFFER *, NUHGOBJ_EVAL *, NUMTX *, ANI3_HEADER *, ANI3_ROOT_CALLBACK, NUVEC *, void *);
    void NuAnimBuffEvaluate_3(ANI3_BUFFER *, NUHGOBJ_EVAL *, NUMTX *, ANI3_HEADER *, ANI3_ROOT_CALLBACK, NUVEC *, void *);
    i32 NuAnimNumNodes(NUANIMDATA2 *);
    f32 NuAnimCurve2CalcValEx(DWA_CURVE_CELL *, NUANIM_TIME2 *, u32);
    extern i32 MaxAnimJoints;
    void NuAnimInit(i32, VARIPTR *, VARIPTR);
    f32 NuAnimCurveCalcVal2(NUANIM_LEGACY_CURVE *, NUANIM_TIME2 *);
    void NuAnimCurveSetApplyToMatrix(NUANIM_LEGACY_SET *, NUANIM_TIME2 *, NUMTX *);
    void NuAnimCurveSetApplyBlendToJoint2(NUANIM_LEGACY_SET *, NUANIM_TIME2 *, NUANIM_LEGACY_SET *, NUANIM_TIME2 *, f32,
                                          NUHGOBJ_EVAL_JOINT *, NUVEC *, NUVEC *, NUMTX *, NUHGOBJ_JOINT_OVERRIDE *);
#ifdef __cplusplus
}
#endif
