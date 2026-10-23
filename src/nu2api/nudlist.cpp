#include <stdio.h>
#include "saga.h"

extern "C" {
    void __cdecl PCThreadCriticalSectionEnter_candidate(i32);
    void __cdecl PCThreadCriticalSectionLeave_candidate(i32);
    extern void __cdecl PCSpecialEmptyHandler_candidate(void *ignored);

    void __cdecl NuDisplaySceneDestroy(NUDISPLAY_LIST_PC *scene) {
        i32 i;
        i32 removed = 0;
        NUDISPLAY_LIST_PC **cursor;
        if (scene == NULL)
            return;
        PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
        NuDisplaySceneDestroyPS(scene);
        for (i = 0, cursor = PCDisplayScenes; i < (i32)nudlist_field_9d8; ++i, ++cursor) {
            if (*cursor == scene)
                goto found;
        }
        AnimationDiagnosticNoOpPc();
        for (;;) {
        }
    found:
        PCSpecialEmptyHandler_candidate(scene->allocation);
        for (i = 0; i < scene->sort_count; ++i) {
            NUSORTPRI *previous = NULL;
            NUSORTPRI *sort;
            for (sort = sort_list; sort != &scene->sort_pris[i]; sort = sort->sys_next)
                previous = sort;
            if (previous == NULL)
                sort_list = sort->sys_next;
            else
                previous->sys_next = sort->sys_next;
            --nused_sort_pris;
        }
        if (scene->material_animations != NULL) {
            PC_MATERIAL_ANIM_SET *scene_animation = (PC_MATERIAL_ANIM_SET *)scene->material_animations;
            PC_MATERIAL_ANIM_SET *animation = (PC_MATERIAL_ANIM_SET *)nudlist_field_fc8;
            if (animation == scene_animation) {
                nudlist_field_fc8 = (u32)scene_animation->next;
            } else {
                while (animation->next != scene_animation)
                    animation = animation->next;
                animation->next = animation->next->next;
            }
        }
        for (i = 0, cursor = PCDisplayScenes; i < (i32)nudlist_field_9d8; ++i, ++cursor) {
            PCDisplayScenes[i - removed] = *cursor;
            if (*cursor == scene)
                ++removed;
        }
        if (removed != 0)
            --nudlist_field_9d8;
        PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    }

    void __cdecl NuDisplayListDestroyMtl(NUMTL *material) {
        u32 index;
        i32 found = -1;
        PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
        if (material->display_list == NULL) {
            PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
            return;
        }
        for (index = 0; index < PCDisplayMaterialCount; ++index) {
            if (PCDisplayMaterialRegistry[index] == material) {
                found = (i32)index;
                break;
            }
        }
        if (found == -1) {
            PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
            return;
        }
        if (PCDisplayMaterialDeleteCount == 600) {
            AnimationDiagnosticNoOpPc();
            PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
            return;
        }
        PCDisplayMaterialDeleteQueue[PCDisplayMaterialDeleteCount++] = material;
        PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    }

    static void __cdecl ResetSceneBeforeFramePC(NUDISPLAY_LIST_PC *scene, i32 force) {
        i32 blocks;
        u32 i;
        if (force == 0 && (scene->render_flags & 6) == 0)
            return;
        if (scene->material_count != 0)
            NuMemSet128(scene->material_used[(scene->render_flags >> 15) & 1], 0,
                        ((scene->material_count + 7) >> 7) + 1);
        if (scene->clip_count != 0) {
            blocks = (((scene->clip_count + 7) / 8) * 2) / 16 + 1;
            NuMemSet128(scene->clip_used[(scene->render_flags >> 15) & 1], 0, blocks);
            if (force != 0)
                NuMemSet128(scene->clip_used[((scene->render_flags >> 15) ^ 1) & 1], 255, blocks);
        }
        for (i = 0; i < scene->material_count; ++i) {
            NUDISPLAYLIST *list = scene->material_lists[i];
            list->mtl_last = list->first;
            if (force != 0)
                list->scene_buffer = 0;
            else
                list->scene_buffer = list->scene_buffer == 0;
            list->scene_next = list->scene_first[list->scene_buffer];
            NuDisplayListResetStateCache(list->state);
        }
        scene->render_flags &= ~14u;
    }

    static void __cdecl DisplayInsertSortPC(NUSORTPRI *sort) {
        NUSORTPRI *previous = NULL;
        NUSORTPRI *current = sort_list;
        if (current == NULL) {
            sort_list = sort;
            sort->sys_next = NULL;
        } else {
            while (current != NULL && current->sort_pri < sort->sort_pri) {
                previous = current;
                current = current->sys_next;
            }
            if (previous == NULL) {
                sort->sys_next = sort_list;
                sort_list = sort;
            } else {
                sort->sys_next = current;
                previous->sys_next = sort;
            }
        }
        ++nused_sort_pris;
    }

    void __cdecl NuDisplaySceneAdd(NUDISPLAY_LIST_PC *scene) {
        i32 i;
        PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
        if ((i32)nudlist_field_9d8 >= 256)
            AnimationDiagnosticNoOpPc();
        PCDisplayScenes[nudlist_field_9d8++] = scene;
        ResetSceneBeforeFramePC(scene, 1);
        for (i = 0; i < scene->sort_count; ++i) {
            if (numtl_renderplane != 0)
                scene->sort_pris[i].sort_pri += numtl_renderplane << 17;
            DisplayInsertSortPC(&scene->sort_pris[i]);
        }
        if (scene->material_animations != NULL) {
            scene->material_animations->next = (PC_MATERIAL_ANIM_SET *)nudlist_field_fc8;
            nudlist_field_fc8 = (u32)scene->material_animations;
        }
        scene->render_flags &= 0xffffdfef;
        scene->alpha_values = NULL;
        NuDisplaySceneAddPS(scene);
        PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    }

    void __cdecl NuDisplayListBeginFrame(void) {
        NUSORTPRI *sort;
        i32 i;
        PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
        PCDynamicDisplayScene.render_flags =
            (PCDynamicDisplayScene.render_flags & ~0x8000u) | (~PCDynamicDisplayScene.render_flags & 0x8000u);
        ResetSceneBeforeFramePC(&PCDynamicDisplayScene, 0);
        i = 0;
        if ((i32)nudlist_field_9d8 > 0) {
            NUDISPLAY_LIST_PC **scenes = PCDisplayScenes;
            do {
                NUDISPLAY_LIST_PC *scene = *scenes;
                scene->render_flags = (scene->render_flags & ~0x8000u) | (~scene->render_flags & 0x8000u);
                ResetSceneBeforeFramePC(scene, 0);
                if (scene->gscene != NULL)
                    scene->gscene->frame_reset_marker = 0;
                ++i;
                ++scenes;
            } while (i < (i32)nudlist_field_9d8);
        }
        render_scene_index = 0;
        for (sort = sort_list; sort != NULL; sort = sort->sys_next)
            sort->flags &= ~2u;
        RndrStateResetSharedGlobalState();
        PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    }

    typedef struct pc_display_material_pc_s {
        union {
            u16 flags;
            struct {
                u16 flags_low : 4;
                i16 sort_layer : 8;
                u16 flags_high : 4;
            };
        };
        u8 unknown_0002[0x3a];
        NUDISPLAYLIST *display_list;
        u8 unknown_0040[4];
        u32 visibility_flags;
        u8 unknown_0048[0x2c];
        i16 texture_id;
        i16 sort_priority;
        u8 unknown_0078[0x80];
        u8 blend_operation[3];
    } PC_DISPLAY_MATERIAL_PC;
    STATIC_ASSERT(offsetof(PC_DISPLAY_MATERIAL_PC, display_list) == 0x3c);
    STATIC_ASSERT(offsetof(PC_DISPLAY_MATERIAL_PC, visibility_flags) == 0x44);
    STATIC_ASSERT(offsetof(PC_DISPLAY_MATERIAL_PC, texture_id) == 0x74);
    STATIC_ASSERT(offsetof(PC_DISPLAY_MATERIAL_PC, sort_priority) == 0x76);
    STATIC_ASSERT(offsetof(PC_DISPLAY_MATERIAL_PC, blend_operation) == 0xf8);

    /* Staged fragment for the original nudlist caller group. */
    void __cdecl NuDisplayListRecalculateMaterialStartsPC(NUDISPLAY_LIST_PC *scene) {
        i32 total = 0, i, j;
        for (i = 0; i < scene->sort_count; ++i)
            total += scene->sort_pris[i].nmtls;
        for (i = 0; i < scene->sort_count; ++i) {
            NUSORTPRI *sort = &scene->sort_pris[i];
            if (sort->nmtls != 0) {
                for (j = 0; j < total; ++j) {
                    PC_DISPLAY_MATERIAL_PC *material = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[j];
                    i32 key = material->sort_layer * 131072 + material->sort_priority;
                    if (key == sort->sort_pri) {
                        sort->mtl_first = (u16)j;
                        break;
                    }
                }
            }
        }
    }

    static void __cdecl DisplayUpdateClipWordsPC(NUDISPLAY_LIST_PC *scene) {
        u32 buffer = (scene->render_flags >> 15) & 1;
        u32 *current = (u32 *)scene->clip_used[buffer];
        u32 *other = (u32 *)scene->clip_used[buffer ^ 1];
        i32 words = (scene->clip_count + 15) / 16;
        i32 word, object = 0;
        NUDISPLAYLISTITEM *items = (NUDISPLAYLISTITEM *)scene->items;
        for (word = 0; word < words; ++word) {
            if (current[word] != other[word]) {
                u8 *bytes = (u8 *)&current[word];
                i32 byte_index, shift;
                for (byte_index = 0; byte_index < 4; ++byte_index) {
                    for (shift = 0; shift < 8; shift += 2) {
                        i32 index;
                        u8 code = (u8)((bytes[byte_index] >> shift) & 3);
                        PC_DISPLAY_CLIP_OBJECT_PC *clip = &scene->clip_objects[object];
                        u16 count = scene->clip_counts[object];
                        for (index = 0; index < count; ++index) {
                            i32 item = clip->item_indices[index];
                            items[item].id = code == 0 ? 0 : 3;
                            items[item - 1].id = code == 0 ? 0 : 3;
                        }
                        ++object;
                        if (object >= scene->clip_count)
                            return;
                    }
                }
            } else {
                object += 16;
            }
        }
    }

    static void __cdecl DisplayUpdateAlphaPC(NUDISPLAY_LIST_PC *scene) {
        i32 object, item_index;
        u16 *counts = scene->clip_counts;
        if (scene->alpha_values == NULL)
            return;
        for (object = 0; object < scene->clip_count; ++object, ++counts) {
            i32 *indices = scene->clip_objects[object].item_indices;
            for (item_index = 0; item_index < *counts; ++item_index) {
                NUDISPLAYLISTITEM *item = &((NUDISPLAYLISTITEM *)scene->items)[indices[item_index]];
                if (item->id != 0 && item->type == 0x82)
                    DisplayListSetAlphaPS(item - 1, item, scene->alpha_values[object]);
            }
        }
    }

    static void __cdecl UpdateMaterialClipBitsPC(NUDISPLAY_LIST_PC *scene) {
        i32 material_index = 0;
        u32 flags;
        u8 *current, *other;
        NUDISPLAYLIST **lists;
        i32 bytes, byte_index;
        if (scene == NULL || scene->display_materials == NULL || scene->display_materials[0] == NULL)
            return;
        flags = scene->render_flags >> 15;
        current = scene->material_used[flags & 1];
        lists = scene->material_lists;
        bytes = (i32)((scene->material_count + 7) >> 3);
        other = scene->material_used[~flags & 1];
        for (byte_index = 0; byte_index < bytes; ++byte_index) {
            u8 value = current[byte_index];
            if (value == other[byte_index]) {
                material_index += 8;
            } else {
                i32 bit;
                for (bit = 0; bit < 8; ++bit) {
                    i32 enabled = (value >> bit) & 1;
                    NUDISPLAYLIST *list = lists[material_index];
                    PC_DISPLAY_MATERIAL_PC *material =
                        (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[list->material_id];
                    if (material != NULL && enabled != 0) {
                        if (PCDeviceHardwareVertexProcessingPC != 1) {
                            enabled = material->blend_operation[0] != 0xff;
                        } else if (material->blend_operation[0] != 0xff && (i32)material->blend_operation[0] +
                                                                                   material->blend_operation[1] +
                                                                                   material->blend_operation[2] !=
                                                                               0) {
                            list->mtl_item->id = 1;
                            goto next;
                        }
                    }
                    if (enabled)
                        list->mtl_item->id = 0;
                    else
                        list->mtl_item->id = 1;
                next:
                    ++material_index;
                    if (material_index >= (i32)scene->material_count)
                        return;
                }
            }
        }
    }

    void __cdecl DisplayListLinkDynamicMtls(void) {
        NUDISPLAY_LIST_PC *scene = &PCDynamicDisplayScene;
        i32 queued;
        if (PCDisplayMaterialAddCount == 0 && PCDisplayMaterialDeleteCount == 0)
            return;
        for (queued = 0; queued < PCDisplayMaterialDeleteCount; ++queued) {
            PC_DISPLAY_MATERIAL_PC *material = (PC_DISPLAY_MATERIAL_PC *)PCDisplayMaterialDeleteQueue[queued];
            NUDISPLAYLISTITEM *items;
            NUSORTPRI *sort;
            i32 index = -1, key, sort_index = -1, i, link, count;
            if (material->display_list == NULL)
                continue;
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (scene->display_materials[i] == (NUMTL *)material) {
                    index = i;
                    break;
                }
            }
            if (index == -1)
                continue;
            if (--PCDisplayMaterialReferenceCounts[index] > 0)
                continue;
            key = material->sort_layer * 131072 + material->sort_priority;
            for (i = 0; i < scene->sort_count; ++i) {
                if (scene->sort_pris[i].sort_pri == key)
                    sort_index = i;
            }
            items = material->display_list->mtl_item;
            if (scene->sort_pris[sort_index].items == items) {
                link = PCMaterialLinkItemIndex;
                scene->sort_pris[sort_index].items = (NUDISPLAYLISTITEM *)items[link].next;
            } else {
                count = scene->sort_pris[sort_index].nmtls;
                link = PCMaterialLinkItemIndex;
                for (i = 0; i < count; ++i) {
                    NUDISPLAYLISTITEM *previous =
                        ((PC_DISPLAY_MATERIAL_PC *)scene->display_materials[scene->sort_pris[sort_index].mtl_first + i])
                            ->display_list->mtl_item;
                    if (previous[link].next == items) {
                        previous[link].next = items[link].next;
                        goto unlink;
                    }
                }
                goto unlinked;
            }
        unlink:
            items[link].id = 4;
            items[link].next = NULL;
            items[link].type = 0x84;
        unlinked:
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialStreamUsed[i] != 0 &&
                    items == (NUDISPLAYLISTITEM *)(PCDisplayMaterialStreamStart.addr + (u32)i * 0x80)) {
                    PCDisplayMaterialStreamUsed[i] = 0;
                    break;
                }
            }
            if (--scene->sort_pris[sort_index].nmtls == 0) {
                NUSORTPRI *previous = NULL;
                NUSORTPRI *current = sort_list;
                sort = &scene->sort_pris[sort_index];
                while (current != sort) {
                    previous = current;
                    current = current->sys_next;
                }
                if (previous == NULL)
                    sort_list = sort->sys_next;
                else
                    previous->sys_next = sort->sys_next;
                --nused_sort_pris;
                for (i = sort_index; i < scene->sort_count - 1; ++i)
                    scene->sort_pris[i] = scene->sort_pris[i + 1];
                --scene->sort_count;
            }
            {
                NUDISPLAYLIST *removed = scene->material_lists[index];
                for (i = index; i < (i32)scene->material_count - 1; ++i) {
                    scene->display_materials[i] = scene->display_materials[i + 1];
                    scene->material_lists[i] = scene->material_lists[i + 1];
                    PCDisplayMaterialReferenceCounts[i] = PCDisplayMaterialReferenceCounts[i + 1];
                }
                scene->material_lists[scene->material_count - 1] = removed;
                PCDisplayMaterialReferenceCounts[scene->material_count - 1] = 0;
            }
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialReferenceCounts[i] != 0) {
                    PC_DISPLAY_MATERIAL_PC *used = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[i];
                    used->display_list->material_id = i;
                }
            }
            NuDisplayListRecalculateMaterialStartsPC(scene);
            material->display_list = NULL;
        }
        PCDisplayMaterialDeleteCount = 0;
        for (queued = 0; queued < PCDisplayMaterialAddCount; ++queued) {
            PC_DISPLAY_MATERIAL_PC *material = (PC_DISPLAY_MATERIAL_PC *)PCDisplayMaterialAddQueue[queued];
            NUDISPLAYLIST *unused;
            NUDISPLAYLISTITEM *items;
            VARIPTR cursor;
            i32 index = -1, insert = -1, stream_index = -1, key, sort_index = -1, i, link;
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (scene->display_materials[i] == (NUMTL *)material) {
                    index = i;
                    break;
                }
            }
            if (index != -1) {
                ++PCDisplayMaterialReferenceCounts[index];
                continue;
            }
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialReferenceCounts[i] == 0) {
                    index = i;
                    break;
                }
            }
            if (index == -1) {
                AnimationDiagnosticNoOpPc();
                continue;
            }
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialStreamUsed[i] == 0) {
                    PCDisplayMaterialStreamUsed[i] = 1;
                    stream_index = i;
                    break;
                }
            }
            key = material->sort_layer * 131072 + material->sort_priority;
            for (i = 0; i < scene->sort_count; ++i) {
                if (scene->sort_pris[i].sort_pri == key)
                    sort_index = i;
            }
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialReferenceCounts[i] != 0) {
                    PC_DISPLAY_MATERIAL_PC *other = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[i];
                    if (key <= other->sort_layer * 131072 + other->sort_priority) {
                        insert = i;
                        break;
                    }
                }
            }
            if (insert == -1)
                insert = index;
            unused = scene->material_lists[scene->material_count - 1];
            for (i = (i32)scene->material_count - 2; i >= insert; --i) {
                scene->display_materials[i + 1] = scene->display_materials[i];
                scene->material_lists[i + 1] = scene->material_lists[i];
                PCDisplayMaterialReferenceCounts[i + 1] = PCDisplayMaterialReferenceCounts[i];
            }
            scene->material_lists[insert] = unused;
            PCDisplayMaterialReferenceCounts[insert] = 0;
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialReferenceCounts[i] != 0)
                    ((PC_DISPLAY_MATERIAL_PC *)scene->display_materials[i])->display_list->material_id = i;
            }
            PCDisplayMaterialReferenceCounts[insert] = 1;
            material->display_list = scene->material_lists[insert];
            cursor.addr = PCDisplayMaterialStreamStart.addr + (stream_index << 7);
            items = NuDisplayListCreateMtlDlist(material->display_list->mtl_item, (NUMTL *)material, &cursor,
                                                PCDisplayMaterialStreamEnd);
            scene->display_materials[insert] = (NUMTL *)material;
            material->display_list->mtl_item = items;
            material->display_list->dynamic_geometry = items + PCMaterialGeometryItemIndex;
            material->display_list->scene = scene;
            material->display_list->material_id = insert;
            if (sort_index != -1) {
                if (insert != 0) {
                    PC_DISPLAY_MATERIAL_PC *previous = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[insert - 1];
                    if (previous->sort_layer * 131072 + previous->sort_priority == key) {
                        NUDISPLAYLISTITEM *anchor =
                            ((PC_DISPLAY_MATERIAL_PC *)scene->display_materials[insert - 1])->display_list->mtl_item;
                        link = PCMaterialLinkItemIndex;
                        items[link].type = 0x8d;
                        items[link].next = anchor[link].next;
                        items[link].id = 1;
                        anchor[link].next = items;
                        goto linked;
                    }
                }
                {
                    NUDISPLAYLISTITEM *head = scene->sort_pris[sort_index].items;
                    link = PCMaterialLinkItemIndex;
                    items[link].type = 0x8d;
                    items[link].next = head;
                    items[link].id = 1;
                    scene->sort_pris[sort_index].items = items;
                }
            } else {
                sort_index = scene->sort_count;
                if (sort_index >= 600)
                    AnimationDiagnosticNoOpPc();
                scene->sort_pris[sort_index].sort_pri = key;
                scene->sort_pris[sort_index].items = items;
                scene->sort_pris[sort_index].sort_flags = nudlist_field_e80;
                scene->sort_pris[sort_index].nmtls = 0;
                DisplayInsertSortPC(&scene->sort_pris[sort_index]);
                ++scene->sort_count;
            }
        linked:
            ++scene->sort_pris[sort_index].nmtls;
            NuDisplayListRecalculateMaterialStartsPC(scene);
        }
        PCDisplayMaterialAddCount = 0;
    }

    NUDISPLAYLISTITEM *__cdecl NuDisplayListCreateMtlDlist(NUDISPLAYLISTITEM *unused, NUMTL *material, VARIPTR *cursor,
                                                           VARIPTR end) {
        NUDISPLAYLISTITEM *items = (NUDISPLAYLISTITEM *)cursor->addr;
        PC_DISPLAY_MATERIAL_PC *view = (PC_DISPLAY_MATERIAL_PC *)material;
        (void)unused;
        (void)end;
        cursor->addr += 0x80;
        items[0].type = 0x85;
        items[0].next = items + PCMaterialBlockItemCount - 1;
        items[0].id = 1;
        NuDisplayListAddClut(items + 1, view->texture_id);
        NuDisplayListAddTexture(items + 2, view->texture_id);
        NuDisplayListAddMaterialState(items + 3, material);
        NuDisplayListAddMicrocode(items + 4, material);
        NuDisplayListAddLightState(items + 5, material);
        items[6].next = items + PCMaterialBlockItemCount - 1;
        items[6].type = 0x8b;
        items[6].id = 1;
        items[7].type = 0x84;
        items[7].next = NULL;
        items[7].id = 4;
        return items;
    }

    /* Same layout as NURNDRLIBSTATE without 1- or 2-byte arrays: retail's stack copy carries no /GS cookie. */
    typedef struct pc_rndr_state_local_s {
        u32 opaque_00_08[2];
        void *mtl;
        u32 opaque_0c_14[2];
        i32 tex_id;
        u16 opaque_18;
        u16 opaque_1a;
        u16 opaque_1c;
        i16 konst_id;
        i32 global_id;
        i16 lights_id;
        u16 opaque_26;
        u16 opaque_28;
        u16 opaque_2a;
        i16 camera_id;
        i16 fog_id;
        u32 opaque_30_34;
    } PC_RNDR_STATE_LOCAL;
    STATIC_ASSERT(sizeof(PC_RNDR_STATE_LOCAL) == sizeof(NURNDRLIBSTATE));
    STATIC_ASSERT(offsetof(PC_RNDR_STATE_LOCAL, fog_id) == offsetof(NURNDRLIBSTATE, fog_id));

    void __cdecl DisplayProcessMaterialRenderStatesPC(void) {
        PC_RNDR_STATE_LOCAL state;
        NUGLOBALRNDRSTATE_PC *last_local = NULL;
        NUSORTPRI *sort;
        i32 i;
        NuDisplayListResetStateCache((NURNDRLIBSTATE *)&state);
        for (sort = sort_list; sort != NULL; sort = sort->sys_next) {
            NUDISPLAY_LIST_PC *scene = sort->display_scene;
            if (scene == NULL) {
                RndrStateUpdateFx((NURNDRLIBSTATE *)&state, sort->items);
            } else if ((scene->render_flags & 6) != 0) {
                for (i = 0; i < sort->nmtls; ++i) {
                    i32 index = sort->mtl_first + i;
                    NUDISPLAYLIST *list = scene->material_lists[index];
                    PC_DISPLAY_MATERIAL_PC *material;
                    if (list->mtl_item->id != 0)
                        continue;
                    material = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[index];
                    if (scene->local_state != NULL && (scene->render_flags & 4) != 0) {
                        if (list->mtl_last != list->first) {
                            DisplayListUpdateRenderState((NUDISPLAYLIST_PC *)list, scene->local_state);
                            last_local = scene->local_state;
                        } else if (last_local != scene->local_state) {
                            DisplayListUpdateRenderState((NUDISPLAYLIST_PC *)list, scene->local_state);
                            last_local = scene->local_state;
                        }
                    } else {
                        last_local = NULL;
                    }
                    if (list->mtl_last != list->first) {
                        *list->dynamic_geometry = *list->first;
                        if ((scene->render_flags & 4) == 0) {
                            list->mtl_last->next = list->mtl_item->next;
                        } else if (list->scene_first[list->scene_buffer] != list->scene_next) {
                            list->mtl_last->next = list->scene_first[list->scene_buffer];
                            list->scene_next->type = 0x8d;
                            list->scene_next->id = 1;
                            list->scene_next->next = list->dynamic_geometry + 1;
                        }
                        list->mtl_last = list->first;
                    } else if (list->scene_first[list->scene_buffer] != list->scene_next) {
                        list->dynamic_geometry->next = list->scene_first[list->scene_buffer];
                        list->scene_next->type = 0x8d;
                        list->scene_next->id = 1;
                        list->scene_next->next = list->dynamic_geometry + 1;
                    } else {
                        list->dynamic_geometry->next = list->dynamic_geometry + 1;
                    }
                    RndrStateUpdate((NURNDRLIBSTATE *)&state, (NUMTL *)material, list->mtl_item);
                    if ((u8)(scene->render_flags >> 5) != 0 &&
                        ((u8)(scene->render_flags >> 5) & (u8)(material->visibility_flags >> 14)) != 0) {
                        list->mtl_item->id = 1;
                        scene->material_used[(scene->render_flags >> 15) & 1][list->material_id / 8] &=
                            (u8) ~(1u << (list->material_id % 8));
                    }
                }
            }
        }
        {
            NUDISPLAY_LIST_PC **scenes = PCDisplayScenes;
            for (i = 0; i < (i32)nudlist_field_9d8; ++i, ++scenes) {
                if ((*scenes)->local_state != NULL)
                    RndrStateResetGlobalState((*scenes)->local_state);
            }
        }
    }

    void __cdecl NuDisplayListSwapBuffersEndFrame(void) {
        i32 i;
        PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
        DisplayListLinkDynamicMtls();
        DisplayListSwapBuffersPS();
        UpdateMaterialClipBitsPC(&PCDynamicDisplayScene);
        for (i = 0; i < (i32)nudlist_field_9d8; ++i) {
            NUDISPLAY_LIST_PC *scene = PCDisplayScenes[i];
            if ((scene->render_flags & 4) != 0) {
                DisplayUpdateClipWordsPC(scene);
                DisplayUpdateAlphaPC(scene);
            }
            if ((scene->render_flags & 6) != 0)
                UpdateMaterialClipBitsPC(scene);
        }
        for (i = 0; i < 24; ++i)
            PCSafeRenderScenes[i] = render_scene_ptrs[i];
        DisplayProcessMaterialRenderStatesPC();
        PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    }

    extern void *PCDisplayClipSet;
    extern i32 PCDisplayClipGroupIndex;
    STATIC_ASSERT(offsetof(NUCAMERA, mtx) + offsetof(NUMTX, m30) == 0x30);
    STATIC_ASSERT(offsetof(NUCAMERA, mtx) + offsetof(NUMTX, m31) == 0x34);
    STATIC_ASSERT(offsetof(NUCAMERA, mtx) + offsetof(NUMTX, m32) == 0x38);

#define CLIP_MARK_MATERIALS()                                                                                          \
    {                                                                                                                  \
        i32 material;                                                                                                  \
        for (material = 0; material < scene->clip_objects[clip_index].material_count; ++material) {                    \
            u32 index = (u32)scene->clip_objects[clip_index].material_ids[material];                                   \
            scene->material_used[(scene->render_flags >> 15) & 1][index >> 3] |= (u8)(1u << (index & 7));              \
        }                                                                                                              \
        scene->clip_used[(scene->render_flags >> 15) & 1][clip_index >> 2] |= (u8)(1u << ((clip_index & 3) * 2));      \
    }
#define CLIP_TEST_BOUNDS()                                                                                             \
    {                                                                                                                  \
        NUVEC4 *center = &scene->clip_bounds[group_index * 2];                                                         \
        NUVEC minimum, maximum;                                                                                        \
        minimum.x = center->x - center[1].x;                                                                           \
        minimum.y = center->y - center[1].y;                                                                           \
        minimum.z = center->z - center[1].z;                                                                           \
        maximum.x = center->x + center[1].x;                                                                           \
        maximum.y = center->y + center[1].y;                                                                           \
        maximum.z = center->z + center[1].z;                                                                           \
        if (((PCFrustumClipSet *)PCDisplayClipSet)->TestBox(&minimum, &maximum, PCDisplayClipGroupIndex))              \
            CLIP_MARK_MATERIALS();                                                                                     \
    }
    static void __cdecl DisplayClipMaterialsPC(NUDISPLAY_LIST_PC *scene) {
        i32 clip_index = 0;
        i32 group_index = 0;
        while (clip_index < scene->clip_count) {
            f32 *range = &scene->fade_levels[clip_index];
            if (*range != 0.0f) {
                if ((scene->visibility_flags[group_index] & 0x20) != 0) {
                    NUVEC4 *center = &scene->clip_bounds[group_index * 2];
                    __declspec(align(16)) NUVEC delta;
                    f32 distance;
                    delta.x = global_camera.mtx.m30 - center->x;
                    delta.y = global_camera.mtx.m31 - center->y;
                    delta.z = global_camera.mtx.m32 - center->z;
                    distance = (delta.x * delta.x + delta.y * delta.y) + delta.z * delta.z;
                    while (*range > distance) {
                        ++range;
                        ++clip_index;
                    }
                    CLIP_TEST_BOUNDS();
                }
                range = &scene->fade_levels[clip_index];
                while (*range != 0.0f)
                    ++range, ++clip_index;
            } else if ((scene->visibility_flags[group_index] & 0x20) != 0) {
                CLIP_TEST_BOUNDS();
            }
            ++clip_index;
            ++group_index;
        }
    }
#undef CLIP_TEST_BOUNDS
#undef CLIP_MARK_MATERIALS
}

typedef struct PC_CLONE_LIST_VIEW {
    NUDISPLAYLIST list;
    i32 scene_item_count;
    u8 unknown_003c[8];
} PC_CLONE_LIST_VIEW;
STATIC_ASSERT(sizeof(PC_CLONE_LIST_VIEW) == 0x44);
STATIC_ASSERT(offsetof(PC_CLONE_LIST_VIEW, scene_item_count) == 0x38);
typedef struct PC_CLONE_MATERIAL_VIEW {
    u8 unknown_0000[0x3c];
    NUDISPLAYLIST *display_list;
    u8 unknown_0040[0x280];
    u16 version;
    u8 unknown_02c2[2];
} PC_CLONE_MATERIAL_VIEW;
STATIC_ASSERT(sizeof(PC_CLONE_MATERIAL_VIEW) == 0x2c4);
STATIC_ASSERT(offsetof(PC_CLONE_MATERIAL_VIEW, display_list) == 0x3c);
STATIC_ASSERT(offsetof(PC_CLONE_MATERIAL_VIEW, version) == 0x2c0);
typedef struct PC_CLONE_ANIMATION_VIEW {
    NUDISPLAY_LIST_PC *scene;
    u8 unknown_0004[8];
    PC_MATERIAL_ANIM_SET *next;
} PC_CLONE_ANIMATION_VIEW;
STATIC_ASSERT(sizeof(PC_CLONE_ANIMATION_VIEW) == 0x10);
STATIC_ASSERT(offsetof(PC_CLONE_ANIMATION_VIEW, next) == 0xc);
typedef struct PC_CLONE_SORT_SELF_VIEW {
    u8 unknown_0000[0x10];
    NUSORTPRI *self;
    u8 unknown_0014[0x10];
} PC_CLONE_SORT_SELF_VIEW;
STATIC_ASSERT(sizeof(PC_CLONE_SORT_SELF_VIEW) == 0x24);
STATIC_ASSERT(offsetof(PC_CLONE_SORT_SELF_VIEW, self) == 0x10);
STATIC_ASSERT(sizeof(NUDISPLAY_LIST_PC) == 0x90);
extern "C" void __cdecl NuDisplaySceneClonePS(NUDISPLAY_LIST_PC *, NUDISPLAY_LIST_PC *, VARIPTR *);

#define CLONE_ALIGN(buffer, alignment) (((buffer)->addr + (alignment) - 1) & ~((alignment) - 1u))
#define CLONE_ITEMS(scene) static_cast<NUDISPLAYLISTITEM *>((scene)->items)
#define CLONE_LIST(scene, index) reinterpret_cast<PC_CLONE_LIST_VIEW *>((scene)->material_lists[index])
#define CLONE_MATERIAL(scene, index) reinterpret_cast<PC_CLONE_MATERIAL_VIEW *>((scene)->display_materials[index])

extern "C" NUDISPLAY_LIST_PC *__cdecl NuDisplaySceneClone(NUDISPLAY_LIST_PC *source, VARIPTR *buffer) {
    NUDISPLAY_LIST_PC *scene;
    PC_CLONE_LIST_VIEW *first_list;
    NURNDRLIBSTATE *states;
    NUDISPLAYLISTITEM *heads;
    PC_CLONE_MATERIAL_VIEW *materials;
    i32 i, j, clip_bytes, material_bytes;
    PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
    scene = reinterpret_cast<NUDISPLAY_LIST_PC *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4) + sizeof(*scene);
    *scene = *source;
    scene->items = reinterpret_cast<void *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->item_count * sizeof(NUDISPLAYLISTITEM);
    memcpy(scene->items, source->items, source->item_count * sizeof(NUDISPLAYLISTITEM));
    clip_bytes = (((source->clip_count + 7) / 8) * 2 / 16 + 1) * 16;
    scene->clip_used[0] = reinterpret_cast<u8 *>(CLONE_ALIGN(buffer, 16));
    buffer->addr = CLONE_ALIGN(buffer, 16) + clip_bytes;
    scene->clip_used[1] = reinterpret_cast<u8 *>(CLONE_ALIGN(buffer, 16));
    buffer->addr = CLONE_ALIGN(buffer, 16) + clip_bytes;
    memset(scene->clip_used[0], 0, clip_bytes);
    memset(scene->clip_used[1], 0, clip_bytes);
    material_bytes = (static_cast<i32>((source->material_count + 7) / 8) / 16 + 1) * 16;
    scene->material_used[0] = reinterpret_cast<u8 *>(CLONE_ALIGN(buffer, 16));
    buffer->addr = CLONE_ALIGN(buffer, 16) + material_bytes;
    scene->material_used[1] = reinterpret_cast<u8 *>(CLONE_ALIGN(buffer, 16));
    buffer->addr = CLONE_ALIGN(buffer, 16) + material_bytes;
    memset(scene->material_used[0], 0, material_bytes);
    memset(scene->material_used[1], 0, material_bytes);
    first_list = reinterpret_cast<PC_CLONE_LIST_VIEW *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->material_count * sizeof(*first_list);
    scene->material_lists = reinterpret_cast<NUDISPLAYLIST **>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->material_count * sizeof(*scene->material_lists);
    states = reinterpret_cast<NURNDRLIBSTATE *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->material_count * sizeof(*states);
    memset(states, 0, source->material_count * sizeof(*states));
    heads = reinterpret_cast<NUDISPLAYLISTITEM *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->material_count * sizeof(*heads);
    memset(heads, 0, source->material_count * sizeof(*heads));
    for (i = 0; i < static_cast<i32>(source->material_count); ++i) {
        scene->material_lists[i] = &first_list[i].list;
        *CLONE_LIST(scene, i) = *CLONE_LIST(source, i);
        scene->material_lists[i]->scene = scene;
        scene->material_lists[i]->state = &states[i];
        scene->material_lists[i]->mtl_item =
            CLONE_ITEMS(scene) + (source->material_lists[i]->mtl_item - CLONE_ITEMS(source));
        scene->material_lists[i]->dynamic_geometry =
            CLONE_ITEMS(scene) + (source->material_lists[i]->dynamic_geometry - CLONE_ITEMS(source));
        scene->material_lists[i]->first = &heads[i];
        scene->material_lists[i]->mtl_last = scene->material_lists[i]->first;
        scene->material_lists[i]->scene_first[0] = reinterpret_cast<NUDISPLAYLISTITEM *>(CLONE_ALIGN(buffer, 16));
        buffer->addr = CLONE_ALIGN(buffer, 16);
        buffer->addr += CLONE_LIST(source, i)->scene_item_count * sizeof(NUDISPLAYLISTITEM);
        scene->material_lists[i]->scene_first[1] = reinterpret_cast<NUDISPLAYLISTITEM *>(CLONE_ALIGN(buffer, 16));
        buffer->addr = CLONE_ALIGN(buffer, 16);
        buffer->addr += CLONE_LIST(source, i)->scene_item_count * sizeof(NUDISPLAYLISTITEM);
        {
            NUDISPLAYLISTITEM *head = scene->material_lists[i]->first;
            head->type = 0x8d;
            head->next = NULL;
            head->id = 1;
        }
    }
    materials = reinterpret_cast<PC_CLONE_MATERIAL_VIEW *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->material_count * sizeof(*materials);
    scene->display_materials = reinterpret_cast<NUMTL **>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->material_count * sizeof(*scene->display_materials);
    for (i = 0; i < static_cast<i32>(source->material_count); ++i) {
        scene->display_materials[i] = reinterpret_cast<NUMTL *>(&materials[i]);
        *CLONE_MATERIAL(scene, i) = *CLONE_MATERIAL(source, i);
        CLONE_MATERIAL(scene, i)->display_list =
            &first_list[reinterpret_cast<PC_CLONE_LIST_VIEW *>(CLONE_MATERIAL(source, i)->display_list) -
                        CLONE_LIST(source, 0)]
                 .list;
        {
            NUMTL *material = scene->display_materials[i];
            NuMtlUpdatePS(material);
            ++reinterpret_cast<PC_CLONE_MATERIAL_VIEW *>(material)->version;
        }
    }
    scene->sort_pris = reinterpret_cast<NUSORTPRI *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->sort_count * sizeof(*scene->sort_pris);
    memcpy(scene->sort_pris, source->sort_pris, source->sort_count * sizeof(*scene->sort_pris));
    for (i = 0; i < source->sort_count; ++i) {
        scene->sort_pris[i].items = CLONE_ITEMS(scene) + (source->sort_pris[i].items - CLONE_ITEMS(source));
        scene->sort_pris[i].display_scene = scene;
        reinterpret_cast<PC_CLONE_SORT_SELF_VIEW *>(&scene->sort_pris[i])->self = &scene->sort_pris[i];
    }
    for (i = 0; i < source->item_count; ++i) {
        switch (CLONE_ITEMS(scene)[i].type) {
            case 0x80: {
                void *next = CLONE_ITEMS(source)[i].next;
                for (j = 0; j < static_cast<i32>(source->material_count); ++j) {
                    if (next == source->display_materials[j]) {
                        CLONE_ITEMS(scene)[i].next = scene->display_materials[j];
                        break;
                    }
                }
                break;
            }
            case 0x85:
                CLONE_ITEMS(scene)
                [i].next = CLONE_ITEMS(scene) +
                           (static_cast<NUDISPLAYLISTITEM *>(CLONE_ITEMS(source)[i].next) - CLONE_ITEMS(source));
                break;
        }
    }
    if (source->material_animations != NULL) {
        scene->material_animations = reinterpret_cast<PC_MATERIAL_ANIM_SET *>(CLONE_ALIGN(buffer, 4));
        buffer->addr = CLONE_ALIGN(buffer, 4) + sizeof(PC_CLONE_ANIMATION_VIEW);
        *reinterpret_cast<PC_CLONE_ANIMATION_VIEW *>(scene->material_animations) =
            *reinterpret_cast<PC_CLONE_ANIMATION_VIEW *>(source->material_animations);
        reinterpret_cast<PC_CLONE_ANIMATION_VIEW *>(scene->material_animations)->scene = scene;
        reinterpret_cast<PC_CLONE_ANIMATION_VIEW *>(scene->material_animations)->next =
            reinterpret_cast<PC_MATERIAL_ANIM_SET *>(nudlist_field_fc8);
        nudlist_field_fc8 = reinterpret_cast<u32>(scene->material_animations);
    } else {
        scene->material_animations = NULL;
    }
    scene->local_state = reinterpret_cast<NUGLOBALRNDRSTATE_PC *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4) + sizeof(*scene->local_state);
    memset(scene->local_state, 0, sizeof(*scene->local_state));
    NuDisplaySceneClonePS(source, scene, buffer);
    PCDisplayScenes[nudlist_field_9d8++] = scene;
    ResetSceneBeforeFramePC(scene, 1);
    for (i = 0; i < scene->sort_count; ++i) {
        scene->sort_pris[i].sort_pri &= 0x1ffff;
        if (numtl_renderplane != 0)
            scene->sort_pris[i].sort_pri += numtl_renderplane << 17;
        DisplayInsertSortPC(&scene->sort_pris[i]);
    }
    scene->render_flags = (scene->render_flags & ~16u) | 0x2000;
    NuDisplaySceneAddPS(scene);
    PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    return scene;
}

typedef struct PC_SPECIAL_RENDER_MATERIAL {
    u8 unknown_0000[0x30];
    struct PC_SPECIAL_RENDER_MATERIAL *next;
    u8 unknown_0034[8];
    NUDISPLAYLIST_PC *display_list;
    u32 attributes;
    u8 unknown_0044[0x56];
    u8 override_slot;
    u8 unknown_009b[0x19];
    u32 shader_flags;
    u8 unknown_00b8[0x40];
    u8 colour[3];
    u8 unknown_00fb[0xf5];
    u32 vertex_format;
    u8 unknown_01f4[0x74];
    u8 shadow_set;
} PC_SPECIAL_RENDER_MATERIAL;
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, next) == 0x30);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, display_list) == 0x3c);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, attributes) == 0x40);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, override_slot) == 0x9a);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, shader_flags) == 0xb4);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, colour) == 0xf8);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, vertex_format) == 0x1f0);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, shadow_set) == 0x268);

typedef union PC_SPECIAL_RENDER_SPECIAL {
    NUDISPLAY_SPECIAL_PC common;
    struct {
        u8 unknown_0000[0xb0];
        PC_DISPLAY_CLIP_OBJECT_PC *clip_objects;
        u8 unknown_00b4[8];
        f32 *lod_ranges;
        u8 unknown_00c0[8];
        i16 wind_speed, wind_scale;
    };
} PC_SPECIAL_RENDER_SPECIAL;
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_SPECIAL, clip_objects) == 0xb0);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_SPECIAL, lod_ranges) == 0xbc);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_SPECIAL, wind_speed) == 0xc8);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_SPECIAL, wind_scale) == 0xca);

typedef union PC_SPECIAL_RENDER_SCENE {
    NUDISPLAY_LIST_PC common;
    struct {
        u8 unknown_0000[0xc];
        f32 *fade_ranges;
    };
} PC_SPECIAL_RENDER_SCENE;
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_SCENE, fade_ranges) == 0xc);

/* PC render-set's second list begins at +0x44, not sizeof the public list prefix. */
typedef struct PC_SHADOW_RENDER_LISTS {
    NUDISPLAYLIST_PC regular;
    u8 unknown_0028[0x1c];
    NUDISPLAYLIST_PC variant;
    u8 unknown_006c[0x314];
} PC_SHADOW_RENDER_LISTS;
STATIC_ASSERT(sizeof(PC_SHADOW_RENDER_LISTS) == 0x380);
STATIC_ASSERT(offsetof(PC_SHADOW_RENDER_LISTS, variant) == 0x44);

typedef union PC_SPECIAL_SHADOW_LIGHT_VIEW {
    struct {
        u8 unknown_0000[0x2a4];
        PC_SHADOW_RENDER_LISTS render_sets[1];
    };
    struct {
        u8 unknown_count_prefix[0x710];
        i32 render_set_count;
    };
} PC_SPECIAL_SHADOW_LIGHT_VIEW;
STATIC_ASSERT(offsetof(PC_SPECIAL_SHADOW_LIGHT_VIEW, render_sets) == 0x2a4);
STATIC_ASSERT(offsetof(PC_SPECIAL_SHADOW_LIGHT_VIEW, render_set_count) == 0x710);

extern "C" {

#define PC_LIST_ITEM (static_cast<NUDISPLAYLISTITEM *>(list->items))
    static __declspec(noinline) void DisplayListProcessGeomPC(NUDISPLAYLIST_PC *list, NUDISPLAYLISTITEM *geometry,
                                                              NUDISPLAYLISTITEM **output, NUMTX *matrix, void **packet,
                                                              f32 alpha, i32 shadow) {
        VARIPTR *buffer = NuDisplayListLinkItems(list, 2);
        NUDISPLAY_LIST_PC *scene = reinterpret_cast<NUDISPLAY_LIST_PC *>(list->scene);
        void *next = geometry->next;
        void *transform = DisplayListCreateGeomTransformPS(buffer, matrix, scene->display_materials[list->material_id],
                                                           next, *packet);
        *packet = transform;
        PC_LIST_ITEM->type = 0x8c;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = transform;
        list->items = PC_LIST_ITEM + 1;
        output[0] = PC_LIST_ITEM - 1;
        PC_LIST_ITEM->type = 0x82;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = next;
        list->items = PC_LIST_ITEM + 1;
        output[1] = PC_LIST_ITEM - 1;
        DisplayListSetAlphaPS(output[0], output[1], alpha);
        DisplayListSetShadowCasterFlagPS(output[0], output[1], shadow);
    }
    static __declspec(noinline) void DisplayListProcessLightmappedPC(NUDISPLAYLIST_PC *list,
                                                                     NUDISPLAYLISTITEM *geometry,
                                                                     NUDISPLAYLISTITEM **output, NUMTX *matrix,
                                                                     void **packet, f32 alpha) {
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        VARIPTR *buffer = NuDisplayListLinkItems(list, 3);
        void *next = geometry->next;
        PC_LIST_ITEM->type = geometry[-2].type;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = geometry[-2].next;
        list->items = PC_LIST_ITEM + 1;
        output[0] = PC_LIST_ITEM - 1;
        NUDISPLAY_LIST_PC *scene = reinterpret_cast<NUDISPLAY_LIST_PC *>(list->scene);
        void *transform = DisplayListCreateGeomTransformPS(buffer, matrix, scene->display_materials[list->material_id],
                                                           next, *packet);
        *packet = transform;
        PC_LIST_ITEM->type = 0x8c;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = transform;
        list->items = PC_LIST_ITEM + 1;
        output[1] = PC_LIST_ITEM - 1;
        PC_LIST_ITEM->type = 0x82;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = next;
        list->items = PC_LIST_ITEM + 1;
        output[2] = PC_LIST_ITEM - 1;
        DisplayListSetAlphaPS(output[1], output[2], alpha);
    }
    static __declspec(noinline) void DisplayListProcessSkinPC(NUDISPLAYLIST_PC *list, NUDISPLAYLISTITEM *geometry,
                                                              NUDISPLAYLISTITEM **output, NUMTX *matrix, void **packet,
                                                              NUMTX *skin, void *weights, i32 shadow) {
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        VARIPTR *buffer = NuDisplayListLinkItems(list, 3);
        void *next = geometry->next;
        NUDISPLAY_LIST_PC *scene = reinterpret_cast<NUDISPLAY_LIST_PC *>(list->scene);
        void *transform = DisplayListCreateGeomTransformPS(buffer, matrix, scene->display_materials[list->material_id],
                                                           next, *packet);
        *packet = transform;
        PC_LIST_ITEM->type = 0x8c;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = transform;
        list->items = PC_LIST_ITEM + 1;
        output[0] = PC_LIST_ITEM - 1;
        void *render_geometry;
        void *skin_packet = DisplayListCreateSkinTransformPS(buffer, skin, weights, next, &render_geometry, output + 2);
        PC_LIST_ITEM->type = 0x99;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = skin_packet;
        list->items = PC_LIST_ITEM + 1;
        output[1] = PC_LIST_ITEM - 1;
        PC_LIST_ITEM->type = 0x98;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = render_geometry;
        SAGA_SKIN_CALL(skin_trace_item, (unsigned)PC_LIST_ITEM, (unsigned)skin_packet, (unsigned)render_geometry,
                       (unsigned)skin);
        list->items = PC_LIST_ITEM + 1;
        output[1] = PC_LIST_ITEM - 1;
        DisplayListSetAlphaPS(output[0], output[1], 1.0f);
        DisplayListSetShadowCasterFlagPS(output[0], output[1], shadow);
    }
    static __declspec(noinline) void DisplayListProcessFaceonPC(NUDISPLAYLIST_PC *list, NUDISPLAYLISTITEM *geometry,
                                                                NUDISPLAYLISTITEM **output, NUMTX *matrix, f32 alpha) {
        VARIPTR *buffer = NuDisplayListLinkItems(list, 2);
        NUDISPLAY_LIST_PC *scene = reinterpret_cast<NUDISPLAY_LIST_PC *>(list->scene);
        void *packet = DisplayListCreateFaceonTransformPS(buffer, matrix, scene->display_materials[list->material_id],
                                                          geometry->next);
        PC_LIST_ITEM->type = 0x90;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = packet;
        list->items = PC_LIST_ITEM + 1;
        output[0] = PC_LIST_ITEM - 1;
        packet = NuDisplayListPrepareFaceonPS(buffer, geometry->next, matrix);
        PC_LIST_ITEM->type = 0x8f;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = packet;
        list->items = PC_LIST_ITEM + 1;
        output[1] = PC_LIST_ITEM - 1;
        DisplayListSetAlphaPS(output[0], output[1], alpha);
    }
#undef PC_LIST_ITEM
    static __declspec(noinline) void
    DisplayListProcessSpecialItemPC(PC_SPECIAL_RENDER_MATERIAL *material, NUDISPLAYLIST_PC *list,
                                    NUDISPLAYLISTITEM *geometry, i32 index, NUDISPLAYLISTITEM **output, NUMTX *matrix,
                                    void **packet, NUMTX *skin, void *weights, f32 alpha, i32 shadow) {
        if (geometry->type != 0x8f) {
            if (skin == NULL) {
                if ((material->shader_flags & 0x80000000u) != 0 && index > 1 &&
                    (geometry[-2].type == 0xb0 || geometry[-2].type == 0xae || geometry[-2].type == 0xaf))
                    DisplayListProcessLightmappedPC(list, geometry, output, matrix, packet, alpha);
                else
                    DisplayListProcessGeomPC(list, geometry, output, matrix, packet, alpha, shadow);
            } else {
                DisplayListProcessSkinPC(list, geometry, output, matrix, packet, skin, weights, shadow);
            }
        } else {
            DisplayListProcessFaceonPC(list, geometry, output, matrix, alpha);
        }
    }

    static __declspec(noinline) f32 DisplaySceneDistanceFadePC(f32 distance, f32 start, f32 end) {
        f32 alpha;
        if (end > start) {
            alpha = 1.0f;
            if (distance > start * start) {
                alpha = (end - NuFsqrt(distance)) / (end - start);
                if (alpha < 0.0f)
                    alpha = 0.0f;
            }
        } else {
            alpha = 0.0f;
            if (distance > end * end) {
                alpha = (NuFsqrt(distance) - end) / (start - end);
                if (alpha > 1.0f)
                    alpha = 1.0f;
            }
        }
        return alpha;
    }

    i32 __cdecl NuDisplayListRndrSpecial(NUSPECIALHANDLE *handle, NUMTX *matrix, i32 skinned, NUMTX *skin_matrices,
                                         void *blend_values) {
        NUMTX wind_matrix;
        NUVEC center, minimum, maximum;
        void *transform_packet = NULL;
        NUDISPLAYLISTITEM *first_and_last[3];
        (void)skinned;
        if (handle == NULL)
            return 0;
        PC_SPECIAL_RENDER_SPECIAL *special = reinterpret_cast<PC_SPECIAL_RENDER_SPECIAL *>(handle->display_special);
        if (special->wind_scale != 0) {
            WindShear(&wind_matrix, matrix, special->wind_scale, special->wind_speed);
            matrix = &wind_matrix;
        }
        i32 clip = PCSpecialClipState;
        if (clip == -1)
            clip = NuCameraClipTestExtents(reinterpret_cast<NUVEC *>(&special->common.bounds_min),
                                           reinterpret_cast<NUVEC *>(&special->common.bounds_max), matrix, 0.0f, 0);
        NUDISPLAY_LIST_PC *scene = static_cast<NUGSCN *>(handle->scene)->display_list;
        i32 previous_shadow = 0, shadow_clip = 0;
        if ((scene->visibility_flags[special->common.instance_ix] & 0x20) != 0 && PCSpecialActiveShadowLights > 0) {
            previous_shadow = PCSpecialShadowClipResultsPresent;
            NuVecMtxTransform(&minimum, reinterpret_cast<NUVEC *>(&special->common.bounds_min), matrix);
            NuVecMtxTransform(&maximum, reinterpret_cast<NUVEC *>(&special->common.bounds_max), matrix);
            shadow_clip = NuSpecialClipTestShadowLights(&minimum, &maximum, 0);
        }
        if (clip == 0 && shadow_clip == 0)
            return 0;
        NuVecAdd(&center, reinterpret_cast<NUVEC *>(&special->common.bounds_min),
                 reinterpret_cast<NUVEC *>(&special->common.bounds_max));
        NuVecScale(&center, &center, 0.5f);
        NuVecMtxTransform(&center, &center, matrix);
        NUVEC camera_position = {global_camera.mtx.m30, global_camera.mtx.m31, global_camera.mtx.m32};
        f32 distance = NuVecDistSqr(&center, &camera_position, NULL);
        if (distance < 0.0f)
            distance = 0.0f;
        PC_DISPLAY_CLIP_OBJECT_PC *object = special->clip_objects;
        if (special->lod_ranges[0] != 0.0f) {
            i32 lod = 0;
            if (PCSpecialForcedLOD != 0) {
                while (special->lod_ranges[lod] != 0.0f && lod != PCSpecialForcedLOD)
                    ++lod;
            } else {
                while (distance < special->lod_ranges[lod])
                    ++lod;
            }
            object += lod;
        }
        f32 alpha = 1.0f;
        PC_SPECIAL_RENDER_SCENE *scene_view = reinterpret_cast<PC_SPECIAL_RENDER_SCENE *>(scene);
        if (scene_view->fade_ranges != NULL && (scene->visibility_flags[special->common.instance_ix] & 0x40) != 0) {
            alpha = DisplaySceneDistanceFadePC(distance, scene_view->fade_ranges[special->common.instance_ix * 2],
                                               scene_view->fade_ranges[special->common.instance_ix * 2 + 1]);
            if (alpha == 0.0f)
                return 0;
        }
        for (u32 i = 0; i < static_cast<u32>(object->material_count); ++i) {
            PC_SPECIAL_RENDER_MATERIAL *material =
                reinterpret_cast<PC_SPECIAL_RENDER_MATERIAL *>(scene->display_materials[object->material_ids[i]]);
            NUMTL *replacement;
            if (PCSpecialForcedMaterialTable != NULL && material->override_slot != 0)
                replacement = PCSpecialForcedMaterialTable[material->override_slot];
            else
                replacement = PCSpecialForcedMaterial;
            if (replacement != NULL) {
                PC_SPECIAL_RENDER_MATERIAL *replacement_view =
                    reinterpret_cast<PC_SPECIAL_RENDER_MATERIAL *>(replacement);
                if (material->vertex_format != replacement_view->vertex_format)
                    material = reinterpret_cast<PC_SPECIAL_RENDER_MATERIAL *>(
                        NuMtlFindVariantMtl(reinterpret_cast<NUMTL *>(material), replacement));
                else
                    material = replacement_view;
            }
            for (; material != NULL; material = material->next) {
                if (PCDeviceHardwareVertexProcessingPC == 1) {
                    if (material->colour[0] != 255 &&
                        material->colour[0] + material->colour[1] + material->colour[2] != 0)
                        continue;
                } else if (material->colour[0] == 255)
                    continue;
                if (clip != 0) {
                    NUDISPLAYLIST_PC *list = material->display_list;
                    list->scene->render_flags |= 2;
                    u8 *used = (list->scene->render_flags & 0x8000) != 0 ? list->scene->material_used_back
                                                                         : list->scene->material_used_front;
                    used[list->material_id / 8] |= static_cast<u8>(1u << (list->material_id % 8));
                    RndrStateSetConstAlphaTint(nuspecial_const_alpha_enabled, nuspecial_const_tint_enabled,
                                               nuspecial_const_alpha, &nuspecial_const_tint,
                                               reinterpret_cast<NUMTL *>(material));
                    DisplayListUpdateRenderState(list, &render_state);
                    i32 shadow_caster = (scene->visibility_flags[special->common.instance_ix] & 0x20) != 0 &&
                                        PCSpecialReflectionPass == 0;
                    i32 index = object->item_indices[i];
                    NUDISPLAYLISTITEM *geometry = static_cast<NUDISPLAYLISTITEM *>(scene->items) + index;
                    DisplayListProcessSpecialItemPC(material, list, geometry, index, first_and_last, matrix,
                                                    &transform_packet, skin_matrices, blend_values, alpha,
                                                    shadow_caster);
                }
                u32 alpha_mode = (material->attributes >> 14) & 3;
                if (shadow_clip != 0 && PCSpecialReflectionPass == 0 && alpha_mode != 1 && alpha_mode != 3) {
                    for (i32 light_index = 0; light_index < PCSpecialActiveShadowLights; ++light_index) {
                        PC_SPECIAL_SHADOW_LIGHT_VIEW *light =
                            reinterpret_cast<PC_SPECIAL_SHADOW_LIGHT_VIEW *>(PCSpecialShadowLights[light_index]);
                        u32 visible =
                            PCSpecialShadowClipResultsPresent == 0 ? ~0u : PCSpecialShadowClipResults[light_index];
                        for (i32 set = 0; set < light->render_set_count; ++set) {
                            if (((visible >> (set & 31)) & 1) == 0)
                                continue;
                            NUDISPLAYLIST_PC *list = material->shadow_set != 0 ? &light->render_sets[set].variant
                                                                               : &light->render_sets[set].regular;
                            list->scene = reinterpret_cast<NUDISPLAYSCENE_PC *>(scene);
                            NuDisplayListLinkItems(list, 1);
                            NUDISPLAYLISTITEM *item = static_cast<NUDISPLAYLISTITEM *>(list->items);
                            item->type = 0x80;
                            item->id = 3;
                            item->next = material;
                            list->items = item + 1;
                            DisplayListUpdateRenderStateShadow(list, &render_state);
                            i32 index = object->item_indices[i];
                            NUDISPLAYLISTITEM *geometry = static_cast<NUDISPLAYLISTITEM *>(scene->items) + index;
                            DisplayListProcessSpecialItemPC(material, list, geometry, index, first_and_last, matrix,
                                                            &transform_packet, skin_matrices, blend_values, alpha, 1);
                        }
                    }
                }
            }
        }
        RndrStateSetConstAlphaTint(0, 0, 0.0f, NULL, NULL);
        if (previous_shadow == 0)
            PCSpecialShadowClipResultsPresent = 0;
        return clip;
    }

    i32 __cdecl NuSpecialDrawAt(NUSPECIALHANDLE *handle, NUMTX *matrix) {
        if (handle == NULL || handle->scene == NULL || handle->display_special == NULL)
            return 0;
        return NuDisplayListRndrSpecial(handle, matrix, 0, NULL, NULL);
    }
}

extern "C" i32 __cdecl NuSpecialDrawAtAlpha(NUSPECIALHANDLE *handle, NUMTX *matrix, f32 alpha) {
    i32 result;
    if (handle->scene == NULL || alpha <= 0.0f)
        return 0;
    if (alpha < 1.0f) {
        NuSpecialConstAlpha(1, alpha);
        result = NuDisplayListRndrSpecial(handle, matrix, 0, NULL, NULL);
        NuSpecialConstAlpha(0, 0.0f);
        return result;
    }
    return NuDisplayListRndrSpecial(handle, matrix, 0, NULL, NULL);
}

typedef struct display_list_allocation_pc_s {
    NUDISPLAYLIST base;
    u8 unknown_0038[0xc];
} DISPLAY_LIST_ALLOCATION_PC;
STATIC_ASSERT(sizeof(DISPLAY_LIST_ALLOCATION_PC) == 0x44);
STATIC_ASSERT(offsetof(DISPLAY_LIST_ALLOCATION_PC, base.first) == 0x10);
typedef struct display_material_allocation_pc_s {
    NUMTL base;
    u8 unknown_tail[0x2c4 - sizeof(NUMTL)];
} DISPLAY_MATERIAL_ALLOCATION_PC;
STATIC_ASSERT(sizeof(DISPLAY_MATERIAL_ALLOCATION_PC) == 0x2c4);
extern "C" void __cdecl NuDisplayListCreate(NUDISPLAY_LIST_PC *scene, VARIPTR *buffer, VARIPTR end, i32 item_count,
                                            i32 material_count, i32 unused5, i32 unused6, i32 sort_count, i32 unused8,
                                            i32 allocate_materials) {
    i32 i, used_size;
    DISPLAY_LIST_ALLOCATION_PC *lists;
    NUDISPLAYLISTITEM *first;
    (void)unused5;
    (void)unused6;
    (void)unused8;
#define DLIST_ALIGN(n) (((n) + 15) & ~15u)
#define DLIST_ALLOC(dst, type, size)                                                                                   \
    (dst) = (type)DLIST_ALIGN(buffer->addr);                                                                           \
    buffer->addr = DLIST_ALIGN(buffer->addr) + (size)
    buffer->addr = DLIST_ALIGN(buffer->addr);
    scene->item_count = item_count;
    DLIST_ALLOC(scene->items, void *, item_count * sizeof(NUDISPLAYLISTITEM));
    scene->material_count = material_count;
    buffer->addr = DLIST_ALIGN(buffer->addr);
    DLIST_ALLOC(scene->display_materials, NUMTL **, material_count * sizeof(NUMTL *));
    DLIST_ALLOC(scene->material_lists, NUDISPLAYLIST **, material_count * sizeof(NUDISPLAYLIST *));
    memset(scene->display_materials, 0, material_count * sizeof(NUMTL *));
    memset(scene->material_lists, 0, material_count * sizeof(NUDISPLAYLIST *));
    if (allocate_materials != 0) {
        DISPLAY_MATERIAL_ALLOCATION_PC *materials;
        DLIST_ALLOC(materials, DISPLAY_MATERIAL_ALLOCATION_PC *, material_count * sizeof(*materials));
        for (i = 0; i < material_count; ++i)
            scene->display_materials[i] = &materials[i].base;
    }
    DLIST_ALLOC(lists, DISPLAY_LIST_ALLOCATION_PC *, material_count * sizeof(*lists));
    memset(lists, 0, material_count * sizeof(*lists));
    DLIST_ALLOC(first, NUDISPLAYLISTITEM *, material_count * sizeof(*first));
    memset(first, 0, material_count * sizeof(*first));
    for (i = 0; i < material_count; ++i) {
        scene->material_lists[i] = &lists->base;
        lists->base.state = (NURNDRLIBSTATE *)((buffer->addr + 3) & ~3u);
        buffer->addr = ((buffer->addr + 3) & ~3u) + sizeof(NURNDRLIBSTATE);
        lists->base.first = first;
        NuDisplayListResetStateCache(lists->base.state);
        ++first;
        ++lists;
    }
    scene->local_state = NULL;
    for (i = 0; (u32)i < scene->material_count; ++i) {
        NUDISPLAYLISTITEM *item = scene->material_lists[i]->first;
        item->type = 0x8d;
        item->next = NULL;
        item->id = 1;
    }
    used_size = (((material_count + 7) / 8) / 16 + 1) * 16;
    DLIST_ALLOC(scene->material_used[0], u8 *, used_size);
    DLIST_ALLOC(scene->material_used[1], u8 *, used_size);
    scene->sort_count = sort_count;
    scene->sort_pris = (NUSORTPRI *)buffer->void_ptr;
    buffer->addr += sort_count * sizeof(NUSORTPRI);
    memset(scene->sort_pris, 0, sort_count * sizeof(NUSORTPRI));
    for (i = 0; i < sort_count; ++i) {
        scene->sort_pris[i].display_scene = scene;
        scene->sort_pris[i].nmtls = 0;
        scene->sort_pris[i].mtl_first = 0;
    }
#undef DLIST_ALLOC
#undef DLIST_ALIGN
    DisplayListCreatePS(scene, buffer, end);
    ResetSceneBeforeFramePC(scene, 1);
}

typedef struct display_scene_name_pc_s {
    const char *name;
} DISPLAY_SCENE_NAME_PC;
STATIC_ASSERT(offsetof(DISPLAY_SCENE_NAME_PC, name) == 0);
extern "C" void __cdecl NuDisplayListCreateDynMtlList_candidate(VARIPTR *buffer, VARIPTR end) {
    i32 i;
    NUDISPLAYLISTITEM *material_item;
    NuDisplayListCreate(&PCDynamicDisplayScene, buffer, end, PCMaterialBlockItemCount * 600, 600, 0, 0, 600, 0, 0);
    PCDynamicDisplayScene.sort_count = 0;
    ((DISPLAY_SCENE_NAME_PC *)&PCDynamicDisplayScene)->name = PCDynamicDisplaySceneNamePointer;
    material_item = (NUDISPLAYLISTITEM *)PCDynamicDisplayScene.items;
    for (i = 0; i < 600; ++i) {
        NUDISPLAYLIST *list = PCDynamicDisplayScene.material_lists[i];
        list->mtl_item = material_item;
        list->dynamic_geometry = material_item + PCMaterialGeometryItemIndex;
        list->scene = &PCDynamicDisplayScene;
        list->material_id = i;
        material_item += PCMaterialBlockItemCount;
    }
    buffer->addr = (buffer->addr + 15) & ~15u;
    PCDisplayMaterialAddCount = 0;
    PCDisplayMaterialAddQueue = (NUMTL **)buffer->void_ptr;
    buffer->addr += 600 * sizeof(NUMTL *);
    PCDisplayMaterialDeleteCount = 0;
    PCDisplayMaterialDeleteQueue = (NUMTL **)buffer->void_ptr;
    buffer->addr += 600 * sizeof(NUMTL *);
    PCDisplayMaterialReferenceCounts = (u8 *)buffer->void_ptr;
    buffer->addr += 600;
    PCDisplayMaterialStreamUsed = (u8 *)buffer->void_ptr;
    buffer->addr += 600;
    memset(PCDisplayMaterialReferenceCounts, 0, 600);
    memset(PCDisplayMaterialStreamUsed, 0, 600);
    buffer->addr = (buffer->addr + 15) & ~15u;
    PCDisplayMaterialStreamStart = *buffer;
    buffer->addr += 0x12c00;
    PCDisplayMaterialStreamEnd = *buffer;
    PCDisplayList2dPc.first->type = 0x8d;
    PCDisplayList2dPc.first->id = 1;
    PCDisplayList2dPc.first->next = NULL;
    PCDisplayList2dPc.mtl_last = PCDisplayList2dPc.first;
    buffer->addr = (buffer->addr + 3) & ~3u;
    PCDisplayList2dPc.state = (NURNDRLIBSTATE *)buffer->void_ptr;
    buffer->addr += sizeof(NURNDRLIBSTATE);
    NuDisplayListResetStateCache((NURNDRLIBSTATE *)PCDisplayList2dPc.state);
    PCDynamicDisplayScene.render_flags |= 0x10;
    PCDisplayList2dPc.mtl_last = PCDisplayList2dPc.first;
}

extern "C" void __cdecl NuDisplayListExecute(NUDISPLAYLISTITEM *item, void(__cdecl **handlers)(void *)) {
    for (;;) {
        switch (item->id) {
            case 3: {
                void(__cdecl * handler)(void *) = handlers[item->type - 0x80];
                if (handler != NULL) {
                    SAGA_SKIN_CALL(skin_trace_enter_item, (unsigned)item);
                    handler(item->next);
                    SAGA_SKIN_CALL(skin_trace_leave_item);
                }
            }
            /* fall through */
            case 0:
                ++item;
                continue;
            case 1:
                item = (NUDISPLAYLISTITEM *)item->next;
                continue;
            case 4:
                break;
            default:
                for (;;) {
                }
        }
        break;
    }
}

union PC_CLIP_SCENE_VIEW {
    NUDISPLAY_LIST_PC common;
    struct {
        u8 unknown_00[0xc];
        f32 *fade_ranges;
        u8 unknown_10[0x1c];
        f32 *far_clip_ranges;
    };
};
STATIC_ASSERT(sizeof(PC_CLIP_SCENE_VIEW) == sizeof(NUDISPLAY_LIST_PC));
STATIC_ASSERT(offsetof(PC_CLIP_SCENE_VIEW, fade_ranges) == 0xc);
STATIC_ASSERT(offsetof(PC_CLIP_SCENE_VIEW, far_clip_ranges) == 0x2c);

static void DisplayClipFallbackPC(NUDISPLAY_LIST_PC *scene, VISIBILITY_RESULT_READER_PC *visibility) {
    PC_CLIP_SCENE_VIEW *view = reinterpret_cast<PC_CLIP_SCENE_VIEW *>(scene);
    i32 instance_index = 0;
    i32 fade_mask = 0;
    i32 clip_index;
    scene->alpha_values = NULL;
    if (view->fade_ranges != NULL) {
        VARIPTR *cursor;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        cursor = NuDisplayListStreamCursor;
        scene->alpha_values = reinterpret_cast<f32 *>((cursor->addr + 3) & ~3u);
        cursor->addr = (cursor->addr + 3) & ~3u;
        cursor->addr += scene->clip_count * sizeof(f32);
        fade_mask = 0x40;
    }
    for (clip_index = 0; clip_index < scene->clip_count; ++clip_index, ++instance_index) {
        i32 visible = 1;
        i32 flags;
        if (visibility != NULL && visibility->instance_tree_bits != NULL)
            visible = (visibility->instance_tree_bits[instance_index >> 3] >> (instance_index % 8)) & 1;
        flags = static_cast<i8>(scene->visibility_flags[instance_index]);
        if (visibility->portal_marker != NULL && PCPortalsEnabled != 0) {
            if ((flags & 8) != 0)
                goto check_flags;
            visible = (visibility->portal_bits[instance_index >> 3] >> (instance_index & 7)) & 1;
        }
        if (visible != 1)
            goto next;
    check_flags:
        if ((flags & 1) != 0) {
            NUVEC4 *center = &scene->clip_bounds[instance_index * 2];
            __declspec(align(16)) NUVEC delta;
            f32 distance, far_clip;
            i32 fades, result;
            delta.x = global_camera.mtx.m30 - center->x;
            delta.y = global_camera.mtx.m31 - center->y;
            delta.z = global_camera.mtx.m32 - center->z;
            distance = (delta.x * delta.x + delta.y * delta.y) + delta.z * delta.z;
            if (scene->fade_levels[clip_index] != 0.0f) {
                while (distance < scene->fade_levels[clip_index])
                    ++clip_index;
            }
            far_clip = view->far_clip_ranges[clip_index];
            fades = flags & fade_mask;
            if (fades != 0 && view->fade_ranges[instance_index * 2 + 1] > view->fade_ranges[instance_index * 2])
                far_clip = view->fade_ranges[instance_index * 2 + 1];
            result = NuCameraClipTestExtentsAxisAligned(reinterpret_cast<NUVEC *>(center),
                                                        reinterpret_cast<NUVEC *>(center + 1), far_clip);
            if (result != 0 && NuOcclusionManagerIsEnabled() != 0) {
                NUVEC minimum, maximum;
                center = &scene->clip_bounds[instance_index * 2];
                minimum.x = center->x - center[1].x;
                minimum.y = center->y - center[1].y;
                minimum.z = center->z - center[1].z;
                maximum.x = center[1].x + center->x;
                maximum.y = center[1].y + center->y;
                maximum.z = center[1].z + center->z;
                if (reinterpret_cast<OcclusionManager *>(&g_OcclusionManager)->IsOccludedOBB(&minimum, &maximum, NULL))
                    goto next;
            }
            if (result != 0) {
                i32 material;
                if (scene->alpha_values != NULL) {
                    scene->alpha_values[clip_index] = 1.0f;
                    if (fades != 0)
                        scene->alpha_values[clip_index] = DisplaySceneDistanceFadePC(
                            distance, view->fade_ranges[instance_index * 2], view->fade_ranges[instance_index * 2 + 1]);
                }
                for (material = 0; material < scene->clip_objects[clip_index].material_count; ++material) {
                    u32 index = (u32)scene->clip_objects[clip_index].material_ids[material];
                    scene->material_used[(scene->render_flags >> 15) & 1][index >> 3] |= (u8)(1u << (index & 7));
                }
                scene->clip_used[(scene->render_flags >> 15) & 1][clip_index >> 2] |=
                    static_cast<u8>(result << ((clip_index & 3) * 2));
            }
        }
    next:
        while (scene->fade_levels[clip_index] != 0.0f)
            ++clip_index;
    }
}

extern "C" {
    VISIBILITY_RESULT_READER_PC *__cdecl NuVisiEvaluate(NUGSCN *, void *);
    void __cdecl DisplayGenerateTransformsNoOpPc(NUDISPLAY_LIST_PC *);
    extern i32 DisplaySceneRenderDisabledPc;
    extern i32 DisplaySceneSpecialsDisabledPc;
    extern i32 VisibilityPortalFilterEnabledPc;
}

typedef union visibility_display_special_reader_pc_u {
    NUDISPLAY_SPECIAL_PC common;
    struct {
        u8 unknown_00[0x40];
        NUMTX draw_matrix;
        u8 unknown_80[0x44];
        NUMTX *draw_matrix_override;
    };
} VISIBILITY_DISPLAY_SPECIAL_READER_PC;
STATIC_ASSERT(sizeof(VISIBILITY_DISPLAY_SPECIAL_READER_PC) == 0xd0);
STATIC_ASSERT(offsetof(VISIBILITY_DISPLAY_SPECIAL_READER_PC, draw_matrix) == 0x40);
STATIC_ASSERT(offsetof(VISIBILITY_DISPLAY_SPECIAL_READER_PC, draw_matrix_override) == 0xc4);

extern "C" void __cdecl NuDisplaySceneRndr(NUDISPLAY_LIST_PC *scene) {
    u8 visibility_context[0x2800];
    NUGSCN temporary_scene;
    VISIBILITY_RESULT_READER_PC *visibility;
    NUGSCN *gscene;
    NUSPECIALHANDLE handle;
    i32 portal_filter;
    if (DisplaySceneRenderDisabledPc != 0)
        return;
    if ((scene->render_flags & 4) != 0) {
        /* Genuine mapped RET diagnostic; native caller pushes no context. */
        reinterpret_cast<void(__cdecl *)(void)>(PCAnimationLoadFailureNoOp)();
        return;
    }
    RndrStateCopyGlobalState(scene->local_state);
    visibility = NuVisiEvaluate(scene->gscene, visibility_context);
    if (PCDisplayClipSet != NULL) {
        if (visibility == NULL || (visibility->flags & 1) == 0)
            DisplayClipMaterialsPC(scene);
    } else {
        if (visibility == NULL || (visibility->flags & 1) == 0)
            DisplayClipFallbackPC(scene, visibility);
    }
    if (scene->clip_count != 0)
        scene->render_flags |= 4;
    DisplayGenerateTransformsNoOpPc(scene);
    gscene = scene->gscene;
    if (gscene == NULL) {
        /* Native writes only the required display_list slot in this local. */
        temporary_scene.display_list = scene;
        gscene = &temporary_scene;
    }
    portal_filter = VisibilityPortalFilterEnabledPc != 0 && visibility->portal_marker != NULL && PCPortalsEnabled != 0;
    if ((scene->instance_visibility_enabled & 1) != 0 || DisplaySceneSpecialsDisabledPc != 0)
        return;
    handle.scene = gscene;
    for (i32 i = 0; i < scene->special_count; ++i) {
        VISIBILITY_DISPLAY_SPECIAL_READER_PC *special =
            reinterpret_cast<VISIBILITY_DISPLAY_SPECIAL_READER_PC *>(&scene->specials[i]);
        if ((special->common.flags & 2) == 0)
            continue;
        if (portal_filter != 0 && (scene->visibility_flags[special->common.instance_ix] & 8) == 0 &&
            ((visibility->portal_bits[special->common.instance_ix >> 3] >> (special->common.instance_ix & 7)) & 1) == 0)
            continue;
        handle.display_special = reinterpret_cast<NUMTX *>(special);
        NUMTX *matrix = special->draw_matrix_override;
        if (matrix == NULL || matrix == reinterpret_cast<NUMTX *>(-1))
            matrix = &special->draw_matrix;
        NuDisplayListRndrSpecial(&handle, matrix, 0, NULL, NULL);
    }
}

extern "C" void PCThreadCriticalSectionEnter_candidate(i32);
extern "C" void PCThreadCriticalSectionLeave_candidate(i32);
extern "C" void NuDisplayListAnimateMtls(f32);
typedef struct FRAME_MTL_ANIM_DESC_PC {
    u8 u_mode, v_mode, reserved02[2];
    f32 u_amplitude, v_amplitude, u_frequency, v_frequency;
} FRAME_MTL_ANIM_DESC_PC;
STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_DESC_PC) == 0x14);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_frequency) == 0xc);
typedef struct FRAME_MTL_ANIM_PC {
    u8 reserved00[0x7b];
    u8 disable_u_animation, disable_v_animation;
    u8 reserved7d[0x143];
    i32 texture_animation_data[4];
    f32 texture_animation_offsets[4][2];
    u8 reserved1f0[8];
    FRAME_MTL_ANIM_DESC_PC animation[4];
} FRAME_MTL_ANIM_PC;
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_u_animation) == 0x7b);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_v_animation) == 0x7c);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_data) == 0x1c0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_offsets) == 0x1d0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, animation) == 0x1f8);
typedef struct FRAME_MTL_ANIM_SET_PC {
    NUDISPLAY_LIST_PC *scene;
    i32 material_count;
    i32 *material_indices;
    struct FRAME_MTL_ANIM_SET_PC *next;
} FRAME_MTL_ANIM_SET_PC;
STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_SET_PC) == 0x10);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, next) == 0xc);
typedef struct FRAME_MTL_SCENE_PC {
    u8 reserved00[0x158];
    i32 shader_texture_animation_enabled;
} FRAME_MTL_SCENE_PC;
STATIC_ASSERT(offsetof(FRAME_MTL_SCENE_PC, shader_texture_animation_enabled) == 0x158);
static f32 FrameMaterialAnimationTimePc = 0;
extern "C" void __cdecl NuMtlSetUVOffsetPS(NUMTL *mtl, u32 layer, f32 u, f32 v);
void __cdecl NuDisplayListAnimateMtls(f32 frame_time) {
    FRAME_MTL_ANIM_SET_PC *set;
    FrameMaterialAnimationTimePc += frame_time;
    PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
    for (set = (FRAME_MTL_ANIM_SET_PC *)nudlist_field_fc8; set != NULL; set = set->next) {
        i32 index;
        NUDISPLAY_LIST_PC *scene = set->scene;
        for (index = 0; index < set->material_count; ++index) {
            FRAME_MTL_ANIM_PC *material = (FRAME_MTL_ANIM_PC *)scene->display_materials[set->material_indices[index]];
            u32 layer;
            if (material->disable_u_animation == 1 || material->disable_v_animation == 1)
                continue;
            for (layer = 0; layer < 4; ++layer) {
                FRAME_MTL_ANIM_DESC_PC *animation;
                f32 u = 0.0f, v = 0.0f;
                if (material->texture_animation_data[layer] == -1)
                    continue;
                animation = &material->animation[layer];
                switch (animation->u_mode) {
                    case 2:
                        material->texture_animation_offsets[layer][0] += frame_time * animation->u_frequency;
                        material->texture_animation_offsets[layer][0] -=
                            NuFloor(material->texture_animation_offsets[layer][0]);
                        u = material->texture_animation_offsets[layer][0];
                        break;
                    case 3:
                        u = NuTrigTable[((i32)(animation->u_frequency * FrameMaterialAnimationTimePc * 65536.0) >> 1) &
                                        0x7fff] *
                            animation->u_amplitude;
                        break;
                    case 4:
                        u = NuTrigTable[((0x4000 -
                                          (i32)(animation->u_frequency * FrameMaterialAnimationTimePc * -65536.0)) >>
                                         1) &
                                        0x7fff] *
                            animation->u_amplitude;
                        break;
                }
                switch (animation->v_mode) {
                    case 2:
                        material->texture_animation_offsets[layer][1] += frame_time * animation->v_frequency;
                        material->texture_animation_offsets[layer][1] -=
                            NuFloor(material->texture_animation_offsets[layer][1]);
                        v = material->texture_animation_offsets[layer][1];
                        break;
                    case 3:
                        v = NuTrigTable[((i32)(animation->v_frequency * FrameMaterialAnimationTimePc * 65536.0) >> 1) &
                                        0x7fff] *
                            animation->v_amplitude;
                        break;
                    case 4:
                        v = NuTrigTable[((0x4000 -
                                          (i32)(animation->v_frequency * FrameMaterialAnimationTimePc * -65536.0)) >>
                                         1) &
                                        0x7fff] *
                            animation->v_amplitude;
                        break;
                }
                NuMtlSetUVOffsetPS((NUMTL *)material, layer, u, v);
            }
        }
    }
    PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
}
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_mode) == 0x0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_mode) == 0x1);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_amplitude) == 0x4);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_amplitude) == 0x8);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_frequency) == 0x10);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, scene) == 0x0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_count) == 0x4);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_indices) == 0x8);

extern "C" void __cdecl NuDisplayListPurge_candidate(void) {
    if (nudlist_field_9d0 != 0) {
        nudlist_frame_active = (i32)(nudlist_field_9d0 - 1U);
        nudlist_field_9d0 = 0;
        nudlist_field_fe4 = 1;
        NUFILE file = NuFileOpen("dlist.htm", 1);
        nudlist_field_958 = file;
        nudlist_field_988 = file;
        PCDisplayCaptureCursorPc = PCDisplayCaptureBufferPc;
        nudlist_field_18b0 = PCDisplayCaptureFlushPointPc;
        NuHtmlWrite("<body bgcolor=#C0C0C0>");
    }
}

extern "C" void __cdecl NuHtmlFlushPc(void);

extern "C" void __cdecl NuDisplayListHtmlEndPc(void) {
    if (nudlist_field_fe4 != 0) {
        NuHtmlWrite("<table   width=100%c height=20 bgcolor=#FF0000><tr><td><font face=arial size=+2></font></table>",
                    0x25);
        NuHtmlFlushPc();
        NuFileClose(nudlist_field_958);
        nudlist_field_fe4 = 0;
    }
}

/* The native debug-name projection reads one pointer at scene+0. Historical
 * visibility/owning extent of this projection is not established. */
typedef struct PC_DISPLAY_DEBUG_NAME_READER {
    const char *name;
} PC_DISPLAY_DEBUG_NAME_READER;
STATIC_ASSERT(sizeof(PC_DISPLAY_DEBUG_NAME_READER) == 4);
STATIC_ASSERT(offsetof(PC_DISPLAY_DEBUG_NAME_READER, name) == 0);
STATIC_ASSERT(offsetof(NUDISPLAYLISTRENDERSCENE, nsort_pris) == 0);
STATIC_ASSERT(offsetof(NUDISPLAYLISTRENDERSCENE, sort_pris) == 4);
STATIC_ASSERT(offsetof(NUSORTPRI, sort_pri) == 0);

extern "C" {
    extern void(__cdecl **PCDisplayItemHandlersPc)(void *);
}

extern "C" void __cdecl NuDisplayListDrawItemsPc(NUDISPLAYLISTITEM *items) {
    NuDisplayListExecute(items, PCDisplayItemHandlersPc);
}

extern "C" void __cdecl NuDisplayListDrawRenderScene_candidate(i32 scene_id) {
    PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
    i32 count = PCSafeRenderScenes[scene_id]->nsort_pris;
    for (i32 index = 0; index < count; ++index) {
        NUSORTPRI *sort = PCSafeRenderScenes[scene_id]->sort_pris[index];
        if (nudlist_field_fe4 != 0) {
            PC_DISPLAY_DEBUG_NAME_READER *debug_scene =
                reinterpret_cast<PC_DISPLAY_DEBUG_NAME_READER *>(sort->display_scene);
            if (debug_scene != NULL && debug_scene->name != NULL) {
                STUBBED_NAMED(0x005d72b0); /* native5f033d: heading(name,sort_pri), ignored VOID */
            } else {
                STUBBED_NAMED(0x005d72b0); /* native5f034f: UNKNOWN heading(sort_pri), ignored VOID */
            }
            STUBBED_NAMED(0x005d7230); /* native5f035c: font HTML, ignored VOID */
            STUBBED_NAMED(0x005e09a0); /* native5f0371: EAX=sort->items; stack(3,0,NULL,nudlist_field_958).
                                       Genuine original private register ABI is held, no public wrapper. */
        }
        NuDisplayListDrawItemsPc(sort->items);
    }
    NuDisplayListDrawItemsPc(&PCSafeRenderScenes[scene_id]->render_2d_first);
    PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
}

extern "C" void __cdecl DisplaySceneRndrSpecials(NUDISPLAY_LIST_PC *scene, i32 unused0, i32 unused1) {
    NUGSCN temporary_scene;
    VISIBILITY_RESULT_READER_PC *visibility = NuVisiEvaluate(scene->gscene, &unused1);
    i32 portal_filter = visibility != NULL && VisibilityPortalFilterEnabledPc != 0 &&
                        visibility->portal_marker != NULL && PCPortalsEnabled != 0;
    i32 shadow_filter = currentScene.dyn_light_ptr != NULL;
    if ((scene->instance_visibility_enabled & 1) != 0 || DisplaySceneSpecialsDisabledPc != 0)
        return;
    NUGSCN *gscene = scene->gscene;
    if (gscene == NULL) {
        temporary_scene.display_list = scene;
        gscene = &temporary_scene;
    }
    NUSPECIALHANDLE handle;
    handle.scene = gscene;
    for (i32 i = 0; i < scene->special_count; ++i) {
        VISIBILITY_DISPLAY_SPECIAL_READER_PC *special =
            reinterpret_cast<VISIBILITY_DISPLAY_SPECIAL_READER_PC *>(&scene->specials[i]);
        if ((special->common.flags & 2) == 0)
            continue;
        if (shadow_filter != 0 && (scene->visibility_flags[special->common.instance_ix] & 0x20) == 0)
            continue;
        if (portal_filter != 0 && (scene->visibility_flags[special->common.instance_ix] & 8) == 0 &&
            ((visibility->portal_bits[(i32)special->common.instance_ix >> 3] >> (special->common.instance_ix & 7)) &
             1) == 0)
            continue;
        handle.display_special = reinterpret_cast<NUMTX *>(special);
        NUMTX *matrix = special->draw_matrix_override;
        if (matrix == NULL || matrix == reinterpret_cast<NUMTX *>(-1))
            matrix = &special->draw_matrix;
        NuDisplayListRndrSpecial(&handle, matrix, 0, NULL, NULL);
    }
}

STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, clip_objects) == 0xb0);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, clip_range) == 0xbc);

extern "C" void __cdecl NuDisplayListBurstRndrSpecial(nuhspecial_s *handle, u32 count, NUMTX *matrices, i32 clip) {
    u16 visible[1024];
    if (handle == NULL)
        return;
    NUDISPLAY_SPECIAL_PC *special = handle->display_special;
    u32 visible_count;
    if (clip != 0) {
        visible_count = 0;
        for (u32 i = 0; i < count; ++i) {
            i32 result =
                NuCameraClipTestExtents(reinterpret_cast<NUVEC *>(&special->bounds_min),
                                        reinterpret_cast<NUVEC *>(&special->bounds_max), &matrices[i], 0.0f, 0);
            if (static_cast<u8>(result) != 0)
                visible[visible_count++] = static_cast<u16>(i | (result << 8));
        }
    } else {
        visible_count = count;
    }
    if (visible_count == 0)
        return;
    PC_DISPLAY_CLIP_OBJECT_PC *object = static_cast<PC_DISPLAY_CLIP_OBJECT_PC *>(special->clip_objects);
    f32 *range = special->clip_range;
    while (*range > 0.0f && object->material_count == 0) {
        ++range;
        ++object;
    }
    NUDISPLAY_LIST_PC *scene = handle->scene->display_list;
    scene->render_flags |= 2;
    for (u32 material = 0; material < static_cast<u32>(object->material_count); ++material) {
        PC_SPECIAL_RENDER_MATERIAL *material_view =
            reinterpret_cast<PC_SPECIAL_RENDER_MATERIAL *>(scene->display_materials[object->material_ids[material]]);
        NUDISPLAYLIST *list = reinterpret_cast<NUDISPLAYLIST *>(material_view->display_list);
        scene->material_used[(scene->render_flags >> 15) & 1][list->material_id / 8] |= 1 << (list->material_id % 8);
        DisplayListUpdateRenderState(reinterpret_cast<NUDISPLAYLIST_PC *>(list), &render_state);
        VARIPTR *buffer = NuDisplayListLinkItems(reinterpret_cast<NUDISPLAYLIST_PC *>(list), visible_count * 2);
        if (static_cast<NUDISPLAYLISTITEM *>(scene->items)[object->item_indices[material]].type != 0x8f) {
            void *geometry_data = static_cast<NUDISPLAYLISTITEM *>(scene->items)[object->item_indices[material]].next;
            for (u32 i = 0; i < visible_count; ++i) {
                NUMTX *matrix = &matrices[clip != 0 ? static_cast<u8>(visible[i]) : i];
                void *transform = DisplayListCreateGeomTransformPS(
                    buffer, matrix, scene->display_materials[list->material_id], geometry_data, NULL);
                list->items->type = 0x8c;
                list->items->id = 3;
                list->items->next = transform;
                NUDISPLAYLISTITEM *first = list->items++;
                list->items->type = 0x82;
                list->items->id = 3;
                list->items->next = geometry_data;
                NUDISPLAYLISTITEM *last = list->items++;
                DisplayListSetAlphaPS(first, last, 1.0f);
            }
        } else {
            for (u32 i = 0; i < visible_count; ++i) {
                NUMTX *matrix = &matrices[clip != 0 ? static_cast<u8>(visible[i]) : i];
                void *transform = DisplayListCreateFaceonTransformPS(
                    buffer, matrix, scene->display_materials[list->material_id],
                    static_cast<NUDISPLAYLISTITEM *>(scene->items)[object->item_indices[material]].next);
                void *faceon = NuDisplayListPrepareFaceonPS(
                    buffer, static_cast<NUDISPLAYLISTITEM *>(scene->items)[object->item_indices[material]].next,
                    matrix);
                list->items->type = 0x90;
                list->items->id = 3;
                list->items->next = transform;
                NUDISPLAYLISTITEM *first = list->items++;
                list->items->type = 0x8f;
                list->items->id = 3;
                list->items->next = faceon;
                NUDISPLAYLISTITEM *last = list->items++;
                DisplayListSetAlphaPS(first, last, 1.0f);
            }
        }
    }
}
