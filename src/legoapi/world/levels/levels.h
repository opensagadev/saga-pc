/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/characters/core/characters.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeI.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"
#endif

#ifdef __cplusplus
extern "C" FadeSystem FadeSys;
#else
extern FADEINFO_PC FadeSys;
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 DrawMiniKitTime;
    extern f32 RedBrickScale;
    extern f32 DrawRedBrickTime;

    extern f32 DrawCoinTotalTime;
    extern f32 BuildUpScale;
    extern f32 CoinTotalScale;
    extern f32 DrawBuildUpTime;
    extern AREA_DATA_PC *PODRACE_ADATA;
    extern AREA_DATA_PC *GUNSHIP_ADATA;
    i32 EffectOffProgress_Update(LEVEL_PROGRESS *progress, char *name, i32 enabled);
    void EffectOffProgress_Reset(LEVEL_PROGRESS *progress);
    void LevelProgress_ReserveBufferSpace(VARIPTR *buffer, VARIPTR buffer_end);
    extern GAMEOBJECT *player;
    void EmperorFightA_Init(WORLDINFO *world);
    void EmperorFightA_Reset(WORLDINFO *);
    void EmperorFightA_Update(WORLDINFO *);
    void SarlaccPitC_Reset(WORLDINFO *);
    void SarlaccPitC_Update(WORLDINFO *);
    extern const char EpisodeVI_liftrlightbon1[17];
    extern const char EpisodeVI_liftrlightbon2[17];
    extern const char EpisodeVI_liftrlightbon3[17];
    extern const char EpisodeVI_liftrlightbon4[17];
    extern const char EpisodeVI_liftrarron1[14];
    extern const char EpisodeVI_liftrarron2[14];
    extern const char EpisodeVI_liftrarron3[14];
    extern const char EpisodeVI_liftrarron4[14];
    extern const char EpisodeVI_doton01[10];
    extern const char EpisodeVI_doton02[10];
    void StoreSceneProgress(NUGSCN *scene, SCENEPROGRESS *progress, i32 reset_factor);
    extern i16 id_ANAKINPADAWAN;

    /* The u16 after the buffer at 0x93b0e8 is the separate global temp_yrot. */
    typedef struct CONFIG_BUFFER_PC {
        char bytes[0x10000];
    } CONFIG_BUFFER_PC;

    STATIC_ASSERT(sizeof(CONFIG_BUFFER_PC) == 0x10000);
    extern CONFIG_BUFFER_PC ConfigBuffer;
    extern GAMEOBJECT *player2;
    extern i32 nethost;
    extern f32 avg_currentspeed_mul;
    extern i32 podrace_section;
    extern f32 pod_roll[2];
    extern f32 pod_roll_target[2];
    extern const char AICombatLastAttackerName[14];
    extern const char AICombatOpponentTypeName[13];
    extern AREA_DATA_PC *JEDI_ADATA;
    extern AREA_DATA_PC *DOOKU_ADATA;
    extern PODSPRINT_PC podsprint;
    extern u16 temp_xrot;
    extern u16 temp_zrot;
    extern AREA_DATA_PC *GUNSHIP_ADATA;
    extern f32 PANEL3DMULX;
    extern GIZAIMESSAGESYS *gizaimessagesys;
    extern GAMECAMERA *GameCam;
    extern i32 MiniCutCam;
    extern i32 pause_rndr_on;
    extern i32 Paused;
    extern i32 hub_from_arcade;
    extern i32 bonus_gunship_store_progress_flag;
    extern i32 bonus_gunship_store_progress_flag;
// NOLINTNEXTLINE(readability-identifier-naming)
#define MiscTime LevInstAnimResetPc.fields.miscellaneous_time
    extern i32 hub_from_superstory;
    extern i32 hub_from_mission;
    extern i32 hub_startoutsidebonusdoor_area;
    extern i32 active_neutral_count;
    extern i32 active_baddy_count;
    extern i32 PARTPAGE_GENERAL;
    void LoadPerm1(void);
    void DeathStarEscapeB_Init(WORLDINFO *);
    void DeathStarEscapeB_Update(WORLDINFO *);

    /* Shared declarations only. Hub/Episode minimum local readers remain original
     * owners. Table and state declarations await the sole-writer canonical gate. */
    void Hub_Init(WORLDINFO *);
    void Hub_Reset(WORLDINFO *);
    void Hub_InitMiniKits(WORLDINFO *);
    void Hub_DrawMiniKits(WORLDINFO *);
    void Hub_Draw3D(WORLDINFO *);

    void MakeFreePlayModelList(i32, i32, i32, i32, i32);

    void LoadPerm2(void);
    extern APICHARACTERMODELLIST PermModelListPc[1];
    extern f32 CreditsTime;
    extern f32 CreditsFinishedTime;
    extern i32 CreditsFlag;
    extern f32 CreditsAlpha;
    void Credits_Init(WORLDINFO *);
    void Credits_GetInfo(f32 *, i32 *, f32 *);
    void Credits_DrawPanel(WORLDINFO *);
    extern f32 pod_animtime[2];
    extern f32 podanimendframe;
    extern f32 pod_animtime[2];
    extern f32 podanimendframe;
    void ResetScene(NUGSCN *scene, SCENEPROGRESS *progress);
    extern f32 cointotaltime;
    extern f32 goldbricktime;
    extern f32 statstime;
    extern void *hothtroopers;
    extern i8 trooper_side[10];
    extern i32 LevForce;
    extern i32 PodRace_nsnipers;
    extern f32 PodRace_sniper_fire_time;
    extern SNIPER_s PodRace_snipers[10];
    extern void *kaminoe_netpacket;
    extern struct PODSPRINTNETPACKET_s *podsprint_netpacket;
    extern u16 temp_yrot;
    extern f32 PodRace_sniper_fire_radius;
    extern f32 PodRace_sniper_fire_range_time;
    extern f32 PodRace_sniper_start_fire_radius;
    extern struct GIZFORCE_s *force_array[4];
    struct GIZMOBLOWUP_PC *DeathStar2BattleD_InZapRange(struct gameobject_s *object);
    extern void *factoryb_netpacket;
    void Hub_UpdateMiniKits(WORLDINFO *world);
    void Hub_UpdateMiniKits(WORLDINFO *);
    void Hub_Update(WORLDINFO *);
    void Hub_Load(WORLDINFO *, VARIPTR *, VARIPTR *);
    void Hub_DrawImportantBrick(i32, f32, f32, f32, i32, i32);

    void Hub_DrawPanel(WORLDINFO *);
    void SpeederChaseA_Init(WORLDINFO *);
    void SpeederChaseA_Reset(WORLDINFO *);
    void SpeederChaseA_Update(WORLDINFO *);
    void SpeederChaseA_Panel(WORLDINFO *);
    void JediB_Init(WORLDINFO *);
    void JediB_Reset(WORLDINFO *);
    void JediB_Update(WORLDINFO *);
#ifdef __cplusplus
}
#endif
