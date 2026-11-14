#ifndef SAGA_LEGOAPI_WORLD_AREAS_H_PART
#define SAGA_LEGOAPI_WORLD_AREAS_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/characters/core/characters.h, legoapi/world/world.h, legogame/game.h,
 * nu2api/nucore/common.h, nu2api/nucore/fixed_width.h, nu2api/numath/nuvec.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/characters/core/characters.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"
#endif
/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_WORLD_AREAS_H_PART == 1 && !defined(SAGA_LEGOAPI_WORLD_AREAS_H_STOP)

typedef struct area_supercounter_pc_s AREA_SUPERCOUNTER_PC;

typedef struct area_supercounter_pickup_pc_s {
    void *gizmo;
    void *position_gizmo;
    void *position_special[3];
    char name[8];
    char position_name[16];
    i16 level_index;
    u8 use_special;
    u8 unknown_002f;
} AREA_SUPERCOUNTER_PICKUP_PC;

STATIC_ASSERT(sizeof(AREA_SUPERCOUNTER_PICKUP_PC) == 0x30);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PICKUP_PC, name) == 0x14);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PICKUP_PC, position_name) == 0x1c);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PICKUP_PC, level_index) == 0x2c);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PICKUP_PC, use_special) == 0x2e);
#ifdef __cplusplus
extern "C" {
#endif
    void Areas_ConfigureResidents(VARIPTR *buffer, VARIPTR *buffer_end);
    void SuperCounters_Reset(i32 area_index);
    extern i32 openlevels;
    extern i32 last_area;
    extern const char LevelConfigKeywordFile[5];
    extern const char LevelConfigKeywordDir[4];
    extern const char LevelConfigKeywordStart[12];
    extern const char LevelConfigKeywordEnd[10];
    extern const char LevelConfigKeywordTest[11];
    extern const char LevelConfigKeywordIntro[12];
    extern const char LevelConfigKeywordMidtro[13];
    extern const char LevelConfigKeywordCutscene[15];
    extern const char LevelConfigKeywordOutro[12];
    extern const char LevelConfigKeywordStatus[13];
    extern const char LevelConfigKeywordNewGame[14];
    extern const char LevelConfigKeywordLoadGame[15];
    extern const char LevelConfigGenericKeyword_mipmapmode[11];
    extern const char LevelConfigGenericKeyword_fix_strobing_anims[19];
    extern const char LevelConfigGenericKeyword_max_ter_groups[15];
    extern const char LevelConfigGenericKeyword_max_ter_platforms[18];
    extern const char LevelConfigGenericKeyword_backr[6];
    extern const char LevelConfigGenericKeyword_backg[6];
    extern const char LevelConfigGenericKeyword_backb[6];
    extern const char LevelConfigGenericKeyword_backr_top[10];
    extern const char LevelConfigGenericKeyword_backg_top[10];
    extern const char LevelConfigGenericKeyword_backb_top[10];
    extern const char LevelConfigGenericKeyword_backr_bottom[13];
    extern const char LevelConfigGenericKeyword_backg_bottom[13];
    extern const char LevelConfigGenericKeyword_backb_bottom[13];
    extern const char LevelConfigGenericKeyword_farclip_hack[13];
    extern const char LevelConfigGenericKeyword_farclip[8];
    extern const char LevelConfigGenericKeyword_nearclip[9];
    extern const char LevelConfigGenericKeyword_farclip_pc[11];
    extern const char LevelConfigGenericKeyword_nearclip_pc[12];
    extern const char LevelConfigGenericKeyword_hover_height[13];
    extern const char LevelConfigGenericKeyword_blobshadow_alpha[17];
    extern const char LevelConfigGenericKeyword_blobshadow_fadenear[20];
    extern const char LevelConfigGenericKeyword_blobshadow_fadefar[19];
    extern const char LevelConfigGenericKeyword_reflect_range[14];
    extern const char AreaConfigKeyword_redbrick_cheat[15];
    extern const char AreaConfigKeyword_timetrial_time[15];
    extern const char AreaConfigKeyword_text_id[8];
    extern const char AreaConfigKeyword_name_id[8];
    extern const char AreaConfigKeyword_no_freeplay[12];
    extern const char AreaConfigKeyword_no_completion_points[21];
    extern const char AreaConfigKeyword_no_gold_brick[14];
    extern const char AreaConfigKeyword_nopickupgravity[16];
    extern const char AreaConfigKeyword_no_character_collisions[24];
    extern const char AreaConfigKeyword_no_character_collision[23];
    extern const char AreaConfigKeyword_nocharactercollisions[22];
    extern const char AreaConfigKeyword_nocharactercollision[21];
    extern const char AreaConfigKeyword_super_bonus_area[17];
    extern const char AreaConfigKeyword_bonus_area[11];
    extern const char AreaConfigKeyword_ending_area[12];
#ifdef __cplusplus
}
#endif

typedef struct AREA_EXTRA_MODEL_PC {
    i16 *model_list;
    i16 *extra_id;
} AREA_EXTRA_MODEL_PC;

STATIC_ASSERT(sizeof(AREA_EXTRA_MODEL_PC) == 8);
STATIC_ASSERT(offsetof(AREA_EXTRA_MODEL_PC, model_list) == 0);
STATIC_ASSERT(offsetof(AREA_EXTRA_MODEL_PC, extra_id) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    void Area_Configure(i32, i32, AREA_EXTRA_MODEL_PC *, i16 *);
    extern i32 AreaMusic;
    extern i32 Area_FreePlayModelCount;
    extern i32 LevelLoadCount;
    extern i32 Area_MissionModelCount;
    extern i32 Area_StoryModelCount;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_AREAS_H_PART
#define SAGA_LEGOAPI_WORLD_AREAS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_AREAS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_AREAS_H_PART == 2 && !defined(SAGA_LEGOAPI_WORLD_AREAS_H_STOP)

struct area_supercounter_pc_s {
    AREA_SUPERCOUNTER_PICKUP_PC pickups[10];
    u16 pickup_count;
    u16 collected_count;
    u8 red;
    u8 green;
    u8 blue;
    u8 processed_flags;
};

STATIC_ASSERT(sizeof(AREA_SUPERCOUNTER_PC) == 0x1e8);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PC, pickup_count) == 0x1e0);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PC, collected_count) == 0x1e2);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PC, processed_flags) == 0x1e7);
#ifdef __cplusplus
extern "C" {
#endif
    void SuperCounters_FixUpGizmos(WORLDINFO *world);
    AREA_SUPERCOUNTER_PC *SuperCounter_FindFromNameAndLevel(char *name, WORLDINFO *world,
                                                            AREA_SUPERCOUNTER_PICKUP_PC **pickup_out);
    void SuperCounters_ResetProcessed(WORLDINFO *world);
    struct area_supercounter_pc_s *SuperCounters_FindPickup(struct WORLDINFO_s *arg0, void *arg1, struct nuvec_s *arg2,
                                                            void *arg3);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_AREAS_H_PART
#define SAGA_LEGOAPI_WORLD_AREAS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_AREAS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_AREAS_H_PART == 3 && !defined(SAGA_LEGOAPI_WORLD_AREAS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern APICHARACTERMODELLIST Area_FreePlayModelList[49];
    extern APICHARACTERMODELLIST Area_PlayerModelList[9];
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_AREAS_H_PART
#define SAGA_LEGOAPI_WORLD_AREAS_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_AREAS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_AREAS_H_PART == 4 && !defined(SAGA_LEGOAPI_WORLD_AREAS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern AREA_GLOBALS_PC AreaGlobals_PC;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_AREAS_H_PART
#define SAGA_LEGOAPI_WORLD_AREAS_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_AREAS_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_WORLD_AREAS_H_STOP
