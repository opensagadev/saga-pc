#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "nu2api/numath/nuvec.h"

struct AIPACKET;
struct AISCRIPTPROCESS;
struct AISYS;
struct climb_slot_s;
struct gameobject_s;

typedef struct AITRIGGERSET_PARTIAL { // size 0x211
    u8 unknown_00[0x210];             // 0x000
    u8 flags_low;                     // 0x210
} AITRIGGERSET_PARTIAL;

#ifdef __cplusplus
extern "C" {
#endif
    extern const char AIUseCurrentSpeedMultiplierName[11];
    extern const char AIUseCurrentSpeedSnapName[12];
    extern const char AIAnimSpeedMultiplyByName[13];
    extern const f64 MoveSplineRunSpeedSentinel;
    extern const f32 MoveSplineTurnRate;
    extern const char AIActionFaceOffsetName[11];
    extern const char AIActionNearestOpponentName[17];
    extern const char AIActionClockwiseName[10];
    extern const char AIActionAnticlockwiseName[14];
    extern const char AIActionReverseName[8];
    extern const char AIActionIgnoreDoneFlagName[17];
    extern f32 party_follow_offsets[8];
    extern f32 aitol;
    extern f32 direction_scale;
    extern const char AIGameStateName[7];
    extern const char AIGameJediGoodieAttackName[18];
    extern const char AIGameRangeName[6];
    extern const char AIGameEndName[5];
    extern const char AIGameStartName[7];
    extern const char AIGameHoldSpecialButtonName[20];
    extern const char AIGameTurnName[5];
    extern const char AIGameRandomTurnDirectionName[14];
    extern const char AIGameMaximumTurnName[9];
    extern const char AIGameMinimumTurnName[9];
    extern const char AIGameTakeOverTargetName[15];
    extern f32 idealgoalrange;
    extern f32 engagefiretime;
    extern const char AICombatStaticName[7];
    extern const char AICombatCircleName[7];
    extern const char AICombatFrontArcOnlyName[13];
    extern const char AICombatDroidName[15];
    extern const char AICombatNearestEnemyName[23];
    extern const char AIExpressionTrueName[5];
    extern const char AIActionNumName[4];

    extern const char AICameraLiteral758070[18];
    extern const char AICameraLiteral757ffc[10];
    extern const char AICameraLiteral758160[18];
    extern const char AICameraLiteral75814c[20];
    extern const char AICameraLiteral758140[12];
    extern const char AICameraLiteral758134[10];
    extern const char AICameraLiteral758128[11];
    extern const char AICameraLiteral75811c[11];
    extern const char AICameraLiteral758110[11];
    extern const char AICameraLiteral758104[11];
    extern const char AICameraLiteral7580fc[6];
    extern const char AreaConfigKeyword_area_start[11];
    extern const char AreaConfigKeyword_redbrick_extra[15];
    extern const char AIControlSeekName[6];
    extern const char AIControlClearName[6];
    extern const char AIControlMaximumDefaultName[12];
    extern const char AIControlMultiplyName[10];
    extern const char AIControlLocatorName[8];

    extern const char GameAIPlayer2Name[9];
    extern const char GameAIPlayerFormat[9];
    extern const char AITransferLiteral757844[14];
    extern const char AITransferLiteral757860[10];
    extern const char AITransferLiteral757bb4[13];
    extern const char AITransferLiteral757ba8[11];
    extern const char AITransferLiteral757b9c[10];
    extern const char AITransferLiteral757ec4[10];
    extern const char AITransferLiteral757a5c[12];
    extern const char AITransferLiteral757cd8[5];
    extern const char AITransferLiteral757cc8[7];
    extern const char AITransferLiteral757e68[5];
    extern const char AITransferLiteral757e70[15];
    extern const char AITransferLiteral757a44[23];
    extern const char AITransferLiteral75773c[8];
    extern const char AITransferLiteral7576e0[10];
    extern const char AITransferLiteral7576d0[14];
    extern const char AITransferLiteral757744[7];
    extern const char AITransferLiteral757eb4[8];
    extern const char AITransferLiteral757ea8[10];
    extern const char AITransferLiteral7583d0[18];
    extern const char AITransferLiteral7583c4[10];
    extern const char AITransferLiteral7583b4[16];
    extern const char AITransferLiteral7583e4[12];
    extern const char AITransferLiteral7583f0[8];
    extern const char AITransferLiteral757a8c[7];
    extern const char AITransferLiteral757884[12];
    extern const char AITransferLiteral757c10[13];
    extern const char AITransferLiteral758194[11];
    extern const char AITransferLiteral758188[11];
    extern const char AITransferLiteral7581a0[8];
    extern const char AITransferLiteral758174[19];
    extern const char AIActionCircleAnticlockwiseName[14];
    extern const char AIActionCircleClockwiseName[10];
    extern const char PlayCountArgPrefix[11];
    extern const char RepeatDelayArgPrefix[10];
    extern const char StartDelayArgPrefix[12];
    extern const char CharacterPosArgPrefix[15];
    extern const char AIScriptFalse[6];
    extern i16 id_BATTLEDROIDCOMMANDER;
    extern i16 id_CLONEEP3;
    extern i16 id_CLONEEP3SAND;
    extern const char AIActionTorpedoPrefix[9];

    extern i16 id_JOKER_GOON;
    extern i16 id_JOKER_GOON_GUN;
    extern i16 id_PENGUIN_GOON;
    extern i16 id_PENGUIN_GOON_GUN;
    extern i16 id_RIDDLER_GOON;
    extern i16 id_RIDDLER_GOON_GUN;
    i16 GetGenericGoon(i32 arg0);
    extern f32 test_missile_scale;
#ifdef __cplusplus
}
#endif

typedef struct climb_system_s {   // size 0x8
    struct climb_slot_s *objects; // 0x0
    u16 count;                    // 0x4
    u16 capacity;                 // 0x6
} CLIMB_SYSTEM;

#ifdef __cplusplus
extern "C" {
#endif
    i32 Action_CompleteLevel(struct AISYS *system, struct AISCRIPTPROCESS *processor, struct AIPACKET *packet, char **params, i32 count,
                             i32 first_time, f32 elapsed);
    i32 Action_RaceOpponent(struct AISYS *system, struct AISCRIPTPROCESS *processor, struct AIPACKET *packet, char **params, i32 count,
                            i32 first_time, f32 elapsed);
    i32 Action_SetAnimSpeedMul(struct AISYS *system, struct AISCRIPTPROCESS *processor, struct AIPACKET *packet, char **params,
                               i32 num_params, i32 first_time, f32 elapsed);
    i32 Action_SnapToPosition(struct AISYS *system, struct AISCRIPTPROCESS *processor, struct AIPACKET *packet, char **params, i32 count,
                              i32 first_time, f32 elapsed);
    i32 Action_SnapToSockPosition(struct AISYS *system, struct AISCRIPTPROCESS *processor, struct AIPACKET *packet, char **params,
                                  i32 count, i32 first_time, f32 elapsed);
    i32 Action_UseCurrentSpeed(struct AISYS *system, struct AISCRIPTPROCESS *processor, struct AIPACKET *packet, char **params,
                               i32 num_params, i32 first_time, f32 elapsed);
    i32 Action_SetScaleOverride(struct AISYS *system, struct AISCRIPTPROCESS *processor, struct AIPACKET *packet, char **params,
                                i32 num_params, i32 first_time, f32 elapsed);
    i32 Action_SetScriptState(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_AttackOpponent(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_FollowDirection(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_FollowCharacter(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_GameFollowPlayer(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_MoveForward(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    AILOCATOR *LocalGetRandomLocator(AILOCATOR **locators, i32 count, f32 clip_radius, NUVEC *position, f32 max_distance,
                                     i32 outside_camera, f32 max_delta_y, f32 min_delta_y);
    i32 Action_DynamicCameraCut(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_BigJump(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetCurrentSpeed(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetGravityHeight(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SnapWeaponOut(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_DontAvoidCharacter(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_MoveAwayFromLastAttacker(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_CannotDropIn(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetSpline(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_CameraShake(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetZeroAcceleration(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_FaceCharacter(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetPath(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetInvulnerable(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetSide(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetTakeOverTarget(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetStateArea(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SnapToOrigin(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_GameDontPush(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetAIOverrideControl(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_PlayGizSpecial(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetLastAttacker(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetUseOneAtOnce(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_SetTaggable(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_TagCharacter(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_RegisterTakeOverObject(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_CircleLocator(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_GoToOriginalPath(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_PlaySfx(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);

    i32 Action_Kill(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_ResetContext(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_TakeDamage(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_CanBeCarried(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_Activate(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_RemoveThrownForceObjects(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_AnimTimeRandom(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_PlayGizObstacle(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_UseForce(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_ProbeDroid(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_AlertCreatures(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    i32 Action_EngageOpponent(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
    struct AILOCATOR *LocalGetNearestLocator(void *arg0, i32 arg1, f32 arg2, struct nuvec_s *arg3, f32 arg4, i32 arg5, f32 arg6, f32 arg7);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    struct gameobject_s *GetNamedGameObject(void *, char *);
    extern i32 turret_gizmotype_id;
    extern AIACTIONDEF lego_aiactiondefs[];
    extern AICONDITIONDEF lego_aiconditiondefs[];
    extern i32 Action_HelpWithTriggers(struct AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);
#ifdef __cplusplus
}
#endif
