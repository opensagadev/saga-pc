/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void DisorientateCode(GAMEOBJECT *, NUVEC *, f32);
    /* FloatAdvancePc static same originalTU; no public prototype. */

    i32 BoltType_FindIDByCreature(GAMEOBJECT *, i32);
    void Move_CANNON(GAMEOBJECT *);
    extern i32 ForcePush_SuperMindTrick;
    i32 CanObjSlide(GAMEOBJECT *, i32);
    i32 StartSlide(GAMEOBJECT *, i32);
    void Move_BEAST(GAMEOBJECT *);
    i32 objInNetWaitContext(GAMEOBJECT *, i32);
    GIZMOBLOWUP_PC *GizmoBlowUpOpponent(GAMEOBJECT *, f32, f32, f32, i32, u32, u32, u32);
    GAMEOBJECT *ObjOpponent(GAMEOBJECT *, f32, f32, i32, i32, i32);

    void Move_VEHICLE(GAMEOBJECT *);
    void Move_POD(GAMEOBJECT *);

    f32 AnimEndFrame(CHARACTER_MODEL_LAYERS_PC *, i32);
    extern i32 GizmoBlowUpOpponent_Behind;
    extern f32 GizmoBlowUpOpponent_Range2;
    void SetHeadTarget(GAMEOBJECT *, NUVEC *, i8, f32, f32, f32);
    i32 SetObjTarget(GAMEOBJECT *, GAMEOBJECT *);
    i32 SetGizmoBlowUpTarget(GAMEOBJECT *, GIZMOBLOWUP_PC *);
    i32 SetPartTarget(GAMEOBJECT *, PART_PC *);
    PART_PC *TargetPart(GAMEOBJECT *, NUVEC *, NUVEC *, f32, f32, i32, i32);
    extern i32 (*BigJump_LandActionFn)(GAMEOBJECT *);
    extern void (*BigJump_EndOfLandFn)(GAMEOBJECT *);
    i32 Glide_SetTargetMom(GAMEOBJECT *);
    i32 Hang_SetTargetMom(GAMEOBJECT *);
    i32 LedgeTerrain_SetTargetMom(GAMEOBJECT *);
    void SeekVec(NUVEC *, NUVEC *, NUVEC *, f32);
    void GameCam_Blend(GAMECAMERA *, f32, f32, i32);
    void PushAway(NUVEC *, float, NUVEC *, NUVEC *, GAMEOBJECT *, GAMEOBJECT *, float, u32);
    i32 Pushing(GAMEOBJECT *, u16 *, i32 *, i32 *);
    i32 CanClimbSurface(GAMEOBJECT *, i32);
    f32 PushingTowardsAngle(u16 push_heading, u16 target_heading);
    NUVEC *Grabber_GetGrabPos(void *, NUMTX *);
    i32 Slide_SetTargetMom(GAMEOBJECT *, u16, float);
    i32 TightRope_SetTargetMom(GAMEOBJECT *);
    i32 Conveyor_AdjustSpeed(NUVEC *);
    void SpecialMove_Attacker_SetTargetMom(GAMEOBJECT *);
    void SetPushAngle(GAMEOBJECT *);
    i32 WallShuffle_SetTargetMom(GAMEOBJECT *object, u16 heading);
    i32 Climb_SetTargetMom(GAMEOBJECT *object, u16 heading);
    void GetShootOrigin_LSW(GAMEOBJECT *, NUVEC *);
    u16 GetShootDirection_LSW(GAMEOBJECT *, NUVEC *);
    u16 SeekRot(u16, u16, f32);
    f32 SeekValF(f32, f32, f32);
    void UpdateLastSafePosition(GAMEOBJECT *);
    i32 StartBigJump(GAMEOBJECT *, NUVEC *, i32, f32, f32, i32);
    i32 ObjInTube(GAMEOBJECT *);
    GAMEPAD_PC *ViewCamGetGamePad(void);
    void CharPivot_Init(struct CHARPIVOT_PC *);
    void CharPivot_Check(GAMEOBJECT *, NUVEC *);
    f32 SeekLinearF(f32, f32, f32);
    u16 TurnRot(u16, u16, i32, i32 *);
    extern i32 ForcePush_SuperPush;
    GAMEOBJECT *TargetGameObject(GAMEOBJECT *, NUVEC *, NUVEC *, f32, f32, u32, i32, i32, i32);
    extern f32 MAGNETOFFSET;
    f32 CameraEmitterDistance(NUVEC *position);
    void GameCameraMakeMiniCut2(NUVEC *, NUVEC *, i32, f32, f32, f32, f32, i32, i32, i32);
    f32 Bolt_ObjTargetPosYAdjust(GAMEOBJECT *);
    i32 ViewCamGetMode(void);
    NUVEC *ViewCamGetTgt(void);
    i32 ObjLandReady(GAMEOBJECT *object);
    i32 MovingBackwards(GAMEOBJECT *);
    void Climb_UpdateMagnetRotation(GAMEOBJECT *);
    extern f32 LEDGETERRAINLOOKAHEAD;
    void GameCam_Judder(GAMECAMERA *camera, f32 amount, i32 axis, NUVEC *position);
    extern const char WorldLevelConfigFormat[7];
    void GameCam_NewShake(GAMECAMERA *, f32, f32, f32);

    GAMEOBJECT *AddDynamicCreature(i32, NUVEC *, i32, char *, AIPATHINFO_PC *, AIGROUP_PC *, i32, NUGSPLINE *, NUVEC *,
                                   i32, i32);
    void Move_DEFAULT(GAMEOBJECT *);
    void KeepPointOnScreen(NUVEC *, NUVEC *);

    void SetMoveAndAnimateFunctions(u32, u32, u32, u32, i32, void (*)(GAMEOBJECT *), void (*)(GAMEOBJECT *),
                                    void (*)(GAMEOBJECT *));
    void Move_ATAT(GAMEOBJECT *);
    void Move_BARMAN(GAMEOBJECT *);
    void Move_DRAGBOMB(GAMEOBJECT *);
    void Move_DROIDEKA(GAMEOBJECT *);
    void Move_HOVERDROID(GAMEOBJECT *);
    void Move_REPUBLICGUNSHIP(GAMEOBJECT *);
    void Move_SPEEDERBIKE(GAMEOBJECT *);
    void Move_SUPERBATTLEDROID(GAMEOBJECT *);
    void Move_WALKER(GAMEOBJECT *);

    void Move_GEONOSIAN(GAMEOBJECT *);
    void Move_JAWA(GAMEOBJECT *);

    void Move_CRITTER(GAMEOBJECT *);
    void Move_DROIDGENERIC(GAMEOBJECT *);
    void Move_JEDI(GAMEOBJECT *);
    void Move_WEIRDO(GAMEOBJECT *);
    extern struct CHARPIVOT_PC CharPivot_LSW[32];
    extern i32 ForcePush_Waft;
    void GameCam_Reset(GAMECAMERA *camera);
    void StartLaunch(struct gameobject_s *object);
#ifdef __cplusplus
}
#endif
