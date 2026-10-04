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

static void __cdecl Gizobstacle_ReadAnimSetData(GAMEANIMOBJ *object, u8 version) {
    u16 fallback_data = 0;
    u16 *data;
    if (!object)
        return;
    data = (u16 *)object->object_data;
    if (!data)
        data = &fallback_data;
    if (version > 7)
        *data = (u16)EdFileReadShort();
    else if (object->flags & 2)
        *data |= 1;
}
void *__cdecl GizObstacles_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system;
    i32 i;
    system = (GIZOBSTACLE_SYSTEM_PC *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(*system));
    system->capacity = world->current_level->max_obstacles;
    system->obstacles = (GIZOBSTACLE *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, system->capacity * 0xa0);
    system->active_gizmos = (GIZMO_PC **)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                         system->capacity * sizeof(*system->active_gizmos));
    system->anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 4,
                                                     (u16)world->current_level->max_obstacle_objects);
    for (i = 0; i < system->capacity; ++i)
        system->obstacles[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_pool, world->game_anim_sys);
    world->giz_obstacle_sys = system;
    return system;
}
i32 __cdecl GizObstacles_Load(void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)data;
    u8 version = (u8)EdFileReadChar();
    i32 index;
    GIZOBSTACLE *obstacle;
    system->count = (u16)EdFileReadShort();
    obstacle = (GIZOBSTACLE *)system->obstacles;
    for (index = 0; index < system->count; ++index, ++obstacle) {
        EdFileRead(obstacle->name, sizeof(obstacle->name));
        EdFileReadNuVec(&obstacle->position);
        if (version > 1)
            EdFileReadNuVec(&obstacle->secondary_position);
        else
            obstacle->secondary_position = obstacle->position;
        obstacle->auto_return_delay = EdFileReadFloat();
        obstacle->trigger_radius = EdFileReadFloat();
        if (version > 2) {
            EdFileReadNuVec(&obstacle->trigger_box_half_extents);
            obstacle->trigger_box_yaw = EdFileReadShort();
        } else
            obstacle->trigger_box_half_extents.x = obstacle->trigger_box_half_extents.y =
                obstacle->trigger_box_half_extents.z = .25f;
        obstacle->config_flags = EdFileReadInt();
        if (version >= 12)
            obstacle->exclude_flags = EdFileReadInt();
        if (version == 6) {
            EdFileReadShort();
            EdFileReadChar();
        }
        obstacle->playback_state = EdFileReadChar();
        obstacle->mode = EdFileReadChar();
        obstacle->trigger_mode = version >= 7 ? (u8)EdFileReadChar() : 0xff;
        GizmoFileReadGameAnimSet(obstacle->anim_set, world, Gizobstacle_ReadAnimSetData, version,
                                 (char *)GizObstacleTypeNamePc, obstacle->name);
        obstacle->forward_speed_factor = version >= 4 ? EdFileReadFloat() : 1.f;
        obstacle->backward_speed_factor = version >= 5 ? EdFileReadFloat() : obstacle->forward_speed_factor;
        if (version >= 8)
            obstacle->reflection_alpha = EdFileReadFloat();
        obstacle->blowup_type = -1;
        if (version >= 9) {
            if (version >= 10) {
                char name[32];
                i32 length = (i8)EdFileReadChar();
                if (length) {
                    EdFileRead(name, length);
                    obstacle->blowup_type = (i16)GizmoBlowupGetNameTableId(name);
                    if (obstacle->blowup_type != -1)
                        obstacle->runtime_flags |= 2;
                }
            } else
                obstacle->blowup_type = EdFileReadShort();
            obstacle->pickup_count = EdFileReadShort();
            obstacle->pickup_direction_x = EdFileReadShort();
            obstacle->pickup_direction_y = EdFileReadShort();
            EdFileReadNuVec(&obstacle->pickup_offset);
        }
        obstacle->pickup_scatter_height =
            version >= 11 ? EdFileReadFloat() : (world->area && (world->area->flags & 1) ? 12.f : 1.75f);
        obstacle->start_sfx_id = obstacle->stop_sfx_id = -1;
        if (version >= 13) {
            char name[32];
            if (GizmoFileReadName(name))
                obstacle->start_sfx_id = (i16)GetSfxId(name);
        } else if (GizObstacle_SetDefaultSFXFn)
            GizObstacle_SetDefaultSFXFn(world, (GIZOBSTACLE *)obstacle);
        if (version >= 14) {
            char name[32];
            if (GizmoFileReadName(name))
                obstacle->stop_sfx_id = (i16)GetSfxId(name);
        }
    }
    return 1;
}
void __cdecl GizObstacles_PostLoad(void *world, void *data) {
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)data;
    i32 i;
    GIZOBSTACLE *obstacle;
    if (!system)
        return;
    obstacle = (GIZOBSTACLE *)system->obstacles;
    for (i = 0; i < system->count; ++i, ++obstacle) {
        if (obstacle->runtime_flags & 2) {
            obstacle->blowup_type = (i16)GizmoBlowupGetTypeFromNameTableId((WORLDINFO *)world, obstacle->blowup_type);
            obstacle->runtime_flags &= ~2u;
        }
    }
}

static void GizObstacle_ResetOnePrivate(WORLDINFO *world, GIZOBSTACLE *obstacle) {
    GAMEANIMSET *set;
    GAMEANIMOBJ *object;
    obstacle->progress_flags = (obstacle->progress_flags & ~0xcu) | 3;
    obstacle->runtime_flags &= 0xffffff03u;
    obstacle->animation_speed = 1.f;
    set = obstacle->anim_set;
    if (!set)
        return;
    for (object = set->objects; object; object = object->next) {
        i16 *object_data = (i16 *)object->object_data;
        object_data[1] = -1;
        if (world->terrain && !(object_data[0] & 1) && NuSpecialExistsFn(&object->special)) {
            object_data[1] = (i16)FindPlatInst(NuSpecialGetInstanceix(&object->special));
            if (object_data[1] != -1)
                obstacle->runtime_flags |= 0x10;
        }
        if (object_data[0] & 2)
            obstacle->runtime_flags |= 0x20;
    }
    obstacle->room_id = world->current_gscn ? (i16)NuPortalWhichRoom(world->current_gscn, &obstacle->position) : -1;
    if (obstacle->config_flags & 2)
        GameAnimSet_GetAveragePos(obstacle->anim_set, &obstacle->secondary_position, 0, 1, 1);
    GameAnimSet_EvaluateState(obstacle->anim_set);
    obstacle->evaluated_radius = 1.f;
    obstacle->evaluated_position = obstacle->position;
    GameAnimSet_GetCentreAndRadius(obstacle->anim_set, &obstacle->evaluated_position, &obstacle->evaluated_radius, 2, 1,
                                   1);
}
void __cdecl GizObstacles_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)data;
    GIZOBSTACLE_PROGRESS_PC *progress = (GIZOBSTACLE_PROGRESS_PC *)progress_ptr;
    i32 i;
    GIZOBSTACLE *obstacle = (GIZOBSTACLE *)system->obstacles;
    for (i = 0; i < system->count; ++i, ++obstacle) {
        GizObstacle_ResetOnePrivate(world, obstacle);
        if (progress && i < 128) {
            i32 word = i >> 5;
            u32 bit = 1u << (i & 31);
            obstacle->progress_flags = (obstacle->progress_flags & ~2u) | ((progress->visible[word] & bit) != 0) * 2;
            obstacle->progress_flags = (obstacle->progress_flags & ~1u) | ((progress->enabled[word] & bit) != 0);
            obstacle->runtime_flags = (obstacle->runtime_flags & ~1u) | ((progress->pickups_spawned[word] & bit) != 0);
            obstacle->runtime_flags = (obstacle->runtime_flags & ~4u) | ((progress->ai_active[word] & bit) != 0) * 4;
            obstacle->runtime_flags = (obstacle->runtime_flags & ~8u) | ((progress->blocked[word] & bit) != 0) * 8;
            obstacle->runtime_flags =
                (obstacle->runtime_flags & ~0x40u) | ((progress->trigger_latched[word] & bit) != 0) * 0x40;
            obstacle->runtime_flags =
                (obstacle->runtime_flags & ~0x80u) | ((progress->destroyed[word] & bit) != 0) * 0x80;
        }
    }
    GizObstacleTriggerCountPc = 0;
}
