/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct EDANTINODE_s EDANTINODE_s;

struct EDANTINODE_s {            // size 0x4c
    NULISTLNK link;              // 0x00
    NUVEC position;              // 0x08
    f32 radius;                  // 0x14
    f32 lower_height;            // 0x18
    f32 upper_height;            // 0x1c
    struct nuhspecial_s special; // 0x20
    NUVEC special_position;      // 0x2c
    i32 rotation_offset;         // 0x38
    i32 flags;                   // 0x3c
    f32 base_radius;             // 0x40
    f32 base_height;             // 0x44
    u8 game_flags;               // 0x48
    u8 type;                     // 0x49
    u8 unknown_4a[2];            // 0x4a
};
