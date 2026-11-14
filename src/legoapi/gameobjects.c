#include "saga.h"
#include <string.h>

STATIC_ASSERT(offsetof(PART_PC, field_206) == 0x206);

typedef struct grapple_rotation_pc_s {
    u8 unknown_00[0x30]; // 0x00
    NUVEC hook_position; // 0x30
} GRAPPLE_ROTATION_PC;   // size 0x3c

extern const f64 GameObjectNearFloorNoFloorHeight;

STATIC_ASSERT(offsetof(GAMEOBJECT, locator_matrices) + 2 * sizeof(NUMTX) == 0x884);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, points_of_interest) + 2 * sizeof(void *) == 0x1c);
STATIC_ASSERT(offsetof(AREA_GLOBALS_PC, field_00) == 0);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, pickup_positions) == 0x3c);
STATIC_ASSERT(offsetof(GAMEDEBRISENTRY_PC, effect) == 0);

typedef struct kill_zipup_pc_s {
    u8 unknown_00[0x68]; // 0x00
    u16 runtime_flags;   // 0x68
} KILL_ZIPUP_PC;         // size 0x6a

/* A copy of the 33-DWORD script cohort (script processor plus takeover link), without extending the
   packet. Held as words so the copy is one block move and the local is not treated as a /GS buffer. */
typedef union TAGSCRIPTSTATE_PC {
    u32 words[0x33]; // 0x00

    struct {                  // 0x00
        u32 unknown000[0x2c]; // 0x00
        u8 creature_set;      // 0xb0
    } fields;
} TAGSCRIPTSTATE_PC; // size 0xcc

STATIC_ASSERT(offsetof(AIPACKET_PC, takeover_script_link) - offsetof(AIPACKET_PC, script_processor) == 0xc8);

/* Bitfield view of flags_1fc: bit 7 marks a player-controlled object, bit 8 an AI-driven one. */
typedef struct TAGFLAGS1FC_PC {
    u32 bits_0_6 : 7;
    u32 player : 1;
    u32 ai : 1;
    u32 bits_9_31 : 23;
} TAGFLAGS1FC_PC;

STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(GAMEOBJECT, floor_height) == 0x21c);
STATIC_ASSERT(offsetof(GAMEOBJECT, water_height) == 0x220);
STATIC_ASSERT(offsetof(GAMEOBJECT, roof_height_pc) == 0x224);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_283) == 0x283);
STATIC_ASSERT(offsetof(GAMEOBJECT, roof_surface_id_pc) == 0x284);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_animation_state) == 0x285);
STATIC_ASSERT(offsetof(GAMEOBJECT, raw_surface_id_pc) == 0xe42);
STATIC_ASSERT(offsetof(GAMEOBJECT, surface_normal) == 0xf24);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_height) == 0x1014);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_105c) == 0x105c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_105e) == 0x105e);
STATIC_ASSERT(offsetof(GAMEOBJECT, terrain_platform_id) == 0x106c);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
STATIC_ASSERT(sizeof(TERRAIN_SURFACE_PC) == 12);
STATIC_ASSERT(offsetof(TERRAIN_SURFACE_PC, flags) == 4);

/* Shadow platform entries. Tail[1] holds the counted traversal, not an allocation count.
 */
typedef struct shadow_platform_entry_pc_s {
    GAMEOBJECT *object;     // 0x00
    u8 unknown_04_14[0x10]; // 0x04
} SHADOW_PLATFORM_ENTRY_PC; // size 0x14

struct shadow_platform_list_pc_s {
    u8 unknown_00_04[4];                 // 0x00
    i32 count;                           // 0x04
    u8 unknown_08_18[0x10];              // 0x08
    SHADOW_PLATFORM_ENTRY_PC entries[1]; // 0x18
}; // size 0x2c

typedef struct gameobject_collision_reset_pc_s {
    u8 unknown_000[0x2b0];       // 0x000
    u32 collision_overlap_low;   // 0x2b0
    u32 collision_overlap_high;  // 0x2b4
} GAMEOBJECT_COLLISION_RESET_PC; // size 0x2b8

typedef struct GAME_ANTI_NODE_DATA_PC {
    NUVEC position;       // 0x00
    f32 radius;           // 0x0c
    f32 min_y;            // 0x10
    f32 max_y;            // 0x14
    f32 extent_x;         // 0x18
    f32 extent_z;         // 0x1c
    u16 angle;            // 0x20
    u8 shape;             // 0x22
    u8 mode;              // 0x23
} GAME_ANTI_NODE_DATA_PC; // size 0x24

STATIC_ASSERT(offsetof(GAMEOBJECT, collision_link) == 0x298);
STATIC_ASSERT(offsetof(GAMEOBJECT, lighting_packet) == 0x4d8);
STATIC_ASSERT(offsetof(GAMEPAD_PC, rumble) == 0x40);

void ClearGameObjects(APIOBJECTSYS_PC *system) {
    memset(Obj, 0, sizeof(GAMEOBJECT) * 64);
    APIObjectDestroyAll(system);
    HIGHGAMEOBJECT = 0;
}

static void RecomputeHighGameObject(void) {
    i32 index;
    HIGHGAMEOBJECT = 0;
    for (index = 0; index < 64; ++index) {
        if ((Obj[index].flags_1fc_word & 1) != 0) {
            HIGHGAMEOBJECT = index + 1;
        }
    }
}

GAMEOBJECT *AddGameObject(void) {
    GAMEOBJECT *object = APIObjectCreate(WORLD->object_sys);
    u64 identity;
    if (object != NULL) {
        object->flags_1fc_word |= 0x1001;
        object->origin_scale = 1.0f;
        object->collision_scale = 1.0f;
        object->reflection_height = 2000000.0f;
        object->sock_index = 0xff;
        object->movement_mode = 2;
        identity = (u64)1 << object->hostility_group;
        object->collision_identity_mask[0] = (u32)identity;
        object->roof_height_pc = 2000000.0f;
        object->collision_identity_mask[1] = (u32)(identity >> 32);
        object->water_height = 2000000.0f;
        object->layer_mask = 1;
        object->floor_height = 2000000.0f;
        object->roof_surface_id_pc = 0xff;
        object->field_283 = -1;
        object->jump_animation_state = 0xff;
        RecomputeHighGameObject();
        ((APIOBJECT_PC *)object)->owner = object;
        object->ai_packet.owner = object;
        ((APIOBJECT_PC *)object)->ai = &object->ai_packet;
        ((GAMEOBJECT_COLLISION_RESET_PC *)object)->collision_overlap_low = 0;
        ((GAMEOBJECT_COLLISION_RESET_PC *)object)->collision_overlap_high = 0;
        return object;
    }
    return NULL;
}

void RemoveGameObject(GAMEOBJECT *object, i32 unused) {
    GAMEOBJECT *other;
    i32 i;
    (void)unused;
    if (object == NULL) {
        return;
    }
    other = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++other) {
        // Shifts through __allshl, which yields 0 for groups >= 64.
        U64_PAIR(other->collision_contact_mask) &= ~U64_PAIR(object->collision_identity_mask);
        U64_PAIR(other->hostility_mask) &= ~((u64)1 << object->hostility_group);
        U64_PAIR(&other->field_eb0) &= ~((u64)1 << object->hostility_group);
        U64_PAIR(other->ai_opponent_exclusion_mask) &= ~((u64)1 << object->hostility_group);
    }
    if (object->pad != NULL) {
        object->pad->runtime_flags &= ~1u;
    }
    DestroySnakeBody(object);
    APIObjectDestroy(WORLD->object_sys, object);
    RecomputeHighGameObject();
    for (i = 0; i < 8; ++i) {
        if (LevGameObject[i] == object) {
            LevGameObject[i] = NULL;
        }
    }
}

void GameObjectDimensions(GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG_PC *config;
    if (object->use_model_origin > 0 && object->origin_scale_override == 1.0f) {
        config = object->character_data->game_character;
        if (config->collision_origin_joint != -1 &&
            object->character_model->points_of_interest[config->collision_origin_joint] != NULL &&
            object->origin_joint_ready != 0) {
            f32 radius = object->character_data->game_character->collision_origin_radius * object->collision_scale;
            object->bounds_half_height = radius;
            object->bounds_radius = radius;
            goto extra;
        }
    }
    object->bounds_radius = object->collision_radius;
    object->bounds_half_height = object->field_b0;
extra:
    if (GameObjectDimensionsExtraFn != NULL) {
        GameObjectDimensionsExtraFn(object);
    }
}

void ScaleGameObject(GAMEOBJECT *object) {
    CHARACTERDATA_PC *character = object->character_data;
    object->scaled_radius = character->scaled_radius * object->origin_scale;
    object->collision_radius = object->collision_radius_unscaled * object->origin_scale;
    object->field_b0 = object->collision_radius * object->collision_y_scale;
    object->scaled_height = (character->top - character->bottom) * object->collision_scale;
}

void GetTopBot(GAMEOBJECT *object) {
    CHARACTERDATA_PC *character = object->character_data;
    object->character_bottom = character->bottom;
    object->character_top = character->top;
    if (object->collision_radius_unscaled == 0.0f) {
        object->collision_y_scale = 0.0f;
    } else {
        object->collision_y_scale = (f32)((f64)(object->character_top - object->character_bottom) /
                                          ((f64)object->collision_radius_unscaled * 2.0));
    }
}

void SetToLastSafePos(GAMEOBJECT *object) {
    object->start_position = object->last_safe_position;
    object->position = object->last_safe_position;
    object->ai_update_position = object->last_safe_position;
}

i32 UnderPlayerControl(GAMEOBJECT *object) {
    if ((object->flags_1fc_word & 0x80) != 0) {
        return 1;
    }
    if (object->takeover_source != NULL && (object->takeover_source->flags_1fc_word & 0x80) != 0) {
        return 1;
    }
    return 0;
}

GAMEOBJECT *FindGameObject(i32 character_id, u32 required_flags, i32 alive_only, i32 vehicle_only, i32 non_level_only) {
    GAMEOBJECT *object = Obj;
    i32 i;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & 1) == 0) {
            continue;
        }
        if (vehicle_only != 0 && (object->flags_1fc_word & 0x1000) == 0) {
            continue;
        }
        if (required_flags != 0 && (object->flags_1f8 & required_flags) != required_flags) {
            continue;
        }
        if (character_id != -1 && (i16)object->id != character_id) {
            continue;
        }
        if (alive_only != 0 &&
            (object->dead != 0 || (U64_PAIR(&CInfo[object->character_context].flags) & 0x8000) != 0)) {
            continue;
        }
        if (non_level_only != 0 && object->level_object_index != -1) {
            continue;
        }
        return object;
    }
    return NULL;
}

GAMEOBJECT *FindNearestGameObject(NUVEC *position, GAMEOBJECT *exclude, u32 required_flags, f32 radius,
                                  f32 extra_radius, i32 animation, i32 character_id, i32 player_index,
                                  f32 *distance_squared, i32 horizontal_only, i32 (*filter)(GAMEOBJECT *)) {
    f32 radius_squared = radius * radius;
    GAMEOBJECT *nearest = NULL;
    f32 nearest_distance = 100000000.0f;
    GAMEOBJECT *object = Obj;
    i32 i;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        f32 x;
        f32 y;
        f32 z;
        f32 distance;
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0 ||
            (object->flags_e20 & 0x20) || (U64_PAIR(&CInfo[object->character_context].flags) & 0x8000)) {
            continue;
        }
        if (character_id != -1 && (i16)object->id != character_id) {
            continue;
        }
        if (player_index >= 0) {
            if (player_index < 2) {
                if (((i8)object->field_280) != player_index) {
                    continue;
                }
            } else if (player_index == 99) {
                if (((i8)object->field_280) == -1) {
                    continue;
                }
            } else if (player_index == 100) {
                if (((i8)object->field_280) != -1) {
                    continue;
                }
            }
        }
        if (exclude && object == exclude) {
            continue;
        }
        if (required_flags && (object->character_data->model_flags & required_flags) != required_flags) {
            continue;
        }
        if (animation != -1 && object->character_model->model_data_b[animation] == NULL) {
            continue;
        }
        if (filter && !filter(object)) {
            continue;
        }
        x = object->collision_position.x - position->x;
        y = object->collision_position.y - position->y;
        z = object->collision_position.z - position->z;
        distance = horizontal_only ? x * x + z * z : x * x + y * y + z * z;
        if (exclude) {
            if (radius_squared <= 0.0f || distance < radius_squared) {
                f32 combined_radius = exclude->collision_radius + object->collision_radius + extra_radius;
                if (distance < combined_radius * combined_radius && distance < nearest_distance) {
                    nearest_distance = distance;
                    nearest = object;
                }
            }
        } else if ((radius_squared <= 0.0f || distance < radius_squared) && distance < nearest_distance) {
            nearest_distance = distance;
            nearest = object;
        }
    }
    if (nearest && distance_squared) {
        *distance_squared = nearest_distance;
    }
    return nearest;
}

i32 GameObjectNearFloor(GAMEOBJECT *object, f32 height, f32 *distance) {
    f32 tolerance;
    f32 floor_distance;

    if (!(height > 0.0f)) {
        height = 0.0f;
    }
    height = (f32)((f64)height * NuTimeBarSlotWidth);
    if ((f64)object->floor_height != GameObjectNearFloorNoFloorHeight) {
        tolerance = height;
        height = (f32)(((f64)object->collision_radius / 0.22499999403953552) * tolerance);
        if (height < tolerance) {
            height = tolerance;
        }
        floor_distance = object->collision_min.y - object->floor_height;
        if (distance != NULL) {
            *distance = floor_distance;
        }
        return height > floor_distance;
    }
    if (distance != NULL) {
        *distance = LevelConfigFarDistance;
    }
    return 0;
}

void GameObjectToCameraDistances() {
    i32 object_index;
    const NUVEC camera_position = GameCam->position;
    GAMEOBJECT *object = Obj;
    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
        f32 dx;
        f32 dy;
        f32 dz;
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0) {
            continue;
        }
        if ((object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0) {
            continue;
        }

        dx = camera_position.x - object->position.x;
        dy = camera_position.y - object->position.y;
        dz = camera_position.z - object->position.z;
        object->ai_update_distance = NuFsqrt(dx * dx + dy * dy + dz * dz);
    }
}

void GameObjectOrigin(GAMEOBJECT *object) {
    f32 min_y;
    f32 max_y;
    f32 height;
    i32 joint;
    object->flags_1f8 |= 0x100;
    if (object->use_model_origin > 0 && (joint = object->character_data->game_character->model_origin_joint) != -1 &&
        object->character_model != NULL && object->character_model->points_of_interest[joint] != NULL) {
        if ((object->flags_e24 & 8) != 0) {
            object->collision_position.x =
                object->locator_matrices[joint].m30 + object->previous_velocity.x * FRAMETIME;
            object->collision_position.y =
                (f32)((f64)(object->locator_matrices[joint].m31 + object->previous_velocity.y * FRAMETIME) +
                      (f64)((object->character_bottom + object->character_top) * object->origin_scale) * 0.5);
            object->collision_position.z =
                object->locator_matrices[joint].m32 + object->previous_velocity.z * FRAMETIME;
        } else {
            object->collision_position.x = 0.0f;
            object->collision_position.y = -object->character_bottom;
            object->collision_position.z = 0.0f;
            NuVecMtxRotate(&object->collision_position, &object->collision_position, &object->transform);
            NuVecAdd(&object->collision_position, &object->collision_position, &object->position);
        }
        object->collision_position.x += object->previous_velocity.x * FRAMETIME;
        object->collision_position.y += object->previous_velocity.y * FRAMETIME;
        object->collision_position.z += object->previous_velocity.z * FRAMETIME;
    } else if (object->use_model_origin > 0 && object->origin_scale_override == 1.0f &&
               object->character_data->game_character->collision_origin_joint != -1 &&
               object->character_model
                       ->points_of_interest[object->character_data->game_character->collision_origin_joint] != NULL &&
               object->origin_joint_ready != 0) {
        object->collision_position =
            *(NUVEC *)&object->locator_matrices[object->character_data->game_character->collision_origin_joint].m30;
    } else {
        object->flags_1f8 ^= 0x100;
        object->collision_position.x = object->position.x + object->previous_velocity.x * FRAMETIME;
        object->collision_position.y =
            (f32)((f64)(object->position.y + object->previous_velocity.y * FRAMETIME) +
                  (f64)((object->character_bottom + object->character_top) * object->origin_scale) * 0.5);
        object->collision_position.z = object->position.z + object->previous_velocity.z * FRAMETIME;
    }
    object->collision_min.x = object->collision_position.x - object->bounds_radius;
    object->collision_max.x = object->bounds_radius + object->collision_position.x;
    object->collision_min.z = object->collision_position.z - object->bounds_radius;
    object->collision_max.z = object->collision_position.z + object->bounds_radius;
    object->collision_min.y = min_y = object->collision_position.y - object->bounds_half_height;
    object->collision_max.y = max_y = object->bounds_half_height + object->collision_position.y;
    object->upper_position.x = object->position.x + object->previous_velocity.x * FRAMETIME;
    object->upper_position.y = max_y;
    object->upper_position.z = object->position.z + object->previous_velocity.z * FRAMETIME;
    object->lower_position.x = object->position.x + object->previous_velocity.x * FRAMETIME;
    object->lower_position.y = min_y;
    object->lower_position.z = object->position.z + object->previous_velocity.z * FRAMETIME;
    object->collision_origin.x = object->position.x + object->previous_velocity.x * FRAMETIME;
    object->collision_origin.y = object->previous_velocity.y * FRAMETIME + min_y;
    object->collision_origin.z = object->position.z + object->previous_velocity.z * FRAMETIME;
    object->ai_terrain_origin = object->collision_origin;
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x80000) != 0 &&
        (object->context_variant_flags & 0x800) != 0) {
        height = max_y - min_y;
        object->ai_terrain_origin.y += height;
    } else if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x100000) != 0) {
        object->ai_terrain_origin = object->position;
    } else {
        object->ai_terrain_origin.y -= object->terrain_origin_floor_offset;
        if (object->ai_terrain_origin.y < object->floor_height &&
            object->floor_height < object->collision_origin.y + object->scaled_height) {
            object->ai_terrain_origin.y = object->floor_height;
        }
    }
}

void PortalGameObject(GAMEOBJECT *object, i32 enabled, i32 update_room, i16 room, NUGSCN *scene) {
    if (enabled != 0) {
        object->portal_flags = ((object->portal_flags ^ ((update_room & 1) << 23)) & 0x00800000) ^ object->portal_flags;
        object->portal_flags |= 0x00400000;
        if (room == -1) {
            if (scene != NULL) {
                room = NuPortalWhichRoom(scene, &object->collision_position);
            } else {
                room = -1;
            }
        }
        object->portal_room = room;
    } else {
        object->portal_flags &= ~0x00400000;
    }
}

void SnapCreaturePos(GAMEOBJECT *object, NUVEC *position, i32 angle, AIPATHINFO_PC *path_info, i32 set_on_surface) {
    object->position = *position;
    object->field_0x276 = (u16)angle;
    object->facing_angle = (u16)angle;
    object->movement_facing_angle = (u16)angle;
    object->collision_position = object->position;
    object->initial_position = object->position;
    plr_lastpos = object->collision_position;
    object->start_position = object->collision_position;
    object->respawn_position = object->position;
    object->last_safe_position = object->position;
    object->ai_update_position = object->position;
    object->reset_velocity = v000;
    object->velocity = object->reset_velocity;
    InitSurfaceInfo(object);
    if (set_on_surface != 0) {
        SetObjOnSurface(object, 0);
    }
    if (path_info != NULL) {
        object->ai_packet.path_info = *path_info;
    } else {
        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
    }
}

void UpdateLastSafePosition(GAMEOBJECT *object) {
    GAMEOBJECT *terrain_object;
    i32 unsafe;
    i8 surface;
    i8 layer;
    i8 context;
    i16 platform;
    i16 shadow_platform;
    u32 model_flags;

    object->flags_ef4 &= ~0x400000;
    if (!object->dead && (LEGOCONTEXT_DOOMED == -1 || object->character_context != LEGOCONTEXT_DOOMED) &&
        (object->flags_1fc & 4) == 0) {
        object->previous_position = object->position;
        terrain_object = object->takeover_source != NULL && LEGOCONTEXT_BEENTAKENOVER != -1 &&
                                 object->character_context == LEGOCONTEXT_BEENTAKENOVER
                             ? object->takeover_source
                             : object;
        surface = terrain_object->jump_animation_state;
        if ((surface != -1 && (TerSurface[surface].flags & 0xc041) != 0) ||
            ((layer = terrain_object->field_283) != -1 && ((TerLayer[layer].flags & 1) != 0 || layer == 9))) {
            unsafe = 1;
            if (VehicleArea && BonusArea && terrain_object->character_data->game_character->field_028 > 0.0f) {
                unsafe = 0;
            }
        } else {
            unsafe = 0;
        }
        do {
            if ((object->flags_ef4 & 4) != 0) {
                break;
            }
            context = object->character_context;
            if ((U64_PAIR(&CInfo[context].flags) & 0x800000000ULL) != 0) {
                break;
            }
            if (object->field_281 == 0 &&
                (context == -1 || (context != LEGOCONTEXT_BEENTAKENOVER && context != LEGOCONTEXT_EATEN &&
                                   context != LEGOCONTEXT_GETIN)) &&
                !VehicleArea) {
                model_flags = object->character_data->model_flags;
                if (((model_flags & 0x2000) == 0 || !(object->character_data->game_character->field_028 > 0.0f)) &&
                    ((model_flags & 0x8000) == 0 || object->jump_variant_state != 1)) {
                    break;
                }
            }
            if (object->floor_height == 2000000.0f) {
                break;
            }
            platform = object->terrain_platform_id;
            if (!((platform == -1 || platform == LevSafePlatID[0] || platform == LevSafePlatID[1]) &&
                  ((shadow_platform = object->supporting_platform_id) == -1 || shadow_platform == LevSafePlatID[0] ||
                   shadow_platform == LevSafePlatID[1])) &&
                (LastSafePosExtraFn == NULL || !LastSafePosExtraFn(object))) {
                break;
            }
            if (!(object->lower_position.y > object->floor_height - (f64)0.1f) || unsafe) {
                break;
            }
            object->flags_ef4 |= 0x400000;
            object->last_safe_position = object->position;
            object->fall_recovery_timer = 0.0f;
            if ((object->ai_packet.path_info.flags & 1) != 0 &&
                (object->ai_packet.path_info.connection->traversal_flags[1] |
                 object->ai_packet.path_info.connection->traversal_flags[0]) == 0) {
                if (object->takeover_owner != NULL &&
                    (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                    (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER)) {
                    object->takeover_owner = NULL;
                }
                object->respawn_position = object->last_safe_position;
                object->flags_1fc_word |= 0x2000;
            }
            if ((object->field_280 == 0 || object->field_280 == 1) && object->field_da8 <= 0.0f &&
                GameCam->blend_time >= GameCam->blend_duration && object->sock_index != 0xff) {
                if (object->takeover_owner != NULL &&
                    (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                    (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER)) {
                    object->takeover_owner = NULL;
                }
                object->camera_saved_position = object->last_safe_position;
            }
            goto safe_position_done;
        } while (0);
        if ((object->flags_1fc & 0x80) != 0 && (layer = object->field_283) != -1 && (TerLayer[layer].flags & 1) != 0 &&
            !NoLayerKill(object)) {
            if (object->fall_recovery_timer <= 0.0f) {
                object->fall_hover_height = object->position.y;
            }
            object->fall_recovery_timer = FRAMETIME + object->fall_recovery_timer;
        } else {
            object->fall_recovery_timer = 0.0f;
        }
        if ((WORLD->current_level->flags & 0x40000) != 0) {
            object->last_safe_position = object->position;
        }
    }
safe_position_done:
    if ((object->flags_ef4 & 0x1000000) == 0 && (object->flags_1fc_word & 0x80) == 0 &&
        (LEGOCONTEXT_EATEN == -1 || object->character_context != LEGOCONTEXT_EATEN) &&
        (object->flags_1fc_word & 0x2000) != 0) {
        if ((object->ai_packet.path_info.flags & 1) != 0) {
            if (object->takeover_owner != NULL &&
                (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER)) {
                object->takeover_owner = NULL;
            }
        }
        object->last_safe_position = object->respawn_position;
    }
    object->flags_ef4 &= ~4;
}

void GameObjectSetCanUse(GAMEOBJECT *object, void *target, u8 action, u8 unused, f32 parameter) {
    object->can_use_object = target;
    object->field_f00 = action;
    object->use_action_frames = grapple_attach_frames;
    object->use_action_parameter = parameter;
}

/* Bitwise absolute value through a float temporary. */

// Copies the antinode timer and its 0x8000 flag from one creature's packet to the other's.
typedef void (*CHECK_ANTINODE_FN_PC)(APIOBJECT_PC *, AIANTINODE_PC *, NUVEC *, f32);
extern CHECK_ANTINODE_FN_PC checkantinodefns[3];

typedef struct OPPONENT_PACKET_PC {
    u8 unknown000[0xd4];                    // 0x000
    APIOBJECT_PC *nearest_opponent_object;  // 0x0d4
    f32 nearest_opponent_metric;            // 0x0d8
    APIOBJECT_PC *pending_nearest_opponent; // 0x0dc
    f32 pending_nearest_metric;             // 0x0e0
    APIOBJECT_PC *opponent_object;          // 0x0e4
    f32 opponent_metric;                    // 0x0e8
    APIOBJECT_PC *pending_opponent;         // 0x0ec
    f32 pending_opponent_metric;            // 0x0f0
    u8 unknown0f4[0xf4];                    // 0x0f4
    u32 flags;                              // 0x1e8
} OPPONENT_PACKET_PC;                       // size 0x1ec

typedef struct OPPONENT_OBJECT_PC {
    u8 unknown000[0x230];           // 0x000
    f32 view_distance;              // 0x230
    u8 unknown234[0xc];             // 0x234
    f32 visibility_range_extension; // 0x240
    u8 unknown244[0x64];            // 0x244
    u64 awareness;                  // 0x2a8
} OPPONENT_OBJECT_PC;               // size 0x2b0

typedef struct OPPONENT_GAME_PC {
    u8 unknown000[0xeb0];       // 0x000
    u64 seen;                   // 0xeb0
    u64 exclusion;              // 0xeb8
    APIOBJECT_PC *alert_target; // 0xec0
    f32 alert_timer;            // 0xec4
} OPPONENT_GAME_PC;             // size 0xec8

typedef struct OPPONENT_SYS_PC {
    u8 unknown000[0x1385]; // 0x0000
    u8 goody_cursor;       // 0x1385
    u16 assignment_flags;  // 0x1386
} OPPONENT_SYS_PC;         // size 0x1388

typedef struct OPPONENT_LOS_PC {
    u32 object_size;       // 0x000
    GAMEOBJECT *objects;   // 0x004
    u64 line_of_sight[64]; // 0x008
} OPPONENT_LOS_PC;         // size 0x208

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_visibility_extension) == 0x240);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_collision_priority_pc) == 0x292);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_resolved_collision_priority_pc) == 0x294);
STATIC_ASSERT(offsetof(GAMEOBJECT, camera_screen_position) == 0xc5c);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_hop_timer) == 0xde8);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_e22) == 0xe22);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, combat_flags) == 0xeec);
STATIC_ASSERT(offsetof(GAMEOBJECT, combat_flags4) == 0xeef);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef3) == 0xef3);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_update_flags) == 0xef4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_f00) == 0xf00);
STATIC_ASSERT(offsetof(GAMEOBJECT, use_action_frames) == 0xf01);
STATIC_ASSERT(offsetof(GAMEOBJECT, use_action_parameter) == 0xf04);
STATIC_ASSERT(offsetof(GAMEOBJECT, route_frame_state) == 0xf08);
STATIC_ASSERT(offsetof(GAMEOBJECT, route_suit_index) == 0x1068);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_frame_event) == 0x1072);
STATIC_ASSERT(offsetof(GAMEOBJECT, route_start_index) == 0x1081);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1088) == 0x1088);
STATIC_ASSERT(offsetof(GAMEOBJECT, route_search_index) == 0x1089);

STATIC_ASSERT(offsetof(WORLDINFO, script_processor_count) == 0x2aec);
STATIC_ASSERT(offsetof(WORLDINFO, script_processors) == 0x2af0);
STATIC_ASSERT(offsetof(WORLDINFO, ai_path_cnx_control_sys) == 0x4670);
STATIC_ASSERT(offsetof(WORLDINFO, ai_connection_helpers) == 0x4674);

typedef struct ai_frame_world_pc_s {
    u8 unknown_0000[0x2aec];                // 0x0000
    i32 processor_count;                    // 0x2aec
    LEVEL_SCRIPT_PROCESS_PC processors[32]; // 0x2af0
} AI_FRAME_WORLD_PC;                        // size 0x4670
