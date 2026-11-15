#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

typedef struct TERRAIN_LAST_IMPACT { // size 0x10
    NUVEC position;                  // 0x0
    f32 hit_type;                    // 0xc
} TERRAIN_LAST_IMPACT;

#ifdef __cplusplus
extern "C" {
#endif
    extern TERRAIN_LAST_IMPACT TerrLastImpact;
    extern i32 WallSplinesOnly;
#ifdef __cplusplus
}
#endif
