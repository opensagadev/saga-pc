/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/render/fx/edsplines.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#endif

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
    extern NUSPECIALHANDLE *hspecial_efloor;
    extern i32 SarlaccPowerPc;
    extern i32 SarlaccRechargingPc;
    extern i32 SarlaccTargetShieldPc[2];
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
    extern LEVELDATA_PC *LevelFixupSlotPc0087b284;
#ifdef __cplusplus
}
#endif

typedef struct SARLACC_NETPACKET_PC {
    f32 height;
    i16 off_mask;
    i16 flash_mask;
    i16 select_mask;
    i16 on_mask;
    i16 finish_mask;
    i16 sound_mask;
    u8 disco_active;
    u8 reserved_11;
    i16 completion_sound_played;
} SARLACC_NETPACKET_PC;

STATIC_ASSERT(sizeof(SARLACC_NETPACKET_PC) == 0x14);
STATIC_ASSERT(offsetof(SARLACC_NETPACKET_PC, disco_active) == 0x10);
STATIC_ASSERT(offsetof(SARLACC_NETPACKET_PC, completion_sound_played) == 0x12);
#ifdef __cplusplus
extern "C" {
#endif
    extern SARLACC_NETPACKET_PC *sarlaccb_netpacket;
    i32 SarlaccPitDiscoActive(WORLDINFO *);
    void DeathStar2BattleFire_SetSlowDownMul(struct gameobject_s *object);
    extern SPLINEPOS fireBackPos;
    extern f32 fireSpeedScale;
    extern SPLINEPOS fireSplinePos;
    extern f32 runningTotalPos;
    extern i32 fire_clip_dist;
#ifdef __cplusplus
}
#endif
