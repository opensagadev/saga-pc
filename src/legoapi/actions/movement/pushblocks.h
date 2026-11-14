#ifndef SAGA_LEGOAPI_ACTIONS_MOVEMENT_PUSHBLOCKS_H
#define SAGA_LEGOAPI_ACTIONS_MOVEMENT_PUSHBLOCKS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/legoapi_types.h (not all of it earlier in
 * the shim). */

#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 Push_UpdateHints(HINT_PC *);
    extern u16 PushBlockFacingTolerancePc;
    PUSH_BLOCK_PC *NearestFacingPushBlock(WORLDINFO *, GAMEOBJECT *, f32);
    i32 PushBlock(GAMEOBJECT *);

    void PushSeekComplete(PUSH_BLOCK_PC *, i32);
    i32 OtherBlockInRange(WORLDINFO *, PUSH_BLOCK_PC *, NUVEC *, i32);
    i32 SnapPosTaken(WORLDINFO *, PUSH_BLOCK_PC *, NUVEC *, i32);
    i32 TerrainBlockOnBlock(WORLDINFO *, PUSH_BLOCK_PC *, NUVEC *, f32 *);

    PUSH_BLOCK_PC *BlockInBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, PUSH_BLOCK_PC **);
    void MoveBlocksOverBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
    void MoveBlocks(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
    void UpdatePushBlocks(void *, void *, f32);
    extern f32 PushVelocitySeekRatePc;
    extern f32 PushFallingSpeedPc;
    extern f32 PushCornerInsetPc;
    extern f32 PushGroundOffsetPc;
    extern f32 PushRumblePeriodPc;
    extern f32 PushRumbleWindowPc;
    extern f32 PushBlockOverlapShrinkPc;
    extern i32 PushBlockHintIdPc;
    void KnockPushBlock(PUSH_BLOCK_PC *, NUVEC *);
    PUSH_BLOCK_PC *NearestPushBlock(WORLDINFO *, NUVEC *, f32);
    extern char *PushBlocksNamePc;
    extern f32 PushRumbleWindowPc;
    extern f32 PushRumblePeriodPc;
    extern f32 PushFallingSpeedPc;
    extern f32 PushVelocitySeekRatePc;
    extern f32 PushGroundOffsetPc;
    extern f32 PushCornerInsetPc;
    extern i32 PushBlockHintIdPc;
    extern f32 PushBlockOverlapShrinkPc;
    extern char *PushBlocksNamePc;
    extern u32 (*CanPushBlocksFn)(GAMEOBJECT *);
    extern i32 runoutofpostabspace;
    void ResetSinglePushBlock(struct WORLDINFO_s *world, struct pushblock_s *block, i32 index);
    void ResetPushProgress(struct WORLDINFO_s *world, void *progress);
    void GizmoPushBlockInitAndReset(struct WORLDINFO_s *world, void *progress);
    void ResetSinglePushBlockHeight(struct WORLDINFO_s *arg0, struct pushblock_s *arg1, i32 arg2);
    extern i32 pushposincrease;
#ifdef __cplusplus
}
#endif

#endif
