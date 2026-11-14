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

/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_WORLD_LEVELS_H_PART == 1 && !defined(SAGA_LEGOAPI_WORLD_LEVELS_H_STOP)
#define AREA_GLOBALS_PROGRESS_FLAGS_PC (AreaGlobals_PC.progress_flags)

typedef struct scene_special_progress_pc_s {
    union {
        u32 flags;

        struct {
            u32 frame : 11;
            u32 visible : 1;
            u32 playing : 1;
            u32 waiting : 1;
            u32 repeating : 1;
            u32 unused : 17;
        };
    };

    f32 tfactor;
} SCENESPECIALPROGRESS_PC;

STATIC_ASSERT(sizeof(SCENESPECIALPROGRESS_PC) == 8);
STATIC_ASSERT(offsetof(SCENESPECIALPROGRESS_PC, tfactor) == 4);

typedef struct scene_progress_pc_s {
    SCENESPECIALPROGRESS_PC specials[1280];
} SCENEPROGRESS;

STATIC_ASSERT(sizeof(SCENEPROGRESS) == 0x2800);

typedef struct LEVEL_OBJECT_CONFIG_PC {
    u8 kind;
    u8 unknown_01;
    i16 reflection;
    char *name;
} LEVEL_OBJECT_CONFIG_PC;

STATIC_ASSERT(offsetof(LEVEL_OBJECT_CONFIG_PC, reflection) == 0x2);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_CONFIG_PC) == 8);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_CONFIG_PC, kind) == 0);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_CONFIG_PC, name) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    /* LevelCompletionFallbackFlagPC: reuse existing canonical u32 declaration. */
    extern const char PlayerBeenDeactivatedPc[16];
    extern i16 VehicleSpecialShooterIdPc;
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
    extern LEVEL_OBJECT_CONFIG_PC *ObjTabList;
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
    extern const char SceneFileExtensionPc[5];
    extern i32 nopakfile;
    void ClearAreaProgress(i32, i32);
    extern i32 LSW2;
    void NewArea_ResetGlobalsPC(void);
    void NewArea(void);
    extern const char GizObstacleTriggerLimitMessagePc[59];

    extern i32 LevPathNodes[8];
    extern void *LevPathCnx[16];
    extern i32 LevSfxId[4];
    extern i32 LevPathCnxDir;
    extern void *LevelLocator;
    extern i32 LevPlatID[2];

    void ClearLevData(void);
    extern i32 KNOBS;
    extern i32 WorldLightUserIdPc;
    extern i32 numtl_force_mipmode;
    extern u64 LevHSpecialExists;
    void LoadAreaCharacters(void);
    extern i32 AreaDataLoaded;
    extern f32 AreaCrawlSoundWaitTimePc;
    extern f32 AreaCrawlMaximumTimePc;
    extern i32 AreaCrawlMusicEnabledPc;
    extern f32 AreaCrawlPromptYPc;
    extern f32 AreaCrawlPromptScalePc;
    extern i32 CharacterDataLoad;
    extern i32 loadareadata_loadlevel;
    extern i16 tTOUCHTOSTART;
    extern i32 netnewgame;
    extern i16 FreePlayId007f1a4cPc;
    extern i16 FreePlayId007f1a48Pc;
    extern i16 FreePlayId007f1a44Pc;
    extern i16 FreePlayId007f1ab0Pc;
    extern i16 FreePlayId007f1ab4Pc;
    extern i16 FreePlayId007f1abcPc;
    extern i32 FreePlayBonusCount;
    extern i16 PermanentAllTitleIdPc;
    extern i16 PermanentJediTitleIdPc;
    extern i16 PermanentBlasterTitleIdPc;
    extern i16 PermanentBountyTitleIdPc;
    extern i16 PermanentCustomiserActionsPc[5];
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
    extern AIAREA_PC *LevArea[4];

    extern NUSPECIALHANDLE LevHSpecial[88];
    void LevelObjects_InitForGame(LEVEL_OBJECT_CONFIG_PC *, VARIPTR *, VARIPTR *, i32, i32);
    extern GIZMO_PC *LevGizmo[12];
    extern GIZMOBLOWUP_PC *LevBlowUp[2];
    extern NUMTL *PermanentEffectsMaterialPc;
    extern NUMTL *PermanentShadowMaterialPc;

    extern AREA_EXTRA_MODEL_PC ExtraModelList[6];

    extern struct text_crawl_view_pc_s TextCrawlLswPc;
    extern COLLECTION_BUILD_PC JediCollectionPc;
    extern COLLECTION_BUILD_PC BlasterCollectionPc;
    extern COLLECTION_BUILD_PC BountyHunterCollectionPc;
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
    extern LEVELDATA_PC *ObstacleReturnLevelPc;
    extern PART_PC *LevGamePart[8];
    extern NUGSPLINE *LevelCodeSpline[8];
    void NewLevelFromMenu(LEVELDATA_PC *, i32, i32, i32);
    extern LEVELDATA_PC *JEDI_B_LDATA;
    extern LEVELDATA_PC *CollisionVehicleExcludedLevelPc;

    extern LEVELDATA_PC *WorldArchiveOneLevelsPc[11];
    extern LEVELDATA_PC *WorldArchiveThreeLevelsPc[4];

    extern LEVELDATA_PC *LevelFixupSlotPc0087b0b4;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b0b8;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b0bc;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b0f0;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b12c;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b138;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b140;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b148;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b150;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b154;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b164;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b170;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b17c;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b194;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1c0;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b200;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b204;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b210;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b218;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b220;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b228;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b258;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b268;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b2b0;
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
