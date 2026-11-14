#ifndef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "MechInputTouch/MechInputTouch_types.h"
#include "editor/edlevelall.h"
#include "gameapi/ai/aisys/aipath.h"
#include "gamelib/util/gamelib_util_types.h"
#include "globals.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/basething.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "MechInputTouch/MechInputTouch_types.h"
#include "editor/edlevelall.h"
#include "gameapi/ai/aisys/aipath.h"
#include "gamelib/util/gamelib_util_types.h"
#include "globals.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/basething.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "MechInputTouch/MechInputTouch_types.h"
#include "editor/edlevelall.h"
#include "gameapi/ai/aisys/aipath.h"
#include "gamelib/util/gamelib_util_types.h"
#include "globals.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/basething.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 14 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 1 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)

struct BOLTTYPE_PC_s;
struct FLOWBOX_PC;
struct FORCE_RESET_GROUP_PC;
struct FRAME_PAD_PC;
struct GIZFLOW_PC;
struct GIZMOPICKUP_PC;
struct NUGSCN_s;
struct PART_PC;
struct VuMtx;
struct WORLDINFO_s;
struct debris_info_pc_s;
struct debris_key_pc_s;
struct debris_particle_chunk_pc_s;
struct eduiitem_s;
struct eduimenu_s;
struct gameobject_s;
struct mission_data_pc_s;
struct numtl_s;

typedef void (*GIZACTIONFN_PC)(struct GIZFLOW_PC *, struct FLOWBOX_PC *, char **, i32);

typedef struct TERRAIN_TRACK_SLOT_PC {
    void *id;
    i16 platform_index;
    u16 flags;
    i16 platform_contact_state;
    i16 wall_contact_state;
    NUVEC impact_normal;
} TERRAIN_TRACK_SLOT_PC;

STATIC_ASSERT(sizeof(TERRAIN_TRACK_SLOT_PC) == 24);
STATIC_ASSERT(offsetof(TERRAIN_TRACK_SLOT_PC, platform_index) == 4);

typedef struct TERRAIN_SPHERE_PC {
    NUVEC position;
    f32 radius;
} TERRAIN_SPHERE_PC;

STATIC_ASSERT(sizeof(TERRAIN_SPHERE_PC) == 16);
STATIC_ASSERT(offsetof(TERRAIN_SPHERE_PC, radius) == 12);

typedef struct TERRAIN_AXIS_FREEDOM_SHAPE_PC {
    NUVEC offset;
    f32 angle;
    f32 radius;
} TERRAIN_AXIS_FREEDOM_SHAPE_PC;

STATIC_ASSERT(sizeof(TERRAIN_AXIS_FREEDOM_SHAPE_PC) == 20);
typedef struct TERRAIN_WALL_POINT TERRAIN_WALL_POINT;
typedef struct TERRAIN_SPATIAL_NODE TERRAIN_SPATIAL_NODE;
typedef struct TERRAIN_SHAPE_BATCH TERRAIN_SHAPE_BATCH;

typedef struct TERRAIN_INDEX_LEVEL_PC {
    i16 cache_age;
    u8 unknown_02[2];
    f32 center_x;
    f32 center_z;
    TERRAIN_SCAN_WORD_PC scan_words[512];
} TERRAIN_INDEX_LEVEL_PC;

typedef struct TERRAIN_GROUP_PC {
    NUVEC origin;
    TERRAIN_SKIN_STREAM_PC *data;
    NUVEC bounds_min;
    NUVEC bounds_max;
    u32 platform_flags;
    i32 chunk_type;
    i16 scene_index;
    i16 group_info;
    f32 radius;
} TERRAIN_GROUP_PC;

typedef struct TERRAIN_CELL_PC {
    i16 first_group;
    i16 group_count;
    f32 min_x;
    f32 min_z;
    f32 max_x;
    f32 max_z;
} TERRAIN_CELL_PC;

STATIC_ASSERT(sizeof(TERRAIN_INDEX_LEVEL_PC) == 0x80c);
STATIC_ASSERT(offsetof(TERRAIN_INDEX_LEVEL_PC, cache_age) == 0);
STATIC_ASSERT(offsetof(TERRAIN_INDEX_LEVEL_PC, center_x) == 4);
STATIC_ASSERT(offsetof(TERRAIN_INDEX_LEVEL_PC, center_z) == 8);
STATIC_ASSERT(offsetof(TERRAIN_INDEX_LEVEL_PC, scan_words) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_CELL_PC, min_x) == 4);
STATIC_ASSERT(offsetof(TERRAIN_CELL_PC, min_z) == 8);
STATIC_ASSERT(offsetof(TERRAIN_CELL_PC, max_x) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_CELL_PC, max_z) == 0x10);
STATIC_ASSERT(sizeof(TERRAIN_GROUP_PC) == 0x38);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, data) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, bounds_min) == 0x10);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, bounds_max) == 0x1c);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, chunk_type) == 0x2c);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, scene_index) == 0x30);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, radius) == 0x34);
STATIC_ASSERT(sizeof(TERRAIN_TRACK_SLOT_PC) == 0x18);
STATIC_ASSERT(offsetof(TERRAIN_TRACK_SLOT_PC, platform_index) == 0x4);
STATIC_ASSERT(sizeof(TERRAIN_CELL_PC) == 0x14);
typedef TERRAIN_GROUP_PC TERRAIN_GROUP;
typedef TERRAIN_CELL_PC TERRAIN_CELL;
typedef TERRAIN_POLYGON_INFO_PC TERRAIN_SHAPE;

typedef struct TerrainQuery_s {
    u32 flags;
    NUVEC start_position;
    NUVEC start_movement;
    NUVEC position;
    NUVEC movement;
    u8 scan_result;
    u8 shape_adjusted;
    i16 object_index;
    f32 radius;
    f32 horizontal_movement_length;
    u8 *hit_flags;
    f32 movement_pitch;
    f32 movement_yaw;
    f32 movement_length;
    f32 collision_radius;
    f32 collision_radius_sq;
    f32 inverse_collision_radius;
    f32 object_scale;
    f32 object_scale_sq;
    f32 inverse_object_scale;
    f32 inverse_object_scale_sq;
    i16 terrain_group_index;
    u8 pad_0x06e[2];
    NUVEC local_start;
    NUVEC local_end;
    u8 hit_type;
    u8 hit_edge;
    i16 scan_group_index;
    void *scan_list;
    TERRAIN_SHAPE *surface;
    TERRAIN_SHAPE *working_surface;
    f32 hit_time;
    f32 unclamped_hit_time;
    f32 separation_epsilon;
    f32 compare_epsilon;
    NUVEC movement_normal;
    NUVEC impact_normal;
    NUVEC platform_position;
    NUVEC platform_normal;
    NUVEC previous_movement_normal;
    u8 pad_0x0e4[0x18];
    NUVEC transformed_vertices[4];
    u8 pad_0x12c[0x1c];

    union {
        u8 scan_list_storage[0x800];
        TERRAIN_SCAN_WORD_PC scan_words[512];
    };
} TerrainQuery_s;

STATIC_ASSERT(sizeof(TerrainQuery_s) == 0x948);
STATIC_ASSERT(offsetof(TerrainQuery_s, terrain_group_index) == 0x6c);
STATIC_ASSERT(offsetof(TerrainQuery_s, surface) == 0x90);
STATIC_ASSERT(offsetof(TerrainQuery_s, scan_words) == 0x148);
STATIC_ASSERT(offsetof(TerrainQuery_s, position) == 0x1c);
STATIC_ASSERT(offsetof(TerrainQuery_s, movement) == 0x28);
STATIC_ASSERT(offsetof(TerrainQuery_s, scan_result) == 0x34);
STATIC_ASSERT(offsetof(TerrainQuery_s, collision_radius) == 0x50);
STATIC_ASSERT(offsetof(TerrainQuery_s, object_scale) == 0x5c);
STATIC_ASSERT(offsetof(TerrainQuery_s, object_scale_sq) == 0x60);
STATIC_ASSERT(offsetof(TerrainQuery_s, inverse_object_scale) == 0x64);
STATIC_ASSERT(offsetof(TerrainQuery_s, scan_group_index) == 0x8a);
STATIC_ASSERT(offsetof(TerrainQuery_s, scan_list) == 0x8c);

struct TERRAIN_SHAPE_BATCH {
    i16 marker;
    i16 shape_count;
    f32 min_x;
    f32 max_x;
    f32 min_z;
    f32 max_z;
};

struct TERRAIN_WALL_POINT {
    NUVEC position;
    u8 material[4];
};

STATIC_ASSERT(sizeof(TERRAIN_SHAPE_BATCH) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_TRACK_SLOT_PC, wall_contact_state) == 0xa);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, platform_flags) == 0x28);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, group_info) == 0x32);

enum {
    TERRAIN_CHUNK_GROUP_PRIMARY = 0,
    TERRAIN_CHUNK_GROUP_SECONDARY = 1,
    TERRAIN_CHUNK_SPATIAL_INDEX = 2,
    TERRAIN_CHUNK_CRASH_DATA = 3,
    TERRAIN_INDEX_LEVEL_COUNT = 17,
    TERRAIN_CELL_RECORD_COUNT = 257,
    TERRAIN_TRACK_SLOT_COUNT = 64,
    TERRAIN_GRID_WIDTH = 7,
    TERRAIN_GRID_CELL_COUNT = 49,
    TERRAIN_PLATFORM_CELL = 256,
    TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED = 4,
    TERRAIN_TRACK_CONTACT_NONE = 0,
    TERRAIN_SHAPE_FLAG_MATERIAL_ALIAS = 0x80,
    TERRAIN_SHAPE_FLAG_MATERIAL_HIGH_BIT = 0x40,
    TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP = 1,
    TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP = 2
};

typedef struct gizaimessage_s {
    NULISTLNK links;     // 0x00
    char name[32];       // 0x08
    f32 value;           // 0x28
    i8 output_values[8]; // 0x2c
    i8 output_count;     // 0x34
    u8 unknown_35;       // 0x35
    u16 flags;           // 0x36
} GIZAIMESSAGE;          // size 0x38

typedef struct gizaimessagesys_s {
    i32 count;              // 0x00
    GIZAIMESSAGE *messages; // 0x04
    NULISTHDR free_list;    // 0x08
    NULISTHDR active_list;  // 0x10
} GIZAIMESSAGESYS;          // size 0x18

typedef struct part_emit_pc_s {
    NUVEC position;
    i32 effect_id;
    char name[16];
    f32 time_20;
    f32 time_24;
    f32 time_28;
    i16 rotation_2c;
    i16 rotation_2e;
    i16 rotation_30;
    i8 page;
    u8 field_33;
    i32 shots_remaining;
    f32 time_38;
    u8 field_3c;
    u8 field_3d;
    u8 field_3e;
    u8 field_3f;
    f32 camera_distance;
    i16 field_44;
    i16 field_46;
    i32 instance_id;
    i32 trailing_state_words[8];
} PART_EMIT_PC;

STATIC_ASSERT(sizeof(PART_EMIT_PC) == 0x6c);
STATIC_ASSERT(offsetof(PART_EMIT_PC, position) == 0);
STATIC_ASSERT(offsetof(PART_EMIT_PC, effect_id) == 0xc);
STATIC_ASSERT(offsetof(PART_EMIT_PC, name) == 0x10);
STATIC_ASSERT(offsetof(PART_EMIT_PC, time_20) == 0x20);
STATIC_ASSERT(offsetof(PART_EMIT_PC, time_24) == 0x24);
STATIC_ASSERT(offsetof(PART_EMIT_PC, time_28) == 0x28);
STATIC_ASSERT(offsetof(PART_EMIT_PC, rotation_2c) == 0x2c);
STATIC_ASSERT(offsetof(PART_EMIT_PC, rotation_2e) == 0x2e);
STATIC_ASSERT(offsetof(PART_EMIT_PC, rotation_30) == 0x30);
STATIC_ASSERT(offsetof(PART_EMIT_PC, page) == 0x32);
STATIC_ASSERT(offsetof(PART_EMIT_PC, shots_remaining) == 0x34);
STATIC_ASSERT(offsetof(PART_EMIT_PC, time_38) == 0x38);
STATIC_ASSERT(offsetof(PART_EMIT_PC, field_33) == 0x33);
STATIC_ASSERT(offsetof(PART_EMIT_PC, field_3d) == 0x3d);
STATIC_ASSERT(offsetof(PART_EMIT_PC, camera_distance) == 0x40);
STATIC_ASSERT(offsetof(PART_EMIT_PC, field_44) == 0x44);
STATIC_ASSERT(offsetof(PART_EMIT_PC, field_46) == 0x46);

typedef struct terrain_surface_pc_s {
    f32 movement_scale;
    u32 flags;

    union {
        u32 field_08;

        struct {
            u16 object_id;
            u16 unknown_object_high_word;
        };
    };
} TERRAIN_SURFACE_PC;

STATIC_ASSERT(sizeof(TERRAIN_SURFACE_PC) == 0x0c);
STATIC_ASSERT(offsetof(TERRAIN_SURFACE_PC, flags) == 4);
STATIC_ASSERT(offsetof(TERRAIN_SURFACE_PC, field_08) == 8);

struct bolt_s;
struct bolt_s;

typedef struct narrowsockexception_pc_s {
    const char *level_name;
    i16 level_index;
    i16 sock_index;
    f32 start_distance;
    f32 end_distance;
} NARROWSOCKEXCEPTION_PC;

STATIC_ASSERT(sizeof(NARROWSOCKEXCEPTION_PC) == 0x10);
STATIC_ASSERT(offsetof(NARROWSOCKEXCEPTION_PC, level_index) == 4);
STATIC_ASSERT(offsetof(NARROWSOCKEXCEPTION_PC, sock_index) == 6);
STATIC_ASSERT(offsetof(NARROWSOCKEXCEPTION_PC, start_distance) == 8);
STATIC_ASSERT(offsetof(NARROWSOCKEXCEPTION_PC, end_distance) == 0xc);
typedef struct cutsys_pc_s CUTSYS_PC;
typedef struct gameanimsys_s GAMEANIMSYS;

typedef struct debris_float_key_pc_s {
    f32 time;
    f32 value;
} DEBRIS_FLOAT_KEY_PC;

typedef struct debris_colour_key_pc_s {
    f32 time;
    u8 red;
    u8 green;
    u8 blue;
    u8 unknown_7;
} DEBRIS_COLOUR_KEY_PC;

typedef struct debris_particle_frame_pc_s {
    NUVEC position;
    NUVEC texture_offset;
    NUVEC extent;
    u32 colour;
} DEBRIS_PARTICLE_FRAME_PC;

STATIC_ASSERT(sizeof(DEBRIS_FLOAT_KEY_PC) == 8);
STATIC_ASSERT(offsetof(DEBRIS_FLOAT_KEY_PC, value) == 4);
STATIC_ASSERT(sizeof(DEBRIS_COLOUR_KEY_PC) == 8);
STATIC_ASSERT(offsetof(DEBRIS_COLOUR_KEY_PC, red) == 4);
STATIC_ASSERT(offsetof(DEBRIS_COLOUR_KEY_PC, green) == 5);
STATIC_ASSERT(offsetof(DEBRIS_COLOUR_KEY_PC, blue) == 6);
STATIC_ASSERT(sizeof(DEBRIS_PARTICLE_FRAME_PC) == 0x28);
STATIC_ASSERT(offsetof(DEBRIS_PARTICLE_FRAME_PC, position) == 0);
STATIC_ASSERT(offsetof(DEBRIS_PARTICLE_FRAME_PC, texture_offset) == 0xc);
STATIC_ASSERT(offsetof(DEBRIS_PARTICLE_FRAME_PC, extent) == 0x18);
STATIC_ASSERT(offsetof(DEBRIS_PARTICLE_FRAME_PC, colour) == 0x24);

STATIC_ASSERT(sizeof(DEBRIS_FLOAT_KEY_PC) == 8);

typedef struct SoundTable {
    u16 bits[100];
} SoundTable;

STATIC_ASSERT(sizeof(SoundTable) == 0xc8);

typedef struct GAMEAUDIO_PC {
    i32 (*override_footstep_fn)(struct gameobject_s *, i32);
    i32 (*check_reverb_fn)(void);
    char *sfx_names[84];
    i16 sfx_ids[84];
} GAMEAUDIO_PC;

STATIC_ASSERT(sizeof(GAMEAUDIO_PC) == 0x200);
STATIC_ASSERT(offsetof(GAMEAUDIO_PC, override_footstep_fn) == 0);
STATIC_ASSERT(offsetof(GAMEAUDIO_PC, sfx_names) == 8);
STATIC_ASSERT(offsetof(GAMEAUDIO_PC, check_reverb_fn) == 4);
STATIC_ASSERT(offsetof(GAMEAUDIO_PC, sfx_ids) == 0x158);

typedef struct cutinfo_s {
    u8 opaque_00_04[0x4];
    void *instance;
    char name[0x40];
    u8 unknown_48[0x8];
    u32 flags;
    u8 unknown_54[0x98];
    i16 skip_level;
} CUTINFO;

STATIC_ASSERT(offsetof(CUTINFO, flags) == 0x50);
STATIC_ASSERT(offsetof(CUTINFO, instance) == 4);
STATIC_ASSERT(offsetof(CUTINFO, name) == 8);
typedef struct gameanimset_s GAMEANIMSET;

typedef struct language_data_s {
    i32 language;
    char *localized_name;
} LANGUAGEDATA;

STATIC_ASSERT(sizeof(LANGUAGEDATA) == 8);

typedef struct text_entry_s {
    i16 *text_id;
    i16 value;
    i16 pad;
} TEXTENTRY;

STATIC_ASSERT(sizeof(TEXTENTRY) == 8);
STATIC_ASSERT(offsetof(TEXTENTRY, value) == 4);

typedef struct FadeBase FadeBase;
#ifdef __cplusplus
enum FADETYPE_VALUE {
    FADE_TYPE_NONE = -1,
    FADE_TYPE_SCREEN = 0,
    FADE_TYPE_WIPE = 1,
    FADE_TYPE_STILL_WIPE = 2,
    FADE_TYPE_STILL = 3,
};

struct FADEINFO_s;

struct FADETYPE {
    FADETYPE_VALUE type;
};
#endif

typedef struct OCCLUDERSET_s {
    u8 unknown_0000[0xc];
    u32 count;
    u8 queries_prepared;
    u8 unknown_0011[0x8f];
} OCCLUDERSET;

STATIC_ASSERT(sizeof(OCCLUDERSET) == 0xa0);
STATIC_ASSERT(offsetof(OCCLUDERSET, count) == 0xc);
STATIC_ASSERT(offsetof(OCCLUDERSET, queries_prepared) == 0x10);

typedef struct OCCLUSIONMANAGER_s {
    u8 initialized;
    u8 enabled;
    u8 unknown_0002[0xe];
    OCCLUDERSET sets[2];
    OCCLUDERSET *building_set;
    OCCLUDERSET *current_set;
    f32 unknown_0158;
    f32 unknown_015c;
    u32 unknown_0160;
    u32 unknown_0164;
    u8 unknown_0168[8];
} OCCLUSIONMANAGER;

STATIC_ASSERT(sizeof(OCCLUSIONMANAGER) == 0x170);
STATIC_ASSERT(offsetof(OCCLUSIONMANAGER, building_set) == 0x150);
STATIC_ASSERT(offsetof(OCCLUSIONMANAGER, current_set) == 0x154);
STATIC_ASSERT(offsetof(OCCLUSIONMANAGER, unknown_0160) == 0x160);
STATIC_ASSERT(offsetof(OCCLUSIONMANAGER, unknown_0164) == 0x164);

typedef struct gizforce_pc_s GIZFORCE_PC;
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct BATARANG_TARGET_s {
        void *object;
        u8 type;
        u8 lost;
        u16 reserved_06_word;

        union {
            u8 reserved_08[12];
            NUVEC fallback_position;
        };
    } BATARANG_TARGET_s;

    STATIC_ASSERT(sizeof(BATARANG_TARGET_s) == 0x14);
    STATIC_ASSERT(offsetof(BATARANG_TARGET_s, fallback_position) == 8);
    struct CHARPIVOT_PC;

    typedef struct debris_process_sphere_pc_s {
        NUVEC position;
        NUVEC momentum;
        f32 time;
    } DEBRIS_PROCESS_SPHERE_PC;

    STATIC_ASSERT(sizeof(DEBRIS_PROCESS_SPHERE_PC) == 0x1c);
    STATIC_ASSERT(offsetof(DEBRIS_PROCESS_SPHERE_PC, time) == 0x18);

    typedef struct edpp_particle_pc_s {
        NUVEC position;
        i32 effect_index;
        i32 instance_id;
        i16 rotation_z;
        i16 rotation_y;
        i16 emitter_rotation_z;
        i16 emitter_rotation_y;
        i16 emitter_rotation_x;
        u8 unknown_01e[2];
        f32 start_offset;
        char name[16];
        i32 switch_type;
        i32 switch_id;
        f32 switch_variable;
        i16 reflection_rotation_z;
        i16 reflection_rotation_y;
        f32 reflection_offset;
        f32 reflection_bounce;
        i16 render_group;
        u16 render_priority;
        i8 dynamic_priority;
        i8 page;
        i8 detail_levels;
        i8 facing_mode;
        i16 facing_rotation_x;
        i16 facing_rotation_y;
    } EDPP_PARTICLE_PC;

    STATIC_ASSERT(sizeof(EDPP_PARTICLE_PC) == 0x58);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, position) == 0);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, effect_index) == 0xc);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, instance_id) == 0x10);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, rotation_z) == 0x14);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, rotation_y) == 0x16);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, emitter_rotation_z) == 0x18);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, emitter_rotation_y) == 0x1a);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, emitter_rotation_x) == 0x1c);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, start_offset) == 0x20);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, name) == 0x24);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, switch_type) == 0x34);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, switch_id) == 0x38);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, switch_variable) == 0x3c);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, reflection_rotation_z) == 0x40);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, reflection_rotation_y) == 0x42);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, reflection_offset) == 0x44);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, reflection_bounce) == 0x48);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, render_group) == 0x4c);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, page) == 0x51);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, detail_levels) == 0x52);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, facing_mode) == 0x53);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, facing_rotation_x) == 0x54);
    STATIC_ASSERT(offsetof(EDPP_PARTICLE_PC, facing_rotation_y) == 0x56);
    struct GIZTURRET_s;

    typedef struct ripple_set_pc_s RIPPLE_SET_PC;
#ifdef __cplusplus
}
#endif

struct CHARPIVOT_PC {
    const char *name;
    i16 animation;
    i16 point_of_interest;
    f32 start_frame;
    f32 end_frame;
    u16 character_id;
    u8 unknown_12[2];
};

STATIC_ASSERT(sizeof(struct CHARPIVOT_PC) == 20);
STATIC_ASSERT(offsetof(struct CHARPIVOT_PC, name) == 0);
STATIC_ASSERT(offsetof(struct CHARPIVOT_PC, animation) == 4);
STATIC_ASSERT(offsetof(struct CHARPIVOT_PC, point_of_interest) == 6);
STATIC_ASSERT(offsetof(struct CHARPIVOT_PC, start_frame) == 8);
STATIC_ASSERT(offsetof(struct CHARPIVOT_PC, end_frame) == 0xc);
STATIC_ASSERT(offsetof(struct CHARPIVOT_PC, character_id) == 0x10);

typedef struct CHARPIVOT_PC CHARPIVOT_PC;
STATIC_ASSERT(sizeof(TERRAIN_SURFACE_PC) == 0x0c);
STATIC_ASSERT(offsetof(TERRAIN_SURFACE_PC, flags) == 4);
STATIC_ASSERT(offsetof(TERRAIN_SURFACE_PC, field_08) == 8);
STATIC_ASSERT(offsetof(TERRAIN_SURFACE_PC, object_id) == 8);
typedef struct AIPATHCNXCONTROLLER_s AIPATHCNXCONTROLLER_s;
typedef struct AITRIGGERSETSYS_s AITRIGGERSETSYS_s;
typedef struct AITRIGGERSET_TARGET AITRIGGERSET_TARGET;
typedef struct AITRIGGERSET_s AITRIGGERSET_s;
typedef struct AREASAVE_s AREASAVE_s;
typedef struct CABLE_s CABLE_s;
typedef struct CHARPLATFORMSYS_s CHARPLATFORMSYS_s;
typedef struct CHARPLATFORM_s CHARPLATFORM_s;
typedef struct DETONATOR_s DETONATOR_s;
typedef struct EXPLOSION EXPLOSION;
typedef struct GAMEANTINODE_s GAMEANTINODE_s;
typedef struct GIZBUILDITANIMDATA_s GIZBUILDITANIMDATA_s;
typedef struct GIZBUILDIT_s GIZBUILDIT_s;
typedef struct GIZFORCEANIMDATA_s GIZFORCEANIMDATA_s;
typedef struct GIZFORCEGROUP_s GIZFORCEGROUP_s;
typedef struct GIZFORCE_s GIZFORCE_s;
typedef struct GIZOBSTACLESYS_s GIZOBSTACLESYS_s;
typedef struct GIZPANEL_s GIZPANEL_s;
typedef struct GIZSPECIAL_s GIZSPECIAL_s;
typedef struct GIZSPINNERARM_s GIZSPINNERARM_s;
typedef struct GIZTURRET_s GIZTURRET_s;
typedef struct HOTHBATTLE_MELEE_WAVE_s HOTHBATTLE_MELEE_WAVE_s;
typedef struct HOTHBATTLE_MELEE_s HOTHBATTLE_MELEE_s;
typedef struct LevelEditorScene LevelEditorScene;
typedef struct PODSPRINTNETPACKET_s PODSPRINTNETPACKET_s;
typedef struct SNIPER_s SNIPER_s;
typedef struct SPECIALMINIKIT_s SPECIALMINIKIT_s;
typedef struct STATUSPACKET_s STATUSPACKET_s;
typedef struct STATUS_STAGE_s STATUS_STAGE_s;
typedef struct TRAFFICANIMINSTANCE_s TRAFFICANIMINSTANCE_s;
typedef struct TRAFFICANIM_s TRAFFICANIM_s;
typedef struct edcam_s edcam_s;
typedef struct minisnowtrooper_s minisnowtrooper_s;
typedef struct minitrooperteam_s minitrooperteam_s;
typedef struct nugraph_s nugraph_s;
typedef struct shopitem_s shopitem_s;

struct AREASAVE_s {
    u8 complete;
    u8 area_complete;
    u8 true_hero_complete[2];
    u8 minikit_complete;

    union {
        u8 field_0x5[3];

        struct {
            u8 minikit_count;
            u8 red_brick_collected;
            u8 reserved_0x7;
        };
    };

    f32 challenge_trial_time;
};

STATIC_ASSERT(sizeof(struct AREASAVE_s) == 12);

struct STATUSPACKET_LSW_s {
    i32 field_0x00;
};

STATIC_ASSERT(sizeof(struct STATUSPACKET_LSW_s) == 4);

struct GIZFORCEANIMDATA_s {
    u16 flags;
    i16 platform_id;

    union {
        u32 field_0x04;

        struct {
            u32 force_glow_active : 1;
            u32 : 31;
        };
    };
};

STATIC_ASSERT(sizeof(struct GIZFORCEANIMDATA_s) == 8);

struct TRAFFICANIMINSTANCE_s {
    NULISTLNK link;
    struct TRAFFICANIM_s *animation;
    f32 frame;
    i8 vehicle_index;
    u8 reserved_11[3];
};

STATIC_ASSERT(sizeof(struct TRAFFICANIMINSTANCE_s) == 20);

struct SNIPER_s {
    NUVEC prev;
    NUVEC pos;
    f32 fire_timer;
    f32 state;
};

STATIC_ASSERT(sizeof(struct SNIPER_s) == 32);

struct PODSPRINTNETPACKET_s {
    i16 ai_state;
    char pad_0x02[4];
    i16 speed;
    i16 speed2;
};

STATIC_ASSERT(sizeof(struct PODSPRINTNETPACKET_s) == 10);

struct minisnowtrooper_s {
    u8 formation_index;

    union {
        u8 state_flags;

        struct {
            u8 state_low : 2;
            u8 step_frame : 2;
            u8 slot : 2;
            u8 state_high : 2;
        };
    };

    u8 reserved_002;
    u8 speed_divisor;
    u16 rotation;
    u16 target_rotation;
    f32 timer;
    f32 formation_x;
    f32 formation_z;
    NUVEC shot_position;
};

STATIC_ASSERT(sizeof(struct minisnowtrooper_s) == 32);
STATIC_ASSERT(offsetof(struct minisnowtrooper_s, state_flags) == 1);
STATIC_ASSERT(offsetof(struct minisnowtrooper_s, rotation) == 4);
STATIC_ASSERT(offsetof(struct minisnowtrooper_s, target_rotation) == 6);
STATIC_ASSERT(offsetof(struct minisnowtrooper_s, timer) == 8);
STATIC_ASSERT(offsetof(struct minisnowtrooper_s, formation_x) == 0xc);
STATIC_ASSERT(offsetof(struct minisnowtrooper_s, formation_z) == 0x10);
STATIC_ASSERT(offsetof(struct minisnowtrooper_s, shot_position) == 0x14);

struct edcam_s {
    NUVEC position;
    i32 pitch;
    i32 yaw;
    f32 distance;
    NUVEC offset;
    NUVEC snap_origin;
    NUVEC snap_step;
    i32 pitch_snap_step;
    i32 yaw_snap_step;
    NUVEC snapped_position;
    i32 snapped_pitch;
    i32 snapped_yaw;
    NUVEC position_speed;
    i32 pad_pitch_speed;
    i32 pad_yaw_speed;
    f32 distance_speed;
    f32 minimum_distance;
    f32 mouse_pitch_speed;
    f32 mouse_yaw_speed;
    f32 mouse_move_speed;
    f32 auto_move_base;
    f32 auto_move_dist_scale;
    f32 auto_zoom_base;
    f32 auto_zoom_dist_scale;

    union {
        u32 freedoms;

        struct {
            u32 allow_position_x : 1;
            u32 allow_position_y : 1;
            u32 allow_position_z : 1;
            u32 allow_pitch : 1;
            u32 allow_yaw : 1;
            u32 allow_distance : 1;
            u32 invert_pad_pitch : 1;
            u32 reserved_freedom : 1;
        };
    };
};

STATIC_ASSERT(sizeof(struct edcam_s) == 148);

enum { EDCAM_FREEDOM_INVERT_PAD_PITCH = 64 };

enum { GIZSPINNER_FLAG_VALID = 1 };

enum { EDCAM_FREEDOM_DISTANCE = 32 };

enum { EDCAM_FREEDOM_PITCH = 8 };

enum { EDCAM_FREEDOM_POSITION_X = 1 };

enum { EDCAM_FREEDOM_POSITION_Y = 2 };

enum { EDCAM_FREEDOM_POSITION_Z = 4 };

enum { EDCAM_FREEDOM_YAW = 16 };

enum { GAMEANIMSET_STATE_AT_END = 2 };

enum { GAMEANIMSET_VISIBILITY_ALL = 2 };

enum { GIZBUILDIT_BUILD_IDLE = 0 };

enum { GIZBUILDIT_STATE_ROTATING_WOBBLE = 1 };
#ifdef __cplusplus

struct ThingLevelData;
struct ThingRemoveData;
#endif
#ifdef __cplusplus

/* Buffered append-only log file embedded in TTNetwork at +0x20. */
class NetLogFile {
  public:
    NetLogFile();
    ~NetLogFile();
    i32 Flush();

    i32 state;
    char name[0x100];
    i32 mode;
    u32 unknown_108[2];
    i32 count;
    u8 buffer[0x2000];
};
#endif
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 2 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)

typedef struct TERRAIN_PLATFORM_PC {
    NUMTX previous_matrix;
    void *scene_object;
    u32 unknown_44;
    i16 terrain_group_index;
    i16 scene_object_index;

    union {
        u32 flags;

        struct {
            u32 rotating : 1;
            u32 : 31;
        };
    };

    u8 unknown_50[4];
    f32 bounce_54;
    f32 bounce_58;
    f32 bounce_5c;
    f32 bounce_60;
    f32 bounce_64;
    u32 *instance_flags;
} TERRAIN_PLATFORM_PC;

STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, previous_matrix) == 0);
STATIC_ASSERT(sizeof(TERRAIN_PLATFORM_PC) == 0x6c);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, scene_object) == 0x40);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, terrain_group_index) == 0x48);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, scene_object_index) == 0x4a);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, flags) == 0x4c);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, bounce_54) == 0x54);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, instance_flags) == 0x68);
typedef TERRAIN_PLATFORM_PC TERRAIN_PLATFORM;

typedef struct gamecamera_s {
    union {
        SOCKPOSITION_PC sock_position;

        struct {
            u8 unknown_00;
            u8 sock;
        };
    };

    NUMTX mtx;
    NUMTX target_mtx;
    NUMTX render_mtx;
    NUVEC shaken_right;
    NUVEC shaken_up;
    NUVEC dir;
    NUVEC position;
    NUVEC target;
    NUVEC blend_start_position;
    NUVEC desired_position;
    NUVEC blend_end_position;
    NUVEC blend_destination_position;
    NUVEC blend_start_target;
    u8 unknown_170[0xc];
    NUVEC blend_end_target;
    NUVEC blend_destination_target;
    NUVEC shake_offset;
    NUVEC shake_direction;
    NUVEC shake_target;
    i32 field_1b8;
    f32 blend_time;
    f32 blend_duration;
    f32 blend_curve;
    f32 judder_time;
    f32 judder_duration;
    f32 shake_amplitude;
    f32 shake_intensity;
    f32 shake_decay;
    f32 shake_freq;
    f32 position_seek;
    f32 angle_seek;
    f32 field_1e8;
    f32 field_1ec;
    f32 field_1f0;
    f32 field_1f4;
    i32 pitch;

    union {
        i32 yaw;
        i32 input_yaw;
    };

    i32 roll;
    f32 field_204;
    f32 field_208;
    f32 field_20c;
    f32 field_210;
    f32 field_214;
    f32 adjusted_yaw_offset;
    u16 blend_start_pitch;
    u16 desired_pitch;
    u16 blend_start_yaw;
    u16 desired_yaw;
    u16 blend_start_roll;
    u16 desired_roll;
    u8 judder_reverse;
    u8 judder_axis;
    u8 reset_blend;
    u8 blend_mode;
    i8 mode;
    i8 previous_mode;
    u8 previous_camera_mode;
} GAMECAMERA;

STATIC_ASSERT(offsetof(GAMECAMERA, input_yaw) == 0x1fc);
STATIC_ASSERT(offsetof(GAMECAMERA, yaw) == 0x1fc);
STATIC_ASSERT(offsetof(GAMECAMERA, sock) == 0x1);
STATIC_ASSERT(offsetof(GAMECAMERA, position) == 0x11c);
STATIC_ASSERT(offsetof(GAMECAMERA, sock_position.sock_index) == 1);
STATIC_ASSERT(offsetof(GAMECAMERA, shake_offset) == 0x194);
STATIC_ASSERT(offsetof(GAMECAMERA, shake_intensity) == 0x1d4);
STATIC_ASSERT(offsetof(GAMECAMERA, shake_decay) == 0x1d8);
STATIC_ASSERT(offsetof(GAMECAMERA, shake_freq) == 0x1dc);
STATIC_ASSERT(offsetof(GAMECAMERA, mtx) == 0x38);
STATIC_ASSERT(offsetof(GAMECAMERA, blend_end_target) == 0x17c);
STATIC_ASSERT(offsetof(GAMECAMERA, blend_time) == 0x1bc);
STATIC_ASSERT(sizeof(GAMECAMERA) == 0x230);
STATIC_ASSERT(offsetof(GAMECAMERA, adjusted_yaw_offset) == 0x218);
STATIC_ASSERT(offsetof(GAMECAMERA, blend_start_pitch) == 0x21c);
STATIC_ASSERT(offsetof(GAMECAMERA, desired_pitch) == 0x21e);
STATIC_ASSERT(offsetof(GAMECAMERA, blend_start_yaw) == 0x220);
STATIC_ASSERT(offsetof(GAMECAMERA, desired_yaw) == 0x222);
STATIC_ASSERT(offsetof(GAMECAMERA, blend_start_roll) == 0x224);
STATIC_ASSERT(offsetof(GAMECAMERA, desired_roll) == 0x226);
STATIC_ASSERT(offsetof(GAMECAMERA, reset_blend) == 0x22a);
STATIC_ASSERT(offsetof(GAMECAMERA, blend_mode) == 0x22b);
STATIC_ASSERT(offsetof(GAMECAMERA, mode) == 0x22c);
STATIC_ASSERT(offsetof(GAMECAMERA, previous_mode) == 0x22d);
STATIC_ASSERT(offsetof(GAMECAMERA, previous_camera_mode) == 0x22e);
#ifdef __cplusplus
extern "C" {
#endif
    STATIC_ASSERT(offsetof(GAMECAMERA, blend_time) == 0x1bc);
    STATIC_ASSERT(offsetof(GAMECAMERA, blend_duration) == 0x1c0);
#ifdef __cplusplus
}
#endif

struct AITRIGGERSET_TARGET {
    u8 reserved_00[16];
    NUVEC position;
    u8 reserved_1c[4];
    AIPATHINFO_PC path;
    u8 reserved_38[4];
};

STATIC_ASSERT(sizeof(struct AITRIGGERSET_TARGET) == 60);

struct GIZSPINNERARM_s {
    NUMTX matrix;
    i16 platform_id;
    u8 field_0x42[2];
};

STATIC_ASSERT(sizeof(struct GIZSPINNERARM_s) == 68);

struct GIZPANEL_s {
    NUMTX matrix;
    char name[16];
    NUVEC position;
    u16 y_rotation;
    u8 field_0x5e[3];
    u8 model_variant;
    u8 field_0x62[2];
    f32 flash_timer;

    union {
        u8 flags;

        struct {
            u8 reserved_flag : 1;
            u8 state : 1;
            u8 visible : 1;
            u8 track_player : 1;
            u8 player_near : 1;
            u8 hide_base : 1;
            u8 hide_target : 1;
            u8 baddie : 1;
        };
    };

    u8 draw_flags;
    u8 field_0x6a[2];
    NUVEC floor_position;
    NUVEC target_offset;
    u16 target_x_rotation;
    u16 target_y_rotation;
    u16 arm_x_rotation;
    u16 target_pitch;
    u16 target_roll;
    i16 platform_id;
    f32 activation_time;
    f32 target_scale;
    struct MechObjectInterface *mech_object_interface;
};

STATIC_ASSERT(sizeof(struct GIZPANEL_s) == 156);

struct GIZBUILDITANIMDATA_s {
    NUMTX start_mtx;
    NUMTX end_mtx;
    NUMTX draw_mtx;
    f32 wobble_time;
    u8 wobble_axis;
    u8 padding_0xc5;
    u8 was_drawn;
    u8 padding_0xc7;
};

STATIC_ASSERT(sizeof(struct GIZBUILDITANIMDATA_s) == 200);
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 3 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)

typedef TERRAIN_SET_PC TERRSET;

struct TERRAIN_SPATIAL_NODE {
    u16 point_count;
    u16 field_02;
    NUVEC points[1];
};
#ifdef __cplusplus
extern "C" {
#endif
    struct TERRAIN_SPATIAL_NODE;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 4 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)

typedef struct levelspline_pc_s {
    struct NUGSCN_s **scene;
    const char *name;
    u16 min_points;
    u16 max_points;
    i16 level;
    i16 area;
} LEVELSPLINE_PC;

STATIC_ASSERT(sizeof(LEVELSPLINE_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, name) == 4);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, min_points) == 8);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, area) == 0xe);

typedef struct nuinstanim_s {
    NUMTX matrix;
    f32 tfactor;
    f32 first_frame;
    f32 interval;
    f32 ltime;
    u32 flags;
    f32 fparam1;
    f32 prev_eval_time;
    u16 animation_index;
    u16 instance_index;
} NUINSTANIM;

STATIC_ASSERT(sizeof(NUINSTANIM) == 0x60);
STATIC_ASSERT(offsetof(NUINSTANIM, matrix) == 0);
STATIC_ASSERT(offsetof(NUINSTANIM, tfactor) == 0x40);
STATIC_ASSERT(offsetof(NUINSTANIM, ltime) == 0x4c);
STATIC_ASSERT(offsetof(NUINSTANIM, flags) == 0x50);
STATIC_ASSERT(offsetof(NUINSTANIM, fparam1) == 0x54);
STATIC_ASSERT(offsetof(NUINSTANIM, prev_eval_time) == 0x58);
STATIC_ASSERT(offsetof(NUINSTANIM, animation_index) == 0x5c);
#ifdef __cplusplus

struct OccluderSet {
    OccluderSet();
    ~OccluderSet();
    static struct numtl_s *ms_pZOnlyMtl3D;
    static struct numtl_s *ms_pZOnlyMtl2D;
    static struct numtl_s *ms_pAlphaMtl2D;
    void Init(u32, VARIPTR *, VARIPTR);
    PC_OCCLUDER_RECORD *occluders;
    u32 *indices;
    u32 capacity;
    u32 count;
    bool queries_prepared;
    u8 unknown_11[15];
    NUMTX projection_matrix;
    NUMTX query_matrix;
    bool IsOccludedOBB(const NUVEC *, const NUVEC *, const NUMTX *);
    void AddOccluder(const NUVEC *, const NUVEC *, const NUVEC *, const NUVEC *);
    void RenderOccluders(bool) const;
    void OnCameraSet();
    bool IsOccludedSphere(const NUVEC *, f32);
    void PrepareForQueries(const NUMTX *, const NUMTX *);
    static i32 SortByDepth(const void *, const void *);
    u32 fun_005c5250();
    u32 fun_005c5260();
    bool fun_005c5270();
    void Clear();
};

STATIC_ASSERT(sizeof(OccluderSet) == 0xa0);
STATIC_ASSERT(offsetof(OccluderSet, occluders) == 0);
STATIC_ASSERT(offsetof(OccluderSet, indices) == 4);
STATIC_ASSERT(offsetof(OccluderSet, capacity) == 8);
STATIC_ASSERT(offsetof(OccluderSet, count) == 0xc);
STATIC_ASSERT(offsetof(OccluderSet, queries_prepared) == 0x10);
STATIC_ASSERT(offsetof(OccluderSet, projection_matrix) == 0x20);
STATIC_ASSERT(offsetof(OccluderSet, query_matrix) == 0x60);

struct OcclusionManager {
    OcclusionManager();
    ~OcclusionManager();
    void Init(u32, VARIPTR *, VARIPTR);
    bool initialized;
    bool enabled;
    u8 unknown_02[14];
    OccluderSet sets[2];
    OccluderSet *building_set;
    OccluderSet *current_set;
    f32 unknown_158;
    f32 unknown_15c;
    u32 occluded_count;
    u32 query_count;
    u8 unknown_168[8];
    bool IsOccludedOBB(const NUVEC *, const NUVEC *, const NUMTX *);
    bool IsOccludedSphere(const NUVEC *, f32);
    void EndFrame();
    void BeginFrame();
    void OnCameraSet();
    void RenderZPass() const;
    void RenderStats() const;
    void AddOccluder(const NUVEC *, const NUVEC *, const NUVEC *, const NUVEC *);
    void AddOccluder(const NUVEC *, f32);
    void AddOccluder(const NUVEC *, const NUVEC *, const NUMTX *);
    bool fun_005c5280();
    bool fun_005c5290();
    void fun_005c52b0(f32 threshold);
    void fun_005c52c0(f32 threshold);
    OccluderSet *fun_005c52d0();
    void SetEnabled(bool value);
};

STATIC_ASSERT(sizeof(OcclusionManager) == sizeof(OCCLUSIONMANAGER));
STATIC_ASSERT(offsetof(OcclusionManager, current_set) == 0x154);
STATIC_ASSERT(offsetof(OcclusionManager, initialized) == 0);
STATIC_ASSERT(offsetof(OcclusionManager, enabled) == 1);
STATIC_ASSERT(offsetof(OcclusionManager, sets) == 0x10);
STATIC_ASSERT(offsetof(OcclusionManager, building_set) == 0x150);
STATIC_ASSERT(offsetof(OcclusionManager, unknown_158) == 0x158);
STATIC_ASSERT(offsetof(OcclusionManager, unknown_15c) == 0x15c);
STATIC_ASSERT(offsetof(OcclusionManager, occluded_count) == 0x160);
STATIC_ASSERT(offsetof(OcclusionManager, query_count) == 0x164);
#endif
#ifdef __cplusplus
#ifdef __cplusplus
extern "C" {
#endif
    extern OcclusionManager g_OcclusionManager;
#ifdef __cplusplus
}
#endif
#else
#ifdef __cplusplus
extern "C" {
#endif
    extern OCCLUSIONMANAGER g_OcclusionManager;
#ifdef __cplusplus
}
#endif
#endif

struct AITRIGGERSET_s {
    union {
        u8 pad_0x000[516];

        struct {
            union {
                u32 reserved_000;
                struct FLOWBOX_PC *flowbox;
            };

            union {
                struct gizmo_pc_s *triggers[8];
                struct gizmo_pc_s *gizmos[8];
            };

            union {
                struct AITRIGGERSET_TARGET targets[8];
                AILOCATOR_PC locators[8];
            };
        };
    };

    i8 trigger_indices[8];

    union {
        u8 unknown_20c[0x2];
        u16 field_20e;
        u8 unknown_210[0x4];

        struct {
            u8 trigger_count;
            u8 field_20d;
            u16 field_0x20e;

            union {
                u16 flags_word;

                struct {
                    u8 flags;
                    u8 reserved_211;
                };
            };

            u8 reserved_212[2];
        };
    };
};

STATIC_ASSERT(sizeof(struct AITRIGGERSET_s) == 532);

struct AITRIGGERSETSYS_s {
    struct AITRIGGERSET_s sets[32];
    i8 field_0x4280[64];
    i8 field_0x42c0[64];
    i32 field_0x4300;
};

STATIC_ASSERT(sizeof(struct AITRIGGERSETSYS_s) == 17156);

struct shopitem_s {
    char name[64];
    char special_name[32];
    i16 item_id;
    u8 type;
    u8 unlocked;
    i32 price;
    struct nuhspecial_s special;
};

STATIC_ASSERT(sizeof(struct shopitem_s) == 116);

struct AIPATHCNXCONTROLLER_s {
    NULISTLNK links;
    struct AIPATHCNX_PC *connection;

    union {
        void *target;
        NUSPECIALHANDLE special;
        i32 fake_animation_id;
    };

    i32 gizmo_output;
    u32 on_flags;
    u32 off_flags;
    u32 on_frames[32];
    u8 target_type;
    u8 path_index;

    union {
        u16 flags_word;

        struct {
            u8 flags;
            u8 padding_0xa7;
        };
    };
};

STATIC_ASSERT(sizeof(struct AIPATHCNXCONTROLLER_s) == 168);

STATIC_ASSERT(offsetof(struct AIPATHCNXCONTROLLER_s, special) == 0xc);
STATIC_ASSERT(offsetof(struct AIPATHCNXCONTROLLER_s, on_flags) == 0x1c);
STATIC_ASSERT(offsetof(struct AIPATHCNXCONTROLLER_s, off_flags) == 0x20);
STATIC_ASSERT(offsetof(struct AIPATHCNXCONTROLLER_s, on_frames) == 0x24);
STATIC_ASSERT(offsetof(struct AIPATHCNXCONTROLLER_s, target_type) == 0xa4);
STATIC_ASSERT(offsetof(struct AIPATHCNXCONTROLLER_s, path_index) == 0xa5);
STATIC_ASSERT(offsetof(struct AIPATHCNXCONTROLLER_s, flags_word) == 0xa6);

struct TRAFFICANIM_s {
    struct nuhspecial_s special;
    void *animation;
    f32 end_frame;
    f32 tfactor;
    f32 frame_interval;
    f32 random_interval;
    f32 room_frame_interval;
    f32 y_offset;
    i8 vehicle_indices[16];
    u8 rooms[256];
    i8 vehicle_count;
    u8 disabled;
    u8 reserved_13a[2];
    f32 next_spawn_time;
};

STATIC_ASSERT(sizeof(struct TRAFFICANIM_s) == 320);

struct LevelEditorScene {
    char name[32];
    char directory[128];
    struct NUGSCN_s *scene;
    i32 active : 1;
    i32 editable : 1;
    i32 saved : 1;
    u32 reserved_flags : 29;
};

STATIC_ASSERT(sizeof(struct LevelEditorScene) == 168);
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 5 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)
#ifdef __cplusplus

class FadeBase {
  public:
    FADEINFO_s *info;

    FadeBase() : info(NULL) {
    }

    virtual ~FadeBase();
    virtual void Init(FADEINFO_s *info) = 0;
    virtual void InitFade() = 0;
    virtual void UpdateFade() = 0;
    virtual void DrawFade() = 0;
    virtual FADETYPE_VALUE GetFadeType() const = 0;
};

struct FADEINFO_s {
    u32 direction;
    f32 fade;
    f32 rate;
    i32 busy;
    i32 stage;
    FadeBase *fades[4];
    FADETYPE_VALUE pending_type;
    i32 field_28;
};

STATIC_ASSERT(sizeof(FADEINFO_s) == 0x2c);
STATIC_ASSERT(offsetof(FADEINFO_s, direction) == 0x00);
STATIC_ASSERT(offsetof(FADEINFO_s, fade) == 0x04);
STATIC_ASSERT(offsetof(FADEINFO_s, rate) == 0x08);
STATIC_ASSERT(offsetof(FADEINFO_s, busy) == 0x0c);
STATIC_ASSERT(offsetof(FADEINFO_s, stage) == 0x10);
STATIC_ASSERT(offsetof(FADEINFO_s, fades) == 0x14);
STATIC_ASSERT(offsetof(FADEINFO_s, pending_type) == 0x24);
STATIC_ASSERT(offsetof(FADEINFO_s, field_28) == 0x28);

class FadeSystem : public FADEINFO_s {
  public:
    void Init();
    void Update();
    void Draw();
    void SetStage(char stage);
    i32 SetFade(FADETYPE const &type, u32 frames);
    i32 AddFade(FadeBase *fade);
};

STATIC_ASSERT(sizeof(FadeSystem) == 0x2c);
#endif
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 6
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 6 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)

typedef struct BOLTSYS_PC_s {
    struct BOLTTYPE_PC_s *types;
    i32 count;
    void (*stop_targeting)(struct gameobject_s *, NUVEC *);
    void (*debris)(struct bolt_s *, NUVEC *, i32, NUVEC *, i32);
    void (*shoot_origin)(struct gameobject_s *, NUVEC *);
    u16 (*shoot_direction)(struct gameobject_s *, NUVEC *);
    i32 (*hit_part)(struct bolt_s *, struct PART_PC *);
    i32 (*alternate_fire)(struct gameobject_s *, i32);
} BOLTSYS_PC;

STATIC_ASSERT(offsetof(BOLTSYS_PC, stop_targeting) == 8);
STATIC_ASSERT(offsetof(BOLTSYS_PC, debris) == 0xc);
STATIC_ASSERT(offsetof(BOLTSYS_PC, shoot_origin) == 0x10);
STATIC_ASSERT(offsetof(BOLTSYS_PC, shoot_direction) == 0x14);
STATIC_ASSERT(offsetof(BOLTSYS_PC, hit_part) == 0x18);
STATIC_ASSERT(offsetof(BOLTSYS_PC, alternate_fire) == 0x1c);
STATIC_ASSERT(sizeof(BOLTSYS_PC) == 0x20);

typedef struct bolt_s {
    struct BOLTTYPE_PC_s *type;
    void *owner;
    NUMTX orientation;
    NUMTX effect_orientation;
    NUVEC position;
    NUVEC previous_position;
    NUVEC velocity;
    NUVEC direction;
    NUVEC ray_end;
    f32 scale;
    f32 time;
    f32 speed;
    f32 lifetime;
    f32 radius;
    f32 collision_radius;
    f32 ray_time;
    f32 ray_radius;
    f32 field_e4;
    f32 reflection_height;
    f32 acceleration_y;
    u32 flags;
    u16 surface_x_rotation;
    u16 surface_z_rotation;
    u16 hit_flags;

    union {
        i16 hit_platform;
        u16 unknown_fa;
    };

    i16 hit_debris;
    i16 hit_part_debris;
    u8 active;
    u8 type_id;
    u8 field_102;
    u8 index;
    i8 field_104;
    i8 field_105;
    u16 unknown_106;
    NUVEC bounds_min;
    NUVEC bounds_max;
    NUVEC hit_normal;
    NUVEC dogfight_hit_position;
} BOLT;

// Access prefixes only: the trailing name and complete original sizes are unknown.
struct cutsys_pc_s {
    CUTINFO **records;
    i32 count;
};

STATIC_ASSERT(offsetof(CUTSYS_PC, records) == 0);
STATIC_ASSERT(offsetof(CUTSYS_PC, count) == 4);

STATIC_ASSERT(offsetof(BOLT, speed) == 0xcc);
typedef struct gameanimobjpool_s GAMEANIMOBJPOOL;
typedef void (*PICKUP_UPDATE_FN_PC)(struct WORLDINFO_s *, struct GIZMOPICKUP_PC *);
#ifdef __cplusplus
extern "C" {
#endif
    extern BOLTSYS_PC *BoltSys;

    struct GIZTURRETSYS_s;
    typedef struct pushblock_s PUSH_BLOCK_PC;
#ifdef __cplusplus
}
#endif
typedef struct FADER_s FADER_s;
typedef struct GIZBUILDITSYS_s GIZBUILDITSYS_s;
typedef struct GIZFORCESYS_s GIZFORCESYS_s;
typedef struct GIZMOPICKUPRUNTIMESYS_s GIZMOPICKUPRUNTIMESYS_s;
typedef struct GIZPANELSYS_s GIZPANELSYS_s;
typedef struct GIZTURRETSYS_s GIZTURRETSYS_s;
typedef struct LEVER_s LEVER_s;
typedef struct SPECIALMINIKITSYS_s SPECIALMINIKITSYS_s;
typedef struct TRAFFICANIMSYS_s TRAFFICANIMSYS_s;
typedef struct pushblock_s pushblock_s;

struct LEVER_s {
    NUMTX transform;
    struct MechObjectInterface *mech_object;
    NUVEC floor_position;
    NUVEC target_offset;
    char name[16];
    NUVEC position;
    f32 idle_animation_timer;
    f32 pull_progress;
    f32 auto_reset_timer;
    f32 flash_timer;
    f32 auto_reset_delay;
    f32 target_indicator_scale;
    u16 y_rotation;
    i16 model_special_index;
    i16 platform_id;
    u16 animation_frame;
    u16 target_x_rotation;
    u16 target_z_rotation;

    union {
        u16 flags;

        struct {
            u8 interacting : 1;
            u8 being_pulled : 1;
            u8 goodie : 1;
            u8 baddie : 1;
            u8 visible : 1;
            u8 auto_reset : 1;
            u8 returning : 1;
            u8 enabled : 1;
            u8 flags_high;
        };
    };

    char model_variant;
    u8 field_0x9f[9];
};

STATIC_ASSERT(sizeof(struct LEVER_s) == 168);

struct GIZPANELSYS_s {
    i32 count;
    struct GIZPANEL_s *panels;
};

STATIC_ASSERT(sizeof(struct GIZPANELSYS_s) == 8);

struct TRAFFICANIMSYS_s {
    struct TRAFFICANIM_s animations[64];
    struct nuhspecial_s vehicles[16];
    struct TRAFFICANIMINSTANCE_s instances[500];
    NULISTHDR free_instances;
    NULISTHDR active_instances;
    i8 animation_count;
    i8 vehicle_count;
    u8 reserved_77e2[2];
};

STATIC_ASSERT(sizeof(struct TRAFFICANIMSYS_s) == 30692);

struct GIZMOPICKUPRUNTIMESYS_s {
    struct GIZMOPICKUP_PC *pickups;
    struct GIZMOPICKUP_PC *temporary_pickups;
    i32 pickup_count;
    i32 temporary_cursor;
    i32 total_score;
    f32 draw_distance;
    f32 pickup_scale;
};

STATIC_ASSERT(sizeof(struct GIZMOPICKUPRUNTIMESYS_s) == 28);

struct SPECIALMINIKITSYS_s {
    struct SPECIALMINIKIT_s *items;
    i32 count;
};

STATIC_ASSERT(sizeof(struct SPECIALMINIKITSYS_s) == 8);

struct SPECIALMINIKIT_s {
    struct nuhspecial_s special;
    void *inst_anim;
    void *anim_data;
    f32 end_frame;
    char special_name[16];
    char pickup_name[8];
    struct gizmo_pc_s *special_gizmo;
    struct gizmo_pc_s *pickup_gizmo;
    u8 flags;
};

STATIC_ASSERT(sizeof(struct SPECIALMINIKIT_s) == 60);

struct FADER_s {
    struct nuhspecial_s special;
    struct nuhspecial_s while_animating;
};

STATIC_ASSERT(sizeof(struct FADER_s) == 24);

struct minitrooperteam_s {
    f32 origin_x;
    f32 origin_z;
    f32 height;
    f32 formation_width;
    f32 formation_depth;
    u16 facing_angle;
    u8 reserved_016[2];

    union {
        u32 packed_state;

        struct {
            u32 formation_shape : 4;
            u32 formation_mode : 4;
            u32 previous_mode : 4;
            u32 mode_changed : 1;
            u32 route_index : 3;
            u32 waypoint_current : 3;
            u32 waypoint_index : 3;
            u32 waypoint_count : 3;
            u32 team_side : 1;
            u32 team_bits : 6;
        };

        struct {
            u8 formation_state;
            u8 route_state;
            u8 waypoint_state;
            u8 team_flags;
        };
    };

    u8 trooper_count;
    u8 reserved_01d[3];
    u32 state_flags;
    f32 state_timer;
    f32 debris_timer;
    f32 fire_timer;
    f32 reserved_02c;
    i32 bolt_type;
    struct nuvec_s *route_point;
    struct nugspline_s *path;
    struct minisnowtrooper_s *troopers;
    NUVEC position;
    i32 target_index;
};

STATIC_ASSERT(sizeof(struct minitrooperteam_s) == 0x54);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, facing_angle) == 0x14);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, packed_state) == 0x18);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, formation_state) == 0x18);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, route_state) == 0x19);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, waypoint_state) == 0x1a);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, team_flags) == 0x1b);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, trooper_count) == 0x1c);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, state_flags) == 0x20);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, state_timer) == 0x24);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, debris_timer) == 0x28);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, fire_timer) == 0x2c);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, reserved_02c) == 0x30);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, bolt_type) == 0x34);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, route_point) == 0x38);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, path) == 0x3c);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, troopers) == 0x40);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, position) == 0x44);
STATIC_ASSERT(offsetof(struct minitrooperteam_s, target_index) == 0x50);
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 7
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 7 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)

typedef struct gameanimobj_s GAMEANIMOBJ;

struct gameanimobj_s {
    GAMEANIMOBJ *next;
    NUSPECIALHANDLE special;
    NUINSTANIM *instance_animation;
    void *animation;
    f32 start_frame;
    f32 end_frame;

    union {
        struct {
            u8 flags;
            u8 reserved_21[3];
        };

        u32 serialized_flags;
    };

    void *object_data;
};

STATIC_ASSERT(sizeof(GAMEANIMOBJ) == 0x28);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, next) == 0);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, special) == 4);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, instance_animation) == 0x10);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, animation) == 0x14);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, start_frame) == 0x18);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, end_frame) == 0x1c);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, flags) == 0x20);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, serialized_flags) == 0x20);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, reserved_21) == 0x21);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, object_data) == 0x24);

struct gameanimobjpool_s {
    u16 object_data_size;
    u16 active_count;
    u16 capacity;
    u16 unknown_06;
    GAMEANIMOBJ *objects;
    GAMEANIMOBJ *free_objects;
    void *object_data;
};

STATIC_ASSERT(sizeof(GAMEANIMOBJPOOL) == 0x14);
STATIC_ASSERT(offsetof(GAMEANIMOBJPOOL, object_data_size) == 0);
STATIC_ASSERT(offsetof(GAMEANIMOBJPOOL, active_count) == 2);
STATIC_ASSERT(offsetof(GAMEANIMOBJPOOL, capacity) == 4);
STATIC_ASSERT(offsetof(GAMEANIMOBJPOOL, objects) == 8);
STATIC_ASSERT(offsetof(GAMEANIMOBJPOOL, free_objects) == 0xc);
STATIC_ASSERT(offsetof(GAMEANIMOBJPOOL, object_data) == 0x10);

struct gameanimset_s {
    NULISTLNK links;
    u8 object_count;
    u8 animated_object_count;
    u16 flags;
    i32 state;
    GAMEANIMOBJPOOL *object_pool;
    GAMEANIMSYS *system;
    GAMEANIMOBJ *objects;
};

STATIC_ASSERT(sizeof(GAMEANIMSET) == 0x1c);
STATIC_ASSERT(offsetof(GAMEANIMSET, links) == 0);
STATIC_ASSERT(offsetof(GAMEANIMSET, object_count) == 8);
STATIC_ASSERT(offsetof(GAMEANIMSET, animated_object_count) == 9);
STATIC_ASSERT(offsetof(GAMEANIMSET, flags) == 0xa);
STATIC_ASSERT(offsetof(GAMEANIMSET, state) == 0xc);
STATIC_ASSERT(offsetof(GAMEANIMSET, object_pool) == 0x10);
STATIC_ASSERT(offsetof(GAMEANIMSET, system) == 0x14);
STATIC_ASSERT(offsetof(GAMEANIMSET, objects) == 0x18);

struct gameanimsys_s {
    NULISTHDR active_sets;
    i32 set_count;
    GAMEANIMSET **sets;
};

STATIC_ASSERT(sizeof(GAMEANIMSYS) == 0x10);
STATIC_ASSERT(offsetof(GAMEANIMSYS, active_sets) == 0);
STATIC_ASSERT(offsetof(GAMEANIMSYS, set_count) == 8);
STATIC_ASSERT(offsetof(GAMEANIMSYS, sets) == 0xc);

typedef struct gizobstacle_s {
    char name[16];
    NUVEC position;
    NUVEC secondary_position;
    NUVEC evaluated_position;
    GAMEANIMSET *anim_set;
    struct gameobject_s *triggering_object;
    f32 auto_return_delay;
    f32 auto_return_timer;
    f32 trigger_radius;
    f32 animation_speed;
    f32 forward_speed_factor;
    f32 backward_speed_factor;
    f32 reflection_alpha;
    f32 evaluated_radius;
    NUVEC trigger_box_half_extents;
    u32 config_flags;
    u32 exclude_flags;
    NUVEC pickup_offset;
    f32 pickup_scatter_height;
    i16 pickup_direction_x;
    i16 pickup_direction_y;
    i16 trigger_box_yaw;
    i16 room_id;
    i16 blowup_type;
    i16 pickup_count;
    i16 start_sfx_id;
    i16 stop_sfx_id;
    u8 playback_state;
    u8 mode;
    u8 trigger_mode;
    u8 unknown_093;
    u32 progress_flags;
    i32 proximity_output;
    u32 runtime_flags;
} GIZOBSTACLE;

STATIC_ASSERT(sizeof(GIZOBSTACLE) == 0xa0);
STATIC_ASSERT(offsetof(GIZOBSTACLE, triggering_object) == 0x38);
STATIC_ASSERT(offsetof(GIZOBSTACLE, auto_return_timer) == 0x40);
STATIC_ASSERT(offsetof(GIZOBSTACLE, name) == 0);
STATIC_ASSERT(offsetof(GIZOBSTACLE, position) == 0x10);
STATIC_ASSERT(offsetof(GIZOBSTACLE, stop_sfx_id) == 0x8e);
STATIC_ASSERT(offsetof(GIZOBSTACLE, secondary_position) == 0x1c);
STATIC_ASSERT(offsetof(GIZOBSTACLE, auto_return_delay) == 0x3c);
STATIC_ASSERT(offsetof(GIZOBSTACLE, reflection_alpha) == 0x54);
STATIC_ASSERT(offsetof(GIZOBSTACLE, trigger_box_half_extents) == 0x5c);
STATIC_ASSERT(offsetof(GIZOBSTACLE, config_flags) == 0x68);
STATIC_ASSERT(offsetof(GIZOBSTACLE, exclude_flags) == 0x6c);
STATIC_ASSERT(offsetof(GIZOBSTACLE, pickup_offset) == 0x70);
STATIC_ASSERT(offsetof(GIZOBSTACLE, pickup_scatter_height) == 0x7c);
STATIC_ASSERT(offsetof(GIZOBSTACLE, pickup_direction_x) == 0x80);
STATIC_ASSERT(offsetof(GIZOBSTACLE, pickup_direction_y) == 0x82);
STATIC_ASSERT(offsetof(GIZOBSTACLE, trigger_box_yaw) == 0x84);
STATIC_ASSERT(offsetof(GIZOBSTACLE, blowup_type) == 0x88);
STATIC_ASSERT(offsetof(GIZOBSTACLE, pickup_count) == 0x8a);
STATIC_ASSERT(offsetof(GIZOBSTACLE, trigger_mode) == 0x92);
STATIC_ASSERT(offsetof(GIZOBSTACLE, evaluated_position) == 0x28);
STATIC_ASSERT(offsetof(GIZOBSTACLE, evaluated_radius) == 0x58);
STATIC_ASSERT(offsetof(GIZOBSTACLE, room_id) == 0x86);
STATIC_ASSERT(offsetof(GIZOBSTACLE, progress_flags) == 0x94);
STATIC_ASSERT(offsetof(GIZOBSTACLE, proximity_output) == 0x98);
STATIC_ASSERT(offsetof(GIZOBSTACLE, anim_set) == 0x34);
STATIC_ASSERT(offsetof(GIZOBSTACLE, trigger_radius) == 0x44);
STATIC_ASSERT(offsetof(GIZOBSTACLE, start_sfx_id) == 0x8c);
STATIC_ASSERT(offsetof(GIZOBSTACLE, mode) == 0x91);
STATIC_ASSERT(offsetof(GIZOBSTACLE, animation_speed) == 0x48);
STATIC_ASSERT(offsetof(GIZOBSTACLE, forward_speed_factor) == 0x4c);
STATIC_ASSERT(offsetof(GIZOBSTACLE, backward_speed_factor) == 0x50);
STATIC_ASSERT(offsetof(GIZOBSTACLE, playback_state) == 0x90);
STATIC_ASSERT(offsetof(GIZOBSTACLE, runtime_flags) == 0x9c);
typedef void (*PICKUP_COLLECT_FN_PC)(struct WORLDINFO_s *, struct GIZMOPICKUP_PC *, i32, struct gameobject_s *, i32);

typedef struct GIZMO_PICKUP_TYPE_PC {
    char *name;
    char *activation_sfx_name;
    char *collection_sfx_name;
    u8 field_c;
    u8 random_model_count;
    u8 flags;
    u8 field_0f;
    i16 first_model_id;
    u16 score;
    f32 shadow_radius_x;
    f32 shadow_radius_z;
    f32 draw_distance;
    f32 bob_height;
    PICKUP_UPDATE_FN_PC update_fn;
    PICKUP_COLLECT_FN_PC collect_fn;
    i16 debris_id;
    i16 overlay_model_id;
    f32 shadow_extent_x;
    f32 shadow_extent_z;
} GIZMO_PICKUP_TYPE_PC;

STATIC_ASSERT(sizeof(GIZMO_PICKUP_TYPE_PC) == 0x38);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, name) == 0x0);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, activation_sfx_name) == 0x4);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, collection_sfx_name) == 0x8);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, field_c) == 0xc);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, random_model_count) == 0xd);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, flags) == 0xe);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, field_0f) == 0xf);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, first_model_id) == 0x10);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, score) == 0x12);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, shadow_radius_x) == 0x14);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, shadow_radius_z) == 0x18);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, draw_distance) == 0x1c);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, bob_height) == 0x20);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, update_fn) == 0x24);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, collect_fn) == 0x28);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, debris_id) == 0x2c);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, overlay_model_id) == 0x2e);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, shadow_extent_x) == 0x30);
STATIC_ASSERT(offsetof(GIZMO_PICKUP_TYPE_PC, shadow_extent_z) == 0x34);

struct gizforce_pc_s {
    char name[16];
    NUVEC file_position;
    NUVEC position;
    GAMEANIMSET *anim_set;
    NUSPECIALHANDLE along_socket;
    struct gameobject_s *using_object;
    u32 reset_word;
    struct FORCE_RESET_GROUP_PC *group;
    f32 force_strength;
    f32 field_48;
    f32 force_range;
    f32 field_50;
    f32 interaction_radius;
    f32 animation_speed;
    f32 start_frame;
    f32 end_frame;
    f32 effect_scale;
    f32 radius;
    f32 vertical_range;
    f32 horizontal_range;
    u32 config_flags;
    u8 unknown_078[4];
    u8 group_index;
    u8 collision_mask;
    u8 activation_mode;
    u8 unknown_07f;
    i16 room_id;
    i16 start_sfx_id;
    i16 loop_sfx_id;
    i16 stop_sfx_id;
    i16 blowup_type;
    i16 pickup_count;
    i16 pickup_rotation_x;
    i16 pickup_rotation_y;
    NUVEC effect_position;
    f32 activation_radius;

    union {
        u32 runtime_flags;

        struct {
            u32 enabled : 1;
            u32 visible : 1;
            u32 runtime_bits_2_8 : 7;
            u32 reward_released : 1;
            u32 runtime_bit_10 : 1;
            u32 socket_hidden : 1;
            u32 runtime_bits_12_13 : 2;
            u32 completion_released : 1;
            u32 completion_pending : 1;
            u32 destroyed : 1;
            u32 runtime_bits_17_20 : 4;
            u32 being_used : 1;
            u32 runtime_bits_22_31 : 10;
        };
    };
};
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct EXPLOSION_PC {
        i32 field_00;
        i32 field_04;
        struct gameobject_s *owner;
        NUVEC position;
        f32 radius;
        f32 time;
        f32 duration;
        u32 flags;
        u8 unknown_28[2];
        u16 random_angle;
        i16 effect_id;
        u16 hit_flags;
        u16 field_30;
        u8 field_32;
        i8 field_33;
        u8 unknown_34[4];
    } EXPLOSION_PC;

    STATIC_ASSERT(sizeof(EXPLOSION_PC) == 0x38);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, owner) == 8);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, position) == 0xc);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, radius) == 0x18);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, time) == 0x1c);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, duration) == 0x20);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, flags) == 0x24);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, field_00) == 0x0);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, field_04) == 0x4);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, random_angle) == 0x2a);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, effect_id) == 0x2c);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, hit_flags) == 0x2e);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, field_30) == 0x30);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, field_32) == 0x32);
    STATIC_ASSERT(offsetof(EXPLOSION_PC, field_33) == 0x33);
#ifdef __cplusplus
}
#endif

struct GIZFORCE_s {
    char name[16];
    NUVEC file_position;
    NUVEC position;
    struct gameanimset_s *anim_set;
    struct nuhspecial_s along_socket;
    struct gameobject_s *using_object;

    union {
        f32 field_0x3c;
        u32 field_0x3c_bits;
    };
    struct GIZFORCEGROUP_s *group;
    f32 force_strength;
    f32 field_0x48;
    f32 force_range;
    f32 field_0x50;
    f32 interaction_radius;
    f32 animation_speed;
    f32 start_frame;
    f32 end_frame;
    f32 effect_scale;
    f32 radius;
    f32 strength_0x6c;
    f32 vertical_range;
    f32 horizontal_range;
    u32 config_flags;
    u32 field_0x7c;
    u8 group_index;
    u8 collision_mask;
    u8 activation_mode;
    u8 field_0x83;
    i16 room_id;
    i16 start_sfx_id;
    i16 loop_sfx_id;
    i16 stop_sfx_id;
    i16 blowup_type;

    union {
        i16 debris_type;
        u16 pickup_count;
        u16 completion_score;
    };

    union {
        i16 hit_points;
        u16 pickup_direction_x;
        u16 pickup_rotation_x;
    };

    union {
        i16 score;
        u16 pickup_direction_y;
        u16 pickup_rotation_y;
    };

    union {
        NUVEC effect_position;
        NUVEC pickup_offset;
    };

    union {
        f32 activation_radius;
        f32 pickup_scatter_height;
    };

    union {
        u32 field_0xa4;
        struct MechObjectInterface *mech_object_interface;
    };

    union {
        struct {
            union {
                u8 progress_flags;

                struct {
                    u8 progress_enabled : 1;
                    u8 progress_visible : 1;
                    u8 : 2;
                    u8 progress_reverse_active : 1;
                    u8 progress_group_member : 1;
                    u8 progress_animation_reversed : 1;
                    u8 progress_draw_active : 1;
                };
            };

            union {
                u8 runtime_flags;

                struct {
                    u8 runtime_has_platform : 1;
                    u8 runtime_reward_released : 1;
                    u8 runtime_pending_blowup_type : 1;
                    u8 runtime_along_socket_hidden : 1;
                    u8 runtime_offset_applied : 1;
                    u8 runtime_force_range_complete : 1;
                    u8 runtime_completion_released : 1;
                    u8 runtime_pending_completion : 1;
                };
            };

            union {
                u8 field_0xaa;
                u8 state_flags;

                struct {
                    u8 state_destroyed_or_thrown : 1;
                    u8 state_debris_active : 1;
                    u8 : 3;
                    u8 state_being_used : 1;
                    u8 : 2;
                };
            };

            u8 field_0xab;
        };

        u32 packed_state;
    };
};

STATIC_ASSERT(sizeof(struct GIZFORCE_s) == 172);

struct DETONATOR_s {
    NUVEC position;
    NUVEC field_0x0c;
    NUVEC field_0x18;
    struct gameobject_s *object;
    u8 active;
    u8 draw_result;
    u16 rotation_x;
    u16 rotation_y;
    u16 rotation_z;
    f32 timer;
    void *field_0x34;
};

STATIC_ASSERT(sizeof(struct DETONATOR_s) == 56);

struct EXPLOSION {
    i32 field_0x00;
    i32 field_0x04;
    struct gameobject_s *object;
    NUVEC position;
    f32 field_0x18;
    f32 field_0x1c;
    f32 field_0x20;
    i32 field_0x24;
    u16 field_0x28;
    u16 field_0x2a;
    u16 field_0x2c;
    u16 field_0x2e;
    u16 field_0x30;
    u8 field_0x32;
    u8 field_0x33;
};

STATIC_ASSERT(sizeof(struct EXPLOSION) == 52);

struct CABLE_s {
    struct gameobject_s *source;
    struct gameobject_s *target;
    NUVEC points[16];
    NUVEC velocities[16];
    f32 segment_lengths[15];
    f32 max_length;
    f32 total_length;
    u8 unknown_1cc[4];
    f32 slack;
    f32 pull_time;
    u8 wrap_indices[15];
    u8 point_count;
    u8 wrap_count;
    u8 flags_1e9;
    u8 unknown_1ea[2];
};

STATIC_ASSERT(sizeof(struct CABLE_s) == 492);

struct GIZBUILDIT_s {
    char name[16];
    struct gameanimset_s *anim_set;
    struct gameanimobj_s **anim_objects;
    struct GIZBUILDIT_s *linked_buildit;
    struct MechObjectInterface *mech_object_interface;
    NUVEC file_position;
    NUVEC start_position;
    NUVEC position;
    f32 bounds_radius;
    f32 step_timer;
    f32 step_duration;
    f32 interaction_radius;
    f32 field_0x54;
    f32 radius_scale;
    i16 field_0x5c;

    union {
        i16 field_0x5e;
        u16 completion_score;
    };

    i16 field_0x60;
    i16 field_0x62;
    NUVEC effect_position;
    f32 activation_radius;
    u8 anim_object_count;
    u8 build_state;
    u8 builders_active;
    u8 built_object_count;
    u8 field_0x78;
    u8 progress;
    u16 state_flags;
    u16 field_0x7c;
    i16 room_index;
    i16 blowup_type;
    u8 availability_flags;
    u8 field_0x83;
};

STATIC_ASSERT(sizeof(struct GIZBUILDIT_s) == 132);

struct STATUSPACKET_s {
    struct STATUSPACKET_LSW_s *lsw_packet;
    i32 (*init_callback)(struct WORLDINFO_s *arg0, struct STATUSPACKET_s *arg1);
    i32 (*finish_callback)(struct WORLDINFO_s *arg0, struct STATUSPACKET_s *arg1, i32 arg2);
    void (*reset_callback)(struct STATUSPACKET_s *arg0);
    void (*draw_background_callback)(struct STATUSPACKET_s *arg0);
    struct area_data_pc_s *area;
    struct episode_data_pc_s *episode;
    u32 *score;
    struct mission_data_pc_s *mission;
    f32 player0_rumble_amount;
    f32 player0_rumble_time;
    f32 player0_rumble_duration;
    f32 player0_buzz_amount;
    u8 player0_rumble_priority;
    u8 field_0x35[3];
    f32 player1_rumble_amount;
    f32 player1_rumble_time;
    f32 player1_rumble_duration;
    f32 player1_buzz_amount;
    u8 player1_rumble_priority;
    u8 field_0x49[3];
    f32 area_time;
    f32 collected_score;
    f32 true_hero_target;
    f32 true_hero_percent;
    f32 previous_best_time;
    f32 new_best_time;
    f32 elapsed_time;
    f32 field_0x68;
    u32 previous_best_score;
    u32 new_best_score;
    u32 original_score;
    u32 reward_score;
    u32 time_reward_score;
    u32 final_reward_score;
    u32 coins_remaining[2];
    u32 coins_collected[2];
    i32 new_minikits;
    u32 field_0x98;
    u16 player0_model;
    u16 player1_model;
    i16 area_id;
    i8 episode_id;
    u8 field_0xa3;

    union {
        u8 player_active[2];

        struct {
            u8 player0_active;
            u8 player1_active;
        };
    };

    u8 challenge_state;
    u8 mission_state;
    f32 superstory_time;
    u32 superstory_score;

    union {
        u32 flags_word;

        struct {
            union {
                u8 field_0xb0;

                struct {
                    u8 : 2;
                    u8 true_hero_complete : 1;
                    u8 next_area_unlocked : 1;
                    u8 new_minikit_complete : 1;
                    u8 bonus_winner : 1;
                    u8 free_play : 1;
                    u8 vehicle_area : 1;
                };
            };

            union {
                u8 mode_flags;

                struct {
                    u8 bonus_mode : 1;
                    u8 super_bonus_mode : 1;
                    u8 super_story_mode : 1;
                    u8 saved_and_exited : 1;
                    u8 continue_story : 1;
                    u8 : 3;
                };
            };

            u8 reserved_flags_b2[2];
        };
    };

    u8 status_flags;
    u8 minikit_count;
    u8 minikit_max;
    u8 stage_count;
    i8 current_gold_brick;
    u8 save_state;
    u8 prompt_choice;
    u8 newly_completed;
    u8 previous_completion;
    u8 previous_gold_bricks;
    u8 displayed_gold_bricks;
    u8 field_0xbd;
    i8 field_0xbe;
    u8 field_0xbf[7];
    i8 stage_types[40];
    u8 gold_brick_enabled[40];
    i16 next_area;
    i8 chapter;
    u8 field_0x119[1];
    struct STATUS_STAGE_s *previous_stage_2;
    struct STATUS_STAGE_s *previous_stage;
    struct STATUS_STAGE_s *stage;
    struct STATUS_STAGE_s *next_stage;
    u8 field_0x12c[32];
};

STATIC_ASSERT(sizeof(struct STATUSPACKET_s) == 332);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, lsw_packet) == 0x0);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, init_callback) == 0x4);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, finish_callback) == 0x8);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, reset_callback) == 0xc);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, draw_background_callback) == 0x10);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, player0_rumble_amount) == 0x24);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, player1_rumble_amount) == 0x38);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, field_0x68) == 0x68);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, player0_model) == 0x9c);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, player_active) == 0xa4);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, flags_word) == 0xb0);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, field_0xb0) == 0xb0);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, mode_flags) == 0xb1);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, status_flags) == 0xb4);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, minikit_count) == 0xb5);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, minikit_max) == 0xb6);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, stage_count) == 0xb7);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, current_gold_brick) == 0xb8);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, score) == 0x1c);
STATIC_ASSERT(offsetof(STATUSPACKET_s, elapsed_time) == 0x64);
STATIC_ASSERT(offsetof(STATUSPACKET_s, collected_score) == 0x50);
STATIC_ASSERT(offsetof(STATUSPACKET_s, true_hero_target) == 0x54);
STATIC_ASSERT(offsetof(STATUSPACKET_s, true_hero_percent) == 0x58);
STATIC_ASSERT(offsetof(STATUSPACKET_s, original_score) == 0x74);
STATIC_ASSERT(offsetof(STATUSPACKET_s, reward_score) == 0x78);
STATIC_ASSERT(offsetof(STATUSPACKET_s, episode_id) == 0xa2);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, coins_remaining) == 0x84);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, coins_collected) == 0x8c);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, save_state) == 0xb9);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, prompt_choice) == 0xba);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, challenge_state) == 0xa6);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, mission_state) == 0xa7);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, stage_types) == 0xc8);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, gold_brick_enabled) == 0xf0);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, next_area) == 0x118);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, chapter) == 0x11a);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, previous_stage_2) == 0x11c);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, previous_stage) == 0x120);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, stage) == 0x124);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, next_stage) == 0x128);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, field_0x12c) == 0x12c);
STATIC_ASSERT(offsetof(STATUSPACKET_s, mission) == 0x20);
STATIC_ASSERT(offsetof(struct STATUSPACKET_s, previous_gold_bricks) == 0xbd);

struct STATUS_STAGE_s {
    void (*draw_callback)(struct STATUS_STAGE_s *arg0, struct STATUSPACKET_s *arg1, i32 arg2);
    void (*update_callback)(struct STATUS_STAGE_s *arg0, struct STATUSPACKET_s *arg1, f32 arg2);
    void (*skip_callback)(struct STATUS_STAGE_s *arg0, struct STATUSPACKET_s *arg1);
    i32 type;
    u16 field_0x10;
    u8 field_0x12;
    u8 pad_0x13;
    i32 field_0x14;
    f32 field_0x18;
    f32 field_0x1c;
};

STATIC_ASSERT(sizeof(struct STATUS_STAGE_s) == 32);
STATIC_ASSERT(offsetof(struct STATUS_STAGE_s, type) == 0xc);
STATIC_ASSERT(offsetof(struct STATUS_STAGE_s, field_0x12) == 0x12);
STATIC_ASSERT(offsetof(struct STATUS_STAGE_s, field_0x14) == 0x14);
STATIC_ASSERT(offsetof(struct STATUS_STAGE_s, field_0x18) == 0x18);
STATIC_ASSERT(offsetof(struct STATUS_STAGE_s, field_0x1c) == 0x1c);

struct GIZTURRET_s {
    u8 field_0x00[8];
    char name[16];
    struct gameanimset_s *anim_set;
    struct gameanimobj_s *primary_anim_obj;
    struct gameanimobj_s *secondary_anim_obj;
    NUVEC position;
    NUVEC opponent_origin;
    NUVEC initial_origin;
    NUVEC opponent_bounds;
    i32 pitch;
    i32 pitch_upper;
    i32 pitch_lower;
    i32 yaw;
    i32 yaw_upper;
    i32 yaw_lower;
    i32 fixed_pitch;
    i32 fixed_yaw;
    NUVEC muzzle_offsets[4];
    NUMTX relative_matrix;
    void *opponent;
    struct gameobject_s *controller;
    f32 opponent_range;
    f32 fire_range;
    f32 reflection_alpha;
    f32 fire_cooldown;
    f32 fire_interval;
    f32 pitch_turn_speed;
    f32 yaw_turn_speed;

    union {
        u32 behavior_flags;

        struct {
            u8 behavior_flags_low;
            u8 animation_flags;
            u16 behavior_flags_high;
        };
    };

    union {
        i16 field_0x10c;
        u16 completion_score;
    };

    i16 room_id;
    i16 pickup_pitch;
    i16 pickup_yaw;
    NUVEC pickup_offset;
    f32 pickup_radius;
    i16 blowup_type;
    i16 rotate_sfx;
    i16 base_y_rotation;
    i16 fire_sfx;
    u8 opponent_type;
    i8 bolt_type_id;
    u8 state;
    u8 initial_state;
    u8 muzzle_count;
    u8 shots_required;
    u8 shots_fired;
    u8 field_0x133;
    u16 y_rotation;
    u8 field_0x136[2];
    i16 destroy_sfx;

    union {
        u16 flags;

        struct {
            u16 : 1;
            u16 active : 1;
            u16 visible : 1;
            u16 aim_locked : 1;
            u16 aim_at_fixed_angles : 1;
            u16 update_disabled : 1;
            u16 rapid_fire_pending : 1;
            u16 fired_this_frame : 1;
            u16 blowup_pending : 1;
            u16 has_platform : 1;
            u16 reward_released : 1;
            u16 rotation_sound_playing : 1;
            u16 : 4;
        };
    };
};

STATIC_ASSERT(sizeof(struct GIZTURRET_s) == 0x13c);

struct GIZFORCEGROUP_s {
    struct GIZFORCE_s *forces[8];
    f32 combined_height;
    u8 field_0x24;
    u8 count;
    u8 configured_count;
    u8 field_0x27;
};

STATIC_ASSERT(sizeof(struct GIZFORCEGROUP_s) == 40);

struct GIZFORCESYS_s {
    struct GIZFORCE_s *forces;
    struct GIZFORCE_s **visible_forces;
    struct gizmo_pc_s **hit_test_gizmos;
    u16 capacity;
    u16 count;
    u16 visible_force_count;
    u16 hit_test_gizmo_count;
    struct gameanimobjpool_s *anim_pool;
    struct GIZFORCEGROUP_s groups[8];
};

STATIC_ASSERT(sizeof(struct GIZFORCESYS_s) == 344);

struct HOTHBATTLE_MELEE_WAVE_s {
    i16 character_id;
    u8 reserved_02[2];
    struct gameobject_s *creatures[4];

    union {
        u8 remaining_count;
        u8 field_0x18;
    };

    union {
        u8 initial_count;
        u8 field_0x19;
    };

    union {
        u8 active_count;
        u8 reserved_1a;
    };

    char name[13];
    u8 reserved_24[4];
};

STATIC_ASSERT(sizeof(struct HOTHBATTLE_MELEE_WAVE_s) == 40);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_WAVE_s, character_id) == 0);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_WAVE_s, creatures) == 4);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_WAVE_s, remaining_count) == 0x14);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_WAVE_s, initial_count) == 0x15);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_WAVE_s, active_count) == 0x16);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_WAVE_s, name) == 0x17);

struct HOTHBATTLE_MELEE_s {
    union {
        u8 current_wave;
        u8 field_0x0;
    };

    union {
        u8 next_wave;
        u8 field_0x1;
    };

    union {
        u8 initialize_wave;
        u8 field_0x2;
    };

    u8 field_0x3;

    union {
        i8 transition_phase;
        i8 field_0x4;
    };

    u8 field_0x5[3];
    f32 wave_delay;
    struct HOTHBATTLE_MELEE_WAVE_s waves[4];
    struct gameobject_s *background_creatures[6];
    u8 creature_count;
    u8 reserved_0xc5[3];
};

STATIC_ASSERT(sizeof(struct HOTHBATTLE_MELEE_s) == 200);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_s, current_wave) == 0);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_s, next_wave) == 1);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_s, initialize_wave) == 2);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_s, transition_phase) == 4);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_s, wave_delay) == 8);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_s, waves) == 0xc);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_s, background_creatures) == 0xac);
STATIC_ASSERT(offsetof(struct HOTHBATTLE_MELEE_s, creature_count) == 0xc4);

struct GIZSPECIAL_s {
    char name[32];
    struct gameanimset_s *anim_set;

    union {
        u8 flags;

        struct {
            u8 is_reversed : 1;
            u8 is_active : 1;
            u8 field_0x24_2 : 6;
        };
    };

    u8 field_0x25[3];
};

STATIC_ASSERT(sizeof(struct GIZSPECIAL_s) == 40);

struct GIZOBSTACLESYS_s {
    struct gizobstacle_s *obstacles;
    struct gizmo_pc_s **active_gizmos;
    u16 count;
    u16 capacity;
    u16 active_gizmo_count;
    u16 field_0x0e;
    struct gameanimobjpool_s *anim_pool;
};

STATIC_ASSERT(sizeof(struct GIZOBSTACLESYS_s) == 20);

struct CHARPLATFORM_s {
    struct nuhspecial_s special;
    i16 object_id;
    i16 platform_id;
    struct gameobject_s *object;
};

STATIC_ASSERT(sizeof(struct CHARPLATFORM_s) == 20);

struct CHARPLATFORMSYS_s {
    i32 field_0x00;
    i32 platform_count;
    struct CHARPLATFORM_s platforms[1];
};

STATIC_ASSERT(sizeof(struct CHARPLATFORMSYS_s) == 28);

struct GIZBUILDITSYS_s {
    struct GIZBUILDIT_s *buildits;
    u16 count;
    u16 capacity;
    u16 max_animation_objects;
    u16 field_0x0a;
    struct gameanimobjpool_s *anim_pool;
    struct gameanimobj_s **anim_objects;
};

STATIC_ASSERT(sizeof(struct GIZBUILDITSYS_s) == 20);

struct pushblock_s {
    union {
        f32 ground_offset;
        f32 vertical_penetration;
    };

    union {
        f32 ground_height;
        f32 support_height;
    };

    f32 snap_timer;
    f32 settled_height;
    i8 terrain_info[4];

    union {
        i8 bytes[4];
        i32 word;
    } extra_terrain_info;

    i32 previous_extra_terrain_info;

    union {
        struct pushblock_s *supporting_block;
        struct pushblock_s *block_below;
    };
    struct pushblock_s *previous_supporting_block;
    NUSPECIALHANDLE special;
    struct gameobject_s *pushing_object;
    i32 platform_id;
    u8 fields_038[8];
    f32 snap_distance;
    char name[16];
    struct nuvec_s *snap_positions;
    struct nuvec_s *position;
    NUSPECIALHANDLE end_position_specials[2];
    NUVEC end_position_origins[2];
    NUVEC velocity;
    NUVEC target_velocity;
    NUVEC snap_origin;
    NUVEC bounds_min;
    NUVEC bounds_max;

    union {
        u32 packed_state_flags;

        struct {
            u8 runtime_flags_0c8;
            u8 runtime_flags_0c9;
            u8 flags_0ca;
            u8 flags_0cb;
        };

        struct {
            u32 flag_000 : 1;
            u32 unknown_flags_001_005 : 5;
            u32 flag_006 : 1;
            u32 flag_007 : 1;
            u32 unknown_flags_008_011 : 4;
            u32 animation_positions : 3;
            u32 unknown_flags_015_017 : 3;
            u32 push_visible : 1;
            u32 unknown_flags_019_023 : 5;
            u32 push_active : 1;
            u32 progress_state : 1;
            u32 unknown_flags_026_031 : 6;
        };
    };

    u8 unknown_0cc[4];
    u8 completed_snap_positions;
    u8 end_position_count;
    u8 output_count;
    u8 unknown_0d3;
};

STATIC_ASSERT(sizeof(struct pushblock_s) == 0xd4);

struct GIZTURRETSYS_s {
    struct GIZTURRET_s *turrets;
    struct gameanimobjpool_s *anim_pool;
    u16 count;
    u16 capacity;
    i8 opponent_cursor;
    i8 field_0x0d[3];
};

STATIC_ASSERT(sizeof(struct GIZTURRETSYS_s) == 16);
STATIC_ASSERT(offsetof(GIZBUILDIT_s, anim_set) == 0x10);
STATIC_ASSERT(offsetof(GIZBUILDIT_s, state_flags) == 0x7a);
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 8
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 8 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)
typedef struct GIZSPINNER_s GIZSPINNER_s;

struct GIZSPINNER_s {
    NUMTX matrix;
    char name[16];
    struct nuhspecial_s special;
    NUVEC position;
    struct gameanimset_s *anim_set;
    struct gameanimobj_s *primary_anim_obj;

    union {
        u8 field_0x070[8];

        struct {
            f32 field_70;
            u32 field_74;
        };
    };

    f32 animation_speed;
    u16 rotation;
    u16 target_rotation;
    u16 previous_rotation;
    u16 initial_rotation;
    u32 state_flags;
    u8 field_0x088;
    i8 room_index;
    u8 state;
    u8 type;
    u16 field_0x08c;
    u8 field_0x08e[2];
    f32 spin_timer;
    f32 field_0x094;
    f32 field_0x098;
    f32 field_0x09c;
    f32 ground_height;
    u8 output_count;
    u8 field_0x0a5[7];
    u8 flags;
    u8 field_0x0ad;
    i16 platform_id;
    u8 field_0x0b0[4];
    struct GIZSPINNERARM_s arms[8];

    union {
        u8 field_0x2d4[4];
        f32 output_pause_time;
    };

    f32 field_0x2d8;
    f32 animation_points[9];
    struct GAMEANTINODE_s *anti_node;
};

STATIC_ASSERT(sizeof(struct GIZSPINNER_s) == 772);

struct GAMEANTINODE_s {
    NUVEC position;
    f32 radius;
    f32 min_y;
    f32 max_y;
    f32 extent_x;
    f32 extent_z;
    f32 remaining_time;
    u32 user_data[2];
    u16 angle;
    u8 shape;
    u8 grid_min_x;
    u8 grid_min_z;
    u8 grid_max_x;
    u8 grid_max_z;
    u8 reserved_33;
    struct GAMEANTINODE_s *next;
};

STATIC_ASSERT(sizeof(struct GAMEANTINODE_s) == 56);
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 9
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 9 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    typedef DEBRISPARTICLE_PC *(*DEBRISGENERATOR_PC)(struct debris_key_pc_s *, struct debris_info_pc_s *, f32);
    typedef void (*DEBRISMOMENTUMADJUSTER_PC)(struct debris_key_pc_s *, struct debris_info_pc_s *, DEBRISPARTICLE_PC *);
#ifdef __cplusplus
}
#endif

struct nugraph_s {
    i8 interpolation;
    i8 point_count;
    u8 pad_02[2];
    f32 x[8];
    f32 y[8];
    f32 x_scale;
    f32 y_scale;
    f32 x_extent;
    f32 y_extent;
    f32 x_offset;
    f32 y_offset;
};

STATIC_ASSERT(sizeof(struct nugraph_s) == 92);
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 10
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 10 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct debris_chunk_control_pc_s {
        struct debris_particle_chunk_pc_s *particle_chunk;
        f32 expiry_time;
        i32 active;
        struct debris_key_pc_s *owner;
        struct debris_chunk_control_pc_s *next;
        i32 particle_index;
        f32 collision_time;
        i16 effect_index;
        i16 field_01e;
        i16 rotation_y;
        i16 field_022;
        f32 collision_plane;
        f32 restitution;
    } DEBRIS_CHUNK_CONTROL_PC;

    STATIC_ASSERT(sizeof(DEBRIS_CHUNK_CONTROL_PC) == 0x2c);
    STATIC_ASSERT(offsetof(DEBRIS_CHUNK_CONTROL_PC, expiry_time) == 4);
    STATIC_ASSERT(offsetof(DEBRIS_CHUNK_CONTROL_PC, next) == 0x10);
    STATIC_ASSERT(offsetof(DEBRIS_CHUNK_CONTROL_PC, particle_index) == 0x14);
    STATIC_ASSERT(offsetof(DEBRIS_CHUNK_CONTROL_PC, effect_index) == 0x1c);
    STATIC_ASSERT(offsetof(DEBRIS_CHUNK_CONTROL_PC, rotation_y) == 0x20);
    STATIC_ASSERT(offsetof(DEBRIS_CHUNK_CONTROL_PC, collision_plane) == 0x24);
    STATIC_ASSERT(offsetof(DEBRIS_CHUNK_CONTROL_PC, restitution) == 0x28);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 11
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 11 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)
#ifdef __cplusplus

/* Fixed-capacity list of BaseThings run once per pass (Android ThingManager).
 * Every pass is virtual; the 0x24-byte object comes from theMemoryManager's
 * pools and its thing array from the permanent buffer. */
class ThingManager {
  public:
    ThingManager(i32 max_things);

    virtual ~ThingManager() {
    }

    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(ThingManager));
    }

    virtual void AddThing(BaseThing *thing);
    virtual BaseThing *AddThingAfterThis(BaseThing *thing);
    virtual void RemoveTemporaryThings();
    virtual i32 RemoveDependanciesThings(ThingRemoveData *data);
    virtual void ResetThings(ThingResetData *data);
    virtual void EnterLevelThings(ThingLevelData *data);
    virtual void ExitLevelThings(ThingLevelData *data);
    virtual void ProcessThings(ThingProcessData *data);
    virtual void RenderThings(ThingRenderData *data);
    virtual void DisplayThings(ThingRenderData *data);
    virtual void EffectsThings(ThingRenderData *data);
    void EnableActions(u32 id, u32 flags, i32 clear);
    static void cbEdTrackCancel(struct eduimenu_s *menu, struct eduimenu_s *parent);
    static void cbEdTimingSelect(struct eduimenu_s *menu, struct eduiitem_s *item, u32 value);
    static void edTimingInit();
    static void edTimingEnter();
    static i32 edTimingProc(f32 delta_time, struct FRAME_PAD_PC *pad);
    static void edTimingRender();

    BaseThing **things;
    i32 max_things;
    i32 count;
    i32 permanent_count;
    i32 pending;
    i32 timebar;
    i32 timing_thing;
    i32 timing_state;
};

/* The game's ThingManager (theGameThings); PC registers no things. */
class GameThingManager : public ThingManager {
  public:
    static void *operator new(size_t size) {
        return theMemoryManager.AllocPool(size, 1);
    }

    static void operator delete(void *memory) {
        theMemoryManager.FreePool(memory, sizeof(GameThingManager));
    }

    GameThingManager(i32 max_things);
    virtual ~GameThingManager();
    void AddOnceOnlyThings();
};
#endif
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 12
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 12 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)
#ifdef __cplusplus

/* Base of every placeable level-editor object (Android legoapi_types.h). */
class Placeable {
  public:
    i16 scene_id;
    i16 led_file;
    i32 attributes;
    EdString name;
    EdString params;
    u32 reserved_14;

    static i16 CurrentLedFile;

    Placeable() : scene_id(0), led_file(CurrentLedFile), attributes(0), reserved_14(0) {
    }

    virtual ~Placeable();

    virtual Placeable *Clone(i32) const {
        return NULL;
    }

    virtual void Reset() {
    }

    virtual const char *GetName() const {
        const char *value = name.data ? name.data + 1 : NULL;
        return value ? value : "No Name";
    }

    virtual void SetName(const char *value) {
        name.Set(value);
    }

    virtual f32 GetRadius() const {
        return 1.0f;
    }

    virtual const struct VuMtx *GetInitialTransform() const {
        return NULL;
    }

    virtual void SetInitialTransform(const struct VuMtx *) {
    }

    virtual const struct VuMtx *GetCurrentTransform() const {
        return NULL;
    }

    virtual void SetCurrentTransform(const struct VuMtx *) {
    }

    virtual const VuVec *GetInitialPosition() const;
    virtual void SetInitialPosition(const VuVec *position);
    virtual const VuVec *GetCurrentPosition() const;
    virtual void SetCurrentPosition(const VuVec *position);

    virtual void Render(const struct VuMtx *) const {
    }
};

struct ClassObjectListEntry {
    ClassObjectListEntry *next;
    ClassObjectListEntry *previous;
    EdClass *ed_class;
    void *object;
    EdRef *reference;
};

struct ClassObjectList {
    ClassObjectListEntry *first;
    ClassObjectListEntry *last;
    i32 count;

    ClassObjectListEntry *fun_006c1440();
    void fun_006c1450(ClassObjectListEntry *entry, ClassObjectListEntry *position);
    void Remove(ClassObjectListEntry *entry);
    i32 fun_006c14f0();
    i32 GetAveragePosition(VuVec &average);
    i32 GetAveragePosition(VuVec &average, f32 &radius);
    i32 IsInList(ClassObject object);
    i32 IsInList(void *object, EdRef *reference);
    i32 IsInList(EdClass *object_class);
};

class EdTool : public CSListLink<EdTool> {
  public:
    virtual ~EdTool();

    virtual void Initialise(variptr_u &, variptr_u &, i32) {
    }

    virtual char const *GetName() {
        return "";
    }

    virtual i32 Process(EdInputContext &) {
        return 0;
    }

    virtual void Render() {
    }
};

class BaseEditor : public CSListLink<BaseEditor> {
  public:
    virtual ~BaseEditor();

    virtual void Initialise(variptr_u &, variptr_u &, i32 id) {
        this->id = id;
    }

    virtual char const *GetName() {
        return "";
    }

    virtual i32 ReadBlock(DATAPTR *) {
        return 0;
    }

    virtual void WriteBlock(i32) {
    }

    virtual void Serialise(EdStream &) {
    }

    virtual void ClearLevel(i32) {
    }

    virtual void Flush() {
    }

    virtual void Enter() {
    }

    virtual void Exit() {
    }

    virtual void Process(EdInputContext &) {
    }

    virtual void Render() {
    }

    virtual void AddMenuItems(struct eduimenu_s *) {
    }

    static void WriteBeginBlock(NUFILE file, i32 type);
    static void WriteEndBlock(NUFILE file);
    static void WriteMetaData(NUFILE file, i32 type, i32 version, i32 count);
    void ReadBuffer(void **destination, void *source, i32 size);

    i32 id;
    static i32 blockStart[8];
    static i32 blockDepth;
};

/* The class/object editor (Android ClassEditor), theClassEditor. */
class ClassEditor : public BaseEditor {
  public:
    ClassEditor();
    virtual ~ClassEditor();
    virtual void Initialise(variptr_u &, variptr_u &, i32);
    virtual char const *GetName();
    virtual i32 ReadBlock(DATAPTR *);
    virtual void WriteBlock(i32);
    virtual void Serialise(EdStream &);
    virtual void ClearLevel(i32);
    virtual void Flush();
    virtual void Enter();
    virtual void Exit();
    virtual void Process(EdInputContext &);
    virtual void Render();
    virtual void AddMenuItems(struct eduimenu_s *);

    EdTool *first_tool;
    EdTool *last_tool;
    i32 tool_count;
    EdTool *active_tool;
    EdManipulator *manipulator;
    ClassObjectList selected_objects;
    ClassObject current_object;
    i32 field_3c;
    ClassObject pending_object;
    struct eduimenu_s *menu;
    VuVec snap_ray;
    f32 snap_distance;
    i32 snap_mode;
    i32 mode;
    i32 class_filter;

    void DrawObjectSphere(ClassObject &selected, i32 colour);
    static void cbEdFilterLED(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdLevelSelectAll(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdLevelDeselectAll(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    void PreSaveInitialisation();
    void PostSaveInitialisation();
    void PreLoadInitialisation(MemoryBuffer *first, MemoryBuffer *second);
    void PostLoadInitialisation(MemoryBuffer *first, MemoryBuffer *second);
    i32 SelectObject(ClassObject &object, i32 mode);
    i32 IsUniqueName(char *name);
    void MakeUniqueName(char const *name, char *destination, i32 size);
    i32 IsSelectedObject(ClassObject &object);
    i32 IsSelectedObject(void *object, EdRef *reference);
    i32 IsSelectedClass(EdClass *object_class);
    void SnapPoint(VuVec &point);
    i32 Editable(void *object, EdClass *object_class, i32 index);
    void SetMode(i32 new_mode);
    void UpdateClassFilter(EdInputContext &input);
    void UpdateSelectedObjects(EdInputContext &input);
    void DestroySelectedObjects();
    void DestroySelectedObjectsNow();
    void SelectLED(i32 class_id);
    void ViewSelected();
    void FocusSelected();
    void InitialiseObject(ClassObject &object);
    i32 CreateObject(ClassObject &object);
    i32 CreateObject(EdClass *object_class);
    i32 CreateObject(i32 class_id);
    i32 FindNearestObject(VuVec &point, ClassObject &nearest, i32 filter);
    i32 FindNearestObject(VuVec &point, ClassObject &nearest, ClassObject &after, i32 filter);
    i32 FindNearestObject(VuVec &origin, VuVec &direction, ClassObject &nearest, i32 filter);
    i32 FindNearestObject(VuVec &origin, VuVec &direction, ClassObject &nearest, ClassObject &after, i32 filter);
    static i32 cbEdCreateClassNewObject(i32 class_id);
    static i32 cbEdCopySelectedObject(EdInputContext &input);
    static void SetViewMenuHilight(struct eduimenu_s *menu);
    static void cbDestroyMenu(struct eduimenu_s *menu, struct eduimenu_s *);
    static void cbDestroyObject(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassDeleteObject(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassFileMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassModeMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassNewMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassNewObject(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassRemoveDuplicates(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSelectClassMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSelectObject(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSelectObjectMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSetMode(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSetPinned(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSetSnap(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSetView(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassSnapMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassToolsMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbEdClassViewMenu(struct eduimenu_s *parent, struct eduiitem_s *item, u32 buttons);
    static void cbFileSelected(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    EdTool *fun_006b8be0();
    f32 &fun_006b8bf0();
    i32 fun_006b8c10();
    void fun_006b8c20(EdTool *tool);
    void fun_006b8c30(ClassObject &object);
    void fun_006b8c50(i32 value);
    i32 fun_006b8c60(ClassObject &object);
    ClassObjectList &fun_006b8c90();
    void RegisterTool(EdTool &tool);
    ClassObject &fun_006b8ca0();
};
#endif
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 13
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 13 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)
#ifdef __cplusplus

/* The level editor thing (Android LevelEditor), theLevelEditor. The PC input
 * context is 8 bytes larger than Android's, so every field after it sits 8
 * bytes later. */
class LevelEditor : public BaseThing {
  public:
    LevelEditor();

    virtual char const *GetName() {
        return "LevelEditor";
    }

    virtual void ProcessEvenWhenPaused(ThingProcessData *);
    virtual void Display(ThingRenderData *);

    void SetPadText(i32 buttons, char *text);
    void RegisterEditor(BaseEditor &editor);
    void ClearLevel(i32 index);
    void Initalise(variptr_u &buffer, variptr_u &buffer_end, i32 from_front);
    void AddInfoText(char *text);
    void SetCameraPosition(VuVec &position);
    void CreateMenu();
    void CreateEditorList(struct eduimenu_s *parent, struct eduiitem_s *item);
    void Enter();
    void Exit();
    struct NUGSCN_s *GetScene(i32 index);
    LevelEditorScene *GetEdScene(i32 index);
    void Flush();
    i32 WriteStream(EdFileOutputStream &stream);
    i32 ReadStream(EdFileInputStream &stream);
    i32 Save();
    void BeginMultiLoad(variptr_u *buffer, variptr_u *buffer_end);
    void EndMultiLoad(variptr_u *buffer, variptr_u *buffer_end);
    i32 SaveState(i32 scene_index, variptr_u *buffer, variptr_u *buffer_end);
    i32 SaveState(variptr_u *buffer, variptr_u *buffer_end);
    void SetSaveFilename(char *name);
    void Reset();
    i32 AddScene(char *name, struct NUGSCN_s *scene, i32 active);
    i32 FindSceneId(char *name);
    i32 IsEditable(i32 index);
    i32 Load(char *filename, variptr_u *buffer, variptr_u *buffer_end, i32 flags);
    i32 LoadState(variptr_u *buffer, variptr_u *buffer_end, variptr_u *scratch_begin, variptr_u *scratch_end,
                  variptr_u *file_begin, variptr_u *file_end);
    void DrawInfoText(char **lines, i32 count, i32 x, i32 y, i32 available_width, i32 available_height, i32 text_colour,
                      i32 background);
    void fun_006b8ff0(bool value);
    void fun_006b9000(u8 value);
    void fun_006b9010(u8 value);
    i16 fun_006b9020();
    void fun_006b9030(i16 value);
    i32 fun_006b9040();
    f32 *fun_006b9080();
    void fun_006b9090(i32 value);
    i32 fun_006b90a0();
    void fun_006b90b0(i32 value);
    void fun_006b90c0(i32 value);
    i32 fun_006b90e0();
    void fun_006b90f0(BaseEditor *editor);
    struct NUGSCN_s *fun_006bc740(char *name);
    i32 fun_006bc7e0(struct NUGSCN_s *scene);
    void fun_006bc9d0(struct eduimenu_s *menu);
    char *fun_006bc9e0(char *text);

    f32 background_colour[4];
    i32 field_0x20;
    i32 field_0x24;
    i32 field_0x28;
    i32 field_0x2c;
    i32 field_0x30;
    f32 overlay_alpha;
    i32 field_0x38;
    i32 editor_buffer_from_front;
    EdInputContext input;
    u8 loading;
    u8 destroying_objects;
    u8 reserved_0x2a2[2];
    i32 editable_scene_count;
    i32 reset_pending;
    LevelEditorScene scenes[10];
    BaseEditor *first_editor;
    BaseEditor *last_editor;
    i32 editor_count;
    BaseEditor *active_editor;
    char save_filename[0x80];
    char editor_filename[0x80];
    i32 file_version;
    i32 multi_load_active;
    i16 current_led_file;
    u8 reserved_0xa56[2];
    VARIPTR editor_buffer_cursor;
    VARIPTR editor_buffer_begin;
    VARIPTR editor_buffer_end;
    char text_buffer[0x400];
    i32 text_length;
    char *info_text[32];
    i32 info_x;
    i32 info_y;
    i32 info_width;
    i32 info_height;
    u32 info_colour;
    u32 info_background;
    u8 reserved_0xf00[0x100];
    i32 pad_x;
    i32 pad_y;
    i32 pad_width;
    i32 pad_height;
    u32 pad_background;
    u32 pad_colour;
    u8 reserved_0x1018[4];
    char *pad_text[32];
    EditorSettings settings;
    i32 editors_entered;
    i32 active;
};

extern LevelEditor theLevelEditor;
#endif
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 14
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 14 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)
#ifdef __cplusplus

class TTNetwork : public NetTransporter, public BaseThing {
  public:
    TTNetwork();
    ~TTNetwork();
    virtual void ProcessEvenWhenPaused(ThingProcessData *data);
    virtual void Display(ThingRenderData *data);
    void Start();
    void Update();
    void Shutdown();
    virtual void Send(NETMESSAGE_PC message, u8 channel, V2Peer *peer);
    virtual void SendEx(NETMESSAGE_PC message, u8 channel, V2Peer *peer, i32 arg0, i32 arg1);
    virtual void Broadcast(NETMESSAGE_PC message, u8 channel);
    virtual void BroadcastEx(NETMESSAGE_PC message, u8 channel);
    virtual char const *GetName();
    i32 PeerRequest(const V2Peer &peer);
    void PeerJoined(const V2Peer &peer);
    void PeerLeft(const V2Peer &peer, i32 reason);
    void PeerDead(const V2Peer &peer);
    void FtpUpload(FtpFile *file);
    void FtpDownload(FtpFile *file);
    void FtpComplete(FtpFile *file, i32 result);
    i32 NosAcquire(NetworkObject *object, const V2Peer &peer);
    void NosAdopted(NetworkObject *object, const V2Peer &peer);
    void UpdateClientStats();
    void DeliverMessage(NETMESSAGE_PC &message, u8 channel, const V2Peer &peer);
    void RecordSent(NETMESSAGE_PC message, u8 channel);
    void RecordReceived(NETMESSAGE_PC message, u8 channel);
    void Resume();
    i32 Suspend();
    NetAddress *GetMyAddress();
    NetAddress *GetMyHostAddress();
    void SetMyHostAddress(NetAddress *address);
    void ClearMyHostAddress();
    i32 fun_00611dc0() const;

    NetLogFile log;
    u32 frame_count;
    i32 field_2138;
    i32 field_213c;
    i32 state;
    i32 field_2144;
    i32 field_2148;
    i32 field_214c;
    i32 display_page;
    f32 display_x;
    f32 display_y;
    f32 display_width;
    f32 display_height;
    u32 unknown_2164[2];
    NetAddress my_address;
    NetAddress my_host_address;
    i32 has_my_host_address;
    NetFtpManager ftp;
    NetworkObjectManager nos;
    V2SessionManager *session_manager;
};
#endif
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 15
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
