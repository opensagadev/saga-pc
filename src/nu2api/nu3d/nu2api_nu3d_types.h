#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nugeom.h"
#include "nu2api/numath/nuvec.h"

typedef struct nugobj_s {  // size 0x50
    u8 unknown_00[0xc];    // 0x00
    NUGEOM *geom;          // 0x0c
    u8 unknown_10[4];      // 0x10
    f32 origin_radius;     // 0x14
    u8 unknown_18[4];      // 0x18
    NUVEC bounds_min;      // 0x1c
    NUVEC bounds_max;      // 0x28
    NUVEC center;          // 0x34
    f32 radius;            // 0x40
    u8 unknown_44[8];      // 0x44
    struct nugobj_s *next; // 0x4c
} NUGOBJ;
