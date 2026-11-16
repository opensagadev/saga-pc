#pragma once

#include "nu2api/nucore/common.h"

#include "gamelib/util/Refpack.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/input/timing.h"
#include "legoapi/world/worldinfo.h"
#include "legogame/game.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nuvec.h"

struct apicharactermodellist_s;
struct char_category_record_s;
struct customiser_completion_reader_s;
struct gizobstacle_s;

struct CHARACTER_MODEL_LAYERS;
struct customiser_config_s;
struct WORLDINFO_s;
struct area_data_s;
struct collection_area_save_s;
struct gameobject_s;
struct level_data_s;
struct level_progress_s;
struct nucamera_s;
struct nufpcomjmp_s;
struct nugspline_s;
struct vufnt_s;

typedef struct terrain_layer_s { // size 0xc
    u32 flags;                   // 0x0
    u32 unknown_04;              // 0x4
    u32 unknown_08;              // 0x8
} TERRAIN_LAYER;

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
    typedef void (*CHECK_LOST_DATA)(void *);
    extern i32 ShadowMode;
    extern CHECK_LOST_DATA CheckLostDataFn;
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

    extern NURAND GAMERAND;
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
    extern struct customiser_config_s *Game_Customiser;
    extern f32 HIGHJUMPHEIGHT;
    extern TIMER AreaTimer;
    extern i32 OldBonusScore[2];
    extern i32 BonusScore[2];
    extern i32 BuildUpTotal;
    extern i32 BuildUpDone;
    extern TERRAIN_LAYER TerLayer[17];
    extern NUVEC nusound_special_positions[5];
    extern u8 CutSceneCameraCTRL;
    extern i32 NOAICREATURES;
    extern i32 LEGOHINT_FREEPLAYTOGGLE;
// Retain the PC global spellings for these named physical members.
// NOLINTNEXTLINE(readability-identifier-naming)
#define LevInstAnim LevInstAnimReset.reset_words
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
    extern i32 HubCompletionPoints;
    extern struct EPISODESAVE_s *EpisodeSave;
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
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern struct collection_area_save_s *Game_AreaSave;
    extern struct level_progress_s *LevelProgressData;
    extern struct vufnt_s *QFont2D;
    extern struct nucamera_s *pNuCam;
    extern i32 (*MakeLayerList)(struct CHARACTER_MODEL_LAYERS *, i16 *, u32);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern struct nufpcomjmp_s *Level_ConfigBeforeLoad_GameKeywords;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    /* SuperCarry_PartKill is static original carrying TU callback; no public declaration. */

    extern struct area_data_s *HOTHBATTLE_ADATA;
    extern struct area_data_s *BOUNTYHUNTERPURSUIT_ADATA;
    extern struct area_data_s *BATTLEOVERCORUSCANT_ADATA;
    extern struct level_data_s *ASTEROIDCHASEC_LDATA;
    extern struct level_data_s *DAGOBAHC_LDATA;
    extern struct level_data_s *ENDORBATTLEC_LDATA;
    extern struct level_data_s *NEGOTIATIONSA_LDATA;
    extern struct level_data_s *NEGOTIATIONSB_LDATA;
    extern struct level_data_s *GUNGAN_A_LDATA;
    extern struct level_data_s *RESCUEC_LDATA;
    extern struct level_data_s *TATOOINEA_LDATA;
    extern struct level_data_s *TATOOINEB_LDATA;
    extern struct level_data_s *MOSEISLEYA_LDATA;
    extern struct level_data_s *DEATHSTARRESCUEB_LDATA;
    extern struct level_data_s *DEATHSTARRESCUEC_LDATA;
    extern struct level_data_s *DEATHSTARRESCUEE_LDATA;
    extern struct level_data_s *DEATHSTARESCAPEA_LDATA;
    extern struct level_data_s *DEATHSTARESCAPED_LDATA;
    extern struct level_data_s *DEATHSTARESCAPEB_LDATA;
    extern struct level_data_s *DEATHSTARESCAPEC_LDATA;
    extern struct level_data_s *DEATHSTARBATTLEA_LDATA;
    extern struct level_data_s *DEATHSTARBATTLEB_LDATA;
    extern struct level_data_s *DEATHSTARBATTLEC_LDATA;
    extern struct level_data_s *HOTHESCAPEA_LDATA;
    extern struct level_data_s *HOTHESCAPEC_LDATA;
    extern struct level_data_s *CLOUDCITYTRAPA_LDATA;
    extern struct level_data_s *CLOUDCITYESCAPEA_LDATA;
    extern struct level_data_s *CLOUDCITYESCAPEC_LDATA;
    extern struct level_data_s *DEATHSTAR2BATTLEB_LDATA;
    extern struct level_data_s *DEATHSTAR2BATTLED_LDATA;
    extern struct level_data_s *JABBASPALACEA_LDATA;
    extern struct level_data_s *JABBASPALACEB_LDATA;
    extern struct level_data_s *JABBASPALACED_LDATA;
    extern struct level_data_s *JABBASPALACEE_LDATA;
    extern struct level_data_s *SARLACCPITB_LDATA;
    extern struct level_data_s *SARLACCPITC_LDATA;
    extern struct level_data_s *ENDORBATTLEA_LDATA;
    extern struct level_data_s *ENDORBATTLEB_LDATA;
    extern struct level_data_s *ENDORBATTLED_LDATA;
    extern struct level_data_s *EMPERORFIGHTA_LDATA;
    extern struct level_data_s *MOSEISLEYC_LDATA;
    extern i32 (*UsingExtraActionsFn)(struct gameobject_s *);
    extern struct level_data_s *SARLACCPITA_LDATA;

    extern struct level_data_s *ASTEROIDCHASEC_LDATA;
    extern struct level_data_s *ENDORBATTLEC_LDATA;
    extern i32 (*FindSlamOrigin_UseCPosFn)(struct gameobject_s *);
    extern void (*GizObstacle_SetDefaultSFXFn)(void *world, struct gizobstacle_s *obstacle);
    extern struct area_data_s *DOGFIGHT_ADATA;
    extern struct level_data_s *SPEEDERCHASEA_LDATA;
    extern struct level_data_s *DEATHSTARBATTLED_LDATA;
    extern struct gameobject_s *CutDeadVehiclePlayer;
    extern i32 (*CanMagnetClimbFn)(struct gameobject_s *);
    extern struct area_data_s *LOSTTEMPLE_ADATA;
    extern struct level_data_s *HUB_LDATA;
    extern struct level_data_s *STATUS_LDATA;
    extern struct level_data_s *CREDITS_LDATA;
    extern MAINFRAMECOUNTERS MainFrameCounters;
    extern struct level_data_s *MOSEISLEYB_LDATA;
    extern struct level_data_s *DEATHSTAR2BATTLEE_LDATA;
    extern struct level_data_s *DEATHSTAR2BATTLEF_LDATA;
    extern struct level_data_s *DEATHSTAR2BATTLEG_LDATA;
    extern struct gameobject_s *LevGameObject[8];
    extern struct level_data_s *NEGOTIATIONSC_LDATA;
    extern struct gizobstacle_s *LevGizObst[8];

    extern struct level_data_s *DEATHSTAR2BATTLEA_LDATA;
    extern struct level_data_s *ASTEROIDCHASED_LDATA;
    extern struct level_data_s *DEATHSTARRESCUEA_LDATA;
    extern ARCADE_MODE Arcade_Mode[7];
    extern ARCADE_ITEM ArcadeItem;
    extern struct level_data_s *HOTHBATTLEA_LDATA;
    extern struct nugspline_s *ObstacleCamSpl;

    extern struct area_data_s *PODSPRINT_ADATA;
    extern struct gameobject_s *BombGenerator_PlayerBomb[2];
    extern struct nugspline_s *script_spline_selected;
    extern struct nugspline_s *script_spline_selected;
    extern struct char_category_record_s *CharCategory;
    extern AREA_LEVEL_LOAD LevelLoad[12];
    extern struct apicharactermodellist_s *CurrentCList;
    extern struct apicharactermodellist_s *CurrentStoryCList;
    extern i32 (*CanGlideFn)(struct gameobject_s *);
    extern i32 (*Jump_PreventJumpFn)(struct gameobject_s *);
    extern i32 (*Slam_GetDebrisFn)(struct gameobject_s *, i32);
    extern void (*Jump_EndOfLandContextFn)(struct gameobject_s *);
    extern struct level_data_s *TATOOINEE_LDATA;
    extern struct level_data_s *ASTEROIDCHASEA_LDATA;
    extern struct level_data_s *ASTEROIDCHASEMITRO_LDATA;

    extern struct area_data_s *SENATE_ADATA;
    extern i32 (*SetSoundFadeDistCallBackFn)(struct WORLDINFO_s *);
    extern struct area_data_s *DEATHSTARESCAPE_ADATA;
    typedef struct DETONATOR_s DETONATOR_s;

    struct DETONATOR_s {             // size 0x38
        NUVEC position;              // 0x00
        NUVEC unknown_0c;            // 0x0c
        NUVEC unknown_18;            // 0x18
        struct gameobject_s *object; // 0x24
        u8 active;                   // 0x28
        u8 draw_result;              // 0x29
        u16 rotation_x;              // 0x2a
        u16 rotation_y;              // 0x2c
        u16 rotation_z;              // 0x2e
        f32 timer;                   // 0x30
        void *unknown_34;            // 0x34
    };

    extern DETONATOR_s Detonator[10];
    extern struct area_data_s *SENATE_ADATA;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern struct customiser_config_s *CharacterCustomiser;
    extern struct customiser_completion_reader_s *CustomiserCompletionSave;
#ifdef __cplusplus
}
#endif
