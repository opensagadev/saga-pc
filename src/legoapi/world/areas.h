#pragma once

#include "nu2api/nucore/common.h"

struct nuvec_s;

struct WORLDINFO_s;

typedef struct area_supercounter_s AREA_SUPERCOUNTER;

typedef struct area_supercounter_pickup_s { // size 0x30
    void *gizmo;                            // 0x00
    void *position_gizmo;                   // 0x04
    void *position_special[3];              // 0x08
    char name[8];                           // 0x14
    char position_name[16];                 // 0x1c
    i16 level_index;                        // 0x2c
    u8 use_special;                         // 0x2e
    u8 unknown_002f;                        // 0x2f
} AREA_SUPERCOUNTER_PICKUP;

#ifdef __cplusplus
extern "C" {
#endif
    void Areas_ConfigureResidents(VARIPTR *buffer, VARIPTR *buffer_end);
    void SuperCounters_Reset(i32 area_index);
    extern i32 openlevels;
    extern i32 last_area;
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

typedef struct AREA_EXTRA_MODEL { // size 0x8
    i16 *model_list;              // 0x0
    i16 *extra_id;                // 0x4
} AREA_EXTRA_MODEL;

#ifdef __cplusplus
extern "C" {
#endif
    extern AREA_EXTRA_MODEL ExtraModelList[6];

    void Area_Configure(i32, i32, AREA_EXTRA_MODEL *, i16 *);
    extern i32 AreaMusic;
    extern i32 Area_FreePlayModelCount;
    extern i32 LevelLoadCount;
    extern i32 Area_MissionModelCount;
    extern i32 Area_StoryModelCount;
#ifdef __cplusplus
}
#endif

struct area_supercounter_s {              // size 0x1e8
    AREA_SUPERCOUNTER_PICKUP pickups[10]; // 0x000
    u16 pickup_count;                     // 0x1e0
    u16 collected_count;                  // 0x1e2
    u8 red;                               // 0x1e4
    u8 green;                             // 0x1e5
    u8 blue;                              // 0x1e6
    u8 processed_flags;                   // 0x1e7
};

#ifdef __cplusplus
extern "C" {
#endif
    void SuperCounters_FixUpGizmos(struct WORLDINFO_s *world);
    AREA_SUPERCOUNTER *SuperCounter_FindFromNameAndLevel(char *name, struct WORLDINFO_s *world, AREA_SUPERCOUNTER_PICKUP **pickup_out);
    void SuperCounters_ResetProcessed(struct WORLDINFO_s *world);
    struct area_supercounter_s *SuperCounters_FindPickup(struct WORLDINFO_s *arg0, void *arg1, struct nuvec_s *arg2, void *arg3);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    // Access prefix only; the original allocation extent remains unknown.
    typedef struct area_globals_s { // size 0x34
        i32 unknown_00;             // 0x00
        i32 unknown_04;             // 0x04
        i32 unknown_08;             // 0x08
        i32 area_phase;             // 0x0c
        i32 new_minikit_count;      // 0x10
        i32 progress_flags;         // 0x14
        i32 unknown_18;             // 0x18
        i32 unknown_1c;             // 0x1c
        i32 unknown_20;             // 0x20
        u32 arcade_player_kills[2]; // 0x24
        u32 arcade_ai_kills[2];     // 0x2c
    } AREA_GLOBALS;

    extern AREA_GLOBALS AreaGlobals;
#ifdef __cplusplus
}
#endif
