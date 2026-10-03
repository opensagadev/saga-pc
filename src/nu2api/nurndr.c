#include "saga.h"

#include <string.h>

NURNDRLIBSCENE currentScene;
NUVPCURRENT vpCurrent;
i32 sceneParametersCount;
NURNDRLIBSTATE *displaylist_2d_state;
NUDISPLAYLISTITEM *displaylist_2d_first;
NUDISPLAYLISTITEM *displaylist_2d_mtl_last;
i32 nused_sort_pris;
NUSORTPRI *sort_list;
i32 render_scene_index;
NURNDRLIBSCENE sceneParameters[32];
NUDISPLAYLISTRENDERSCENE *render_scene_ptrs[16];
i32 nudlist_alloc_count;
u32 nudlist_alloc_base;
u32 nudlist_alloc_end;
u32 nudlist_item_base;
i32 nudlist_frame_active;
u32 nudlist_flags;
u32 nudlist_field_e58;
u32 nudlist_field_e80;
u32 nudlist_field_fc8;
i32 nudlist_field_fc4;
u32 nudlist_field_9d0;
u32 nudlist_field_9d8;
u32 nudlist_field_fe4;
NUDISPLAYLISTITEM nudlist_2d_static;
VARIPTR nudlist_stream_cursor;

i32 __cdecl NuRndrBeginScene(i32 begin_flags) {
    currentScene.clear_alpha_4c = 0.0f;
    currentScene.unknown_24 = 0;
    currentScene.unknown_28 = 0;
    currentScene.unknown_58 = 0;
    currentScene.dyn_light_count = 0;
    currentScene.dyn_light_ptr = NULL;
    currentScene.unknown_48 = 0;
    currentScene.unknown_ac = 0;
    currentScene.unknown_c4 = 0;
    currentScene.unknown_e4 = 0;
    currentScene.unknown_e0 = 0;
    currentScene.unknown_178 = 0;
    currentScene.unknown_174 = 0;
    currentScene.unknown_188 = 0;
    currentScene.unknown_214 = 0;
    currentScene.scene_id = -1;
    currentScene.unknown_40 = -1;
    return 1;
}

void __cdecl NuRndrClear(i32 clear_flags, i32 bg_colour, f32 alpha) {
    currentScene.clear_alpha = alpha;
    currentScene.clear_flags_08 |= clear_flags;
    currentScene.bg_colour = bg_colour;
    NuVpGetPosition2(&currentScene.unknown_14, &currentScene.unknown_18);
    currentScene.calc_1c = (f32)(0.0625 * vpCurrent.width / ((f64)DebrisGlassTextureWidth / PS2_VREZ_W));
    currentScene.calc_20 = (f32)(0.0625 * vpCurrent.height / ((f64)DebrisGlassTextureHeight / PS2_VREZ_H));
}

void __cdecl NuRndrGradClear(i32 clear_flags, i32 top_colour, i32 bottom_colour, f32 alpha) {
    NuRndrClear(clear_flags, bottom_colour, alpha);
}

static __forceinline f32 NuFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

void __cdecl NuVpGetPosition2(f32 *x, f32 *y) {
    *x = NuFdiv((f32)vpCurrent.x * 0.0625f - ((f32)PS2_VCNTR_X - (f32)(DebrisGlassTextureWidth >> 1)),
                NuFdiv((f32)DebrisGlassTextureWidth, (f32)PS2_VREZ_W));
    *y = NuFdiv((f32)vpCurrent.y * 0.0625f - ((f32)PS2_VCNTR_Y - (f32)(DebrisGlassTextureHeight >> 1)),
                NuFdiv((f32)DebrisGlassTextureHeight, (f32)PS2_VREZ_H));
}

void __cdecl NuDisplayListResetStateCache(NURNDRLIBSTATE *state) {
    state->mtl = NULL;
    state->tex_id = -1;
    state->global_id = -1;
    state->lights_id = -1;
    state->camera_id = -1;
    state->fog_id = -1;
    state->konst_id = -1;
}

i32 __cdecl NuDisplayListAddRenderScene(void) {
    NUSORTPRI **pris;
    NUSORTPRI *node;
    NUSORTPRI *rebuilt;
    NUDISPLAYSCENE *sub;
    NUDISPLAYLIST *dlist;
    NUDISPLAYLISTITEM *item;
    NUDISPLAYLISTITEM *src;
    NURNDRLIBSTATE *state;
    u32 items;
    u32 offset;
    i32 count;
    i32 m;
    i32 result;

    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xfU) & ~0xfU;
    pris = (NUSORTPRI **)NuDisplayListStreamCursor->addr;
    NuDisplayListStreamCursor->addr += (nused_sort_pris + 1) * sizeof(NUSORTPRI *);
    count = 0;

    for (node = sort_list; node != NULL; node = node->sys_next) {
        sub = node->display_scene;
        if (sub != NULL && (sub->flags & 0x10) != 0) {
            if ((sub->flags & 0x2) != 0) {
                continue;
            }
            rebuilt = (NUSORTPRI *)((NuDisplayListStreamCursor->addr + 3U) & ~3U);
            NuDisplayListStreamCursor->addr = (u32)rebuilt + sizeof(NUSORTPRI);
            rebuilt->sort_pri = node->sort_pri;
            rebuilt->items = (NUDISPLAYLISTITEM *)((u8 *)rebuilt + sizeof(NUSORTPRI));
            rebuilt->display_scene = node->display_scene;
            rebuilt->nmtls = node->nmtls;
            rebuilt->mtl_first = node->mtl_first;
            items = (u32)rebuilt->items;
            offset = 0;
            if (rebuilt->nmtls != 0) {
                for (m = 0; m < rebuilt->nmtls; m++) {
                    dlist = sub->dlist_mtls[(u16)(rebuilt->mtl_first + m)];
                    if (dlist->mtl_last == dlist->first) {
                        continue;
                    }
                    item = dlist->mtl_item;
                    src = (item[1].type == 0x80) ? &item[1] : &item[3];
                    *(NUDISPLAYLISTITEM *)(items + offset) = *src;
                    offset += 0x10;
                    *(NUDISPLAYLISTITEM *)(items + offset) = *dlist->first;
                    offset += 0x10;
                    dlist->mtl_last->next = (void *)(items + offset);
                    dlist->mtl_last = dlist->first;
                    NuDisplayListResetStateCache(dlist->state);
                }
            }
            *(u8 *)(items + offset) = 0x84;
            *((u8 *)items + offset + 1) = 4;
            *(u32 *)(items + offset + 4) = 0;
            offset += 0x10;
            NuDisplayListStreamCursor->addr = items + offset;
            pris[count] = rebuilt;
            count++;
            rebuilt->flags |= 0x2;
        } else {
            if ((node->flags & 0x2) != 0) {
                continue;
            }
            if (sub != NULL && (sub->flags & 0x6) == 0) {
                continue;
            }
            pris[count] = node;
            count++;
            node->flags |= 0x2;
        }
    }

    if (count == 0 && displaylist_2d_mtl_last == displaylist_2d_first) {
        return -1;
    }

    result = (NuDisplayListStreamCursor->addr + 3U) & ~3U;
    render_scene_ptrs[render_scene_index] = (NUDISPLAYLISTRENDERSCENE *)result;
    NuDisplayListStreamCursor->addr = result + sizeof(NUDISPLAYLISTRENDERSCENE);
    render_scene_ptrs[render_scene_index]->nsort_pris = count;
    render_scene_ptrs[render_scene_index]->sort_pris = pris;

    if (displaylist_2d_mtl_last != displaylist_2d_first) {
        *(u8 *)displaylist_2d_mtl_last = 0x84;
        *(u32 *)((u8 *)displaylist_2d_mtl_last + 4) = 0;
        *((u8 *)displaylist_2d_mtl_last + 1) = 4;
        item = displaylist_2d_first;
        *(NUDISPLAYLISTITEM *)((u8 *)render_scene_ptrs[render_scene_index] + 8) = *item;
        NuDisplayListResetStateCache(displaylist_2d_state);
        displaylist_2d_mtl_last = displaylist_2d_first;
    } else {
        item = (NUDISPLAYLISTITEM *)((u8 *)render_scene_ptrs[render_scene_index] + 8);
        *(u8 *)item = 0x84;
        *(u32 *)((u8 *)item + 4) = 0;
        *((u8 *)item + 1) = 4;
    }

    result = render_scene_index;
    render_scene_index += 1;
    return result;
}

void __cdecl NuRndrEndScene(void) {
    i32 scene_id = NuDisplayListAddRenderScene();

    currentScene.scene_id = scene_id;
    if (currentScene.dyn_light_count != 0 && scene_id != -1) {
        NURNDRLIGHTRECORD *records = (NURNDRLIGHTRECORD *)currentScene.dyn_light_ptr;
        NURNDRLIGHTRECORD *record;

        if (records[2].enabled != 0) {
            record = &records[currentScene.unknown_40];
            record->pending[record->count] = scene_id;
            record->count += 1;
        } else {
            currentScene.dyn_light_count = 0;
        }
        currentScene.scene_id = -1;
    }
    sceneParameters[sceneParametersCount] = currentScene;
    sceneParametersCount += 1;
}

void __cdecl NuDisplayListBeginFrame(void) {
    u32 flags;
    u32 node_flags;
    NUSORTPRI **entry;
    NUSORTPRI *node;
    i32 index;

    PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
    flags = nudlist_flags;
    flags ^= (~flags & 0x8000);
    nudlist_flags = flags;
    NuDisplayListRender2D(0);
    if (nudlist_field_9d8 > 0) {
        entry = (NUSORTPRI **)PCDisplayScenes;
        for (index = 0; index < nudlist_field_9d8; index++, entry++) {
            node = *entry;
            node_flags = node->flags;
            node_flags ^= (~node_flags & 0x8000);
            node->flags = node_flags;
            NuDisplayListRender2D(0);
            if (node->sys_next != NULL) {
                *(u32 *)((u8 *)node->sys_next + 0x44) = 0;
            }
        }
    }
    for (node = sort_list; node != NULL; node = node->sys_next) {
        node->flags &= ~0x2;
    }
    render_scene_index = 0;
    RndrStateResetSharedGlobalState();
    PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
}

void __cdecl NuDisplayListStreamAlloc(VARIPTR *cursor, i32 arg2, i32 count, i32 arg4, VARIPTR *arg5) {
    u32 aligned;
    i32 index;
    u32 offset;

    nudlist_alloc_count = count;
    nudlist_alloc_base = cursor->addr;
    cursor->addr += count;
    aligned = (cursor->addr + 3U) & ~3U;
    nudlist_alloc_end = aligned;
    cursor->addr = aligned + count * 36;
    nudlist_item_base = (cursor->addr + 0xfU) & ~0xfU;
    cursor->addr = nudlist_item_base;
    cursor->addr += count * 32;
    for (index = 0; index < count; index++) {
        *(u8 *)(nudlist_alloc_base + index) = 0;
        offset = index * 0x20;
        *(u8 *)(nudlist_item_base + offset + 0x10) = 0x84;
        *(u32 *)(nudlist_item_base + offset + 0x14) = 0;
        *((u8 *)nudlist_item_base + offset + 0x11) = 4;
    }
}

void __cdecl NuDisplayListInit(VARIPTR *cursor, i32 arg2) {
    STUBBED_NAMED("NuDisplayListInit: 0x5fd1c0 setup");
    nudlist_field_9d8 = 0;
    nudlist_field_e80 = 0;
    sort_list = NULL;
    nudlist_field_fc8 = 0;
    displaylist_2d_first = &nudlist_2d_static;
    STUBBED_NAMED("NuDisplayListInit: NuDisplayListCreateDynMtlList (0x5efbf0)");
    nudlist_flags |= 0x10;
    nudlist_field_e58 = 0;
    NuDisplayListStreamAlloc(cursor, arg2, 0x40, arg2, cursor);
    NuDisplayListStreamCursor = &nudlist_stream_cursor;
    nudlist_field_1598 = nuprim_field_ba8;
    STUBBED_NAMED("NuDisplayListInit: 0x4ffa60 setup");
    STUBBED_NAMED("NuDisplayListInit: 0x5f0d70 first-frame draw");
    STUBBED_NAMED("NuDisplayListInit: NuDisplayListBeginFrame (0x5dfab0)");
}

DWA_WEIGHT_BLOCK_PC *__cdecl NuRndrCreateBlendShapeDeformerWeightsArray(i32 count) {
    i32 clear_bytes = count * 4 + 0x204;
    DWA_WEIGHT_BLOCK_PC *result;
    DwaWeightCursorPc -= clear_bytes;
    if (DwaWeightCursorPc < 0) {
        PCClipScreenUnsupported();
        return NULL;
    }
    result = &DwaWeightPoolPc[DwaWeightCursorPc];
    memset(result, 0, clear_bytes);
    return result;
}
DWA_WEIGHT_BLOCK_PC **__cdecl NuRndrCreateBlendShapeDWAPointers(i32 count) {
    DwaPointerCursorPc -= count;
    if (DwaWeightCursorPc < 0) {
        PCClipScreenUnsupported();
        return NULL;
    }
    return &DwaPointerPoolPc[DwaPointerCursorPc];
}
