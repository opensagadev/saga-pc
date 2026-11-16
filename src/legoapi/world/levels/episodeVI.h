#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/render/fx/edsplines.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"

/* The emperor fight's level-hack packet (SetLevelHack(8)): the floor state and which players stand on it. */
typedef struct emperor_fight_packet_s { // size 0x8
    i32 state;                          // 0x0
    u8 enabled;                         // 0x4
    u8 zap;                             // 0x5
    i8 player_floor_mask;               // 0x6
    u8 reserved;                        // 0x7
} EMPEROR_FIGHT_PACKET;

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
    extern EMPEROR_FIGHT_PACKET *emperorfighta_netpacket;
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
    void DeathStar2BattleA_AlwaysUpdate(WORLDINFO *);
    void DeathStar2BattleD_Init(WORLDINFO *);
    void DeathStar2BattleD_Update(WORLDINFO *);
    void DeathStar2BattleFire_Init(WORLDINFO *);
    void EmperorFightA_Panel(WORLDINFO *);
    void EndorBattleA_Init(WORLDINFO *);
    void EndorBattleA_Update(WORLDINFO *);
    void FairgroundbadA_Draw(WORLDINFO *);
    void FairgroundbadA_Init(WORLDINFO *);
    void FairgroundbadA_Update(WORLDINFO *);
    void JabbasPalaceA_Init(WORLDINFO *);
    void JabbasPalaceA_Reset(WORLDINFO *);
    void JabbasPalaceA_Update(WORLDINFO *);
    void JabbasPalaceB_Init(WORLDINFO *);
    void JabbasPalaceB_Reset(WORLDINFO *);
    void JabbasPalaceD_Reset(WORLDINFO *);
    void JabbasPalaceE_Init(WORLDINFO *);
    void JabbasPalaceE_Panel(WORLDINFO *);
    void JabbasPalaceE_Reset(WORLDINFO *);
    void JabbasPalaceE_Update(WORLDINFO *);
    void LegoCity_Reset(WORLDINFO *);
    void LegoCity_Update(WORLDINFO *);
    void NewTown_Init(WORLDINFO *);
    void NewTown_Reset(WORLDINFO *);
    void NewTown_Update(WORLDINFO *);
    void SarlaccPitA_Draw(WORLDINFO *);
    void SarlaccPitA_Reset(WORLDINFO *);
    void SarlaccPitB_Init(WORLDINFO *);
    void SarlaccPitB_Reset(WORLDINFO *);
    void SarlaccPitB_Update(WORLDINFO *);
    void SarlaccPitC_Init(WORLDINFO *);
#ifdef __cplusplus
}
#endif
