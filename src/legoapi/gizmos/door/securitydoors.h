/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct SECURITYDOOR_s SECURITYDOOR_s;

struct SECURITYDOOR_s {   // size 0xc0
    NUMTX leaf_matrix[2]; // 0x00
    char name[16];        // 0x80
    NUVEC position;       // 0x90
    i16 platform_id[2];   // 0x9c
    u16 yaw;              // 0xa0

    union { // size 0x2
        u16 flags;

        struct {                    // size 0x2
            u16 active : 1;         // 0xa2:0
            u16 visible : 1;        // 0xa2:1
            u16 opened : 1;         // 0xa2:2
            u16 flag_08 : 1;        // 0xa2:3
            u16 reserved_flags : 4; // 0xa2:4
            u16 state : 8;          // 0xa2:8
        };
    }; // 0xa2

    NUVEC player_position; // 0xa4
    u16 terrain_angle_z;   // 0xb0
    u16 terrain_angle_x;   // 0xb2
    f32 opening;           // 0xb4
    char unknown_b8[8];    // 0xb8
};
