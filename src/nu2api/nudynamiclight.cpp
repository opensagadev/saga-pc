#include "saga.h"

STATIC_ASSERT(2 * sizeof(NURNDRLIGHTRECORD) + offsetof(NURNDRLIGHTRECORD, enabled) == 0x808);
extern "C" i32 __cdecl NuDynamicLightIsEnabled(NURNDRLIGHTRECORD *records) {
    return records[2].enabled != 0;
}
extern "C" void __cdecl NuDynamicLightSetParameterf(PC_SPECIAL_SHADOW_LIGHT *light, i32 parameter, f32 value) {
    light->SetParameterfPc(parameter, value);
}
extern "C" void __cdecl NuDynamicLightSetParameteri(PC_SPECIAL_SHADOW_LIGHT *light, i32 parameter, i32 value) {
    switch (parameter) {
        case 4:
            light->parameter_4 = value;
            break;
        case 5:
            light->parameter_5 = value;
            break;
    }
}
extern "C" void __cdecl NuDynamicLightAddRenderScene(NURNDRLIGHTRECORD *records, i32 index, i32 scene_id) {
    NURNDRLIGHTRECORD *record = &records[index];
    record->pending[record->count] = scene_id;
    ++record->count;
}

/* Access-only native views; storage remains PC_SPECIAL_SHADOW_LIGHT/NUGSCN. */
struct SHADOW_PRODUCER_SET_PC {
    u8 unknown000[0x280];
    NUVEC4 endpoint0, endpoint1;
    f32 radius;
    struct List {
        NUDISPLAYLIST_PC list;
        u8 unknown028[0x1c];
    } lists[2];
    u8 unknown32c[0x10];
    NUDISPLAYLISTITEM *cursor[2], *first[2], *last[2];
    u8 unknown354[0x2c];
};
STATIC_ASSERT(sizeof(SHADOW_PRODUCER_SET_PC) == 0x380);
STATIC_ASSERT(offsetof(SHADOW_PRODUCER_SET_PC, endpoint0) == 0x280);
STATIC_ASSERT(offsetof(SHADOW_PRODUCER_SET_PC, lists) == 0x2a4);
STATIC_ASSERT(offsetof(SHADOW_PRODUCER_SET_PC, cursor) == 0x33c);
STATIC_ASSERT(offsetof(SHADOW_PRODUCER_SET_PC, first) == 0x344);
STATIC_ASSERT(offsetof(SHADOW_PRODUCER_SET_PC, last) == 0x34c);
struct SHADOW_PRODUCER_BOUNDS_PC {
    NUVEC4 center_radius, extent;
};
STATIC_ASSERT(sizeof(SHADOW_PRODUCER_BOUNDS_PC) == 0x20);
struct SHADOW_PRODUCER_MATERIAL_PC {
    u8 unknown000[0x268];
    u8 alternate;
};
STATIC_ASSERT(offsetof(SHADOW_PRODUCER_MATERIAL_PC, alternate) == 0x268);
/* The local helper's native compiler ABI uses ECX, EDX and three caller-clean stack words. */
static __declspec(noinline) i32 ShadowSegmentClipPc(const NUVEC *point, const NUVEC *end, const NUVEC *start,
                                                    f32 radius0, f32 radius1) {
    NUVEC segment = {end->x - start->x, end->y - start->y, end->z - start->z};
    NUVEC delta = {point->x - start->x, point->y - start->y, point->z - start->z};
    f32 length = (segment.x * segment.x + segment.y * segment.y) + segment.z * segment.z;
    f32 dot = (segment.x * delta.x + segment.y * delta.y) + segment.z * delta.z;
    f32 inverse = 1.0f / length;
    f32 projection = inverse * dot;
    projection *= projection >= 0.0f;
    projection = projection * (projection <= 1.0f) + (projection > 1.0f);
    f32 distance = (delta.x * delta.x + delta.y * delta.y) + delta.z * delta.z;
    distance = distance + projection * projection * length - (dot + dot) * projection;
    f64 sum = (f64)radius0 + radius1;
    f32 sum_squared = sum * sum;
    if (sum_squared < distance)
        return 0;
    f64 difference = (f64)radius0 - radius1;
    f32 difference_squared = difference * difference;
    return difference_squared > distance ? 1 : 2;
}
extern "C" void __cdecl DisplaySceneRndrSpecials(NUDISPLAY_LIST_PC *, i32, i32);
void PC_SPECIAL_SHADOW_LIGHT::RenderSceneShadowPc(NUGSCN *scene) {
    SHADOW_PRODUCER_SET_PC *sets = reinterpret_cast<SHADOW_PRODUCER_SET_PC *>(render_sets);
    for (i32 set = 0; set < render_set_count; ++set)
        for (i32 variant = 0; variant < 2; ++variant)
            sets[set].cursor[variant] = sets[set].first[variant] = sets[set].last[variant] = NULL;
    NUDISPLAY_LIST_PC *display = scene->display_list;
    i32 clip = 0;
    for (i32 instance = 0; clip < display->clip_count; ++instance, ++clip) {
        if ((display->visibility_flags[instance] & 0x21) == 0x21) {
            NUVEC4 *bounds = display->clip_bounds + instance * 2;
            NUVEC delta = {global_camera.mtx.m30 - bounds->x, global_camera.mtx.m31 - bounds->y,
                           global_camera.mtx.m32 - bounds->z};
            if (display->fade_levels[clip] != 0.0f) {
                f32 distance = (delta.x * delta.x + delta.y * delta.y) + delta.z * delta.z;
                while (display->fade_levels[clip] > distance)
                    ++clip;
            }
            PC_DISPLAY_CLIP_OBJECT_PC *object = &display->clip_objects[clip];
            for (i32 set = 0; set < render_set_count; ++set) {
                SHADOW_PRODUCER_BOUNDS_PC *world_bounds =
                    static_cast<SHADOW_PRODUCER_BOUNDS_PC *>(scene->portal_boxes) + instance;
                if (ShadowSegmentClipPc(reinterpret_cast<NUVEC *>(&world_bounds->center_radius),
                                        reinterpret_cast<NUVEC *>(&sets[set].endpoint1),
                                        reinterpret_cast<NUVEC *>(&sets[set].endpoint0), sets[set].radius,
                                        world_bounds->center_radius.w) != 0) {
                    NUVEC minimum = {world_bounds->center_radius.x - world_bounds->extent.x,
                                     world_bounds->center_radius.y - world_bounds->extent.y,
                                     world_bounds->center_radius.z - world_bounds->extent.z};
                    NUVEC maximum = {world_bounds->center_radius.x + world_bounds->extent.x,
                                     world_bounds->center_radius.y + world_bounds->extent.y,
                                     world_bounds->center_radius.z + world_bounds->extent.z};
                    if (reinterpret_cast<PCFrustumClipSet *>(this)->TestBox(&minimum, &maximum, set)) {
                        for (i32 material_index = 0; material_index < object->material_count; ++material_index) {
                            NUMTL *material = scene->materials[object->material_ids[material_index]];
                            i32 variant = reinterpret_cast<SHADOW_PRODUCER_MATERIAL_PC *>(material)->alternate > 0;
                            SHADOW_PRODUCER_SET_PC &record = sets[set];
                            if (record.cursor[variant] >= record.last[variant]) {
                                if (record.cursor[variant] > record.last[variant])
                                    AnimationDiagnosticNoOpPc();
                                NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
                                record.cursor[variant] = reinterpret_cast<NUDISPLAYLISTITEM *>(
                                    (NuDisplayListStreamCursor->addr + 15) & ~15u);
                                NuDisplayListStreamCursor->addr =
                                    ((NuDisplayListStreamCursor->addr + 15) & ~15u) + 0x310;
                                if (record.last[variant] != NULL) {
                                    record.last[variant]->next = record.cursor[variant];
                                    record.last[variant]->type = 0x8d;
                                    record.last[variant]->id = 1;
                                } else
                                    record.first[variant] = record.cursor[variant];
                                record.last[variant] =
                                    reinterpret_cast<NUDISPLAYLISTITEM *>(NuDisplayListStreamCursor->addr - 0x10);
                            }
                            NUDISPLAYLISTITEM *item = record.cursor[variant];
                            item->next = material;
                            item->type = 0x80;
                            item->id = 3;
                            NUDISPLAYLISTITEM *geometry =
                                static_cast<NUDISPLAYLISTITEM *>(display->items) + object->item_indices[material_index];
                            item[1] = geometry[-1];
                            item[2] = geometry[0];
                            item[1].id = 3;
                            item[2].id = 3;
                            record.cursor[variant] += 3;
                        }
                    }
                }
            }
        }
        while (display->fade_levels[clip] != 0.0f)
            ++clip;
    }
    DisplaySceneRndrSpecials(display, 0, 0);
    for (i32 set = 0; set < render_set_count; ++set)
        for (i32 variant = 0; variant < 2; ++variant) {
            SHADOW_PRODUCER_SET_PC &record = sets[set];
            if (record.first[variant] != NULL) {
                NUDISPLAYLIST_PC *list = &record.lists[variant].list;
                NuDisplayListLinkItems(list, 1);
                NUDISPLAYLISTITEM *item = static_cast<NUDISPLAYLISTITEM *>(list->items);
                item->type = 0x80;
                item->id = 3;
                item->next = scene->materials[0];
                list->items = item + 1;
                NUDISPLAYLIST *full_list = reinterpret_cast<NUDISPLAYLIST *>(list);
                full_list->mtl_last->next = record.first[variant];
                record.cursor[variant]->type = 0x8d;
                record.cursor[variant]->next = full_list->dynamic_geometry + 1;
                record.cursor[variant]->id = 1;
                full_list->mtl_last = record.cursor[variant];
            }
        }
}
