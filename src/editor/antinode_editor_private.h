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

struct EDANTINODE_s {
    NULISTLNK link;
    NUVEC position;
    f32 radius;
    f32 lower_height;
    f32 upper_height;
    struct nuhspecial_s special;
    NUVEC special_position;
    i32 rotation_offset;
    i32 flags;
    f32 base_radius;
    f32 base_height;
    u8 game_flags;
    u8 type;
    u8 unknown_4a[2];
};

STATIC_ASSERT(sizeof(struct EDANTINODE_s) == 76);
