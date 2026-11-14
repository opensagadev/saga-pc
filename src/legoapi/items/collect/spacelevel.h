#ifndef SAGA_LEGOAPI_ITEMS_COLLECT_SPACELEVEL_H
#define SAGA_LEGOAPI_ITEMS_COLLECT_SPACELEVEL_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#endif

typedef struct anakin_door_s anakin_door_s;
typedef struct anakin_door_setup_s anakin_door_setup_s;
typedef struct spacelevel_flight_group_s spacelevel_flight_group_s;
typedef struct starfighter_s starfighter_s;

struct anakin_door_s {
    NUMTX matrix;
    NUMTX secondary_matrix;
    NUMTX original_matrix;
    NUMTX original_secondary_matrix;
    NUVEC direction;
    f32 unknown_10c;
    struct nuhspecial_s special;
    struct nuhspecial_s secondary_special;
    f32 minimum_offset;
    f32 offset;
    f32 speed;
    f32 unknown_134;
    u8 active;
    u8 flags;
    i16 has_secondary;
    i16 platform_id;
    u8 unknown_13e[2];
};

STATIC_ASSERT(sizeof(struct anakin_door_s) == 320);

struct starfighter_s {
    NUMTX matrix;
    NUVEC4 previous_axis;
    NUVEC4 previous_position;
    NUVEC4 velocity;
    NUVEC4 parent_offset;
    NUVEC4 target_position;
    NUVEC4 movement;
    NUVEC4 reserved_0a0;
    NUVEC4 aim_direction;
    i32 pitch;
    i32 yaw;
    i32 reserved_0c8;
    f32 aim_bias;
    struct spacelevel_flight_group_s *parent;
    struct flight_spline_resource_pc_s *spline;
    f32 spline_progress;
    f32 initial_delay;
    f32 shoot_timer;
    f32 death_timer;
    f32 death_rotate_x;
    f32 death_rotate_z;
    f32 scale;
    f32 speed;
    f32 spawn_time;
    i16 draw_flags;
    i16 model_id;
    i16 target_id;
    i16 target_ready;
    struct starfighter_s *target;
    i32 object_id;
    i32 health;
    i32 active;
    i32 reserved_114;
    i32 hit_count;
    i32 fired;
    i32 delete_when_done;
    i32 missile_count;
};

STATIC_ASSERT(sizeof(struct starfighter_s) == 296);

struct spacelevel_flight_group_s {
    NUMTX matrix;
    struct starfighter_s fighters[5];
    NUVEC4 velocity;
    NUVEC4 camera_position;
    NUVEC4 target;
    u8 reserved_638[8];
    i32 active;
    i32 state;
    u32 reset_colour;
    i32 draw_target;
    f32 duration;
    f32 speed;
};

STATIC_ASSERT(sizeof(struct spacelevel_flight_group_s) == 1624);

struct anakin_door_setup_s {
    char *name;
    char *secondary_name;
    f32 initial_offset;
    f32 speed;
    i16 flags;
    u8 unknown_12[2];
    NUVEC direction;
    f32 unknown_20;
    f32 minimum_offset;
};

STATIC_ASSERT(sizeof(struct anakin_door_setup_s) == 40);
#ifdef __cplusplus
extern "C" {
#endif
    extern struct anakin_door_s *AnakinC;
    extern anakin_door_setup_s DoorSetupList[15];
#ifdef __cplusplus
}
#endif

#endif
