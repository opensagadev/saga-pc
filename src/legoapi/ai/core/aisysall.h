#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aiscript_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nuvec.h"

struct AIPATH;
struct EDAIPATH_s;
struct eduiitem_s;
struct nurnd_vertex3d_s;

#ifdef __cplusplus
extern "C" {
#endif
    void AIScriptCopyConditions(NULISTHDR *, NULISTHDR *, VARIPTR *, VARIPTR *);
    void AIScriptCopyActions(NULISTHDR *, NULISTHDR *, VARIPTR *, VARIPTR *);
    i32 AIScriptBuildDerivedScript(AISCRIPT *, VARIPTR *, VARIPTR *, AISYS *);

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
    void AICircle(AISYS *, AIPACKET *, APIOBJECT *, i32);
    void AIMoveDirectlyToDestination(AISYS *, AIPACKET *, APIOBJECT *, i32);
    void xConditions(NUFPAR *parser);
    void xActions(NUFPAR *parser);
    void xConditionMacro(NUFPAR *parser);
    void xActionMacro(NUFPAR *parser);
    void xRefScript(NUFPAR *parser);
    i16 PCPathArcSine(f32);
    i32 Action_NoTerrain(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_NoLosCheck(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_FlatTerrain(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_DontUseShadowTerrain(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_DontPush(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_CanSeeBehind(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_RequiresLOS(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetFullPathSearch(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_NoShadows(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    f32 Condition_Timer(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char *, void *);
    f32 Condition_Random(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char *, void *);
    f32 Condition_AlwaysTrue(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char *, void *);

    void AIPathCalcExtents(AIPATH *path);
    AISYS *AISysLoadEx(VARIPTR *, VARIPTR *, i32, NUGSCN *, char *, char *, char *, char *);
    void DrawLocator(struct nuvec_s *arg0, f32 arg1, i32 arg2, i32 arg3);
    extern i32 aidata_version;
    void locatorEditor_PathNodeMoved(struct EDAIPATHNODE_s *node);
    void locatorEditor_cbSetLocatorSet(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void pathEditor_cbSetCurrentPath(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    extern f32 AiRndrCullDistance;
    struct AIGROUP *CreateAIGroup(struct AISYS *arg0, i32 arg1, f32 arg2, f32 arg3, f32 arg4);
    void AiRndrLine3dDbg(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, u32 colour);
    extern f32 aiEditor_DrawYOffset;
    extern f32 default_xspacing;
    extern f32 default_zspacing;
    extern u8 default_nacross;
    extern u8 default_ngroup;
    extern void (*AIPathDeletedFn)(struct EDAIPATH_s *path);
    extern void (*GameAISaveFn)(AIPATHSYS *);
    extern void AiRndrLine3d(struct nurnd_vertex3d_s *, NUMTL *, NUMTX *);
    void AIRetreatFromDestination2(AISYS *, AIPACKET *, APIOBJECT *, i32);
    void areaEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale);
    void routeEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale);
    void xConst(NUFPAR *parser);
    void xDeriveFromScript(NUFPAR *parser);
    void xInclude(NUFPAR *parser);
    void xParam(NUFPAR *parser);
    void xState(NUFPAR *parser);
    extern AIACTIONDEF *game_aiactiondefs;
    extern AIACTIONDEF api_aiactiondefs[];
    extern AICONDITIONDEF *game_aiconditiondefs;
    void InitFn_AIActionParseSpeed(AI_PARSE_SPEED);
    void InitFn_AIBigJumpToDestination(AI_BIG_JUMP_TO_DESTINATION);
    void InitFn_AIRespawnOnPath(AI_RESPAWN_ON_PATH);
    void InitFn_FindAlternativeSpecialObjectFn(AIFINDALTERNATIVESPECIALOBJECT *);
    void InitFn_GameAILoad(void (*)(AISYS *, i32, NUGSCN *, VARIPTR *, VARIPTR *));
    void InitFn_GetHearDistance(AI_CHARACTER_DISTANCE);
    void InitFn_GetViewRange(AI_CHARACTER_DISTANCE);
    void InitFn_GlobalCharacterTypeID(i32 (*)(char *));
    void InitFn_GlobalGetMaxViewHeight(AI_CHARACTER_DISTANCE);
    void InitFn_GlobalGetMinViewHeight(AI_CHARACTER_DISTANCE);
    void InitFn_LevelCharacterGlobalID(AI_LEVEL_CHARACTER_GLOBAL_ID);
    void InitFn_LevelCharacterTypeID(AI_LEVEL_CHARACTER_TYPE_ID);
    void InitFn_SpecialRouteCharacterTypeID(AI_LEVEL_CHARACTER_TYPE_ID);
    void RegisterAIScriptActions(AIACTIONDEF *);
    void RegisterAIScriptConditions(AICONDITIONDEF *);
    void SetScriptErrorLevel(i32);
    extern AIACTIONDEF *game_aiactiondefs;
    extern AIACTIONDEF api_aiactiondefs[];
    void AINetTrace(const char *format, ...);
    void AINetReplicateAction(AIACTION *action, struct AIPACKET *packet);
#ifdef __cplusplus
}
#endif
