#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"

#ifdef __cplusplus
extern "C" {
#endif
    void SnakeBeenHit(GAMEOBJECT *);
    extern f32 SnakeSegmentHeight;
    extern f32 SnakeDebrisUpwardSpeed;
    void InitSnakes(WORLDINFO *world);
    void DestroySnakeBody(GAMEOBJECT *);
    void BlowUpSnakeBody(GAMEOBJECT *);
    extern f32 SnakeHeadHeight;
    extern f32 SnakeSegmentSpacing;
    extern f32 SnakeYawSeekRate;
    extern u8 snakebodies[0x440];
    void DrawSnakeBody(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif
