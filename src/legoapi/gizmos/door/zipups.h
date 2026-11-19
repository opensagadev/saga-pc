#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct WORLDINFO_s;
struct gameobject_s;
struct numtl_s;

typedef struct zipup_s ZIPUP;
#ifdef __cplusplus
extern "C" {
#endif
    struct addgizmotype_s *ZipUps_RegisterGizmo(i32 type_id);
    void InitRopeMtl(char *name, VARIPTR *buffer, VARIPTR *buffer_end);
    extern i32 zipup_gizmotype_id;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void DrawRopeSingle(NUVEC *start, NUVEC *end, f32 amount, struct numtl_s *material, f32 time, f32 grow_time, f32 spacing, f32 scale);
    extern struct numtl_s *ropemtl;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    ZIPUP *ZipUp_FindNearest(struct WORLDINFO_s *, NUVEC *, f32, f32 *, i32 *, struct gameobject_s *);
#ifdef __cplusplus
}
#endif
enum ZIPUP_FLAGS {
    ZIPUP_FLAG_CONFIG_0 = 1,
    ZIPUP_FLAG_CONFIG_1 = 2,
    ZIPUP_FLAG_CONFIG_2 = 4,
    ZIPUP_FLAG_CONFIG_3 = 8,
    ZIPUP_FLAG_CONFIG_4 = 0x10,
    ZIPUP_FLAG_CONFIG_5 = 0x20,
    ZIPUP_FLAG_ACTIVE = 0x40,
    ZIPUP_FLAG_VISIBLE = 0x80,
    ZIPUP_FLAG_IN_USE = 0x100 /* runtime: a character rides it (attached by the zip-up start or the AI) */
};

struct zipup_s {   // size 0x7c
    char name[16]; // 0x00

    union { // size 0x24
        NUVEC endpoints[3];

        struct {                  // size 0x24
            NUVEC lower_position; // 0x10
            NUVEC hook_origin;    // 0x1c
            NUVEC upper_position; // 0x28
        };
    }; // 0x10

    u16 hook_x_rotation;           // 0x34
    u16 hook_y_rotation;           // 0x36
    NUVEC hook_position;           // 0x38
    NUVEC rider_start_offset;      // 0x44
    NUVEC rider_target_position;   // 0x50
    struct gameobject_s *occupant; // 0x5c
    u16 pitch_adjustment;          // 0x60
    u16 direction;                 // 0x62
    u16 facing_angle;              // 0x64
    u8 unknown_66[2];              // 0x66

    union { // size 0x2
        u16 flags_word;

        struct {              // size 0x2
            u8 flags;         // 0x68
            u8 runtime_flags; // 0x69
        };

        struct {            // size 0x2
            u16 bit_01 : 1; // 0x68:0
            u16 bit_02 : 1; // 0x68:1
            u16 bit_04 : 1; // 0x68:2
            u16 bit_08 : 1; // 0x68:3
            u16 bit_10 : 1; // 0x68:4
            u16 bit_20 : 1; // 0x68:5
        } config;

        struct {                  // size 0x2
            u16 unknown_bits : 6; // 0x68:0
            u16 active : 1;       // 0x68:6
            u16 visible : 1;      // 0x68:7
        };
    }; // 0x68

    u8 unknown_6a[2]; // 0x6a

    union { // size 0x10

        struct {                         // size 0x10
            f32 lower_ground_height;     // 0x6c
            f32 upper_ground_height;     // 0x70
            u16 lower_ground_x_rotation; // 0x74
            u16 upper_ground_x_rotation; // 0x76
            u16 lower_ground_z_rotation; // 0x78
            u16 upper_ground_z_rotation; // 0x7a
        };

        struct {                       // size 0x10
            f32 ground_heights[2];     // 0x6c
            u16 ground_x_rotations[2]; // 0x74
            u16 ground_z_rotations[2]; // 0x78
        };
    }; // 0x6c
};
