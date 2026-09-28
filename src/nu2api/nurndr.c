#include "saga.h"

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
