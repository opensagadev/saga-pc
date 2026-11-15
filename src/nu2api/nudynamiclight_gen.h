/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nualloc.h"
#include "nu2api/numath/nuplane.h"
#endif

typedef struct pc_frustum_group_s { // size 0x380
    u8 unknown_0000[0x1a0];         // 0x000
    NUPLANE planes[12];             // 0x1a0
    i32 plane_count;                // 0x260
    u8 unknown_0264[0x11c];         // 0x264
} PC_FRUSTUM_GROUP;

struct TERRAIN_IMPACT_RECORD {
    NUVEC position;
    NUVEC normal;
    u8 material[2];
    u8 flags;
    u8 normal_flags;
};

typedef struct debris_material_secondary_s { // size 0x4
    u32 alpha_fail : 2;                      // 0x0:0
    u32 disable_depth_write : 1;             // 0x0:2
    u32 unknown_03_31 : 29;                  // 0x0:3
} DEBRIS_MATERIAL_SECONDARY;

typedef struct debris_material_s {       // size 0x1f4
    u8 unknown_000[0x40];                // 0x000
    DEBRIS_MATERIAL_FLAGS attribs;       // 0x040
    DEBRIS_MATERIAL_SECONDARY secondary; // 0x044
    u8 unknown_048[0xc];                 // 0x048
    NUVEC diffuse;                       // 0x054
    f32 glass_parameter;                 // 0x060
    u8 unknown_064[0xc];                 // 0x064
    f32 opacity;                         // 0x070
    i16 texture_id;                      // 0x074
    i16 sort_priority;                   // 0x076
    u8 unknown_078[0x21];                // 0x078
    i8 particle_type_tag;                // 0x099
    u8 unknown_09a[0x156];               // 0x09a
    u32 vertex_flags;                    // 0x1f0
} DEBRIS_MATERIAL;
