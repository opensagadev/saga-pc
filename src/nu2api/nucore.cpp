#include <new>

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
/* Eight-slot renderer object pools; slot storage keeps the 16-byte stride. */
template <typename T> struct LightObjectPool {
    struct Slot {
        u8 storage[(sizeof(T) + 15) & ~15];
    } slots[8];
    i8 occupied[1];
    i32 next;
    T *allocate();
};
template <typename T> T *LightObjectPool<T>::allocate() {
    i32 i;
    for (i = next; i < 8; ++i) {
        if (((occupied[i >> 3] >> (i & 7)) & 1) == 0) {
            occupied[i >> 3] |= 1 << (i & 7);
            next = (i + 1) % 8;
            return reinterpret_cast<T *>(&slots[i]);
        }
    }
    for (i = 0; i < next; ++i) {
        if (((occupied[i >> 3] >> (i & 7)) & 1) == 0) {
            occupied[i >> 3] |= 1 << (i & 7);
            next = (i + 1) % 8;
            return reinterpret_cast<T *>(&slots[i]);
        }
    }
    return NULL;
}
static LightObjectPool<NUDISPLAYLISTITEM> dlistItemPool = {0};
static LightObjectPool<PC_SPECIAL_SHADOW_LIGHT> dynamicLightPool = {0};
static LightObjectPool<NURNDRLIBSTATE> rndrStatePool = {0};
STATIC_ASSERT(sizeof(dlistItemPool) == 0x88);
STATIC_ASSERT(sizeof(dynamicLightPool) == 0x4188);
STATIC_ASSERT(sizeof(rndrStatePool) == 0x208);

extern "C" void __cdecl NuOcclusionManagerInit(u32 capacity, VARIPTR *buffer, VARIPTR buffer_end) {
    g_OcclusionManager.Init(capacity, buffer, buffer_end);
}

nurndrlightrecord_s::nurndrlightrecord_s() {
    parameter_100 = 0.01f;
    count = 0;
    shadow_plane_count = 0;
    parameter_104 = 0.001f;
    parameter_110 = 1.0f;
    for (i32 i = 0; i < 2; ++i) {
        list_items[i] = dlistItemPool.allocate();
        render_states[i] = rndrStatePool.allocate();
        memset(&shadow_lists_pc[i], 0, sizeof(shadow_lists_pc[i]));
        NUDISPLAYLISTITEM *item = list_items[i];
        shadow_lists_pc[i].first = item;
        item->type = 0x8d;
        item->next = NULL;
        item->id = 1;
        shadow_lists_pc[i].material_last = shadow_lists_pc[i].first;
        shadow_lists_pc[i].state = render_states[i];
        NuDisplayListResetStateCache(shadow_lists_pc[i].state);
        shadow_lists_pc[i].material_last = shadow_lists_pc[i].first;
    }
}
PC_SPECIAL_SHADOW_LIGHT::PC_SPECIAL_SHADOW_LIGHT() {
    enabled = 0;
    render_set_count = 2;
    active_render_set_count = 0;
    parameter_4 = 0;
    parameter_5 = 0;
    used_on_specials = 0;
    parameter_1 = 4.0f;
    parameter_2 = 0.25f;
    parameter_0 = 0.8f;
    parameter_3 = 0.0f;
    parameter_19 = 100.0f;
    parameter_20 = 110.0f;
}
extern "C" PC_SPECIAL_SHADOW_LIGHT *__cdecl NuDynamicLightCreate(void) {
    return new (dynamicLightPool.allocate()) PC_SPECIAL_SHADOW_LIGHT;
}
