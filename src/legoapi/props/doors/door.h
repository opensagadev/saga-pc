#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nuvec.h"

struct game_door_callback_s;

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

/* DOOR_s.flags (door_config_s while loading): the door config keywords that set them. */
#define DOOR_FLAG_ONE_WAY 0x1                /* 1_way, one_way */
#define DOOR_FLAG_CAMERA_LOOK_AT_PLAYERS 0x2 /* cam_lookatplayers */
#define DOOR_FLAG_DO_NOT_USE 0x4             /* do_not_use */
#define DOOR_FLAG_USE_AS_START 0x8           /* use_as_start */
#define DOOR_FLAG_TWO_PLAYERS_ONLY 0x10      /* 2_player_only, two_players_only and spellings */
#ifdef __cplusplus
extern "C" {
#endif
    void Doors_Check(WORLDINFO *, GAMEOBJECT *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern void (*Door_GoThrough_ExtraCodeFn)(WORLDINFO *world, struct game_door_callback_s *door);
#ifdef __cplusplus
}
#endif

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

        struct {                    // size 0x8
            u32 takeover_mask_low;  // 0x108
            u32 takeover_mask_high; // 0x10c
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
