#ifndef SAGA_LEGOAPI_AI_GAME_CREATURE_H
#define SAGA_LEGOAPI_AI_GAME_CREATURE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/lib/camera.h"
#include "legoapi/world/level.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/lib/camera.h"
#include "legoapi/world/level.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/lib/camera.h"
#include "legoapi/world/level.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 ComplexShadowBasePc;
    void ConfigureComplexShadow(GAMEOBJECT *object);
    void TerrainPlayer(GAMEOBJECT *);
    extern i32 NOTERRAINSTOP;
    extern i16 PunchRangeCompanionIdPc;
    extern i32 NOTERRAINSTOP;
    extern f32 TerrainPlayerOtherMovementScalePc;
    extern f32 TerrainPlayerBikeMovementScalePc;
    extern f32 TerrainPlayerPlatConnectTolerancePc;
    extern i32 TerrainPlayerShadowFixPc;
    extern i16 ID_DEFAULTCHARACTER_A;
    extern i16 ID_DEFAULTCHARACTER_B;
    extern f32 WaitingForCharacterTime;
    extern f32 spline_seek_pos;
    extern f32 spline_seek_ang;
    extern f32 spline_xoffset_speed_seek;
    extern f32 spline_xoffset_speed_roll;
    extern const char AIScriptListFilename[11];
    extern const char GameObjectState_InActive[9];
    void DeactivateGameObject(GAMEOBJECT *);
    extern f32 PlayerPadCurrentSpeed[2];
    extern f32 PlayerPadPreviousSpeed[2];
    extern u16 PlayerPadCurrentAngle[2];
    extern u16 PlayerPadPreviousAngle[2];
    i32 AnakinGreenSabre(GAMEOBJECT *);
    GAMEOBJECT *GetOtherActivePlayer(GAMEOBJECT *);
    extern i32 waiting_for_character;

    void StoreProgressAICharacter(LEVEL_PROGRESS *progress);
    void AICreatureResumeScript(GAMEOBJECT *);
    void PlayerInitCapabilitiesVariant0Pc(GAMEOBJECT *);
    void ResetPlayerPacket(void *, CHARACTERDATA_PC *);
    void SetGameObjectCharacterData(GAMEOBJECT *);
    void ResetCharacterIdle(GAMEOBJECT *, i32, i32);
    void ResetPlayerMoves(GAMEOBJECT *);
    void ResetPlayerAI(GAMEOBJECT *);

    void ResetAICreature(GAMEOBJECT *, AISYS_PC *);
    void ResetAICreatures(AISYS_PC *);
    void KeepOnScreen(GAMEOBJECT *);

    extern KEEP_SCREEN_PLANE_PC PlayPlanePc[6];
    extern i16 AreaExtraTargetId7f17bcPc;
    extern i16 AreaExtraSourceId7f17c8Pc;
    extern i16 AreaExtraTargetId7f17ccPc;
    extern i16 PermanentWeirdo1IdPc;
    extern i16 PermanentWeirdo2IdPc;
    extern i16 DefaultAnimationJabbaIdPc;
    void UpdateCharacterIdle(GAMEOBJECT *);
    extern i16 DefaultAnimationJabbaIdPc;
    void FixUpCharacters(CHARFIXUP_PC *);
    void ExtraCharacterFixUpAfterConfig(void);
    void MovePlayer(struct gameobject_s *object);
    void MovePlayerSpline(struct gameobject_s *object);
    i32 SpawnMeleeCreatureType(i32 type);
    void DrawGameObjectsDraw(i32 pass);
    void ResetPlayer(struct gameobject_s *arg0, i32 arg1, struct nuvec_s *arg2, i32 arg3);
    void DrawWeapon_SetSabreObjects(struct gameobject_s *object, i32 red, i32 green, i32 blue, i32 purple, i32 *models,
                                    i32 *hilt);
    struct AILOCATOR_PC *getSpawnLocator(f32 clip_radius, char *name);
    extern i32 CharacterLoadAreaModelsPc;
    void InitAICreatures(AISYS_PC *);
#ifdef __cplusplus
}
#endif

#endif
