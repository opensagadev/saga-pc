#include "saga.h"

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
        NuMemSet128(scene->material_used[(scene->render_flags >> 15) & 1], 0, ((scene->material_count + 7) >> 7) + 1);
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
