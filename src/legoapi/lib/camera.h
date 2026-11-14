#ifndef SAGA_LEGOAPI_LIB_CAMERA_H
#define SAGA_LEGOAPI_LIB_CAMERA_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/characters/motion/move.h, legoapi/items/base/apiobject.h,
 * legoapi/legoapi_types.h (not all of it earlier in the shim). */

#include "legoapi/props/system/socksys.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void GameCam_HitJudder(void);
    u16 GameCam_GetAdjustedYRot(GAMECAMERA *);
    void GameCam_HitRoll(void);

    typedef struct VIEWCAM_PC {
        i32 mode;
        NUVEC target;
        i16 pitch;
        i16 yaw;
        f32 distance;
        f32 height;
        f32 zoom_step;
        u32 unknown_20;
        GAMEPAD_PC *gamepad;
    } VIEWCAM_PC;

    STATIC_ASSERT(offsetof(VIEWCAM_PC, mode) == 0);
    STATIC_ASSERT(offsetof(VIEWCAM_PC, target) == 4);
    STATIC_ASSERT(offsetof(VIEWCAM_PC, pitch) == 0x10);
    STATIC_ASSERT(offsetof(VIEWCAM_PC, yaw) == 0x12);
    STATIC_ASSERT(offsetof(VIEWCAM_PC, distance) == 0x14);
    STATIC_ASSERT(offsetof(VIEWCAM_PC, height) == 0x18);
    STATIC_ASSERT(offsetof(VIEWCAM_PC, zoom_step) == 0x1c);
    STATIC_ASSERT(offsetof(VIEWCAM_PC, gamepad) == 0x24);
    STATIC_ASSERT(sizeof(VIEWCAM_PC) == 0x28);
    extern i32 GUNSHIPAHACK;

    extern NARROWSOCKEXCEPTION_PC *NarrowSockException;
    i32 ObjInNarrowSock(GAMEOBJECT *, SOCKSYS_PC *, i32);
    void SetCameraMatrices(void);

    typedef struct KEEP_SCREEN_PLANE_PC {
        NUVEC point;
        NUVEC normal;
    } KEEP_SCREEN_PLANE_PC;

    STATIC_ASSERT(sizeof(KEEP_SCREEN_PLANE_PC) == 24);
    STATIC_ASSERT(offsetof(KEEP_SCREEN_PLANE_PC, point) == 0);
    STATIC_ASSERT(offsetof(KEEP_SCREEN_PLANE_PC, normal) == 12);
    extern u8 CutSceneCameraMatrixSetPc;
    extern i32 (*AlternateGameCallbackPc)(GAMEOBJECT *object);
#ifdef __cplusplus
}
#endif

#ifndef TYPES_OFF_24
typedef struct CAMERA_CONTEXT_PATH_PC {
    u8 unknown_00[0x40];      // 0x00
    struct nugspline_s *path; // 0x40
    u8 unknown_44[8];         // 0x44
    u8 flags;                 // 0x4c
} CAMERA_CONTEXT_PATH_PC;     // size 0x50

typedef struct CAMERA_CONTEXT_FOCUS_PC {
    u8 unknown_00[0x28];   // 0x00
    NUVEC focus;           // 0x28
} CAMERA_CONTEXT_FOCUS_PC; // size 0x34

typedef struct CAMERA_CONTEXT_BUILDIT_PC {
    u8 unknown_00[0x34];     // 0x00
    NUVEC position;          // 0x34
} CAMERA_CONTEXT_BUILDIT_PC; // size 0x40

typedef struct CAMERA_GIZFORCE_TARGET_PC {
    u8 unknown_00[0x1c];     // 0x00
    NUVEC position;          // 0x1c
} CAMERA_GIZFORCE_TARGET_PC; // size 0x28

typedef struct CAMERA_CABLE_PC {
    GAMEOBJECT *source; // 0x0
    GAMEOBJECT *target; // 0x4
} CAMERA_CABLE_PC;      // size 0x8

typedef struct CAMERA_GRABBER_PC {
    u8 unknown_000[0x48c]; // 0x000
    NUVEC grab_position;   // 0x48c
} CAMERA_GRABBER_PC;       // size 0x498

#endif /* TYPES_OFF_24 */
#endif
