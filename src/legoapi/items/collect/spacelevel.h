#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"

typedef struct anakin_door_s anakin_door_s;
typedef struct anakin_door_setup_s anakin_door_setup_s;
typedef struct spacelevel_flight_group_s spacelevel_flight_group_s;
typedef struct starfighter_s starfighter_s;

struct anakin_door_s {                     // size 0x140
    NUMTX matrix;                          // 0x000
    NUMTX secondary_matrix;                // 0x040
    NUMTX original_matrix;                 // 0x080
    NUMTX original_secondary_matrix;       // 0x0c0
    NUVEC direction;                       // 0x100
    f32 unknown_10c;                       // 0x10c
    struct nuhspecial_s special;           // 0x110
    struct nuhspecial_s secondary_special; // 0x11c
    f32 minimum_offset;                    // 0x128
    f32 offset;                            // 0x12c
    f32 speed;                             // 0x130
    f32 unknown_134;                       // 0x134
    u8 active;                             // 0x138
    u8 flags;                              // 0x139
    i16 has_secondary;                     // 0x13a
    i16 platform_id;                       // 0x13c
    u8 unknown_13e[2];                     // 0x13e
};

struct starfighter_s {                        // size 0x128
    NUMTX matrix;                             // 0x000
    NUVEC4 previous_axis;                     // 0x040
    NUVEC4 previous_position;                 // 0x050
    NUVEC4 velocity;                          // 0x060
    NUVEC4 parent_offset;                     // 0x070
    NUVEC4 target_position;                   // 0x080
    NUVEC4 movement;                          // 0x090
    NUVEC4 reserved_0a0;                      // 0x0a0
    NUVEC4 aim_direction;                     // 0x0b0
    i32 pitch;                                // 0x0c0
    i32 yaw;                                  // 0x0c4
    i32 reserved_0c8;                         // 0x0c8
    f32 aim_bias;                             // 0x0cc
    struct spacelevel_flight_group_s *parent; // 0x0d0
    struct flight_spline_resource_s *spline;  // 0x0d4
    f32 spline_progress;                      // 0x0d8
    f32 initial_delay;                        // 0x0dc
    f32 shoot_timer;                          // 0x0e0
    f32 death_timer;                          // 0x0e4
    f32 death_rotate_x;                       // 0x0e8
    f32 death_rotate_z;                       // 0x0ec
    f32 scale;                                // 0x0f0
    f32 speed;                                // 0x0f4
    f32 spawn_time;                           // 0x0f8
    i16 draw_flags;                           // 0x0fc
    i16 model_id;                             // 0x0fe
    i16 target_id;                            // 0x100
    i16 target_ready;                         // 0x102
    struct starfighter_s *target;             // 0x104
    i32 object_id;                            // 0x108
    i32 health;                               // 0x10c
    i32 active;                               // 0x110
    i32 reserved_114;                         // 0x114
    i32 hit_count;                            // 0x118
    i32 fired;                                // 0x11c
    i32 delete_when_done;                     // 0x120
    i32 missile_count;                        // 0x124
};

struct spacelevel_flight_group_s {    // size 0x658
    NUMTX matrix;                     // 0x000
    struct starfighter_s fighters[5]; // 0x040
    NUVEC4 velocity;                  // 0x608
    NUVEC4 camera_position;           // 0x618
    NUVEC4 target;                    // 0x628
    u8 reserved_638[8];               // 0x638
    i32 active;                       // 0x640
    i32 state;                        // 0x644
    u32 reset_colour;                 // 0x648
    i32 draw_target;                  // 0x64c
    f32 duration;                     // 0x650
    f32 speed;                        // 0x654
};

struct anakin_door_setup_s { // size 0x28
    char *name;              // 0x00
    char *secondary_name;    // 0x04
    f32 initial_offset;      // 0x08
    f32 speed;               // 0x0c
    i16 flags;               // 0x10
    u8 unknown_12[2];        // 0x12
    NUVEC direction;         // 0x14
    f32 unknown_20;          // 0x20
    f32 minimum_offset;      // 0x24
};

#ifdef __cplusplus
extern "C" {
#endif
    extern struct anakin_door_s *AnakinC;
    extern anakin_door_setup_s DoorSetupList[15];
#ifdef __cplusplus
}
#endif
