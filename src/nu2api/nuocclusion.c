#include "saga.h"

void NuOcclusionManagerBeginFrame(void) {
    OCCLUDERSET *completed;
    OCCLUDERSET *building;

    if (g_OcclusionManager.initialized == 0 || g_OcclusionManager.enabled == 0) {
        return;
    }

    completed = g_OcclusionManager.building_set;
    building = g_OcclusionManager.current_set;
    g_OcclusionManager.building_set = building;
    g_OcclusionManager.current_set = completed;
    building->count = 0;
    building->queries_prepared = 0;
    g_OcclusionManager.unknown_0160 = 0;
    g_OcclusionManager.unknown_0164 = 0;
}
