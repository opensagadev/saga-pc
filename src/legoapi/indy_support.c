#include "saga.h"
INDY_GUIDELINE_PC *__cdecl GuideLine_FindNearest(NUVEC *position, WORLDINFO *world, i32 *endpoint, f32 *distance) {
    INDY_GUIDELINE_PC *nearest = NULL, *guide = (INDY_GUIDELINE_PC *)world->guidelines;
    i32 nearest_endpoint = -1, index, side;
    f32 nearest_distance = 1000000000.0f, candidate;
    for (index = 0; index < world->guideline_count; ++index, ++guide) {
        for (side = 0; side < 2; ++side) {
            candidate = NuVecDistSqr(position, &guide->endpoints[side], NULL);
            if (candidate < nearest_distance) {
                nearest_distance = candidate;
                nearest = guide;
                nearest_endpoint = side;
            }
        }
    }
    if (endpoint != NULL)
        *endpoint = nearest_endpoint;
    if (distance != NULL)
        *distance = nearest_distance;
    return nearest;
}

STATIC_ASSERT(offsetof(AIPATH_PC, connection_count) == 0x12);
STATIC_ASSERT(offsetof(AIPATH_PC, connections) == 0x80);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, objects) == 0);
