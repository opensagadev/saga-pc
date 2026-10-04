#include "saga.h"

extern i32 obstacle_gizmotype_id;
extern const char GizObstacleActionNamePC[5];
extern const char GizObstacleActionBackwardPC[9];
extern const char GizObstacleActionForwardPC[8];
extern const char GizObstacleActionStayOpenPC[9];
extern const char GizObstacleActionStayShutPC[9];
extern const char GizmoSnapOptionNamePc[5];

void __cdecl GizObstacle_JumpToStart(GIZOBSTACLE *obstacle) {
    if (obstacle && obstacle->anim_set)
        GameAnimSet_JumpToStart(obstacle->anim_set);
}
void __cdecl GizObstacle_JumpToEnd(GIZOBSTACLE *obstacle) {
    if (obstacle && obstacle->anim_set)
        GameAnimSet_JumpToEnd(obstacle->anim_set);
}
void __cdecl GizObstacle_PlayForwards(GIZOBSTACLE *obstacle) {
    if (obstacle) {
        GameAnimSet_SetRepeating(obstacle->anim_set, obstacle->playback_state == 2);
        if (obstacle->animation_speed < 0.0f)
            GameAnimSet_Play(obstacle->anim_set, obstacle->animation_speed * obstacle->backward_speed_factor, 0);
        else
            GameAnimSet_Play(obstacle->anim_set, obstacle->animation_speed * obstacle->forward_speed_factor, 0);
    }
}
void __cdecl GizObstacle_PlayBackwards(GIZOBSTACLE *obstacle) {
    if (obstacle) {
        GameAnimSet_SetRepeating(obstacle->anim_set, obstacle->playback_state == 2);
        if (obstacle->animation_speed < 0.0f)
            GameAnimSet_Play(obstacle->anim_set, -obstacle->animation_speed * obstacle->forward_speed_factor, 0);
        else
            GameAnimSet_Play(obstacle->anim_set, -obstacle->animation_speed * obstacle->backward_speed_factor, 0);
    }
}
void __cdecl GizActions_PlayObstacle(GIZFLOW_PC *flow, FLOWBOX_PC *box, char **params, i32 count) {
    char *name = NULL;
    i32 forwards = 1, snap = 0, stay_open = 0, stay_shut = 0, i;
    GIZMO_PC *gizmo;
    GIZOBSTACLE *obstacle;
    u32 flags;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)GizObstacleActionNamePC);
        if (value)
            name = value + NuStrLen(GizObstacleActionNamePC) + 1;
        else if (!NuStrICmp(params[i], GizObstacleActionBackwardPC))
            forwards = 0;
        else if (!NuStrICmp(params[i], GizObstacleActionForwardPC))
            forwards = 1;
        else if (!NuStrICmp(params[i], GizmoSnapOptionNamePc))
            snap = 1;
        else if (!NuStrICmp(params[i], GizObstacleActionStayOpenPC))
            stay_open = 1;
        else if (!NuStrICmp(params[i], GizObstacleActionStayShutPC))
            stay_shut = 1;
    }
    if (!name)
        return;
    gizmo = (GIZMO_PC *)GizmoFindByName(flow->gizmo_sys, obstacle_gizmotype_id, name);
    obstacle = gizmo ? (GIZOBSTACLE *)gizmo->object : NULL;
    if (!obstacle)
        return;
    if (forwards) {
        if (snap)
            GizObstacle_JumpToEnd(obstacle);
        else
            GizObstacle_PlayForwards(obstacle);
    } else if (snap)
        GizObstacle_JumpToStart(obstacle);
    else
        GizObstacle_PlayBackwards(obstacle);
    flags = obstacle->runtime_flags;
    flags &= ~0xcu;
    flags |= (stay_shut & 1) << 3;
    flags |= stay_open << 2;
    obstacle->runtime_flags = flags;
}

extern void *__cdecl GizmoBufferAlloc(VARIPTR *, VARIPTR *, i32);
typedef struct gizobstacle_system_pc_s {
    GIZOBSTACLE *obstacles;
    GIZMO_PC **active_gizmos;
    u16 count, capacity, active_gizmo_count, unknown_00e;
    GAMEANIMOBJPOOL *anim_pool;
} GIZOBSTACLE_SYSTEM_PC;
STATIC_ASSERT(sizeof(GIZOBSTACLE_SYSTEM_PC) == 0x14);
STATIC_ASSERT(offsetof(GIZOBSTACLE_SYSTEM_PC, count) == 8);
STATIC_ASSERT(offsetof(GIZOBSTACLE_SYSTEM_PC, capacity) == 0xa);
STATIC_ASSERT(offsetof(GIZOBSTACLE_SYSTEM_PC, active_gizmo_count) == 0xc);
STATIC_ASSERT(offsetof(GIZOBSTACLE_SYSTEM_PC, anim_pool) == 0x10);
typedef struct gizobstacle_progress_pc_s {
    u32 enabled[4], visible[4], pickups_spawned[4], ai_active[4], blocked[4], trigger_latched[4], destroyed[4];
} GIZOBSTACLE_PROGRESS_PC;
STATIC_ASSERT(sizeof(GIZOBSTACLE_PROGRESS_PC) == 0x70);

void __cdecl GizmoObstacle_Activate(void *ptr, i32 activate) {
    GIZMO_PC *gizmo = (GIZMO_PC *)ptr;
    GIZOBSTACLE *obstacle;
    if (!gizmo)
        return;
    obstacle = (GIZOBSTACLE *)gizmo->object;
    if (activate && !(obstacle->progress_flags & 1))
        GameAnimSet_JumpToStart(obstacle->anim_set);
    obstacle->proximity_output = 0;
    obstacle->progress_flags = (obstacle->progress_flags & ~1u) | (activate != 0);
    if (activate) {
        obstacle->runtime_flags &= ~0x80u;
        obstacle->progress_flags &= ~0x100u;
        GameAnimSet_EvaluateState(obstacle->anim_set);
    }
}
void __cdecl GizmoObstacle_SetVisibility(void *ptr, i32 visibility) {
    GIZMO_PC *gizmo = (GIZMO_PC *)ptr;
    GIZOBSTACLE *obstacle = gizmo ? (GIZOBSTACLE *)gizmo->object : NULL;
    if (!obstacle)
        return;
    GameAnimSet_SetVisibility(obstacle->anim_set, visibility);
    obstacle->progress_flags = (obstacle->progress_flags & ~2u) | ((visibility != 0) << 1);
}
NUVEC *__cdecl GizmoObstacle_GetPos(GIZMO_PC *gizmo) {
    GIZOBSTACLE *obstacle = gizmo ? (GIZOBSTACLE *)gizmo->object : NULL;
    return obstacle ? &obstacle->position : NULL;
}
char *__cdecl GizmoObstacle_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo ? (char *)gizmo->object : NULL;
}
i32 __cdecl GizmoObstacle_GetNumOutputs(GIZMO_PC *gizmo) {
    (void)gizmo;
    return 5;
}
i32 __cdecl GizmoObstacle_GetOutput(GIZMO_PC *gizmo, i32 output, i32 ignore_activation) {
    GIZOBSTACLE *obstacle = (GIZOBSTACLE *)gizmo->object;
    if (!obstacle)
        return 0;
    if ((obstacle->progress_flags & 3) != 3 && !ignore_activation)
        return 0;
    switch (output) {
        case 0:
            return obstacle->anim_set && obstacle->anim_set->state == 2;
        case 1:
            return obstacle->anim_set && obstacle->anim_set->state != 0;
        case 2:
            return obstacle->anim_set && obstacle->proximity_output != 0;
        case 3:
            return obstacle->anim_set && obstacle->anim_set->state == 0;
        case 4:
            return obstacle->anim_set && obstacle->anim_set->state == 1;
        default:
            return 0;
    }
}
i32 __cdecl GizmoObstacle_ActivateRev(void *ptr, i32 activate, i32 flags) {
    GIZMO_PC *gizmo = (GIZMO_PC *)ptr;
    GIZOBSTACLE *obstacle = gizmo ? (GIZOBSTACLE *)gizmo->object : NULL;
    i32 ai_active, reverse;
    if (!obstacle)
        return 0;
    if ((obstacle->progress_flags & 0x100) && (obstacle->anim_set->state == 2 || obstacle->anim_set->state == 1))
        obstacle->progress_flags = (obstacle->progress_flags & ~0x100u) | 1;
    ai_active = (obstacle->runtime_flags >> 2) & 1;
    if (ai_active)
        obstacle->progress_flags = (obstacle->progress_flags & ~0x100u) | 1;
    if (!(flags & 1)) {
        if (activate) {
            GizObstacle_PlayBackwards((GIZOBSTACLE *)obstacle);
            obstacle->progress_flags = (obstacle->progress_flags & ~1u) | 0x100;
        } else
            obstacle->progress_flags = (obstacle->progress_flags & ~0x100u) | 1;
        return 1;
    }
    reverse = (obstacle->progress_flags >> 8) & 1;
    if (activate == reverse && (!(flags & 4) || ((obstacle->progress_flags >> 1) & 1) != activate))
        return 0;
    if (activate == 1 && ai_active)
        return 0;
    return 1;
}
void *__cdecl GizObstacles_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(GIZOBSTACLE_PROGRESS_PC));
}
void __cdecl GizObstacles_ClearProgress(void *world, void *ptr) {
    GIZOBSTACLE_PROGRESS_PC *progress = (GIZOBSTACLE_PROGRESS_PC *)ptr;
    i32 i;
    (void)world;
    if (!progress)
        return;
    for (i = 0; i < 4; ++i) {
        progress->enabled[i] = progress->visible[i] = 0xffffffff;
        progress->pickups_spawned[i] = progress->ai_active[i] = progress->blocked[i] = 0;
        progress->trigger_latched[i] = progress->destroyed[i] = 0;
    }
}
void __cdecl GizObstacles_StoreProgress(void *world, void *ptr, void *progress_ptr) {
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)ptr;
    GIZOBSTACLE_PROGRESS_PC *progress = (GIZOBSTACLE_PROGRESS_PC *)progress_ptr;
    i32 i;
    GizObstacles_ClearProgress(world, progress);
    if (!progress)
        return;
    for (i = 0; i < system->count && i < 128; ++i) {
        GIZOBSTACLE *obstacle = &system->obstacles[i];
        i32 word = i >> 5;
        u32 bit = 1u << (i & 31);
        if (!(obstacle->progress_flags & 2))
            progress->visible[word] &= ~bit;
        if (!(obstacle->progress_flags & 1))
            progress->enabled[word] &= ~bit;
        if (obstacle->runtime_flags & 1)
            progress->pickups_spawned[word] |= bit;
        if (obstacle->runtime_flags & 4)
            progress->ai_active[word] |= bit;
        if (obstacle->runtime_flags & 8)
            progress->blocked[word] |= bit;
        if (obstacle->runtime_flags & 0x40)
            progress->trigger_latched[word] |= bit;
        if (obstacle->runtime_flags & 0x80)
            progress->destroyed[word] |= bit;
    }
}
void __cdecl GizObstacles_AddLevelSfx(void *world, void *ptr, i32 *ids, i32 *count, i32 maximum) {
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)ptr;
    i32 i;
    (void)world;
    if (!system)
        return;
    for (i = 0; i < system->count; ++i) {
        GIZOBSTACLE *obstacle = &system->obstacles[i];
        if (obstacle->start_sfx_id != -1)
            AddLevelSfxFromId(obstacle->start_sfx_id, ids, count, maximum);
        if (obstacle->stop_sfx_id != -1)
            AddLevelSfxFromId(obstacle->stop_sfx_id, ids, count, maximum);
    }
}
