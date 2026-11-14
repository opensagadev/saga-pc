#ifndef SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART
#define SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART == 1 && !defined(SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 gone_through_door_to_new_level;
    extern i32 Door_UseCutCam;
    extern char DoorExitNamePc[64];
    extern i32 Door_Start;
    extern i32 Door_NextSock;
    extern void *LastDoorPointerPc;
    extern char DoorExitCameraSplineNamePc[32];
    extern NUVEC DoorCutCameraPosition0Pc;
    extern NUVEC DoorCutCameraPosition1Pc;
    i32 StartDoorPositions(void);
    void Door_Reset(void);
    extern f32 Door_CutCamBlendTime;
    extern NUVEC Door_CutCamPos0;
    extern NUVEC Door_CutCamPos1;
    extern f32 Door_CutCamWait;
    extern f32 Door_CutCamWaitTime;
    extern i32 Door_CutLookAtPlayers;
    extern char Door_ExitName[64];
    extern i32 gone_through_door_to_new_mode;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART
#define SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART == 2 && !defined(SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void Doors_Init(WORLDINFO *);
    void *Door_FindByName(WORLDINFO *, char *);
    void *Door_FindByIndex(WORLDINFO *, i32, i32, NUVEC *);
#ifdef __cplusplus
}
#endif
typedef struct DOOR_s DOOR_s;
#ifdef __cplusplus
extern "C" {
#endif
    void Doors_Check(WORLDINFO *, GAMEOBJECT *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART
#define SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART == 3 && !defined(SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern void (*Door_GoThrough_ExtraCodeFn)(WORLDINFO *world, struct game_door_callback_pc_s *door);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART
#define SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART == 4 && !defined(SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_STOP)

struct DOOR_s {
    char name[64];
    char gizmo_name[64];
    char camera_spline_name[32];
    struct nugspline_s *spline;
    NUVEC point0;
    NUVEC point1;
    NUVEC opposite_midpoint;
    NUVEC point3;
    NUVEC pos;
    f32 radius;
    NUVEC normal;
    i16 level;
    i16 freeplay_level;
    u8 next_sock;
    u8 flags;
    u8 vehicle;
    u8 active;
    struct nugspline_s *camera_spline;
    f32 camera_wait;
    f32 camera_blend_time;

    union {
        u64 takeover_character_mask;

        struct {
            u32 vehicle_mask;
            i32 vehicle_mode;
        };
    };

    NUPLANE plane;
    void *cutscene;
};

STATIC_ASSERT(sizeof(struct DOOR_s) == 296);
#ifdef __cplusplus
extern "C" {
#endif
    void Door_GoThrough(struct WORLDINFO_s *world, struct DOOR_s *door, i32 player_triggered);
    extern struct DOOR_s *setlastdoor_last;
    void Doors_SetLastDoor(DOOR_s *door);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART
#define SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_STOP
