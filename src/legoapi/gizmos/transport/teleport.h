#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/numath/numtx.h"

/* TELEPORT_CONFIG.flags: the teleport config keywords (Tel_*) */
#define TELEPORT_ONE_WAY 0x1    /* 1_way */
#define TELEPORT_RANGE 0x2      /* range */
#define TELEPORT_CRAWL 0x4      /* crawl */
#define TELEPORT_FLIP_FLAP 0x8  /* flip_flap */
#define TELEPORT_HACKY_CAM 0x10 /* Hacky_Cam: the camera blends when the teleport ends */

/* A teleport: a spline a character crawls or flies along, with two flaps at its ends. WORLDINFO.teleports holds
 * them; the teleport gizmos point at them. */
typedef struct TELEPORT_CONFIG {   // size 0xfc
    char name[0x40];               // 0x00
    NUGSPLINE *path;               // 0x40
    f32 duration;                  // 0x44
    f32 range_squared;             // 0x48
    u16 flags;                     // 0x4c
    u8 enabled;                    // 0x4e
    u8 active;                     // 0x4f
    nuhspecial_s blocking_special; // 0x50
    nuhspecial_s flap1_special;    // 0x5c
    nuhspecial_s flap2_special;    // 0x68
    u16 flap_angle[2];             // 0x74
    u16 flap_target[2];            // 0x78
    NUMTX flap1_matrix;            // 0x7c
    NUMTX flap2_matrix;            // 0xbc
} TELEPORT_CONFIG;

#ifdef __cplusplus
extern "C" {
#endif
    i32 Teleport_UpdateHints(HINT *);
    void Teleports_Configure(WORLDINFO *, char *);
    TELEPORT_CONFIG *Teleport_Find(GAMEOBJECT *, f32);
    void Teleport_MoveCode(GAMEOBJECT *, i32);
    void Teleports_Reset(WORLDINFO *);
    void Teleports_UpdateBeforeGameObjects(WORLDINFO *);
    void Teleports_UpdateAfterGameObjects(WORLDINFO *);
    void Teleport_NetMoveCode(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif
