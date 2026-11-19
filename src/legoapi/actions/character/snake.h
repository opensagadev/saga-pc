#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nuvec.h"

typedef struct snake_segment_s { // size 0x18
    NUVEC position;              // 0x00
    f32 ground_height;           // 0x0c
    NUANG yaw;                   // 0x10
    NUANG pitch;                 // 0x14
} SNAKE_SEGMENT;

typedef struct snake_body_s {   // size 0x110
    SNAKE_SEGMENT segments[11]; // 0x000
    f32 scale;                  // 0x108
    u16 segment_count;          // 0x10c
    u16 flags;                  // 0x10e
} SNAKE_BODY;

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
    extern SNAKE_BODY snakebodies[4];
    void DrawSnakeBody(GAMEOBJECT *);
    void UpdateSnakeBody(GAMEOBJECT *object);
#ifdef __cplusplus
}
#endif
