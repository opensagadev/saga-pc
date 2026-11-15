#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/render/fx/edsplines.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 objhitobj_nohurtsfx;
    extern const char EpisodeVI_obstacle21[11];
    extern const char EpisodeVI_force15[8];
    extern const char EpisodeVI_force16[8];
    extern const char EpisodeVI_lift_force_lnull1[18];
    extern const char EpisodeVI_lift_force_rnull1[18];
    extern const char EpisodeVI_lift_main1[11];
    extern const char EpisodeVI_electric_on[12];
    extern u64 routemask_efloor_on;
    extern u64 routemask_efloor_off;
    extern f32 eFloor_timer;
    extern nuhspecial_s *hspecial_efloor;
    extern i32 SarlaccPower;
    extern i32 SarlaccRecharging;
    extern i32 SarlaccTargetShield[2];
    extern const char EpisodeVI_magnet_drop[21];
    extern const char EpisodeVI_magnet_on[14];
    extern const char EpisodeVI_cannon_1[9];
    extern const char EpisodeVI_cover_format[9];
    extern const char EpisodeVI_cover_gizmo_format[10];
    extern const char EpisodeVI_spinner_format[17];
    extern const char EpisodeVI_doton03[10];
    extern const char EpisodeVI_doton04[10];
    extern const char EpisodeVI_doton05[10];
    extern const char EpisodeVI_doton06[10];
    extern const char EpisodeVI_doton07[10];
    extern const char EpisodeVI_doton08[10];
    extern const char EpisodeVI_doton09[10];
    extern const char EpisodeVI_doton10[10];
    extern const char EpisodeVI_doton11[10];
    extern const char EpisodeVI_doton12[10];
    extern void *emperorfighta_netpacket;
    f32 DeathStar2BattleFire_GetSlowDownMul(GAMEOBJECT *);
    extern LEVELDATA *dat_0087b284;
#ifdef __cplusplus
}
#endif

typedef struct SARLACC_NETPACKET { // size 0x14
    f32 height;                    // 0x00
    i16 off_mask;                  // 0x04
    i16 flash_mask;                // 0x06
    i16 select_mask;               // 0x08
    i16 on_mask;                   // 0x0a
    i16 finish_mask;               // 0x0c
    i16 sound_mask;                // 0x0e
    u8 disco_active;               // 0x10
    u8 reserved_11;                // 0x11
    i16 completion_sound_played;   // 0x12
} SARLACC_NETPACKET;

#ifdef __cplusplus
extern "C" {
#endif
    extern SARLACC_NETPACKET *sarlaccb_netpacket;
    i32 SarlaccPitDiscoActive(WORLDINFO *);
    void DeathStar2BattleFire_SetSlowDownMul(struct gameobject_s *object);
    extern SPLINEPOS fireBackPos;
    extern f32 fireSpeedScale;
    extern SPLINEPOS fireSplinePos;
    extern f32 runningTotalPos;
    extern i32 fire_clip_dist;
    extern i16 dat_007f1a7c;
    extern i16 dat_007f1a88;
#ifdef __cplusplus
}
#endif
