#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 Push_UpdateHints(HINT *);
    extern u16 PushBlockFacingTolerance;
    PUSH_BLOCK *NearestFacingPushBlock(WORLDINFO *, GAMEOBJECT *, f32);
    i32 PushBlock(GAMEOBJECT *);

    void PushSeekComplete(PUSH_BLOCK *, i32);
    i32 OtherBlockInRange(WORLDINFO *, PUSH_BLOCK *, NUVEC *, i32);
    i32 SnapPosTaken(WORLDINFO *, PUSH_BLOCK *, NUVEC *, i32);
    i32 TerrainBlockOnBlock(WORLDINFO *, PUSH_BLOCK *, NUVEC *, f32 *);

    PUSH_BLOCK *BlockInBlock(WORLDINFO *, PUSH_BLOCK *, i32, PUSH_BLOCK **);
    void MoveBlocksOverBlock(WORLDINFO *, PUSH_BLOCK *, i32, NUVEC *);
    void MoveBlocks(WORLDINFO *, PUSH_BLOCK *, i32, NUVEC *);
    void UpdatePushBlocks(void *, void *, f32);
    extern f32 PushVelocitySeekRate;
    extern f32 PushFallingSpeed;
    extern f32 PushCornerInset;
    extern f32 PushGroundOffset;
    extern f32 PushRumblePeriod;
    extern f32 PushRumbleWindow;
    extern f32 PushBlockOverlapShrink;
    extern i32 PushBlockHintId;
    void KnockPushBlock(PUSH_BLOCK *, NUVEC *);
    PUSH_BLOCK *NearestPushBlock(WORLDINFO *, NUVEC *, f32);
    extern char *PushBlocksName;
    extern f32 PushRumbleWindow;
    extern f32 PushRumblePeriod;
    extern f32 PushFallingSpeed;
    extern f32 PushVelocitySeekRate;
    extern f32 PushGroundOffset;
    extern f32 PushCornerInset;
    extern i32 PushBlockHintId;
    extern f32 PushBlockOverlapShrink;
    extern char *PushBlocksName;
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
