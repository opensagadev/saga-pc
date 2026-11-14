#ifndef SAGA_GLOBALS_H_PART
#define SAGA_GLOBALS_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: gamelib/util/Refpack.h, legoapi/characters/core/characters.h,
 * legoapi/core/input/timer.h, legoapi/core/input/timing.h, legoapi/items/base/apiobject.h, legoapi/legoapi_types.h,
 * legoapi/lib/hub.h, legoapi/menus/screens/customise.h, legoapi/world/level.h, legoapi/world/levels.h,
 * legoapi/world/world.h, legoapi/world/worldinfo.h, legogame/game.h, nu2api/nu3d/nucamera.h, nu2api/nu3d/nuqfnt.h,
 * nu2api/nu3d/nuspline.h, nu2api/nufile/nufpar.h, nu2api/numath/nuvec.h, nu2api/nusound/nusound.h (not all of it
 * earlier in the shim). */

#ifndef SAGA_SHIM
#include "gamelib/util/Refpack.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/input/timing.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/worldinfo.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nusound/nusound.h"
#endif
/* 5 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_GLOBALS_H_PART == 1 && !defined(SAGA_GLOBALS_H_STOP)

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
    extern COLLECTION_AREA_SAVE_PC *Game_AreaSave;
    extern LEVEL_PROGRESS *LevelProgressData;
    extern VUFNT *QFont2D;
    extern NUCAMERA *pNuCam;
    extern i32 (*MakeLayerList)(CHARACTER_MODEL_LAYERS_PC *, i16 *, u32);
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
    extern NUFPCOMJMP *Level_ConfigBeforeLoad_GameKeywords;
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

    extern AREA_DATA_PC *HOTHBATTLE_ADATA;
    extern AREA_DATA_PC *BOUNTYHUNTERPURSUIT_ADATA;
    extern AREA_DATA_PC *BATTLEOVERCORUSCANT_ADATA;
    extern LEVELDATA_PC *ASTEROIDCHASEC_LDATA;
    extern LEVELDATA_PC *DAGOBAHC_LDATA;
    extern LEVELDATA_PC *ENDORBATTLEC_LDATA;
    extern LEVELDATA_PC *NEGOTIATIONSA_LDATA;
    extern LEVELDATA_PC *NEGOTIATIONSB_LDATA;
    extern LEVELDATA_PC *GUNGAN_A_LDATA;
    extern LEVELDATA_PC *RESCUEC_LDATA;
    extern LEVELDATA_PC *TATOOINEA_LDATA;
    extern LEVELDATA_PC *TATOOINEB_LDATA;
    extern LEVELDATA_PC *MOSEISLEYA_LDATA;
    extern LEVELDATA_PC *DEATHSTARRESCUEB_LDATA;
    extern LEVELDATA_PC *DEATHSTARRESCUEC_LDATA;
    extern LEVELDATA_PC *DEATHSTARRESCUEE_LDATA;
    extern LEVELDATA_PC *DEATHSTARESCAPEA_LDATA;
    extern LEVELDATA_PC *DEATHSTARESCAPED_LDATA;
    extern LEVELDATA_PC *DEATHSTARESCAPEB_LDATA;
    extern LEVELDATA_PC *DEATHSTARESCAPEC_LDATA;
    extern LEVELDATA_PC *DEATHSTARBATTLEA_LDATA;
    extern LEVELDATA_PC *DEATHSTARBATTLEB_LDATA;
    extern LEVELDATA_PC *DEATHSTARBATTLEC_LDATA;
    extern LEVELDATA_PC *HOTHESCAPEA_LDATA;
    extern LEVELDATA_PC *HOTHESCAPEC_LDATA;
    extern LEVELDATA_PC *CLOUDCITYTRAPA_LDATA;
    extern LEVELDATA_PC *CLOUDCITYESCAPEA_LDATA;
    extern LEVELDATA_PC *CLOUDCITYESCAPEC_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEB_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLED_LDATA;
    extern LEVELDATA_PC *JABBASPALACEA_LDATA;
    extern LEVELDATA_PC *JABBASPALACEB_LDATA;
    extern LEVELDATA_PC *JABBASPALACED_LDATA;
    extern LEVELDATA_PC *JABBASPALACEE_LDATA;
    extern LEVELDATA_PC *SARLACCPITB_LDATA;
    extern LEVELDATA_PC *SARLACCPITC_LDATA;
    extern LEVELDATA_PC *ENDORBATTLEA_LDATA;
    extern LEVELDATA_PC *ENDORBATTLEB_LDATA;
    extern LEVELDATA_PC *ENDORBATTLED_LDATA;
    extern LEVELDATA_PC *EMPERORFIGHTA_LDATA;
    extern LEVELDATA_PC *MOSEISLEYC_LDATA;
    extern i32 (*UsingExtraActionsFn)(GAMEOBJECT *);
    extern LEVELDATA_PC *SARLACCPITA_LDATA;

    extern LEVELDATA_PC *ASTEROIDCHASEC_LDATA;
    extern LEVELDATA_PC *ENDORBATTLEC_LDATA;
    extern i32 (*FindSlamOrigin_UseCPosFn)(GAMEOBJECT *);
    extern void (*GizObstacle_SetDefaultSFXFn)(void *world, GIZOBSTACLE *obstacle);
    extern AREA_DATA_PC *DOGFIGHT_ADATA;
    extern LEVELDATA_PC *SPEEDERCHASEA_LDATA;
    extern LEVELDATA_PC *DEATHSTARBATTLED_LDATA;
    extern GAMEOBJECT *CutDeadVehiclePlayer;
    extern i32 (*CanMagnetClimbFn)(GAMEOBJECT *);
    extern AREA_DATA_PC *LOSTTEMPLE_ADATA;
    extern LEVELDATA_PC *HUB_LDATA;
    extern LEVELDATA_PC *STATUS_LDATA;
    extern LEVELDATA_PC *CREDITS_LDATA;
    extern APICHARACTERMODELLIST Hub_ModelList[6];
    extern MAINFRAMECOUNTERS MainFrameCounters;
    extern LEVELDATA_PC *MOSEISLEYB_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEE_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEF_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEG_LDATA;
    extern GAMEOBJECT *LevGameObject[8];
    extern LEVELDATA_PC *NEGOTIATIONSC_LDATA;
    extern GIZOBSTACLE *LevGizObst[8];

    extern LEVELDATA_PC *DEATHSTAR2BATTLEA_LDATA;
    extern LEVELDATA_PC *ASTEROIDCHASED_LDATA;
    extern LEVELDATA_PC *DEATHSTARRESCUEA_LDATA;
    extern ARCADE_MODE_PC Arcade_Mode[7];
    extern ARCADE_ITEM_PC ArcadeItem;
    extern LEVELDATA_PC *HOTHBATTLEA_LDATA;
    extern NUGSPLINE *ObstacleCamSpl;

    extern AREA_DATA_PC *PODSPRINT_ADATA;
    extern GAMEOBJECT *BombGenerator_PlayerBomb[2];
    extern NUGSPLINE *script_spline_selected;
    extern NUGSPLINE *script_spline_selected;
    extern CHAR_CATEGORY_RECORD_PC *CharCategory;
    extern APICHARACTERMODELLIST FreePlayModelList[49];
    extern AREA_LEVEL_LOAD_PC LevelLoad[12];
    extern APICHARACTERMODELLIST Area_MissionModelList[49];
    extern APICHARACTERMODELLIST *CurrentCList;
    extern APICHARACTERMODELLIST *CurrentStoryCList;
    extern i32 (*CanGlideFn)(GAMEOBJECT *);
    extern i32 (*Jump_PreventJumpFn)(GAMEOBJECT *);
    extern i32 (*Slam_GetDebrisFn)(GAMEOBJECT *, i32);
    extern void (*Jump_EndOfLandContextFn)(GAMEOBJECT *);
    extern LEVELDATA_PC *TATOOINEE_LDATA;
    extern LEVELDATA_PC *ASTEROIDCHASEA_LDATA;
    extern LEVELDATA_PC *ASTEROIDCHASEMITRO_LDATA;

    extern AREA_DATA_PC *SENATE_ADATA;
    extern i32 (*SetSoundFadeDistCallBackFn)(WORLDINFO *);
    extern AREA_DATA_PC *DEATHSTARESCAPE_ADATA;
    extern DETONATOR_s Detonator[10];
    extern AREA_DATA_PC *SENATE_ADATA;
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
    extern CUSTOMISER_LAYERS_PC *CharacterCustomiser;
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
