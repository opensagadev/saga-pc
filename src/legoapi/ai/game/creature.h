#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/lib/camera.h"
#include "legoapi/world/level.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 ComplexShadowBase;
    void ConfigureComplexShadow(GAMEOBJECT *object);
    void TerrainPlayer(GAMEOBJECT *);
    extern i32 NOTERRAINSTOP;
    extern i16 PunchRangeCompanionId;
    extern i32 NOTERRAINSTOP;
    extern f32 TerrainPlayerOtherMovementScale;
    extern f32 TerrainPlayerBikeMovementScale;
    extern f32 TerrainPlayerPlatConnectTolerance;
    extern i32 TerrainPlayerShadowFix;
    extern i16 ID_DEFAULTCHARACTER_A;
    extern i16 ID_DEFAULTCHARACTER_B;
    extern f32 WaitingForCharacterTime;
    extern f32 spline_seek_pos;
    extern f32 spline_seek_ang;
    extern f32 spline_xoffset_speed_seek;
    extern f32 spline_xoffset_speed_roll;
    extern const char AIScriptListFilename[11];
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
    void PlayerInitCapabilitiesVariant0(GAMEOBJECT *);
    void ResetPlayerPacket(void *, CHARACTERDATA *);
    void SetGameObjectCharacterData(GAMEOBJECT *);
    void ResetCharacterIdle(GAMEOBJECT *, i32, i32);
    void ResetPlayerMoves(GAMEOBJECT *);
    void ResetPlayerAI(GAMEOBJECT *);

    void ResetAICreature(GAMEOBJECT *, AISYS *);
    void ResetAICreatures(AISYS *);
    void KeepOnScreen(GAMEOBJECT *);

    extern KEEP_SCREEN_PLANE PlayPlane[6];
    extern i16 dat_007f17bc;
    extern i16 dat_007f17c8;
    extern i16 dat_007f17cc;
    extern i16 PermanentWeirdo1Id;
    extern i16 PermanentWeirdo2Id;
    extern i16 DefaultAnimationJabbaId;
    void UpdateCharacterIdle(GAMEOBJECT *);
    extern i16 DefaultAnimationJabbaId;
    void FixUpCharacters(CHARFIXUP *);
    void ExtraCharacterFixUpAfterConfig(void);
    void MovePlayer(struct gameobject_s *object);
    void MovePlayerSpline(struct gameobject_s *object);
    i32 SpawnMeleeCreatureType(i32 type);
    void DrawGameObjectsDraw(i32 pass);
    void ResetPlayer(struct gameobject_s *arg0, i32 arg1, struct nuvec_s *arg2, i32 arg3);
    void DrawWeapon_SetSabreObjects(struct gameobject_s *object, i32 red, i32 green, i32 blue, i32 purple, i32 *models, i32 *hilt);
    struct AILOCATOR *getSpawnLocator(f32 clip_radius, char *name);
    extern i32 CharacterLoadAreaModels;
    void InitAICreatures(AISYS *);
    extern i16 dat_007f1810;
    extern i16 dat_007f1820;
    i32 fun_0041aec0(GAMEOBJECT *object);
    void ClearAICreatures(void);
    void PlayerSetCurrentHp(GAMEOBJECT *object, i8 hitpoints);
    i32 DrawGameObjectsProcess(void);
    i32 MakePlayerList(i32);
    void DrawGameObjects(void);
#ifdef __cplusplus
}
#endif
