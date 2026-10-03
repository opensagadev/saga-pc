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
