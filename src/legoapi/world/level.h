/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_WORLD_LEVEL_H_PART
#define SAGA_LEGOAPI_WORLD_LEVEL_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/render/core/screen.h"
#include "legoapi/world/levels.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_WORLD_LEVEL_H_PART == 1 && !defined(SAGA_LEGOAPI_WORLD_LEVEL_H_STOP)

struct BOLTTYPE_PC_s;
struct WORLDINFO_s;
struct bolt_s;
struct cutinfo_s;
struct cutsys_s;
struct gameobject_s;
struct nufpcomjmp_s;
struct numtx_s;

typedef struct level_progress_s { // size 0x2e28
    SCENEPROGRESS scene_progress; // 0x0000

    union { // size 0x14
        u8 unknown_2800[0x14];
        u32 flags;
    }; // 0x2800

    f32 minimum_tfactor; // 0x2814

    union { // size 0x8
        u8 unknown_2818[8];

        struct {                               // size 0x8
            u8 unknown_2818_prefix_pc[4];      // 0x2818
            u32 destroyed_trooper_cannon_mask; // 0x281c
        };
    }; // 0x2818

    u32 progress_state; // 0x2820

    union { // size 0x544
        u8 unknown_2824[0x544];

        struct {                         // size 0x544
            u8 unknown_2824_pc[0x400];   // 0x2824
            u8 giz_flow_progress[0x144]; // 0x2c24
        };
    }; // 0x2824

    char disabled_effect_names[12][16]; // 0x2d68
} LEVEL_PROGRESS;

#ifdef __cplusplus
extern "C" {
#endif
    i32 LevelObject_FindIndexFromName_RefOnly(char *);
    i32 LevelObject_GetReflection(i32);
    void KillBossCompleteLevel(i32, i32, f32);
    i32 KillBossPlayCutScene(i32, i32, f32, char *);
    i32 KillBoss(i32, i32, f32);
    void *SetLevelHack(i32 size);
    extern i32 LEVELCOUNT;
    void GoToNewLevel(i32 level_index);
    i32 LevelObject_FindIndexFromName(char *);
    i32 LevelObject_AddExtra(char *, i32);
    i32 Text_StripComments(char *text, char *dest, i32 separators);
    extern const char LevelConfigKeyword_xspeed[7];
    char *LevelObject_FindNameFromIndex(i32);
    extern i32 LEGOSPL_SPLIT;

    extern i32 LEGOSPL_SPLIT;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_LEVEL_H_PART
#define SAGA_LEGOAPI_WORLD_LEVEL_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_LEVEL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_LEVEL_H_PART == 2 && !defined(SAGA_LEGOAPI_WORLD_LEVEL_H_STOP)

typedef struct LEVEL_OBJECT_RUNTIME { // size 0x10
    NUSPECIALHANDLE special;          // 0x0
    u8 unknown_c[2];                  // 0xc
    u8 active;                        // 0xe
    u8 padding_f;                     // 0xf
} LEVEL_OBJECT_RUNTIME;

#undef SAGA_LEGOAPI_WORLD_LEVEL_H_PART
#define SAGA_LEGOAPI_WORLD_LEVEL_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_LEVEL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_LEVEL_H_PART == 3 && !defined(SAGA_LEGOAPI_WORLD_LEVEL_H_STOP)

typedef struct level_data_s { // size 0x130
    char dir[0x40];           // 0x000
    char name[0x20];          // 0x040

    union { // size 0x2
        i16 unknown_0060;
        u16 title_text_id;
    }; // 0x060

    i16 index;                // 0x062
    u32 flags;                // 0x064
    void *callbacks[7];       // 0x068
    LEVEL_DISPLAY_PC display; // 0x084
    i16 music_index;          // 0x094
    i16 sfx_ambient;          // 0x096
    i16 max_ter_platforms;    // 0x098
    i16 max_ter_groups;       // 0x09a
    i16 music_other_index;    // 0x09c
    i16 unknown_009e;         // 0x09e
    u8 mipmap_mode;           // 0x0a0
    u8 blob_shadow_alpha;     // 0x0a1

    union { // size 0x1
        u8 unknown_00a2;
        u8 episode;
    }; // 0x0a2

    i8 area_index;                 // 0x0a3
    f32 cam_tilt;                  // 0x0a4
    f32 hover_height;              // 0x0a8
    u8 water_ripple_start_rgba[4]; // 0x0ac
    u8 water_ripple_end_rgba[4];   // 0x0b0
    f32 water_ripple_life;         // 0x0b4
    f32 cam_pullback_dist;         // 0x0b8
    f32 cam_lateral_dist;          // 0x0bc
    f32 reflection_y;              // 0x0c0
    i32 shadow_type;               // 0x0c4
    i8 area_level_index;           // 0x0c8
    u8 blob_shadow_fade_near;      // 0x0c9
    u8 blob_shadow_fade_far;       // 0x0ca
    u8 cam_pos_seek;               // 0x0cb
    u8 cam_angle_seek;             // 0x0cc
    u8 reflection_range;           // 0x0cd
    u8 raycast_step;               // 0x0ce
    u8 platform_scan_distance;     // 0x0cf
    f32 conveyor_x_speed;          // 0x0d0
    f32 conveyor_z_speed;          // 0x0d4
    i16 max_gameantinodes;         // 0x0d8
    i16 max_gizmoblowups;          // 0x0da
    i16 max_gizmoblowuptypes;      // 0x0dc
    i16 max_pickups;               // 0x0de
    i16 max_obstacle_objects;      // 0x0e0
    i16 max_buildit_objects;       // 0x0e2
    i16 max_force_objects;         // 0x0e4
    i16 max_bombgen_objects;       // 0x0e6
    u8 max_tightropes;             // 0x0e8
    u8 max_timers;                 // 0x0e9
    u8 max_signals;                // 0x0ea
    u8 max_levers;                 // 0x0eb
    u8 max_technos;                // 0x0ec
    u8 max_zipups;                 // 0x0ed
    u8 max_grapples;               // 0x0ee
    u8 max_obstacles;              // 0x0ef
    u8 max_buildits;               // 0x0f0
    u8 max_shards;                 // 0x0f1
    u8 max_spinners;               // 0x0f2
    u8 max_minicuts;               // 0x0f3
    u8 max_minicut_stages;         // 0x0f4
    u8 max_gizspecials;            // 0x0f5
    u8 max_attractos;              // 0x0f6
    u8 max_climb_objects;          // 0x0f7
    u8 max_guidelines;             // 0x0f8
    u8 max_ledges;                 // 0x0f9
    u8 max_securitydoors;          // 0x0fa
    u8 max_tubes;                  // 0x0fb
    u8 max_gizpanels;              // 0x0fc
    u8 max_hatmachines;            // 0x0fd
    u8 max_force;                  // 0x0fe
    u8 max_pushblocks;             // 0x0ff
    u8 max_pushblock_endpos;       // 0x100
    u8 max_doors;                  // 0x101
    u8 max_teleports;              // 0x102
    u8 max_gizrandoms;             // 0x103
    u8 max_torpmachines;           // 0x104
    u8 max_spinneranim_objs;       // 0x105
    u8 max_turrets;                // 0x106
    u8 max_bombgens;               // 0x107
    u8 max_bridges;                // 0x108
    u8 max_plugs;                  // 0x109
    u8 unknown_010a[6];            // 0x10a
    f32 wind_speed;                // 0x110
    f32 wind_size;                 // 0x114
    i32 music_tracks[6];           // 0x118
} LEVELDATA_PC;

typedef struct level_fixup_pc_s { // size 0x24
    char *name;                   // 0x00
    LEVELDATA_PC **level;         // 0x04
    void *load;                   // 0x08
    void *init;                   // 0x0c
    void *reset;                  // 0x10
    void *update;                 // 0x14
    void *always_update;          // 0x18
    void *draw;                   // 0x1c
    void *draw_status;            // 0x20
} LEVELFIXUP_PC;

#ifdef __cplusplus
extern "C" {
#endif
    extern LEVELDATA_PC *TITLES_LDATA;
    struct cutinfo_s *NewCutScene(struct cutinfo_s *cutscene, struct cutsys_s *system, char *name, i32 unused_4);
    void Levels_FixUp(struct level_fixup_pc_s *levels);
    LEVELDATA_PC *Level_FindByName(char *name, i32 *index);
    extern LEVELDATA_PC *LDataList;
    extern LEVELDATA_PC *NEWGAME_LDATA;
    extern LEVELDATA_PC *LOADGAME_LDATA;
    extern LEVELDATA_PC *LastLData;
    extern LEVELDATA_PC *NewLData;
    void ClearLevelProgress(i32 index, struct WORLDINFO_s *world);
    struct BOLTTYPE_PC_s *BoltType_FindByID(i32, struct WORLDINFO_s *);
    void ResetLevel(struct WORLDINFO_s *world, char *cutscene, i32 unused);
    void Level_SetDefaults(LEVELDATA_PC *level);
    extern LEVELDATA_PC *levelconfig_ldata;
    i32 BoltType_FindIDByName(const char *, struct WORLDINFO_s *);
    void Level_Update(struct WORLDINFO_s *world);
    void Level_Draw(struct WORLDINFO_s *world);
    void FixUpLevels(struct level_fixup_pc_s *fixup);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void Level_RegisterGameConfigKeywords(struct nufpcomjmp_s *, struct nufpcomjmp_s *);
    void InitMiniSnowTroopers(struct WORLDINFO_s *arg0, i32 arg1, i32 arg2, i32 arg3);
    void UpdateMiniSnowTroopers(struct WORLDINFO_s *arg0);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_LEVEL_H_PART
#define SAGA_LEGOAPI_WORLD_LEVEL_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_LEVEL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_LEVEL_H_PART == 4 && !defined(SAGA_LEGOAPI_WORLD_LEVEL_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    struct bolt_s *Bolt_Add(struct gameobject_s *, NUVEC *, struct numtx_s *, i32, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_WORLD_LEVEL_H_PART
#define SAGA_LEGOAPI_WORLD_LEVEL_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_LEVEL_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_WORLD_LEVEL_H_STOP
