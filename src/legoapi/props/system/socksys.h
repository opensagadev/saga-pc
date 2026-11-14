/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART
#define SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART == 1 && !defined(SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_STOP)

struct NUGSCN_s;
struct nugspline_s;
struct numtx_s;
struct nuspecial_handle_s;

typedef struct sockrot_pc_s {
    u16 x;
    u16 y;
} SOCKROT_PC;

STATIC_ASSERT(sizeof(SOCKROT_PC) == 4);
STATIC_ASSERT(offsetof(SOCKROT_PC, x) == 0);
STATIC_ASSERT(offsetof(SOCKROT_PC, y) == 2);

typedef struct socksegment_pc_s {
    NUVEC min;
    NUVEC max;
    NUVEC planes[6];
    NUVEC midpoint;
    NUVEC next_midpoint;
    f32 length;
    f32 distance_from_start;
} SOCKSEGMENT_PC;

STATIC_ASSERT(sizeof(SOCKSEGMENT_PC) == 0x80);
STATIC_ASSERT(offsetof(SOCKSEGMENT_PC, min) == 0);
STATIC_ASSERT(offsetof(SOCKSEGMENT_PC, max) == 0xc);
STATIC_ASSERT(offsetof(SOCKSEGMENT_PC, planes) == 0x18);
STATIC_ASSERT(offsetof(SOCKSEGMENT_PC, midpoint) == 0x60);
STATIC_ASSERT(offsetof(SOCKSEGMENT_PC, next_midpoint) == 0x6c);
STATIC_ASSERT(offsetof(SOCKSEGMENT_PC, length) == 0x78);
STATIC_ASSERT(offsetof(SOCKSEGMENT_PC, distance_from_start) == 0x7c);

typedef struct sockposition_pc_s {
    u8 unknown_00;
    i8 sock_index;
    i16 segment;
    f32 ratio;
    NUVEC midpoint;
    NUVEC camera_position;
    SOCKROT_PC camera_rotation;

    union {
        SOCKROT_PC midpoint_rotation;

        struct {
            u16 midpoint_pitch;
            u16 heading;
        };
    };

    i16 next_segment;
    i8 candidate_count;
    u8 flags;
    f32 distance;
    f32 normalized_distance;
    u32 candidate_mask;
} SOCKPOSITION_PC;

STATIC_ASSERT(sizeof(SOCKPOSITION_PC) == 0x38);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, unknown_00) == 0x0);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, sock_index) == 0x1);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, segment) == 0x2);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, ratio) == 0x4);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, midpoint) == 0x8);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, camera_position) == 0x14);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, camera_rotation) == 0x20);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, midpoint_rotation) == 0x24);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, midpoint_pitch) == 0x24);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, heading) == 0x26);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, next_segment) == 0x28);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, candidate_count) == 0x2a);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, flags) == 0x2b);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, distance) == 0x2c);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, normalized_distance) == 0x30);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, candidate_mask) == 0x34);
#ifdef __cplusplus
extern "C" {
#endif
    void ComplexSockAngles(SOCKROT_PC *);
    f32 RatioBetweenPlanes(NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUVEC *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART
#define SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART == 2 && !defined(SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_STOP)

typedef struct sock_pc_s {
    struct nugspline_s *cam;
    struct nugspline_s *a;
    struct nugspline_s *b;
    struct nugspline_s *c;
    struct nugspline_s *d;
    struct nugspline_s *mid;
    struct nugspline_s *left;
    struct nugspline_s *right;
    struct nugspline_s *look;
    struct nugspline_s *lateral;
    struct nugspline_s *trackin;
    struct nugspline_s *limit;
    u16 length;
    u8 valid;

    union {
        u8 unknown_33;
        u8 looping;
    };
    struct socksegment_pc_s *segments;
    struct sockrot_pc_s *cam_rotations;
    struct sockrot_pc_s *mid_rotations;
    NUVEC min;
    NUVEC max;
    NUVEC center;
    f32 extent;
    u16 flags;
    u8 unknown_6a;
    u8 unknown_6b;
    u8 unknown_6c;
    u8 look_ahead_segments;
    u16 input_yaw;

    union {
        struct {
            u8 unknown_70;
            u8 unknown_71;
            u8 unknown_72;
            u8 unknown_73;
        };

        f32 current_speed;
    };

    union {
        struct {
            u8 unknown_74;
            u8 unknown_75;
            u8 unknown_76;
            u8 unknown_77;
        };

        f32 mid_force_inner_radius;
    };

    union {
        struct {
            u8 unknown_78;
            u8 unknown_79;
            u8 unknown_7a;
            u8 unknown_7b;
        };

        f32 mid_force_outer_radius;
    };

    union {
        f32 unknown_7c;
        f32 vertical_scale;
    };

    f32 unknown_80;
    f32 unknown_84;
    f32 look_ratio_xz;
    f32 look_ratio_y;
    f32 camera_position_seek;
    f32 camera_angle_seek;
    f32 unknown_98;
    f32 camera_local_x_ratio;
    f32 camera_vertical_ratio;
    f32 camera_lateral_ratio;
    NUVEC camera_arena_offset;
    f32 camera_rail_offset;
    NUVEC camera_arena_blend;
    f32 camera_distance_to_target;
    f32 camera_pullback_ratio;
    f32 single_player_pullback;
    f32 two_player_pullback;
    f32 camera_height_above_ground;
    f32 camera_shake;
    u32 overlap_exclusion_mask[2];
    char name[16];
    f32 overlap_blend_ratio;
    struct nuspecial_handle_s *objects;
    u16 object_count;
    u8 unknown_fe;
    u8 unknown_ff;

    struct {
        u8 value;
        i8 edge;
    } blend_entries[8];

    union {
        u32 unknown_110;
        u32 blend_count;
    };

    union {
        u8 unknown_114[40];

        struct {
            f32 camera_character_close_lift;
            f32 camera_min_distance;
            f32 camera_range_of_effect;
            f32 camera_tilt_angle_change;
            f32 camera_tilt_angle_rate;
            f32 camera_tilt_distance;
            f32 camera_tilt_height;
            f32 manual_camera_max_x;
            f32 manual_camera_max_y;
            i32 terrain_camera_inactive;
        };
    };
} SOCK_PC;

STATIC_ASSERT(sizeof(SOCK_PC) == 0x13c);
STATIC_ASSERT(offsetof(SOCK_PC, cam) == 0x0);
STATIC_ASSERT(offsetof(SOCK_PC, a) == 0x4);
STATIC_ASSERT(offsetof(SOCK_PC, b) == 0x8);
STATIC_ASSERT(offsetof(SOCK_PC, c) == 0xc);
STATIC_ASSERT(offsetof(SOCK_PC, d) == 0x10);
STATIC_ASSERT(offsetof(SOCK_PC, mid) == 0x14);
STATIC_ASSERT(offsetof(SOCK_PC, left) == 0x18);
STATIC_ASSERT(offsetof(SOCK_PC, right) == 0x1c);
STATIC_ASSERT(offsetof(SOCK_PC, look) == 0x20);
STATIC_ASSERT(offsetof(SOCK_PC, lateral) == 0x24);
STATIC_ASSERT(offsetof(SOCK_PC, trackin) == 0x28);
STATIC_ASSERT(offsetof(SOCK_PC, limit) == 0x2c);
STATIC_ASSERT(offsetof(SOCK_PC, length) == 0x30);
STATIC_ASSERT(offsetof(SOCK_PC, valid) == 0x32);
STATIC_ASSERT(offsetof(SOCK_PC, segments) == 0x34);
STATIC_ASSERT(offsetof(SOCK_PC, cam_rotations) == 0x38);
STATIC_ASSERT(offsetof(SOCK_PC, mid_rotations) == 0x3c);
STATIC_ASSERT(offsetof(SOCK_PC, min) == 0x40);
STATIC_ASSERT(offsetof(SOCK_PC, max) == 0x4c);
STATIC_ASSERT(offsetof(SOCK_PC, center) == 0x58);
STATIC_ASSERT(offsetof(SOCK_PC, extent) == 0x64);
STATIC_ASSERT(offsetof(SOCK_PC, flags) == 0x68);
STATIC_ASSERT(offsetof(SOCK_PC, unknown_6a) == 0x6a);
STATIC_ASSERT(offsetof(SOCK_PC, unknown_6b) == 0x6b);
STATIC_ASSERT(offsetof(SOCK_PC, unknown_6c) == 0x6c);
STATIC_ASSERT(offsetof(SOCK_PC, look_ahead_segments) == 0x6d);
STATIC_ASSERT(offsetof(SOCK_PC, input_yaw) == 0x6e);
STATIC_ASSERT(offsetof(SOCK_PC, current_speed) == 0x70);
STATIC_ASSERT(offsetof(SOCK_PC, mid_force_inner_radius) == 0x74);
STATIC_ASSERT(offsetof(SOCK_PC, mid_force_outer_radius) == 0x78);
STATIC_ASSERT(offsetof(SOCK_PC, unknown_7c) == 0x7c);
STATIC_ASSERT(offsetof(SOCK_PC, unknown_80) == 0x80);
STATIC_ASSERT(offsetof(SOCK_PC, unknown_84) == 0x84);
STATIC_ASSERT(offsetof(SOCK_PC, look_ratio_xz) == 0x88);
STATIC_ASSERT(offsetof(SOCK_PC, look_ratio_y) == 0x8c);
STATIC_ASSERT(offsetof(SOCK_PC, camera_position_seek) == 0x90);
STATIC_ASSERT(offsetof(SOCK_PC, camera_angle_seek) == 0x94);
STATIC_ASSERT(offsetof(SOCK_PC, unknown_98) == 0x98);
STATIC_ASSERT(offsetof(SOCK_PC, camera_local_x_ratio) == 0x9c);
STATIC_ASSERT(offsetof(SOCK_PC, camera_vertical_ratio) == 0xa0);
STATIC_ASSERT(offsetof(SOCK_PC, camera_lateral_ratio) == 0xa4);
STATIC_ASSERT(offsetof(SOCK_PC, camera_arena_offset) == 0xa8);
STATIC_ASSERT(offsetof(SOCK_PC, camera_rail_offset) == 0xb4);
STATIC_ASSERT(offsetof(SOCK_PC, camera_arena_blend) == 0xb8);
STATIC_ASSERT(offsetof(SOCK_PC, camera_distance_to_target) == 0xc4);
STATIC_ASSERT(offsetof(SOCK_PC, camera_pullback_ratio) == 0xc8);
STATIC_ASSERT(offsetof(SOCK_PC, single_player_pullback) == 0xcc);
STATIC_ASSERT(offsetof(SOCK_PC, two_player_pullback) == 0xd0);
STATIC_ASSERT(offsetof(SOCK_PC, camera_height_above_ground) == 0xd4);
STATIC_ASSERT(offsetof(SOCK_PC, camera_shake) == 0xd8);
STATIC_ASSERT(offsetof(SOCK_PC, overlap_exclusion_mask) == 0xdc);
STATIC_ASSERT(offsetof(SOCK_PC, name) == 0xe4);
STATIC_ASSERT(offsetof(SOCK_PC, overlap_blend_ratio) == 0xf4);
STATIC_ASSERT(offsetof(SOCK_PC, objects) == 0xf8);
STATIC_ASSERT(offsetof(SOCK_PC, object_count) == 0xfc);
STATIC_ASSERT(offsetof(SOCK_PC, unknown_fe) == 0xfe);
STATIC_ASSERT(offsetof(SOCK_PC, unknown_ff) == 0xff);
STATIC_ASSERT(offsetof(SOCK_PC, blend_entries) == 0x100);
STATIC_ASSERT(offsetof(SOCK_PC, camera_character_close_lift) == 0x114);
STATIC_ASSERT(offsetof(SOCK_PC, camera_min_distance) == 0x118);
STATIC_ASSERT(offsetof(SOCK_PC, camera_range_of_effect) == 0x11c);
STATIC_ASSERT(offsetof(SOCK_PC, camera_tilt_angle_change) == 0x120);
STATIC_ASSERT(offsetof(SOCK_PC, camera_tilt_angle_rate) == 0x124);
STATIC_ASSERT(offsetof(SOCK_PC, camera_tilt_distance) == 0x128);
STATIC_ASSERT(offsetof(SOCK_PC, camera_tilt_height) == 0x12c);
STATIC_ASSERT(offsetof(SOCK_PC, manual_camera_max_x) == 0x130);
STATIC_ASSERT(offsetof(SOCK_PC, manual_camera_max_y) == 0x134);
STATIC_ASSERT(offsetof(SOCK_PC, terrain_camera_inactive) == 0x138);
STATIC_ASSERT(offsetof(SOCK_PC, looping) == 0x33);
STATIC_ASSERT(offsetof(SOCK_PC, blend_count) == 0x110);

typedef struct socksys_pc_s {
    SOCK_PC *socks;
    i32 count;
} SOCKSYS_PC;

STATIC_ASSERT(sizeof(SOCKSYS_PC) == 8);
STATIC_ASSERT(offsetof(SOCKSYS_PC, socks) == 0);
STATIC_ASSERT(offsetof(SOCKSYS_PC, count) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    void SetSockPostion(SOCKSYS_PC *system, SOCKPOSITION_PC *position, i32 index, i32 segment, f32 fraction);
    void SockSys_GenerateData(SOCKSYS_PC *, VARIPTR *, VARIPTR);
    STATIC_ASSERT(offsetof(SOCK_PC, vertical_scale) == 0x7c);
    void SockRotationMatrix(SOCKSYS_PC *, SOCKPOSITION_PC *, struct numtx_s *, i32, i32);
    void ComplexSockPosition(SOCKSYS_PC *, NUVEC *, i32, i32, SOCKPOSITION_PC *);
    f32 MidDistanceFromSockStart(SOCKSYS_PC *, SOCKPOSITION_PC *);
    void MoveSockPosition(SOCKSYS_PC *, SOCKPOSITION_PC *, f32, SOCKPOSITION_PC *);
    i32 SockSysTrackInSplineInfo(SOCKSYS_PC *, SOCKPOSITION_PC *, NUVEC *, f32 *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART
#define SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART == 3 && !defined(SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif

    void SockSysFindInScene(SOCKSYS_PC *, struct NUGSCN_s *);
    SOCKSYS_PC *SockSysInit(VARIPTR *, VARIPTR, struct NUGSCN_s *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART
#define SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART == 4 && !defined(SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void SockSysPointAlongSpline(NUVEC *, struct nugspline_s *, i32, i32, f32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART
#define SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_PROPS_SYSTEM_SOCKSYS_H_STOP
