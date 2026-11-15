#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 StartFallLand(GAMEOBJECT *, i32);
    void BigJumpCode(GAMEOBJECT *object);
    void StartEndOfJump(GAMEOBJECT *object);
    void FindSlamOrigin(GAMEOBJECT *object, NUVEC *start, NUVEC *end);

    void StartJump(GAMEOBJECT *, i32);

    extern f32 JumpFailedLandChainTime;
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
