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
#ifdef __cplusplus
}
#endif
