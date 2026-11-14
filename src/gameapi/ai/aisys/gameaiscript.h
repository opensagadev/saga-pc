/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_PART
#define SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_PART == 1 && !defined(SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_STOP)

struct AIPACKET_PC;
struct AISCRIPTPROCESS_PC;
struct AISYS_PC;
struct climb_slot_pc_s;
struct gameobject_s;

typedef struct AITRIGGERSET_PARTIAL_PC {
    u8 unknown_00[0x210];
    u8 flags_low;
} AITRIGGERSET_PARTIAL_PC;

STATIC_ASSERT(offsetof(AITRIGGERSET_PARTIAL_PC, flags_low) == 0x210);
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
    extern const char AIActionIgnoreDoneFlagNamePc[17];
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
    extern const char AITransferLiteral757a7c[8];
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
    extern const char AITransferLiteral757824[7];
    extern const char AITransferLiteral757884[12];
    extern const char AITransferLiteral757c10[13];
    extern const char AITransferLiteral758194[11];
    extern const char AITransferLiteral758188[11];
    extern const char AITransferLiteral7581a0[8];
    extern const char AITransferLiteral758174[19];
    extern const char GizmoSnapOptionNamePc[];
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
    extern const char AIActionTorpedoPrefixPc[9];

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
#undef SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_PART
#define SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_PART == 2 && !defined(SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_STOP)

typedef struct climb_system_pc_s {
    struct climb_slot_pc_s *objects;
    u16 count;
    u16 capacity;
} CLIMB_SYSTEM_PC;

STATIC_ASSERT(sizeof(CLIMB_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, count) == 4);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, capacity) == 6);
#ifdef __cplusplus
extern "C" {
#endif
    i32 Action_CompleteLevel(struct AISYS_PC *system, struct AISCRIPTPROCESS_PC *processor, struct AIPACKET_PC *packet,
                             char **params, i32 count, i32 first_time, f32 elapsed);
    i32 Action_RaceOpponent(struct AISYS_PC *system, struct AISCRIPTPROCESS_PC *processor, struct AIPACKET_PC *packet,
                            char **params, i32 count, i32 first_time, f32 elapsed);
    i32 Action_SetAnimSpeedMul(struct AISYS_PC *system, struct AISCRIPTPROCESS_PC *processor,
                               struct AIPACKET_PC *packet, char **params, i32 num_params, i32 first_time, f32 elapsed);
    i32 Action_SnapToPosition(struct AISYS_PC *system, struct AISCRIPTPROCESS_PC *processor, struct AIPACKET_PC *packet,
                              char **params, i32 count, i32 first_time, f32 elapsed);
    i32 Action_SnapToSockPosition(struct AISYS_PC *system, struct AISCRIPTPROCESS_PC *processor,
                                  struct AIPACKET_PC *packet, char **params, i32 count, i32 first_time, f32 elapsed);
    i32 Action_UseCurrentSpeed(struct AISYS_PC *system, struct AISCRIPTPROCESS_PC *processor,
                               struct AIPACKET_PC *packet, char **params, i32 num_params, i32 first_time, f32 elapsed);
    i32 Action_SetScaleOverride(struct AISYS_PC *system, struct AISCRIPTPROCESS_PC *processor,
                                struct AIPACKET_PC *packet, char **params, i32 num_params, i32 first_time, f32 elapsed);
    i32 Action_SetScriptState(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                              f32);
    i32 Action_AttackOpponent(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                              f32);
    i32 Action_FollowDirection(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                               f32);
    i32 Action_FollowCharacter(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                               f32);
    i32 Action_GameFollowPlayer(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                                f32);
    i32 Action_MoveForward(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                           f32);
    AILOCATOR_PC *LocalGetRandomLocator(AILOCATOR_PC **locators, i32 count, f32 clip_radius, NUVEC *position,
                                        f32 max_distance, i32 outside_camera, f32 max_delta_y, f32 min_delta_y);
    i32 Action_DynamicCameraCut(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                                f32);
    i32 Action_BigJump(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_SetCurrentSpeed(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                               f32);
    i32 Action_SetGravityHeight(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                                f32);
    i32 Action_SnapWeaponOut(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                             f32);
    i32 Action_DontAvoidCharacter(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32,
                                  i32, f32);
    i32 Action_MoveAwayFromLastAttacker(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **,
                                        i32, i32, f32);
    i32 Action_CannotDropIn(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                            f32);
    i32 Action_SetSpline(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_CameraShake(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                           f32);
    i32 Action_SetZeroAcceleration(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32,
                                   i32, f32);
    i32 Action_FaceCharacter(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                             f32);
    i32 Action_SetPath(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_SetInvulnerable(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                               f32);
    i32 Action_SetSide(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_SetTakeOverTarget(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32,
                                 i32, f32);
    i32 Action_SetStateArea(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                            f32);
    i32 Action_SnapToOrigin(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                            f32);
    i32 Action_GameDontPush(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                            f32);
    i32 Action_SetAIOverrideControl(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32,
                                    i32, f32);
    i32 Action_PlayGizSpecial(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                              f32);
    i32 Action_SetLastAttacker(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                               f32);
    i32 Action_SetUseOneAtOnce(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                               f32);
    i32 Action_SetTaggable(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                           f32);
    i32 Action_TagCharacter(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                            f32);
    i32 Action_RegisterTakeOverObject(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **,
                                      i32, i32, f32);
    i32 Action_CircleLocator(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                             f32);
    i32 Action_GoToOriginalPath(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                                f32);
    i32 Action_PlaySfx(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32, f32);

    i32 Action_Kill(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_ResetContext(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                            f32);
    i32 Action_TakeDamage(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_CanBeCarried(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                            f32);
    i32 Action_Activate(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_RemoveThrownForceObjects(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **,
                                        i32, i32, f32);
    i32 Action_AnimTimeRandom(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                              f32);
    i32 Action_PlayGizObstacle(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                               f32);
    i32 Action_UseForce(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_ProbeDroid(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32, f32);
    i32 Action_AlertCreatures(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                              f32);
    i32 Action_EngageOpponent(struct AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32,
                              f32);
    struct AILOCATOR_PC *LocalGetNearestLocator(void *arg0, i32 arg1, f32 arg2, struct nuvec_s *arg3, f32 arg4,
                                                i32 arg5, f32 arg6, f32 arg7);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_PART
#define SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_PART == 3 && !defined(SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    struct gameobject_s *GetNamedGameObject(void *, char *);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_PART
#define SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_STOP
#endif
#endif

#undef SAGA_GAMEAPI_AI_AISYS_GAMEAISCRIPT_H_STOP
