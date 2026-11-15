#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

typedef struct SHARD_s SHARD_s;

struct SHARD_s {   // size 0x54
    char name[16]; // 0x00

    union { // size 0x44
        char reserved_10[68];

        struct {            // size 0x44
            NUVEC position; // 0x10

            union { // size 0xc
                u8 reserved_1c[12];
                NUVEC current_position;
            }; // 0x1c

            NUVEC screen_position; // 0x28

            union { // size 0x2
                u8 reserved_34[2];
                i16 model_index;
            }; // 0x34

            u16 angle_x; // 0x36
            u16 angle_z; // 0x38

            union { // size 0x2
                u8 reserved_3a[2];
                u16 spin_angle;
            }; // 0x3a

            union { // size 0x1
                u8 state_flags;

                struct {                   // size 0x1
                    u8 active : 1;         // 0x3c:0
                    u8 visible : 1;        // 0x3c:1
                    u8 collecting : 1;     // 0x3c:2
                    u8 collected : 1;      // 0x3c:3
                    u8 drawn : 1;          // 0x3c:4
                    u8 random_offset : 1;  // 0x3c:5
                    u8 tumble_x : 1;       // 0x3c:6
                    u8 reserved_state : 1; // 0x3c:7
                };
            }; // 0x3c

            u8 reserved_3d[3];              // 0x3d
            f32 collection_time;            // 0x40
            struct gameobject_s *collector; // 0x44
            NUVEC collection_velocity;      // 0x48
        };
    }; // 0x10
};
