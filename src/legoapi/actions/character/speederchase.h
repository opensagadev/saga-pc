#ifndef SAGA_LEGOAPI_ACTIONS_CHARACTER_SPEEDERCHASE_H
#define SAGA_LEGOAPI_ACTIONS_CHARACTER_SPEEDERCHASE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/items/objects/game_obj.h (not all of it
 * earlier in the shim). */

#include "legoapi/gizmo/object/gizmoblowups.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 ObjOpponentStillThere(GAMEOBJECT *, GAMEOBJECT *, f32);
    extern i32 test_ai_combo;
    extern i32 CanPunchGirls;
    extern i32 objopponent_ignoreaiopponent;
    void KillParts_SpeederBike(ADDPART_PC *, i32, i32, GAMEOBJECT *);
    i32 ObjIsTargetSpeeder(GAMEOBJECT *object);
    f32 SpeederChaseATATInOutMul(NUVEC *, NUVEC *);
#ifdef __cplusplus
}
#endif
typedef struct SPEEDERCHASEVALUES SPEEDERCHASEVALUES;

struct SPEEDERCHASEVALUES {
    f32 distance;
    f32 ahead_range;
    f32 ahead_speed;
    f32 ahead_seek;
    f32 behind_range;
    f32 behind_speed;
    f32 behind_seek;
};

STATIC_ASSERT(sizeof(struct SPEEDERCHASEVALUES) == 28);
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

typedef struct SPEEDERCHASE_NETPACKET_PC {
    u8 reserved_0;
    u8 target_count;
    u8 killed_count;
    u8 reserved_3;
    u8 phase;
    u8 reserved_5[2];
} SPEEDERCHASE_NETPACKET_PC;

STATIC_ASSERT(sizeof(SPEEDERCHASE_NETPACKET_PC) == 7);
STATIC_ASSERT(offsetof(SPEEDERCHASE_NETPACKET_PC, target_count) == 1);
STATIC_ASSERT(offsetof(SPEEDERCHASE_NETPACKET_PC, killed_count) == 2);
STATIC_ASSERT(offsetof(SPEEDERCHASE_NETPACKET_PC, phase) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    extern SPEEDERCHASE_NETPACKET_PC *SpeederChaseNetPacketPc;
    extern i32 SpeederChaseState0Pc;
    extern i32 SpeederChaseState1Pc;
    extern f32 SpeederChaseTime0Pc;
    extern f32 SpeederChaseTime1Pc;
    i32 SpeederBlowupHack(GIZMOBLOWUP_PC *, i32);
#ifdef __cplusplus
}
#endif

#endif
