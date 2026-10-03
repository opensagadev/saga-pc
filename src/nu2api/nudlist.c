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
