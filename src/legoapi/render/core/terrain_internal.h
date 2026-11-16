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

/* A polygon vertex into a homogeneous vector (w = 0) for the platform transforms, and back. */
#define TERRAIN_LOAD_VERTEX(vector, vertex) ((vector).x = (vertex).x, (vector).y = (vertex).y, (vector).z = (vertex).z, (vector).w = 0.0f)
#define TERRAIN_STORE_VERTEX(vertex, vector) ((vertex).x = (vector).x, (vertex).z = (vector).z, (vertex).y = (vector).y)
