/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#include "globals.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/missions.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nuvec.h"
#endif

struct PART_PC;
struct WORLDINFO_s;
struct area_data_s;
struct gameobject_s;
struct level_data_s;

typedef struct gamesave_s { // size 0x7e4c

    union { // size 0x4
        u32 unknown_0000;

        struct {                // size 0x4
            u8 unknown_game_00; // 0x0000

            union { // size 0x1
                u8 difficulty;
                u8 save_version;
            }; // 0x0001

            u8 unknown_game_02[2]; // 0x0002
        };
    }; // 0x0000

    OPTIONSSAVE options_save;       // 0x0004
    u8 level_save[0x781b];          // 0x0011
    AREA_SAVE_PICKUP area_save[72]; // 0x782c

    union { // size 0x108

        struct { // size 0x108

            union { // size 0x8f
                u8 unknown_7b8c[0x8f];

                struct {                                // size 0x8f
                    u8 unknown_gold_brick_prefix[0x8e]; // 0x7b8c
                    u8 gold_bricks;                     // 0x7c1a
                };
            }; // 0x7b8c

            u8 customizer_weirdo_flags; // 0x7c1b

            union { // size 0x4
                u8 unknown_7c1c[4];

                struct {                      // size 0x4
                    u8 hub_buildit_completed; // 0x7c1c

                    union { // size 0x1
                        u8 losttemple_trailer_watched;
                        u8 lost_temple_area_complete;
                    }; // 0x7c1d

                    u8 unknown_7c1e[2]; // 0x7c1e
                };
            }; // 0x7c1c

            f32 game_time;                       // 0x7c20
            u8 unknown_7c24[0xa];                // 0x7c24
            u16 customizer_cape_piece;           // 0x7c2e
            u8 unknown_7c30[0x36];               // 0x7c30
            u16 customizer_secondary_cape_piece; // 0x7c66
            u8 unknown_7c68[0x2c];               // 0x7c68
        };

        struct {                                  // size 0x108
            struct EPISODESAVE_s episode_save[6]; // 0x7b8c
            u8 newgame_reserved_7bd4[0x28];       // 0x7bd4
            u32 initial_store_pack_flags;         // 0x7bfc
            u32 newgame_reserved_7c00;            // 0x7c00
            u32 purchased_extra_bits_high;        // 0x7c04

            union { // size 0x10
                u8 newgame_reserved_7c08[0x10];

                struct { // size 0x10

                    union { // size 0xc
                        u8 hud_coin_reserved_7c08[0xc];
                        u32 hint_completion_bits[3];
                    }; // 0x7c08

                    u32 coins; // 0x7c14
                };
            }; // 0x7c08

            u16 save_hash;                             // 0x7c18
            u8 newgame_reserved_7c1a[0xa];             // 0x7c1a
            GAME_CUSTOMISER_START customiser_start[2]; // 0x7c24
        };
    }; // 0x7b8c

    MISSION_SAVE_READER mission_save; // 0x7c94

    union { // size 0x154
        u8 unknown_7cf8[0x154];
        u8 character_save[0x154];
    }; // 0x7cf8
} GAMESAVE;

STATIC_ASSERT(offsetof(GAMESAVE, mission_save) + offsetof(MISSION_SAVE_READER, completed) == 0x7ce4);
STATIC_ASSERT(offsetof(GAMESAVE, mission_save) + offsetof(MISSION_SAVE_READER, completed) == 0x7ce4);

// Access prefix only; the original allocation extent remains unknown.
typedef struct area_globals_s { // size 0x34
    i32 field_00;               // 0x00
    i32 field_04;               // 0x04
    i32 field_08;               // 0x08
    i32 area_phase;             // 0x0c
    i32 new_minikit_count;      // 0x10
    i32 progress_flags;         // 0x14
    i32 field_18;               // 0x18
    i32 field_1c;               // 0x1c
    i32 field_20;               // 0x20
    u32 arcade_player_kills[2]; // 0x24
    u32 arcade_ai_kills[2];     // 0x2c
} AREA_GLOBALS_PC;

typedef struct cutscenecharacter_s {
    u8 opaque_00_40[0x40];
    char *name;
    u8 opaque_44[0x4];
} CUTSCENECHARACTER;

typedef union PC_SURFACE_MASK { // size 0x4
    u8 low;
    u32 word;
} PC_SURFACE_MASK;

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 CombatReactionHorizontalScale;
    /* New public declarations only; private PartCollidePC stays original parts.c-local. */
    void PartCollide_2D(struct PART_PC *);
    void DeflectPart(struct PART_PC *, struct gameobject_s *, f32, f32, i32, i32);
    void InitGameMode(void);

    typedef union GIZMO_BLOWUP_HIT_MULTIPLIER_STORAGE { // size 0x4
        i32 value;
        u8 low_byte;
    } GIZMO_BLOWUP_HIT_MULTIPLIER_STORAGE;

    extern i32 LegoObjectIconQuestion;

    typedef struct CUT_DRAW_SETTINGS { // size 0x8
        i16 blaster_object_0;          // 0x0
        i16 blaster_object_1;          // 0x2
        i16 object_id;                 // 0x4
        i8 cheat_id;                   // 0x6
        u8 normal_layers;              // 0x7
    } CUT_DRAW_SETTINGS;

    extern i32 CutSceneLookupAllDebrisPages;
    extern struct area_data_s *JABBASPALACE_ADATA;
    extern struct area_data_s *DAGOBAH_ADATA;
    extern const char LevelConfigGameKeyword_max_guidelines[15];
    extern const char LevelConfigGameKeyword_max_timers[11];
    extern const char LevelConfigGameKeyword_max_attractos[14];
    extern const char LevelConfigGameKeyword_max_climb_objects[18];
    extern const char LevelConfigGameKeyword_maxgizmoblowups[16];
    extern const char LevelConfigGameKeyword_maxgizmoblowuptypes[20];
    extern const char LevelConfigGameKeyword_max_minicut_stages[19];
    extern const char LevelConfigGameKeyword_max_minicuts[13];
    extern const char LevelConfigGameKeyword_max_spinners[13];
    extern const char LevelConfigGameKeyword_max_shards[11];
    extern const char LevelConfigGameKeyword_max_spinneranim_objs[21];
    extern const char LevelConfigGameKeyword_max_buildit_objects[20];
    extern const char LevelConfigGameKeyword_max_buildits[13];
    extern const char LevelConfigGameKeyword_max_turrets[12];
    extern const char LevelConfigGameKeyword_max_obstacle_objects[21];
    extern const char LevelConfigGameKeyword_max_obstacles[14];
    extern const char LevelConfigGameKeyword_max_grapples[13];
    extern const char LevelConfigGameKeyword_max_zipups[11];
    extern const char LevelConfigGameKeyword_max_technos[12];
    extern const char LevelConfigGameKeyword_max_levers[11];
    extern const char LevelConfigGameKeyword_max_signals[12];
    extern const char LevelConfigGameKeyword_max_tightropes[15];
    extern NUFPCOMJMP LevelConfigKeywords_BeforeLoad[45];
    extern char SaveProductCode[12];
    i32 CutScene_StartFn_LSW(CUTINFO *cut);
    void CutScene_PreUpdateFn_LSW(CUTINFO *cut);
    void CutScene_PostUpdateFn_LSW(void);
    void CutScene_StoppedFn_LSW(CUTINFO *cut);
    i32 CutScene_ReplaceCharacterModelFn_LSW(CUTINFO *cut, CUTSCENECHARACTER *character);
    i32 InitBolt_AddMomentumType_LSW(BOLT *bolt, struct gameobject_s *object, NUVEC *momentum);
    i32 Bolt_HitPlatFn_LSW(BOLT *bolt);
    i32 Bolt_HitCustomFn_LSW(void *bolt, void *points);
    void GameBlowUpBlownUpFn_LSW(void *blowup);
    extern void *game_cutscenes_cutscene;
    extern void *game_cutscenes_podrace_pod_explode;
    extern void *game_cutscenes_podrace_out_of_time;
    extern void *game_cutscenes_bonus_gunship_cavalry_explode;
    extern NUVEC PodRaceADiePos;
    extern NUVEC PodRaceBDiePos;
    extern NUVEC PodRaceCDiePos;
    extern NUVEC GunshipADiePos;
    extern NUVEC GunshipBDiePos;
    extern void *game_cutscenes_podrace_sebulba;
    extern void *game_cutscenes_podsprint_out_of_time;
    extern void *game_cutscenes_podsprint_sebulba;
    extern void *game_cutscenes_dogfight_die;
    i32 CutScene_ReplaceCharacterModelFn_LSW(CUTINFO *cut, CUTSCENECHARACTER *character);
    extern u16 podrace_die_angle_a;
    extern u16 podrace_die_angle_b;
    extern u16 podrace_die_angle_c;
    void CompleteLevel(struct WORLDINFO_s *world);
    extern PC_SURFACE_MASK GetSurfaceInfoDefaultMask;
    extern PC_SURFACE_MASK GetSurfaceInfoRuntimeMask;
    extern struct area_data_s *SPEEDERCHASE_ADATA;

    extern f32 TargetDist_Near2;
    extern const f32 ApplyGravityMinimumFrameTime;
    extern i32 reset_reimport;
    extern GIZMO_BLOWUP_HIT_MULTIPLIER_STORAGE GizmoBlowupHitMultiplier;
    extern u16 ZipUpSwingStartAnimationPc;
    extern u16 ZipUpSwingSwingAnimationPc;
    extern u16 ZipUpSwingJumpAnimationPc;
    extern i16 ZipUpLineCharacterId;

    typedef struct ADAPTIVEDIFFICULTY { // size 0xc
        i32 level;                      // 0x0
        f32 accumulator;                // 0x4
        f32 time_elapsed;               // 0x8
    } ADAPTIVEDIFFICULTY;

    extern f32 AdaptiveDifficultyRecoveryTime;
    f32 AdaptiveDifficultyUpdate(ADAPTIVEDIFFICULTY *);
    f32 AdaptiveDifficultyChange(ADAPTIVEDIFFICULTY *difficulty, f32 amount);
    extern f32 SUPERCARRY_THROWSPEED_XZ;
    extern f32 SUPERCARRY_THROWSPEED_Y;
    extern f32 SUPERCARRY_RELEASESPEED_XZ;
    void StoreLevelProgress(struct WORLDINFO_s *world);
    extern NUVEC GizmoBlowupUnitScale;
// NOLINTNEXTLINE(readability-identifier-naming)
#define TargetDeg_Mid LevInstAnimReset.fields.target_deg_mid
// NOLINTNEXTLINE(readability-identifier-naming)
#define TargetDeg_Far LevInstAnimReset.fields.target_deg_far
#ifdef __cplusplus
}
#endif

typedef struct customiser_completion_reader_s { // size 0x4
    u8 unknown_00[3];                           // 0x0
    u8 flags;                                   // 0x3
} CUSTOMISER_COMPLETION_READER_PC;

#ifdef __cplusplus
extern "C" {
#endif
    extern NUFPCOMJMP WorldLoadAfterKeywordsPc[1];
    extern i32 EpisodeGoldBrickForSuperStory;
    extern i32 EpisodeGoldBrickForSuperBonus;
    extern i32 EpisodeGoldBrickForChallenge;
    extern i16 dat_007f1b44;
    extern i16 dat_007f1b40;
    extern i16 dat_007f1a98;
    extern i32 DefaultMotionLedgeTerrainEnabled;
    extern i32 Grapple_RopeSwingsAvailable;
    extern struct level_data_s *dat_0087b1a8;
    extern struct level_data_s *dat_0087b1bc;
    extern struct level_data_s *dat_0087b1d0;
    extern struct level_data_s *dat_0087b2c4;
    extern i32 Lighting_FlashRedOnLastHeart;
    extern struct area_data_s *UTAPAU_ADATA;
    extern struct area_data_s *HOTH_ADATA;
    extern struct area_data_s *BONUSDAGOBAH_ADATA;
    extern struct area_data_s *BONUSKAMINO_ADATA;
    extern struct area_data_s *BONUSKASHYYYK_ADATA;
    void NewGame(void);
    extern ANIM_REDIRECT_VIEW_PC AnimRedirectList_LSW[41];
    extern ANIM_REDIRECT_VIEW_PC AnimRedirectList_Batman[1];

    extern u8 CheatC3poBitsEnabled;
    extern i32 deathstar_hold_count;
    extern i32 (*PauseRenderOffFn)(void);
    extern f32 minikittime;
#ifdef __cplusplus
}
#endif

struct game_door_callback_s;
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 CompletionPointInfo[7];
    void InitGameAfterConfig(void);
    extern i32 LEGOMENU_TITLES;
    extern i32 LegoGameDebrisSplash;
    extern i16 ShopHintIds[24];
    extern i16 ShopHintTextIds[];
    extern i32 PlayerHitPointsTwoHearts;
    extern i32 ConvertOldPickups;
    extern i32 GrabScreenWhenFading;
    extern i32 TrooperGameDebris[4];
    extern i32 SuperCarryEnable;
    extern i32 CombatRollEnable;
    extern i16 TechnoDefaultObject;
    extern i32 LEGOHINT_PUNCH;
    extern const char *InitAsciiRight;
    extern const char *InitAsciiBigArrow;
    extern const char *LEGOASCII_LEFT;
    extern const char *LEGOASCII_RIGHT;
    extern struct area_data_s *ASTEROIDCHASE_ADATA;
    extern struct area_data_s *DEATHSTARBATTLE2_ADATA;
    extern struct area_data_s *E1CHARACTER_ADATA;
    extern struct area_data_s *E2CHARACTER_ADATA;
    extern struct area_data_s *E3CHARACTER_ADATA;
    extern i32 SuperCarry_UseActionButton;
    extern i16 tCLIPi;
    extern i16 tINTRO;
    extern i16 tMIDTRO;
    extern i16 tOUTRO;
    extern i16 tENDING;
    extern i16 tPOWERUP;

    extern struct area_data_s *JIMTEST_ADATA;
    void LSW_SetIndy(i32 area_index);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern MISSION_SAVE_READER *Game_MissionSave;
    void Game_KillPart(struct PART_PC *part, i32 reason);
    extern GAMEAUDIO GameAudioLsw;
    extern GAMEAUDIO GameAudioAlternate;
    extern struct area_data_s *UTAPAU_ADATA;
    extern struct area_data_s *HOTH_ADATA;
    extern struct area_data_s *BONUSDAGOBAH_ADATA;
    extern struct area_data_s *BONUSKAMINO_ADATA;
    extern struct area_data_s *BONUSKASHYYYK_ADATA;
    extern struct area_data_s *E1CHARACTER_ADATA;
    extern struct area_data_s *E2CHARACTER_ADATA;
    extern struct area_data_s *E3CHARACTER_ADATA;
    extern struct area_data_s *E4CHARACTER_ADATA;
    extern struct area_data_s *E5CHARACTER_ADATA;
    extern struct area_data_s *E6CHARACTER_ADATA;
#ifdef __cplusplus
}
#endif
