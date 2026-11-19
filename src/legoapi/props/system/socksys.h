#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct NUGSCN_s;
struct nugspline_s;
struct numtx_s;
struct nuhspecial_s;

typedef struct sockrot_s { // size 0x4
    u16 x;                 // 0x0
    u16 y;                 // 0x2
} SOCKROT;

typedef struct socksegment_s { // size 0x80
    NUVEC min;                 // 0x00
    NUVEC max;                 // 0x0c
    NUVEC planes[6];           // 0x18
    NUVEC midpoint;            // 0x60
    NUVEC next_midpoint;       // 0x6c
    f32 length;                // 0x78
    f32 distance_from_start;   // 0x7c
} SOCKSEGMENT;

typedef struct sockposition_s { // size 0x38
    u8 reserved_00;              // 0x00
    i8 sock_index;              // 0x01
    i16 segment;                // 0x02
    f32 ratio;                  // 0x04
    NUVEC midpoint;             // 0x08
    NUVEC camera_position;      // 0x14
    SOCKROT camera_rotation;    // 0x20

    union { // size 0x4
        SOCKROT midpoint_rotation;

        struct {                // size 0x4
            u16 midpoint_pitch; // 0x24
            u16 heading;        // 0x26
        };
    }; // 0x24

    i16 next_segment;        // 0x28
    i8 candidate_count;      // 0x2a
    u8 flags;                // 0x2b
    f32 distance;            // 0x2c
    f32 normalized_distance; // 0x30
    u32 candidate_mask;      // 0x34
} SOCKPOSITION;

#ifdef __cplusplus
extern "C" {
#endif
    void ComplexSockAngles(SOCKROT *);
    f32 RatioBetweenPlanes(NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUVEC *);
#ifdef __cplusplus
}
#endif

/* sock_s.flags: set by the sock config keywords (the SockPar* handlers of socksys.c). */
#define SOCK_FLAG_NO_CD 0x1                             /* no c and d splines of the b spline's length: the sock runs on a and b alone */
#define SOCK_FLAG_MID_FORCE_Y 0x2                       /* mid_force y */
#define SOCK_FLAG_MID_FORCE_XZ 0x4                      /* mid_force xz (xyz sets and clears it) */
#define SOCK_FLAG_MID_FORCE_DOWN 0x8                    /* mid_force_down */
#define SOCK_FLAG_CLAMP_TARGET_Y 0x20                   /* y_limits */
#define SOCK_FLAG_MOVE_ANGLE 0x40                       /* move_angle */
#define SOCK_FLAG_PROJECT_CAMERA_FROM_PLAYER 0x80       /* cam_behind_setting */
#define SOCK_FLAG_TURN_OFF 0x100                        /* turn_off */
#define SOCK_FLAG_SCALE_LATERAL_OFFSET 0x200            /* cam_scale_lateral */
#define SOCK_FLAG_TWO_PLAYER_PLANAR_PULLBACK 0x400      /* cam_pullback_ratio_xz */
#define SOCK_FLAG_MISC_1 0x800                          /* set_misc_1 */
#define SOCK_FLAG_MISC_2 0x1000                         /* set_misc_2 */
#define SOCK_FLAG_CAMERA_DISTANCE_XZ 0x2000             /* cam_dist_to_target_xz */
#define SOCK_FLAG_TWO_PLAYER_VERTICAL_SEPARATION 0x4000 /* cam_pullback_yref */

typedef struct sock_s {          // size 0x13c
    struct nugspline_s *cam;     // 0x000
    struct nugspline_s *a;       // 0x004
    struct nugspline_s *b;       // 0x008
    struct nugspline_s *c;       // 0x00c
    struct nugspline_s *d;       // 0x010
    struct nugspline_s *mid;     // 0x014
    struct nugspline_s *left;    // 0x018
    struct nugspline_s *right;   // 0x01c
    struct nugspline_s *look;    // 0x020
    struct nugspline_s *lateral; // 0x024
    struct nugspline_s *trackin; // 0x028
    struct nugspline_s *limit;   // 0x02c
    u16 length;                  // 0x030
    u8 valid;                    // 0x032

    union { // size 0x1
        u8 looped;
        u8 looping;
    }; // 0x033
    struct socksegment_s *segments;  // 0x034
    struct sockrot_s *cam_rotations; // 0x038
    struct sockrot_s *mid_rotations; // 0x03c
    NUVEC min;                       // 0x040
    NUVEC max;                       // 0x04c
    NUVEC center;                    // 0x058
    f32 extent;                      // 0x064
    u16 flags;                       // 0x068
    u8 reserved_6a;                   // 0x06a
    u8 reserved_6b;                   // 0x06b
    u8 reserved_6c;                   // 0x06c
    u8 look_ahead_segments;          // 0x06d
    u16 input_yaw;                   // 0x06e

    union { // size 0x4

        struct {           // size 0x4
            u8 reserved_70; // 0x070
            u8 reserved_71; // 0x071
            u8 reserved_72; // 0x072
            u8 reserved_73; // 0x073
        };

        f32 current_speed;
    }; // 0x070

    union { // size 0x4

        struct {           // size 0x4
            u8 reserved_74; // 0x074
            u8 reserved_75; // 0x075
            u8 reserved_76; // 0x076
            u8 reserved_77; // 0x077
        };

        f32 mid_force_inner_radius;
    }; // 0x074

    union { // size 0x4

        struct {           // size 0x4
            u8 reserved_78; // 0x078
            u8 reserved_79; // 0x079
            u8 reserved_7a; // 0x07a
            u8 reserved_7b; // 0x07b
        };

        f32 mid_force_outer_radius;
    }; // 0x078

    union { // size 0x4
        f32 look_scale;
        f32 vertical_scale;
    }; // 0x07c

    f32 look_width;                 // 0x080
    f32 look_height;                // 0x084
    f32 look_ratio_xz;              // 0x088
    f32 look_ratio_y;               // 0x08c
    f32 camera_position_seek;       // 0x090
    f32 camera_angle_seek;          // 0x094
    f32 loop_length;                // 0x098
    f32 camera_local_x_ratio;       // 0x09c
    f32 camera_vertical_ratio;      // 0x0a0
    f32 camera_lateral_ratio;       // 0x0a4
    NUVEC camera_arena_offset;      // 0x0a8
    f32 camera_rail_offset;         // 0x0b4
    NUVEC camera_arena_blend;       // 0x0b8
    f32 camera_distance_to_target;  // 0x0c4
    f32 camera_pullback_ratio;      // 0x0c8
    f32 single_player_pullback;     // 0x0cc
    f32 two_player_pullback;        // 0x0d0
    f32 camera_height_above_ground; // 0x0d4
    f32 camera_shake;               // 0x0d8
    u32 overlap_exclusion_mask[2];  // 0x0dc
    char name[16];                  // 0x0e4
    f32 overlap_blend_ratio;        // 0x0f4
    struct nuhspecial_s *objects;   // 0x0f8
    u16 object_count;               // 0x0fc
    u8 reserved_fe;                  // 0x0fe
    u8 reserved_ff;                  // 0x0ff

    struct {            // size 0x2
        u8 value;       // 0x100
        i8 edge;        // 0x101
    } blend_entries[8]; // 0x100

    union { // size 0x4
        u32 reserved_110;
        u32 blend_count;
    }; // 0x110

    union { // size 0x28
        u8 reserved_114[40];

        struct {                             // size 0x28
            f32 camera_character_close_lift; // 0x114
            f32 camera_min_distance;         // 0x118
            f32 camera_range_of_effect;      // 0x11c
            f32 camera_tilt_angle_change;    // 0x120
            f32 camera_tilt_angle_rate;      // 0x124
            f32 camera_tilt_distance;        // 0x128
            f32 camera_tilt_height;          // 0x12c
            f32 manual_camera_max_x;         // 0x130
            f32 manual_camera_max_y;         // 0x134
            i32 terrain_camera_inactive;     // 0x138
        };
    }; // 0x114
} SOCK;

typedef struct socksys_s { // size 0x8
    SOCK *socks;           // 0x0
    i32 count;             // 0x4
} SOCKSYS;

#ifdef __cplusplus
extern "C" {
#endif
    void SetSockPostion(SOCKSYS *system, SOCKPOSITION *position, i32 index, i32 segment, f32 fraction);
    void SockSys_GenerateData(SOCKSYS *, VARIPTR *, VARIPTR);
    void SockRotationMatrix(SOCKSYS *, SOCKPOSITION *, struct numtx_s *, i32, i32);
    void ComplexSockPosition(SOCKSYS *, NUVEC *, i32, i32, SOCKPOSITION *);
    f32 MidDistanceFromSockStart(SOCKSYS *, SOCKPOSITION *);
    void MoveSockPosition(SOCKSYS *, SOCKPOSITION *, f32, SOCKPOSITION *);
    i32 SockSysTrackInSplineInfo(SOCKSYS *, SOCKPOSITION *, NUVEC *, f32 *);
    SOCK *FindSock(SOCKSYS *sys, char *name);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void SockSysFindInScene(SOCKSYS *, struct NUGSCN_s *);
    SOCKSYS *SockSysInit(VARIPTR *, VARIPTR, struct NUGSCN_s *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void SockSysPointAlongSpline(NUVEC *, struct nugspline_s *, i32, i32, f32);
    extern i32 complexsockposition_forcesock;
    i32 SockBitSet(SOCK *sock, i32 index);
    void SetSockBit(SOCK *sock, i32 index);
    void SockOff(SOCKSYS *sys, i32 index);
    void SockOn(SOCKSYS *sys, i32 index);
#ifdef __cplusplus
}
#endif
