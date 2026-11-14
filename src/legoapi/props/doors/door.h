/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART
#define SAGA_LEGOAPI_PROPS_DOORS_DOOR_H_PART 1
#endif

#include "nu2api/nucore/common.h"

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
    extern char DoorExitName[64];
    extern i32 Door_Start;
    extern i32 Door_NextSock;
    extern void *LastDoorPointer;
    extern char DoorExitCameraSplineName[32];
    extern NUVEC DoorCutCameraPosition0;
    extern NUVEC DoorCutCameraPosition1;
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
    extern void (*Door_GoThrough_ExtraCodeFn)(WORLDINFO *world, struct game_door_callback_s *door);
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

struct DOOR_s {                        // size 0x128
    char name[64];                     // 0x000
    char gizmo_name[64];               // 0x040
    char camera_spline_name[32];       // 0x080
    struct nugspline_s *spline;        // 0x0a0
    NUVEC point0;                      // 0x0a4
    NUVEC point1;                      // 0x0b0
    NUVEC opposite_midpoint;           // 0x0bc
    NUVEC point3;                      // 0x0c8
    NUVEC pos;                         // 0x0d4
    f32 radius;                        // 0x0e0
    NUVEC normal;                      // 0x0e4
    i16 level;                         // 0x0f0
    i16 freeplay_level;                // 0x0f2
    u8 next_sock;                      // 0x0f4
    u8 flags;                          // 0x0f5
    u8 vehicle;                        // 0x0f6
    u8 active;                         // 0x0f7
    struct nugspline_s *camera_spline; // 0x0f8
    f32 camera_wait;                   // 0x0fc
    f32 camera_blend_time;             // 0x100

    union { // size 0x8
        u64 takeover_character_mask;

        struct {              // size 0x8
            u32 vehicle_mask; // 0x108
            i32 vehicle_mode; // 0x10c
        };
    }; // 0x108

    NUPLANE plane;  // 0x110
    void *cutscene; // 0x120
};

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
