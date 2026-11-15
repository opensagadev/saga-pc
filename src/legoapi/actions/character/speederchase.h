#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 ObjOpponentStillThere(GAMEOBJECT *, GAMEOBJECT *, f32);
    extern i32 test_ai_combo;
    extern i32 CanPunchGirls;
    extern i32 objopponent_ignoreaiopponent;
    void KillParts_SpeederBike(ADDPART *, i32, i32, GAMEOBJECT *);
    i32 ObjIsTargetSpeeder(GAMEOBJECT *object);
    f32 SpeederChaseATATInOutMul(NUVEC *, NUVEC *);
#ifdef __cplusplus
}
#endif
typedef struct SPEEDERCHASEVALUES SPEEDERCHASEVALUES;

struct SPEEDERCHASEVALUES { // size 0x1c
    f32 distance;           // 0x00
    f32 ahead_range;        // 0x04
    f32 ahead_speed;        // 0x08
    f32 ahead_seek;         // 0x0c
    f32 behind_range;       // 0x10
    f32 behind_speed;       // 0x14
    f32 behind_seek;        // 0x18
};

#ifdef __cplusplus
extern "C" {
#endif
    extern u8 troopercannons_beenReset;
    void InitBikeParts(void);
    extern i32 players_going_forward;
    extern i32 set_speedermode;
    extern f32 speeder_ahead_time;
    extern i32 speeder_hitpoints_lost;
    extern f32 speeder_kill_dist;
    extern f32 speeder_level_height;
    extern f32 speeder_level_height_lerpf;
    extern f32 speeder_midrange_time;
    extern f32 speeder_mode_ahead_timer;
    extern f32 speeder_shootrate;
    extern f32 speederfirerange;
    extern SPEEDERCHASEVALUES speedervals[4];
    extern f32 zoom_ahead_extra;
    extern f32 zoom_ahead_extra_time;
#ifdef __cplusplus
}
#endif

typedef struct SPEEDERCHASE_NETPACKET { // size 0x7
    u8 reserved_0;                      // 0x0
    u8 target_count;                    // 0x1
    u8 killed_count;                    // 0x2
    u8 reserved_3;                      // 0x3
    u8 phase;                           // 0x4
    u8 reserved_5[2];                   // 0x5
} SPEEDERCHASE_NETPACKET;

#ifdef __cplusplus
extern "C" {
#endif
    extern SPEEDERCHASE_NETPACKET *SpeederChaseNetPacket;
    extern i32 SpeederChaseState0;
    extern i32 SpeederChaseState1;
    extern f32 SpeederChaseTime0;
    extern f32 SpeederChaseTime1;
    i32 SpeederBlowupHack(GIZMOBLOWUP_PC *, i32);
#ifdef __cplusplus
}
#endif
