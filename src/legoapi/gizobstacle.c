#include "saga.h"

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(GIZBOMBGEN_s, name) == 0);
STATIC_ASSERT(offsetof(GIZBOMBGEN_s, position) == 0x10);
extern const char GizmoSnapOptionNamePc[5];

void GizObstacle_JumpToStart(GIZOBSTACLE *obstacle) {
    if (obstacle && obstacle->anim_set) {
        GameAnimSet_JumpToStart(obstacle->anim_set);
    }
}

void GizObstacle_JumpToEnd(GIZOBSTACLE *obstacle) {
    if (obstacle && obstacle->anim_set) {
        GameAnimSet_JumpToEnd(obstacle->anim_set);
    }
}

void GizObstacle_PlayForwards(GIZOBSTACLE *obstacle) {
    if (obstacle) {
        GameAnimSet_SetRepeating(obstacle->anim_set, obstacle->playback_state == 2);
        if (obstacle->animation_speed < 0.0f) {
            GameAnimSet_Play(obstacle->anim_set, obstacle->animation_speed * obstacle->backward_speed_factor, 0);
        } else {
            GameAnimSet_Play(obstacle->anim_set, obstacle->animation_speed * obstacle->forward_speed_factor, 0);
        }
    }
}

void GizObstacle_PlayBackwards(GIZOBSTACLE *obstacle) {
    if (obstacle) {
        GameAnimSet_SetRepeating(obstacle->anim_set, obstacle->playback_state == 2);
        if (obstacle->animation_speed < 0.0f) {
            GameAnimSet_Play(obstacle->anim_set, -obstacle->animation_speed * obstacle->forward_speed_factor, 0);
        } else {
            GameAnimSet_Play(obstacle->anim_set, -obstacle->animation_speed * obstacle->backward_speed_factor, 0);
        }
    }
}

STATIC_ASSERT(offsetof(WORLDINFO, progress_flag_5174) == 0x5170);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_index) == 0x400);

typedef struct gizobstacle_system_pc_s {
    GIZOBSTACLE *obstacles;     // 0x00
    GIZMO_PC **active_gizmos;   // 0x04
    u16 count;                  // 0x08
    u16 capacity;               // 0x0a
    u16 active_gizmo_count;     // 0x0c
    u16 unknown_00e;            // 0x0e
    GAMEANIMOBJPOOL *anim_pool; // 0x10
} GIZOBSTACLE_SYSTEM_PC;        // size 0x14

typedef struct gizobstacle_progress_pc_s {
    u32 enabled[4];         // 0x00
    u32 visible[4];         // 0x10
    u32 pickups_spawned[4]; // 0x20
    u32 ai_active[4];       // 0x30
    u32 blocked[4];         // 0x40
    u32 trigger_latched[4]; // 0x50
    u32 destroyed[4];       // 0x60
} GIZOBSTACLE_PROGRESS_PC;  // size 0x70

typedef struct obstacle_progress_flags_pc_s {
    u32 active : 1;           // 0x0:0
    u32 visible : 1;          // 0x0:1
    u32 unknown : 30;         // 0x0:2
} OBSTACLE_PROGRESS_FLAGS_PC; // size 0x4

typedef struct obstacle_runtime_flags_pc_s {
    u32 pickups_spawned : 1;     // 0x0:0
    u32 pending_blowup_type : 1; // 0x0:1
    u32 ai_active : 1;           // 0x0:2
    u32 blocked : 1;             // 0x0:3
    u32 has_platform : 1;        // 0x0:4
    u32 anim_object_bit_1 : 1;   // 0x0:5
    u32 trigger_latched : 1;     // 0x0:6
    u32 destroyed : 1;           // 0x0:7
    u32 unknown : 24;            // 0x0:8
} OBSTACLE_RUNTIME_FLAGS_PC;     // size 0x4

STATIC_ASSERT(offsetof(GIZOBSTACLE, progress_flags) == 0x94);

void GizmoObstacle_Activate(GIZMO_PC *ptr, i32 activate) {
    GIZMO_PC *gizmo = ptr;
    GIZOBSTACLE *obstacle;
    if (!gizmo) {
        return;
    }
    obstacle = gizmo->object;
    if (activate && !(obstacle->progress_flags & 1)) {
        GameAnimSet_JumpToStart(obstacle->anim_set);
    }
    obstacle->proximity_output = 0;
    ((OBSTACLE_PROGRESS_FLAGS_PC *)&obstacle->progress_flags)->active = activate != 0;
    if (activate) {
        obstacle->runtime_flags &= ~0x80u;
        obstacle->progress_flags &= ~0x100u;
        GameAnimSet_EvaluateState(obstacle->anim_set);
    }
}

void GizmoObstacle_SetVisibility(GIZMO_PC *ptr, i32 visibility) {
    GIZMO_PC *gizmo = ptr;
    GIZOBSTACLE *obstacle = gizmo ? (GIZOBSTACLE *)gizmo->object : NULL;
    if (!obstacle) {
        return;
    }
    GameAnimSet_SetVisibility(obstacle->anim_set, visibility);
    ((OBSTACLE_PROGRESS_FLAGS_PC *)&obstacle->progress_flags)->visible = visibility != 0;
}

NUVEC *GizmoObstacle_GetPos(GIZMO_PC *gizmo) {
    GIZOBSTACLE *obstacle = gizmo ? (GIZOBSTACLE *)gizmo->object : NULL;
    return obstacle ? &obstacle->position : NULL;
}

char *GizmoObstacle_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo ? (char *)gizmo->object : NULL;
}

i32 GizmoObstacle_GetNumOutputs(GIZMO_PC *gizmo) {
    (void)gizmo;
    return 5;
}

i32 GizmoObstacle_GetOutput(GIZMO_PC *gizmo, i32 output, i32 ignore_activation) {
    GIZOBSTACLE *obstacle = gizmo->object;
    if (!obstacle) {
        return 0;
    }
    if (((obstacle->progress_flags & 2) == 0 || (obstacle->progress_flags & 1) == 0) && !ignore_activation) {
        return 0;
    }
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

i32 GizmoObstacle_ActivateRev(GIZMO_PC *gizmo, i32 activate, i32 flags) {
    GIZOBSTACLE *obstacle;
    i32 ai_active;
    if (!gizmo) {
        return 0;
    }
    obstacle = gizmo->object;
    if (!obstacle) {
        return 0;
    }
    if ((obstacle->progress_flags & 0x100) && (obstacle->anim_set->state == 2 || obstacle->anim_set->state == 1)) {
        obstacle->progress_flags = (obstacle->progress_flags & ~0x100u) | 1;
    }
    ai_active = (obstacle->runtime_flags >> 2) & 1;
    if (ai_active) {
        obstacle->progress_flags = (obstacle->progress_flags & ~0x100u) | 1;
    }
    if (flags & 1) {
        if (activate == ((obstacle->progress_flags >> 8) & 1) &&
            (!(flags & 4) || ((obstacle->progress_flags >> 1) & 1) != activate)) {
            return 0;
        }
        if (activate == 1 && ai_active) {
            return 0;
        }
        return 1;
    }
    if (activate) {
        GizObstacle_PlayBackwards(obstacle);
        obstacle->progress_flags = (obstacle->progress_flags & ~1u) | 0x100;
        return 1;
    }
    obstacle->progress_flags = (obstacle->progress_flags & ~0x100u) | 1;
    return 1;
}

void *GizObstacles_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(GIZOBSTACLE_PROGRESS_PC));
}

void GizObstacles_ClearProgress(void *world, void *ptr) {
    GIZOBSTACLE_PROGRESS_PC *progress = ptr;
    (void)world;
    if (!progress) {
        return;
    }
    memset(progress->enabled, -1, sizeof(progress->enabled));
    memset(progress->visible, -1, sizeof(progress->visible));
    memset(progress->pickups_spawned, 0, sizeof(progress->pickups_spawned));
    memset(progress->ai_active, 0, sizeof(progress->ai_active));
    memset(progress->blocked, 0, sizeof(progress->blocked));
    memset(progress->trigger_latched, 0, sizeof(progress->trigger_latched));
    memset(progress->destroyed, 0, sizeof(progress->destroyed));
}

GIZOBSTACLE *GizObstacle_FindByName(GIZOBSTACLESYS_s *system, char *name) {
    GIZOBSTACLE *obstacle = NULL;
    i32 index;
    if (system != NULL && name != NULL) {
        obstacle = system->obstacles;
        for (index = 0; index < system->count; ++index, ++obstacle) {
            if (NuStrICmp(obstacle->name, name) == 0) {
                return obstacle;
            }
        }
    }
    return obstacle;
}

void GizObstacles_StoreProgress(void *world, void *ptr, void *progress_ptr) {
    GIZOBSTACLE_SYSTEM_PC *system = ptr;
    GIZOBSTACLE_PROGRESS_PC *progress = progress_ptr;
    i32 i;
    GIZOBSTACLE *obstacle;
    GizObstacles_ClearProgress(world, progress);
    if (!progress) {
        return;
    }
    obstacle = system->obstacles;
    for (i = 0; i < system->count; ++i, ++obstacle) {
        i32 word;
        u32 bit;
        if (i >= 128) {
            break;
        }
        word = i / 32;
        bit = 1u << (i & 31);
        if (!(obstacle->progress_flags & 2)) {
            progress->visible[word] &= ~bit;
        }
        if (!(obstacle->progress_flags & 1)) {
            progress->enabled[word] &= ~bit;
        }
        if (obstacle->runtime_flags & 1) {
            progress->pickups_spawned[word] |= bit;
        }
        if (obstacle->runtime_flags & 4) {
            progress->ai_active[word] |= bit;
        }
        if (obstacle->runtime_flags & 8) {
            progress->blocked[word] |= bit;
        }
        if (obstacle->runtime_flags & 0x40) {
            progress->trigger_latched[word] |= bit;
        }
        if (obstacle->runtime_flags & 0x80) {
            progress->destroyed[word] |= bit;
        }
    }
}

void GizObstacles_AddLevelSfx(void *world, void *ptr, i32 *ids, i32 *count, i32 maximum) {
    GIZOBSTACLE_SYSTEM_PC *system = ptr;
    i32 i;
    GIZOBSTACLE *obstacle;
    (void)world;
    if (!system) {
        return;
    }
    obstacle = system->obstacles;
    for (i = 0; i < system->count; ++i, ++obstacle) {
        if (obstacle->start_sfx_id != -1) {
            AddLevelSfxFromId(obstacle->start_sfx_id, ids, count, maximum);
        }
        if (obstacle->stop_sfx_id != -1) {
            AddLevelSfxFromId(obstacle->stop_sfx_id, ids, count, maximum);
        }
    }
}

static void Gizobstacle_ReadAnimSetData(GAMEANIMOBJ *object, u8 version) {
    union {
        u32 storage;
        u16 value;
    } fallback_data;

    u16 *data;
    STATIC_ASSERT(sizeof(fallback_data) == 4);
    if (!object) {
        return;
    }
    data = object->object_data;
    if (!data) {
        fallback_data.storage = 0;
        data = &fallback_data.value;
    }
    if (version > 7) {
        *data = (u16)EdFileReadShort();
    } else if (object->flags & 2) {
        *data |= 1;
    }
}

void *GizObstacles_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system;
    i32 i;
    system = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(*system));
    system->capacity = world->current_level->max_obstacles;
    system->obstacles = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, system->capacity * 0xa0);
    system->active_gizmos =
        GameBufferAlloc(&world->giz_buffer, &world->buffer_end, system->capacity * sizeof(*system->active_gizmos));
    system->anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 4,
                                                     (u16)world->current_level->max_obstacle_objects);
    for (i = 0; i < system->capacity; ++i) {
        system->obstacles[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_pool, world->game_anim_sys);
    }
    world->giz_obstacle_sys = system;
    return system;
}

i32 GizObstacles_Load(void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system = data;
    u8 version = (u8)EdFileReadChar();
    i32 index;
    GIZOBSTACLE *obstacle;
    system->count = (u16)EdFileReadShort();
    if (system->count != 0) {
        obstacle = system->obstacles;
        for (index = 0; index < system->count; ++index, ++obstacle) {
            char sfx_name[64];
            EdFileRead(obstacle->name, sizeof(obstacle->name));
            EdFileReadNuVec(&obstacle->position);
            if (version > 1) {
                EdFileReadNuVec(&obstacle->secondary_position);
            } else {
                obstacle->secondary_position = obstacle->position;
            }
            obstacle->auto_return_delay = EdFileReadFloat();
            obstacle->trigger_radius = EdFileReadFloat();
            if (version > 2) {
                EdFileReadNuVec(&obstacle->trigger_box_half_extents);
                obstacle->trigger_box_yaw = EdFileReadShort();
            } else {
                obstacle->trigger_box_half_extents.x = .25f;
                obstacle->trigger_box_half_extents.y = .25f;
                obstacle->trigger_box_half_extents.z = .25f;
            }
            obstacle->config_flags = EdFileReadInt();
            if (version >= 12) {
                obstacle->exclude_flags = EdFileReadInt();
            }
            if (version == 6) {
                EdFileReadShort();
                EdFileReadChar();
            }
            obstacle->playback_state = EdFileReadChar();
            obstacle->mode = EdFileReadChar();
            if (version >= 7) {
                obstacle->trigger_mode = EdFileReadChar();
            } else {
                obstacle->trigger_mode = 0xff;
            }
            GizmoFileReadGameAnimSet(obstacle->anim_set, world, Gizobstacle_ReadAnimSetData, version,
                                     (char *)GizObstacleTypeNamePc, obstacle->name);
            if (version >= 4) {
                obstacle->forward_speed_factor = EdFileReadFloat();
            } else {
                obstacle->forward_speed_factor = 1.f;
            }
            if (version >= 5) {
                obstacle->backward_speed_factor = EdFileReadFloat();
            } else {
                obstacle->backward_speed_factor = obstacle->forward_speed_factor;
            }
            if (version >= 8) {
                obstacle->reflection_alpha = EdFileReadFloat();
            }
            obstacle->blowup_type = -1;
            if (version >= 9) {
                if (version >= 10) {
                    char name[32];
                    i32 length = EdFileReadChar();
                    if (length) {
                        EdFileRead(name, length);
                        obstacle->blowup_type = (i16)GizmoBlowupGetNameTableId(name);
                        if (obstacle->blowup_type != -1) {
                            obstacle->runtime_flags |= 2;
                        }
                    }
                } else {
                    obstacle->blowup_type = EdFileReadShort();
                }
                obstacle->pickup_count = EdFileReadShort();
                obstacle->pickup_direction_x = EdFileReadShort();
                obstacle->pickup_direction_y = EdFileReadShort();
                EdFileReadNuVec(&obstacle->pickup_offset);
            }
            if (version >= 11) {
                obstacle->pickup_scatter_height = EdFileReadFloat();
            } else if (world->area && (world->area->flags & 1)) {
                obstacle->pickup_scatter_height = 12.f;
            } else {
                obstacle->pickup_scatter_height = 1.75f;
            }
            obstacle->start_sfx_id = -1;
            obstacle->stop_sfx_id = -1;
            if (version >= 13) {
                if (GizmoFileReadName(sfx_name)) {
                    obstacle->start_sfx_id = (i16)GetSfxId(sfx_name);
                }
            } else if (GizObstacle_SetDefaultSFXFn) {
                GizObstacle_SetDefaultSFXFn(world, obstacle);
            }
            if (version >= 14) {
                if (GizmoFileReadName(sfx_name)) {
                    obstacle->stop_sfx_id = (i16)GetSfxId(sfx_name);
                }
            }
        }
    }
    return 1;
}

void GizObstacles_PostLoad(void *world, void *data) {
    GIZOBSTACLE_SYSTEM_PC *system = data;
    i32 i;
    GIZOBSTACLE *obstacle;
    if (!system) {
        return;
    }
    obstacle = system->obstacles;
    for (i = 0; i < system->count; ++i, ++obstacle) {
        if (obstacle->runtime_flags & 2) {
            obstacle->blowup_type = (i16)GizmoBlowupGetTypeFromNameTableId(world, obstacle->blowup_type);
            obstacle->runtime_flags &= ~2u;
        }
    }
}

static void GizObstacle_EvalAveragePosAndRadius(GIZOBSTACLE *obstacle, i32 state) {
    obstacle->evaluated_position = obstacle->position;
    obstacle->evaluated_radius = 1.0f;
    GameAnimSet_GetCentreAndRadius(obstacle->anim_set, &obstacle->evaluated_position, &obstacle->evaluated_radius,
                                   state, 1, 1);
}

static void GizObstacle_ResetOnePrivate(WORLDINFO *world, GIZOBSTACLE *obstacle) {
    GAMEANIMSET *set;
    GAMEANIMOBJ *object;
    obstacle->progress_flags = (obstacle->progress_flags & ~0xcu) | 3;
    obstacle->runtime_flags &= 0xffffff03u;
    obstacle->animation_speed = 1.f;
    set = obstacle->anim_set;
    if (!set) {
        return;
    }
    for (object = set->objects; object; object = object->next) {
        i16 *object_data = object->object_data;
        object_data[1] = -1;
        if (world->terrain && !(object_data[0] & 1) && NuSpecialExistsFn(&object->special)) {
            object_data[1] = (i16)FindPlatInst(NuSpecialGetInstanceix(&object->special));
            if (object_data[1] != -1) {
                obstacle->runtime_flags |= 0x10;
            }
        }
        if (object_data[0] & 2) {
            obstacle->runtime_flags |= 0x20;
        }
    }
    obstacle->room_id = world->current_gscn ? NuPortalWhichRoom(world->current_gscn, &obstacle->position) : -1;
    if (obstacle->config_flags & 2) {
        GameAnimSet_GetAveragePos(obstacle->anim_set, &obstacle->secondary_position, 0, 1, 1);
    }
    GameAnimSet_EvaluateState(obstacle->anim_set);
    GizObstacle_EvalAveragePosAndRadius(obstacle, 2);
}

void GizObstacles_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system = data;
    GIZOBSTACLE_PROGRESS_PC *progress = progress_ptr;
    i32 i;
    GIZOBSTACLE *obstacle = system->obstacles;
    for (i = 0; i < system->count; ++i, ++obstacle) {
        GizObstacle_ResetOnePrivate(world, obstacle);
        if (progress && i < 128) {
            i32 word = i / 32;
            u32 bit = 1u << (i & 31);
            ((OBSTACLE_PROGRESS_FLAGS_PC *)&obstacle->progress_flags)->visible = (progress->visible[word] & bit) != 0;
            ((OBSTACLE_PROGRESS_FLAGS_PC *)&obstacle->progress_flags)->active = (progress->enabled[word] & bit) != 0;
            ((OBSTACLE_RUNTIME_FLAGS_PC *)&obstacle->runtime_flags)->pickups_spawned =
                (progress->pickups_spawned[word] & bit) != 0;
            ((OBSTACLE_RUNTIME_FLAGS_PC *)&obstacle->runtime_flags)->ai_active = (progress->ai_active[word] & bit) != 0;
            ((OBSTACLE_RUNTIME_FLAGS_PC *)&obstacle->runtime_flags)->blocked = (progress->blocked[word] & bit) != 0;
            ((OBSTACLE_RUNTIME_FLAGS_PC *)&obstacle->runtime_flags)->trigger_latched =
                (progress->trigger_latched[word] & bit) != 0;
            ((OBSTACLE_RUNTIME_FLAGS_PC *)&obstacle->runtime_flags)->destroyed = (progress->destroyed[word] & bit) != 0;
        }
    }
    ngizobstacletriggers = 0;
}

typedef struct gizobstacle_object_data_pc_s {
    i16 flags;                // 0x0
    i16 platform_id;          // 0x2
} GIZOBSTACLE_OBJECT_DATA_PC; // size 0x4

STATIC_ASSERT(offsetof(GIZOBSTACLE, triggering_object) == 0x38);
STATIC_ASSERT(offsetof(GIZOBSTACLE, auto_return_timer) == 0x40);
extern i32 (*GizObstacleExcludeFnPc)(GIZOBSTACLE *, GAMEOBJECT *);

void GizObstacle_Stop(GIZOBSTACLE *obstacle) {
    if (obstacle) {
        GameAnimSet_Stop(obstacle->anim_set);
    }
}

static i32 GizObstacle_PosWithinBox(GIZOBSTACLE *obstacle, NUVEC *position) {
    NUVEC local;
    if (obstacle->mode != 5 && obstacle->mode != 6) {
        return 1;
    }
    NuVecSub(&local, position, &obstacle->secondary_position);
    NuVecRotateY(&local, &local, -(u16)obstacle->trigger_box_yaw);
    if (local.x > obstacle->trigger_box_half_extents.x) {
        return 0;
    }
    if (local.x < -obstacle->trigger_box_half_extents.x) {
        return 0;
    }
    if (local.y > obstacle->trigger_box_half_extents.y) {
        return 0;
    }
    if (local.y < 0.0f) {
        return 0;
    }
    if (local.z > obstacle->trigger_box_half_extents.z) {
        return 0;
    }
    if (local.z < -obstacle->trigger_box_half_extents.z) {
        return 0;
    }
    return 1;
}

static i32 GizObstacle_SatisfyingTerrainChecks(GIZOBSTACLE *obstacle, GAMEOBJECT *object) {
    GAMEANIMOBJ *anim_object;
    if ((i8)obstacle->trigger_mode >= 0) {
        if ((!object->field_281 && !(object->flags_ef4 & 0x20000000)) ||
            object->jump_animation_state != obstacle->trigger_mode) {
            return 0;
        }
    } else if (obstacle->config_flags & 0x10) {
        if (!object->field_281 && !(object->flags_ef4 & 0x20000000)) {
            return 0;
        }
        if (object->supporting_platform_id >= 0 && obstacle->anim_set && (obstacle->runtime_flags & 0x10)) {
            for (anim_object = obstacle->anim_set->objects; anim_object; anim_object = anim_object->next) {
                if (((GIZOBSTACLE_OBJECT_DATA_PC *)anim_object->object_data)->platform_id ==
                    object->supporting_platform_id) {
                    return 1;
                }
            }
        }
        return 0;
    }
    return 1;
}

static void GizObstacleUpdate_Proximity(GIZOBSTACLE *obstacle) {
    i32 state;
    u32 players = 0;
    obstacle->proximity_output = 0;
    if (!(obstacle->runtime_flags & 8)) {
        f32 closest = obstacle->trigger_radius * obstacle->trigger_radius;
        NUVEC *closest_position = NULL;
        GAMEOBJECT *closest_object = NULL;
        f32 distance;
        i32 i;
        GAMEOBJECT *object;
        if (obstacle->mode == 1 || obstacle->mode == 2 || obstacle->mode == 5 || obstacle->mode == 6) {
            if ((obstacle->mode == 1 || obstacle->mode == 5) && GameCam->mode != -1 && obstacle->trigger_mode == 0xff &&
                !(obstacle->config_flags & 0x4435)) {
                distance = NuVecDistSqr(&GameCam->position, &obstacle->secondary_position, NULL);
                if (distance < closest && GizObstacle_PosWithinBox(obstacle, &GameCam->position)) {
                    closest = distance;
                    closest_position = &GameCam->position;
                }
            }
            if (!(obstacle->config_flags & 1)) {
                for (i = 0; i < ngizobstacletriggers; ++i) {
                    distance = NuVecDistSqr(&obstacle->secondary_position, GizObstacleTriggersPc[i], NULL);
                    if (distance < closest && GizObstacle_PosWithinBox(obstacle, GizObstacleTriggersPc[i])) {
                        closest_position = GizObstacleTriggersPc[i];
                        closest = distance;
                    }
                }
            }
        }
        for (i = 0, object = Obj; i < HIGHGAMEOBJECT; ++i, ++object) {
            i8 player;
            if (!(object->flags_1fc_word & 1) || !(object->flags_1fc_word & 0x1000) || object->dead ||
                (LEGOCONTEXT_DOOMED != -1 && object->character_context == LEGOCONTEXT_DOOMED) ||
                !(object->flags_1044 & 1) || (object->flags_e20 & 0x20)) {
                continue;
            }
            player = (i8)object->field_280;
            if (player != -1) {
                players |= 1 << player;
            }
            if ((obstacle->config_flags & 4) && player == -1) {
                continue;
            }
            if (((obstacle->config_flags & 0x20) && !(object->flags_1fc_word & 0x80)) ||
                (U64_PAIR(&CInfo[object->character_context].flags) & 0x8000) ||
                ((obstacle->config_flags & 0x400) && !object->rider) ||
                ((obstacle->config_flags & 0x4000) && !(object->character_data->model_flags & 4)) ||
                ((obstacle->config_flags & 8) && !(object->character_data->model_flags & 0x2000)) ||
                (GizObstacleExcludeFnPc && GizObstacleExcludeFnPc(obstacle, object))) {
                continue;
            }
            if ((obstacle->mode == 2 || obstacle->mode == 6) && !object->field_281 &&
                !(object->flags_ef4 & 0x20000000) && object->jump_variant_state != 1) {
                GAMEANIMOBJ *anim_object = obstacle->anim_set ? obstacle->anim_set->objects : NULL;
                while (anim_object) {
                    i16 platform = ((GIZOBSTACLE_OBJECT_DATA_PC *)anim_object->object_data)->platform_id;
                    if (platform != -1 && object->supporting_platform_id == platform) {
                        break;
                    }
                    anim_object = anim_object->next;
                }
                if (!anim_object) {
                    continue;
                }
            }
            distance = NuVecDistSqr(&obstacle->secondary_position, &object->lower_position, NULL);
            if (distance < closest) {
                if (GizObstacle_PosWithinBox(obstacle, &object->collision_position) &&
                    GizObstacle_SatisfyingTerrainChecks(obstacle, object)) {
                    closest = distance;
                    closest_position = &object->lower_position;
                    closest_object = object;
                    if ((i8)object->field_280 != -1) {
                        players &= ~(1 << (i8)object->field_280);
                    }
                }
            } else if ((obstacle->config_flags & 0x2000) && (i8)object->field_280 != -1 &&
                       distance < obstacle->trigger_radius * obstacle->trigger_radius &&
                       GizObstacle_PosWithinBox(obstacle, &object->collision_position) &&
                       GizObstacle_SatisfyingTerrainChecks(obstacle, object)) {
                players &= ~(1 << (i8)object->field_280);
            }
        }
        if (obstacle->config_flags & 1) {
            if (!closest_position) {
                obstacle->triggering_object = closest_object;
                obstacle->proximity_output = 1;
            }
        } else if (closest_position && (!(obstacle->config_flags & 0x2000) || !players)) {
            obstacle->triggering_object = closest_object;
            obstacle->proximity_output = 1;
        }
    }
    state = obstacle->anim_set->state;
    if (state != 0 && state != 2 &&
        !(WORLD->current_level == ObstacleReturnLevelPc && obstacle == LevGizObst[0] && state == 4)) {
        return;
    }
    if (state == 2 && !obstacle->playback_state) {
        return;
    }
    if (!state) {
        if (obstacle->proximity_output || (obstacle->runtime_flags & 0x44)) {
            GizObstacle_PlayForwards(obstacle);
            obstacle->auto_return_timer = obstacle->auto_return_delay;
        }
    } else if (!obstacle->proximity_output && !(obstacle->runtime_flags & 0x44)) {
        if (obstacle->auto_return_timer > 0.0f) {
            if (state == 2) {
                obstacle->auto_return_timer -= FRAMETIME;
            }
        } else {
            GizObstacle_PlayBackwards(obstacle);
        }
    }
}

static void GizObstacleUpdate_NoTrigger(GIZOBSTACLE *obstacle) {
    if (!(obstacle->config_flags & 0x800) && !(obstacle->runtime_flags & 4) && !obstacle->anim_set->state) {
        return;
    }
    obstacle->runtime_flags |= 8;
    GizObstacleUpdate_Proximity(obstacle);
}

static void GizObstacleUpdate_TechnoOnly(GIZOBSTACLE *obstacle) {
    (void)obstacle;
}

static void GizObstacleUpdate_PushOnly(GIZOBSTACLE *obstacle) {
    (void)obstacle;
}

static void GizObstacleUpdate_AutoStart(GIZOBSTACLE *obstacle) {
    if (obstacle->config_flags & 0x800) {
        obstacle->runtime_flags |= 8;
        GizObstacleUpdate_Proximity(obstacle);
    } else if (!obstacle->anim_set->state || obstacle->anim_set->state == 3) {
        GizObstacle_PlayForwards(obstacle);
    }
}

static void (*GizObstacleUpdateFnsPc[8])(GIZOBSTACLE *) = {
    GizObstacleUpdate_AutoStart,  GizObstacleUpdate_Proximity, GizObstacleUpdate_Proximity, GizObstacleUpdate_NoTrigger,
    GizObstacleUpdate_TechnoOnly, GizObstacleUpdate_Proximity, GizObstacleUpdate_Proximity, GizObstacleUpdate_PushOnly};

void GizObstacles_Update(void *world_ptr, void *data, f32 time) {
    WORLDINFO *world = world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system = data;
    GIZMOSYS_PREFIX_PC *gizmo_sys;
    GIZMOSET_PC *set;
    GIZMO_PC *gizmo;
    i32 i;
    (void)time;
    if (!world || !system) {
        return;
    }
    gizmo_sys = world->gizmo_sys;
    if (!gizmo_sys || !gizmo_sys->sets) {
        return;
    }
    system->active_gizmo_count = 0;
    set = &gizmo_sys->sets[obstacle_gizmotype_id];
    for (gizmo = set->gizmos, i = 0; i < set->count; ++i, ++gizmo) {
        GIZOBSTACLE *obstacle = gizmo->object;
        GAMEOBJECT *previous = obstacle->triggering_object;
        obstacle->triggering_object = NULL;
        if (!(obstacle->progress_flags & 2) || !(obstacle->progress_flags & 5) || !obstacle->anim_set ||
            (obstacle->progress_flags & 0x100) || (obstacle->runtime_flags & 0x80)) {
            continue;
        }
        if (obstacle->mode == 7) {
            if (obstacle->progress_flags & 8) {
                obstacle->triggering_object = previous;
            } else if (obstacle->playback_state == 1) {
                obstacle->animation_speed = -1.0f;
            } else {
                obstacle->animation_speed = 0.0f;
            }
            if (obstacle->animation_speed != 0.0f) {
                GizObstacle_PlayForwards(obstacle);
            } else {
                GizObstacle_Stop(obstacle);
            }
        } else if ((obstacle->progress_flags & 4) && obstacle->animation_speed != 0.0f) {
            GizObstacle_PlayForwards(obstacle);
        } else if ((obstacle->progress_flags & 4) && obstacle->animation_speed == 0.0f) {
            GizObstacle_Stop(obstacle);
        } else if (GizObstacleUpdateFnsPc[obstacle->mode]) {
            GizObstacleUpdateFnsPc[obstacle->mode](obstacle);
        }
        // MATCH-EXCEPTION: retail tests the three low GAMEANIMSET flag bits one by one (word load, test al, 7);
        // a single `flags & 7` compiles to a byte test.
        if ((obstacle->anim_set->flags & 1) || (obstacle->anim_set->flags & 2) || (obstacle->anim_set->flags & 4)) {
            GizObstacle_EvalAveragePosAndRadius(obstacle, 2);
            if (obstacle->runtime_flags & 0x20) {
                GAMEANIMOBJ *object;
                for (object = obstacle->anim_set->objects; object; object = object->next) {
                    GIZOBSTACLE_OBJECT_DATA_PC *object_data = object->object_data;
                    if (object_data->flags & 2) {
                        AddShoveObject(&object->special, object_data->platform_id);
                    }
                }
            }
            if (obstacle->animation_speed < 0.0f) {
                if (obstacle->stop_sfx_id != -1 &&
                    (obstacle->anim_set->state == 2 || IsSfxLooping(obstacle->stop_sfx_id))) {
                    GameAudio_PlaySfxById(obstacle->stop_sfx_id, &obstacle->evaluated_position, 0, 0);
                }
            } else if (obstacle->start_sfx_id != -1 &&
                       (!obstacle->anim_set->state || IsSfxLooping(obstacle->start_sfx_id))) {
                GameAudio_PlaySfxById(obstacle->start_sfx_id, &obstacle->evaluated_position, 0, 0);
            }
        }
        if (obstacle->anim_set->state == 2) {
            obstacle->runtime_flags &= ~0x40u;
        }
        if ((!obstacle->playback_state || obstacle->playback_state == 3 || (obstacle->config_flags & 0x80)) &&
            ((obstacle->anim_set->flags & 2) || !obstacle->anim_set->animated_object_count) &&
            obstacle->anim_set->state == 2) {
            if (obstacle->blowup_type != -1 || obstacle->pickup_count) {
                if (obstacle->blowup_type != -1) {
                    if (obstacle->config_flags & 0x200) {
                        GAMEANIMOBJ *object;
                        for (object = obstacle->anim_set ? obstacle->anim_set->objects : NULL; object;
                             object = object->next) {
                            NUVEC *position = NuSpecialGetDrawPos(&object->special);
                            if (position) {
                                GizmoBlowUpTypeBlowUp(world, obstacle->blowup_type, position);
                            }
                        }
                    } else {
                        GizmoBlowUpTypeBlowUp(world, obstacle->blowup_type, &obstacle->evaluated_position);
                    }
                    GameAnimSet_SetVisibility(obstacle->anim_set, 0);
                    obstacle->runtime_flags |= 0x80;
                }
                if (obstacle->pickup_count && (!(obstacle->runtime_flags & 1) || (obstacle->config_flags & 0x80))) {
                    NUVEC position;
                    NUVEC direction;
                    NuVecAdd(&position, &obstacle->evaluated_position, &obstacle->pickup_offset);
                    NuVecRotateX(&direction, &v010, (u16)obstacle->pickup_direction_x);
                    NuVecRotateY(&direction, &direction, (u16)obstacle->pickup_direction_y);
                    AddPickups((u16)obstacle->pickup_count, 0, 0, 0, &position, &direction, 2.0f, -1,
                               obstacle->pickup_scatter_height, 2000000.0f, NULL, 1, 0);
                    obstacle->runtime_flags |= 1;
                }
            }
            if (obstacle->playback_state == 3) {
                GameAnimSet_JumpToStart(obstacle->anim_set);
                GizObstacle_EvalAveragePosAndRadius(obstacle, 2);
                GameAnimSet_SetVisibility(obstacle->anim_set, 1);
                obstacle->runtime_flags &= ~0x80u;
            }
        }
        if (!(obstacle->runtime_flags & 0x80) && (obstacle->config_flags & 0x1800)) {
            system->active_gizmos[system->active_gizmo_count++] = gizmo;
        }
        obstacle->animation_speed = 1.0f;
        obstacle->progress_flags &= ~0xcu;
    }
    ngizobstacletriggers = 0;
}

void GizObstacles_AddTrigger(NUVEC *position) {
    NUVEC message_position;
    if (!position) {
        return;
    }
    if (ngizobstacletriggers < 16) {
        GizObstacleTriggersPc[ngizobstacletriggers++] = position;
    } else {
        message_position = v001;
        AddGameMessage("MAXGIZOBSTACLETRIGGERS needs increasing, ask a programmer!", &message_position, 0.4f, NULL,
                       0.4f, 200, 200, 200, 0x2080, 0.0f);
    }
}

void GizObstacle_SetTechnoControlled(GIZOBSTACLE *obstacle, f32 speed) {
    obstacle->progress_flags |= 4;
    obstacle->animation_speed = speed;
}

void GizObstacle_SetPushControlled(GIZOBSTACLE *obstacle, GAMEOBJECT *object, f32 speed) {
    if (!obstacle->playback_state && obstacle->anim_set->state == 2 && !(obstacle->config_flags & 0x100)) {
        return;
    }
    obstacle->progress_flags |= 8;
    obstacle->animation_speed = speed;
    obstacle->triggering_object = object;
}

GIZOBSTACLE *GizObstacle_FindNearest(void *ptr, NUVEC *position, GAMEOBJECT *object, f32 *distance_squared, i32 mode) {
    GIZOBSTACLE_SYSTEM_PC *system = ptr;
    GIZOBSTACLE *nearest;
    GIZOBSTACLE *obstacle;
    f32 closest;
    i32 i;
    if (system) {
        nearest = NULL;
        closest = 1.0e9f;
        for (i = 0, obstacle = system->obstacles; i < system->count; ++i, ++obstacle) {
            f32 distance;
            if ((mode != -1 && obstacle->mode != mode) || !(obstacle->progress_flags & 2) ||
                !(obstacle->progress_flags & 1) || (obstacle->runtime_flags & 0x80)) {
                continue;
            }
            if (object && obstacle->anim_set) {
                NUVEC average;
                GameAnimSet_GetAveragePos(obstacle->anim_set, &average, 2, 1, 0);
                distance = NuVecDistSqr(position, &average, NULL);
            } else {
                distance = NuVecDistSqr(position, &obstacle->position, NULL);
            }
            if (distance < closest) {
                closest = distance;
                nearest = obstacle;
            }
        }
        if (distance_squared) {
            *distance_squared = closest;
        }
        return nearest;
    }
    return NULL;
}

i32 GizObstacles_Hit(void *world, GIZOBSTACLE *obstacle, NUVEC *position, i32 player, i32 damage) {
    (void)position;
    (void)damage;
    if (!(obstacle->progress_flags & 2) || !(obstacle->progress_flags & 1) || (obstacle->runtime_flags & 0x80)) {
        return 0;
    }
    if (obstacle->config_flags & 0x800) {
        obstacle->runtime_flags |= 0x40;
        if (player != -1 && (((GAMEOBJECT *)Player[player])->flags_1fc & 0x80)) {
            NewBuzz(((GAMEOBJECT *)Player[player])->pad->bound_pad, 0.1f, 0);
        }
        return 1;
    }
    if (!(obstacle->config_flags & 0x1000)) {
        return 0;
    }
    if (obstacle->blowup_type != -1) {
        if (obstacle->config_flags & 0x200) {
            GAMEANIMOBJ *object;
            for (object = obstacle->anim_set ? obstacle->anim_set->objects : NULL; object; object = object->next) {
                NUVEC *draw_position = NuSpecialGetDrawPos(&object->special);
                if (draw_position) {
                    GizmoBlowUpTypeBlowUp(world, obstacle->blowup_type, draw_position);
                }
            }
        } else {
            GizmoBlowUpTypeBlowUp(world, obstacle->blowup_type, &obstacle->evaluated_position);
        }
    }
    GameAnimSet_JumpToEnd(obstacle->anim_set);
    GizObstacle_EvalAveragePosAndRadius(obstacle, 2);
    GameAnimSet_SetVisibility(obstacle->anim_set, 0);
    obstacle->runtime_flags |= 0x80;
    if (player != -1 && (((GAMEOBJECT *)Player[player])->flags_1fc & 0x80)) {
        NewBuzz(((GAMEOBJECT *)Player[player])->pad->bound_pad, 0.1f, 0);
    }
    return 1;
}

GIZMO_PC *GizObstacles_GetBestBoltTarget(GIZMOSET_PC *set, f32 *result_distance, NUVEC *result_position,
                                         NUVEC *result_velocity, void *object_ptr, NUVEC *position, NUVEC *direction,
                                         f32 radius, f32 range_squared, i32 directional, i32 planar, i32 bolt_id) {
    BOLTTYPE_PC *type = BoltType_FindByID(bolt_id, WORLD);
    u16 near_angle = TargetDeg_Near;
    u16 mid_angle = TargetDeg_Mid;
    u16 far_angle = TargetDeg_Far;
    f32 near_distance = TargetDist_Near2;
    f32 mid_distance = TargetDist_Mid2;
    GIZOBSTACLE_SYSTEM_PC *system;
    NUVEC aim;
    NUVEC min;
    NUVEC max;
    f32 nearest_distance;
    GAMEOBJECT *object;
    NUVEC *best_position;
    NUVEC *previous_position;
    GIZMO_PC *best;
    GIZMO_PC *previous;
    i32 i;
    if (set && set->data && type) {
        nearest_distance = 1.0e9f;
        best_position = NULL;
        previous_position = NULL;
        best = NULL;
        previous = NULL;
        aim = *direction;
        min.x = position->x - radius;
        min.z = position->z - radius;
        max.x = position->x + radius;
        max.z = position->z + radius;
        if (directional && (type->flags_60 & 0x20000)) {
            aim.y = 0.0f;
            NuVecNorm(&aim, &aim);
        }
        system = set->data;
        object = object_ptr;
        for (i = 0; i < system->active_gizmo_count; ++i) {
            GIZMO_PC *gizmo = system->active_gizmos[i];
            GIZOBSTACLE *obstacle = gizmo->object;
            NUVEC *target;
            NUVEC delta;
            f32 distance;
            f32 dot;
            u16 angle;
            if (!(obstacle->progress_flags & 2) || !(obstacle->progress_flags & 1) ||
                (obstacle->runtime_flags & 0x80)) {
                continue;
            }
            target = &obstacle->evaluated_position;
            if (obstacle->evaluated_position.x < min.x || obstacle->evaluated_position.x > max.x ||
                obstacle->evaluated_position.z < min.z || obstacle->evaluated_position.z > max.z) {
                continue;
            }
            distance = !planar ? NuVecDistSqr(target, position, &delta) : NuVecXZDistSqr(target, position, &delta);
            if (!(distance < range_squared)) {
                continue;
            }
            if (!directional) {
                NuVecRotateY(&aim, &v001,
                             NuAtan2D(obstacle->evaluated_position.x - position->x,
                                      obstacle->evaluated_position.z - position->z));
            }
            if (type->flags_60 & 0x20000) {
                delta.y = 0.0f;
            }
            NuVecNorm(&delta, &delta);
            dot = NuVecDot(&delta, &aim);
            if (distance < near_distance && directional) {
                angle = near_angle;
            } else if (mid_distance > distance) {
                angle = mid_angle;
            } else {
                angle = far_angle;
            }
            if (!(dot > NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff])) {
                continue;
            }
            if (nearest_distance > distance) {
                if (object && (void *)gizmo == object->collision_target) {
                    previous = gizmo;
                    previous_position = target;
                } else {
                    nearest_distance = distance;
                    best = gizmo;
                    best_position = target;
                }
            }
        }
        if (best) {
            *result_position = *best_position;
            *result_distance = nearest_distance;
            result_velocity->x = 0.0f;
            result_velocity->y = 0.0f;
            result_velocity->z = 0.0f;
            return best;
        }
        if (previous) {
            *result_position = *previous_position;
            *result_distance = 1.0e9f;
            result_velocity->x = 0.0f;
            result_velocity->y = 0.0f;
            result_velocity->z = 0.0f;
            return previous;
        }
    }
    return NULL;
}

void GizObstacles_Draw(void *world_ptr, void *data, f32 time) {
    WORLDINFO *world = world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system = data;
    GIZOBSTACLE *obstacle;
    i32 i;
    (void)time;
    if (!system) {
        return;
    }
    for (i = 0, obstacle = system->obstacles; i < system->count; ++i, ++obstacle) {
        if ((obstacle->progress_flags & 2) && (obstacle->room_id < 0 || world->rooms_visible_ptr[obstacle->room_id]) &&
            !(obstacle->runtime_flags & 0x80) && (obstacle->config_flags & 0x40)) {
            GameAnimSet_DrawReflection(obstacle->anim_set, 2, obstacle->reflection_alpha, NULL);
        }
    }
}

STATIC_ASSERT(offsetof(BOLT_PC, owner) == 4);
STATIC_ASSERT(offsetof(BOLT_PC, position) == 0x88);
STATIC_ASSERT(offsetof(BOLT_PC, direction) == 0xac);
STATIC_ASSERT(offsetof(BOLT_PC, hit_platform) == 0xfa);
STATIC_ASSERT(offsetof(BOLT_PC, type_id) == 0x101);

static i32 GizObstacles_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    return world ? world->current_level->max_obstacles : 0;
}

static void GizObstacles_AddGizmos(void *gizmo_sys, i32 type_id, void *world, void *data) {
    GIZOBSTACLE_SYSTEM_PC *system = data;
    i32 i;
    (void)world;
    if (system) {
        for (i = 0; i < system->count; ++i) {
            if (NuStrLen(system->obstacles[i].name)) {
                AddGizmo(gizmo_sys, type_id, NULL, &system->obstacles[i]);
            }
        }
    }
}

static char *GizmoObstacle_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    (void)gizmo;
    switch (output) {
        case 0:
            return "AtEnd";
        case 1:
            return "NotAtStart";
        case 2:
            return "Proximity";
        case 3:
            return "AtStart";
        case 4:
            return "PlayingForward";
        default:
            return NULL;
    }
}

static i32 GizObstacles_BoltHitPlat(void *world, void *data, BOLT_PC *bolt, u8 *hit_flags) {
    GIZOBSTACLE_SYSTEM_PC *system = data;
    i32 i;
    i32 count;
    (void)hit_flags;
    if (system) {
        count = system->active_gizmo_count;
        for (i = 0; i < count; ++i) {
            GIZOBSTACLE *obstacle = system->active_gizmos[i]->object;
            GAMEANIMOBJ *object;
            if (!(obstacle->progress_flags & 2) || !(obstacle->progress_flags & 1) ||
                !(obstacle->runtime_flags & 0x10) || (obstacle->runtime_flags & 0x80)) {
                continue;
            }
            for (object = obstacle->anim_set->objects; object; object = object->next) {
                typedef struct obstacle_anim_object_data_pc_s {
                    u16 flags;
                    i16 platform;
                } OBSTACLE_ANIM_OBJECT_DATA_PC;

                OBSTACLE_ANIM_OBJECT_DATA_PC *object_data = object->object_data;
                STATIC_ASSERT(sizeof(OBSTACLE_ANIM_OBJECT_DATA_PC) == 4);
                STATIC_ASSERT(offsetof(OBSTACLE_ANIM_OBJECT_DATA_PC, platform) == 2);
                if (object_data->platform == bolt->hit_platform) {
                    BOLTTYPE_PC *type = BoltType_FindByID(bolt->type_id, world);
                    GAMEOBJECT *owner = bolt->owner;
                    i32 player = owner ? (i8)owner->field_280 : -1;
                    GizObstacles_Hit(world, obstacle, &bolt->position, player, type->field_3c);
                    return 1;
                }
            }
        }
    }
    return 0;
}

static i32 GizObstacles_BoltHit(void *world, void *data, void *attacker_ptr, NUVEC *points, i32 point_count, f32 radius,
                                NUVEC *minimum, NUVEC *maximum, BOLT_PC *bolt, u32 hit_type, u8 *hit_flags) {
    GIZOBSTACLE_SYSTEM_PC *system = data;
    GAMEOBJECT *attacker = attacker_ptr;
    GIZOBSTACLE *nearest = NULL;
    f32 nearest_distance = 1000000000.0f;
    i32 i;
    if (system) {
        for (i = 0; i < system->active_gizmo_count; ++i) {
            GIZOBSTACLE *obstacle = system->active_gizmos[i]->object;
            NUVEC *position;
            i32 point_index;
            f32 obstacle_radius;
            if (!(obstacle->progress_flags & 2) || !(obstacle->progress_flags & 1) ||
                (obstacle->runtime_flags & 0x80)) {
                continue;
            }
            // MATCH-EXCEPTION: the radius local feeds only the first bound; retail then loads the position before
            // the radius in the z and y "+" bounds, which no other form of this test reproduces.
            obstacle_radius = obstacle->evaluated_radius;
            if (maximum->x < obstacle->evaluated_position.x - obstacle_radius ||
                minimum->x > obstacle->evaluated_position.x + obstacle->evaluated_radius ||
                maximum->z < obstacle->evaluated_position.z - obstacle->evaluated_radius ||
                minimum->z > obstacle->evaluated_position.z + obstacle->evaluated_radius ||
                maximum->y < obstacle->evaluated_position.y - obstacle->evaluated_radius ||
                minimum->y > obstacle->evaluated_position.y + obstacle->evaluated_radius) {
                continue;
            }
            position = &obstacle->evaluated_position;
            for (point_index = point_count - 1; point_index >= 0; --point_index) {
                f32 distance;
                if (!SphereSphereOverlap(position, obstacle->evaluated_radius, &points[point_index], radius)) {
                    continue;
                }
                distance = NuVecDistSqr(attacker ? &attacker->collision_position : &points[point_index], position, 0);
                if (distance < nearest_distance) {
                    nearest_distance = distance;
                    nearest = obstacle;
                }
                break;
            }
        }
        if (nearest) {
            BOLTTYPE_PC *type;
            GAMEOBJECT *owner;
            i32 player;
            if (hit_type != 2 && hit_type != 7 && bolt) {
                BoltType_FindByID(bolt->type_id, WORLD);
                Cheats_CheckFlags(2);
            }
            type = BoltType_FindByID(bolt->type_id, world);
            owner = bolt->owner;
            player = owner ? (i8)owner->field_280 : -1;
            if (GizObstacles_Hit(world, nearest, &bolt->position, player, type->field_3c)) {
                if (attacker) {
                    NewRumble(attacker->pad->bound_pad, 0.4f, 0);
                    GameCam_HitJudder();
                }
            } else {
                NUVEC normal;
                NuVecSub(&normal, &nearest->evaluated_position, &bolt->position);
                NuVecNorm(&normal, &normal);
                Bolt_AddDeflectedBolt(bolt, &bolt->direction, &normal, hit_flags);
            }
            if (BoltSys->stop_targeting) {
                BoltSys->stop_targeting(attacker, points);
            }
            return 1;
        }
    }
    return 0;
}

ADDGIZMOTYPE_PC *GizObstacles_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC GizObstaclesAddTypePc = {0};
    ADDGIZMOTYPE_PC *addtype = &GizObstaclesAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizObstacleRegistrationNamePtrPc;
    addtype->prefix = "";
    addtype->fns.options = 0x70;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.get_max_gizmos_fn = GizObstacles_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = GizObstacles_AddGizmos;
    addtype->fns.late_update_fn = GizObstacles_Update;
    addtype->fns.draw_fn = GizObstacles_Draw;
    addtype->fns.get_gizmo_name_fn = GizmoObstacle_GetGizmoName;
    addtype->fns.get_output_fn = GizmoObstacle_GetOutput;
    addtype->fns.get_output_name_fn = GizmoObstacle_GetOutputName;
    addtype->fns.get_num_outputs_fn = GizmoObstacle_GetNumOutputs;
    addtype->fns.activate_fn = GizmoObstacle_Activate;
    addtype->fns.activate_rev_fn = GizmoObstacle_ActivateRev;
    addtype->fns.set_visibility_fn = GizmoObstacle_SetVisibility;
    addtype->fns.get_pos_fn = GizmoObstacle_GetPos;
    addtype->fns.bolt_hit_plat_fn = GizObstacles_BoltHitPlat;
    addtype->fns.get_best_bolt_target_fn = GizObstacles_GetBestBoltTarget;
    addtype->fns.bolt_hit_fn = GizObstacles_BoltHit;
    addtype->fns.allocate_progress_data_fn = GizObstacles_AllocateProgressData;
    addtype->fns.clear_progress_fn = GizObstacles_ClearProgress;
    addtype->fns.store_progress_fn = GizObstacles_StoreProgress;
    addtype->fns.reset_fn = GizObstacles_Reset;
    addtype->fns.reserve_buffer_space_fn = GizObstacles_ReserveBufferSpace;
    addtype->fns.load_fn = GizObstacles_Load;
    addtype->fns.post_load_fn = GizObstacles_PostLoad;
    addtype->fns.add_level_sfx_fn = GizObstacles_AddLevelSfx;
    obstacle_gizmotype_id = type_id;
    return addtype;
}

typedef struct bomb_generator_runtime_pc_s {
    char name[16];              // 0x00
    NUVEC position;             // 0x10
    GAMEANIMSET *anim_set;      // 0x1c
    i32 interval;               // 0x20
    GAMEOBJECT *generated_bomb; // 0x24
    u16 unknown_28;             // 0x28

    union {        // 0x2a
        u16 flags; // 0x2a

        struct {                    // 0x2a
            u16 active : 1;         // 0x2a:0
            u16 visible : 1;        // 0x2a:1
            u16 unknown_flags : 14; // 0x2a:2
        };
    };
} BOMB_GENERATOR_RUNTIME_PC; // size 0x2c

typedef struct bomb_generator_system_runtime_pc_s {
    BOMB_GENERATOR_RUNTIME_PC *items; // 0x00
    u16 count;                        // 0x04
    u16 capacity;                     // 0x06
    void *pool;                       // 0x08
    i16 flags;                        // 0x0c
    u16 unknown_0e;                   // 0x0e
    void *progress;                   // 0x10
} BOMB_GENERATOR_SYSTEM_RUNTIME_PC;   // size 0x14

STATIC_ASSERT(offsetof(BOMB_GENERATOR_SYSTEM_RUNTIME_PC, flags) == 0xc &&
              sizeof(((BOMB_GENERATOR_SYSTEM_RUNTIME_PC *)0)->flags) == 2);
STATIC_ASSERT(offsetof(GAMEOBJECT, cable) == 0x1040);
STATIC_ASSERT(offsetof(CABLE_s, target) == 4);
STATIC_ASSERT(offsetof(CABLE_s, max_length) == 0x1c4);
STATIC_ASSERT(offsetof(WORLDINFO, game_debris_sys) == 0x134);

STATIC_ASSERT(offsetof(GIZBOMBGEN_s, flags) == 0x2a);

/* Studs the level's obstacles release (TotalLevelCoinTally). */
u32 GizObstacles_TotalScore(WORLDINFO *world) {
    GIZOBSTACLESYS_s *system;
    GIZOBSTACLE *obstacle;
    u32 total = 0;
    i32 i;
    system = world->giz_obstacle_sys;
    if (system != NULL) {
        obstacle = system->obstacles;
        if (obstacle != NULL) {
            for (i = 0; i < system->count; i++, obstacle++) {
                total += (u16)obstacle->pickup_count;
            }
        }
    }
    return total;
}
