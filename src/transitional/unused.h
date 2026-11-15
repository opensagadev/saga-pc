/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizflow.h"
#include "legoapi/gizmos/transport/grapples.h"
#include "legoapi/render/core/rtl.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#endif

struct KEEP_SCREEN_PLANE;
struct parttype_lookup_s;

#define QRAND_FLOAT() (qrand() * (1.0f / 65535.0f))

/* Types used by the units below. */
typedef struct AIAREA_s AIAREA_s;
typedef struct AIPATHCNXCONTROLSYS_s AIPATHCNXCONTROLSYS_s;
typedef struct AREA_GLOBAL_VALUES AREA_GLOBAL_VALUES;
typedef struct CHARACTER_SHADOW_s CHARACTER_SHADOW_s;
typedef struct CLIMBOBJECTSYS_s CLIMBOBJECTSYS_s;
typedef struct CLIMBOBJECT_s CLIMBOBJECT_s;
typedef struct CUSTOMPIECE CUSTOMPIECE;
typedef struct CUSTOMPIECECATEGORY CUSTOMPIECECATEGORY;
typedef struct CUTSYS CUTSYS;
typedef struct HINT_s HINT_s;
typedef struct LEVELSCRIPTPROGRESS_s LEVELSCRIPTPROGRESS_s;
typedef struct LEVEL_PROGRESS_DATA_s LEVEL_PROGRESS_DATA_s;
typedef struct LEVEL_PROGRESS_s LEVEL_PROGRESS_s;
typedef struct MINEENTRY_s MINEENTRY_s;
typedef struct MINESYS_s MINESYS_s;
typedef struct NuLegacyInstanceLayout NuLegacyInstanceLayout;
typedef struct NuPlatform NuPlatform;
typedef struct NuSpecialBoundsDisplayLayout NuSpecialBoundsDisplayLayout;
typedef struct NuSpecialLegacyLayout NuSpecialLegacyLayout;
typedef struct NuSpecialLegacyObjectBoundsLayout NuSpecialLegacyObjectBoundsLayout;
typedef struct NuSpecialLegacySceneLayout NuSpecialLegacySceneLayout;
typedef struct PARTLIGHTSOURCE_s PARTLIGHTSOURCE_s;
typedef struct PLAYERCHARACTERCONFIG_s PLAYERCHARACTERCONFIG_s;
typedef struct PLAYERITEM_s PLAYERITEM_s;
typedef struct PLAYERPACKET_s PLAYERPACKET_s;
typedef struct PLAYERSUBOBJ2_s PLAYERSUBOBJ2_s;
typedef struct PLAYERSUBOBJ_s PLAYERSUBOBJ_s;
typedef struct PLUG_s PLUG_s;
typedef struct RUMBLEPACKET RUMBLEPACKET;
typedef struct SCENEPROGRESS_s SCENEPROGRESS_s;
typedef struct SCENESPECIALPROGRESS_s SCENESPECIALPROGRESS_s;
typedef struct SHOPINPUT SHOPINPUT;
typedef struct SOCKSYS SOCKSYS;
typedef struct TintStack TintStack;
typedef struct _vuv_s _vuv_s;
typedef struct instNUGCUTRIGID_s instNUGCUTRIGID_s;
typedef struct nuclipbounds_s nuclipbounds_s;
typedef struct nuclipobject_s nuclipobject_s;
typedef struct nucolour3_s nucolour3_s;
typedef struct rtldata_s rtldata_s;
typedef struct spacelevel_cross_s spacelevel_cross_s;
typedef struct storepack_s storepack_s;

struct spacelevel_cross_s { // size 0x50
    u8 reserved_00[16];     // 0x00
    NUVEC4 local_position;  // 0x10
    u8 reserved_20[16];     // 0x20
    NUVEC4 world_position;  // 0x30
    u16 flags;              // 0x40
    u16 enabled;            // 0x42
    u8 reserved_44[4];      // 0x44
    u32 colour;             // 0x48
    f32 scale;              // 0x4c
};

struct _vuv_s { // size 0x10
    f32 x;      // 0x0
    f32 y;      // 0x4
    f32 z;      // 0x8
    f32 w;      // 0xc
};

struct storepack_s { // size 0x34
    char *name;      // 0x00

    union { // size 0x4
        char *product_id;

        struct {           // size 0x4
            u8 field1_0x4; // 0x04
            u8 field2_0x5; // 0x05
            u8 field3_0x6; // 0x06
            u8 field4_0x7; // 0x07
        };
    }; // 0x04

    union { // size 0x2
        i16 message_text_index;

        struct {           // size 0x2
            u8 field5_0x8; // 0x08
            u8 field6_0x9; // 0x09
        };
    }; // 0x08

    u8 field7_0xa;                    // 0x0a
    u8 field8_0xb;                    // 0x0b
    void (*unlock_fn)(void);          // 0x0c
    char *floor_target_door_name;     // 0x10
    char *custodian_locator_set;      // 0x14
    f32 offset_floor_target;          // 0x18
    struct DOOR_s *floor_target_door; // 0x1c
    i16 *id;                          // 0x20
    NUVEC custodian_position;         // 0x24
    u16 custodian_angle;              // 0x30
    u8 field44_0x32;                  // 0x32
    u8 field45_0x33;                  // 0x33
};

struct PLAYERCHARACTERCONFIG_s { // size 0x118
    u8 unknown_00[48];           // 0x000
    f32 reset_scale;             // 0x030
    u8 unknown_34[24];           // 0x034
    f32 collision_origin_radius; // 0x04c
    u8 unknown_50[60];           // 0x050
    f32 shadow_radius;           // 0x08c
    u32 flags_090;               // 0x090
    u32 flags_094;               // 0x094
    u8 unknown_98[88];           // 0x098
    u16 shadow_joint_mask;       // 0x0f0
    u8 unknown_f2[4];            // 0x0f2
    u8 blob_shadow_alpha;        // 0x0f6
    u8 unknown_f7;               // 0x0f7
    i8 weapon_joints[4];         // 0x0f8
    i8 weapon_shoot_joints[4];   // 0x0fc
    u8 unknown_100[8];           // 0x100
    i8 hand_joints[2];           // 0x108
    i8 grapple_joint_a;          // 0x10a
    i8 grapple_joint_b;          // 0x10b
    u8 unknown_10c;              // 0x10c
    i8 collision_origin_joint;   // 0x10d
    u8 unknown_10e;              // 0x10e
    i8 model_origin_joint;       // 0x10f
    u8 unknown_110;              // 0x110
    i8 helmet_locator;           // 0x111

    union { // size 0x5
        u8 unknown_112[5];

        struct {               // size 0x5
            i8 batarang_joint; // 0x112
            u8 unknown_113[4]; // 0x113
        };
    }; // 0x112

    u8 variant; // 0x117
};

struct CUSTOMPIECE {       // size 0x28
    char *name;            // 0x00
    i16 character_id;      // 0x04
    i16 icon_character_id; // 0x06

    union { // size 0xa
        u8 unknown_08[10];

        struct {                       // size 0xa
            i16 weapon_model;          // 0x08
            i16 unknown_indices_0a[3]; // 0x0a
            i8 unknown_10;             // 0x10
            i8 collection_type;        // 0x11
        };
    }; // 0x08

    union { // size 0x2

        struct {                        // size 0x2
            u8 layer_flags;             // 0x12
            u8 availability_flags_high; // 0x13
        };

        u16 availability_flags;
    }; // 0x12

    u32 model_flags;    // 0x14
    u32 gameplay_flags; // 0x18
    u8 unknown_1c[12];  // 0x1c
};

struct CUSTOMPIECECATEGORY { // size 0xc
    char *name;              // 0x0
    u8 uses_special;         // 0x4
    i8 material_tag;         // 0x5
    u8 pad_06[2];            // 0x6
    char *shared_scene;      // 0x8
};

struct rtldata_s { // size 0x144
    RTL_DATA base; // 0x000
};

struct CHARACTER_SHADOW_s { // size 0x14
    NUVEC position;         // 0x00
    u16 x_rotation;         // 0x0c
    u16 z_rotation;         // 0x0e
    f32 opacity;            // 0x10
};

struct PLAYERPACKET_s {                             // size 0x798
    struct CHARACTER_SHADOW_s character_shadows[5]; // 0x000
    u8 pad_064[132];                                // 0x064
    u16 context_animation;                          // 0x0e8
    u8 pad_0ea[4];                                  // 0x0ea
    u8 context_mode;                                // 0x0ee
    u8 pad_0ef[2];                                  // 0x0ef
    i8 build_context;                               // 0x0f1
    u8 pad_0f2[2];                                  // 0x0f2
    u8 action_movement_state;                       // 0x0f4
    u8 action_movement_variant;                     // 0x0f5
    u8 pad_0f6[6];                                  // 0x0f6
    f32 context_animation_time;                     // 0x0fc
    i8 context_target;                              // 0x100
    u8 context_state_flags;                         // 0x101
    u8 pad_102[1234];                               // 0x102
    NUVEC reset_up_direction;                       // 0x5d4
    struct GAMEPAD *gamepad;                        // 0x5e0
    u8 pad_5e4[32];                                 // 0x5e4
    void *unknown_604;                              // 0x604
    u8 pad_608[4];                                  // 0x608
    struct gameobject_s *linked_object;             // 0x60c
    u8 pad_610[4];                                  // 0x610
    u32 unknown_614;                                // 0x614
    u32 unknown_618;                                // 0x618
    u32 unknown_61c;                                // 0x61c
    u32 unknown_620;                                // 0x620
    u32 unknown_624;                                // 0x624
    u32 unknown_628;                                // 0x628
    u32 unknown_62c;                                // 0x62c
    u32 unknown_630;                                // 0x630
    u8 pad_634[20];                                 // 0x634
    u32 unknown_648;                                // 0x648
    u8 pad_64c[8];                                  // 0x64c

    union { // size 0x4
        i32 force_glow_mode;
        void *force_glow_object;
    }; // 0x654

    union { // size 0x4
        i32 force_glow_state;
        void *force_glow_candidate;
    }; // 0x658

    u8 pad_65c[4];            // 0x65c
    u32 input_state;          // 0x660
    u32 unknown_664;          // 0x664
    u32 unknown_668;          // 0x668
    u8 pad_66c[4];            // 0x66c
    u32 unknown_670;          // 0x670
    u32 unknown_674;          // 0x674
    u32 unknown_678;          // 0x678
    f32 context_blend;        // 0x67c
    f32 context_distance;     // 0x680
    u8 pad_684[4];            // 0x684
    u32 unknown_688;          // 0x688
    f32 delayed_turn_timer;   // 0x68c
    u32 unknown_690;          // 0x690
    u32 unknown_694;          // 0x694
    u8 pad_698[4];            // 0x698
    u32 unknown_69c;          // 0x69c
    u32 unknown_6a0;          // 0x6a0
    u32 unknown_6a4;          // 0x6a4
    u8 pad_6a8[32];           // 0x6a8
    u32 unknown_6c8;          // 0x6c8
    f32 force_glow_x;         // 0x6cc
    f32 force_glow_y;         // 0x6d0
    f32 force_glow_intensity; // 0x6d4
    u8 pad_6d8[4];            // 0x6d8
    f32 force_glow_z;         // 0x6dc
    u32 unknown_6e0;          // 0x6e0
    u8 pad_6e4[4];            // 0x6e4
    u32 unknown_6e8;          // 0x6e8
    f32 ground_height;        // 0x6ec
    u8 pad_6f0[8];            // 0x6f0
    u32 unknown_6f8;          // 0x6f8
    u32 unknown_6fc;          // 0x6fc
    u8 pad_700[16];           // 0x700
    u32 unknown_710;          // 0x710
    u8 pad_714[4];            // 0x714
    u32 unknown_718;          // 0x718
    u32 unknown_71c;          // 0x71c
    u32 unknown_720;          // 0x720
    u32 unknown_724;          // 0x724
    u32 unknown_728;          // 0x728
    u8 pad_72c[4];            // 0x72c
    u32 unknown_730;          // 0x730
    u32 unknown_734;          // 0x734
    u8 pad_738[4];            // 0x738
    u32 unknown_73c;          // 0x73c
    u8 pad_740[28];           // 0x740
    i16 movement_angle_0;     // 0x75c
    i16 movement_angle_1;     // 0x75e
    i16 movement_angle_2;     // 0x760
    i16 movement_angle_3;     // 0x762
    i16 movement_lean_angle;  // 0x764
    i16 secondary_lean_angle; // 0x766
    i16 tertiary_lean_angle;  // 0x768
    i16 force_glow_index;     // 0x76a
    u8 movement_flags;        // 0x76c
    u8 secondary_flags;       // 0x76d
    u8 animation_flags;       // 0x76e
    u8 pad_76f;               // 0x76f
    u8 render_flags;          // 0x770
    u8 unknown_771;           // 0x771
    u8 pad_772;               // 0x772
    u8 random_state[4];       // 0x773
    u8 pad_777[6];            // 0x777
    u8 unknown_77d;           // 0x77d
    u8 unknown_77e;           // 0x77e
    u8 pad_77f[13];           // 0x77f
    u8 random_context;        // 0x78c
    u8 pad_78d;               // 0x78d
    i8 surface_type;          // 0x78e
    u8 pad_78f[5];            // 0x78f
    u32 unknown_794;          // 0x794
};

struct HINT_s {              // size 0x24
    i16 control_mode_ids[2]; // 0x00
    u8 flags;                // 0x04
    u8 pad_05[3];            // 0x05
    i32 shop_price;          // 0x08
    f32 display_duration;    // 0x0c
    f32 repeat_delay;        // 0x10

    union { // size 0x4
        i32 (*availability_fn)(struct HINT_s *arg0);
        u8 pad_14[4];
    }; // 0x14

    void (*on_display)(struct HINT_s *arg0); // 0x18
    u8 completion_flags[4];                  // 0x1c
    f32 unknown_20;                          // 0x20
};

struct AREA_GLOBAL_VALUES { // size 0x34
    i32 unknown_00;         // 0x00
    i32 unknown_04;         // 0x04
    i32 unknown_08;         // 0x08
    i32 unknown_0c;         // 0x0c
    i32 unknown_10;         // 0x10
    i32 unknown_14;         // 0x14
    i32 unknown_18;         // 0x18
    i32 unknown_1c;         // 0x1c
    i32 unknown_20;         // 0x20

    union { // size 0x8

        struct {            // size 0x8
            i32 unknown_24; // 0x24
            i32 unknown_28; // 0x28
        };

        u32 arcade_player_kills[2];
    }; // 0x24

    union { // size 0x8

        struct {            // size 0x8
            i32 unknown_2c; // 0x2c
            i32 unknown_30; // 0x30
        };

        u32 arcade_ai_kills[2];
    }; // 0x2c
};

struct SCENESPECIALPROGRESS_s { // size 0x8
    u32 frame : 11;             // 0x0:0
    u32 visible : 1;            // 0x0:11
    u32 playing : 1;            // 0x0:12
    u32 waiting : 1;            // 0x0:13
    u32 repeating : 1;          // 0x0:14
    u32 reserved : 17;          // 0x0:15
    f32 tfactor;                // 0x4
};

struct SCENEPROGRESS_s {                          // size 0x2800
    struct SCENESPECIALPROGRESS_s specials[1280]; // 0x0000
};

struct AIAREA_s {  // size 0x3c
    char name[16]; // 0x00

    union { // size 0xc
        NUVEC position;

        struct {       // size 0xc
            f32 min_x; // 0x10
            f32 min_y; // 0x14
            f32 min_z; // 0x18
        };
    }; // 0x10

    union { // size 0xc

        struct {            // size 0xc
            f32 half_width; // 0x1c
            f32 height;     // 0x20
            f32 half_depth; // 0x24
        };

        struct {       // size 0xc
            f32 max_x; // 0x1c
            f32 max_y; // 0x20
            f32 max_z; // 0x24
        };
    }; // 0x1c

    union { // size 0x2
        i16 rotation;
        i16 flags;
    }; // 0x28

    u8 runtime_flags;        // 0x2a
    u8 game_flags;           // 0x2b
    u8 pad_2c[8];            // 0x2c
    struct AISYS_PC *system; // 0x34
    u8 pad_38[4];            // 0x38
};

struct AIPATHCNXCONTROLSYS_s {                 // size 0x18
    i32 controller_count;                      // 0x00
    struct AIPATHCNXCONTROLLER_s *controllers; // 0x04
    NULISTHDR available_controllers;           // 0x08
    NULISTHDR active_controllers;              // 0x10
};

struct nucolour3_s { // size 0xc
    f32 r;           // 0x0
    f32 g;           // 0x4
    f32 b;           // 0x8
};

struct TintStack {              // size 0xc
    struct nucolour3_s ambient; // 0x0
};

struct LEVEL_PROGRESS_DATA_s { // size 0x2800
    u32 words[2560];           // 0x0000
};

struct LEVELSCRIPTPROGRESS_s { // size 0x20
    char name[16];             // 0x00
    f32 params[4];             // 0x10
};

struct LEVEL_PROGRESS_s {              // size 0x2e24
    struct LEVEL_PROGRESS_DATA_s data; // 0x0000

    union { // size 0x4
        i32 flags;

        struct {               // size 0x4
            u8 flags_low;      // 0x2800
            u8 flags_upper[3]; // 0x2801
        };
    }; // 0x2800

    u32 disabled_ai_object_mask[2];           // 0x2804
    f32 grabber_field_0x48c;                  // 0x280c
    f32 grabber_field_0x484;                  // 0x2810
    f32 grabber_field_0x494;                  // 0x2814
    u32 destroyed_trooper_cannon_mask;        // 0x2818
    u32 played_cutscene_mask;                 // 0x281c
    struct LEVELSCRIPTPROGRESS_s scripts[32]; // 0x2820
    GIZFLOWPROGRESS giz_flow_progress;        // 0x2c20
    char disabled_effect_names[12][16];       // 0x2d64
};

struct RUMBLEPACKET {  // size 0x14
    u32 unknown_00;    // 0x00
    f32 rumble_amount; // 0x04
    u32 unknown_08;    // 0x08
    f32 rumble_time;   // 0x0c

    union { // size 0x4
        u32 unknown_10;

        struct {          // size 0x4
            u8 active;    // 0x10
            u8 pad_11[3]; // 0x11
        };
    }; // 0x10
};

struct PARTLIGHTSOURCE_s { // size 0xcc
    u8 reserved_00[120];   // 0x00
    NUVEC values[7];       // 0x78
};

struct PLAYERITEM_s {              // size 0x8
    struct PLAYERITEMTYPE_s *type; // 0x0
    u8 ammunition;                 // 0x4
    u8 unknown_5;                  // 0x5
    u8 reserved[2];                // 0x6
};

struct SHOPINPUT { // size 0x28

    union { // size 0x28
        i32 value[10];

        struct {               // size 0x28
            i32 left_pressed;  // 0x00
            i32 right_pressed; // 0x04
            i32 up_pressed;    // 0x08
            i32 down_pressed;  // 0x0c
            i32 left_held;     // 0x10
            i32 right_held;    // 0x14
            i32 up_held;       // 0x18
            i32 down_held;     // 0x1c
            i32 confirm;       // 0x20
            i32 cancel;        // 0x24
        };
    }; // 0x00
};

struct NuSpecialBoundsDisplayLayout {        // size 0xd0
    NUMTX mtx;                               // 0x00
    NUMTX draw_mtx;                          // 0x40
    NUVEC min;                               // 0x80
    f32 min_w;                               // 0x8c
    NUVEC max;                               // 0x90
    f32 max_w;                               // 0x9c
    NUVEC center;                            // 0xa0
    f32 radius;                              // 0xac
    struct nuclipobject_s *clip_objects;     // 0xb0
    char *name;                              // 0xb4
    u32 flags;                               // 0xb8
    f32 *clip_range;                         // 0xbc
    i32 instance_ix;                         // 0xc0
    struct nuinstanim_s *instance_animation; // 0xc4
    i16 wind_speed;                          // 0xc8
    i16 wind_scale;                          // 0xca
    u32 pad_cc;                              // 0xcc
};

struct NuSpecialLegacyObjectBoundsLayout {          // size 0x3c
    struct NuSpecialLegacyMaterialLink *materials;  // 0x00
    f32 origin_radius;                              // 0x04
    u8 pad_08[4];                                   // 0x08
    NUVEC minimum;                                  // 0x0c
    NUVEC maximum;                                  // 0x18
    NUVEC center;                                   // 0x24
    f32 radius;                                     // 0x30
    u8 pad_34[4];                                   // 0x34
    struct NuSpecialLegacyObjectBoundsLayout *next; // 0x38
};

struct NuSpecialLegacySceneLayout { // size 0x24
    u8 pad_00[24];                  // 0x00
    void **objects;                 // 0x18
    i32 instance_count;             // 0x1c
    u8 *instances;                  // 0x20
};

struct MINEENTRY_s { // size 0x1c
    u32 active;      // 0x00
    NUVEC pos;       // 0x04
    f32 grow;        // 0x10
    f32 grow_alpha;  // 0x14
    u16 rotx;        // 0x18
    u16 roty;        // 0x1a
};

struct MINESYS_s {                    // size 0x748
    struct nuhspecial_s mine_special; // 0x000
    struct MINEENTRY_s mines[64];     // 0x00c
    f32 mine_radius;                  // 0x70c
    f32 spawn_timer;                  // 0x710
    void *nomine_areas[10];           // 0x714
    i16 nomine_count;                 // 0x73c
    i16 mine_debris;                  // 0x73e
    i32 mine_part;                    // 0x740
    f32 update_timer;                 // 0x744
};

struct SOCKSYS {         // size 0x8
    struct sock_s *sock; // 0x0
    i32 count;           // 0x4
};

struct PLAYERSUBOBJ2_s { // size 0x20
    char pad_00[28];     // 0x00
    f32 value;           // 0x1c
};

struct PLAYERSUBOBJ_s { // size 0x28
    char pad_00[36];    // 0x00
    void *unknown_24;   // 0x24
};

struct instNUGCUTRIGID_s {  // size 0x10
    struct NUGSCN_s *scene; // 0x0
    void *special;          // 0x4
    void *display_special;  // 0x8
    u8 state_index;         // 0xc
    u8 visible;             // 0xd
    u8 pad_0e[2];           // 0xe
};

struct CUTSYS {          // size 0xc
    void **cuts;         // 0x0
    i32 count;           // 0x4
    u32 *character_bits; // 0x8
};

struct CLIMBOBJECTSYS_s {          // size 0x8
    struct CLIMBOBJECT_s *objects; // 0x0
    u16 count;                     // 0x4
    u16 capacity;                  // 0x6
};

struct CLIMBOBJECT_s {            // size 0x20
    NUVEC normal;                 // 0x00
    f32 impact_x;                 // 0x0c
    f32 impact_z;                 // 0x10
    struct AIPATHCNX *connection; // 0x14
    struct AIPATH *path;          // 0x18
    u8 flags;                     // 0x1c
    u8 unknown_1d[3];             // 0x1d
};

struct NuPlatform {          // size 0x10
    char *name;              // 0x0
    char *texture_extension; // 0x4
    char *font_extension;    // 0x8
    i32 current_platform;    // 0xc
};

struct PLUG_s {     // size 0x34
    char name[16];  // 0x00
    NUVEC position; // 0x10
    u16 x_rotation; // 0x1c
    u16 y_rotation; // 0x1e
    u16 z_rotation; // 0x20
    u8 enabled;     // 0x22

    union { // size 0x1
        u8 flags;

        struct {                   // size 0x1
            u8 active : 1;         // 0x23:0
            u8 visible : 1;        // 0x23:1
            u8 reserved_flags : 6; // 0x23:2
        };
    }; // 0x23

    u8 reserved[16]; // 0x24
};

struct nuclipbounds_s { // size 0x20
    NUVEC center;       // 0x00
    f32 center_w;       // 0x0c
    NUVEC extent;       // 0x10
    f32 extent_w;       // 0x1c
};

struct nuclipobject_s { // size 0xc
    i32 nmaterials;     // 0x0
    i32 *material_ids;  // 0x4
    i32 *indices;       // 0x8
};

struct NuLegacyInstanceLayout { // size 0x4c
    u8 pad_00[68];              // 0x00

    union { // size 0x1
        u8 flags;

        struct {                       // size 0x1
            u8 visible : 1;            // 0x44:0
            u8 on_screen : 1;          // 0x44:1
            u8 unused_flag_2 : 1;      // 0x44:2
            u8 no_visibility_test : 1; // 0x44:3
            u8 unused_flags_4_7 : 4;   // 0x44:4
        };
    }; // 0x44

    u8 pad_45[3];                   // 0x45
    struct nuinstanim_s *animation; // 0x48
};

struct NuSpecialLegacyLayout { // size 0x4c
    u8 pad_00[64];             // 0x00
    void *instance;            // 0x40
    char *name;                // 0x44
    u32 flags;                 // 0x48
};

#ifdef __cplusplus
extern "C" {
#endif
    extern u32 edpp_page_scene[8];
    extern i32 LEGOMENU_PAUSECUT;
    extern struct gameobject_s *pod_pacemaker;
    extern void *gunship_weapons[4];
    extern struct level_data_s *CLOUDCITYTRAPOUTRO_LDATA;
    extern i16 id_DARTHVADER;
    extern void (*LEGO_SET_SLOWDOWNFN)(struct gameobject_s *arg0);
    extern i32 (*GizObstacle_CheckExcludeFlagsFn)(struct gizobstacle_s *arg0, struct gameobject_s *arg1);
    extern i16 trooper_boltid[2];
    extern i32 grab_screen_image;
    extern u8 Reflections_On;
    extern i32 nudlist_debug_level;
    extern i32 NUSOUND_STREAM_3;
    void aieditor_ClearMainMenu(void);
    void aieditor_cbCancelMainMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void aieditor_cbCancelSaveMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void locatorEditor_cbCancelRenameMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void locatorEditor_cbRenameLocator(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void routeEditor_cbCancelRouteUsers(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void routeEditor_cbSetRouteUsers(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    char *edbitsGetSoundName(i32 sound_type);
    u32 FS_EncodedDateKey(char *name);
    void DebrisStatusNormal(i32 *handle);
    extern i16 tCHALLENGECOMPLETE;
    extern i16 tWINNER;
    extern f32 GIZPANEL_PLAYERPOSLIFT;
    void GizTurret_ReadAnimSetData(struct gameanimobj_s *object, u8 version);
    u8 Store_IsPackUnlocked(i32 pack);
    extern struct level_data_s *PODRACEC_LDATA;
    extern char Door_ExitCameraSplineName[32];
    void PodSprint_InitAISpline(struct WORLDINFO_s *world, void *ai, char *name);
    extern i32 max_nsnipers;
    extern struct gameobject_s *gameobj;
    extern i16 id_MILLENNIUMFALCON;
    void GizBuildIt_TurnOff(struct GIZBUILDIT_s *buildit);
    extern f32 ICONSIZE;
    extern u8 MENUEXITB;
    extern u8 MENUEXITG;
    extern u8 MENUEXITR;
    extern u8 MENULOSTA1;
    extern u8 MENULOSTA2;
    extern u8 MENULOSTB1;
    extern u8 MENULOSTB2;
    extern u8 MENULOSTG1;
    extern u8 MENULOSTG2;
    extern u8 MENULOSTR1;
    extern u8 MENULOSTR2;
    extern f32 MENUTEXTSCALE;
    extern i16 tALLLEVELPROGRESSWILLBELOST;
    extern i16 tALLSUPERSTORYPROGRESSWILLBELOST;
    extern i16 tARCADE_NOWIN;
    extern i16 tCANCEL;
    extern i16 tEXITTOMAP;
    extern i16 tEXITWITHOUTSAVING;
    extern i16 tGAMEPROGRESSWILLBELOST;
    extern i16 tNO;
    extern i16 tQUITGAME;
    extern i16 tRESTARTDEMO2;
    extern i16 tRESTARTLEVEL;
    extern i16 tSAVEANDEXIT;
    extern i16 tYES;
    extern i16 id_PROBEDROID;
    extern f32 ForceBackRadius;
    void edpartFinishMenu(struct eduimenu_s *menu);
    extern f32 CameraZoom;
    extern f32 menu_pulse;
    extern i32 smarttextex_drawmessagebox;
    extern i16 tCONTINUESTORY;
    extern i16 tFINALCHAPTERLOCKED;
    extern i16 tFINISHSTORY;
    extern i16 tMAP;
    extern i16 tNEXTCHAPTERLOCKED;
    extern i16 tRESTART;
    extern const f64 ShopCharacterAmplitude;
    extern struct level_data_s *DAGOBAHA_LDATA;
    extern struct level_data_s *DAGOBAHD_LDATA;
    extern struct level_data_s *DAGOBAHE_LDATA;
    void NuTexAddReference(i32 tex_id, struct NUGSCN_s *scene);
    extern void (*nudebug_handler)(char *arg0);
    void NuHtmlFlush(i32 force);
    void NuRndrPrimUV(f32 u, f32 v);
    extern i32 (*FindAlternativeSpecialObjectFn)(struct AISYS_PC *arg0, nuhspecial_s *arg1);
    extern f32 SpaceRumbleTimer;
    void Hub_MarkCharacterUnavailable(i16 *characters, i16 character);
    extern storepack_s StorePack[11];
    extern i16 id_JABBA;
    extern i16 id_TRAININGREMOTE;
    extern i16 id_WEIRDO1;
    extern i16 id_WEIRDO2;
    extern i16 id_WHIP;
    extern i16 id_WOMPRAT;
    extern i16 id_ATST;
    extern i16 id_JEDISTARFIGHTERREDEP3;
    extern i16 id_MINIIMPERIALSHUTTLE;
    extern i16 id_MINIMILLENNIUMFALCON;
    extern i16 id_MINITIEINTERCEPTOR;
    extern i16 id_CLONEARC;
    extern i16 id_IMPERIALSHUTTLE;
    extern i16 id_NABOOSTARFIGHTER;
    extern i16 id_SNOWSPEEDER;
    extern struct level_data_s *TEMPLESTATUS_LDATA;
    i32 ImplodeGetI(void *buf, i32 size);
    void *NuFramebufferGetFrontBuffer(void);
    extern struct level_data_s *HOTHBATTLED_LDATA;
    extern i32 TERRAINMASK_NONDROID;
    u32 ParseAIPathCnxFlag(char *name);
    void *CreatePodRaceMine(struct nuvec_s *pos);
    void HothBattle_AssignSpawnLocator(struct gameobject_s *object, struct AILOCATOR *locator, struct AILOCATORSET *set);
    extern i16 id_SPEEDERBIKESNOW;
    void eduiMenuEnsureSelection(struct eduimenu_s *menu);
    i32 eduiMenuProcessSelectedItem(struct eduimenu_s *menu, f32 delta_time, struct FRAME_PAD *pad);
    extern u8 hub_makefreeplaylist_addotherid;
    extern i32 hub_new_level;
    void edpartRemoveInstance(struct parttype_lookup_s *type, i32 index);
    extern const char NuFileIopImageName[13];
    void MakePlayCorner(struct gamecamera_s *camera, f32 screen_x, f32 screen_y, struct nuvec_s *near_corner, struct nuvec_s *far_corner);
    void SetPlayPlane(struct KEEP_SCREEN_PLANE *plane, struct nuvec_s *a, struct nuvec_s *b, struct nuvec_s *c, struct nuvec_s *d,
                      struct nuvec_s *edge_a0, struct nuvec_s *edge_a1, struct nuvec_s *edge_b0, struct nuvec_s *edge_b1);
    void edcamSetSpeed(f32 position_x, f32 position_y, f32 position_z, f32 distance);
    extern char nullobjectname[16];
    extern GRAPPLE_DYNAMIC DynamicGrapple[4];
    void Grapples_DrawList(struct WORLDINFO_s *world, struct GRAPPLE_DYNAMIC *grapples, i32 grapple_count, u16 target_rotation,
                           f32 target_alpha);
    extern NUVEC edpart_cam_pos;
    extern struct PC_SCENE_MTL_VIEW *SolidMtl3D;
    extern i16 tCHEAT_SELFDESTRUCT;
    void eduiFntPrintEx(void *font, i32 x, i32 y, i32 alignment, char *format, ...);
    void edanimParamDestroy(i32 parameter_index);
    extern i32 g_loadingCharacterInHub;
    extern f32 KITPOS2Y;
    extern i32 LEGOOBJ_CHARKIT;
    extern i32 LEGOOBJ_MINIKIT;
    f32 GetAspectRatio(void);
    extern f32 LastTouchTime;
    extern i32 subitemselected;
    u8 Text_MenuColourLerp(u8 first, u8 second, f32 amount);
    extern i32 PickUpFlickerFrames;
    extern i32 PickUpFlickerTest;
    extern i32 PickupFlickerFrame;
#ifdef __cplusplus
}
#endif

enum { GIZTORPMACHINE_FLAG_ACTIVE = 1 };

enum { GIZTORPMACHINE_FLAG_VISIBLE = 2 };

enum { GIZTORP_LEVEL_OBJECT_ALTERNATE = 291 };

enum { GIZTORP_LEVEL_OBJECT_PRIMARY = 290 };

enum { GIZPANEL_DRAW_FLAG_ALT_MODEL = 1 };

enum { GIZPANEL_DRAW_FLAG_LEGACY = 4 };

enum { GIZPANEL_DRAW_FLAG_NO_SHADOW_RESET = 2 };

enum { GIZPANEL_FLAG_BADDIE = -128 };

enum { GIZPANEL_FLAG_HIDE_BASE = 32 };

enum { GIZPANEL_FLAG_HIDE_TARGET = 64 };

enum { GIZBUILDIT_AVAILABILITY_HAS_BLOWUP_TYPE = 128 };

enum { GIZBUILDIT_AVAILABILITY_LINKED = 3 };

enum { TECHNO_FLAG_COMPLETE = 8 };

enum { GAMEANIMSET_VISIBILITY_NONE = 0 };

enum { SPINNER_RUNTIME_AT_OUTPUT = 64 };

enum { SPINNER_RUNTIME_AUTO_RETURN = 16 };

enum { SPINNER_RUNTIME_INPUT_DISABLED = 8 };

enum { SPINNER_STATE_ROTATING = 64 };

enum { SPINNER_STATE_STOP_ANIMATION = 32 };

enum { GIZSPINNER_STATE_SHADOW_PLATFORM = 2048 };

enum { APIOBJECT_STATE_FLAG_IGNORE_DOORS = 262144 };

enum { RESETBIT_DOOR_TRANSITION = 8 };

enum { GIZFORCE_CONFIG_RESET_STATE_ON_ACTIVATE = 8 };

enum { GIZFORCE_PROGRESS_ENABLED = 1 };

enum { GIZFORCE_RUNTIME_HAS_PLATFORM = 1 };

enum { GIZFORCE_STATE_DESTROYED_OR_THROWN = 1 };

enum { GIZBUILDIT_STATE_AUTO_BUILD_STATIC_OBJECTS = 64 };

enum { GIZBUILDIT_STATE_REISSUE_REWARD_ON_ACTIVATE = 512 };

enum { HUB_FREEPLAY_MODEL_MINIKIT = 67108864 };

enum { HUB_FREEPLAY_MODEL_VEHICLE = 8192 };

enum { GIZSPINNER_FLAG_HIDE_ARM = 2 };

enum { GIZSPINNER_FLAG_HIDE_BASE = 4 };

enum { SPINNER_RUNTIME_ANIMATION_HIDDEN = 32 };

enum { GAMEOBJECT_EF8_FLAG_KEEP_WEAPON_OUT = 16 };

enum { GIZBUILDIT_AVAILABILITY_INTERACTING = 4 };

enum { GIZFORCE_CONFIG_ALONG_SOCKET = 64 };

enum { GIZFORCE_CONFIG_CAST_SHADOW = 4096 };

enum { GIZFORCE_CONFIG_DRAW_REFLECTION = 4 };

enum { GIZFORCE_PROGRESS_VISIBLE = 2 };

enum { GIZFORCE_RUNTIME_ALONG_SOCKET_HIDDEN = 8 };

enum { GIZBUILDIT_AVAILABILITY_ACTIVE = 1 };

enum { GIZBUILDIT_AVAILABILITY_VISIBLE = 2 };

enum { GIZBUILDIT_RUNTIME_REWARD_RELEASED = 2 };

enum { GIZBUILDIT_STATE_TURN_OFF_WHEN_COMPLETE = 2 };

enum { GIZTORPMACHINE_FLAG_ALTERNATE_MODEL = 4 };

enum { GIZTURRET_RUNTIME_FLAG_BLOWUP_NAME_ID = 1 };

enum { GIZTURRET_RUNTIME_FLAG_ROTATION_SFX_PLAYING = 8 };

enum { NUQFNT_CSMODE_ABSOLUTE = 4 };

enum { NUQFNT_CSMODE_NORMALISED = 3 };
#ifdef __cplusplus
extern "C" {
#endif
    extern const char GameAudioName0075c1ec[9];
    extern const char GameAudioName0075c1f8[10];
    extern const char GameAudioName0075c204[11];
    extern const char GameAudioName0075c210[9];
    extern const char GameAudioName0075c21c[10];
    extern const char GameAudioName0075c228[10];
    extern const char GameAudioName0075c234[11];
    extern const char GameAudioName0075c284[10];
    extern const char GameAudioName0075c3ac[9];
    extern const char GameAudioName0075c3fc[11];
    extern const char GameAudioName0075c408[11];
    extern const char GameAudioName0075c420[10];
    extern const char GameAudioName0075c42c[10];
    extern const char GameAudioName0075c438[12];
    extern const char GameAudioName0075c444[11];
    extern const char GameAudioName0075c490[19];
    extern const char GameAudioName0075c4a4[9];
    extern const char GameAudioName0075c4b0[10];
    extern const char GameAudioName0075c4bc[10];
    extern const char GameAudioName0075c4c8[10];
    extern const char GameAudioName0075c4d4[10];
    extern const char GameAudioName0075c4e0[10];
    extern const char GameAudioName0075c4ec[12];
    extern const char GameAudioName0075c4f8[9];
    extern const char GameAudioName0075c504[10];
    extern const char GameAudioName0075c510[12];
    extern const char GameAudioName0075c51c[11];
    extern const char GameAudioName0075c528[12];
    extern const char MenuVideoAspectErrorFormat[10];
    extern const char MenuVideoAspect1610Format[10];
    extern const char MenuVideoAspect169Format[9];
    extern const char MenuVideoAspect43Format[8];
    extern const char MenuVideoRefreshFormat[9];
    extern const char MenuVideoResolutionFormat[12];
    extern const char MenuEffects16xFormat[8];
    extern const char MenuEffects8xFormat[7];
    extern const char MenuEffects4xFormat[7];
    extern const char MenuEffects2xFormat[7];
    extern const char MenuVideoCountdownFormat[9];

    extern const char ComboSaber1Name[12];
    extern const char ComboSaber2Name[12];
#ifdef __cplusplus
}
#endif
