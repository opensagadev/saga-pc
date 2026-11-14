#ifndef SAGA_LEGOAPI_CHARACTERS_MOTION_GAMEANIM_H
#define SAGA_LEGOAPI_CHARACTERS_MOTION_GAMEANIM_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void RootFnEx(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32, i32);
    void RootFn(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32);
    void RootFnY(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32);
    void BlendRootFn(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32);
    GAMEANIMOBJ *GameAnimSet_AddObject(GAMEANIMSET *set, NUSPECIALHANDLE *special, f32 start_frame, f32 end_frame,
                                       i32 append);
    i32 GameAnimSet_GetAveragePos(GAMEANIMSET *set, NUVEC *position, i32 frame_selection, i32 include_animated,
                                  i32 include_static);
    i32 GameAnimSet_GetCentreAndRadius(GAMEANIMSET *set, NUVEC *centre, f32 *radius, i32 frame_selection,
                                       i32 include_animated, i32 include_static);
    i32 GameAnimSet_GetVisibility(GAMEANIMSET *set);
    void GameAnimSet_SetVisibility(GAMEANIMSET *set, i32 visibility);
    void GameAnimSet_EvalAnim(GAMEANIMSET *set);
    void GameAnimSys_Update(GAMEANIMSYS *system);

    void GameAnimSys_AllocateLevelProgressData(VARIPTR *buf, VARIPTR *end, i32 capacity, i32 level_count);
    void GameAnimSys_StoreProgress(GAMEANIMSYS *system, i32 index);
    u8 *GameAnimSys_GetProgressData(i32 index);
    void GameAnimSys_ClearProgress(i32 index);
    GAMEANIMSET *GameAnimSet_Create(VARIPTR *buf, VARIPTR *end, GAMEANIMOBJPOOL *pool, GAMEANIMSYS *system);
    void GameAnimSet_AddToSystemList(GAMEANIMSET *set);
    void GameAnimSet_EvaluateState(GAMEANIMSET *set);
    void GameAnimSet_RemoveFromSystemList(GAMEANIMSET *set);
    void GameAnimSys_ReStoreProgress(GAMEANIMSYS *system, i32 index);
    GAMEANIMOBJPOOL *GameAnimSet_CreateObjectPool(VARIPTR *buf, VARIPTR *end, i32 object_data_size, i32 capacity);
    void GameAnimSet_RemoveObject(GAMEANIMSET *set, GAMEANIMOBJ *object);
    void GameAnimSet_RemoveAllObjects(GAMEANIMSET *set);
    void GameAnimSet_SetTFactor(GAMEANIMSET *set, f32 factor);
    void GameAnimSet_ScaleFParam1(GAMEANIMSET *set, f32 scale);
    i32 GameAnimSet_IsAnimationReset(GAMEANIMSET *set);
    i32 GameAnimSet_Reset(GAMEANIMSET *set);
    i32 GameAnimSet_Play(GAMEANIMSET *set, f32 speed, i32 evaluate_state);
    i32 GameAnimSet_Stop(GAMEANIMSET *set);
    i32 GameAnimSet_SetRepeating(GAMEANIMSET *set, i32 repeating);
    i32 GameAnimSet_JumpToEnd(GAMEANIMSET *set);
    i32 GameAnimSet_JumpToStart(GAMEANIMSET *set);
    void GameAnimSet_JumpToAnimPos(GAMEANIMSET *set, f32 position);
    i32 GameAnimSet_Playing(GAMEANIMSET *set);
    f32 GameAnimSet_GetCurrentFrame(GAMEANIMSET *set);
    f32 GameAnimSet_GetAnimPos(GAMEANIMOBJ *object);
    void GameAnimSet_SetAnimPos(GAMEANIMOBJ *object, f32 position);
    f32 GameAnimSet_GetCompletionRatio(GAMEANIMSET *set);
    i32 CurrentAnim(ANIMPACKET_PC *);
    f32 AnimDuration(i32, i32, f32, f32, i32);
    void ResetAnimPacket(ANIMPACKET_PC *, i32);
    void EvalAnim(NUSPECIALHANDLE *special, f32 frame, NUMTX *matrix, i32 update);
    void EvalAnim2(NUSPECIALHANDLE *special, f32 frame);
    extern u8 ForcePlayEndFrame;
    extern u8 ForceEulerToQuat;
    f32 AnimStopFrame(CHARACTER_MODEL_LAYERS_PC *, i32);
    i32 AnimMiscFlags(CHARACTER_MODEL_LAYERS_PC *, i32);
    f32 AnimSpeed(CHARACTER_MODEL_LAYERS_PC *, i32);
    void EvalModelAnim(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *, NUMTX *, NUMTX *, DWA_WEIGHT_BLOCK_PC ***, NUVEC *,
                       NUMTX *, u32);

    /* Proposed finite public interfaces; UpdateAnimTimer remains original-TU private. */
    void AnimatePlayer(GAMEOBJECT *);
    void UpdateAnimPacket(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *, f32, f32, f32, f32);
    void UpdateMiniAnimPacket(CHARACTER_MODEL_LAYERS_PC *, DRAW_ATTACHMENTS_MINI_PC *, f32, f32, f32);
    void SetAnimTimeRandom(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *);
    GAMEANIMOBJ *GameAnimSet_AddObjectByName(GAMEANIMSET *, NUGSCN *, char *, f32, f32, i32, void *, char *, char *);
    i32 GizmoFileReadGameAnimSet(GAMEANIMSET *, void *, void (*)(GAMEANIMOBJ *, u8), u8, char *, char *);
    void GameAnimSet_DrawReflection(GAMEANIMSET *, i32, f32, NUMTX *);
    void Animate_ASTROMECH(GAMEOBJECT *);
    void Animate_ATAT(GAMEOBJECT *);
    void Animate_BARMAN(GAMEOBJECT *);
    void Animate_BATTLEDROID(GAMEOBJECT *);
    void Animate_BEAST(GAMEOBJECT *);
    void Animate_CANNON(GAMEOBJECT *);
    void Animate_CRITTER(GAMEOBJECT *);
    void Animate_DEFAULT(GAMEOBJECT *);
    void Animate_DROIDEKA(GAMEOBJECT *);
    void Animate_GEONOSIAN(GAMEOBJECT *);
    void Animate_HOVERDROID(GAMEOBJECT *);
    void Animate_JEDI(GAMEOBJECT *);
    void Animate_PROTOCOL(GAMEOBJECT *);
    void Animate_REPUBLICGUNSHIP(GAMEOBJECT *);
    void Animate_SPEEDERBIKE(GAMEOBJECT *);
    void Animate_SUPERBATTLEDROID(GAMEOBJECT *);
    void Animate_VEHICLE(GAMEOBJECT *);
    void Animate_WALKER(GAMEOBJECT *);
    void Animate_WEIRDO(GAMEOBJECT *);
    void Animate_POD(GAMEOBJECT *);
    i16 FindAnimIX(struct characterdata_pc_s *character, char *name);
#ifdef __cplusplus
}
#endif

#endif
