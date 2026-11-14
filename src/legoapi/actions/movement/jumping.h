/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 StartFallLand(GAMEOBJECT *, i32);
    void BigJumpCode(GAMEOBJECT *object);
    void StartEndOfJump(GAMEOBJECT *object);
    void FindSlamOrigin(GAMEOBJECT *object, NUVEC *start, NUVEC *end);

    void StartJump(GAMEOBJECT *, i32);

    extern f32 JumpFailedLandChainTimePc;
    extern f32 SLAMJUMPSPEED;
    extern i32 DoubleJump_AlwaysReachJump2Height;
    extern i32 DoubleJump_JediSlam;
    i32 Slam_Start(GAMEOBJECT *, f32);
    i32 StartBackFlip(GAMEOBJECT *);
    i32 UseFallAnim(GAMEOBJECT *);
    void JumpCode(GAMEOBJECT *, i32, i32, u32, i32, i32, i32);
    void MakeJumpReachHeight(GAMEOBJECT *, f32, i32);

    void BackFlipCode(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif
