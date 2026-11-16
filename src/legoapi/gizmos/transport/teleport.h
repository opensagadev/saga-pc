#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"

/* TELEPORT_MOTION.flags: the teleport config keywords (Tel_*) */
#define TELEPORT_ONE_WAY 0x1    /* 1_way */
#define TELEPORT_RANGE 0x2      /* range */
#define TELEPORT_CRAWL 0x4      /* crawl */
#define TELEPORT_FLIP_FLAP 0x8  /* flip_flap */
#define TELEPORT_HACKY_CAM 0x10 /* Hacky_Cam: the camera blends when the teleport ends */

#ifdef __cplusplus
extern "C" {
#endif
    i32 Teleport_UpdateHints(HINT *);
    void Teleports_Configure(WORLDINFO *, char *);
    typedef struct TELEPORT_MOTION TELEPORT_MOTION;
    TELEPORT_MOTION *Teleport_Find(GAMEOBJECT *, f32);
    void Teleport_MoveCode(GAMEOBJECT *, i32);
    void Teleports_Reset(WORLDINFO *);
    void Teleports_UpdateBeforeGameObjects(WORLDINFO *);
    void Teleports_UpdateAfterGameObjects(WORLDINFO *);
    void Teleport_NetMoveCode(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif
