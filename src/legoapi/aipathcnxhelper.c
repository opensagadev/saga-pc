#include "saga.h"
#include <float.h>

void __cdecl AIPathCalcExtents(AIPATH_PC *path) {
    NUVEC delta;
    NUVEC corner;
    i32 index;
    path->bounds_min.x = FLT_MAX;
    path->bounds_min.y = FLT_MAX;
    path->bounds_min.z = FLT_MAX;
    path->bounds_max.x = -FLT_MAX;
    path->bounds_max.y = -FLT_MAX;
    path->bounds_max.z = -FLT_MAX;
    for (index = 0; index < path->node_count; ++index) {
        AIPATHNODE_PC *node = &path->nodes[index];
        corner.x = node->position.x - node->radius;
        corner.y = node->min_height;
        corner.z = node->position.z - node->radius;
        if (path->bounds_min.x > corner.x)
            path->bounds_min.x = corner.x;
        if (path->bounds_min.y > corner.y)
            path->bounds_min.y = corner.y;
        if (path->bounds_min.z > corner.z)
            path->bounds_min.z = corner.z;
        corner.x = node->radius + node->position.x;
        corner.y = node->max_height;
        corner.z = node->radius + node->position.z;
        if (path->bounds_max.x < corner.x)
            path->bounds_max.x = corner.x;
        if (path->bounds_max.y < corner.y)
            path->bounds_max.y = corner.y;
        if (path->bounds_max.z < corner.z)
            path->bounds_max.z = corner.z;
    }
    NuVecSub(&delta, &path->bounds_max, &path->bounds_min);
    NuVecScale(&delta, &delta, 0.5f);
    NuVecSub(&path->bounds_min, &path->bounds_min, &delta);
    NuVecAdd(&path->bounds_max, &path->bounds_max, &delta);
}

typedef struct ai_control_slot_pc_s {
    NULISTLNK links;
    u8 unknown_08[0xa0];
} AI_CONTROL_SLOT_PC;
typedef struct ai_control_system_pc_s {
    i32 capacity;
    AI_CONTROL_SLOT_PC *controllers;
    NULISTHDR available;
    NULISTHDR active;
} AI_CONTROL_SYSTEM_PC;
typedef struct ai_helper_slot_pc_s {
    u8 unknown_00[0x10];
} AI_HELPER_SLOT_PC;
struct ai_helper_system_pc_s {
    i16 capacity;
    i16 count;
    AI_HELPER_SLOT_PC *helpers;
};
typedef struct climb_slot_pc_s {
    u8 unknown_00[0x20];
} CLIMB_SLOT_PC;
typedef struct climb_system_pc_s {
    CLIMB_SLOT_PC *objects;
    u16 count;
    u16 capacity;
} CLIMB_SYSTEM_PC;
STATIC_ASSERT(sizeof(AI_CONTROL_SLOT_PC) == 0xa8);
STATIC_ASSERT(offsetof(AI_CONTROL_SLOT_PC, links) == 0);
STATIC_ASSERT(sizeof(AI_CONTROL_SYSTEM_PC) == 0x18);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, capacity) == 0);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, controllers) == 4);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, available) == 8);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, active) == 0x10);
STATIC_ASSERT(sizeof(AI_HELPER_SLOT_PC) == 0x10);
STATIC_ASSERT(sizeof(AI_HELPER_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(AI_HELPER_SYSTEM_PC, capacity) == 0);
STATIC_ASSERT(offsetof(AI_HELPER_SYSTEM_PC, helpers) == 4);
STATIC_ASSERT(sizeof(CLIMB_SLOT_PC) == 0x20);
STATIC_ASSERT(sizeof(CLIMB_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, objects) == 0);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, capacity) == 6);
void __cdecl AIPathCnxControlSysReset(AI_CONTROL_SYSTEM_PC *system);

void __cdecl AIPathCnxControlSysReset(AI_CONTROL_SYSTEM_PC *system) {
    i32 index;
    if (system == NULL)
        return;
    memset(&system->available, 0, sizeof(system->available));
    memset(&system->active, 0, sizeof(system->active));
    memset(system->controllers, 0, (u32)system->capacity * sizeof(AI_CONTROL_SLOT_PC));
    for (index = 0; index < system->capacity; ++index)
        NuLinkedListAppend(&system->available, &system->controllers[index].links);
}

/* Native 0x005ab0f0, 66 bytes. */
extern void(__cdecl *AIPathCnxHelperSysInitFn)(WORLDINFO *);
void __cdecl AIPathCnxHelperSysReset(WORLDINFO *world, AI_HELPER_SYSTEM_PC *system) {
    if (system != NULL) {
        if (system->count != 0) {
            memset(system->helpers, 0, (i32)system->count * sizeof(AI_HELPER_SLOT_PC));
            system->count = 0;
        }
        if (AIPathCnxHelperSysInitFn != NULL)
            AIPathCnxHelperSysInitFn(world);
    }
}

typedef struct ai_controller_scene_view_pc_s {
    u8 unknown_0000[0x1394];
    NUGSCN *scene;
} AI_CONTROLLER_SCENE_VIEW_PC;
STATIC_ASSERT(offsetof(AI_CONTROLLER_SCENE_VIEW_PC, scene) == 0x1394);
extern i32 blowup_gizmotype_id;
extern i32 force_gizmotype_id;
extern i32 zipup_gizmotype_id;
extern f32 fakeanimendframe[1];
void *__cdecl CutScene_FindInst(void *, char *);
void *__cdecl GizBuildIt_Find(WORLDINFO *, char *);

struct AIPATHCNXCONTROLLER_s *__cdecl AIPathCnxControllerCreate(AI_CONTROL_SYSTEM_PC *system, AISYS_PC *ai,
                                                                AIPATH_PC *path, char *from, char *to, i32 target_type,
                                                                char *target_name, i32 fake_animation_id,
                                                                i32 gizmo_output) {
    NUSPECIALHANDLE special = {0};
    AIPATHCNX_PC *connection;
    void *target = NULL;
    GIZMO_PC *gizmo;
    NULISTLNK *node;
    struct AIPATHCNXCONTROLLER_s *controller;
    i32 direction, type;
    u8 index;
    if (from == NULL || system == NULL || to == NULL)
        return NULL;
    connection = (AIPATHCNX_PC *)AIPAthFindPathCnx(ai, path, from, to, &direction);
    if (connection != NULL) {
        switch (target_type) {
            case 0:
                if (target_name != NULL)
                    NuSpecialFind(((AI_CONTROLLER_SCENE_VIEW_PC *)ai)->scene, &special, target_name, 1);
                break;
            case 1:
                target = CutScene_FindInst(WORLD->cutscene_sys, target_name);
                break;
            case 2:
                target = GizBuildIt_Find(WORLD, target_name);
                break;
            case 3:
                target = GizmoFindByName(WORLD->gizmo_sys, -1, target_name);
                break;
            case 4:
                type = blowup_gizmotype_id;
                goto typed_gizmo;
            case 5:
                if (fake_animation_id < 0 || fake_animation_id >= 1)
                    return NULL;
                break;
            case 6:
                target = FlowBoxFindByName(WORLD->giz_flow, target_name);
                break;
            case 7:
                type = force_gizmotype_id;
                goto typed_gizmo;
            case 8:
                type = obstacle_gizmotype_id;
                goto typed_gizmo;
            case 9:
                type = zipup_gizmotype_id;
            typed_gizmo:
                gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, type, target_name);
                if (gizmo != NULL)
                    target = gizmo->object;
                break;
        }
    }
    if (target == NULL && NuSpecialExistsFn(&special) == 0 && target_type != 5)
        return NULL;
    node = NuLinkedListGetHead(&system->available);
    if (node == NULL)
        return NULL;
    NuLinkedListRemove(&system->available, node);
    NuLinkedListAppend(&system->active, node);
    controller = (struct AIPATHCNXCONTROLLER_s *)node;
    for (index = 0; index < ai->path_sys->path_count; ++index) {
        if (ai->path_sys->paths[index] == path) {
            controller->path_index = index;
            break;
        }
    }
    controller->flags_word = (u16)((controller->flags_word & ~1u) | (direction & 1));
    controller->connection = connection;
    controller->gizmo_output = gizmo_output;
    if (target != NULL)
        controller->target = target;
    else if (NuSpecialExistsFn(&special) != 0)
        controller->special = special;
    else
        controller->fake_animation_id = fake_animation_id;
    controller->target_type = (u8)target_type;
    return controller;
}

typedef struct controller_animation_view_pc_s {
    u8 unknown_00[0x5c];
    u16 animation_index;
} CONTROLLER_ANIMATION_VIEW_PC;
typedef struct controller_cut_animation_pc_s {
    u8 unknown_00[8];
    f32 end_frame;
} CONTROLLER_CUT_ANIMATION_PC;
typedef struct controller_cut_view_pc_s {
    u8 unknown_00[0x58];
    CONTROLLER_CUT_ANIMATION_PC *animation;
    u8 unknown_5c[0x34];
    f32 current_frame;
} CONTROLLER_CUT_VIEW_PC;
STATIC_ASSERT(offsetof(CONTROLLER_ANIMATION_VIEW_PC, animation_index) == 0x5c);
STATIC_ASSERT(offsetof(CONTROLLER_CUT_ANIMATION_PC, end_frame) == 8);
STATIC_ASSERT(offsetof(CONTROLLER_CUT_VIEW_PC, animation) == 0x58);
void __cdecl AIPathCnxControllerSetOnRange(struct AIPATHCNXCONTROLLER_s *controller, i32 first, i32 last) {
    i32 end = 1;
    i32 frame;
    CONTROLLER_ANIMATION_VIEW_PC *animation;
    void *data;
    NUGSCN *scene;
    if (controller == NULL)
        return;
    switch (controller->target_type) {
        case 0:
            animation = (CONTROLLER_ANIMATION_VIEW_PC *)NuSpecialGetInstAnim(&controller->special);
            if (animation != NULL) {
                scene = (NUGSCN *)controller->special.scene;
                data = scene->instance_animation_data[animation->animation_index];
                if (data != NULL)
                    end = (i32)NuAnimEndFrameOld(data);
            }
            break;
        case 1:
            end = (i32)((CONTROLLER_CUT_VIEW_PC *)controller->target)->animation->end_frame;
            break;
        case 2:
        case 3:
        case 4:
        case 6:
            return;
        case 5:
            end = (i32)fakeanimendframe[controller->fake_animation_id];
            break;
    }
    if (end > 1024)
        end = 1024;
    if (first == -1)
        first = end;
    else if (first == -2)
        first = end - 1;
    if (last == -1)
        last = end;
    else if (last == -2)
        last = end - 1;
    if (first < 1)
        first = 1;
    if (last > end)
        last = end;
    for (frame = first - 1; frame <= last - 1; ++frame)
        controller->on_frames[frame / 32] |= 1u << (frame % 32);
}

STATIC_ASSERT(offsetof(CONTROLLER_CUT_VIEW_PC, current_frame) == 0x90);

/* Native 0x0043d0c0, 840-byte owner including its internal ten-case table. */
STATIC_ASSERT(offsetof(AIPATHCNX_PC, traversal_flags) == 0);
STATIC_ASSERT(offsetof(AIPATHCNX_PC, open) == 0x16);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, build_state) == 0x6d);
STATIC_ASSERT(offsetof(struct ZIPUP_s, flags) == 0x68);
void __cdecl AIPathCnxControlSysUpdate(AI_CONTROL_SYSTEM_PC *system) {
    struct AIPATHCNXCONTROLLER_s *controller;
    NUINSTANIM_TIME_VIEW *animation;
    CONTROLLER_CUT_VIEW_PC *cut;
    void *data;
    i32 end, frame;
    if (system == NULL)
        return;
    controller = (struct AIPATHCNXCONTROLLER_s *)NuLinkedListGetHead(&system->active);
    while (controller != NULL) {
        controller->flags_word &= (u16)~0x20u;
        switch (controller->target_type) {
            case 0:
                if ((controller->flags_word & 4) != 0 && NuSpecialGetVisibilityFn(&controller->special) == 0)
                    goto controller_off;
                animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&controller->special);
                if (animation == NULL)
                    goto frame_one;
                data = ((NUGSCN *)controller->special.scene)->instance_animation_data[animation->animation_index];
                if (data == NULL)
                    goto frame_one;
                end = (i32)NuAnimEndFrameOld(data);
                if (end > 1024)
                    end = 1024;
                frame = (i32)animation->ltime;
                goto clamp_frame;
            case 1:
                cut = (CONTROLLER_CUT_VIEW_PC *)controller->target;
                end = (i32)cut->animation->end_frame;
                if (end > 1024)
                    end = 1024;
                frame = (i32)cut->current_frame;
                goto clamp_frame;
            case 2:
                if (((GIZBUILDIT_PC *)controller->target)->build_state == 2)
                    goto controller_on;
                goto controller_off;
            case 3:
                if (GizmoGetOutput(WORLD->gizmo_sys, controller->target, controller->gizmo_output, 1) != 0)
                    goto controller_on;
                goto controller_off;
            case 4:
                if ((((GIZMOBLOWUP_PC *)controller->target)->status_flags & 1) != 0)
                    goto controller_off;
                goto controller_on;
            case 5:
                end = (i32)fakeanimendframe[controller->fake_animation_id];
                if (end > 1024)
                    end = 1024;
                frame = (i32)fakeanimframe[controller->fake_animation_id];
                goto clamp_frame;
            case 6:
                if ((((FLOWBOX_PC *)controller->target)->state_flags_low & 2) != 0)
                    goto controller_on;
                goto controller_off;
            case 7:
                if (GizForce_Complete((GIZFORCE_PC *)controller->target) != 0)
                    goto controller_on;
                goto controller_off;
            case 8:
                if (((GIZOBSTACLE *)controller->target)->anim_set->state == 2)
                    goto controller_on;
                goto controller_off;
            case 9:
                if ((((struct ZIPUP_s *)controller->target)->flags & 0x40) != 0 &&
                    (i8)((struct ZIPUP_s *)controller->target)->flags < 0)
                    goto controller_on;
                goto controller_off;
            default:
                goto controller_on;
        }
    clamp_frame:
        if (frame < 1)
            goto frame_one;
        if (frame > end)
            frame = end;
        goto test_frame;
    frame_one:
        frame = 1;
    test_frame:
        --frame;
        if ((controller->on_frames[frame / 32] & (1u << ((u32)(frame % 32) & 31))) == 0)
            goto controller_off;
    controller_on:
        controller->flags_word |= 0x20;
        controller->connection->open = (u8)((controller->flags_word >> 3) & 1);
        controller->connection->traversal_flags[controller->flags_word & 1] &= ~controller->off_flags;
        controller->connection->traversal_flags[controller->flags_word & 1] |= controller->on_flags;
        if ((controller->flags_word & 2) != 0) {
            controller->connection->traversal_flags[(~controller->flags_word) & 1] &= ~controller->off_flags;
            controller->connection->traversal_flags[(~controller->flags_word) & 1] |= controller->on_flags;
        }
        goto next_controller;
    controller_off:
        controller->connection->open = (u8)((controller->flags_word >> 4) & 1);
        controller->connection->traversal_flags[controller->flags_word & 1] &= ~controller->on_flags;
        controller->connection->traversal_flags[controller->flags_word & 1] |= controller->off_flags;
        if ((controller->flags_word & 2) != 0) {
            controller->connection->traversal_flags[(~controller->flags_word) & 1] &= ~controller->on_flags;
            controller->connection->traversal_flags[(~controller->flags_word) & 1] |= controller->off_flags;
        }
    next_controller:
        controller = (struct AIPATHCNXCONTROLLER_s *)NuLinkedListGetNext(&system->active, &controller->links);
    }
}
