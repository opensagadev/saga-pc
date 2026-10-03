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
