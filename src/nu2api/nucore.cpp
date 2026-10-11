#include "saga.h"

bool NuDeferredFilterGen::isEnabled() {
    return enabled && (dynamic_light_count > 0 || deferred_geometry_count > 0);
}
void NuDeferredFilterGen::resetAll() {
    for (i32 i = 0; i < dynamic_light_count; ++i)
        dynamic_lights[i]->resetGeometry();
    deferred_geometry_count = 0;
    dynamic_light_count = 0;
}
/* Original nudynamiclight/nucore helper; proposed named fields are documented
 * in light-reset-field-contract.json. No byte-cast/unknown-layout bypass. */
void NuDynamicLight::resetGeometry() {
    for (i32 i = 0; i < render_set_count_pc; ++i) {
        NURNDRLIGHTRECORD &set = render_sets_pc[i];
        set.count = 0;
        NuDisplayListResetStateCache(set.shadow_lists_pc[0].state);
        set.shadow_lists_pc[0].material_last = set.shadow_lists_pc[0].first;
        set.shadow_lists_pc[0].first->type = 0x8d;
        set.shadow_lists_pc[0].first->next = NULL;
        set.shadow_lists_pc[0].first->id = 1;
        NuDisplayListResetStateCache(set.shadow_lists_pc[1].state);
        set.shadow_lists_pc[1].material_last = set.shadow_lists_pc[1].first;
        set.shadow_lists_pc[1].first->type = 0x8d;
        set.shadow_lists_pc[1].first->next = NULL;
        set.shadow_lists_pc[1].first->id = 1;
    }
}
extern "C" void __cdecl NuOcclusionManagerInit(u32 capacity, VARIPTR *buffer, VARIPTR buffer_end) {
    g_OcclusionManager.Init(capacity, buffer, buffer_end);
}
