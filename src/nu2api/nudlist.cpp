#include "saga.h"

extern "C" {
    void __cdecl PCThreadCriticalSectionEnter_candidate(i32);
    void __cdecl PCThreadCriticalSectionLeave_candidate(i32);
    extern void __cdecl PCSpecialEmptyHandler_candidate(void *ignored);

    void __cdecl NuDisplaySceneDestroyPS(NUDISPLAY_LIST_PC *scene) {
    }

    void __cdecl NuDisplaySceneDestroy(NUDISPLAY_LIST_PC *scene) {
        i32 i, removed;
        PC_MATERIAL_ANIM_SET *animation;
        if (scene == NULL)
            return;
        PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
        NuDisplaySceneDestroyPS(scene);
        for (i = 0; i < (i32)nudlist_field_9d8; ++i) {
            if (PCDisplayScenes[i] == scene)
                break;
        }
        if (i == (i32)nudlist_field_9d8) {
            AnimationDiagnosticNoOpPc();
            for (;;) {
            }
        }
        PCSpecialEmptyHandler_candidate(scene->allocation);
        for (i = 0; i < scene->sort_count; ++i) {
            NUSORTPRI *sort = &scene->sort_pris[i];
            if (sort_list == sort)
                sort_list = sort->sys_next;
            else {
                NUSORTPRI *previous = sort_list;
                while (previous->sys_next != sort)
                    previous = previous->sys_next;
                previous->sys_next = sort->sys_next;
            }
            --nused_sort_pris;
        }
        if (scene->material_animations != NULL) {
            PC_MATERIAL_ANIM_SET *scene_animation = (PC_MATERIAL_ANIM_SET *)scene->material_animations;
            animation = (PC_MATERIAL_ANIM_SET *)nudlist_field_fc8;
            if (animation == scene_animation)
                nudlist_field_fc8 = (u32)scene_animation->next;
            else {
                while (animation->next != scene_animation)
                    animation = animation->next;
                animation->next = scene_animation->next;
            }
        }
        removed = 0;
        for (i = 0; i < (i32)nudlist_field_9d8; ++i) {
            PCDisplayScenes[i - removed] = PCDisplayScenes[i];
            if (PCDisplayScenes[i] == scene)
                ++removed;
        }
        if (removed != 0)
            --nudlist_field_9d8;
        PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    }

    void __cdecl NuDisplayListDestroyMtl(NUMTL *material) {
        u32 index;
        PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
        if (material->display_list != NULL) {
            for (index = 0; index < PCDisplayMaterialCount; ++index) {
                if (PCDisplayMaterialRegistry[index] == material)
                    break;
            }
            if (index < PCDisplayMaterialCount) {
                if (PCDisplayMaterialDeleteCount == 600)
                    AnimationDiagnosticNoOpPc();
                else
                    PCDisplayMaterialDeleteQueue[PCDisplayMaterialDeleteCount++] = material;
            }
        }
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
            NUSORTPRI *sort = &scene->sort_pris[i];
            if (numtl_renderplane != 0)
                sort->sort_pri += numtl_renderplane << 17;
            DisplayInsertSortPC(sort);
        }
        if (scene->material_animations != NULL) {
            PC_MATERIAL_ANIM_SET *animation = scene->material_animations;
            animation->next = (PC_MATERIAL_ANIM_SET *)nudlist_field_fc8;
            nudlist_field_fc8 = (u32)animation;
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
        PCDynamicDisplayScene.render_flags ^= 0x8000;
        ResetSceneBeforeFramePC(&PCDynamicDisplayScene, 0);
        for (i = 0; i < (i32)nudlist_field_9d8; ++i) {
            NUDISPLAY_LIST_PC *scene = PCDisplayScenes[i];
            scene->render_flags ^= 0x8000;
            ResetSceneBeforeFramePC(scene, 0);
            if (scene->gscene != NULL)
                scene->gscene->frame_reset_marker = 0;
        }
        render_scene_index = 0;
        for (sort = sort_list; sort != NULL; sort = sort->sys_next)
            sort->flags &= ~2u;
        RndrStateResetSharedGlobalState();
        PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    }

    typedef struct pc_display_material_pc_s {
        u16 flags;
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
                    i32 key = (i32)(i8)(material->flags >> 4) * 131072 + material->sort_priority;
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
        NUDISPLAYLISTITEM *items = (NUDISPLAYLISTITEM *)scene->items;
        if (scene->alpha_values == NULL)
            return;
        for (object = 0; object < scene->clip_count; ++object) {
            PC_DISPLAY_CLIP_OBJECT_PC *clip = &scene->clip_objects[object];
            for (item_index = 0; item_index < scene->clip_counts[object]; ++item_index) {
                NUDISPLAYLISTITEM *item = &items[clip->item_indices[item_index]];
                if (item->id != 0 && item->type == 0x82)
                    DisplayListSetAlphaPS(item - 1, item, scene->alpha_values[object]);
            }
        }
    }

    static void __cdecl UpdateMaterialClipBitsPC(NUDISPLAY_LIST_PC *scene) {
        u32 buffer;
        u8 *current, *other;
        i32 bytes, byte_index, material_index = 0;
        if (scene == NULL || scene->display_materials == NULL || scene->display_materials[0] == NULL)
            return;
        buffer = (scene->render_flags >> 15) & 1;
        current = scene->material_used[buffer];
        other = scene->material_used[buffer ^ 1];
        bytes = (i32)((scene->material_count + 7) >> 3);
        for (byte_index = 0; byte_index < bytes; ++byte_index) {
            if (current[byte_index] != other[byte_index]) {
                i32 bit;
                for (bit = 0; bit < 8; ++bit) {
                    NUDISPLAYLIST *list = scene->material_lists[material_index];
                    PC_DISPLAY_MATERIAL_PC *material =
                        (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[list->material_id];
                    i32 enabled = (current[byte_index] >> bit) & 1;
                    if (material != NULL && enabled != 0) {
                        if (PCDeviceHardwareVertexProcessingPC == 1) {
                            if (material->blend_operation[0] != 0xff && (i32)material->blend_operation[0] +
                                                                                material->blend_operation[1] +
                                                                                material->blend_operation[2] !=
                                                                            0)
                                enabled = 0;
                        } else if (material->blend_operation[0] == 0xff) {
                            enabled = 0;
                        }
                    }
                    list->mtl_item->id = enabled ? 0 : 1;
                    ++material_index;
                    if (material_index >= (i32)scene->material_count)
                        return;
                }
            } else {
                material_index += 8;
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
            NUDISPLAYLIST *list = material->display_list;
            NUDISPLAYLISTITEM *items;
            NUSORTPRI *sort;
            i32 index, key, sort_index = -1, i, unlinked = 0;
            if (list == NULL || scene->material_count == 0)
                continue;
            for (index = 0; index < (i32)scene->material_count; ++index) {
                if (scene->display_materials[index] == (NUMTL *)material)
                    break;
            }
            if (index == (i32)scene->material_count)
                continue;
            if (--PCDisplayMaterialReferenceCounts[index] != 0)
                continue;
            key = (i32)(i8)(material->flags >> 4) * 131072 + material->sort_priority;
            for (i = 0; i < scene->sort_count; ++i) {
                if (scene->sort_pris[i].sort_pri == key)
                    sort_index = i;
            }
            sort = &scene->sort_pris[sort_index];
            items = list->mtl_item;
            if (sort->items == items) {
                sort->items = (NUDISPLAYLISTITEM *)items[PCMaterialLinkItemIndex].next;
                unlinked = 1;
            } else {
                for (i = 0; i < sort->nmtls; ++i) {
                    PC_DISPLAY_MATERIAL_PC *previous_material =
                        (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[sort->mtl_first + i];
                    NUDISPLAYLISTITEM *previous = previous_material->display_list->mtl_item;
                    if (previous[PCMaterialLinkItemIndex].next == items) {
                        previous[PCMaterialLinkItemIndex].next = items[PCMaterialLinkItemIndex].next;
                        unlinked = 1;
                        break;
                    }
                }
            }
            if (unlinked != 0) {
                items[PCMaterialLinkItemIndex].next = NULL;
                items[PCMaterialLinkItemIndex].type = 0x84;
                items[PCMaterialLinkItemIndex].id = 4;
            }
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialStreamUsed[i] != 0 &&
                    items == (NUDISPLAYLISTITEM *)(PCDisplayMaterialStreamStart.addr + (u32)i * 0x80)) {
                    PCDisplayMaterialStreamUsed[i] = 0;
                    break;
                }
            }
            if (--sort->nmtls == 0) {
                NUSORTPRI *previous = NULL;
                NUSORTPRI *current = sort_list;
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
            NUDISPLAYLIST *list, *unused;
            NUDISPLAYLISTITEM *items;
            NUSORTPRI *sort;
            VARIPTR cursor;
            i32 index, insert, stream_index = -1, key, sort_index = -1, i;
            for (index = 0; index < (i32)scene->material_count; ++index) {
                if (scene->display_materials[index] == (NUMTL *)material)
                    break;
            }
            if (index < (i32)scene->material_count) {
                ++PCDisplayMaterialReferenceCounts[index];
                continue;
            }
            for (index = 0; index < (i32)scene->material_count; ++index) {
                if (PCDisplayMaterialReferenceCounts[index] == 0)
                    break;
            }
            if (index == (i32)scene->material_count) {
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
            key = (i32)(i8)(material->flags >> 4) * 131072 + material->sort_priority;
            for (i = 0; i < scene->sort_count; ++i) {
                if (scene->sort_pris[i].sort_pri == key)
                    sort_index = i;
            }
            insert = index;
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialReferenceCounts[i] != 0) {
                    PC_DISPLAY_MATERIAL_PC *other = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[i];
                    i32 other_key = (i32)(i8)(other->flags >> 4) * 131072 + other->sort_priority;
                    if (key <= other_key) {
                        insert = i;
                        break;
                    }
                }
            }
            unused = scene->material_lists[scene->material_count - 1];
            for (i = (i32)scene->material_count - 2; i >= insert; --i) {
                scene->display_materials[i + 1] = scene->display_materials[i];
                scene->material_lists[i + 1] = scene->material_lists[i];
                PCDisplayMaterialReferenceCounts[i + 1] = PCDisplayMaterialReferenceCounts[i];
            }
            scene->material_lists[insert] = unused;
            PCDisplayMaterialReferenceCounts[insert] = 0;
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialReferenceCounts[i] != 0) {
                    PC_DISPLAY_MATERIAL_PC *used = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[i];
                    used->display_list->material_id = i;
                }
            }
            PCDisplayMaterialReferenceCounts[insert] = 1;
            list = scene->material_lists[insert];
            material->display_list = list;
            cursor.addr = PCDisplayMaterialStreamStart.addr + (u32)stream_index * 0x80;
            items = NuDisplayListCreateMtlDlist(list->mtl_item, (NUMTL *)material, &cursor, PCDisplayMaterialStreamEnd);
            scene->display_materials[insert] = (NUMTL *)material;
            list->mtl_item = items;
            list->dynamic_geometry = items + PCMaterialGeometryItemIndex;
            list->scene = scene;
            list->material_id = insert;
            if (sort_index != -1) {
                sort = &scene->sort_pris[sort_index];
                if (insert != 0) {
                    PC_DISPLAY_MATERIAL_PC *previous = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[insert - 1];
                    i32 previous_key = (i32)(i8)(previous->flags >> 4) * 131072 + previous->sort_priority;
                    if (previous_key == key) {
                        NUDISPLAYLISTITEM *anchor = previous->display_list->mtl_item;
                        items[PCMaterialLinkItemIndex].next = anchor[PCMaterialLinkItemIndex].next;
                        anchor[PCMaterialLinkItemIndex].next = items;
                    } else {
                        items[PCMaterialLinkItemIndex].next = sort->items;
                        sort->items = items;
                    }
                } else {
                    items[PCMaterialLinkItemIndex].next = sort->items;
                    sort->items = items;
                }
                items[PCMaterialLinkItemIndex].type = 0x8d;
                items[PCMaterialLinkItemIndex].id = 1;
            } else {
                NUSORTPRI *previous = NULL;
                NUSORTPRI *current = sort_list;
                if (scene->sort_count >= 600)
                    AnimationDiagnosticNoOpPc();
                sort = &scene->sort_pris[scene->sort_count];
                sort->sort_pri = key;
                sort->items = items;
                sort->sort_flags = nudlist_field_e80;
                sort->nmtls = 0;
                while (current != NULL && current->sort_pri < key) {
                    previous = current;
                    current = current->sys_next;
                }
                sort->sys_next = current;
                if (previous == NULL)
                    sort_list = sort;
                else
                    previous->sys_next = sort;
                ++nused_sort_pris;
                ++scene->sort_count;
            }
            ++sort->nmtls;
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
        items[0].id = 1;
        items[0].next = items + PCMaterialBlockItemCount - 1;
        NuDisplayListAddClut(items + 1, view->texture_id);
        NuDisplayListAddTexture(items + 2, view->texture_id);
        NuDisplayListAddMaterialState(items + 3, material);
        NuDisplayListAddMicrocode(items + 4, material);
        NuDisplayListAddLightState(items + 5, material);
        items[6].type = 0x8b;
        items[6].id = 1;
        items[6].next = items + PCMaterialBlockItemCount - 1;
        items[7].type = 0x84;
        items[7].id = 4;
        items[7].next = NULL;
        return items;
    }

    void __cdecl RndrStateUpdate(NURNDRLIBSTATE *state, NUMTL *material, NUDISPLAYLISTITEM *item) {
        (void)state;
        (void)material;
        (void)item;
    }

    void __cdecl RndrStateUpdateFx(NURNDRLIBSTATE *state, NUDISPLAYLISTITEM *item) {
        (void)state;
        (void)item;
    }

    void __cdecl DisplayProcessMaterialRenderStatesPC(void) {
        NURNDRLIBSTATE state;
        NUGLOBALRNDRSTATE_PC *last_local = NULL;
        NUSORTPRI *sort;
        i32 i;
        NuDisplayListResetStateCache(&state);
        for (sort = sort_list; sort != NULL; sort = sort->sys_next) {
            NUDISPLAY_LIST_PC *scene = sort->display_scene;
            if (scene == NULL) {
                RndrStateUpdateFx(&state, sort->items);
            } else if ((scene->render_flags & 6) != 0) {
                for (i = 0; i < sort->nmtls; ++i) {
                    i32 index = sort->mtl_first + i;
                    NUDISPLAYLIST *list = scene->material_lists[index];
                    PC_DISPLAY_MATERIAL_PC *material;
                    if (list->mtl_item->id != 0)
                        continue;
                    material = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[index];
                    if (scene->local_state != NULL && (scene->render_flags & 4) != 0) {
                        if (list->mtl_last != list->first || last_local != scene->local_state) {
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
                        } else {
                            list->mtl_last->next = list->dynamic_geometry + 1;
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
                    RndrStateUpdate(&state, (NUMTL *)material, list->mtl_item);
                    if (((u8)(scene->render_flags >> 5) & (u8)(material->visibility_flags >> 14)) != 0) {
                        list->mtl_item->id = 1;
                        scene->material_used[(scene->render_flags >> 15) & 1][list->material_id / 8] &=
                            (u8) ~(1u << (list->material_id % 8));
                    }
                }
            }
        }
        for (i = 0; i < (i32)nudlist_field_9d8; ++i) {
            if (PCDisplayScenes[i]->local_state != NULL)
                RndrStateResetGlobalState(PCDisplayScenes[i]->local_state);
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

    void __cdecl DisplayClipMaterialsPC(NUDISPLAY_LIST_PC *scene) {
        i32 clip_index = 0;
        i32 group_index = 0;
        while (clip_index < scene->clip_count) {
            f32 *range = &scene->fade_levels[clip_index];
            if (*range != 0.0f) {
                if ((scene->visibility_flags[group_index] & 0x20) != 0) {
                    NUVEC4 *center = &scene->clip_bounds[group_index * 2];
                    f32 dx = global_camera.mtx.m30 - center->x;
                    f32 dy = global_camera.mtx.m31 - center->y;
                    f32 dz = global_camera.mtx.m32 - center->z;
                    f32 distance = (dx * dx + dy * dy) + dz * dz;
                    while (*range > distance) {
                        ++range;
                        ++clip_index;
                    }
                    NUVEC minimum, maximum;
                    NUVEC4 *extent = center + 1;
                    minimum.x = center->x - extent->x;
                    minimum.y = center->y - extent->y;
                    minimum.z = center->z - extent->z;
                    maximum.x = center->x + extent->x;
                    maximum.y = center->y + extent->y;
                    maximum.z = center->z + extent->z;
                    if (((PCFrustumClipSet *)PCDisplayClipSet)->TestBox(&minimum, &maximum, PCDisplayClipGroupIndex)) {
                        PC_DISPLAY_CLIP_OBJECT_PC *object = &scene->clip_objects[clip_index];
                        i32 material;
                        u32 buffer = (scene->render_flags >> 15) & 1;
                        for (material = 0; material < object->material_count; ++material) {
                            u32 index = (u32)object->material_ids[material];
                            scene->material_used[buffer][index >> 3] |= (u8)(1u << (index & 7));
                        }
                        scene->clip_used[buffer][clip_index / 4] |= (u8)(1u << ((clip_index & 3) * 2));
                    }
                }
                range = &scene->fade_levels[clip_index];
                while (*range != 0.0f)
                    ++range, ++clip_index;
            } else if ((scene->visibility_flags[group_index] & 0x20) != 0) {
                NUVEC4 *center = &scene->clip_bounds[group_index * 2];
                NUVEC4 *extent = center + 1;
                NUVEC minimum, maximum;
                minimum.x = center->x - extent->x;
                minimum.y = center->y - extent->y;
                minimum.z = center->z - extent->z;
                maximum.x = center->x + extent->x;
                maximum.y = center->y + extent->y;
                maximum.z = center->z + extent->z;
                if (((PCFrustumClipSet *)PCDisplayClipSet)->TestBox(&minimum, &maximum, PCDisplayClipGroupIndex)) {
                    PC_DISPLAY_CLIP_OBJECT_PC *object = &scene->clip_objects[clip_index];
                    i32 material;
                    u32 buffer = (scene->render_flags >> 15) & 1;
                    for (material = 0; material < object->material_count; ++material) {
                        u32 index = (u32)object->material_ids[material];
                        scene->material_used[buffer][index >> 3] |= (u8)(1u << (index & 7));
                    }
                    scene->clip_used[buffer][clip_index / 4] |= (u8)(1u << ((clip_index & 3) * 2));
                }
            }
            ++clip_index;
            ++group_index;
        }
    }
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

extern "C" NUDISPLAY_LIST_PC *__cdecl NuDisplaySceneClone(NUDISPLAY_LIST_PC *source, VARIPTR *buffer) {
    NUDISPLAY_LIST_PC *scene;
    PC_CLONE_LIST_VIEW *lists, *first_list;
    NURNDRLIBSTATE *states;
    NUDISPLAYLISTITEM *heads;
    PC_CLONE_MATERIAL_VIEW *materials;
    i32 i, j, clip_bytes;
    u32 material_bytes;
    PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
    buffer->addr = (buffer->addr + 3) & ~3u;
    scene = static_cast<NUDISPLAY_LIST_PC *>(buffer->void_ptr);
    buffer->addr += sizeof(*scene);
    *scene = *source;
    buffer->addr = (buffer->addr + 3) & ~3u;
    scene->items = buffer->void_ptr;
    buffer->addr += source->item_count * sizeof(NUDISPLAYLISTITEM);
    memcpy(scene->items, source->items, source->item_count * sizeof(NUDISPLAYLISTITEM));
    clip_bytes = (((source->clip_count + 7) / 8) * 2 / 16 + 1) * 16;
    for (i = 0; i < 2; ++i) {
        buffer->addr = (buffer->addr + 15) & ~15u;
        scene->clip_used[i] = static_cast<u8 *>(buffer->void_ptr);
        buffer->addr += clip_bytes;
    }
    memset(scene->clip_used[0], 0, clip_bytes);
    memset(scene->clip_used[1], 0, clip_bytes);
    material_bytes = (((source->material_count + 7) >> 7) + 1) * 16;
    for (i = 0; i < 2; ++i) {
        buffer->addr = (buffer->addr + 15) & ~15u;
        scene->material_used[i] = static_cast<u8 *>(buffer->void_ptr);
        buffer->addr += material_bytes;
    }
    memset(scene->material_used[0], 0, material_bytes);
    memset(scene->material_used[1], 0, material_bytes);
    buffer->addr = (buffer->addr + 3) & ~3u;
    lists = static_cast<PC_CLONE_LIST_VIEW *>(buffer->void_ptr);
    first_list = lists;
    buffer->addr += source->material_count * sizeof(*lists);
    buffer->addr = (buffer->addr + 3) & ~3u;
    scene->material_lists = static_cast<NUDISPLAYLIST **>(buffer->void_ptr);
    buffer->addr += source->material_count * sizeof(*scene->material_lists);
    buffer->addr = (buffer->addr + 3) & ~3u;
    states = static_cast<NURNDRLIBSTATE *>(buffer->void_ptr);
    buffer->addr += source->material_count * sizeof(*states);
    memset(states, 0, source->material_count * sizeof(*states));
    buffer->addr = (buffer->addr + 3) & ~3u;
    heads = static_cast<NUDISPLAYLISTITEM *>(buffer->void_ptr);
    buffer->addr += source->material_count * sizeof(*heads);
    memset(heads, 0, source->material_count * sizeof(*heads));
    for (i = 0; i < static_cast<i32>(source->material_count); ++i, ++lists, ++states, ++heads) {
        PC_CLONE_LIST_VIEW *original = reinterpret_cast<PC_CLONE_LIST_VIEW *>(source->material_lists[i]);
        scene->material_lists[i] = &lists->list;
        *lists = *original;
        lists->list.scene = scene;
        lists->list.state = states;
        lists->list.mtl_item = static_cast<NUDISPLAYLISTITEM *>(scene->items) +
                               (original->list.mtl_item - static_cast<NUDISPLAYLISTITEM *>(source->items));
        lists->list.dynamic_geometry =
            static_cast<NUDISPLAYLISTITEM *>(scene->items) +
            (original->list.dynamic_geometry - static_cast<NUDISPLAYLISTITEM *>(source->items));
        lists->list.first = heads;
        lists->list.mtl_last = heads;
        for (j = 0; j < 2; ++j) {
            buffer->addr = (buffer->addr + 15) & ~15u;
            lists->list.scene_first[j] = static_cast<NUDISPLAYLISTITEM *>(buffer->void_ptr);
            buffer->addr += original->scene_item_count * sizeof(NUDISPLAYLISTITEM);
        }
        heads->type = 0x8d;
        heads->next = NULL;
        heads->id = 1;
    }
    buffer->addr = (buffer->addr + 3) & ~3u;
    materials = static_cast<PC_CLONE_MATERIAL_VIEW *>(buffer->void_ptr);
    buffer->addr += source->material_count * sizeof(*materials);
    buffer->addr = (buffer->addr + 3) & ~3u;
    scene->display_materials = static_cast<NUMTL **>(buffer->void_ptr);
    buffer->addr += source->material_count * sizeof(*scene->display_materials);
    for (i = 0; i < static_cast<i32>(source->material_count); ++i, ++materials) {
        PC_CLONE_MATERIAL_VIEW *original = reinterpret_cast<PC_CLONE_MATERIAL_VIEW *>(source->display_materials[i]);
        scene->display_materials[i] = reinterpret_cast<NUMTL *>(materials);
        *materials = *original;
        materials->display_list = &first_list[(reinterpret_cast<PC_CLONE_LIST_VIEW *>(original->display_list) -
                                               reinterpret_cast<PC_CLONE_LIST_VIEW *>(source->material_lists[0]))]
                                       .list;
        NuMtlUpdatePS(reinterpret_cast<NUMTL *>(materials));
        ++materials->version;
    }
    buffer->addr = (buffer->addr + 3) & ~3u;
    scene->sort_pris = static_cast<NUSORTPRI *>(buffer->void_ptr);
    buffer->addr += source->sort_count * sizeof(*scene->sort_pris);
    memcpy(scene->sort_pris, source->sort_pris, source->sort_count * sizeof(*scene->sort_pris));
    for (i = 0; i < source->sort_count; ++i) {
        NUSORTPRI *sort = &scene->sort_pris[i];
        sort->items = static_cast<NUDISPLAYLISTITEM *>(scene->items) +
                      (source->sort_pris[i].items - static_cast<NUDISPLAYLISTITEM *>(source->items));
        sort->display_scene = scene;
        reinterpret_cast<PC_CLONE_SORT_SELF_VIEW *>(sort)->self = sort;
    }
    for (i = 0; i < source->item_count; ++i) {
        NUDISPLAYLISTITEM *original = &static_cast<NUDISPLAYLISTITEM *>(source->items)[i];
        NUDISPLAYLISTITEM *item = &static_cast<NUDISPLAYLISTITEM *>(scene->items)[i];
        if (item->type == 0x80) {
            for (j = 0; j < static_cast<i32>(source->material_count); ++j) {
                if (original->next == source->display_materials[j]) {
                    item->next = scene->display_materials[j];
                    break;
                }
            }
        } else if (item->type == 0x85) {
            item->next =
                static_cast<NUDISPLAYLISTITEM *>(scene->items) +
                (static_cast<NUDISPLAYLISTITEM *>(original->next) - static_cast<NUDISPLAYLISTITEM *>(source->items));
        }
    }
    if (source->material_animations != NULL) {
        PC_CLONE_ANIMATION_VIEW *animation;
        buffer->addr = (buffer->addr + 3) & ~3u;
        animation = static_cast<PC_CLONE_ANIMATION_VIEW *>(buffer->void_ptr);
        buffer->addr += sizeof(*animation);
        *animation = *reinterpret_cast<PC_CLONE_ANIMATION_VIEW *>(source->material_animations);
        animation->scene = scene;
        animation->next = reinterpret_cast<PC_MATERIAL_ANIM_SET *>(nudlist_field_fc8);
        scene->material_animations = reinterpret_cast<PC_MATERIAL_ANIM_SET *>(animation);
        nudlist_field_fc8 = reinterpret_cast<u32>(animation);
    } else {
        scene->material_animations = NULL;
    }
    buffer->addr = (buffer->addr + 3) & ~3u;
    scene->local_state = static_cast<NUGLOBALRNDRSTATE_PC *>(buffer->void_ptr);
    buffer->addr += sizeof(*scene->local_state);
    memset(scene->local_state, 0, sizeof(*scene->local_state));
    NuDisplaySceneClonePS(source, scene, buffer);
    PCDisplayScenes[nudlist_field_9d8++] = scene;
    ResetSceneBeforeFramePC(scene, 1);
    for (i = 0; i < scene->sort_count; ++i) {
        NUSORTPRI *sort = &scene->sort_pris[i];
        sort->sort_pri &= 0x1ffff;
        if (numtl_renderplane != 0)
            sort->sort_pri += numtl_renderplane * 0x20000;
        DisplayInsertSortPC(sort);
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

    static __declspec(noinline) void DisplayListProcessGeomPC(NUDISPLAYLIST_PC *list, NUDISPLAYLISTITEM *geometry,
                                                              NUDISPLAYLISTITEM **output, NUMTX *matrix, void **packet,
                                                              f32 alpha, i32 shadow) {
        VARIPTR *buffer = NuDisplayListLinkItems(list, 2);
        NUDISPLAY_LIST_PC *scene = reinterpret_cast<NUDISPLAY_LIST_PC *>(list->scene);
        *packet = DisplayListCreateGeomTransformPS(buffer, matrix, scene->display_materials[list->material_id],
                                                   geometry->next, *packet);
        NUDISPLAYLISTITEM *item = static_cast<NUDISPLAYLISTITEM *>(list->items);
        item->type = 0x8c;
        item->id = 3;
        item->next = *packet;
        output[0] = item++;
        item->type = 0x82;
        item->id = 3;
        item->next = geometry->next;
        output[1] = item++;
        list->items = item;
        DisplayListSetAlphaPS(output[0], output[1], alpha);
        DisplayListSetShadowCasterFlagPS(output[0], output[1], shadow);
    }
    static __declspec(noinline) void DisplayListProcessLightmappedPC(NUDISPLAYLIST_PC *list,
                                                                     NUDISPLAYLISTITEM *geometry,
                                                                     NUDISPLAYLISTITEM **output, NUMTX *matrix,
                                                                     void **packet, f32 alpha) {
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        VARIPTR *buffer = NuDisplayListLinkItems(list, 3);
        NUDISPLAYLISTITEM *item = static_cast<NUDISPLAYLISTITEM *>(list->items);
        item->type = geometry[-2].type;
        item->id = 3;
        item->next = geometry[-2].next;
        output[0] = item++;
        list->items = item;
        NUDISPLAY_LIST_PC *scene = reinterpret_cast<NUDISPLAY_LIST_PC *>(list->scene);
        *packet = DisplayListCreateGeomTransformPS(buffer, matrix, scene->display_materials[list->material_id],
                                                   geometry->next, *packet);
        item = static_cast<NUDISPLAYLISTITEM *>(list->items);
        item->type = 0x8c;
        item->id = 3;
        item->next = *packet;
        output[1] = item++;
        item->type = 0x82;
        item->id = 3;
        item->next = geometry->next;
        output[2] = item++;
        list->items = item;
        DisplayListSetAlphaPS(output[1], output[2], alpha);
    }
    static __declspec(noinline) void DisplayListProcessSkinPC(NUDISPLAYLIST_PC *list, NUDISPLAYLISTITEM *geometry,
                                                              NUDISPLAYLISTITEM **output, NUMTX *matrix, void **packet,
                                                              NUMTX *skin, void *weights, i32 shadow) {
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        VARIPTR *buffer = NuDisplayListLinkItems(list, 3);
        NUDISPLAY_LIST_PC *scene = reinterpret_cast<NUDISPLAY_LIST_PC *>(list->scene);
        *packet = DisplayListCreateGeomTransformPS(buffer, matrix, scene->display_materials[list->material_id],
                                                   geometry->next, *packet);
        NUDISPLAYLISTITEM *item = static_cast<NUDISPLAYLISTITEM *>(list->items);
        item->type = 0x8c;
        item->id = 3;
        item->next = *packet;
        output[0] = item++;
        list->items = item;
        void *render_geometry;
        void *skin_packet =
            DisplayListCreateSkinTransformPS(buffer, skin, weights, geometry->next, &render_geometry, output + 2);
        item = static_cast<NUDISPLAYLISTITEM *>(list->items);
        item->type = 0x99;
        item->id = 3;
        item->next = skin_packet;
        output[1] = item++;
        item->type = 0x98;
        item->id = 3;
        item->next = render_geometry;
        output[1] = item++;
        list->items = item;
        DisplayListSetAlphaPS(output[0], output[1], 1.0f);
        DisplayListSetShadowCasterFlagPS(output[0], output[1], shadow);
    }
    static __declspec(noinline) void DisplayListProcessFaceonPC(NUDISPLAYLIST_PC *list, NUDISPLAYLISTITEM *geometry,
                                                                NUDISPLAYLISTITEM **output, NUMTX *matrix, f32 alpha) {
        VARIPTR *buffer = NuDisplayListLinkItems(list, 2);
        NUDISPLAY_LIST_PC *scene = reinterpret_cast<NUDISPLAY_LIST_PC *>(list->scene);
        void *packet = DisplayListCreateFaceonTransformPS(buffer, matrix, scene->display_materials[list->material_id],
                                                          geometry->next);
        NUDISPLAYLISTITEM *item = static_cast<NUDISPLAYLISTITEM *>(list->items);
        item->type = 0x90;
        item->id = 3;
        item->next = packet;
        output[0] = item++;
        list->items = item;
        packet = NuDisplayListPrepareFaceonPS(buffer, geometry->next, matrix);
        item = static_cast<NUDISPLAYLISTITEM *>(list->items);
        item->type = 0x8f;
        item->id = 3;
        item->next = packet;
        output[1] = item++;
        list->items = item;
        DisplayListSetAlphaPS(output[0], output[1], alpha);
    }
    static __declspec(noinline) void
    DisplayListProcessSpecialItemPC(PC_SPECIAL_RENDER_MATERIAL *material, NUDISPLAYLIST_PC *list,
                                    NUDISPLAYLISTITEM *geometry, i32 index, NUDISPLAYLISTITEM **output, NUMTX *matrix,
                                    void **packet, NUMTX *skin, void *weights, f32 alpha, i32 shadow) {
        if (geometry->type == 0x8f) {
            DisplayListProcessFaceonPC(list, geometry, output, matrix, alpha);
        } else if (skin != NULL) {
            DisplayListProcessSkinPC(list, geometry, output, matrix, packet, skin, weights, shadow);
        } else if ((material->shader_flags & 0x80000000u) != 0 && index > 1 &&
                   (geometry[-2].type == 0xb0 || geometry[-2].type == 0xae || geometry[-2].type == 0xaf)) {
            DisplayListProcessLightmappedPC(list, geometry, output, matrix, packet, alpha);
        } else {
            DisplayListProcessGeomPC(list, geometry, output, matrix, packet, alpha, shadow);
        }
    }

    static __declspec(noinline) f32 DisplaySceneDistanceFadePC(f32 distance, f32 start, f32 end) {
        if (start < end) {
            if (distance > start * start) {
                f32 alpha = (end - NuFsqrt(distance)) / (end - start);
                return alpha < 0.0f ? 0.0f : alpha;
            }
            return 1.0f;
        }
        if (distance > end * end) {
            f32 alpha = (NuFsqrt(distance) - end) / (start - end);
            return alpha > 1.0f ? 1.0f : alpha;
        }
        return 0.0f;
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
    DISPLAY_LIST_ALLOCATION_PC *lists;
    NUDISPLAYLISTITEM *first;
    i32 i, used_size;
    (void)unused5;
    (void)unused6;
    (void)unused8;
    buffer->addr = (buffer->addr + 15) & ~15u;
    scene->item_count = item_count;
    scene->items = buffer->void_ptr;
    buffer->addr += item_count * sizeof(NUDISPLAYLISTITEM);
    scene->material_count = material_count;
    buffer->addr = (buffer->addr + 15) & ~15u;
    scene->display_materials = (NUMTL **)buffer->void_ptr;
    buffer->addr += material_count * sizeof(NUMTL *);
    buffer->addr = (buffer->addr + 15) & ~15u;
    scene->material_lists = (NUDISPLAYLIST **)buffer->void_ptr;
    buffer->addr += material_count * sizeof(NUDISPLAYLIST *);
    memset(scene->display_materials, 0, material_count * sizeof(NUMTL *));
    memset(scene->material_lists, 0, material_count * sizeof(NUDISPLAYLIST *));
    if (allocate_materials != 0) {
        DISPLAY_MATERIAL_ALLOCATION_PC *materials;
        buffer->addr = (buffer->addr + 15) & ~15u;
        materials = (DISPLAY_MATERIAL_ALLOCATION_PC *)buffer->void_ptr;
        buffer->addr += material_count * sizeof(*materials);
        for (i = 0; i < material_count; ++i)
            scene->display_materials[i] = &materials[i].base;
    }
    buffer->addr = (buffer->addr + 15) & ~15u;
    lists = (DISPLAY_LIST_ALLOCATION_PC *)buffer->void_ptr;
    buffer->addr += material_count * sizeof(*lists);
    memset(lists, 0, material_count * sizeof(*lists));
    buffer->addr = (buffer->addr + 15) & ~15u;
    first = (NUDISPLAYLISTITEM *)buffer->void_ptr;
    buffer->addr += material_count * sizeof(*first);
    memset(first, 0, material_count * sizeof(*first));
    for (i = 0; i < material_count; ++i) {
        scene->material_lists[i] = &lists[i].base;
        buffer->addr = (buffer->addr + 3) & ~3u;
        lists[i].base.state = (NURNDRLIBSTATE *)buffer->void_ptr;
        buffer->addr += sizeof(NURNDRLIBSTATE);
        lists[i].base.first = &first[i];
        NuDisplayListResetStateCache(lists[i].base.state);
    }
    scene->local_state = NULL;
    for (i = 0; (u32)i < scene->material_count; ++i) {
        scene->material_lists[i]->first->type = 0x8d;
        scene->material_lists[i]->first->id = 1;
        scene->material_lists[i]->first->next = NULL;
    }
    used_size = (((material_count + 7) / 8) / 16 + 1) * 16;
    buffer->addr = (buffer->addr + 15) & ~15u;
    scene->material_used[0] = (u8 *)buffer->void_ptr;
    buffer->addr += used_size;
    buffer->addr = (buffer->addr + 15) & ~15u;
    scene->material_used[1] = (u8 *)buffer->void_ptr;
    buffer->addr += used_size;
    scene->sort_count = sort_count;
    scene->sort_pris = (NUSORTPRI *)buffer->void_ptr;
    buffer->addr += sort_count * sizeof(NUSORTPRI);
    memset(scene->sort_pris, 0, sort_count * sizeof(NUSORTPRI));
    for (i = 0; i < sort_count; ++i) {
        scene->sort_pris[i].display_scene = scene;
        scene->sort_pris[i].nmtls = 0;
        scene->sort_pris[i].mtl_first = 0;
    }
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
    displaylist_2d_first->type = 0x8d;
    displaylist_2d_first->id = 1;
    displaylist_2d_first->next = NULL;
    displaylist_2d_mtl_last = displaylist_2d_first;
    buffer->addr = (buffer->addr + 3) & ~3u;
    displaylist_2d_state = (NURNDRLIBSTATE *)buffer->void_ptr;
    buffer->addr += sizeof(NURNDRLIBSTATE);
    NuDisplayListResetStateCache((NURNDRLIBSTATE *)displaylist_2d_state);
    PCDynamicDisplayScene.render_flags |= 0x10;
    displaylist_2d_mtl_last = displaylist_2d_first;
}

extern "C" void __cdecl NuDisplayListExecute(NUDISPLAYLISTITEM *item, void(__cdecl **handlers)(void *)) {
    for (;;) {
        switch (item->id) {
            case 0:
                ++item;
                break;
            case 1:
                item = (NUDISPLAYLISTITEM *)item->next;
                break;
            case 3:
                if (handlers[item->type - 0x80] != NULL)
                    handlers[item->type - 0x80](item->next);
                ++item;
                break;
            case 4:
                return;
            default:
                for (;;) {
                }
        }
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
    i32 clip_index = 0;
    i32 instance_index = 0;
    scene->alpha_values = NULL;
    if (view->fade_ranges != NULL) {
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 3) & ~3u;
        scene->alpha_values = reinterpret_cast<f32 *>(NuDisplayListStreamCursor->addr);
        NuDisplayListStreamCursor->addr += scene->clip_count * sizeof(f32);
    }
    while (clip_index < scene->clip_count) {
        i32 visible = 1;
        if (visibility != NULL && visibility->instance_tree_bits != NULL)
            visible = (visibility->instance_tree_bits[instance_index >> 3] >> (instance_index & 7)) & 1;
        i32 flags = static_cast<i8>(scene->visibility_flags[instance_index]);
        if (visibility->portal_marker != NULL && PCPortalsEnabled != 0 && (flags & 8) == 0)
            visible = (visibility->portal_bits[instance_index >> 3] >> (instance_index & 7)) & 1;
        if (((flags & 8) != 0 && visibility->portal_marker != NULL && PCPortalsEnabled != 0 || visible == 1) &&
            (flags & 1) != 0) {
            NUVEC4 *center = &scene->clip_bounds[instance_index * 2];
            NUVEC4 *extent = center + 1;
            f32 x = global_camera.mtx.m30 - center->x;
            f32 y = global_camera.mtx.m31 - center->y;
            f32 z = global_camera.mtx.m32 - center->z;
            f32 distance = (x * x + y * y) + z * z;
            f32 *lod = &scene->fade_levels[clip_index];
            if (*lod != 0.0f) {
                while (distance < *lod) {
                    ++lod;
                    ++clip_index;
                }
            }
            f32 far_clip = view->far_clip_ranges[clip_index];
            i32 fades = view->fade_ranges != NULL && (flags & 0x40) != 0;
            if (fades != 0 && view->fade_ranges[instance_index * 2] < view->fade_ranges[instance_index * 2 + 1])
                far_clip = view->fade_ranges[instance_index * 2 + 1];
            i32 result = NuCameraClipTestExtentsAxisAligned(reinterpret_cast<NUVEC *>(center),
                                                            reinterpret_cast<NUVEC *>(extent), far_clip);
            if (result != 0 && NuOcclusionManagerIsEnabled() != 0) {
                NUVEC minimum = {center->x - extent->x, center->y - extent->y, center->z - extent->z};
                NUVEC maximum = {center->x + extent->x, center->y + extent->y, center->z + extent->z};
                if (reinterpret_cast<OcclusionManager *>(&g_OcclusionManager)->IsOccludedOBB(&minimum, &maximum, NULL))
                    result = 0;
            }
            if (result != 0) {
                if (scene->alpha_values != NULL) {
                    scene->alpha_values[clip_index] = 1.0f;
                    if (fades != 0)
                        scene->alpha_values[clip_index] = DisplaySceneDistanceFadePC(
                            distance, view->fade_ranges[instance_index * 2], view->fade_ranges[instance_index * 2 + 1]);
                }
                PC_DISPLAY_CLIP_OBJECT_PC *clip = &scene->clip_objects[clip_index];
                u32 buffer = (scene->render_flags >> 15) & 1;
                for (i32 material = 0; material < clip->material_count; ++material) {
                    u32 index = clip->material_ids[material];
                    scene->material_used[buffer][index >> 3] |= static_cast<u8>(1u << (index & 7));
                }
                scene->clip_used[buffer][clip_index >> 2] |= static_cast<u8>(result << ((clip_index & 3) * 2));
            }
        }
        while (scene->fade_levels[clip_index] != 0.0f)
            ++clip_index;
        ++clip_index;
        ++instance_index;
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
static f32 FrameMaterialAnimationTimePc;
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
        STUBBED_NAMED(0x005d7230); /* native5ded34: NuHtmlWrite("<body bgcolor=#C0C0C0>"); ignored VOID debug output */
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
