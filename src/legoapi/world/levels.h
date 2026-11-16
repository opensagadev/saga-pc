#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/apitext.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/world/area.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/numtx.h"

struct AIAREA;
struct WORLDINFO_s;
struct gizaimessage_s;
struct gizmo_s;
struct level_data_s;
struct levelspline_s;
struct textcrawl_s;

#define AREA_GLOBALS_PROGRESS_FLAGS (AreaGlobals.progress_flags)

typedef struct LEVEL_OBJECT_CONFIG { // size 0x8
    u8 kind;                         // 0x0
    u8 unknown_01;                   // 0x1
    i16 reflection;                  // 0x2
    char *name;                      // 0x4
} LEVEL_OBJECT_CONFIG;

/* LEVFLAG.podrace.state: the pod race's one-off step (B: the hint shown; C: the boosts shown after the
 * Tusken Raider cut scene). */
#define PODRACE_STEP_START 0
#define PODRACE_STEP_TRIGGERED 1
#define PODRACE_STEP_DONE 2

/* LEVFLAG.podrace.mushroom_state: the collapsing mushroom on lap two. */
#define PODRACE_MUSHROOM_APPROACH 0  /* until the camera passes mushroom0_along: the first cut scene */
#define PODRACE_MUSHROOM_FALLING 1   /* until that cut scene ends: the second, and the countdown starts */
#define PODRACE_MUSHROOM_COUNTDOWN 2 /* until it runs out (the third cut scene) or the camera passes */
#define PODRACE_MUSHROOM_PASSED 3    /* until the mushroom is out of view */
#define PODRACE_MUSHROOM_DONE 4

/* LEVFLAG.gunship.progress */
#define GUNSHIP_PROGRESS_START 0
#define GUNSHIP_PROGRESS_MIDWAY 1 /* A: the midway checkpoint stored; B: the countdown runs */
#define GUNSHIP_PROGRESS_LATE 2   /* A: restarted from the checkpoint; B: the countdown is over */

/* LEVFLAG.cruiser.starfighter_state: the two starfighters of Cruiser A. */
#define CRUISER_STARFIGHTERS_HIDDEN 0
#define CRUISER_STARFIGHTERS_SHOW 1
#define CRUISER_STARFIGHTERS_SHOWN 2

/* LevFlag: sixteen bytes of state that the current level's own code keeps, each level its own way. */
typedef union levflag_u { // size 0x10
    i32 words[4];
    u8 bytes[16];

    struct {               // size 0x2
        u8 state;          // 0x0
        u8 mushroom_state; // 0x1
    } podrace;

    struct {          // size 0x2
        u8 progress;  // 0x0
        u8 secondary; // 0x1
    } gunship;

    struct {                  // size 0x1
        u8 starfighter_state; // 0x0
    } cruiser;

    struct {        // size 0x1
        u8 started; // 0x0
    } cloud_city;

    struct {               // size 0x9
        u8 first;          // 0x0
        u8 light_flags[4]; // 0x1
        u8 arrow_flags[4]; // 0x5
    } emperor_fight;
} LEVFLAG;

#ifdef __cplusplus
extern "C" {
#endif
    extern LEVEL_OBJECT_CONFIG ObjTab[750];

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
    extern LEVFLAG LevFlag;
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

#ifdef __cplusplus
extern "C" {
#endif
    void LC_BL_mipmapmode(NUFPAR *parser);
    void LC_BL_fix_strobing_anims(NUFPAR *parser);
    void LC_BL_max_ter_groups(NUFPAR *parser);
    void LC_BL_max_ter_platforms(NUFPAR *parser);
    void LC_AL_backr(NUFPAR *parser);
    void LC_AL_backg(NUFPAR *parser);
    void LC_AL_backb(NUFPAR *parser);
    void LC_AL_backr_top(NUFPAR *parser);
    void LC_AL_backg_top(NUFPAR *parser);
    void LC_AL_backb_top(NUFPAR *parser);
    void LC_AL_backr_bottom(NUFPAR *parser);
    void LC_AL_backg_bottom(NUFPAR *parser);
    void LC_AL_backb_bottom(NUFPAR *parser);
    void LC_AL_farclip_hack(NUFPAR *parser);
    void LC_AL_farclip(NUFPAR *parser);
    void LC_AL_nearclip(NUFPAR *parser);
    void LC_AL_hover_height(NUFPAR *parser);
    void LC_AL_blobshadow_alpha(NUFPAR *parser);
    void LC_AL_blobshadow_fadenear(NUFPAR *parser);
    void LC_AL_blobshadow_fadefar(NUFPAR *parser);
    void LC_AL_reflect_range(NUFPAR *parser);
    void LC_AL_reflect_y(NUFPAR *parser);
    void LC_AL_metal(NUFPAR *parser);
    void LC_AL_in_space(NUFPAR *parser);
    void LC_AL_override_nopickupgravity(NUFPAR *parser);
    void LC_AL_hidden_icons(NUFPAR *parser);
    void LC_AL_pickups_to_panel(NUFPAR *parser);
    void LC_AL_forget_takeovers(NUFPAR *parser);
    void LC_AL_narrow_socks(NUFPAR *parser);
    void LC_AL_camera_rain(NUFPAR *parser);
    void LC_AL_terrain_rain(NUFPAR *parser);
    void LC_AL_double_score(NUFPAR *parser);
    void LC_AL_flat_terrain(NUFPAR *parser);
    void LC_AL_cam_tilt(NUFPAR *parser);
    void LC_AL_raycaststep(NUFPAR *parser);
    void LC_AL_plat_scan_dist(NUFPAR *parser);
    void LC_AL_waterripple_startcol_r(NUFPAR *parser);
    void LC_AL_waterripple_startcol_g(NUFPAR *parser);
    void LC_AL_waterripple_startcol_b(NUFPAR *parser);
    void LC_AL_waterripple_startcol_a(NUFPAR *parser);
    void LC_AL_waterripple_endcol_r(NUFPAR *parser);
    void LC_AL_waterripple_endcol_g(NUFPAR *parser);
    void LC_AL_waterripple_endcol_b(NUFPAR *parser);
    void LC_AL_waterripple_endcol_a(NUFPAR *parser);
    void LC_AL_waterripple_life(NUFPAR *parser);
    void LC_AL_cam_pullback_dist(NUFPAR *parser);
    void LC_AL_cam_lateral_dist(NUFPAR *parser);
    void LC_AL_campos_seek(NUFPAR *parser);
    void LC_AL_camang_seek(NUFPAR *parser);
    void LC_AL_shadowtype(NUFPAR *parser);
    void LC_AL_music(NUFPAR *parser);
    void LC_AL_music_other(NUFPAR *parser);
    void LC_AL_sfx_ambient(NUFPAR *parser);
    void LC_AL_conveyor(NUFPAR *parser);
    void LC_BL_max_tightropes(NUFPAR *parser);
    void LC_BL_max_signals(NUFPAR *parser);
    void LC_BL_max_levers(NUFPAR *parser);
    void LC_BL_max_technos(NUFPAR *parser);
    void LC_BL_max_zipups(NUFPAR *parser);
    void LC_BL_max_grapples(NUFPAR *parser);
    void LC_BL_max_obstacles(NUFPAR *parser);
    void LC_BL_max_obstacle_objects(NUFPAR *parser);
    void LC_BL_max_turrets(NUFPAR *parser);
    void LC_BL_max_buildits(NUFPAR *parser);
    void LC_BL_max_buildit_objects(NUFPAR *parser);
    void LC_BL_max_spinneranim_objs(NUFPAR *parser);
    void LC_BL_max_climb_objects(NUFPAR *parser);
    void LC_BL_max_shards(NUFPAR *parser);
    void LC_BL_max_spinners(NUFPAR *parser);
    void LC_BL_max_minicuts(NUFPAR *parser);
    void LC_BL_max_minicutParts(NUFPAR *parser);
    void LC_BL_max_gizmoblowuptypes(NUFPAR *parser);
    void LC_BL_max_gizmoblowups(NUFPAR *parser);
    void LC_BL_max_pushblocks(NUFPAR *parser);
    void LC_BL_max_pushblock_endpos(NUFPAR *parser);
    void LC_BL_max_attractos(NUFPAR *parser);
    void LC_BL_max_timers(NUFPAR *parser);
    void LC_BL_max_guidelines(NUFPAR *parser);
    void LC_BL_max_ledges(NUFPAR *parser);
    void LC_BL_max_securitydoors(NUFPAR *parser);
    void LC_BL_max_tubes(NUFPAR *parser);
    void LC_BL_max_pickups(NUFPAR *parser);
    void LC_BL_max_gameantinodes(NUFPAR *parser);
    void LC_BL_max_gizpanels(NUFPAR *parser);
    void LC_BL_max_hatmachines(NUFPAR *parser);
    void LC_BL_max_force(NUFPAR *parser);
    void LC_BL_max_force_objects(NUFPAR *parser);
    void LC_BL_max_bombgen_objects(NUFPAR *parser);
    void LC_BL_max_bombgens(NUFPAR *parser);
    void LC_BL_max_gizspecials(NUFPAR *parser);
    void LC_BL_max_doors(NUFPAR *parser);
    void LC_BL_max_teleports(NUFPAR *parser);
    void LC_BL_max_gizrandoms(NUFPAR *parser);
    void LC_BL_max_torpmachines(NUFPAR *parser);
    void LC_BL_max_bridges(NUFPAR *parser);
    void LC_BL_max_plugs(NUFPAR *parser);
    void LC_BL_wind_speed(NUFPAR *parser);
    void LC_BL_wind_size(NUFPAR *parser);
    extern NUFPCOMJMP LevelConfig_BeforeLoad_GenericKeywords[5];
    extern NUFPCOMJMP LevelConfig_AfterLoad_GenericKeywords[53];
    extern NUFPCOMJMP *Level_ConfigAfterLoad_GameKeywords;
    extern struct levelspline_s *LevSplList;
    void LevelSplines_InitForGame(struct levelspline_s *table);
    extern struct gizaimessage_s *LevAIMessage[8];
    extern AIAREA *LevArea[4];

    extern nuhspecial_s LevHSpecial[88];
    void LevelObjects_InitForGame(LEVEL_OBJECT_CONFIG *, VARIPTR *, VARIPTR *, i32, i32);
    extern struct gizmo_s *LevGizmo[12];
    extern GIZMOBLOWUP *LevBlowUp[2];
    extern NUMTL *PermanentEffectsMaterial;
    extern NUMTL *PermanentShadowMaterial;

    extern struct textcrawl_s TextCrawlLsw;
    extern NUMTX LevMtx;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern struct level_data_s *CRUISERC_LDATA;
    extern struct level_data_s *GUNSHIPB_LDATA;

    extern struct level_data_s *BOUNTYHUNTERPURSUITE_LDATA;
    extern struct level_data_s *RETAKEB_LDATA;
    extern struct level_data_s *RETAKED_LDATA;
    extern struct level_data_s *RETAKEE_LDATA;
    extern struct level_data_s *MAULA_LDATA;
    extern struct level_data_s *MAULD_LDATA;
    extern struct level_data_s *KAMINOC_LDATA;
    extern struct level_data_s *FACTORYB_LDATA;
    extern struct level_data_s *FACTORYD_LDATA;
    extern struct level_data_s *FACTORYF_LDATA;
    extern struct level_data_s *FACTORYG_LDATA;
    extern struct level_data_s *DOOKUC_LDATA;
    extern struct level_data_s *CRUISERD_LDATA;
    extern struct level_data_s *CRUISERG_LDATA;
    extern struct level_data_s *KASHYYYKD_LDATA;
    extern struct level_data_s *KAMINOE_LDATA;
    extern struct level_data_s *PODRACEA_LDATA;
    extern struct level_data_s *BONUS_GUNSHIPB_LDATA;
    extern struct level_data_s *PODSPRINTA_LDATA;
    extern struct level_data_s *PODRACEB_LDATA;
    extern struct level_data_s *BONUS_GUNSHIPA_LDATA;
    extern struct level_data_s *DOGFIGHTA_LDATA;
    extern struct level_data_s *VADERA_LDATA;
    extern AREAFIXUP AreaFixUpLSW[65];
    extern AREAFIXUP AreaFixUpLostTemple[2];
    extern AREAFIXUP AreaFixUpGotham[5];
    extern struct level_data_s *VADERC_LDATA;
    extern struct level_data_s *MAULF_LDATA;
    extern struct level_data_s *GUNSHIPA_LDATA;
    extern struct level_data_s *UpdateSockets_LDATA;
    extern struct level_data_s *ObstacleReturnLevel;
    extern PART *LevGamePart[8];
    extern NUGSPLINE *LevelCodeSpline[8];
    void NewLevelFromMenu(struct level_data_s *, i32, i32, i32);
    extern struct level_data_s *JEDI_B_LDATA;
    extern struct level_data_s *CollisionVehicleExcludedLevel;

    extern struct level_data_s *WorldArchiveOneLevels[11];
    extern struct level_data_s *WorldArchiveThreeLevels[4];

    extern struct level_data_s *dat_0087b0b4;
    extern struct level_data_s *dat_0087b0b8;
    extern struct level_data_s *dat_0087b0bc;
    extern struct level_data_s *dat_0087b0f0;
    extern struct level_data_s *dat_0087b12c;
    extern struct level_data_s *dat_0087b138;
    extern struct level_data_s *dat_0087b140;
    extern struct level_data_s *dat_0087b148;
    extern struct level_data_s *dat_0087b150;
    extern struct level_data_s *dat_0087b154;
    extern struct level_data_s *dat_0087b164;
    extern struct level_data_s *dat_0087b170;
    extern struct level_data_s *dat_0087b17c;
    extern struct level_data_s *dat_0087b194;
    extern struct level_data_s *dat_0087b1c0;
    extern struct level_data_s *dat_0087b200;
    extern struct level_data_s *dat_0087b204;
    extern struct level_data_s *dat_0087b210;
    extern struct level_data_s *dat_0087b218;
    extern struct level_data_s *dat_0087b220;
    extern struct level_data_s *dat_0087b228;
    extern struct level_data_s *dat_0087b258;
    extern struct level_data_s *dat_0087b268;
    extern struct level_data_s *dat_0087b2b0;
    void Titles_Init(struct WORLDINFO_s *);
    void Titles_Update(struct WORLDINFO_s *);
    void Titles_Draw(struct WORLDINFO_s *);
    void Credits_Init_Game(struct WORLDINFO_s *);
    void Credits_Update_Game(struct WORLDINFO_s *);
    void Credits_Draw_Game(struct WORLDINFO_s *);
    extern char AreaFixUpName_Gungan[8];
    void KillBossNewLevel(i32 id, i32 unused_4, f32 delay, i32 level_index);
    void ClearUpAreaData(void);
#ifdef __cplusplus
}
#endif
