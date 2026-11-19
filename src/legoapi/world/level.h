#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizflow.h"
#include "legoapi/world/sceneprogress.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/nuvec.h"

struct BOLTTYPE_s;
struct WORLDINFO_s;
struct bolt_s;
struct cutinfo_s;
struct cutsys_s;
struct gameobject_s;
struct nufpcomjmp_s;
struct numtx_s;

typedef struct level_progress_s { // size 0x2e28
    SCENEPROGRESS scene_progress; // 0x0000

    /* bit 0 the level is complete */
    u32 flags;         // 0x2800
    u32 reserved_2804; // 0x2804

    union { // size 0x8 StoreProgressAICharacter: one bit per level AI object that is switched off
        u32 disabled_ai_object_mask[2];
        u64 disabled_ai_objects;
    }; // 0x2808

    /* Grabber_StoreProgress; y 2000000 when none is stored */
    NUVEC grabber_position;            // 0x2810
    u32 destroyed_trooper_cannon_mask; // 0x281c
    u32 progress_state;                // 0x2820
    LEVEL_SCRIPT_PROGRESS scripts[32]; // 0x2824
    GIZFLOWPROGRESS giz_flow_progress; // 0x2c24

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

typedef struct LEVEL_OBJECT_RUNTIME { // size 0x10
    nuhspecial_s special;             // 0x0
    i16 platform_id;                  // 0xc
    u8 active;                        // 0xe
    u8 padding_f;                     // 0xf
} LEVEL_OBJECT_RUNTIME;

/* level_data_s.flags: levels\\levels.txt keywords (test_level, intro_level, ...) and the level config switches
 * (LevelConfigSetFlag). 0x4 is set on every level by default; its meaning is not known yet. */
enum LEVEL_FLAGS {
    LEVEL_CONFIG_LOADED = 0x1,                   /* set once the level's config file has been read */
    LEVEL_GAMEPLAY = 0x2,                        /* a playable level: AI and gizmos load (status_level clears it) */
    LEVEL_TERRAIN = 0x8,                         /* the level has terrain (status_level clears it) */
    LEVEL_FLAT_TERRAIN = 0x10,                   /* flat_terrain */
    LEVEL_INTRO = 0x20,                          /* intro_level */
    LEVEL_MIDTRO = 0x40,                         /* midtro_level, cutscene_level */
    LEVEL_OUTRO = 0x80,                          /* outro_level */
    LEVEL_FIX_STROBING_ANIMS = 0x100,            /* fix_strobing_anims */
    LEVEL_TEST = 0x200,                          /* test_level */
    LEVEL_STATUS = 0x400,                        /* status_level */
    LEVEL_DOUBLE_SCORE = 0x800,                  /* double_score */
    LEVEL_METAL = 0x1000,                        /* metal */
    LEVEL_SHOW_COIN_TOTAL = 0x2000,              /* story levels of an episode: Super Story draws the coin total */
    LEVEL_CAMERA_RAIN = 0x4000,                  /* camera_rain */
    LEVEL_TERRAIN_RAIN = 0x8000,                 /* terrain_rain */
    LEVEL_NEWGAME = 0x10000,                     /* newgame_level */
    LEVEL_LOADGAME = 0x20000,                    /* loadgame_level */
    LEVEL_IN_SPACE = 0x40000,                    /* in_space */
    LEVEL_PICKUPS_TO_PANEL = 0x80000,            /* pickups_to_panel */
    LEVEL_FORGET_TAKEOVERS = 0x100000,           /* forget_takeovers */
    LEVEL_NARROW_SOCKS = 0x200000,               /* narrow_socks */
    LEVEL_OVERRIDE_NO_PICKUP_GRAVITY = 0x400000, /* override_nopickupgravity */
    LEVEL_HIDE_ICONS = 0x800000                  /* hidden_icons off */
};

typedef struct level_display_s { // size 0x10
    f32 farclip_hack;            // 0x0
    f32 near_clip;               // 0x4
    i16 far_clip;                // 0x8
    u8 bg_red_top;               // 0xa
    u8 bg_red_bottom;            // 0xb
    u8 bg_green_top;             // 0xc
    u8 bg_green_bottom;          // 0xd
    u8 bg_blue_top;              // 0xe
    u8 bg_blue_bottom;           // 0xf
} LEVEL_DISPLAY;

typedef struct level_data_s { // size 0x130
    char dir[0x40];           // 0x000
    char name[0x20];          // 0x040

    union { // size 0x2
        i16 override_index;
        u16 title_text_id;
    }; // 0x060

    i16 index; // 0x062
    u32 flags; // 0x064

    union { // size 0x1c
        void *callbacks[7];

        struct {                                                                      // size 0x1c
            void (*load)(struct WORLDINFO_s *, union variptr_u *, union variptr_u *); // 0x068
            void (*init)(struct WORLDINFO_s *);                                       // 0x06c
            void (*reset)(struct WORLDINFO_s *);                                      // 0x070
            void (*update)(struct WORLDINFO_s *);                                     // 0x074
            void (*always_update)(struct WORLDINFO_s *);                              // 0x078
            void (*draw)(struct WORLDINFO_s *);                                       // 0x07c
            void (*draw_panel)(struct WORLDINFO_s *);                                 // 0x080
        };
    }; // 0x068

    LEVEL_DISPLAY display; // 0x084
    i16 music_index;       // 0x094
    i16 sfx_ambient;       // 0x096
    i16 max_ter_platforms; // 0x098
    i16 max_ter_groups;    // 0x09a
    i16 music_other_index; // 0x09c
    i16 secondary_index;   // 0x09e
    u8 mipmap_mode;        // 0x0a0
    u8 blob_shadow_alpha;  // 0x0a1

    union { // size 0x1
        u8 dfs_mode;
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
} LEVELDATA;

typedef struct level_fixup_s { // size 0x24
    char *name;                // 0x00
    LEVELDATA **level;         // 0x04
    void *load;                // 0x08
    void *init;                // 0x0c
    void *reset;               // 0x10
    void *update;              // 0x14
    void *always_update;       // 0x18
    void *draw;                // 0x1c
    void *draw_status;         // 0x20
} LEVELFIXUP;

#ifdef __cplusplus
extern "C" {
#endif
    extern LEVELDATA *TITLES_LDATA;
    struct cutinfo_s *NewCutScene(struct cutinfo_s *cutscene, struct cutsys_s *system, char *name, i32 unused_4);
    void Levels_FixUp(struct level_fixup_s *levels);
    LEVELDATA *Level_FindByName(char *name, i32 *index);
    extern LEVELDATA *LDataList;
    extern LEVELDATA *NEWGAME_LDATA;
    extern LEVELDATA *LOADGAME_LDATA;
    extern LEVELDATA *LastLData;
    extern LEVELDATA *NewLData;
    void ClearLevelProgress(i32 index, struct WORLDINFO_s *world);
    struct BOLTTYPE_s *BoltType_FindByID(i32, struct WORLDINFO_s *);
    void ResetLevel(struct WORLDINFO_s *world, char *cutscene, i32 unused);
    void Level_SetDefaults(LEVELDATA *level);
    extern LEVELDATA *levelconfig_ldata;
    i32 BoltType_FindIDByName(const char *, struct WORLDINFO_s *);
    void Level_Update(struct WORLDINFO_s *world);
    void Level_Draw(struct WORLDINFO_s *world);
    void FixUpLevels(struct level_fixup_s *fixup);
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

#ifdef __cplusplus
extern "C" {
#endif
    struct bolt_s *Bolt_Add(struct gameobject_s *, NUVEC *, struct numtx_s *, i32, i32);
    void LevelStreaming_DoorOverride(struct WORLDINFO_s *world, LEVELDATA *level, f32 radius, f32 *distance);
    extern LEVELDATA *Levels_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count, i32 *count_out,
                                           void *set_defaults_fn);
    void LevelConfig_AfterLoad(LEVELDATA *, char *, struct nufpcomjmp_s *);
    void LevelConfig_BeforeLoad(LEVELDATA *, char *, struct nufpcomjmp_s *);
    void Level_LoadConfigFile(struct WORLDINFO_s *);
#ifdef __cplusplus
}
#endif
