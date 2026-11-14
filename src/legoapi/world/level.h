#ifndef SAGA_LEGOAPI_WORLD_LEVEL_H_PART
#define SAGA_LEGOAPI_WORLD_LEVEL_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/render/core/screen.h"
#include "legoapi/world/levels.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/nuvec.h"
#endif

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
struct cutsys_pc_s;
struct gameobject_s;
struct nufpcomjmp_s;
struct numtx_s;

typedef struct level_progress_pc_s {
    SCENEPROGRESS scene_progress;

    union {
        u8 unknown_2800[0x14];
        u32 flags;
    };

    f32 minimum_tfactor;

    union {
        u8 unknown_2818[8];

        struct {
            u8 unknown_2818_prefix_pc[4];
            u32 destroyed_trooper_cannon_mask;
        };
    };

    u32 progress_state;

    union {
        u8 unknown_2824[0x544];

        struct {
            u8 unknown_2824_pc[0x400];
            u8 giz_flow_progress[0x144];
        };
    };

    char disabled_effect_names[12][16];
} LEVEL_PROGRESS;

STATIC_ASSERT(sizeof(LEVEL_PROGRESS) == 0x2e28);
STATIC_ASSERT(offsetof(LEVEL_PROGRESS, minimum_tfactor) == 0x2814);
STATIC_ASSERT(offsetof(LEVEL_PROGRESS, progress_state) == 0x2820);
STATIC_ASSERT(offsetof(LEVEL_PROGRESS, disabled_effect_names) == 0x2d68);
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
STATIC_ASSERT(offsetof(LEVEL_PROGRESS, destroyed_trooper_cannon_mask) == 0x281c);
#undef SAGA_LEGOAPI_WORLD_LEVEL_H_PART
#define SAGA_LEGOAPI_WORLD_LEVEL_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_LEVEL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_LEVEL_H_PART == 2 && !defined(SAGA_LEGOAPI_WORLD_LEVEL_H_STOP)

typedef struct LEVEL_OBJECT_RUNTIME_PC {
    NUSPECIALHANDLE special;
    u8 unknown_c[2];
    u8 active;
    u8 padding_f;
} LEVEL_OBJECT_RUNTIME_PC;

STATIC_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, special) == 0);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 16);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 14);
#undef SAGA_LEGOAPI_WORLD_LEVEL_H_PART
#define SAGA_LEGOAPI_WORLD_LEVEL_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_WORLD_LEVEL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_WORLD_LEVEL_H_PART == 3 && !defined(SAGA_LEGOAPI_WORLD_LEVEL_H_STOP)

typedef struct level_data_pc_s {
    char dir[0x40];
    char name[0x20];

    union {
        i16 unknown_0060;
        u16 title_text_id;
    };

    i16 index;
    u32 flags;
    void *callbacks[7];
    LEVEL_DISPLAY_PC display;
    i16 music_index;
    i16 sfx_ambient;
    i16 max_ter_platforms;
    i16 max_ter_groups;
    i16 music_other_index;
    i16 unknown_009e;
    u8 mipmap_mode;
    u8 blob_shadow_alpha;

    union {
        u8 unknown_00a2;
        u8 episode;
    };

    i8 area_index;
    f32 cam_tilt;
    f32 hover_height;
    u8 water_ripple_start_rgba[4];
    u8 water_ripple_end_rgba[4];
    f32 water_ripple_life;
    f32 cam_pullback_dist;
    f32 cam_lateral_dist;
    f32 reflection_y;
    i32 shadow_type;
    i8 area_level_index;
    u8 blob_shadow_fade_near;
    u8 blob_shadow_fade_far;
    u8 cam_pos_seek;
    u8 cam_angle_seek;
    u8 reflection_range;
    u8 raycast_step;
    u8 platform_scan_distance;
    f32 conveyor_x_speed;
    f32 conveyor_z_speed;
    i16 max_gameantinodes;
    i16 max_gizmoblowups;
    i16 max_gizmoblowuptypes;
    i16 max_pickups;
    i16 max_obstacle_objects;
    i16 max_buildit_objects;
    i16 max_force_objects;
    i16 max_bombgen_objects;
    u8 max_tightropes;
    u8 max_timers;
    u8 max_signals;
    u8 max_levers;
    u8 max_technos;
    u8 max_zipups;
    u8 max_grapples;
    u8 max_obstacles;
    u8 max_buildits;
    u8 max_shards;
    u8 max_spinners;
    u8 max_minicuts;
    u8 max_minicut_stages;
    u8 max_gizspecials;
    u8 max_attractos;
    u8 max_climb_objects;
    u8 max_guidelines;
    u8 max_ledges;
    u8 max_securitydoors;
    u8 max_tubes;
    u8 max_gizpanels;
    u8 max_hatmachines;
    u8 max_force;
    u8 max_pushblocks;
    u8 max_pushblock_endpos;
    u8 max_doors;
    u8 max_teleports;
    u8 max_gizrandoms;
    u8 max_torpmachines;
    u8 max_spinneranim_objs;
    u8 max_turrets;
    u8 max_bombgens;
    u8 max_bridges;
    u8 max_plugs;
    u8 unknown_010a[6];
    f32 wind_speed;
    f32 wind_size;
    i32 music_tracks[6];
} LEVELDATA_PC;

STATIC_ASSERT(sizeof(LEVELDATA_PC) == 0x130);
STATIC_ASSERT(offsetof(LEVELDATA_PC, dir) == 0x00);
STATIC_ASSERT(offsetof(LEVELDATA_PC, name) == 0x40);
STATIC_ASSERT(offsetof(LEVELDATA_PC, index) == 0x62);
STATIC_ASSERT(offsetof(LEVELDATA_PC, flags) == 0x64);
STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) == 0x68);
STATIC_ASSERT(offsetof(LEVELDATA_PC, display) == 0x84);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_index) == 0x94);
STATIC_ASSERT(offsetof(LEVELDATA_PC, sfx_ambient) == 0x96);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_ter_platforms) == 0x98);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_other_index) == 0x9c);
STATIC_ASSERT(offsetof(LEVELDATA_PC, mipmap_mode) == 0xa0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, blob_shadow_alpha) == 0xa1);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_index) == 0xa3);
STATIC_ASSERT(offsetof(LEVELDATA_PC, cam_tilt) == 0xa4);
STATIC_ASSERT(offsetof(LEVELDATA_PC, hover_height) == 0xa8);
STATIC_ASSERT(offsetof(LEVELDATA_PC, water_ripple_start_rgba) == 0xac);
STATIC_ASSERT(offsetof(LEVELDATA_PC, water_ripple_end_rgba) == 0xb0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, water_ripple_life) == 0xb4);
STATIC_ASSERT(offsetof(LEVELDATA_PC, cam_pullback_dist) == 0xb8);
STATIC_ASSERT(offsetof(LEVELDATA_PC, cam_lateral_dist) == 0xbc);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_y) == 0xc0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, shadow_type) == 0xc4);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_level_index) == 0xc8);
STATIC_ASSERT(offsetof(LEVELDATA_PC, blob_shadow_fade_near) == 0xc9);
STATIC_ASSERT(offsetof(LEVELDATA_PC, blob_shadow_fade_far) == 0xca);
STATIC_ASSERT(offsetof(LEVELDATA_PC, cam_pos_seek) == 0xcb);
STATIC_ASSERT(offsetof(LEVELDATA_PC, cam_angle_seek) == 0xcc);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_range) == 0xcd);
STATIC_ASSERT(offsetof(LEVELDATA_PC, raycast_step) == 0xce);
STATIC_ASSERT(offsetof(LEVELDATA_PC, platform_scan_distance) == 0xcf);
STATIC_ASSERT(offsetof(LEVELDATA_PC, conveyor_x_speed) == 0xd0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, conveyor_z_speed) == 0xd4);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_gameantinodes) == 0xd8);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_gizmoblowups) == 0xda);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_gizmoblowuptypes) == 0xdc);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_pickups) == 0xde);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_obstacle_objects) == 0xe0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_buildit_objects) == 0xe2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_force_objects) == 0xe4);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_bombgen_objects) == 0xe6);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_tightropes) == 0xe8);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_timers) == 0xe9);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_signals) == 0xea);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_levers) == 0xeb);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_technos) == 0xec);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_zipups) == 0xed);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_grapples) == 0xee);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_obstacles) == 0xef);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_buildits) == 0xf0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_shards) == 0xf1);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_spinners) == 0xf2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_minicuts) == 0xf3);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_minicut_stages) == 0xf4);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_gizspecials) == 0xf5);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_attractos) == 0xf6);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_climb_objects) == 0xf7);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_guidelines) == 0xf8);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_ledges) == 0xf9);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_securitydoors) == 0xfa);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_tubes) == 0xfb);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_gizpanels) == 0xfc);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_hatmachines) == 0xfd);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_force) == 0xfe);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_pushblocks) == 0xff);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_pushblock_endpos) == 0x100);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_doors) == 0x101);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_teleports) == 0x102);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_gizrandoms) == 0x103);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_torpmachines) == 0x104);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_spinneranim_objs) == 0x105);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_turrets) == 0x106);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_bombgens) == 0x107);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_bridges) == 0x108);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_plugs) == 0x109);
STATIC_ASSERT(offsetof(LEVELDATA_PC, wind_speed) == 0x110);
STATIC_ASSERT(offsetof(LEVELDATA_PC, wind_size) == 0x114);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_tracks) == 0x118);

typedef struct level_fixup_pc_s {
    char *name;
    LEVELDATA_PC **level;
    void *load;
    void *init;
    void *reset;
    void *update;
    void *always_update;
    void *draw;
    void *draw_status;
} LEVELFIXUP_PC;

STATIC_ASSERT(sizeof(LEVELFIXUP_PC) == 0x24);
STATIC_ASSERT(offsetof(LEVELFIXUP_PC, name) == 0);
STATIC_ASSERT(offsetof(LEVELFIXUP_PC, level) == 4);
STATIC_ASSERT(offsetof(LEVELFIXUP_PC, load) == 8);
STATIC_ASSERT(offsetof(LEVELFIXUP_PC, draw_status) == 0x20);
#ifdef __cplusplus
extern "C" {
#endif
    extern LEVELDATA_PC *TITLES_LDATA;
    struct cutinfo_s *NewCutScene(struct cutinfo_s *cutscene, struct cutsys_pc_s *system, char *name, i32 unused_4);
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
STATIC_ASSERT(offsetof(LEVELDATA_PC, title_text_id) == 0x60);
STATIC_ASSERT(offsetof(LEVELDATA_PC, episode) == 0xa2);
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
