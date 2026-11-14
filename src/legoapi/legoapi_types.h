/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 1
#endif

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

/* 14 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 1 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)

struct BOLTTYPE_PC_s;
struct FLOWBOX;
struct FORCE_RESET_GROUP;
struct FRAME_PAD_PC;
struct GIZFLOW;
struct GIZMOPICKUP;
struct NUGSCN_s;
struct PART_PC;
struct VuMtx;
struct WORLDINFO_s;
struct debris_info_s;
struct debris_key_s;
struct debris_particle_chunk_s;
struct eduiitem_s;
struct eduimenu_s;
struct gameobject_s;
struct mission_data_s;
struct numtl_s;

typedef void (*GIZACTIONFN)(struct GIZFLOW *, struct FLOWBOX *, char **, i32);

typedef struct TERRAIN_TRACK_SLOT { // size 0x18
    void *id;                       // 0x00
    i16 platform_index;             // 0x04
    u16 flags;                      // 0x06
    i16 platform_contact_state;     // 0x08
    i16 wall_contact_state;         // 0x0a
    NUVEC impact_normal;            // 0x0c
} TERRAIN_TRACK_SLOT;

typedef struct TERRAIN_SPHERE_PC { // size 0x10
    NUVEC position;                // 0x0
    f32 radius;                    // 0xc
} TERRAIN_SPHERE_PC;

typedef struct TERRAIN_AXIS_FREEDOM_SHAPE_PC { // size 0x14
    NUVEC offset;                              // 0x00
    f32 angle;                                 // 0x0c
    f32 radius;                                // 0x10
} TERRAIN_AXIS_FREEDOM_SHAPE_PC;

typedef struct TERRAIN_WALL_POINT TERRAIN_WALL_POINT;
typedef struct TERRAIN_SPATIAL_NODE TERRAIN_SPATIAL_NODE;
typedef struct TERRAIN_SHAPE_BATCH TERRAIN_SHAPE_BATCH;

typedef struct TERRAIN_INDEX_LEVEL {   // size 0x80c
    i16 cache_age;                     // 0x000
    u8 unknown_02[2];                  // 0x002
    f32 center_x;                      // 0x004
    f32 center_z;                      // 0x008
    TERRAIN_SCAN_WORD scan_words[512]; // 0x00c
} TERRAIN_INDEX_LEVEL;

typedef struct TERRAIN_GROUP_PC { // size 0x38
    NUVEC origin;                 // 0x00
    TERRAIN_SKIN_STREAM *data;    // 0x0c
    NUVEC bounds_min;             // 0x10
    NUVEC bounds_max;             // 0x1c
    u32 platform_flags;           // 0x28
    i32 chunk_type;               // 0x2c
    i16 scene_index;              // 0x30
    i16 group_info;               // 0x32
    f32 radius;                   // 0x34
} TERRAIN_GROUP_PC;

typedef struct TERRAIN_CELL_PC { // size 0x14
    i16 first_group;             // 0x00
    i16 group_count;             // 0x02
    f32 min_x;                   // 0x04
    f32 min_z;                   // 0x08
    f32 max_x;                   // 0x0c
    f32 max_z;                   // 0x10
} TERRAIN_CELL_PC;

typedef TERRAIN_GROUP_PC TERRAIN_GROUP;
typedef TERRAIN_CELL_PC TERRAIN_CELL;
typedef TERRAIN_POLYGON_INFO TERRAIN_SHAPE;

typedef struct TerrainQuery_s {     // size 0x948
    u32 flags;                      // 0x000
    NUVEC start_position;           // 0x004
    NUVEC start_movement;           // 0x010
    NUVEC position;                 // 0x01c
    NUVEC movement;                 // 0x028
    u8 scan_result;                 // 0x034
    u8 shape_adjusted;              // 0x035
    i16 object_index;               // 0x036
    f32 radius;                     // 0x038
    f32 horizontal_movement_length; // 0x03c
    u8 *hit_flags;                  // 0x040
    f32 movement_pitch;             // 0x044
    f32 movement_yaw;               // 0x048
    f32 movement_length;            // 0x04c
    f32 collision_radius;           // 0x050
    f32 collision_radius_sq;        // 0x054
    f32 inverse_collision_radius;   // 0x058
    f32 object_scale;               // 0x05c
    f32 object_scale_sq;            // 0x060
    f32 inverse_object_scale;       // 0x064
    f32 inverse_object_scale_sq;    // 0x068
    i16 terrain_group_index;        // 0x06c
    u8 pad_06e[2];                  // 0x06e
    NUVEC local_start;              // 0x070
    NUVEC local_end;                // 0x07c
    u8 hit_type;                    // 0x088
    u8 hit_edge;                    // 0x089
    i16 scan_group_index;           // 0x08a
    void *scan_list;                // 0x08c
    TERRAIN_SHAPE *surface;         // 0x090
    TERRAIN_SHAPE *working_surface; // 0x094
    f32 hit_time;                   // 0x098
    f32 unclamped_hit_time;         // 0x09c
    f32 separation_epsilon;         // 0x0a0
    f32 compare_epsilon;            // 0x0a4
    NUVEC movement_normal;          // 0x0a8
    NUVEC impact_normal;            // 0x0b4
    NUVEC platform_position;        // 0x0c0
    NUVEC platform_normal;          // 0x0cc
    NUVEC previous_movement_normal; // 0x0d8
    u8 pad_0e4[0x18];               // 0x0e4
    NUVEC transformed_vertices[4];  // 0x0fc
    u8 pad_12c[0x1c];               // 0x12c

    union { // size 0x800
        u8 scan_list_storage[0x800];
        TERRAIN_SCAN_WORD scan_words[512];
    }; // 0x148
} TerrainQuery_s;

struct TERRAIN_SHAPE_BATCH { // size 0x14
    i16 marker;              // 0x00
    i16 shape_count;         // 0x02
    f32 min_x;               // 0x04
    f32 max_x;               // 0x08
    f32 min_z;               // 0x0c
    f32 max_z;               // 0x10
};

struct TERRAIN_WALL_POINT { // size 0x10
    NUVEC position;         // 0x0
    u8 material[4];         // 0xc
};

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

typedef struct gizaimessage_s { // size 0x38
    NULISTLNK links;            // 0x00
    char name[32];              // 0x08
    f32 value;                  // 0x28
    i8 output_values[8];        // 0x2c
    i8 output_count;            // 0x34
    u8 unknown_35;              // 0x35
    u16 flags;                  // 0x36
} GIZAIMESSAGE;

typedef struct gizaimessagesys_s { // size 0x18
    i32 count;                     // 0x00
    GIZAIMESSAGE *messages;        // 0x04
    NULISTHDR free_list;           // 0x08
    NULISTHDR active_list;         // 0x10
} GIZAIMESSAGESYS;

typedef struct part_emit_s {     // size 0x6c
    NUVEC position;              // 0x00
    i32 effect_id;               // 0x0c
    char name[16];               // 0x10
    f32 time_20;                 // 0x20
    f32 time_24;                 // 0x24
    f32 time_28;                 // 0x28
    i16 rotation_2c;             // 0x2c
    i16 rotation_2e;             // 0x2e
    i16 rotation_30;             // 0x30
    i8 page;                     // 0x32
    u8 field_33;                 // 0x33
    i32 shots_remaining;         // 0x34
    f32 time_38;                 // 0x38
    u8 field_3c;                 // 0x3c
    u8 field_3d;                 // 0x3d
    u8 field_3e;                 // 0x3e
    u8 field_3f;                 // 0x3f
    f32 camera_distance;         // 0x40
    i16 field_44;                // 0x44
    i16 unknown_46;              // 0x46
    i32 instance_id;             // 0x48
    i32 trailing_state_words[8]; // 0x4c
} PART_EMIT_PC;

typedef struct terrain_surface_s { // size 0xc
    f32 movement_scale;            // 0x0
    u32 flags;                     // 0x4

    union { // size 0x4
        u32 field_08;

        struct {                          // size 0x4
            u16 object_id;                // 0x8
            u16 unknown_object_high_word; // 0xa
        };
    }; // 0x8
} TERRAIN_SURFACE_PC;

struct bolt_s;
struct bolt_s;

typedef struct narrowsockexception_s { // size 0x10
    const char *level_name;            // 0x0
    i16 level_index;                   // 0x4
    i16 sock_index;                    // 0x6
    f32 start_distance;                // 0x8
    f32 end_distance;                  // 0xc
} NARROWSOCKEXCEPTION_PC;

typedef struct cutsys_s CUTSYS_PC;
typedef struct gameanimsys_s GAMEANIMSYS;

typedef struct debris_float_key_s { // size 0x8
    f32 time;                       // 0x0
    f32 value;                      // 0x4
} DEBRIS_FLOAT_KEY_PC;

typedef struct debris_colour_key_s { // size 0x8
    f32 time;                        // 0x0
    u8 red;                          // 0x4
    u8 green;                        // 0x5
    u8 blue;                         // 0x6
    u8 unknown_7;                    // 0x7
} DEBRIS_COLOUR_KEY_PC;

typedef struct debris_particle_frame_s { // size 0x28
    NUVEC position;                      // 0x00
    NUVEC texture_offset;                // 0x0c
    NUVEC extent;                        // 0x18
    u32 colour;                          // 0x24
} DEBRIS_PARTICLE_FRAME;

typedef struct SoundTable { // size 0xc8
    u16 bits[100];          // 0x00
} SoundTable;

typedef struct GAMEAUDIO {                                   // size 0x200
    i32 (*override_footstep_fn)(struct gameobject_s *, i32); // 0x000
    i32 (*check_reverb_fn)(void);                            // 0x004
    char *sfx_names[84];                                     // 0x008
    i16 sfx_ids[84];                                         // 0x158
} GAMEAUDIO;

typedef struct cutinfo_s { // size 0xf0
    u8 opaque_00_04[0x4];  // 0x00
    void *instance;        // 0x04
    char name[0x40];       // 0x08
    u8 unknown_48[0x8];    // 0x48
    u32 flags;             // 0x50
    u8 unknown_54[0x98];   // 0x54
    i16 skip_level;        // 0xec
} CUTINFO;

typedef struct gameanimset_s GAMEANIMSET;

typedef struct language_data_s { // size 0x8
    i32 language;                // 0x0
    char *localized_name;        // 0x4
} LANGUAGEDATA;

typedef struct text_entry_s { // size 0x8
    i16 *text_id;             // 0x0
    i16 value;                // 0x4
    i16 pad;                  // 0x6
} TEXTENTRY;

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

struct FADETYPE {        // size 0x4
    FADETYPE_VALUE type; // 0x0
};
#endif

typedef struct OCCLUDERSET_s { // size 0xa0
    u8 unknown_0000[0xc];      // 0x00
    u32 count;                 // 0x0c
    u8 queries_prepared;       // 0x10
    u8 unknown_0011[0x8f];     // 0x11
} OCCLUDERSET;

typedef struct OCCLUSIONMANAGER_s { // size 0x170
    u8 initialized;                 // 0x000
    u8 enabled;                     // 0x001
    u8 unknown_0002[0xe];           // 0x002
    OCCLUDERSET sets[2];            // 0x010
    OCCLUDERSET *building_set;      // 0x150
    OCCLUDERSET *current_set;       // 0x154
    f32 unknown_0158;               // 0x158
    f32 unknown_015c;               // 0x15c
    u32 unknown_0160;               // 0x160
    u32 unknown_0164;               // 0x164
    u8 unknown_0168[8];             // 0x168
} OCCLUSIONMANAGER;

typedef struct gizforce_s GIZFORCE_PC;
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct BATARANG_TARGET_s { // size 0x14
        void *object;                  // 0x00
        u8 type;                       // 0x04
        u8 lost;                       // 0x05
        u16 reserved_06_word;          // 0x06

        union { // size 0xc
            u8 reserved_08[12];
            NUVEC fallback_position;
        }; // 0x08
    } BATARANG_TARGET_s;

    struct CHARPIVOT;

    typedef struct debris_process_sphere_s { // size 0x1c
        NUVEC position;                      // 0x00
        NUVEC momentum;                      // 0x0c
        f32 time;                            // 0x18
    } DEBRIS_PROCESS_SPHERE;

    typedef struct edpp_particle_s { // size 0x58
        NUVEC position;              // 0x00
        i32 effect_index;            // 0x0c
        i32 instance_id;             // 0x10
        i16 rotation_z;              // 0x14
        i16 rotation_y;              // 0x16
        i16 emitter_rotation_z;      // 0x18
        i16 emitter_rotation_y;      // 0x1a
        i16 emitter_rotation_x;      // 0x1c
        u8 unknown_01e[2];           // 0x1e
        f32 start_offset;            // 0x20
        char name[16];               // 0x24
        i32 switch_type;             // 0x34
        i32 switch_id;               // 0x38
        f32 switch_variable;         // 0x3c
        i16 reflection_rotation_z;   // 0x40
        i16 reflection_rotation_y;   // 0x42
        f32 reflection_offset;       // 0x44
        f32 reflection_bounce;       // 0x48
        i16 render_group;            // 0x4c
        u16 render_priority;         // 0x4e
        i8 dynamic_priority;         // 0x50
        i8 page;                     // 0x51
        i8 detail_levels;            // 0x52
        i8 facing_mode;              // 0x53
        i16 facing_rotation_x;       // 0x54
        i16 facing_rotation_y;       // 0x56
    } EDPP_PARTICLE_PC;

    struct GIZTURRET_s;

    typedef struct ripple_set_s RIPPLE_SET_PC;
#ifdef __cplusplus
}
#endif

struct CHARPIVOT {         // size 0x14
    const char *name;      // 0x00
    i16 animation;         // 0x04
    i16 point_of_interest; // 0x06
    f32 start_frame;       // 0x08
    f32 end_frame;         // 0x0c
    u16 character_id;      // 0x10
    u8 unknown_12[2];      // 0x12
};

typedef struct CHARPIVOT CHARPIVOT;
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

struct AREASAVE_s {           // size 0xc
    u8 complete;              // 0x0
    u8 area_complete;         // 0x1
    u8 true_hero_complete[2]; // 0x2
    u8 minikit_complete;      // 0x4

    union { // size 0x3
        u8 field_0x5[3];

        struct {                    // size 0x3
            u8 minikit_count;       // 0x5
            u8 red_brick_collected; // 0x6
            u8 reserved_0x7;        // 0x7
        };
    }; // 0x5

    f32 challenge_trial_time; // 0x8
};

struct STATUSPACKET_LSW_s { // size 0x4
    i32 field_0x00;         // 0x0
};

struct GIZFORCEANIMDATA_s { // size 0x8
    u16 flags;              // 0x0
    i16 platform_id;        // 0x2

    union { // size 0x4
        u32 field_0x04;

        struct {                       // size 0x4
            u32 force_glow_active : 1; // 0x4:0
            u32 : 31;                  // 0x4:1
        };
    }; // 0x4
};

struct TRAFFICANIMINSTANCE_s {       // size 0x14
    NULISTLNK link;                  // 0x00
    struct TRAFFICANIM_s *animation; // 0x08
    f32 frame;                       // 0x0c
    i8 vehicle_index;                // 0x10
    u8 reserved_11[3];               // 0x11
};

struct SNIPER_s {   // size 0x20
    NUVEC prev;     // 0x00
    NUVEC pos;      // 0x0c
    f32 fire_timer; // 0x18
    f32 state;      // 0x1c
};

struct PODSPRINTNETPACKET_s { // size 0xa
    i16 ai_state;             // 0x0
    char pad_0x02[4];         // 0x2
    i16 speed;                // 0x6
    i16 speed2;               // 0x8
};

struct minisnowtrooper_s { // size 0x20
    u8 formation_index;    // 0x00

    union { // size 0x1
        u8 state_flags;

        struct {               // size 0x1
            u8 state_low : 2;  // 0x01:0
            u8 step_frame : 2; // 0x01:2
            u8 slot : 2;       // 0x01:4
            u8 state_high : 2; // 0x01:6
        };
    }; // 0x01

    u8 reserved_002;     // 0x02
    u8 speed_divisor;    // 0x03
    u16 rotation;        // 0x04
    u16 target_rotation; // 0x06
    f32 timer;           // 0x08
    f32 formation_x;     // 0x0c
    f32 formation_z;     // 0x10
    NUVEC shot_position; // 0x14
};

struct edcam_s {              // size 0x94
    NUVEC position;           // 0x00
    i32 pitch;                // 0x0c
    i32 yaw;                  // 0x10
    f32 distance;             // 0x14
    NUVEC offset;             // 0x18
    NUVEC snap_origin;        // 0x24
    NUVEC snap_step;          // 0x30
    i32 pitch_snap_step;      // 0x3c
    i32 yaw_snap_step;        // 0x40
    NUVEC snapped_position;   // 0x44
    i32 snapped_pitch;        // 0x50
    i32 snapped_yaw;          // 0x54
    NUVEC position_speed;     // 0x58
    i32 pad_pitch_speed;      // 0x64
    i32 pad_yaw_speed;        // 0x68
    f32 distance_speed;       // 0x6c
    f32 minimum_distance;     // 0x70
    f32 mouse_pitch_speed;    // 0x74
    f32 mouse_yaw_speed;      // 0x78
    f32 mouse_move_speed;     // 0x7c
    f32 auto_move_base;       // 0x80
    f32 auto_move_dist_scale; // 0x84
    f32 auto_zoom_base;       // 0x88
    f32 auto_zoom_dist_scale; // 0x8c

    union { // size 0x4
        u32 freedoms;

        struct {                      // size 0x4
            u32 allow_position_x : 1; // 0x90:0
            u32 allow_position_y : 1; // 0x90:1
            u32 allow_position_z : 1; // 0x90:2
            u32 allow_pitch : 1;      // 0x90:3
            u32 allow_yaw : 1;        // 0x90:4
            u32 allow_distance : 1;   // 0x90:5
            u32 invert_pad_pitch : 1; // 0x90:6
            u32 reserved_freedom : 1; // 0x90:7
        };
    }; // 0x90
};

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
class NetLogFile { // size 0x2114
  public:
    NetLogFile();
    ~NetLogFile();
    i32 Flush();

    i32 state;          // 0x0000
    char name[0x100];   // 0x0004
    i32 mode;           // 0x0104
    u32 unknown_108[2]; // 0x0108
    i32 count;          // 0x0110
    u8 buffer[0x2000];  // 0x0114
};
#endif
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 2 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)

typedef struct TERRAIN_PLATFORM_PC { // size 0x6c
    NUMTX previous_matrix;           // 0x00
    void *scene_object;              // 0x40
    u32 unknown_44;                  // 0x44
    i16 terrain_group_index;         // 0x48
    i16 scene_object_index;          // 0x4a

    union { // size 0x4
        u32 flags;

        struct {              // size 0x4
            u32 rotating : 1; // 0x4c:0
            u32 : 31;         // 0x4c:1
        };
    }; // 0x4c

    u8 unknown_50[4];    // 0x50
    f32 bounce_54;       // 0x54
    f32 bounce_58;       // 0x58
    f32 bounce_5c;       // 0x5c
    f32 bounce_60;       // 0x60
    f32 bounce_64;       // 0x64
    u32 *instance_flags; // 0x68
} TERRAIN_PLATFORM_PC;

typedef TERRAIN_PLATFORM_PC TERRAIN_PLATFORM;

typedef struct gamecamera_s { // size 0x230

    union { // size 0x38
        SOCKPOSITION_PC sock_position;

        struct {           // size 0x2
            u8 unknown_00; // 0x000
            u8 sock;       // 0x001
        };
    }; // 0x000

    NUMTX mtx;                        // 0x038
    NUMTX target_mtx;                 // 0x078
    NUMTX render_mtx;                 // 0x0b8
    NUVEC shaken_right;               // 0x0f8
    NUVEC shaken_up;                  // 0x104
    NUVEC dir;                        // 0x110
    NUVEC position;                   // 0x11c
    NUVEC target;                     // 0x128
    NUVEC blend_start_position;       // 0x134
    NUVEC desired_position;           // 0x140
    NUVEC blend_end_position;         // 0x14c
    NUVEC blend_destination_position; // 0x158
    NUVEC blend_start_target;         // 0x164
    u8 unknown_170[0xc];              // 0x170
    NUVEC blend_end_target;           // 0x17c
    NUVEC blend_destination_target;   // 0x188
    NUVEC shake_offset;               // 0x194
    NUVEC shake_direction;            // 0x1a0
    NUVEC shake_target;               // 0x1ac
    i32 unknown_1b8;                  // 0x1b8
    f32 blend_time;                   // 0x1bc
    f32 blend_duration;               // 0x1c0
    f32 blend_curve;                  // 0x1c4
    f32 judder_time;                  // 0x1c8
    f32 judder_duration;              // 0x1cc
    f32 shake_amplitude;              // 0x1d0
    f32 shake_intensity;              // 0x1d4
    f32 shake_decay;                  // 0x1d8
    f32 shake_freq;                   // 0x1dc
    f32 position_seek;                // 0x1e0
    f32 angle_seek;                   // 0x1e4
    f32 unknown_1e8;                  // 0x1e8
    f32 unknown_1ec;                  // 0x1ec
    f32 unknown_1f0;                  // 0x1f0
    f32 unknown_1f4;                  // 0x1f4
    i32 pitch;                        // 0x1f8

    union { // size 0x4
        i32 yaw;
        i32 input_yaw;
    }; // 0x1fc

    i32 roll;                // 0x200
    f32 field_204;           // 0x204
    f32 field_208;           // 0x208
    f32 field_20c;           // 0x20c
    f32 field_210;           // 0x210
    f32 field_214;           // 0x214
    f32 adjusted_yaw_offset; // 0x218
    u16 blend_start_pitch;   // 0x21c
    u16 desired_pitch;       // 0x21e
    u16 blend_start_yaw;     // 0x220
    u16 desired_yaw;         // 0x222
    u16 blend_start_roll;    // 0x224
    u16 desired_roll;        // 0x226
    u8 judder_reverse;       // 0x228
    u8 judder_axis;          // 0x229
    u8 reset_blend;          // 0x22a
    u8 blend_mode;           // 0x22b
    i8 mode;                 // 0x22c
    i8 previous_mode;        // 0x22d
    u8 previous_camera_mode; // 0x22e
} GAMECAMERA;

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

struct AITRIGGERSET_TARGET { // size 0x3c
    u8 reserved_00[16];      // 0x00
    NUVEC position;          // 0x10
    u8 reserved_1c[4];       // 0x1c
    AIPATHINFO path;         // 0x20
    u8 reserved_38[4];       // 0x38
};

struct GIZSPINNERARM_s { // size 0x44
    NUMTX matrix;        // 0x00
    i16 platform_id;     // 0x40
    u8 field_0x42[2];    // 0x42
};

struct GIZPANEL_s {   // size 0x9c
    NUMTX matrix;     // 0x00
    char name[16];    // 0x40
    NUVEC position;   // 0x50
    u16 y_rotation;   // 0x5c
    u8 field_0x5e[3]; // 0x5e
    u8 model_variant; // 0x61
    u8 field_0x62[2]; // 0x62
    f32 flash_timer;  // 0x64

    union { // size 0x1
        u8 flags;

        struct {                  // size 0x1
            u8 reserved_flag : 1; // 0x68:0
            u8 state : 1;         // 0x68:1
            u8 visible : 1;       // 0x68:2
            u8 track_player : 1;  // 0x68:3
            u8 player_near : 1;   // 0x68:4
            u8 hide_base : 1;     // 0x68:5
            u8 hide_target : 1;   // 0x68:6
            u8 baddie : 1;        // 0x68:7
        };
    }; // 0x68

    u8 draw_flags;                                     // 0x69
    u8 field_0x6a[2];                                  // 0x6a
    NUVEC floor_position;                              // 0x6c
    NUVEC target_offset;                               // 0x78
    u16 target_x_rotation;                             // 0x84
    u16 target_y_rotation;                             // 0x86
    u16 arm_x_rotation;                                // 0x88
    u16 target_pitch;                                  // 0x8a
    u16 target_roll;                                   // 0x8c
    i16 platform_id;                                   // 0x8e
    f32 activation_time;                               // 0x90
    f32 target_scale;                                  // 0x94
    struct MechObjectInterface *mech_object_interface; // 0x98
};

struct GIZBUILDITANIMDATA_s { // size 0xc8
    NUMTX start_mtx;          // 0x00
    NUMTX end_mtx;            // 0x40
    NUMTX draw_mtx;           // 0x80
    f32 wobble_time;          // 0xc0
    u8 wobble_axis;           // 0xc4
    u8 pad_c5;                // 0xc5
    u8 was_drawn;             // 0xc6
    u8 pad_c7;                // 0xc7
};

#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 3 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)

typedef TERRAIN_SET_PC TERRSET;

struct TERRAIN_SPATIAL_NODE { // size 0x10
    u16 point_count;          // 0x0
    u16 field_02;             // 0x2
    NUVEC points[1];          // 0x4
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

typedef struct levelspline_s { // size 0x10
    struct NUGSCN_s **scene;   // 0x0
    const char *name;          // 0x4
    u16 min_points;            // 0x8
    u16 max_points;            // 0xa
    i16 level;                 // 0xc
    i16 area;                  // 0xe
} LEVELSPLINE_PC;

typedef struct nuinstanim_s { // size 0x60
    NUMTX matrix;             // 0x00
    f32 tfactor;              // 0x40
    f32 first_frame;          // 0x44
    f32 interval;             // 0x48
    f32 ltime;                // 0x4c
    u32 flags;                // 0x50
    f32 fparam1;              // 0x54
    f32 prev_eval_time;       // 0x58
    u16 animation_index;      // 0x5c
    u16 instance_index;       // 0x5e
} NUINSTANIM;

#ifdef __cplusplus

struct OccluderSet { // size 0xa0
    OccluderSet();
    ~OccluderSet();
    static struct numtl_s *ms_pZOnlyMtl3D;
    static struct numtl_s *ms_pZOnlyMtl2D;
    static struct numtl_s *ms_pAlphaMtl2D;
    void Init(u32, VARIPTR *, VARIPTR);
    PC_OCCLUDER_RECORD *occluders; // 0x00
    u32 *indices;                  // 0x04
    u32 capacity;                  // 0x08
    u32 count;                     // 0x0c
    bool queries_prepared;         // 0x10
    u8 unknown_11[15];             // 0x11
    NUMTX projection_matrix;       // 0x20
    NUMTX query_matrix;            // 0x60
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

struct OcclusionManager { // size 0x170
    OcclusionManager();
    ~OcclusionManager();
    void Init(u32, VARIPTR *, VARIPTR);
    bool initialized;          // 0x000
    bool enabled;              // 0x001
    u8 unknown_02[14];         // 0x002
    OccluderSet sets[2];       // 0x010
    OccluderSet *building_set; // 0x150
    OccluderSet *current_set;  // 0x154
    f32 unknown_158;           // 0x158
    f32 unknown_15c;           // 0x15c
    u32 occluded_count;        // 0x160
    u32 query_count;           // 0x164
    u8 unknown_168[8];         // 0x168
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

struct AITRIGGERSET_s { // size 0x214

    union { // size 0x204
        u8 pad_000[516];

        struct { // size 0x204

            union { // size 0x4
                u32 reserved_000;
                struct FLOWBOX *flowbox;
            }; // 0x000

            union { // size 0x20
                struct gizmo_s *triggers[8];
                struct gizmo_s *gizmos[8];
            }; // 0x004

            union { // size 0x1e0
                struct AITRIGGERSET_TARGET targets[8];
                AILOCATOR locators[8];
            }; // 0x024
        };
    }; // 0x000

    i8 trigger_indices[8]; // 0x204

    union { // size 0x8
        u8 unknown_20c[0x2];
        u16 field_20e;
        u8 unknown_210[0x4];

        struct {              // size 0x8
            u8 trigger_count; // 0x20c
            u8 field_20d;     // 0x20d
            u16 field_0x20e;  // 0x20e

            union { // size 0x2
                u16 flags_word;

                struct {             // size 0x2
                    u8 flags;        // 0x210
                    u8 reserved_211; // 0x211
                };
            }; // 0x210

            u8 reserved_212[2]; // 0x212
        };
    }; // 0x20c
};

struct AITRIGGERSETSYS_s {          // size 0x4304
    struct AITRIGGERSET_s sets[32]; // 0x0000
    i8 unknown_4280[64];            // 0x4280
    i8 unknown_42c0[64];            // 0x42c0
    i32 field_0x4300;               // 0x4300
};

struct shopitem_s {              // size 0x74
    char name[64];               // 0x00
    char special_name[32];       // 0x40
    i16 item_id;                 // 0x60
    u8 type;                     // 0x62
    u8 unlocked;                 // 0x63
    i32 price;                   // 0x64
    struct nuhspecial_s special; // 0x68
};

struct AIPATHCNXCONTROLLER_s {    // size 0xa8
    NULISTLNK links;              // 0x00
    struct AIPATHCNX *connection; // 0x08

    union { // size 0xc
        void *target;
        NUSPECIALHANDLE special;
        i32 fake_animation_id;
    }; // 0x0c

    i32 gizmo_output;  // 0x18
    u32 on_flags;      // 0x1c
    u32 off_flags;     // 0x20
    u32 on_frames[32]; // 0x24
    u8 target_type;    // 0xa4
    u8 path_index;     // 0xa5

    union { // size 0x2
        u16 flags_word;

        struct {       // size 0x2
            u8 flags;  // 0xa6
            u8 pad_a7; // 0xa7
        };
    }; // 0xa6
};

struct TRAFFICANIM_s {           // size 0x140
    struct nuhspecial_s special; // 0x000
    void *animation;             // 0x00c
    f32 end_frame;               // 0x010
    f32 tfactor;                 // 0x014
    f32 frame_interval;          // 0x018
    f32 random_interval;         // 0x01c
    f32 room_frame_interval;     // 0x020
    f32 y_offset;                // 0x024
    i8 vehicle_indices[16];      // 0x028
    u8 rooms[256];               // 0x038
    i8 vehicle_count;            // 0x138
    u8 disabled;                 // 0x139
    u8 reserved_13a[2];          // 0x13a
    f32 next_spawn_time;         // 0x13c
};

struct LevelEditorScene {    // size 0xa8
    char name[32];           // 0x00
    char directory[128];     // 0x20
    struct NUGSCN_s *scene;  // 0xa0
    i32 active : 1;          // 0xa4:0
    i32 editable : 1;        // 0xa4:1
    i32 saved : 1;           // 0xa4:2
    u32 reserved_flags : 29; // 0xa4:3
};

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

struct FADEINFO_s {              // size 0x2c
    u32 direction;               // 0x00
    f32 fade;                    // 0x04
    f32 rate;                    // 0x08
    i32 busy;                    // 0x0c
    i32 stage;                   // 0x10
    FadeBase *fades[4];          // 0x14
    FADETYPE_VALUE pending_type; // 0x24
    i32 field_28;                // 0x28
};

class FadeSystem : public FADEINFO_s { // size 0x2c
  public:
    void Init();
    void Update();
    void Draw();
    void SetStage(char stage);
    i32 SetFade(FADETYPE const &type, u32 frames);
    i32 AddFade(FadeBase *fade);
};

#endif
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 6
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 6 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)

typedef struct BOLTSYS_PC_s {                                    // size 0x20
    struct BOLTTYPE_PC_s *types;                                 // 0x00
    i32 count;                                                   // 0x04
    void (*stop_targeting)(struct gameobject_s *, NUVEC *);      // 0x08
    void (*debris)(struct bolt_s *, NUVEC *, i32, NUVEC *, i32); // 0x0c
    void (*shoot_origin)(struct gameobject_s *, NUVEC *);        // 0x10
    u16 (*shoot_direction)(struct gameobject_s *, NUVEC *);      // 0x14
    i32 (*hit_part)(struct bolt_s *, struct PART_PC *);          // 0x18
    i32 (*alternate_fire)(struct gameobject_s *, i32);           // 0x1c
} BOLTSYS_PC;

typedef struct bolt_s {         // size 0x138
    struct BOLTTYPE_PC_s *type; // 0x000
    void *owner;                // 0x004
    NUMTX orientation;          // 0x008
    NUMTX effect_orientation;   // 0x048
    NUVEC position;             // 0x088
    NUVEC previous_position;    // 0x094
    NUVEC velocity;             // 0x0a0
    NUVEC direction;            // 0x0ac
    NUVEC ray_end;              // 0x0b8
    f32 scale;                  // 0x0c4
    f32 time;                   // 0x0c8
    f32 speed;                  // 0x0cc
    f32 lifetime;               // 0x0d0
    f32 radius;                 // 0x0d4
    f32 collision_radius;       // 0x0d8
    f32 ray_time;               // 0x0dc
    f32 ray_radius;             // 0x0e0
    f32 field_e4;               // 0x0e4
    f32 reflection_height;      // 0x0e8
    f32 acceleration_y;         // 0x0ec
    u32 flags;                  // 0x0f0
    u16 surface_x_rotation;     // 0x0f4
    u16 surface_z_rotation;     // 0x0f6
    u16 hit_flags;              // 0x0f8

    union { // size 0x2
        i16 hit_platform;
        u16 unknown_fa;
    }; // 0x0fa

    i16 hit_debris;              // 0x0fc
    i16 hit_part_debris;         // 0x0fe
    u8 active;                   // 0x100
    u8 type_id;                  // 0x101
    u8 field_102;                // 0x102
    u8 index;                    // 0x103
    i8 field_104;                // 0x104
    i8 unknown_105;              // 0x105
    u16 unknown_106;             // 0x106
    NUVEC bounds_min;            // 0x108
    NUVEC bounds_max;            // 0x114
    NUVEC hit_normal;            // 0x120
    NUVEC dogfight_hit_position; // 0x12c
} BOLT;

// Access prefixes only: the trailing name and complete original sizes are unknown.
struct cutsys_s {      // size 0x8
    CUTINFO **records; // 0x0
    i32 count;         // 0x4
};

typedef struct gameanimobjpool_s GAMEANIMOBJPOOL;
typedef void (*PICKUP_UPDATE_FN)(struct WORLDINFO_s *, struct GIZMOPICKUP *);
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

struct LEVER_s {                             // size 0xa8
    NUMTX transform;                         // 0x00
    struct MechObjectInterface *mech_object; // 0x40
    NUVEC floor_position;                    // 0x44
    NUVEC target_offset;                     // 0x50
    char name[16];                           // 0x5c
    NUVEC position;                          // 0x6c
    f32 idle_animation_timer;                // 0x78
    f32 pull_progress;                       // 0x7c
    f32 auto_reset_timer;                    // 0x80
    f32 flash_timer;                         // 0x84
    f32 auto_reset_delay;                    // 0x88
    f32 target_indicator_scale;              // 0x8c
    u16 y_rotation;                          // 0x90
    i16 model_special_index;                 // 0x92
    i16 platform_id;                         // 0x94
    u16 animation_frame;                     // 0x96
    u16 target_x_rotation;                   // 0x98
    u16 target_z_rotation;                   // 0x9a

    union { // size 0x2
        u16 flags;

        struct {                 // size 0x2
            u8 interacting : 1;  // 0x9c:0
            u8 being_pulled : 1; // 0x9c:1
            u8 goodie : 1;       // 0x9c:2
            u8 baddie : 1;       // 0x9c:3
            u8 visible : 1;      // 0x9c:4
            u8 auto_reset : 1;   // 0x9c:5
            u8 returning : 1;    // 0x9c:6
            u8 enabled : 1;      // 0x9c:7
            u8 flags_high;       // 0x9d
        };
    }; // 0x9c

    char model_variant; // 0x9e
    u8 unknown_9f[9];   // 0x9f
};

struct GIZPANELSYS_s {         // size 0x8
    i32 count;                 // 0x0
    struct GIZPANEL_s *panels; // 0x4
};

struct TRAFFICANIMSYS_s {                        // size 0x77e4
    struct TRAFFICANIM_s animations[64];         // 0x0000
    struct nuhspecial_s vehicles[16];            // 0x5000
    struct TRAFFICANIMINSTANCE_s instances[500]; // 0x50c0
    NULISTHDR free_instances;                    // 0x77d0
    NULISTHDR active_instances;                  // 0x77d8
    i8 animation_count;                          // 0x77e0
    i8 vehicle_count;                            // 0x77e1
    u8 reserved_77e2[2];                         // 0x77e2
};

struct GIZMOPICKUPRUNTIMESYS_s {           // size 0x1c
    struct GIZMOPICKUP *pickups;           // 0x00
    struct GIZMOPICKUP *temporary_pickups; // 0x04
    i32 pickup_count;                      // 0x08
    i32 temporary_cursor;                  // 0x0c
    i32 total_score;                       // 0x10
    f32 draw_distance;                     // 0x14
    f32 pickup_scale;                      // 0x18
};

struct SPECIALMINIKITSYS_s {        // size 0x8
    struct SPECIALMINIKIT_s *items; // 0x0
    i32 count;                      // 0x4
};

struct SPECIALMINIKIT_s {          // size 0x3c
    struct nuhspecial_s special;   // 0x00
    void *inst_anim;               // 0x0c
    void *anim_data;               // 0x10
    f32 end_frame;                 // 0x14
    char special_name[16];         // 0x18
    char pickup_name[8];           // 0x28
    struct gizmo_s *special_gizmo; // 0x30
    struct gizmo_s *pickup_gizmo;  // 0x34
    u8 flags;                      // 0x38
};

struct FADER_s {                         // size 0x18
    struct nuhspecial_s special;         // 0x00
    struct nuhspecial_s while_animating; // 0x0c
};

struct minitrooperteam_s { // size 0x54
    f32 origin_x;          // 0x00
    f32 origin_z;          // 0x04
    f32 height;            // 0x08
    f32 formation_width;   // 0x0c
    f32 formation_depth;   // 0x10
    u16 facing_angle;      // 0x14
    u8 reserved_016[2];    // 0x16

    union { // size 0x4
        u32 packed_state;

        struct {                      // size 0x4
            u32 formation_shape : 4;  // 0x18:0
            u32 formation_mode : 4;   // 0x18:4
            u32 previous_mode : 4;    // 0x18:8
            u32 mode_changed : 1;     // 0x18:12
            u32 route_index : 3;      // 0x18:13
            u32 waypoint_current : 3; // 0x18:16
            u32 waypoint_index : 3;   // 0x18:19
            u32 waypoint_count : 3;   // 0x18:22
            u32 team_side : 1;        // 0x18:25
            u32 team_bits : 6;        // 0x18:26
        };

        struct {                // size 0x4
            u8 formation_state; // 0x18
            u8 route_state;     // 0x19
            u8 waypoint_state;  // 0x1a
            u8 team_flags;      // 0x1b
        };
    }; // 0x18

    u8 trooper_count;                   // 0x1c
    u8 reserved_01d[3];                 // 0x1d
    u32 state_flags;                    // 0x20
    f32 state_timer;                    // 0x24
    f32 debris_timer;                   // 0x28
    f32 fire_timer;                     // 0x2c
    f32 reserved_02c;                   // 0x30
    i32 bolt_type;                      // 0x34
    struct nuvec_s *route_point;        // 0x38
    struct nugspline_s *path;           // 0x3c
    struct minisnowtrooper_s *troopers; // 0x40
    NUVEC position;                     // 0x44
    i32 target_index;                   // 0x50
};

#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 7
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 7 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)

typedef struct gameanimobj_s GAMEANIMOBJ;

struct gameanimobj_s {              // size 0x28
    GAMEANIMOBJ *next;              // 0x00
    NUSPECIALHANDLE special;        // 0x04
    NUINSTANIM *instance_animation; // 0x10
    void *animation;                // 0x14
    f32 start_frame;                // 0x18
    f32 end_frame;                  // 0x1c

    union { // size 0x4

        struct {               // size 0x4
            u8 flags;          // 0x20
            u8 reserved_21[3]; // 0x21
        };

        u32 serialized_flags;
    }; // 0x20

    void *object_data; // 0x24
};

struct gameanimobjpool_s {     // size 0x14
    u16 object_data_size;      // 0x00
    u16 active_count;          // 0x02
    u16 capacity;              // 0x04
    u16 unknown_06;            // 0x06
    GAMEANIMOBJ *objects;      // 0x08
    GAMEANIMOBJ *free_objects; // 0x0c
    void *object_data;         // 0x10
};

struct gameanimset_s {            // size 0x1c
    NULISTLNK links;              // 0x00
    u8 object_count;              // 0x08
    u8 animated_object_count;     // 0x09
    u16 flags;                    // 0x0a
    i32 state;                    // 0x0c
    GAMEANIMOBJPOOL *object_pool; // 0x10
    GAMEANIMSYS *system;          // 0x14
    GAMEANIMOBJ *objects;         // 0x18
};

struct gameanimsys_s {     // size 0x10
    NULISTHDR active_sets; // 0x0
    i32 set_count;         // 0x8
    GAMEANIMSET **sets;    // 0xc
};

typedef struct gizobstacle_s {              // size 0xa0
    char name[16];                          // 0x00
    NUVEC position;                         // 0x10
    NUVEC secondary_position;               // 0x1c
    NUVEC evaluated_position;               // 0x28
    GAMEANIMSET *anim_set;                  // 0x34
    struct gameobject_s *triggering_object; // 0x38
    f32 auto_return_delay;                  // 0x3c
    f32 auto_return_timer;                  // 0x40
    f32 trigger_radius;                     // 0x44
    f32 animation_speed;                    // 0x48
    f32 forward_speed_factor;               // 0x4c
    f32 backward_speed_factor;              // 0x50
    f32 reflection_alpha;                   // 0x54
    f32 evaluated_radius;                   // 0x58
    NUVEC trigger_box_half_extents;         // 0x5c
    u32 config_flags;                       // 0x68
    u32 exclude_flags;                      // 0x6c
    NUVEC pickup_offset;                    // 0x70
    f32 pickup_scatter_height;              // 0x7c
    i16 pickup_direction_x;                 // 0x80
    i16 pickup_direction_y;                 // 0x82
    i16 trigger_box_yaw;                    // 0x84
    i16 room_id;                            // 0x86
    i16 blowup_type;                        // 0x88
    i16 pickup_count;                       // 0x8a
    i16 start_sfx_id;                       // 0x8c
    i16 stop_sfx_id;                        // 0x8e
    u8 playback_state;                      // 0x90
    u8 mode;                                // 0x91
    u8 trigger_mode;                        // 0x92
    u8 unknown_093;                         // 0x93
    u32 progress_flags;                     // 0x94
    i32 proximity_output;                   // 0x98
    u32 runtime_flags;                      // 0x9c
} GIZOBSTACLE;

typedef void (*PICKUP_COLLECT_FN)(struct WORLDINFO_s *, struct GIZMOPICKUP *, i32, struct gameobject_s *, i32);

typedef struct GIZMO_PICKUP_TYPE { // size 0x38
    char *name;                    // 0x00
    char *activation_sfx_name;     // 0x04
    char *collection_sfx_name;     // 0x08
    u8 field_c;                    // 0x0c
    u8 random_model_count;         // 0x0d
    u8 flags;                      // 0x0e
    u8 field_0f;                   // 0x0f
    i16 first_model_id;            // 0x10
    u16 score;                     // 0x12
    f32 shadow_radius_x;           // 0x14
    f32 shadow_radius_z;           // 0x18
    f32 draw_distance;             // 0x1c
    f32 bob_height;                // 0x20
    PICKUP_UPDATE_FN update_fn;    // 0x24
    PICKUP_COLLECT_FN collect_fn;  // 0x28
    i16 debris_id;                 // 0x2c
    i16 overlay_model_id;          // 0x2e
    f32 shadow_extent_x;           // 0x30
    f32 shadow_extent_z;           // 0x34
} GIZMO_PICKUP_TYPE;

struct gizforce_s {                    // size 0xa4
    char name[16];                     // 0x00
    NUVEC file_position;               // 0x10
    NUVEC position;                    // 0x1c
    GAMEANIMSET *anim_set;             // 0x28
    NUSPECIALHANDLE along_socket;      // 0x2c
    struct gameobject_s *using_object; // 0x38
    u32 reset_word;                    // 0x3c
    struct FORCE_RESET_GROUP *group;   // 0x40
    f32 force_strength;                // 0x44
    f32 field_48;                      // 0x48
    f32 force_range;                   // 0x4c
    f32 field_50;                      // 0x50
    f32 interaction_radius;            // 0x54
    f32 animation_speed;               // 0x58
    f32 start_frame;                   // 0x5c
    f32 end_frame;                     // 0x60
    f32 effect_scale;                  // 0x64
    f32 radius;                        // 0x68
    f32 vertical_range;                // 0x6c
    f32 horizontal_range;              // 0x70
    u32 config_flags;                  // 0x74
    u8 unknown_078[4];                 // 0x78
    u8 group_index;                    // 0x7c
    u8 collision_mask;                 // 0x7d
    u8 activation_mode;                // 0x7e
    u8 unknown_07f;                    // 0x7f
    i16 room_id;                       // 0x80
    i16 start_sfx_id;                  // 0x82
    i16 loop_sfx_id;                   // 0x84
    i16 stop_sfx_id;                   // 0x86
    i16 blowup_type;                   // 0x88
    i16 pickup_count;                  // 0x8a
    i16 pickup_rotation_x;             // 0x8c
    i16 pickup_rotation_y;             // 0x8e
    NUVEC effect_position;             // 0x90
    f32 activation_radius;             // 0x9c

    union { // size 0x4
        u32 runtime_flags;

        struct {                         // size 0x4
            u32 enabled : 1;             // 0xa0:0
            u32 visible : 1;             // 0xa0:1
            u32 runtime_bits_2_8 : 7;    // 0xa0:2
            u32 reward_released : 1;     // 0xa0:9
            u32 runtime_bit_10 : 1;      // 0xa0:10
            u32 socket_hidden : 1;       // 0xa0:11
            u32 runtime_bits_12_13 : 2;  // 0xa0:12
            u32 completion_released : 1; // 0xa0:14
            u32 completion_pending : 1;  // 0xa0:15
            u32 destroyed : 1;           // 0xa0:16
            u32 runtime_bits_17_20 : 4;  // 0xa0:17
            u32 being_used : 1;          // 0xa0:21
            u32 runtime_bits_22_31 : 10; // 0xa0:22
        };
    }; // 0xa0
};
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct EXPLOSION_PC {   // size 0x38
        i32 field_00;               // 0x00
        i32 field_04;               // 0x04
        struct gameobject_s *owner; // 0x08
        NUVEC position;             // 0x0c
        f32 radius;                 // 0x18
        f32 time;                   // 0x1c
        f32 duration;               // 0x20
        u32 flags;                  // 0x24
        u8 unknown_28[2];           // 0x28
        u16 random_angle;           // 0x2a
        i16 effect_id;              // 0x2c
        u16 hit_flags;              // 0x2e
        u16 field_30;               // 0x30
        u8 field_32;                // 0x32
        i8 field_33;                // 0x33
        u8 unknown_34[4];           // 0x34
    } EXPLOSION_PC;

#ifdef __cplusplus
}
#endif

struct GIZFORCE_s {                    // size 0xac
    char name[16];                     // 0x00
    NUVEC file_position;               // 0x10
    NUVEC position;                    // 0x1c
    struct gameanimset_s *anim_set;    // 0x28
    struct nuhspecial_s along_socket;  // 0x2c
    struct gameobject_s *using_object; // 0x38

    union { // size 0x4
        f32 field_0x3c;
        u32 field_0x3c_bits;
    }; // 0x3c
    struct GIZFORCEGROUP_s *group; // 0x40
    f32 force_strength;            // 0x44
    f32 field_0x48;                // 0x48
    f32 force_range;               // 0x4c
    f32 field_0x50;                // 0x50
    f32 interaction_radius;        // 0x54
    f32 animation_speed;           // 0x58
    f32 start_frame;               // 0x5c
    f32 end_frame;                 // 0x60
    f32 effect_scale;              // 0x64
    f32 radius;                    // 0x68
    f32 strength_0x6c;             // 0x6c
    f32 vertical_range;            // 0x70
    f32 horizontal_range;          // 0x74
    u32 config_flags;              // 0x78
    u32 field_0x7c;                // 0x7c
    u8 group_index;                // 0x80
    u8 collision_mask;             // 0x81
    u8 activation_mode;            // 0x82
    u8 field_0x83;                 // 0x83
    i16 room_id;                   // 0x84
    i16 start_sfx_id;              // 0x86
    i16 loop_sfx_id;               // 0x88
    i16 stop_sfx_id;               // 0x8a
    i16 blowup_type;               // 0x8c

    union { // size 0x2
        i16 debris_type;
        u16 pickup_count;
        u16 completion_score;
    }; // 0x8e

    union { // size 0x2
        i16 hit_points;
        u16 pickup_direction_x;
        u16 pickup_rotation_x;
    }; // 0x90

    union { // size 0x2
        i16 score;
        u16 pickup_direction_y;
        u16 pickup_rotation_y;
    }; // 0x92

    union { // size 0xc
        NUVEC effect_position;
        NUVEC pickup_offset;
    }; // 0x94

    union { // size 0x4
        f32 activation_radius;
        f32 pickup_scatter_height;
    }; // 0xa0

    union { // size 0x4
        u32 field_0xa4;
        struct MechObjectInterface *mech_object_interface;
    }; // 0xa4

    union { // size 0x4

        struct { // size 0x4

            union { // size 0x1
                u8 progress_flags;

                struct {                                // size 0x1
                    u8 progress_enabled : 1;            // 0xa8:0
                    u8 progress_visible : 1;            // 0xa8:1
                    u8 : 2;                             // 0xa8:2
                    u8 progress_reverse_active : 1;     // 0xa8:4
                    u8 progress_group_member : 1;       // 0xa8:5
                    u8 progress_animation_reversed : 1; // 0xa8:6
                    u8 progress_draw_active : 1;        // 0xa8:7
                };
            }; // 0xa8

            union { // size 0x1
                u8 runtime_flags;

                struct {                                 // size 0x1
                    u8 runtime_has_platform : 1;         // 0xa9:0
                    u8 runtime_reward_released : 1;      // 0xa9:1
                    u8 runtime_pending_blowup_type : 1;  // 0xa9:2
                    u8 runtime_along_socket_hidden : 1;  // 0xa9:3
                    u8 runtime_offset_applied : 1;       // 0xa9:4
                    u8 runtime_force_range_complete : 1; // 0xa9:5
                    u8 runtime_completion_released : 1;  // 0xa9:6
                    u8 runtime_pending_completion : 1;   // 0xa9:7
                };
            }; // 0xa9

            union { // size 0x1
                u8 field_0xaa;
                u8 state_flags;

                struct {                              // size 0x1
                    u8 state_destroyed_or_thrown : 1; // 0xaa:0
                    u8 state_debris_active : 1;       // 0xaa:1
                    u8 : 3;                           // 0xaa:2
                    u8 state_being_used : 1;          // 0xaa:5
                    u8 : 2;                           // 0xaa:6
                };
            }; // 0xaa

            u8 field_0xab; // 0xab
        };

        u32 packed_state;
    }; // 0xa8
};

struct DETONATOR_s {             // size 0x38
    NUVEC position;              // 0x00
    NUVEC field_0x0c;            // 0x0c
    NUVEC field_0x18;            // 0x18
    struct gameobject_s *object; // 0x24
    u8 active;                   // 0x28
    u8 draw_result;              // 0x29
    u16 rotation_x;              // 0x2a
    u16 rotation_y;              // 0x2c
    u16 rotation_z;              // 0x2e
    f32 timer;                   // 0x30
    void *field_0x34;            // 0x34
};

struct EXPLOSION {               // size 0x34
    i32 field_0x00;              // 0x00
    i32 field_0x04;              // 0x04
    struct gameobject_s *object; // 0x08
    NUVEC position;              // 0x0c
    f32 field_0x18;              // 0x18
    f32 field_0x1c;              // 0x1c
    f32 field_0x20;              // 0x20
    i32 field_0x24;              // 0x24
    u16 field_0x28;              // 0x28
    u16 field_0x2a;              // 0x2a
    u16 field_0x2c;              // 0x2c
    u16 field_0x2e;              // 0x2e
    u16 field_0x30;              // 0x30
    u8 field_0x32;               // 0x32
    u8 field_0x33;               // 0x33
};

struct CABLE_s {                 // size 0x1ec
    struct gameobject_s *source; // 0x000
    struct gameobject_s *target; // 0x004
    NUVEC points[16];            // 0x008
    NUVEC velocities[16];        // 0x0c8
    f32 segment_lengths[15];     // 0x188
    f32 max_length;              // 0x1c4
    f32 total_length;            // 0x1c8
    u8 unknown_1cc[4];           // 0x1cc
    f32 slack;                   // 0x1d0
    f32 pull_time;               // 0x1d4
    u8 wrap_indices[15];         // 0x1d8
    u8 point_count;              // 0x1e7
    u8 wrap_count;               // 0x1e8
    u8 flags_1e9;                // 0x1e9
    u8 unknown_1ea[2];           // 0x1ea
};

struct GIZBUILDIT_s {                                  // size 0x84
    char name[16];                                     // 0x00
    struct gameanimset_s *anim_set;                    // 0x10
    struct gameanimobj_s **anim_objects;               // 0x14
    struct GIZBUILDIT_s *linked_buildit;               // 0x18
    struct MechObjectInterface *mech_object_interface; // 0x1c
    NUVEC file_position;                               // 0x20
    NUVEC start_position;                              // 0x2c
    NUVEC position;                                    // 0x38
    f32 bounds_radius;                                 // 0x44
    f32 step_timer;                                    // 0x48
    f32 step_duration;                                 // 0x4c
    f32 interaction_radius;                            // 0x50
    f32 field_0x54;                                    // 0x54
    f32 radius_scale;                                  // 0x58
    i16 field_0x5c;                                    // 0x5c

    union { // size 0x2
        i16 field_0x5e;
        u16 completion_score;
    }; // 0x5e

    i16 field_0x60;        // 0x60
    i16 field_0x62;        // 0x62
    NUVEC effect_position; // 0x64
    f32 activation_radius; // 0x70
    u8 anim_object_count;  // 0x74
    u8 build_state;        // 0x75
    u8 builders_active;    // 0x76
    u8 built_object_count; // 0x77
    u8 field_0x78;         // 0x78
    u8 progress;           // 0x79
    u16 state_flags;       // 0x7a
    u16 field_0x7c;        // 0x7c
    i16 room_index;        // 0x7e
    i16 blowup_type;       // 0x80
    u8 availability_flags; // 0x82
    u8 field_0x83;         // 0x83
};

struct STATUSPACKET_s {                                                                      // size 0x14c
    struct STATUSPACKET_LSW_s *lsw_packet;                                                   // 0x000
    i32 (*init_callback)(struct WORLDINFO_s *arg0, struct STATUSPACKET_s *arg1);             // 0x004
    i32 (*finish_callback)(struct WORLDINFO_s *arg0, struct STATUSPACKET_s *arg1, i32 arg2); // 0x008
    void (*reset_callback)(struct STATUSPACKET_s *arg0);                                     // 0x00c
    void (*draw_background_callback)(struct STATUSPACKET_s *arg0);                           // 0x010
    struct area_data_s *area;                                                                // 0x014
    struct episode_data_s *episode;                                                          // 0x018
    u32 *score;                                                                              // 0x01c
    struct mission_data_s *mission;                                                          // 0x020
    f32 player0_rumble_amount;                                                               // 0x024
    f32 player0_rumble_time;                                                                 // 0x028
    f32 player0_rumble_duration;                                                             // 0x02c
    f32 player0_buzz_amount;                                                                 // 0x030
    u8 player0_rumble_priority;                                                              // 0x034
    u8 field_0x35[3];                                                                        // 0x035
    f32 player1_rumble_amount;                                                               // 0x038
    f32 player1_rumble_time;                                                                 // 0x03c
    f32 player1_rumble_duration;                                                             // 0x040
    f32 player1_buzz_amount;                                                                 // 0x044
    u8 player1_rumble_priority;                                                              // 0x048
    u8 unknown_49[3];                                                                        // 0x049
    f32 area_time;                                                                           // 0x04c
    f32 collected_score;                                                                     // 0x050
    f32 true_hero_target;                                                                    // 0x054
    f32 true_hero_percent;                                                                   // 0x058
    f32 previous_best_time;                                                                  // 0x05c
    f32 new_best_time;                                                                       // 0x060
    f32 elapsed_time;                                                                        // 0x064
    f32 field_0x68;                                                                          // 0x068
    u32 previous_best_score;                                                                 // 0x06c
    u32 new_best_score;                                                                      // 0x070
    u32 original_score;                                                                      // 0x074
    u32 reward_score;                                                                        // 0x078
    u32 time_reward_score;                                                                   // 0x07c
    u32 final_reward_score;                                                                  // 0x080
    u32 coins_remaining[2];                                                                  // 0x084
    u32 coins_collected[2];                                                                  // 0x08c
    i32 new_minikits;                                                                        // 0x094
    u32 field_0x98;                                                                          // 0x098
    u16 player0_model;                                                                       // 0x09c
    u16 player1_model;                                                                       // 0x09e
    i16 area_id;                                                                             // 0x0a0
    i8 episode_id;                                                                           // 0x0a2
    u8 unknown_a3;                                                                           // 0x0a3

    union { // size 0x2
        u8 player_active[2];

        struct {               // size 0x2
            u8 player0_active; // 0x0a4
            u8 player1_active; // 0x0a5
        };
    }; // 0x0a4

    u8 challenge_state;   // 0x0a6
    u8 mission_state;     // 0x0a7
    f32 superstory_time;  // 0x0a8
    u32 superstory_score; // 0x0ac

    union { // size 0x4
        u32 flags_word;

        struct { // size 0x4

            union { // size 0x1
                u8 field_0xb0;

                struct {                         // size 0x1
                    u8 : 2;                      // 0x0b0:0
                    u8 true_hero_complete : 1;   // 0x0b0:2
                    u8 next_area_unlocked : 1;   // 0x0b0:3
                    u8 new_minikit_complete : 1; // 0x0b0:4
                    u8 bonus_winner : 1;         // 0x0b0:5
                    u8 free_play : 1;            // 0x0b0:6
                    u8 vehicle_area : 1;         // 0x0b0:7
                };
            }; // 0x0b0

            union { // size 0x1
                u8 mode_flags;

                struct {                     // size 0x1
                    u8 bonus_mode : 1;       // 0x0b1:0
                    u8 super_bonus_mode : 1; // 0x0b1:1
                    u8 super_story_mode : 1; // 0x0b1:2
                    u8 saved_and_exited : 1; // 0x0b1:3
                    u8 continue_story : 1;   // 0x0b1:4
                    u8 : 3;                  // 0x0b1:5
                };
            }; // 0x0b1

            u8 reserved_flags_b2[2]; // 0x0b2
        };
    }; // 0x0b0

    u8 status_flags;                         // 0x0b4
    u8 minikit_count;                        // 0x0b5
    u8 minikit_max;                          // 0x0b6
    u8 stage_count;                          // 0x0b7
    i8 current_gold_brick;                   // 0x0b8
    u8 save_state;                           // 0x0b9
    u8 prompt_choice;                        // 0x0ba
    u8 newly_completed;                      // 0x0bb
    u8 previous_completion;                  // 0x0bc
    u8 previous_gold_bricks;                 // 0x0bd
    u8 displayed_gold_bricks;                // 0x0be
    u8 field_0xbd;                           // 0x0bf
    i8 field_0xbe;                           // 0x0c0
    u8 field_0xbf[7];                        // 0x0c1
    i8 stage_types[40];                      // 0x0c8
    u8 gold_brick_enabled[40];               // 0x0f0
    i16 next_area;                           // 0x118
    i8 chapter;                              // 0x11a
    u8 unknown_119[1];                       // 0x11b
    struct STATUS_STAGE_s *previous_stage_2; // 0x11c
    struct STATUS_STAGE_s *previous_stage;   // 0x120
    struct STATUS_STAGE_s *stage;            // 0x124
    struct STATUS_STAGE_s *next_stage;       // 0x128
    u8 field_0x12c[32];                      // 0x12c
};

struct STATUS_STAGE_s {                                                                          // size 0x20
    void (*draw_callback)(struct STATUS_STAGE_s *arg0, struct STATUSPACKET_s *arg1, i32 arg2);   // 0x00
    void (*update_callback)(struct STATUS_STAGE_s *arg0, struct STATUSPACKET_s *arg1, f32 arg2); // 0x04
    void (*skip_callback)(struct STATUS_STAGE_s *arg0, struct STATUSPACKET_s *arg1);             // 0x08
    i32 type;                                                                                    // 0x0c
    u16 field_0x10;                                                                              // 0x10
    u8 field_0x12;                                                                               // 0x12
    u8 pad_13;                                                                                   // 0x13
    i32 field_0x14;                                                                              // 0x14
    f32 field_0x18;                                                                              // 0x18
    f32 field_0x1c;                                                                              // 0x1c
};

struct GIZTURRET_s {                          // size 0x13c
    u8 field_0x00[8];                         // 0x000
    char name[16];                            // 0x008
    struct gameanimset_s *anim_set;           // 0x018
    struct gameanimobj_s *primary_anim_obj;   // 0x01c
    struct gameanimobj_s *secondary_anim_obj; // 0x020
    NUVEC position;                           // 0x024
    NUVEC opponent_origin;                    // 0x030
    NUVEC initial_origin;                     // 0x03c
    NUVEC opponent_bounds;                    // 0x048
    i32 pitch;                                // 0x054
    i32 pitch_upper;                          // 0x058
    i32 pitch_lower;                          // 0x05c
    i32 yaw;                                  // 0x060
    i32 yaw_upper;                            // 0x064
    i32 yaw_lower;                            // 0x068
    i32 fixed_pitch;                          // 0x06c
    i32 fixed_yaw;                            // 0x070
    NUVEC muzzle_offsets[4];                  // 0x074
    NUMTX relative_matrix;                    // 0x0a4
    void *opponent;                           // 0x0e4
    struct gameobject_s *controller;          // 0x0e8
    f32 opponent_range;                       // 0x0ec
    f32 fire_range;                           // 0x0f0
    f32 reflection_alpha;                     // 0x0f4
    f32 fire_cooldown;                        // 0x0f8
    f32 fire_interval;                        // 0x0fc
    f32 pitch_turn_speed;                     // 0x100
    f32 yaw_turn_speed;                       // 0x104

    union { // size 0x4
        u32 behavior_flags;

        struct {                     // size 0x4
            u8 behavior_flags_low;   // 0x108
            u8 animation_flags;      // 0x109
            u16 behavior_flags_high; // 0x10a
        };
    }; // 0x108

    union { // size 0x2
        i16 field_0x10c;
        u16 completion_score;
    }; // 0x10c

    i16 room_id;         // 0x10e
    i16 pickup_pitch;    // 0x110
    i16 pickup_yaw;      // 0x112
    NUVEC pickup_offset; // 0x114
    f32 pickup_radius;   // 0x120
    i16 blowup_type;     // 0x124
    i16 rotate_sfx;      // 0x126
    i16 base_y_rotation; // 0x128
    i16 fire_sfx;        // 0x12a
    u8 opponent_type;    // 0x12c
    i8 bolt_type_id;     // 0x12d
    u8 state;            // 0x12e
    u8 initial_state;    // 0x12f
    u8 muzzle_count;     // 0x130
    u8 shots_required;   // 0x131
    u8 shots_fired;      // 0x132
    u8 unknown_133;      // 0x133
    u16 y_rotation;      // 0x134
    u8 unknown_136[2];   // 0x136
    i16 destroy_sfx;     // 0x138

    union { // size 0x2
        u16 flags;

        struct {                            // size 0x2
            u16 : 1;                        // 0x13a:0
            u16 active : 1;                 // 0x13a:1
            u16 visible : 1;                // 0x13a:2
            u16 aim_locked : 1;             // 0x13a:3
            u16 aim_at_fixed_angles : 1;    // 0x13a:4
            u16 update_disabled : 1;        // 0x13a:5
            u16 rapid_fire_pending : 1;     // 0x13a:6
            u16 fired_this_frame : 1;       // 0x13a:7
            u16 blowup_pending : 1;         // 0x13a:8
            u16 has_platform : 1;           // 0x13a:9
            u16 reward_released : 1;        // 0x13a:10
            u16 rotation_sound_playing : 1; // 0x13a:11
            u16 : 4;                        // 0x13a:12
        };
    }; // 0x13a
};

struct GIZFORCEGROUP_s {          // size 0x28
    struct GIZFORCE_s *forces[8]; // 0x00
    f32 combined_height;          // 0x20
    u8 field_0x24;                // 0x24
    u8 count;                     // 0x25
    u8 configured_count;          // 0x26
    u8 unknown_27;                // 0x27
};

struct GIZFORCESYS_s {                   // size 0x158
    struct GIZFORCE_s *forces;           // 0x000
    struct GIZFORCE_s **visible_forces;  // 0x004
    struct gizmo_s **hit_test_gizmos;    // 0x008
    u16 capacity;                        // 0x00c
    u16 count;                           // 0x00e
    u16 visible_force_count;             // 0x010
    u16 hit_test_gizmo_count;            // 0x012
    struct gameanimobjpool_s *anim_pool; // 0x014
    struct GIZFORCEGROUP_s groups[8];    // 0x018
};

struct HOTHBATTLE_MELEE_WAVE_s {       // size 0x28
    i16 character_id;                  // 0x00
    u8 reserved_02[2];                 // 0x02
    struct gameobject_s *creatures[4]; // 0x04

    union { // size 0x1
        u8 remaining_count;
        u8 field_0x18;
    }; // 0x14

    union { // size 0x1
        u8 initial_count;
        u8 field_0x19;
    }; // 0x15

    union { // size 0x1
        u8 active_count;
        u8 reserved_1a;
    }; // 0x16

    char name[13];     // 0x17
    u8 reserved_24[4]; // 0x24
};

struct HOTHBATTLE_MELEE_s { // size 0xc8

    union { // size 0x1
        u8 current_wave;
        u8 field_0x0;
    }; // 0x00

    union { // size 0x1
        u8 next_wave;
        u8 field_0x1;
    }; // 0x01

    union { // size 0x1
        u8 initialize_wave;
        u8 field_0x2;
    }; // 0x02

    u8 field_0x3; // 0x03

    union { // size 0x1
        i8 transition_phase;
        i8 field_0x4;
    }; // 0x04

    u8 field_0x5[3];                              // 0x05
    f32 wave_delay;                               // 0x08
    struct HOTHBATTLE_MELEE_WAVE_s waves[4];      // 0x0c
    struct gameobject_s *background_creatures[6]; // 0xac
    u8 creature_count;                            // 0xc4
    u8 reserved_0xc5[3];                          // 0xc5
};

struct GIZSPECIAL_s {               // size 0x28
    char name[32];                  // 0x00
    struct gameanimset_s *anim_set; // 0x20

    union { // size 0x1
        u8 flags;

        struct {                 // size 0x1
            u8 is_reversed : 1;  // 0x24:0
            u8 is_active : 1;    // 0x24:1
            u8 field_0x24_2 : 6; // 0x24:2
        };
    }; // 0x24

    u8 field_0x25[3]; // 0x25
};

struct GIZOBSTACLESYS_s {                // size 0x14
    struct gizobstacle_s *obstacles;     // 0x00
    struct gizmo_s **active_gizmos;      // 0x04
    u16 count;                           // 0x08
    u16 capacity;                        // 0x0a
    u16 active_gizmo_count;              // 0x0c
    u16 field_0x0e;                      // 0x0e
    struct gameanimobjpool_s *anim_pool; // 0x10
};

struct CHARPLATFORM_s {          // size 0x14
    struct nuhspecial_s special; // 0x00
    i16 object_id;               // 0x0c
    i16 platform_id;             // 0x0e
    struct gameobject_s *object; // 0x10
};

struct CHARPLATFORMSYS_s {              // size 0x1c
    i32 field_0x00;                     // 0x00
    i32 platform_count;                 // 0x04
    struct CHARPLATFORM_s platforms[1]; // 0x08
};

struct GIZBUILDITSYS_s {                 // size 0x14
    struct GIZBUILDIT_s *buildits;       // 0x00
    u16 count;                           // 0x04
    u16 capacity;                        // 0x06
    u16 max_animation_objects;           // 0x08
    u16 field_0x0a;                      // 0x0a
    struct gameanimobjpool_s *anim_pool; // 0x0c
    struct gameanimobj_s **anim_objects; // 0x10
};

struct pushblock_s { // size 0xd4

    union { // size 0x4
        f32 ground_offset;
        f32 vertical_penetration;
    }; // 0x00

    union { // size 0x4
        f32 ground_height;
        f32 support_height;
    }; // 0x04

    f32 snap_timer;     // 0x08
    f32 settled_height; // 0x0c
    i8 terrain_info[4]; // 0x10

    union { // size 0x4
        i8 bytes[4];
        i32 word;
    } extra_terrain_info; // 0x14

    i32 previous_extra_terrain_info; // 0x18

    union { // size 0x4
        struct pushblock_s *supporting_block;
        struct pushblock_s *block_below;
    }; // 0x1c
    struct pushblock_s *previous_supporting_block; // 0x20
    NUSPECIALHANDLE special;                       // 0x24
    struct gameobject_s *pushing_object;           // 0x30
    i32 platform_id;                               // 0x34
    u8 fields_038[8];                              // 0x38
    f32 snap_distance;                             // 0x40
    char name[16];                                 // 0x44
    struct nuvec_s *snap_positions;                // 0x54
    struct nuvec_s *position;                      // 0x58
    NUSPECIALHANDLE end_position_specials[2];      // 0x5c
    NUVEC end_position_origins[2];                 // 0x74
    NUVEC velocity;                                // 0x8c
    NUVEC target_velocity;                         // 0x98
    NUVEC snap_origin;                             // 0xa4
    NUVEC bounds_min;                              // 0xb0
    NUVEC bounds_max;                              // 0xbc

    union { // size 0x4
        u32 packed_state_flags;

        struct {                  // size 0x4
            u8 runtime_flags_0c8; // 0xc8
            u8 runtime_flags_0c9; // 0xc9
            u8 flags_0ca;         // 0xca
            u8 flags_0cb;         // 0xcb
        };

        struct {                           // size 0x4
            u32 flag_000 : 1;              // 0xc8:0
            u32 unknown_flags_001_005 : 5; // 0xc8:1
            u32 flag_006 : 1;              // 0xc8:6
            u32 flag_007 : 1;              // 0xc8:7
            u32 unknown_flags_008_011 : 4; // 0xc8:8
            u32 animation_positions : 3;   // 0xc8:12
            u32 unknown_flags_015_017 : 3; // 0xc8:15
            u32 push_visible : 1;          // 0xc8:18
            u32 unknown_flags_019_023 : 5; // 0xc8:19
            u32 push_active : 1;           // 0xc8:24
            u32 progress_state : 1;        // 0xc8:25
            u32 unknown_flags_026_031 : 6; // 0xc8:26
        };
    }; // 0xc8

    u8 unknown_0cc[4];           // 0xcc
    u8 completed_snap_positions; // 0xd0
    u8 end_position_count;       // 0xd1
    u8 output_count;             // 0xd2
    u8 unknown_0d3;              // 0xd3
};

struct GIZTURRETSYS_s {                  // size 0x10
    struct GIZTURRET_s *turrets;         // 0x0
    struct gameanimobjpool_s *anim_pool; // 0x4
    u16 count;                           // 0x8
    u16 capacity;                        // 0xa
    i8 opponent_cursor;                  // 0xc
    i8 field_0x0d[3];                    // 0xd
};

#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 8
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART == 8 && !defined(SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP)
typedef struct GIZSPINNER_s GIZSPINNER_s;

struct GIZSPINNER_s {                       // size 0x304
    NUMTX matrix;                           // 0x000
    char name[16];                          // 0x040
    struct nuhspecial_s special;            // 0x050
    NUVEC position;                         // 0x05c
    struct gameanimset_s *anim_set;         // 0x068
    struct gameanimobj_s *primary_anim_obj; // 0x06c

    union { // size 0x8
        u8 field_0x070[8];

        struct {          // size 0x8
            f32 field_70; // 0x070
            u32 field_74; // 0x074
        };
    }; // 0x070

    f32 animation_speed;            // 0x078
    u16 rotation;                   // 0x07c
    u16 target_rotation;            // 0x07e
    u16 previous_rotation;          // 0x080
    u16 initial_rotation;           // 0x082
    u32 state_flags;                // 0x084
    u8 field_0x088;                 // 0x088
    i8 room_index;                  // 0x089
    u8 state;                       // 0x08a
    u8 type;                        // 0x08b
    u16 field_0x08c;                // 0x08c
    u8 unknown_08e[2];              // 0x08e
    f32 spin_timer;                 // 0x090
    f32 field_0x094;                // 0x094
    f32 field_0x098;                // 0x098
    f32 field_0x09c;                // 0x09c
    f32 ground_height;              // 0x0a0
    u8 output_count;                // 0x0a4
    u8 unknown_0a5[7];              // 0x0a5
    u8 flags;                       // 0x0ac
    u8 unknown_0ad;                 // 0x0ad
    i16 platform_id;                // 0x0ae
    u8 field_0x0b0[4];              // 0x0b0
    struct GIZSPINNERARM_s arms[8]; // 0x0b4

    union { // size 0x4
        u8 unknown_2d4[4];
        f32 output_pause_time;
    }; // 0x2d4

    f32 unknown_2d8;                  // 0x2d8
    f32 animation_points[9];          // 0x2dc
    struct GAMEANTINODE_s *anti_node; // 0x300
};

struct GAMEANTINODE_s {          // size 0x38
    NUVEC position;              // 0x00
    f32 radius;                  // 0x0c
    f32 min_y;                   // 0x10
    f32 max_y;                   // 0x14
    f32 extent_x;                // 0x18
    f32 extent_z;                // 0x1c
    f32 remaining_time;          // 0x20
    u32 user_data[2];            // 0x24
    u16 angle;                   // 0x2c
    u8 shape;                    // 0x2e
    u8 grid_min_x;               // 0x2f
    u8 grid_min_z;               // 0x30
    u8 grid_max_x;               // 0x31
    u8 grid_max_z;               // 0x32
    u8 reserved_33;              // 0x33
    struct GAMEANTINODE_s *next; // 0x34
};

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
    typedef DEBRISPARTICLE_PC *(*DEBRISGENERATOR_PC)(struct debris_key_s *, struct debris_info_s *, f32);
    typedef void (*DEBRISMOMENTUMADJUSTER_PC)(struct debris_key_s *, struct debris_info_s *, DEBRISPARTICLE_PC *);
#ifdef __cplusplus
}
#endif

struct nugraph_s {    // size 0x5c
    i8 interpolation; // 0x00
    i8 point_count;   // 0x01
    u8 pad_02[2];     // 0x02
    f32 x[8];         // 0x04
    f32 y[8];         // 0x24
    f32 x_scale;      // 0x44
    f32 y_scale;      // 0x48
    f32 x_extent;     // 0x4c
    f32 y_extent;     // 0x50
    f32 x_offset;     // 0x54
    f32 y_offset;     // 0x58
};

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

    typedef struct debris_chunk_control_s {             // size 0x2c
        struct debris_particle_chunk_s *particle_chunk; // 0x00
        f32 expiry_time;                                // 0x04
        i32 active;                                     // 0x08
        struct debris_key_s *owner;                     // 0x0c
        struct debris_chunk_control_s *next;            // 0x10
        i32 particle_index;                             // 0x14
        f32 collision_time;                             // 0x18
        i16 effect_index;                               // 0x1c
        i16 field_01e;                                  // 0x1e
        i16 rotation_y;                                 // 0x20
        i16 unknown_022;                                // 0x22
        f32 collision_plane;                            // 0x24
        f32 restitution;                                // 0x28
    } DEBRIS_CHUNK_CONTROL_PC;

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

/* Fixed-capacity list of BaseThings run once per pass.
 * Every pass is virtual; the 0x24-byte object comes from theMemoryManager's
 * pools and its thing array from the permanent buffer. */
class ThingManager { // size 0x24
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

    BaseThing **things;  // 0x04
    i32 max_things;      // 0x08
    i32 count;           // 0x0c
    i32 permanent_count; // 0x10
    i32 pending;         // 0x14
    i32 timebar;         // 0x18
    i32 timing_thing;    // 0x1c
    i32 timing_state;    // 0x20
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

/* Base of every placeable level-editor object. */
class Placeable { // size 0x18
  public:
    i16 scene_id;    // 0x04
    i16 led_file;    // 0x06
    i32 attributes;  // 0x08
    EdString name;   // 0x0c
    EdString params; // 0x10
    u32 reserved_14; // 0x14

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

    i32 fun_006b8ce0();
    i32 fun_006b8cf0();
    void fun_006b8d00(i32 value);
    char *fun_006b8d10();
    i32 fun_006b8d20();
    void fun_006b8d30(i16 value);
    void fun_006c22b0(const char *value);
};

struct ClassObjectListEntry : public CSListLink<ClassObjectListEntry> { // size 0x14
    static void *operator new(size_t size);
    static void operator delete(void *memory);

    ClassObjectListEntry();

    EdClass *ed_class; // 0x08
    void *object;      // 0x0c
    EdRef *reference;  // 0x10
};

struct ClassObjectList {         // size 0xc
    ClassObjectListEntry *first; // 0x0
    ClassObjectListEntry *last;  // 0x4
    i32 count;                   // 0x8

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

class EdTool : public CSListLink<EdTool> { // size 0xc
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

class BaseEditor : public CSListLink<BaseEditor> { // size 0x10
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

    i32 id; // 0xc
    static i32 blockStart[8];
    static i32 blockDepth;
};

/* The class/object editor, theClassEditor. */
class ClassEditor : public BaseEditor { // size 0x70
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

    EdTool *first_tool;               // 0x10
    EdTool *last_tool;                // 0x14
    i32 tool_count;                   // 0x18
    EdTool *active_tool;              // 0x1c
    EdManipulator *manipulator;       // 0x20
    ClassObjectList selected_objects; // 0x24
    ClassObject current_object;       // 0x30
    i32 field_3c;                     // 0x3c
    ClassObject pending_object;       // 0x40
    struct eduimenu_s *menu;          // 0x4c
    VuVec snap_ray;                   // 0x50
    f32 snap_distance;                // 0x60
    i32 snap_mode;                    // 0x64
    i32 mode;                         // 0x68
    i32 class_filter;                 // 0x6c

    void DrawObjectSphere(ClassObject &selected, i32 colour);
    static void cbEdFilterLED(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdLevelSelectAll(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    static void cbEdLevelDeselectAll(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons);
    void PreSaveInitialisation();
    void PostSaveInitialisation();
    void UpdateLists(MemoryBuffer *first, MemoryBuffer *second);
    void UpdateSnapRay(VuVec &position);
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

/* The level editor thing, theLevelEditor. */
class LevelEditor : public BaseThing { // size 0x10b0
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
    i32 LoadState(variptr_u *buffer, variptr_u *buffer_end, variptr_u *scratch_begin, variptr_u *scratch_end, variptr_u *file_begin,
                  variptr_u *file_end);
    void DrawInfoText(char **lines, i32 count, i32 x, i32 y, i32 available_width, i32 available_height, i32 text_colour, i32 background);
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

    f32 background_colour[4];     // 0x0010
    i32 field_0x20;               // 0x0020
    i32 field_0x24;               // 0x0024
    i32 field_0x28;               // 0x0028
    i32 field_0x2c;               // 0x002c
    i32 field_0x30;               // 0x0030
    f32 overlay_alpha;            // 0x0034
    i32 field_0x38;               // 0x0038
    i32 editor_buffer_from_front; // 0x003c
    EdInputContext input;         // 0x0040
    u8 loading;                   // 0x02a0
    u8 destroying_objects;        // 0x02a1
    u8 reserved_0x2a2[2];         // 0x02a2
    i32 editable_scene_count;     // 0x02a4
    i32 reset_pending;            // 0x02a8
    LevelEditorScene scenes[10];  // 0x02ac
    BaseEditor *first_editor;     // 0x093c
    BaseEditor *last_editor;      // 0x0940
    i32 editor_count;             // 0x0944
    BaseEditor *active_editor;    // 0x0948
    char save_filename[0x80];     // 0x094c
    char editor_filename[0x80];   // 0x09cc
    i32 file_version;             // 0x0a4c
    i32 multi_load_active;        // 0x0a50
    i16 current_led_file;         // 0x0a54
    u8 reserved_0xa56[2];         // 0x0a56
    VARIPTR editor_buffer_cursor; // 0x0a58
    VARIPTR editor_buffer_begin;  // 0x0a5c
    VARIPTR editor_buffer_end;    // 0x0a60
    char text_buffer[0x400];      // 0x0a64
    i32 text_length;              // 0x0e64
    char *info_text[32];          // 0x0e68
    i32 info_x;                   // 0x0ee8
    i32 info_y;                   // 0x0eec
    i32 info_width;               // 0x0ef0
    i32 info_height;              // 0x0ef4
    u32 info_colour;              // 0x0ef8
    u32 info_background;          // 0x0efc
    u8 reserved_0xf00[0x100];     // 0x0f00
    i32 pad_x;                    // 0x1000
    i32 pad_y;                    // 0x1004
    i32 pad_width;                // 0x1008
    i32 pad_height;               // 0x100c
    u32 pad_background;           // 0x1010
    u32 pad_colour;               // 0x1014
    u8 reserved_0x1018[4];        // 0x1018
    char *pad_text[32];           // 0x101c
    EditorSettings settings;      // 0x109c
    i32 editors_entered;          // 0x10a8
    i32 active;                   // 0x10ac
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

class TTNetwork : public NetTransporter, public BaseThing { // size 0x110f4
  public:
    TTNetwork();
    ~TTNetwork();
    virtual void ProcessEvenWhenPaused(ThingProcessData *data);
    virtual void Display(ThingRenderData *data);
    void Start();
    void Update();
    void Shutdown();
    virtual void Send(NETMESSAGE message, u8 channel, V2Peer *peer);
    virtual void SendEx(NETMESSAGE message, u8 channel, V2Peer *peer, i32 arg0, i32 arg1);
    virtual void Broadcast(NETMESSAGE message, u8 channel);
    virtual void BroadcastEx(NETMESSAGE message, u8 channel);
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
    void DeliverMessage(NETMESSAGE &message, u8 channel, const V2Peer &peer);
    void RecordSent(NETMESSAGE message, u8 channel);
    void RecordReceived(NETMESSAGE message, u8 channel);
    void Resume();
    i32 Suspend();
    NetAddress *GetMyAddress();
    NetAddress *GetMyHostAddress();
    void SetMyHostAddress(NetAddress *address);
    void ClearMyHostAddress();
    i32 fun_00611dc0() const;

    NetLogFile log;                    // 0x00020
    u32 frame_count;                   // 0x02134
    i32 unknown_2138;                  // 0x02138
    i32 unknown_213c;                  // 0x0213c
    i32 state;                         // 0x02140
    i32 unknown_2144;                  // 0x02144
    i32 unknown_2148;                  // 0x02148
    i32 unknown_214c;                  // 0x0214c
    i32 display_page;                  // 0x02150
    f32 display_x;                     // 0x02154
    f32 display_y;                     // 0x02158
    f32 display_width;                 // 0x0215c
    f32 display_height;                // 0x02160
    u32 unknown_2164[2];               // 0x02164
    NetAddress my_address;             // 0x0216c
    NetAddress my_host_address;        // 0x02170
    i32 has_my_host_address;           // 0x02174
    NetFtpManager ftp;                 // 0x02178
    NetworkObjectManager nos;          // 0x03780
    V2SessionManager *session_manager; // 0x110f0
};
#endif
#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_PART 15
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_LEGOAPI_TYPES_H_STOP
