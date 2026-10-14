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
