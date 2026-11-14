#ifndef SAGA_NU2API_NUCORE_NUANIM3_H
#define SAGA_NU2API_NUCORE_NUANIM3_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/cutscenes/gcutscn.h, nu2api/nu3d/nu3d_includes.h (not all of it earlier in
 * the shim). */

#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/cutscenes/gcutscn.h"
#include "nu2api/nu3d/nu3d_includes.h"
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
    extern ANI3_EVAL_CALLBACK_PC AnimBuffEvalCB;
    extern void **AnimBuffEvalData;
    extern i32 *AnimBuffEvalJoint;
    f32 NuAnimEndFrameOld(void *animation_data);
    extern ANI3_GLOBALBUFFER_PC *globalbuffer;
    f32 CalcValue1648(char *, i32, i32, f32, ANI3_SCALEMIN_PC *);
    void CalcValue1648Get2Values(char *, i32, i32, ANI3_SCALEMIN_PC *, f32 *, f32 *);
    void ANI_Ani3ExtractAllNodeCurves(ANI3_HEADER_PC *, f32, f32 *, i32, char *);
    f32 *NuAnimCurveExtractAllNodeCurves_3(ANI3_HEADER_PC *, i32, f32, char *);
    void NuAnimCurve2SetApplyToMatrix_3(ANI3_HEADER_PC *, i32, f32, NUMTX *);
    void NuAnimData2CalcMatrix(ANI3_HEADER_PC *, i32, f32, NUMTX *);
    i32 ANI_SimpleAni3PlayerV4Joint(ANI3_HEADER_PC *, f32, ANI3_BUFFER_PC *, i32, i32);
    i32 ANI_SimpleAni3PlayerV4Joint_Blend(ANI3_HEADER_PC *, f32, ANI3_BUFFER_PC *, f32, i32, i32, NUVEC *);
    void NuAnimRelocatePtrsANI3(ANI3_HEADER_PC *, i32);
    void ANI_FixUpAddrs(ANI3_HEADER_PC *, i32, i32);
    void *NuAnimData2FixPtrs(void *, i32, i32, i32);
    void *NuAnimData2Fixup(i32, void **);
    void *NuAnimData2LoadBuffFromPAK(void *, i32);
    void *NuAnimData2LoadBuffEx(char *, VARIPTR *, VARIPTR *, void **);
    void *NuAnimData2LoadBuff(char *, VARIPTR *, VARIPTR *);
    void NuAnimBuffEvaluateCallback(ANI3_EVAL_CALLBACK_PC callback, void **data, i32 *joints);
    void NuAnimBuffEvaluate_3_QuatB(NUMTX *, ANI3_BUFFER_PC *, NUHGOBJ_EVAL_PC *, NUMTX *, ANI3_HEADER_PC *,
                                    ANI3_ROOT_CALLBACK_PC, NUVEC *, void *);
    void NuAnimBuffEvaluate_3(ANI3_BUFFER_PC *, NUHGOBJ_EVAL_PC *, NUMTX *, ANI3_HEADER_PC *, ANI3_ROOT_CALLBACK_PC,
                              NUVEC *, void *);
    i32 NuAnimNumNodes(NUANIMDATA2_PC *);
    f32 NuAnimCurve2CalcValEx(DWA_CURVE_CELL_PC *, NUANIM_TIME2_PC *, u32);
    extern i32 MaxAnimJoints;
    void NuAnimInit(i32, VARIPTR *, VARIPTR);
    f32 NuAnimCurveCalcVal2(NUANIM_LEGACY_CURVE_PC *, NUANIM_TIME2_PC *);
    void NuAnimCurveSetApplyToMatrix(NUANIM_LEGACY_SET_PC *, NUANIM_TIME2_PC *, NUMTX *);
    void NuAnimCurveSetApplyBlendToJoint2(NUANIM_LEGACY_SET_PC *, NUANIM_TIME2_PC *, NUANIM_LEGACY_SET_PC *,
                                          NUANIM_TIME2_PC *, f32, NUHGOBJ_EVAL_JOINT_PC *, NUVEC *, NUVEC *, NUMTX *,
                                          NUHGOBJ_JOINT_OVERRIDE_PC *);
#ifdef __cplusplus
}
#endif

#endif
