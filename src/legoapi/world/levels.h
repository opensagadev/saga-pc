/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_WORLD_LEVELS_H_PART
#define SAGA_LEGOAPI_WORLD_LEVELS_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/apitext.h"
#include "legoapi/audio/audio.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/numtx.h"
#endif

struct AIAREA;

/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_WORLD_LEVELS_H_PART == 1 && !defined(SAGA_LEGOAPI_WORLD_LEVELS_H_STOP)
#define AREA_GLOBALS_PROGRESS_FLAGS (AreaGlobals.progress_flags)

typedef struct scene_special_progress_s { // size 0x8

    union { // size 0x4
        u32 flags;

        struct {               // size 0x4
            u32 frame : 11;    // 0x0:0
            u32 visible : 1;   // 0x0:11
            u32 playing : 1;   // 0x0:12
            u32 waiting : 1;   // 0x0:13
            u32 repeating : 1; // 0x0:14
            u32 unused : 17;   // 0x0:15
        };
    }; // 0x0

    f32 tfactor; // 0x4
} SCENESPECIALPROGRESS;

typedef struct scene_progress_s {        // size 0x2800
    SCENESPECIALPROGRESS specials[1280]; // 0x0000
} SCENEPROGRESS;

typedef struct LEVEL_OBJECT_CONFIG { // size 0x8
    u8 kind;                         // 0x0
    u8 unknown_01;                   // 0x1
    i16 reflection;                  // 0x2
    char *name;                      // 0x4
} LEVEL_OBJECT_CONFIG;

#ifdef __cplusplus
extern "C" {
#endif
    /* LevelCompletionFallbackFlagPC: reuse existing canonical u32 declaration. */
    extern const char PlayerBeenDeactivated[16];
    extern i16 VehicleSpecialShooterId;
    void NuWin32SetDFS(i32);
    extern const char LevelAfterLoadName[15];
    extern const char LevelConfigGameKeyword_windsize[9];
    extern const char LevelConfigGameKeyword_windspeed[10];
    extern const char LevelConfigGameKeyword_max_bombgens[13];
    extern const char LevelConfigGameKeyword_max_bombgen_objects[20];
    extern const char LevelConfigGameKeyword_max_plugs[10];
    extern const char LevelConfigGameKeyword_max_bridges[12];
    extern const char LevelConfigGameKeyword_max_torpmachines[17];
    extern const char LevelConfigGameKeyword_max_gizrandoms[15];
    extern const char LevelConfigGameKeyword_max_teleports[14];
    extern const char LevelConfigGameKeyword_max_doors[10];
    extern const char LevelConfigGameKeyword_max_hatmachines[16];
    extern const char LevelConfigGameKeyword_max_gizspecials[16];
    extern const char LevelConfigGameKeyword_max_pushblock_endpos[21];
    extern const char LevelConfigGameKeyword_max_pushblocks[15];
    extern const char LevelConfigGameKeyword_max_force_objects[18];
    extern const char LevelConfigGameKeyword_max_force[10];
    extern const char LevelConfigGameKeyword_max_gizpanels[14];
    extern const char LevelConfigGameKeyword_max_gameantinodes[18];
    extern const char LevelConfigGameKeyword_max_pickups[12];
    extern const char LevelConfigGameKeyword_max_tubes[10];
    extern const char LevelConfigGameKeyword_max_securitydoors[18];
    extern const char LevelConfigGameKeyword_max_ledges[11];
    extern i32 StartupGameFlag0;
    extern i32 StartupGameFlag1;
    extern i32 levspl_i_start;
    extern i32 levspl_i_startcam;
    extern i32 LEVELSPLINECOUNT;
    extern i32 LSW1;
    extern i32 LevSfxFlag[4];
    extern const f32 PCKeyboardDefaultValue;
    extern i32 MAXLDATA;
    extern i32 LevFlag[4];
    extern LEVEL_OBJECT_CONFIG *ObjTabList;
    extern i32 LEVELOBJECTCOUNT;
    extern i32 EXTRALEVELOBJECTCOUNT;
    extern i32 LEVOBJREF_FIRSTOBJ;
    extern i32 LEVOBJREF_LASTOBJ;
    extern i32 LEVOBJREF_FIRSTREFOBJ;
    extern i32 LEVOBJREF_LASTREFOBJ;

    extern const char AICharacterInitialiseName[11];
    extern f32 TargetDist_Mid2;
    extern u16 TargetDeg_Near;
    extern i32 makefreeplaymodellist;
    extern i32 loadareacharacters_loadedlevel;
    extern const char LevelConfigOn[3];
    extern u32 areaSuitBits;
    extern const char SceneFileExtension[5];
    extern i32 nopakfile;
    void ClearAreaProgress(i32, i32);
    extern i32 LSW2;
    void NewArea_ResetGlobalsPC(void);
    void NewArea(void);
    extern const char GizObstacleTriggerLimitMessage[59];

    extern i32 LevPathNodes[8];
    extern struct AIPATHCNX *LevPathCnx[16];
    extern i32 LevSfxId[4];
    extern i32 LevPathCnxDir;
    extern void *LevelLocator;
    extern i32 LevPlatID[2];

    void ClearLevData(void);
    extern i32 KNOBS;
    extern i32 WorldLightUserId;
    extern i32 numtl_force_mipmode;
    extern u64 LevHSpecialExists;
    void LoadAreaCharacters(void);
    extern i32 AreaDataLoaded;
    extern f32 AreaCrawlSoundWaitTime;
    extern f32 AreaCrawlMaximumTime;
    extern i32 AreaCrawlMusicEnabled;
    extern f32 AreaCrawlPromptY;
    extern f32 AreaCrawlPromptScale;
    extern i32 CharacterDataLoad;
    extern i32 loadareadata_loadlevel;
    extern i16 tTOUCHTOSTART;
    extern i32 netnewgame;
    extern i16 dat_007f1a4c;
    extern i16 dat_007f1a48;
    extern i16 dat_007f1a44;
    extern i16 dat_007f1ab0;
    extern i16 dat_007f1ab4;
    extern i16 dat_007f1abc;
    extern i32 FreePlayBonusCount;
    extern i16 PermanentAllTitleId;
    extern i16 PermanentJediTitleId;
    extern i16 PermanentBlasterTitleId;
    extern i16 PermanentBountyTitleId;
    extern i16 PermanentCustomiserActions[5];
    extern f32 LevAlpha;
    extern f32 TitlesAlpha;
    extern f32 LevTime[5];
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_LEVELS_H_PART
#define SAGA_LEGOAPI_WORLD_LEVELS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_LEVELS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_LEVELS_H_PART == 2 && !defined(SAGA_LEGOAPI_WORLD_LEVELS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void LC_BL_mipmapmode(NUFPAR_PC *parser);
    void LC_BL_fix_strobing_anims(NUFPAR_PC *parser);
    void LC_BL_max_ter_groups(NUFPAR_PC *parser);
    void LC_BL_max_ter_platforms(NUFPAR_PC *parser);
    void LC_AL_backr(NUFPAR_PC *parser);
    void LC_AL_backg(NUFPAR_PC *parser);
    void LC_AL_backb(NUFPAR_PC *parser);
    void LC_AL_backr_top(NUFPAR_PC *parser);
    void LC_AL_backg_top(NUFPAR_PC *parser);
    void LC_AL_backb_top(NUFPAR_PC *parser);
    void LC_AL_backr_bottom(NUFPAR_PC *parser);
    void LC_AL_backg_bottom(NUFPAR_PC *parser);
    void LC_AL_backb_bottom(NUFPAR_PC *parser);
    void LC_AL_farclip_hack(NUFPAR_PC *parser);
    void LC_AL_farclip(NUFPAR_PC *parser);
    void LC_AL_nearclip(NUFPAR_PC *parser);
    void LC_AL_hover_height(NUFPAR_PC *parser);
    void LC_AL_blobshadow_alpha(NUFPAR_PC *parser);
    void LC_AL_blobshadow_fadenear(NUFPAR_PC *parser);
    void LC_AL_blobshadow_fadefar(NUFPAR_PC *parser);
    void LC_AL_reflect_range(NUFPAR_PC *parser);
    void LC_AL_reflect_y(NUFPAR_PC *parser);
    void LC_AL_metal(NUFPAR_PC *parser);
    void LC_AL_in_space(NUFPAR_PC *parser);
    void LC_AL_override_nopickupgravity(NUFPAR_PC *parser);
    void LC_AL_hidden_icons(NUFPAR_PC *parser);
    void LC_AL_pickups_to_panel(NUFPAR_PC *parser);
    void LC_AL_forget_takeovers(NUFPAR_PC *parser);
    void LC_AL_narrow_socks(NUFPAR_PC *parser);
    void LC_AL_camera_rain(NUFPAR_PC *parser);
    void LC_AL_terrain_rain(NUFPAR_PC *parser);
    void LC_AL_double_score(NUFPAR_PC *parser);
    void LC_AL_flat_terrain(NUFPAR_PC *parser);
    void LC_AL_cam_tilt(NUFPAR_PC *parser);
    void LC_AL_raycaststep(NUFPAR_PC *parser);
    void LC_AL_plat_scan_dist(NUFPAR_PC *parser);
    void LC_AL_waterripple_startcol_r(NUFPAR_PC *parser);
    void LC_AL_waterripple_startcol_g(NUFPAR_PC *parser);
    void LC_AL_waterripple_startcol_b(NUFPAR_PC *parser);
    void LC_AL_waterripple_startcol_a(NUFPAR_PC *parser);
    void LC_AL_waterripple_endcol_r(NUFPAR_PC *parser);
    void LC_AL_waterripple_endcol_g(NUFPAR_PC *parser);
    void LC_AL_waterripple_endcol_b(NUFPAR_PC *parser);
    void LC_AL_waterripple_endcol_a(NUFPAR_PC *parser);
    void LC_AL_waterripple_life(NUFPAR_PC *parser);
    void LC_AL_cam_pullback_dist(NUFPAR_PC *parser);
    void LC_AL_cam_lateral_dist(NUFPAR_PC *parser);
    void LC_AL_campos_seek(NUFPAR_PC *parser);
    void LC_AL_camang_seek(NUFPAR_PC *parser);
    void LC_AL_shadowtype(NUFPAR_PC *parser);
    void LC_AL_music(NUFPAR_PC *parser);
    void LC_AL_music_other(NUFPAR_PC *parser);
    void LC_AL_sfx_ambient(NUFPAR_PC *parser);
    void LC_AL_conveyor(NUFPAR_PC *parser);
    void LC_BL_max_tightropes(NUFPAR_PC *parser);
    void LC_BL_max_signals(NUFPAR_PC *parser);
    void LC_BL_max_levers(NUFPAR_PC *parser);
    void LC_BL_max_technos(NUFPAR_PC *parser);
    void LC_BL_max_zipups(NUFPAR_PC *parser);
    void LC_BL_max_grapples(NUFPAR_PC *parser);
    void LC_BL_max_obstacles(NUFPAR_PC *parser);
    void LC_BL_max_obstacle_objects(NUFPAR_PC *parser);
    void LC_BL_max_turrets(NUFPAR_PC *parser);
    void LC_BL_max_buildits(NUFPAR_PC *parser);
    void LC_BL_max_buildit_objects(NUFPAR_PC *parser);
    void LC_BL_max_spinneranim_objs(NUFPAR_PC *parser);
    void LC_BL_max_climb_objects(NUFPAR_PC *parser);
    void LC_BL_max_shards(NUFPAR_PC *parser);
    void LC_BL_max_spinners(NUFPAR_PC *parser);
    void LC_BL_max_minicuts(NUFPAR_PC *parser);
    void LC_BL_max_minicutParts(NUFPAR_PC *parser);
    void LC_BL_max_gizmoblowuptypes(NUFPAR_PC *parser);
    void LC_BL_max_gizmoblowups(NUFPAR_PC *parser);
    void LC_BL_max_pushblocks(NUFPAR_PC *parser);
    void LC_BL_max_pushblock_endpos(NUFPAR_PC *parser);
    void LC_BL_max_attractos(NUFPAR_PC *parser);
    void LC_BL_max_timers(NUFPAR_PC *parser);
    void LC_BL_max_guidelines(NUFPAR_PC *parser);
    void LC_BL_max_ledges(NUFPAR_PC *parser);
    void LC_BL_max_securitydoors(NUFPAR_PC *parser);
    void LC_BL_max_tubes(NUFPAR_PC *parser);
    void LC_BL_max_pickups(NUFPAR_PC *parser);
    void LC_BL_max_gameantinodes(NUFPAR_PC *parser);
    void LC_BL_max_gizpanels(NUFPAR_PC *parser);
    void LC_BL_max_hatmachines(NUFPAR_PC *parser);
    void LC_BL_max_force(NUFPAR_PC *parser);
    void LC_BL_max_force_objects(NUFPAR_PC *parser);
    void LC_BL_max_bombgen_objects(NUFPAR_PC *parser);
    void LC_BL_max_bombgens(NUFPAR_PC *parser);
    void LC_BL_max_gizspecials(NUFPAR_PC *parser);
    void LC_BL_max_doors(NUFPAR_PC *parser);
    void LC_BL_max_teleports(NUFPAR_PC *parser);
    void LC_BL_max_gizrandoms(NUFPAR_PC *parser);
    void LC_BL_max_torpmachines(NUFPAR_PC *parser);
    void LC_BL_max_bridges(NUFPAR_PC *parser);
    void LC_BL_max_plugs(NUFPAR_PC *parser);
    void LC_BL_wind_speed(NUFPAR_PC *parser);
    void LC_BL_wind_size(NUFPAR_PC *parser);
    extern NUFPCOMJMP LevelConfig_BeforeLoad_GenericKeywords[5];
    extern NUFPCOMJMP LevelConfig_AfterLoad_GenericKeywords[53];
    extern NUFPCOMJMP *Level_ConfigAfterLoad_GameKeywords;
    extern LEVELSPLINE_PC *LevSplList;
    void LevelSplines_InitForGame(LEVELSPLINE_PC *table);
    extern GIZAIMESSAGE *LevAIMessage[8];
    extern AIAREA *LevArea[4];

    extern NUSPECIALHANDLE LevHSpecial[88];
    void LevelObjects_InitForGame(LEVEL_OBJECT_CONFIG *, VARIPTR *, VARIPTR *, i32, i32);
    extern GIZMO_PC *LevGizmo[12];
    extern GIZMOBLOWUP_PC *LevBlowUp[2];
    extern NUMTL *PermanentEffectsMaterial;
    extern NUMTL *PermanentShadowMaterial;

    extern AREA_EXTRA_MODEL ExtraModelList[6];

    extern struct text_crawl_view_s TextCrawlLsw;
    extern COLLECTION_BUILD_PC JediCollection;
    extern COLLECTION_BUILD_PC BlasterCollection;
    extern COLLECTION_BUILD_PC BountyHunterCollection;
    extern NUMTX LevMtx;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_LEVELS_H_PART
#define SAGA_LEGOAPI_WORLD_LEVELS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_LEVELS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_LEVELS_H_PART == 3 && !defined(SAGA_LEGOAPI_WORLD_LEVELS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern LEVELDATA_PC *CRUISERC_LDATA;
    extern LEVELDATA_PC *GUNSHIPB_LDATA;

    extern LEVELDATA_PC *BOUNTYHUNTERPURSUITE_LDATA;
    extern LEVELDATA_PC *RETAKEB_LDATA;
    extern LEVELDATA_PC *RETAKED_LDATA;
    extern LEVELDATA_PC *RETAKEE_LDATA;
    extern LEVELDATA_PC *MAULA_LDATA;
    extern LEVELDATA_PC *MAULD_LDATA;
    extern LEVELDATA_PC *KAMINOC_LDATA;
    extern LEVELDATA_PC *FACTORYB_LDATA;
    extern LEVELDATA_PC *FACTORYD_LDATA;
    extern LEVELDATA_PC *FACTORYF_LDATA;
    extern LEVELDATA_PC *FACTORYG_LDATA;
    extern LEVELDATA_PC *DOOKUC_LDATA;
    extern LEVELDATA_PC *CRUISERD_LDATA;
    extern LEVELDATA_PC *CRUISERG_LDATA;
    extern LEVELDATA_PC *KASHYYYKD_LDATA;
    extern OPTIONSSAVE TempOptions;
    extern LEVELDATA_PC *KAMINOE_LDATA;
    extern LEVELDATA_PC *PODRACEA_LDATA;
    extern LEVELDATA_PC *BONUS_GUNSHIPB_LDATA;
    extern LEVELDATA_PC *PODSPRINTA_LDATA;
    extern LEVELDATA_PC *PODRACEB_LDATA;
    extern LEVELDATA_PC *BONUS_GUNSHIPA_LDATA;
    extern LEVELDATA_PC *DOGFIGHTA_LDATA;
    extern LEVELDATA_PC *VADERA_LDATA;
    extern AREAFIXUP_PC AreaFixUpLSW[65];
    extern AREAFIXUP_PC AreaFixUpLostTemple[2];
    extern AREAFIXUP_PC AreaFixUpGotham[5];
    extern LEVELDATA_PC *VADERC_LDATA;
    extern LEVELDATA_PC *MAULF_LDATA;
    extern LEVELDATA_PC *GUNSHIPA_LDATA;
    extern LEVELDATA_PC *UpdateSockets_LDATA;
    extern LEVELDATA_PC *ObstacleReturnLevel;
    extern PART_PC *LevGamePart[8];
    extern NUGSPLINE *LevelCodeSpline[8];
    void NewLevelFromMenu(LEVELDATA_PC *, i32, i32, i32);
    extern LEVELDATA_PC *JEDI_B_LDATA;
    extern LEVELDATA_PC *CollisionVehicleExcludedLevel;

    extern LEVELDATA_PC *WorldArchiveOneLevels[11];
    extern LEVELDATA_PC *WorldArchiveThreeLevels[4];

    extern LEVELDATA_PC *dat_0087b0b4;
    extern LEVELDATA_PC *dat_0087b0b8;
    extern LEVELDATA_PC *dat_0087b0bc;
    extern LEVELDATA_PC *dat_0087b0f0;
    extern LEVELDATA_PC *dat_0087b12c;
    extern LEVELDATA_PC *dat_0087b138;
    extern LEVELDATA_PC *dat_0087b140;
    extern LEVELDATA_PC *dat_0087b148;
    extern LEVELDATA_PC *dat_0087b150;
    extern LEVELDATA_PC *dat_0087b154;
    extern LEVELDATA_PC *dat_0087b164;
    extern LEVELDATA_PC *dat_0087b170;
    extern LEVELDATA_PC *dat_0087b17c;
    extern LEVELDATA_PC *dat_0087b194;
    extern LEVELDATA_PC *dat_0087b1c0;
    extern LEVELDATA_PC *dat_0087b200;
    extern LEVELDATA_PC *dat_0087b204;
    extern LEVELDATA_PC *dat_0087b210;
    extern LEVELDATA_PC *dat_0087b218;
    extern LEVELDATA_PC *dat_0087b220;
    extern LEVELDATA_PC *dat_0087b228;
    extern LEVELDATA_PC *dat_0087b258;
    extern LEVELDATA_PC *dat_0087b268;
    extern LEVELDATA_PC *dat_0087b2b0;
    void Titles_Init(WORLDINFO *);
    void Titles_Update(WORLDINFO *);
    void Titles_Draw(WORLDINFO *);
    void Credits_Init_Game(WORLDINFO *);
    void Credits_Update_Game(WORLDINFO *);
    void Credits_Draw_Game(WORLDINFO *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_LEVELS_H_PART
#define SAGA_LEGOAPI_WORLD_LEVELS_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_LEVELS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_LEVELS_H_PART == 4 && !defined(SAGA_LEGOAPI_WORLD_LEVELS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern GAMESAVE BackupGame;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_LEVELS_H_PART
#define SAGA_LEGOAPI_WORLD_LEVELS_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_LEVELS_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_WORLD_LEVELS_H_STOP
