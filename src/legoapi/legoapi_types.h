#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aipath.h"
#include "globals.h"
#include "legoapi/items/objects/basething.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

struct AIAREA;
struct ClassObject;
struct EdClass;
struct EdInputContext;
struct FtpFile;
struct NETMESSAGE;
struct NetAddress;
struct NetworkObject;
struct PC_OCCLUDER_RECORD;
struct TERRAIN_PLATFORM;
struct TERRAIN_POLYGON_INFO;
struct TERRAIN_SKIN_STREAM;
struct ThingProcessData;
struct VuVec;
struct debris_particle_s;

struct BOLTTYPE_s;
struct FLOWBOX;
struct FORCE_RESET_GROUP;
struct FRAME_PAD;
struct GIZFLOW;
struct GIZMOPICKUP;
struct NUGSCN_s;
struct PART;
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

typedef struct TERRAIN_WALL_POINT TERRAIN_WALL_POINT;
typedef struct TERRAIN_SHAPE_BATCH TERRAIN_SHAPE_BATCH;

typedef struct TERRAIN_GROUP {        // size 0x38
    NUVEC origin;                     // 0x00
    struct TERRAIN_SKIN_STREAM *data; // 0x0c
    NUVEC bounds_min;                 // 0x10
    NUVEC bounds_max;                 // 0x1c
    u32 platform_flags;               // 0x28
    i32 chunk_type;                   // 0x2c
    i16 scene_index;                  // 0x30
    i16 group_info;                   // 0x32
    f32 radius;                       // 0x34
} TERRAIN_GROUP;

typedef struct TERRAIN_CELL { // size 0x14
    i16 first_group;          // 0x00
    i16 group_count;          // 0x02
    f32 min_x;                // 0x04
    f32 min_z;                // 0x08
    f32 max_x;                // 0x0c
    f32 max_z;                // 0x10
} TERRAIN_CELL;

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

typedef struct terrain_surface_s { // size 0xc
    f32 movement_scale;            // 0x0
    u32 flags;                     // 0x4

    union { // size 0x4
        u32 unknown_08;

        struct {                          // size 0x4
            u16 object_id;                // 0x8
            u16 unknown_object_high_word; // 0xa
        };
    }; // 0x8
} TERRAIN_SURFACE;

struct bolt_s;
struct bolt_s;

typedef struct narrowsockexception_s { // size 0x10
    const char *level_name;            // 0x0
    i16 level_index;                   // 0x4
    i16 sock_index;                    // 0x6
    f32 start_distance;                // 0x8
    f32 end_distance;                  // 0xc
} NARROWSOCKEXCEPTION;

typedef struct cutsys_s CUTSYS;
typedef struct gameanimsys_s GAMEANIMSYS;

typedef struct SoundTable { // size 0xc8
    u16 bits[100];          // 0x00
} SoundTable;

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

typedef struct gizforce_s GIZFORCE;
#ifdef __cplusplus
extern "C" {
#endif

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
    } EDPP_PARTICLE;

/* EDPP_PARTICLE.instance_id of a placed effect that has no debris instance (edppPtlShelve, a page that is off, an
 * AddDebrisEffect without a free key); -1 is a free slot. */
#define EDPP_SHELVED 99999

    struct GIZTURRET_s;

    typedef struct ripple_set_s RIPPLE_SET;
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
        u8 unknown_5[3];

        struct {                    // size 0x3
            u8 minikit_count;       // 0x5
            u8 red_brick_collected; // 0x6
            u8 reserved_0x7;        // 0x7
        };
    }; // 0x5

    f32 challenge_trial_time; // 0x8
};

struct STATUSPACKET_LSW_s { // size 0x4
    i32 unknown_00;         // 0x0
};

struct GIZFORCEANIMDATA_s { // size 0x8
    u16 flags;              // 0x0
    i16 platform_id;        // 0x2

    union { // size 0x4
        u32 unknown_04;

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
    char pad_02[4];           // 0x2
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

/* gameanimset_s.state, from GameAnimSet_EvaluateState and GameAnimSys_Update: where the set's animations are. */
#define GAMEANIMSET_STATE_AT_START 0
#define GAMEANIMSET_STATE_FORWARD 1 /* playing, all forward */
#define GAMEANIMSET_STATE_AT_END 2
#define GAMEANIMSET_STATE_BACKWARD 3 /* playing, some backward */
#define GAMEANIMSET_STATE_PART_WAY 4 /* stopped between start and end */

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

/* gamecamera_s.mode: the camera GameCam_Update picks (LEGOCAMMODE_DOORCUT and LEGOCAMMODE_OBSTACLE are set to 4 and 2
 * at start-up); the spline modes use the level splines mission_cam, char_cam and point_cam. */
#define GAMECAM_MODE_NONE (-1) /* none chosen yet: the next update picks one */
#define GAMECAM_MODE_DEFAULT 0
#define GAMECAM_MODE_SOCKS 1    /* follows the players through the sock system */
#define GAMECAM_MODE_OBSTACLE 2 /* an obstacle or DynamicCameraCut spline */
#define GAMECAM_MODE_TITLES 3   /* the titles level's point_cam spline */
#define GAMECAM_MODE_DOOR_CUT 4
#define GAMECAM_MODE_VIEWCAM 5   /* ViewCam has a pad */
#define GAMECAM_MODE_MISSION 6   /* the mission menu's mission_cam spline */
#define GAMECAM_MODE_CUSTOMISE 7 /* the customiser menu's char_cam spline */
#define GAMECAM_MODE_SHOP 8
#define GAMECAM_MODE_MINIKIT_VIEWER 9 /* the hub's minikit viewer spline */
#define GAMECAM_MODE_GUNSHIP 11       /* GUNSHIPAHACK on the gunship bonus level */

typedef struct gamecamera_s { // size 0x230

    union { // size 0x38
        SOCKPOSITION sock_position;

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
    f32 unknown_204;         // 0x204
    f32 unknown_208;         // 0x208
    f32 unknown_20c;         // 0x20c
    f32 unknown_210;         // 0x210
    f32 unknown_214;         // 0x214
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
    u8 unknown_42[2];    // 0x42
};

struct GIZPANEL_s {   // size 0x9c
    NUMTX matrix;     // 0x00
    char name[16];    // 0x40
    NUVEC position;   // 0x50
    u16 y_rotation;   // 0x5c
    u8 unknown_5e[3]; // 0x5e
    u8 model_variant; // 0x61
    u8 unknown_62[2]; // 0x62
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
    u8 unknown_6a[2];                                  // 0x6a
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

typedef struct TERRAIN_INDEX_LEVEL {   // size 0x80c
    i16 cache_age;                     // 0x000
    u8 unknown_02[2];                  // 0x002
    f32 center_x;                      // 0x004
    f32 center_z;                      // 0x008
    TERRAIN_SCAN_WORD scan_words[512]; // 0x00c
} TERRAIN_INDEX_LEVEL;

typedef struct TERRAIN_SET {                    // size 0xa490
    struct TERRAIN_GROUP *groups;               // 0x0000
    u8 unknown_004[0x60];                       // 0x0004
    void *unknown_064;                          // 0x0064
    struct TERRAIN_PLATFORM *platforms;         // 0x0068
    i16 active_platform_groups[96];             // 0x006c
    i32 active_platform_count;                  // 0x012c
    f32 platform_query_min_x;                   // 0x0130
    u8 unknown_134[4];                          // 0x0134
    f32 platform_query_min_z;                   // 0x0138
    f32 platform_query_max_x;                   // 0x013c
    u8 unknown_140[4];                          // 0x0140
    f32 platform_query_max_z;                   // 0x0144
    struct TERRAIN_SPATIAL_NODE *spatial_nodes; // 0x0148
    i16 group_count;                            // 0x014c
    u16 file_version;                           // 0x014e
    i32 group_index_count;                      // 0x0150
    i32 removed_platform_count;                 // 0x0154
    i16 removed_platforms[32];                  // 0x0158
    TERRAIN_TRACK_SLOT track_slots[64];         // 0x0198
    TERRAIN_CELL cells[256];                    // 0x0798
    TERRAIN_CELL platform_cell;                 // 0x1b98
    i32 minimum_height;                         // 0x1bac
    i32 used_cell_count;                        // 0x1bb0
    i16 *group_indices;                         // 0x1bb4
    TERRAIN_INDEX_LEVEL index_levels[17];       // 0x1bb8
    i32 max_group_indices;                      // 0xa484
    i32 max_groups;                             // 0xa488
    i32 max_platforms;                          // 0xa48c
} TERRAIN_SET;

typedef TERRAIN_SET TERRSET;

#ifdef __cplusplus
extern "C" {
#endif
    struct TERRAIN_SPATIAL_NODE;
#ifdef __cplusplus
}
#endif

typedef struct levelspline_s { // size 0x10
    struct NUGSCN_s **scene;   // 0x0
    const char *name;          // 0x4
    u16 min_points;            // 0x8
    u16 max_points;            // 0xa
    i16 level;                 // 0xc
    i16 area;                  // 0xe
} LEVELSPLINE;

/* nuinstanim_s.flags: the animation editor's control modes start (0x1) and loop (0x8) instance animations, and the
 * game animation sets copy their `repeating` setting into 0x8. */
#define NUINSTANIM_FLAG_PLAYING 0x1
#define NUINSTANIM_FLAG_REPEATING 0x8

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
    struct PC_OCCLUDER_RECORD *occluders; // 0x00
    u32 *indices;                         // 0x04
    u32 capacity;                         // 0x08
    u32 count;                            // 0x0c
    bool queries_prepared;                // 0x10
    u8 unknown_11[15];                    // 0x11
    NUMTX projection_matrix;              // 0x20
    NUMTX query_matrix;                   // 0x60
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
            u8 unknown_20d;   // 0x20d
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
    i32 unknown_4300;               // 0x4300
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

/* AIPATHCNXCONTROLLER_s.target_type: what AIPathCnxControllerCreate looks the target name up as (the
 * controller script action's target keyword). */
#define AIPATHCNX_TARGET_SPECIAL 0
#define AIPATHCNX_TARGET_CUTSCENE 1
#define AIPATHCNX_TARGET_BUILDIT 2
#define AIPATHCNX_TARGET_GIZMO 3
#define AIPATHCNX_TARGET_BLOWUP 4
#define AIPATHCNX_TARGET_FAKE_ANIMATION 5 /* no target object, fake_animation_id */
#define AIPATHCNX_TARGET_FLOW 6
#define AIPATHCNX_TARGET_FORCE 7
#define AIPATHCNX_TARGET_OBSTACLE 8
#define AIPATHCNX_TARGET_ZIPUP 9

struct AIPATHCNXCONTROLLER_s {    // size 0xa8
    NULISTLNK links;              // 0x00
    struct AIPATHCNX *connection; // 0x08

    union { // size 0xc
        void *target;
        nuhspecial_s special;
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
    i32 unknown_28;              // 0x28
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

typedef struct BOLTSYS_s {                                       // size 0x20
    struct BOLTTYPE_s *types;                                    // 0x00
    i32 count;                                                   // 0x04
    void (*stop_targeting)(struct gameobject_s *, NUVEC *);      // 0x08
    void (*debris)(struct bolt_s *, NUVEC *, i32, NUVEC *, i32); // 0x0c
    void (*shoot_origin)(struct gameobject_s *, NUVEC *);        // 0x10
    u16 (*shoot_direction)(struct gameobject_s *, NUVEC *);      // 0x14
    i32 (*hit_part)(struct bolt_s *, struct PART *);             // 0x18
    i32 (*alternate_fire)(struct gameobject_s *, i32);           // 0x1c
} BOLTSYS;

#ifdef __cplusplus
extern "C" {
#endif
    extern BOLTSYS BoltSys_Batman;

    extern BOLTSYS BoltSys_LSW;
#ifdef __cplusplus
}
#endif

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
    extern BOLTSYS *BoltSys;

    struct GIZTURRETSYS_s;
    typedef struct pushblock_s PUSH_BLOCK;
#ifdef __cplusplus
}
#endif
typedef struct FADER_s FADER_s;
typedef struct GIZBUILDITSYS_s GIZBUILDITSYS_s;
typedef struct GIZFORCESYS_s GIZFORCESYS_s;
typedef struct GIZMOPICKUPRUNTIMESYS_s GIZMOPICKUPRUNTIMESYS_s;
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

typedef struct gameanimobj_s GAMEANIMOBJ;

struct gameanimobj_s {              // size 0x28
    GAMEANIMOBJ *next;              // 0x00
    nuhspecial_s special;           // 0x04
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

/* gameanimset_s.flags, from gameanim.c. */
#define GAMEANIMSET_PLAYING 0x1        /* an object's animation played this update */
#define GAMEANIMSET_STOPPED 0x2        /* GameAnimSet_Stop; the next update takes the set off the active list */
#define GAMEANIMSET_IN_ACTIVE_LIST 0x4 /* GameAnimSet_AddToSystemList */

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

/* gizobstacle_s.runtime_flags: the bits gizobstacle.c saves in the level progress, and the AI script's
 * locks (AlwaysTriggerObstacle and ObstacleLockedOpen, CanTriggerObstacle and ObstacleLockedShut). */
#define GIZOBSTACLE_PICKUPS_SPAWNED 0x1 /* the reward has been released */
#define GIZOBSTACLE_BLOWUP_NAME 0x2     /* blowup_type holds a name table id until the post-load */
#define GIZOBSTACLE_LOCKED_OPEN 0x4
#define GIZOBSTACLE_LOCKED_SHUT 0x8
#define GIZOBSTACLE_ON_PLATFORM 0x10 /* an animated object has a terrain platform */
#define GIZOBSTACLE_TRIGGER_LATCHED 0x40
#define GIZOBSTACLE_DESTROYED 0x80

/* gizobstacle_s.progress_flags, from gizobstacle.c. */
#define OBSTACLE_ACTIVE 0x1            /* GizmoObstacle_Activate (saved in the progress) */
#define OBSTACLE_VISIBLE 0x2           /* saved in the progress */
#define OBSTACLE_TECHNO_CONTROLLED 0x4 /* GizObstacle_SetTechnoControlled */
#define OBSTACLE_PUSH_CONTROLLED 0x8   /* GizObstacle_SetPushControlled */

typedef void (*PICKUP_COLLECT_FN)(struct WORLDINFO_s *, struct GIZMOPICKUP *, i32, struct gameobject_s *, i32);

typedef struct GIZMO_PICKUP_TYPE { // size 0x38
    char *name;                    // 0x00
    char *activation_sfx_name;     // 0x04
    char *collection_sfx_name;     // 0x08
    u8 field_c;                    // 0x0c
    u8 random_model_count;         // 0x0d
    u8 flags;                      // 0x0e
    u8 unknown_0f;                 // 0x0f
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
    nuhspecial_s along_socket;         // 0x2c
    struct gameobject_s *using_object; // 0x38
    u32 reset_word;                    // 0x3c
    struct FORCE_RESET_GROUP *group;   // 0x40
    f32 force_strength;                // 0x44
    f32 unknown_48;                    // 0x48
    f32 force_range;                   // 0x4c
    f32 unknown_50;                    // 0x50
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

/* gizforce_s.runtime_flags: the bits the bit-field above names, and the others from their setters in gizforce.c. */
#define GIZFORCE_ENABLED 0x1                /* GizmoForce_Activate; GizmoForce_ActivateRev forwards */
#define GIZFORCE_VISIBLE 0x2                /* GizForce_SetVisibility */
#define GIZFORCE_REVERSED 0x10              /* GizmoForce_ActivateRev backwards */
#define GIZFORCE_IN_STACK 0x20              /* added to its group's stack of forces */
#define GIZFORCE_STACK_COMPLETE 0x40        /* stacked, its animation complete and nobody using it */
#define GIZFORCE_ON_SCREEN 0x80             /* in the camera's view, listed in visible_forces */
#define GIZFORCE_ON_PLATFORM 0x100          /* an animated object has a terrain platform */
#define GIZFORCE_REWARD_RELEASED 0x200      /* the pickups have been released */
#define GIZFORCE_BLOWUP_NAME 0x400          /* blowup_type holds a name table id until GizForces_PostLoad */
#define GIZFORCE_SOCKET_HIDDEN 0x800        /* hidden at the end of the animation (base special forces) */
#define GIZFORCE_SHAKE_OFFSET 0x1000        /* the shake offset is applied to the animation */
#define GIZFORCE_SHAKE_DONE 0x2000          /* shaken for force_range seconds */
#define GIZFORCE_COMPLETION_RELEASED 0x4000 /* the completion blowup and reward have happened */
#define GIZFORCE_COMPLETION_PENDING 0x8000  /* a bolt hit set it moving; cleared when complete */
#define GIZFORCE_DESTROYED 0x10000          /* blown up or thrown, hidden for good */
#define GIZFORCE_BEING_USED 0x200000        /* a character is using the force */
#ifdef __cplusplus
extern "C" {
#endif

    /* One of the eight explosions that run at a time (Explosions, a ring that AddExplosion advances). */
    typedef struct EXPLOSION {      // size 0x38
        i32 hit_identity_mask[2];   // 0x00 collision identities of the objects already hit
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
        u16 kill_count;             // 0x30 hits that killed
        u8 damage;                  // 0x32
        i8 player_index;            // 0x33 credited player without an owner (0, 1), -1 none
        u8 unknown_34[4];           // 0x34
    } EXPLOSION;

    extern EXPLOSION Explosions[8];

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
        f32 unknown_3c;
        u32 field_0x3c_bits;
    }; // 0x3c
    struct GIZFORCEGROUP_s *group; // 0x40
    f32 force_strength;            // 0x44
    f32 unknown_48;                // 0x48
    f32 force_range;               // 0x4c
    f32 unknown_50;                // 0x50
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
    u32 unknown_7c;                // 0x7c
    u8 group_index;                // 0x80
    u8 collision_mask;             // 0x81
    u8 activation_mode;            // 0x82
    u8 unknown_83;                 // 0x83
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
        u32 unknown_a4;
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
    f32 unknown_54;                                    // 0x54
    f32 radius_scale;                                  // 0x58
    i16 unknown_5c;                                    // 0x5c

    union { // size 0x2
        i16 unknown_5e;
        u16 completion_score;
    }; // 0x5e

    i16 unknown_60;        // 0x60
    i16 unknown_62;        // 0x62
    NUVEC effect_position; // 0x64
    f32 activation_radius; // 0x70
    u8 anim_object_count;  // 0x74
    u8 build_state;        // 0x75
    u8 builders_active;    // 0x76
    u8 built_object_count; // 0x77
    u8 unknown_78;         // 0x78
    u8 progress;           // 0x79
    u16 state_flags;       // 0x7a
    u16 unknown_7c;        // 0x7c
    i16 room_index;        // 0x7e
    i16 blowup_type;       // 0x80
    u8 availability_flags; // 0x82
    u8 unknown_83;         // 0x83
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
    u8 unknown_35[3];                                                                        // 0x035
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
    f32 unknown_68;                                                                          // 0x068
    u32 previous_best_score;                                                                 // 0x06c
    u32 new_best_score;                                                                      // 0x070
    u32 original_score;                                                                      // 0x074
    u32 reward_score;                                                                        // 0x078
    u32 time_reward_score;                                                                   // 0x07c
    u32 final_reward_score;                                                                  // 0x080
    u32 coins_remaining[2];                                                                  // 0x084
    u32 coins_collected[2];                                                                  // 0x08c
    i32 new_minikits;                                                                        // 0x094
    u32 unknown_98;                                                                          // 0x098
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
                u8 unknown_b0;

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
    u8 unknown_12c[32];                      // 0x12c
};

/* STATUSPACKET_s.flags_word: the bits of the bit-fields above, set from the area flags and the game globals
 * (BuildUpDone, BonusWinner, FreePlay, SuperStory, PauseSaveAndExit). */
#define STATUSPACKET_TRUE_HERO_COMPLETE 0x4 /* BuildUpDone */
#define STATUSPACKET_NEXT_AREA_UNLOCKED 0x8
#define STATUSPACKET_NEW_MINIKIT_COMPLETE 0x10
#define STATUSPACKET_BONUS_WINNER 0x20 /* the second player won the bonus */
#define STATUSPACKET_FREE_PLAY 0x40
#define STATUSPACKET_VEHICLE_AREA 0x80      /* AREAFLAG_VEHICLE_AREA */
#define STATUSPACKET_BONUS_MODE 0x100       /* AREAFLAG_BONUS_AREA */
#define STATUSPACKET_SUPER_BONUS_MODE 0x200 /* AREAFLAG_SUPER_BONUS */
#define STATUSPACKET_SUPER_STORY_MODE 0x400
#define STATUSPACKET_SAVED_AND_EXITED 0x800 /* PauseSaveAndExit */
#define STATUSPACKET_CONTINUE_STORY 0x1000

/* STATUS_STAGE_s.type: named after the stage callbacks of the status screen tables (StatusStagesLsw,
 * StatusStagesAlt) and the conditions AddStatusStage is called under; 5-7, 24 and 27-34 are not known yet. */
#define STATUS_STAGE_TRUE_JEDI 1       /* True Jedi reached (POINTS_PER_TRUEJEDI) */
#define STATUS_STAGE_TRUE_JEDI_SCORE 2 /* the studs collected, True Jedi not reached */
#define STATUS_STAGE_COINS 3
#define STATUS_STAGE_MINIKIT 4
#define STATUS_STAGE_GOTHAM_COMPLETE 8 /* GOTHAMSTREETS_ADATA newly completed */
#define STATUS_STAGE_PROMPT 9
#define STATUS_STAGE_EXIT 10
#define STATUS_STAGE_SAVE 11
#define STATUS_STAGE_FADE 12
#define STATUS_STAGE_LEVEL_COMPLETE 13       /* a newly completed level */
#define STATUS_STAGE_SUPER_STORY_COMPLETE 14 /* the super story reward */
#define STATUS_STAGE_SUPER_STORY_TIME 15
#define STATUS_STAGE_SUPER_STORY_SCORE 16
#define STATUS_STAGE_ALL_MINIKITS 17
#define STATUS_STAGE_RED_BRICK 18
#define STATUS_STAGE_GOLD_BRICK 19
#define STATUS_STAGE_BONUS_WIN 20
#define STATUS_STAGE_BONUS_TIME 21
#define STATUS_STAGE_BONUS_COMPLETE 22
#define STATUS_STAGE_CHALLENGE 23 /* challenge_state */
#define STATUS_STAGE_CHALLENGE_CASH 25
#define STATUS_STAGE_MISSION 26 /* mission_state */
#define STATUS_STAGE_COLLECT_CHARACTERS 35
#define STATUS_STAGE_COLLECT_CHARACTERS_OFF 36

struct STATUS_STAGE_s {                                                                          // size 0x20
    void (*draw_callback)(struct STATUS_STAGE_s *arg0, struct STATUSPACKET_s *arg1, i32 arg2);   // 0x00
    void (*update_callback)(struct STATUS_STAGE_s *arg0, struct STATUSPACKET_s *arg1, f32 arg2); // 0x04
    void (*skip_callback)(struct STATUS_STAGE_s *arg0, struct STATUSPACKET_s *arg1);             // 0x08
    i32 type;                                                                                    // 0x0c
    u16 unknown_10;                                                                              // 0x10
    u8 unknown_12;                                                                               // 0x12
    u8 pad_13;                                                                                   // 0x13
    i32 unknown_14;                                                                              // 0x14
    f32 unknown_18;                                                                              // 0x18
    f32 unknown_1c;                                                                              // 0x1c
};

struct GIZTURRET_s {                          // size 0x13c
    u8 unknown_00[8];                         // 0x000
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
        i16 unknown_10c;
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
    u8 unknown_24;                // 0x24
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

    u8 unknown_25[3]; // 0x25
};

struct GIZOBSTACLESYS_s {                // size 0x14
    struct gizobstacle_s *obstacles;     // 0x00
    struct gizmo_s **active_gizmos;      // 0x04
    u16 count;                           // 0x08
    u16 capacity;                        // 0x0a
    u16 active_gizmo_count;              // 0x0c
    u16 unknown_0e;                      // 0x0e
    struct gameanimobjpool_s *anim_pool; // 0x10
};

struct CHARPLATFORM_s {          // size 0x14
    struct nuhspecial_s special; // 0x00
    i16 object_id;               // 0x0c
    i16 platform_id;             // 0x0e
    struct gameobject_s *object; // 0x10
};

struct CHARPLATFORMSYS_s {              // size 0x1c
    i32 unknown_00;                     // 0x00
    i32 platform_count;                 // 0x04
    struct CHARPLATFORM_s platforms[1]; // 0x08
};

struct GIZBUILDITSYS_s {                 // size 0x14
    struct GIZBUILDIT_s *buildits;       // 0x00
    u16 count;                           // 0x04
    u16 capacity;                        // 0x06
    u16 max_animation_objects;           // 0x08
    u16 unknown_0a;                      // 0x0a
    struct gameanimobjpool_s *anim_pool; // 0x0c
    struct gameanimobj_s **anim_objects; // 0x10
};

/* pushblock_s.packed_state_flags: push_visible, push_active and progress_state are the struct's bitfield names; the
 * axis and direction bits are loaded by edpush_Load and limit the pushes NearestFacingPushBlock offers. */
#define PUSHBLOCK_X_AXIS_ONLY 0x100000
#define PUSHBLOCK_Z_AXIS_ONLY 0x200000
#define PUSHBLOCK_NO_NEGATIVE_PUSH 0x400000
#define PUSHBLOCK_NO_POSITIVE_PUSH 0x800000
#define PUSHBLOCK_PUSH_VISIBLE 0x40000
#define PUSHBLOCK_PUSH_ACTIVE 0x1000000
#define PUSHBLOCK_PROGRESS_STATE 0x2000000

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
    nuhspecial_s special;                          // 0x24
    struct gameobject_s *pushing_object;           // 0x30
    i32 platform_id;                               // 0x34
    u8 fields_038[8];                              // 0x38
    f32 snap_distance;                             // 0x40
    char name[16];                                 // 0x44
    struct nuvec_s *snap_positions;                // 0x54
    struct nuvec_s *position;                      // 0x58
    nuhspecial_s end_position_specials[2];         // 0x5c
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
    i8 unknown_0d[3];                    // 0xd
};

typedef struct GIZSPINNER_s GIZSPINNER_s;

struct GIZSPINNER_s {                       // size 0x304
    NUMTX matrix;                           // 0x000
    char name[16];                          // 0x040
    struct nuhspecial_s special;            // 0x050
    NUVEC position;                         // 0x05c
    struct gameanimset_s *anim_set;         // 0x068
    struct gameanimobj_s *primary_anim_obj; // 0x06c

    union { // size 0x8
        u8 unknown_070[8];

        struct {            // size 0x8
            f32 unknown_70; // 0x070
            u32 unknown_74; // 0x074
        };
    }; // 0x070

    f32 animation_speed;            // 0x078
    u16 rotation;                   // 0x07c
    u16 target_rotation;            // 0x07e
    u16 previous_rotation;          // 0x080
    u16 initial_rotation;           // 0x082
    u32 state_flags;                // 0x084
    u8 unknown_088;                 // 0x088
    i8 room_index;                  // 0x089
    u8 state;                       // 0x08a
    u8 type;                        // 0x08b
    u16 unknown_08c;                // 0x08c
    u8 unknown_08e[2];              // 0x08e
    f32 spin_timer;                 // 0x090
    f32 unknown_094;                // 0x094
    f32 unknown_098;                // 0x098
    f32 unknown_09c;                // 0x09c
    f32 ground_height;              // 0x0a0
    u8 output_count;                // 0x0a4
    u8 unknown_0a5[7];              // 0x0a5
    u8 flags;                       // 0x0ac
    u8 unknown_0ad;                 // 0x0ad
    i16 platform_id;                // 0x0ae
    u8 unknown_0b0[4];              // 0x0b0
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

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct debris_particle_s *(*DEBRISGENERATOR)(struct debris_key_s *, struct debris_info_s *, f32);

    extern DEBRISGENERATOR gensorttab[13];
    typedef void (*DEBRISMOMENTUMADJUSTER)(struct debris_key_s *, struct debris_info_s *, struct debris_particle_s *);

    extern DEBRISMOMENTUMADJUSTER gencodetab[7];
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
    virtual void ProcessThings(struct ThingProcessData *data);
    virtual void RenderThings(ThingRenderData *data);
    virtual void DisplayThings(ThingRenderData *data);
    virtual void EffectsThings(ThingRenderData *data);
    void EnableActions(u32 id, u32 flags, i32 clear);
    static void cbEdTrackCancel(struct eduimenu_s *menu, struct eduimenu_s *parent);
    static void cbEdTimingSelect(struct eduimenu_s *menu, struct eduiitem_s *item, u32 value);
    static void edTimingInit();
    static void edTimingEnter();
    static i32 edTimingProc(f32 delta_time, struct FRAME_PAD *pad);
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

typedef struct mine_entry_s { // size 0x1c
    u32 active;               // 0x00
    NUVEC pos;                // 0x04
    f32 grow;                 // 0x10
    f32 grow_alpha;           // 0x14
    u16 rotx;                 // 0x18
    u16 roty;                 // 0x1a
} MINE_ENTRY;

typedef struct minesys_s {           // size 0x748
    nuhspecial_s mine_special;       // 0x000
    MINE_ENTRY mines[64];            // 0x00c
    f32 mine_radius;                 // 0x70c
    f32 spawn_timer;                 // 0x710
    struct AIAREA *nomine_areas[10]; // 0x714
    i16 nomine_count;                // 0x73c
    i16 mine_debris;                 // 0x73e
    i32 mine_part;                   // 0x740
    f32 update_timer;                // 0x744
} MINESYS;
