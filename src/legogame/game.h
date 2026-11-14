#ifndef SAGA_LEGOGAME_GAME_H
#define SAGA_LEGOGAME_GAME_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: decomp.h, globals.h, legoapi/items/base/apiobject.h,
 * legoapi/items/objects/objectsall.h, legoapi/legoapi_types.h, legoapi/lib/hub.h (not all of it earlier in the shim).
 */

#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/world/level.h"
#include "legoapi/world/missions.h"
#include "legoapi/world/world.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "decomp.h"
#include "globals.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#endif

typedef struct gamesave_s {
    union {
        u32 unknown_0000;

        struct {
            u8 unknown_game_00;

            union {
                u8 difficulty;
                u8 save_version;
            };

            u8 unknown_game_02[2];
        };
    };

    OPTIONSSAVE options_save;
    u8 level_save[0x781b];
    AREA_SAVE_PICKUP_PC area_save[72];

    union {
        struct {
            union {
                u8 unknown_7b8c[0x8f];

                struct {
                    u8 unknown_gold_brick_prefix[0x8e];
                    u8 gold_bricks;
                };
            };

            u8 customizer_weirdo_flags;

            union {
                u8 unknown_7c1c[4];

                struct {
                    u8 hub_buildit_completed;

                    union {
                        u8 losttemple_trailer_watched;
                        u8 lost_temple_area_complete;
                    };

                    u8 unknown_7c1e[2];
                };
            };

            f32 game_time;
            u8 unknown_7c24[0xa];
            u16 customizer_cape_piece;
            u8 unknown_7c30[0x36];
            u16 customizer_secondary_cape_piece;
            u8 unknown_7c68[0x2c];
        };

        struct {
            struct EPISODESAVE_s episode_save[6];
            u8 newgame_reserved_7bd4[0x28];
            u32 initial_store_pack_flags;
            u32 newgame_reserved_7c00;
            u32 purchased_extra_bits_high;

            union {
                u8 newgame_reserved_7c08[0x10];

                struct {
                    union {
                        u8 hud_coin_reserved_7c08[0xc];
                        u32 hint_completion_bits[3];
                    };

                    u32 coins;
                };
            };

            u16 save_hash;
            u8 newgame_reserved_7c1a[0xa];
            GAME_CUSTOMISER_START_PC customiser_start[2];
        };
    };

    MISSION_SAVE_READER_PC mission_save;

    union {
        u8 unknown_7cf8[0x154];
        u8 character_save[0x154];
    };
} GAMESAVE;

STATIC_ASSERT(sizeof(GAMESAVE) == 0x7e4c);
STATIC_ASSERT(offsetof(GAMESAVE, gold_bricks) == 0x7c1a);
STATIC_ASSERT(offsetof(GAMESAVE, difficulty) == 1);
STATIC_ASSERT(offsetof(GAMESAVE, save_version) == 1);
STATIC_ASSERT(offsetof(GAMESAVE, options_save) == 4);
STATIC_ASSERT(offsetof(GAMESAVE, area_save) == 0x782c);
STATIC_ASSERT(offsetof(GAMESAVE, episode_save) == 0x7b8c);
STATIC_ASSERT(offsetof(GAMESAVE, hint_completion_bits) == 0x7c08);
STATIC_ASSERT(offsetof(GAMESAVE, coins) == 0x7c14);
STATIC_ASSERT(offsetof(GAMESAVE, initial_store_pack_flags) == 0x7bfc);
STATIC_ASSERT(offsetof(GAMESAVE, purchased_extra_bits_high) == 0x7c04);
STATIC_ASSERT(offsetof(GAMESAVE, customizer_weirdo_flags) == 0x7c1b);
STATIC_ASSERT(offsetof(GAMESAVE, game_time) == 0x7c20);
STATIC_ASSERT(offsetof(GAMESAVE, customiser_start) == 0x7c24);
STATIC_ASSERT(offsetof(GAMESAVE, customizer_cape_piece) == 0x7c2e);
STATIC_ASSERT(offsetof(GAMESAVE, customizer_secondary_cape_piece) == 0x7c66);
STATIC_ASSERT(offsetof(GAMESAVE, mission_save) == 0x7c94);
STATIC_ASSERT(offsetof(GAMESAVE, mission_save) + offsetof(MISSION_SAVE_READER_PC, completed) == 0x7ce4);
STATIC_ASSERT(offsetof(GAMESAVE, character_save) == 0x7cf8);
STATIC_ASSERT(offsetof(GAMESAVE, difficulty) == 0x1);
STATIC_ASSERT(sizeof(GAMESAVE) == 0x7e4c);
STATIC_ASSERT(offsetof(GAMESAVE, options_save) == 4);
STATIC_ASSERT(offsetof(GAMESAVE, customizer_weirdo_flags) == 0x7c1b);
STATIC_ASSERT(offsetof(GAMESAVE, customizer_cape_piece) == 0x7c2e);
STATIC_ASSERT(offsetof(GAMESAVE, customizer_secondary_cape_piece) == 0x7c66);
STATIC_ASSERT(offsetof(GAMESAVE, mission_save) == 0x7c94);
STATIC_ASSERT(offsetof(GAMESAVE, mission_save) + offsetof(MISSION_SAVE_READER_PC, completed) == 0x7ce4);

// Access prefix only; the original allocation extent remains unknown.
typedef struct area_globals_pc_s {
    i32 field_00;
    i32 field_04;
    i32 field_08;
    i32 area_phase;
    i32 new_minikit_count;
    i32 progress_flags;
    i32 field_18;
    i32 field_1c;
    i32 field_20;
    u32 arcade_player_kills[2];
    u32 arcade_ai_kills[2];
} AREA_GLOBALS_PC;

STATIC_ASSERT(sizeof(AREA_GLOBALS_PC) == 0x34);
STATIC_ASSERT(offsetof(AREA_GLOBALS_PC, area_phase) == 0x0c);
STATIC_ASSERT(offsetof(AREA_GLOBALS_PC, new_minikit_count) == 0x10);
STATIC_ASSERT(offsetof(AREA_GLOBALS_PC, progress_flags) == 0x14);
STATIC_ASSERT(offsetof(AREA_GLOBALS_PC, arcade_player_kills) == 0x24);
STATIC_ASSERT(offsetof(AREA_GLOBALS_PC, arcade_ai_kills) == 0x2c);

typedef struct cutscenecharacter_s {
    u8 opaque_00_40[0x40];
    char *name;
    u8 opaque_44[0x4];
} CUTSCENECHARACTER;

typedef union PC_SURFACE_MASK_PC {
    u8 low;
    u32 word;
} PC_SURFACE_MASK_PC;

STATIC_ASSERT(sizeof(PC_SURFACE_MASK_PC) == 4);
STATIC_ASSERT(offsetof(PC_SURFACE_MASK_PC, low) == 0);
STATIC_ASSERT(offsetof(PC_SURFACE_MASK_PC, word) == 0);
STATIC_ASSERT(offsetof(GAMESAVE, area_save) == 0x782c);
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 CombatReactionHorizontalScalePc;
    /* New public declarations only; private PartCollidePC stays original parts.c-local. */
    void PartCollide_2D(PART_PC *);
    void DeflectPart(PART_PC *, GAMEOBJECT *, f32, f32, i32, i32);
    void InitGameMode(void);

    typedef union GIZMO_BLOWUP_HIT_MULTIPLIER_STORAGE_PC {
        i32 value;
        u8 low_byte;
    } GIZMO_BLOWUP_HIT_MULTIPLIER_STORAGE_PC;

    STATIC_ASSERT(sizeof(GIZMO_BLOWUP_HIT_MULTIPLIER_STORAGE_PC) == 4);
    STATIC_ASSERT(offsetof(GIZMO_BLOWUP_HIT_MULTIPLIER_STORAGE_PC, value) == 0);
    STATIC_ASSERT(offsetof(GIZMO_BLOWUP_HIT_MULTIPLIER_STORAGE_PC, low_byte) == 0);
    extern i32 LegoObjectIconQuestionPc;

    typedef struct CUT_DRAW_SETTINGS_PC {
        i16 blaster_object_0;
        i16 blaster_object_1;
        i16 object_id;
        i8 cheat_id;
        u8 normal_layers;
    } CUT_DRAW_SETTINGS_PC;

    STATIC_ASSERT(sizeof(CUT_DRAW_SETTINGS_PC) == 8);
    STATIC_ASSERT(offsetof(CUT_DRAW_SETTINGS_PC, blaster_object_1) == 2);
    STATIC_ASSERT(offsetof(CUT_DRAW_SETTINGS_PC, object_id) == 4);
    STATIC_ASSERT(offsetof(CUT_DRAW_SETTINGS_PC, cheat_id) == 6);
    STATIC_ASSERT(offsetof(CUT_DRAW_SETTINGS_PC, normal_layers) == 7);
    extern i32 CutSceneLookupAllDebrisPagesPc;
    extern AREA_DATA_PC *JABBASPALACE_ADATA;
    extern AREA_DATA_PC *DAGOBAH_ADATA;
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
    extern char SaveProductCodePc[12];
    i32 CutScene_StartFn_LSW(CUTINFO *cut);
    void CutScene_PreUpdateFn_LSW(CUTINFO *cut);
    void CutScene_PostUpdateFn_LSW(void);
    void CutScene_StoppedFn_LSW(CUTINFO *cut);
    i32 CutScene_ReplaceCharacterModelFn_LSW(CUTINFO *cut, CUTSCENECHARACTER *character);
    i32 InitBolt_AddMomentumType_LSW(BOLT *bolt, GAMEOBJECT *object, NUVEC *momentum);
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
    void CompleteLevel(WORLDINFO *world);
    extern PC_SURFACE_MASK_PC GetSurfaceInfoDefaultMaskPc;
    extern PC_SURFACE_MASK_PC GetSurfaceInfoRuntimeMaskPc;
    extern AREA_DATA_PC *SPEEDERCHASE_ADATA;

    extern f32 TargetDist_Near2;
    extern const f32 ApplyGravityMinimumFrameTime;
    extern i32 reset_reimport;
    extern GIZMO_BLOWUP_HIT_MULTIPLIER_STORAGE_PC GizmoBlowupHitMultiplierPc;
    extern u16 ZipUpSwingStartAnimationPc;
    extern u16 ZipUpSwingSwingAnimationPc;
    extern u16 ZipUpSwingJumpAnimationPc;
    extern i16 ZipUpLineCharacterIdPc;

    typedef struct ADAPTIVEDIFFICULTY_PC {
        i32 level;
        f32 accumulator;
        f32 time_elapsed;
    } ADAPTIVEDIFFICULTY_PC;

    STATIC_ASSERT(sizeof(ADAPTIVEDIFFICULTY_PC) == 0xc);
    STATIC_ASSERT(offsetof(ADAPTIVEDIFFICULTY_PC, level) == 0);
    STATIC_ASSERT(offsetof(ADAPTIVEDIFFICULTY_PC, accumulator) == 4);
    STATIC_ASSERT(offsetof(ADAPTIVEDIFFICULTY_PC, time_elapsed) == 8);
    extern f32 AdaptiveDifficultyRecoveryTimePc;
    f32 AdaptiveDifficultyUpdate(ADAPTIVEDIFFICULTY_PC *);
    f32 AdaptiveDifficultyChange(ADAPTIVEDIFFICULTY_PC *difficulty, f32 amount);
    extern f32 SUPERCARRY_THROWSPEED_XZ;
    extern f32 SUPERCARRY_THROWSPEED_Y;
    extern f32 SUPERCARRY_RELEASESPEED_XZ;
    void StoreLevelProgress(WORLDINFO *world);
    extern NUVEC GizmoBlowupUnitScalePc;
// NOLINTNEXTLINE(readability-identifier-naming)
#define TargetDeg_Mid LevInstAnimResetPc.fields.target_deg_mid
// NOLINTNEXTLINE(readability-identifier-naming)
#define TargetDeg_Far LevInstAnimResetPc.fields.target_deg_far
#ifdef __cplusplus
}
#endif

typedef struct customiser_completion_reader_pc_s {
    u8 unknown_00[3];
    u8 flags;
} CUSTOMISER_COMPLETION_READER_PC;

STATIC_ASSERT(sizeof(CUSTOMISER_COMPLETION_READER_PC) == 4);
STATIC_ASSERT(offsetof(CUSTOMISER_COMPLETION_READER_PC, flags) == 3);
#ifdef __cplusplus
extern "C" {
#endif
    extern NUFPCOMJMP WorldLoadAfterKeywordsPc[1];
    extern i32 EpisodeGoldBrickForSuperStoryPc;
    extern i32 EpisodeGoldBrickForSuperBonusPc;
    extern i32 EpisodeGoldBrickForChallengePc;
    extern i16 FreePlayId007f1b44Pc;
    extern i16 FreePlayId007f1b40Pc;
    extern i16 FreePlayId007f1a98Pc;
    extern i32 DefaultMotionLedgeTerrainEnabledPc;
    extern i32 Grapple_RopeSwingsAvailable;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1a8;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1bc;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1d0;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b2c4;
    extern i32 Lighting_FlashRedOnLastHeart;
    extern AREA_DATA_PC *UTAPAU_ADATA;
    extern AREA_DATA_PC *HOTH_ADATA;
    extern AREA_DATA_PC *BONUSDAGOBAH_ADATA;
    extern AREA_DATA_PC *BONUSKAMINO_ADATA;
    extern AREA_DATA_PC *BONUSKASHYYYK_ADATA;
    void NewGame(void);
    extern ANIM_REDIRECT_VIEW_PC AnimRedirectList_LSW[41];
    extern ANIM_REDIRECT_VIEW_PC AnimRedirectList_Batman[1];

    extern u8 CheatC3poBitsEnabledPc;
    extern i32 deathstar_hold_count;
    extern i32 (*PauseRenderOffFn)(void);
    extern f32 minikittime;
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(GAMESAVE, options_save) == 4);

struct game_door_callback_pc_s;
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 CompletionPointInfoPc[7];
    void InitGameAfterConfig(void);
    extern i32 LEGOMENU_TITLES;
    extern i32 LegoGameDebrisSplashPc;
    extern i16 ShopHintIdsPc[24];
    extern i16 ShopHintTextIdsPc[];
    extern i32 PlayerHitPointsTwoHeartsPc;
    extern i32 ConvertOldPickupsPc;
    extern i32 GrabScreenWhenFadingPc;
    extern i32 TrooperGameDebrisPc[4];
    extern i32 SuperCarryEnablePc;
    extern i32 CombatRollEnablePc;
    extern i16 TechnoDefaultObjectPc;
    extern i32 LEGOHINT_PUNCH;
    extern const char *InitAsciiRightPc;
    extern const char *InitAsciiBigArrowPc;
    extern const char *LEGOASCII_LEFT;
    extern const char *LEGOASCII_RIGHT;
    extern AREA_DATA_PC *ASTEROIDCHASE_ADATA;
    extern AREA_DATA_PC *DEATHSTARBATTLE2_ADATA;
    extern AREA_DATA_PC *E1CHARACTER_ADATA;
    extern AREA_DATA_PC *E2CHARACTER_ADATA;
    extern AREA_DATA_PC *E3CHARACTER_ADATA;
    extern i32 SuperCarry_UseActionButton;
    extern i16 tCLIPi;
    extern i16 tINTRO;
    extern i16 tMIDTRO;
    extern i16 tOUTRO;
    extern i16 tENDING;
    extern i16 tPOWERUP;

    extern AREA_DATA_PC *JIMTEST_ADATA;
    void LSW_SetIndy(i32 area_index);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(GAMESAVE, save_hash) == 0x7c18);
#ifdef __cplusplus
extern "C" {
#endif
    extern MISSION_SAVE_READER_PC *Game_MissionSave;
    void Game_KillPart(struct PART_PC *part, i32 reason);
    extern GAMEAUDIO_PC GameAudioLswPc;
    extern GAMEAUDIO_PC GameAudioAlternatePc;
    extern const char ForceThrowExplodeNamePc[9];
    extern AREA_DATA_PC *UTAPAU_ADATA;
    extern AREA_DATA_PC *HOTH_ADATA;
    extern AREA_DATA_PC *BONUSDAGOBAH_ADATA;
    extern AREA_DATA_PC *BONUSKAMINO_ADATA;
    extern AREA_DATA_PC *BONUSKASHYYYK_ADATA;
    extern AREA_DATA_PC *E1CHARACTER_ADATA;
    extern AREA_DATA_PC *E2CHARACTER_ADATA;
    extern AREA_DATA_PC *E3CHARACTER_ADATA;
    extern AREA_DATA_PC *E4CHARACTER_ADATA;
    extern AREA_DATA_PC *E5CHARACTER_ADATA;
    extern AREA_DATA_PC *E6CHARACTER_ADATA;
#ifdef __cplusplus
}
#endif

#endif
