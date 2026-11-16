#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

/* NuAnimCurve2CalcValEx curve formats, by the key block each one decodes (0: the cell's constant). */
#define NUANIMCURVE2_CONSTANT 0
#define NUANIMCURVE2_FULL 1    /* values.full: float keys with tangents */
#define NUANIMCURVE2_PACKED4 2 /* values.packed4 */
#define NUANIMCURVE2_STEP 3    /* values.step: no interpolation */
#define NUANIMCURVE2_BITS 4    /* one bit per frame in the chunk mask */
#define NUANIMCURVE2_PACKED6 5 /* values.packed6 */
#define NUANIMCURVE2_SCALED4 6 /* values.scaled4 */

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

/* Interpolated key value of a 16-bit (12-bit fraction) animation curve, scaled into the channel range. */
#define NUANIM_1648_VALUE(scale_min, start, end, fraction)                                                                                 \
    (((f32)((i32)(end) - (i32)(start)) * (fraction) / 4095.0 + (start)) * (scale_min)->scale + (scale_min)->minimum)
#define NUANIM_1648_LAST_KEY(keys) ((u16)(((keys)[2] & 0xf000) >> 8 | ((keys)[3] & 0xf000) >> 4 | (keys)[1] >> 12))

#define NUANIM_REBASE_POINTER(pointer, amount) ((pointer) == NULL ? NULL : (void *)((u32)(pointer) + (u32)(amount)))

#define PC_LEGACY_LIMIT_ANGLE(destination, maximum, minimum)                                                                               \
    do {                                                                                                                                   \
        (destination) = (i32)((u32)(destination) & 0xffff);                                                                                \
        if ((destination) >= 0x8000)                                                                                                       \
            (destination) -= 0x10000;                                                                                                      \
        if ((destination) > (maximum))                                                                                                     \
            (destination) = (maximum);                                                                                                     \
        else if ((destination) < (minimum))                                                                                                \
            (destination) = (minimum);                                                                                                     \
    } while (0)
