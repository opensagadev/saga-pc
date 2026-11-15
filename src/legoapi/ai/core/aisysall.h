/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aiscript_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nuvec.h"
#endif

struct AIPATH;
struct EDAIPATH_s;
struct eduiitem_s;
struct nurnd_vertex3d_s;

#ifdef __cplusplus
extern "C" {
#endif
    void AIScriptCopyConditions(NULISTHDR *, NULISTHDR *, VARIPTR *, VARIPTR *);
    void AIScriptCopyActions(NULISTHDR *, NULISTHDR *, VARIPTR *, VARIPTR *);
    i32 AIScriptBuildDerivedScript(AISCRIPT *, VARIPTR *, VARIPTR *, AISYS_PC *);

#ifdef __cplusplus
    void AIScriptMissingStateDiagnostic(...);
    void AIScriptMissingStateDiagnosticAlternate(...);
    void AIScriptMissingConditionDiagnostic(...);
#ifdef __cplusplus
}
#endif
#elif !defined(AISYSALL_SCRIPT_DIAGNOSTIC_PROTOTYPES)
#ifdef __cplusplus
extern "C" {
#endif
    /* aisysall.c defines these variadic and declares their prototypes itself; other C units keep this form, since
     * VC8's code generation for unrelated units shifts with the declarations it sees here. */
    void AIScriptMissingStateDiagnostic();
    void AIScriptMissingStateDiagnosticAlternate();
    void AIScriptMissingConditionDiagnostic();
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void AddToAIGroup(AIGROUP *, APIOBJECT *);
    i32 AIPathCheckExtents(AIPATH *, NUVEC *);
    void AICircle(AISYS_PC *, AIPACKET *, APIOBJECT *, i32);
    void AIMoveDirectlyToDestination(AISYS_PC *, AIPACKET *, APIOBJECT *, i32);
    void xConditions(NUFPAR_PC *parser);
    void xActions(NUFPAR_PC *parser);
    void xConditionMacro(NUFPAR_PC *parser);
    void xActionMacro(NUFPAR_PC *parser);
    void xRefScript(NUFPAR_PC *parser);
    i16 PCPathArcSine(f32);
    i32 Action_NoTerrain(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_NoLosCheck(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_FlatTerrain(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_DontUseShadowTerrain(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_DontPush(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_CanSeeBehind(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_RequiresLOS(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetFullPathSearch(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_NoShadows(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    f32 Condition_Timer(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char *, void *);
    f32 Condition_Random(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char *, void *);
    f32 Condition_AlwaysTrue(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char *, void *);

    void AIPathCalcExtents(AIPATH *path);
    AISYS_PC *AISysLoadEx(VARIPTR *, VARIPTR *, i32, NUGSCN *, char *, char *, char *, char *);
    void DrawLocator(struct nuvec_s *arg0, f32 arg1, i32 arg2, i32 arg3);
    extern i32 aidata_version;
    void locatorEditor_PathNodeMoved(struct EDAIPATHNODE_s *node);
    void locatorEditor_cbSetLocatorSet(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void pathEditor_cbSetCurrentPath(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    extern f32 AiRndrCullDistance;
    struct AIGROUP *CreateAIGroup(struct AISYS_PC *arg0, i32 arg1, f32 arg2, f32 arg3, f32 arg4);
    void AiRndrLine3dDbg(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, u32 colour);
    extern f32 aiEditor_DrawYOffset;
    extern f32 default_xspacing;
    extern f32 default_zspacing;
    extern u8 default_nacross;
    extern u8 default_ngroup;
    extern void (*AIPathDeletedFn)(struct EDAIPATH_s *path);
    extern void (*GameAISaveFn)(AIPATHSYS *);
    extern void AiRndrLine3d(struct nurnd_vertex3d_s *, NUMTL *, NUMTX *);
    void AIRetreatFromDestination2(AISYS_PC *, AIPACKET *, APIOBJECT *, i32);
    void areaEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale);
    void routeEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale);
    void xConst(NUFPAR_PC *parser);
    void xDeriveFromScript(NUFPAR_PC *parser);
    void xInclude(NUFPAR_PC *parser);
    void xParam(NUFPAR_PC *parser);
    void xState(NUFPAR_PC *parser);
#ifdef __cplusplus
}
#endif
