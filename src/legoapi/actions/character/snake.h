#ifndef SAGA_LEGOAPI_ACTIONS_CHARACTER_SNAKE_H
#define SAGA_LEGOAPI_ACTIONS_CHARACTER_SNAKE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void SnakeBeenHit(GAMEOBJECT *);
    extern f32 SnakeSegmentHeightPc;
    extern f32 SnakeDebrisUpwardSpeedPc;
    void InitSnakes(WORLDINFO *world);
    void DestroySnakeBody(GAMEOBJECT *);
    void BlowUpSnakeBody(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif

#endif
