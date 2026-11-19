#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nualloc.h"
#include "nu2api/numath/nuplane.h"

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
