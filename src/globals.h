#ifndef SAGA_GLOBALS_H_PART
#define SAGA_GLOBALS_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gamelib/util/Refpack.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/input/timing.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/worldinfo.h"
#include "legogame/game.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 5 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_GLOBALS_H_PART == 1 && !defined(SAGA_GLOBALS_H_STOP)

struct CHARACTER_MODEL_LAYERS_PC;
struct CUSTOMISER_LAYERS_PC;
struct WORLDINFO_s;
struct area_data_pc_s;
struct collection_area_save_pc_s;
struct gameobject_s;
struct level_data_pc_s;
struct level_progress_pc_s;
struct nucamera_s;
struct nufpcomjmp_s;
struct nugspline_s;
struct vufnt_s;

typedef struct terrain_layer_pc_s {
    u32 flags;
    u32 unknown_04;
    u32 unknown_08;
} TERRAIN_LAYER_PC;

STATIC_ASSERT(sizeof(TERRAIN_LAYER_PC) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_LAYER_PC, flags) == 0);
STATIC_ASSERT(offsetof(TERRAIN_LAYER_PC, unknown_04) == 4);
STATIC_ASSERT(offsetof(TERRAIN_LAYER_PC, unknown_08) == 8);

struct EPISODESAVE_s {
    f32 superstory_time_limit;
    i32 superstory_score_target;

    union {
        u32 flags;

        struct {
            u8 superstory_complete;
            u8 reserved_0x9[3];
        };
    };
};

STATIC_ASSERT(sizeof(struct EPISODESAVE_s) == 12);
STATIC_ASSERT(offsetof(struct EPISODESAVE_s, superstory_time_limit) == 0);
STATIC_ASSERT(offsetof(struct EPISODESAVE_s, superstory_score_target) == 4);
STATIC_ASSERT(offsetof(struct EPISODESAVE_s, superstory_complete) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 ObstacleCamHoldUntilPlayersMove;
    extern f32 builduptime;
    extern i32 WeaponInOut_NoAIJediSfx;
    extern u8 *Game_CharacterSave;
    extern i32 drawcharicon_i_panel;
    extern i32 drawcharicon_find;
    extern void *CutStopInfo;
    extern i32 (*MatrixReflection_CanOverrideFn)(void);
    extern f32 PUNCHGAP;
    extern f32 PUNCHCHARGAP;
    extern f32 MiniKitScale;
    extern f32 STATSPOSY;
    extern f32 AreaPickupGravity;
    extern f32 PANEL_COINX;
    extern f32 PANEL_COINY;
    extern f32 PANEL_COINSCALE_END;
    extern f32 STATSPOSY;
    extern u32 BonusCoinTarget;
    extern i32 BonusWinFlag;
    extern f32 cointotal_x[2];
    extern f32 DoubleScoreTime;
    extern i32 LookAtBoth;
    extern u32 SuperStoryScore;
    extern f32 AreaPickupGravity;
    extern i32 menu_flash;
    extern i32 MenuLoadOccurred;
    extern i32 BGLOAD;
    extern i32 LOADEROFF;
    extern void *OldLevelHackData;
    extern void *LevelHackData;
    extern i16 id_DEFAULTCHARACTER[2];
    extern MEMORYMANAGER theMemoryManager;
    extern i32 PAL;
    extern i32 finishloop_backdroponly;
    extern i32 EPISODECOUNT;
    extern f32 EShadY;
    extern f32 WaitingForLevelTime;
    extern f32 MAXFRAMETIME;
    extern i32 NOSOUND;
    extern i32 CHARPAK;
    typedef void (*CHECK_LOST_DATA_PC)(void *);
    extern i32 ShadowMode;
    extern CHECK_LOST_DATA_PC CheckLostDataFn;
    extern i32 LevSafePlatID[2];
    extern i32 CUTSKIPLOCK;
    extern i32 BonusArea;
    extern u8 aicreature_sets_alive[16];
    extern f32 drop_back_in_timer;
    extern f32 OFFSCREEN_CATCHUP_TIME;
    extern f32 DEFAULT_MOVE_RANGE;
    extern i32 disable_narrow_socks;
    extern i32 MAXPARTS;
    extern TIMER GameTimer;

    extern TIMER GamePlayTimer;

    extern i32 GAMERAND;
    extern f32 PANEL3DMULY;
    extern char **TTab;
    extern char *txt_UNKNOWN;
    extern f32 ObstacleCamStart;
    extern f32 ObstacleCamEnd;
    extern f32 ObstacleCamTime;
    extern i16 ObstacleCamRotZ;
    extern f32 ObstacleCamBlendInTime;
    extern f32 ObstacleCamBlendOutTime;
    extern NUVEC *ObstacleCamCutTgtPtr;
    extern NUVEC *ObstacleCamCutCamPtr;
    extern i32 ObstacleCamBorders;
    extern f32 CutBorderScale;
    extern f32 DEFAULTFPS;
    extern i32 Level;
    extern i32 NextArea_FreePlay;
    extern i32 FreePlay;
    extern i32 new_level_from_menu;
    extern i32 reset_restart;
    extern i32 BonusWinner;
    extern i32 BonusCoinTotal;
    extern i32 CUTSTOPGAME;
    extern i32 LevelChange;
    extern i32 Arcade;
    extern i32 ChallengeMode;
    extern i32 EDITBUFFERENDSIZE;
    extern VARIPTR permbuffer_base;
    extern VARIPTR permbuffer_ptr;
    extern VARIPTR permbuffer_end;
    extern struct nusound_filename_info_s *MusicInfo;
    extern void *Game_Customiser;
    extern f32 HIGHJUMPHEIGHT;
    extern TIMER AreaTimer;
    extern i32 OldBonusScore[2];
    extern i32 BonusScore[2];
    extern i32 BuildUpTotal;
    extern i32 BuildUpDone;
    extern TERRAIN_LAYER_PC TerLayer[17];
    extern NUVEC nusound_special_positions[5];
    extern u8 CutSceneCameraCTRL;
    extern i32 NOAICREATURES;
    extern i32 LEGOHINT_FREEPLAYTOGGLE;
// Retain the PC global spellings for these named physical members.
// NOLINTNEXTLINE(readability-identifier-naming)
#define LevInstAnim LevInstAnimResetPc.reset_words
    extern u8 LevLock[5];
    extern i32 SuperStoryEpisode;
    extern i32 LevDeaths;
    extern i32 newlevelfrommenu_newmenuid;
    extern i32 newlevelfrommenu_newmenuy;
    extern u32 ResetBits;
    extern i32 CHARCATEGORYCOUNT;
    extern i32 NetPaused;
    extern i32 KEEPONSCREEN_SIDESONLY;
    extern f32 fakeanimendframe[1];
    extern i32 pause_i_pad;
    extern i32 HubCompletionPointsPc;
    extern struct EPISODESAVE_s *EpisodeSavePc;
    extern i32 Area_PlayerModelCount;
    extern i32 FreePlayModelCount;
    extern i32 FreePlayResidentCount;
    extern i32 Customiser_AccessoriesLoaded;
    extern i32 MenuSaveOccurred;
    extern i32 Grapples_Available;
    extern u32 texanimbits;
    extern f32 newgamealpha;
    extern i32 newgame_menudrawoff;
    extern i32 other_level_override;
    extern i32 other_level_override;
    extern TIMER JoinInTimer;
    extern i32 Tag_DoneFirst;
    extern i32 Tag_DoneAny;
    extern f32 game_pulse;
    extern void (*PauseGame_ExtraCodeFn)(void);
    extern i32 LEGOMENU_NEWGAME;
    extern i32 LEGOMENU_PAUSEMAIN;
    extern i32 LEGOMENU_CREDITS;
    extern i32 LEGOCAMMODE_DOORCUT;
    extern i32 LEGOCAMMODE_OBSTACLE;
    extern i32 POINTS_PER_CHARACTER;
    extern i32 POINTS_PER_SUPERSTORY;
    extern i32 POINTS_PER_SUPERBONUSCOMPLETE;
    extern i32 POINTS_PER_TIMETRIAL;
    extern i32 POINTS_PER_STORY;
    extern i32 POINTS_PER_CHALLENGE;
    extern i32 POINTS_PER_MINIKIT;
    extern i32 POINTS_PER_TRUEJEDI;
    extern i32 POINTS_PER_REDBRICK;
    extern i32 POINTS_PER_MISSION;
    extern i32 POINTS_PER_CHEAT;
    extern i32 POINTS_PER_GOLDBRICK;
    extern i32 GOLDBRICKPOINTS;
    extern i32 SHOPGOLDBRICKS;
    extern i32 LEGOHINT_SHOOTCAMERAS;
    extern i32 LEGOHINT_BUILD;
    extern i32 LEGOOBJ_FLOORTARGET;
    extern i32 LEGOOBJ_GRAPPLE_HOOK;
    extern const char *LEGOASCII_DOWN;
    extern i32 global_frame_count_paused;
#ifdef __cplusplus
}
#endif
typedef struct EPISODESAVE_s EPISODESAVE_s;
#ifdef __cplusplus
extern "C" {
#endif
    extern u16 *Game_CompletionSave;
    extern i32 g_lowEndLevelBehaviour;
    extern i32 COMPLETIONPOINTS;
    extern i32 PauseMenus_Align;
    extern f32 PauseMenus_X;
    extern TIMER SuperStoryTimer;
    extern TIMER ChallengeTimer;
    extern i32 radios_playing;
    extern struct EPISODESAVE_s *Game_EpisodeSave;
    extern void *newmode_cutinfo;
    extern LEVEL_OBJECT_CONFIG_PC ObjTab[750];
#ifdef __cplusplus
}
#endif
#undef SAGA_GLOBALS_H_PART
#define SAGA_GLOBALS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_GLOBALS_H_STOP
#endif
#endif

#if SAGA_GLOBALS_H_PART == 2 && !defined(SAGA_GLOBALS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern struct collection_area_save_pc_s *Game_AreaSave;
    extern struct level_progress_pc_s *LevelProgressData;
    extern struct vufnt_s *QFont2D;
    extern struct nucamera_s *pNuCam;
    extern i32 (*MakeLayerList)(struct CHARACTER_MODEL_LAYERS_PC *, i16 *, u32);
#ifdef __cplusplus
}
#endif
#undef SAGA_GLOBALS_H_PART
#define SAGA_GLOBALS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_GLOBALS_H_STOP
#endif
#endif

#if SAGA_GLOBALS_H_PART == 3 && !defined(SAGA_GLOBALS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern struct nufpcomjmp_s *Level_ConfigBeforeLoad_GameKeywords;
#ifdef __cplusplus
}
#endif
#undef SAGA_GLOBALS_H_PART
#define SAGA_GLOBALS_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_GLOBALS_H_STOP
#endif
#endif

#if SAGA_GLOBALS_H_PART == 4 && !defined(SAGA_GLOBALS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    /* SuperCarry_PartKill is static original carrying TU callback; no public declaration. */

    extern struct area_data_pc_s *HOTHBATTLE_ADATA;
    extern struct area_data_pc_s *BOUNTYHUNTERPURSUIT_ADATA;
    extern struct area_data_pc_s *BATTLEOVERCORUSCANT_ADATA;
    extern struct level_data_pc_s *ASTEROIDCHASEC_LDATA;
    extern struct level_data_pc_s *DAGOBAHC_LDATA;
    extern struct level_data_pc_s *ENDORBATTLEC_LDATA;
    extern struct level_data_pc_s *NEGOTIATIONSA_LDATA;
    extern struct level_data_pc_s *NEGOTIATIONSB_LDATA;
    extern struct level_data_pc_s *GUNGAN_A_LDATA;
    extern struct level_data_pc_s *RESCUEC_LDATA;
    extern struct level_data_pc_s *TATOOINEA_LDATA;
    extern struct level_data_pc_s *TATOOINEB_LDATA;
    extern struct level_data_pc_s *MOSEISLEYA_LDATA;
    extern struct level_data_pc_s *DEATHSTARRESCUEB_LDATA;
    extern struct level_data_pc_s *DEATHSTARRESCUEC_LDATA;
    extern struct level_data_pc_s *DEATHSTARRESCUEE_LDATA;
    extern struct level_data_pc_s *DEATHSTARESCAPEA_LDATA;
    extern struct level_data_pc_s *DEATHSTARESCAPED_LDATA;
    extern struct level_data_pc_s *DEATHSTARESCAPEB_LDATA;
    extern struct level_data_pc_s *DEATHSTARESCAPEC_LDATA;
    extern struct level_data_pc_s *DEATHSTARBATTLEA_LDATA;
    extern struct level_data_pc_s *DEATHSTARBATTLEB_LDATA;
    extern struct level_data_pc_s *DEATHSTARBATTLEC_LDATA;
    extern struct level_data_pc_s *HOTHESCAPEA_LDATA;
    extern struct level_data_pc_s *HOTHESCAPEC_LDATA;
    extern struct level_data_pc_s *CLOUDCITYTRAPA_LDATA;
    extern struct level_data_pc_s *CLOUDCITYESCAPEA_LDATA;
    extern struct level_data_pc_s *CLOUDCITYESCAPEC_LDATA;
    extern struct level_data_pc_s *DEATHSTAR2BATTLEB_LDATA;
    extern struct level_data_pc_s *DEATHSTAR2BATTLED_LDATA;
    extern struct level_data_pc_s *JABBASPALACEA_LDATA;
    extern struct level_data_pc_s *JABBASPALACEB_LDATA;
    extern struct level_data_pc_s *JABBASPALACED_LDATA;
    extern struct level_data_pc_s *JABBASPALACEE_LDATA;
    extern struct level_data_pc_s *SARLACCPITB_LDATA;
    extern struct level_data_pc_s *SARLACCPITC_LDATA;
    extern struct level_data_pc_s *ENDORBATTLEA_LDATA;
    extern struct level_data_pc_s *ENDORBATTLEB_LDATA;
    extern struct level_data_pc_s *ENDORBATTLED_LDATA;
    extern struct level_data_pc_s *EMPERORFIGHTA_LDATA;
    extern struct level_data_pc_s *MOSEISLEYC_LDATA;
    extern i32 (*UsingExtraActionsFn)(struct gameobject_s *);
    extern struct level_data_pc_s *SARLACCPITA_LDATA;

    extern struct level_data_pc_s *ASTEROIDCHASEC_LDATA;
    extern struct level_data_pc_s *ENDORBATTLEC_LDATA;
    extern i32 (*FindSlamOrigin_UseCPosFn)(struct gameobject_s *);
    extern void (*GizObstacle_SetDefaultSFXFn)(void *world, GIZOBSTACLE *obstacle);
    extern struct area_data_pc_s *DOGFIGHT_ADATA;
    extern struct level_data_pc_s *SPEEDERCHASEA_LDATA;
    extern struct level_data_pc_s *DEATHSTARBATTLED_LDATA;
    extern struct gameobject_s *CutDeadVehiclePlayer;
    extern i32 (*CanMagnetClimbFn)(struct gameobject_s *);
    extern struct area_data_pc_s *LOSTTEMPLE_ADATA;
    extern struct level_data_pc_s *HUB_LDATA;
    extern struct level_data_pc_s *STATUS_LDATA;
    extern struct level_data_pc_s *CREDITS_LDATA;
    extern APICHARACTERMODELLIST Hub_ModelList[6];
    extern MAINFRAMECOUNTERS MainFrameCounters;
    extern struct level_data_pc_s *MOSEISLEYB_LDATA;
    extern struct level_data_pc_s *DEATHSTAR2BATTLEE_LDATA;
    extern struct level_data_pc_s *DEATHSTAR2BATTLEF_LDATA;
    extern struct level_data_pc_s *DEATHSTAR2BATTLEG_LDATA;
    extern struct gameobject_s *LevGameObject[8];
    extern struct level_data_pc_s *NEGOTIATIONSC_LDATA;
    extern GIZOBSTACLE *LevGizObst[8];

    extern struct level_data_pc_s *DEATHSTAR2BATTLEA_LDATA;
    extern struct level_data_pc_s *ASTEROIDCHASED_LDATA;
    extern struct level_data_pc_s *DEATHSTARRESCUEA_LDATA;
    extern ARCADE_MODE_PC Arcade_Mode[7];
    extern ARCADE_ITEM_PC ArcadeItem;
    extern struct level_data_pc_s *HOTHBATTLEA_LDATA;
    extern struct nugspline_s *ObstacleCamSpl;

    extern struct area_data_pc_s *PODSPRINT_ADATA;
    extern struct gameobject_s *BombGenerator_PlayerBomb[2];
    extern struct nugspline_s *script_spline_selected;
    extern struct nugspline_s *script_spline_selected;
    extern CHAR_CATEGORY_RECORD_PC *CharCategory;
    extern APICHARACTERMODELLIST FreePlayModelList[49];
    extern AREA_LEVEL_LOAD_PC LevelLoad[12];
    extern APICHARACTERMODELLIST Area_MissionModelList[49];
    extern APICHARACTERMODELLIST *CurrentCList;
    extern APICHARACTERMODELLIST *CurrentStoryCList;
    extern i32 (*CanGlideFn)(struct gameobject_s *);
    extern i32 (*Jump_PreventJumpFn)(struct gameobject_s *);
    extern i32 (*Slam_GetDebrisFn)(struct gameobject_s *, i32);
    extern void (*Jump_EndOfLandContextFn)(struct gameobject_s *);
    extern struct level_data_pc_s *TATOOINEE_LDATA;
    extern struct level_data_pc_s *ASTEROIDCHASEA_LDATA;
    extern struct level_data_pc_s *ASTEROIDCHASEMITRO_LDATA;

    extern struct area_data_pc_s *SENATE_ADATA;
    extern i32 (*SetSoundFadeDistCallBackFn)(struct WORLDINFO_s *);
    extern struct area_data_pc_s *DEATHSTARESCAPE_ADATA;
    extern DETONATOR_s Detonator[10];
    extern struct area_data_pc_s *SENATE_ADATA;
#ifdef __cplusplus
}
#endif
#undef SAGA_GLOBALS_H_PART
#define SAGA_GLOBALS_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_GLOBALS_H_STOP
#endif
#endif

#if SAGA_GLOBALS_H_PART == 5 && !defined(SAGA_GLOBALS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern GAMESAVE Game;
    extern struct CUSTOMISER_LAYERS_PC *CharacterCustomiser;
    extern CUSTOMISER_COMPLETION_READER_PC *CustomiserCompletionSavePc;
#ifdef __cplusplus
}
#endif
#undef SAGA_GLOBALS_H_PART
#define SAGA_GLOBALS_H_PART 6
#ifdef SAGA_SHIM
#define SAGA_GLOBALS_H_STOP
#endif
#endif

#undef SAGA_GLOBALS_H_STOP
