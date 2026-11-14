#ifndef SAGA_LEGOAPI_AI_CORE_AISYSALL_H
#define SAGA_LEGOAPI_AI_CORE_AISYSALL_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "editor/edpath_types.h"
#include "gameapi/ai/aisys/aiscript_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/edtools/edui.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void AIScriptCopyConditions(NULISTHDR *, NULISTHDR *, VARIPTR *, VARIPTR *);
    void AIScriptCopyActions(NULISTHDR *, NULISTHDR *, VARIPTR *, VARIPTR *);
    i32 AIScriptBuildDerivedScript(AISCRIPT_PC *, VARIPTR *, VARIPTR *, AISYS_PC *);

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
    void AddToAIGroup(AIGROUP_PC *, APIOBJECT_PC *);
    i32 AIPathCheckExtents(AIPATH_PC *, NUVEC *);
    void AICircle(AISYS_PC *, AIPACKET_PC *, APIOBJECT_PC *, i32);
    void AIMoveDirectlyToDestination(AISYS_PC *, AIPACKET_PC *, APIOBJECT_PC *, i32);
    void xConditions(NUFPAR_PC *parser);
    void xActions(NUFPAR_PC *parser);
    void xConditionMacro(NUFPAR_PC *parser);
    void xActionMacro(NUFPAR_PC *parser);
    void xRefScript(NUFPAR_PC *parser);
    i16 PCPathArcSine(f32);
    i32 Action_NoTerrain(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_NoLosCheck(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_FlatTerrain(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_DontUseShadowTerrain(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_DontPush(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_CanSeeBehind(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_RequiresLOS(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_SetFullPathSearch(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_NoShadows(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
    f32 Condition_Timer(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
    f32 Condition_Random(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
    f32 Condition_AlwaysTrue(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);

    void AIPathCalcExtents(AIPATH_PC *path);
    AISYS_PC *AISysLoadEx(VARIPTR *, VARIPTR *, i32, NUGSCN *, char *, char *, char *, char *);
    void DrawLocator(struct nuvec_s *arg0, f32 arg1, i32 arg2, i32 arg3);
    extern i32 aidata_version;
    void locatorEditor_PathNodeMoved(struct EDAIPATHNODE_s *node);
    void locatorEditor_cbSetLocatorSet(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void pathEditor_cbSetCurrentPath(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    extern f32 AiRndrCullDistance;
    struct AIGROUP_PC *CreateAIGroup(struct AISYS_PC *arg0, i32 arg1, f32 arg2, f32 arg3, f32 arg4);
    void AiRndrLine3dDbg(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, u32 colour);
    extern f32 aiEditor_DrawYOffset;
#ifdef __cplusplus
}
#endif

#endif
