/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/props/system/socksys.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void GameCam_HitJudder(void);
    u16 GameCam_GetAdjustedYRot(GAMECAMERA *);
    void GameCam_HitRoll(void);

    typedef struct VIEWCAM { // size 0x28
        i32 mode;            // 0x00
        NUVEC target;        // 0x04
        i16 pitch;           // 0x10
        i16 yaw;             // 0x12
        f32 distance;        // 0x14
        f32 height;          // 0x18
        f32 zoom_step;       // 0x1c
        u32 unknown_20;      // 0x20
        GAMEPAD *gamepad;    // 0x24
    } VIEWCAM;

    extern i32 GUNSHIPAHACK;

    extern NARROWSOCKEXCEPTION *NarrowSockException;
    i32 ObjInNarrowSock(GAMEOBJECT *, SOCKSYS_PC *, i32);
    void SetCameraMatrices(void);

    typedef struct KEEP_SCREEN_PLANE { // size 0x18
        NUVEC point;                   // 0x00
        NUVEC normal;                  // 0x0c
    } KEEP_SCREEN_PLANE;

    extern u8 CutSceneCameraMatrixSet;
    extern i32 (*AlternateGameCallback)(GAMEOBJECT *object);
#ifdef __cplusplus
}
#endif
