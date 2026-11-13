#include "saga.h"

/* Retail unit 0x55c950-0x55dd9f: definitions in retail address order. */

i32 __cdecl TagCode(GAMEOBJECT *source, GAMEOBJECT *destination, i32 takeover, i32 blend_camera, i32 unused);
#pragma bss_seg(".bss$player_tag_from")
GAMEOBJECT *player_tag_from = 0;
#pragma bss_seg()
void __cdecl MovePlayer_NETWORK(GAMEOBJECT *);

STATIC_ASSERT(offsetof(PART_PC, field_206) == 0x206);

typedef struct grapple_rotation_pc_s {
    u8 unknown_00[0x30];
    NUVEC hook_position;
} GRAPPLE_ROTATION_PC;
STATIC_ASSERT(offsetof(GRAPPLE_ROTATION_PC, hook_position) == 0x30);

extern const f64 GameObjectNearFloorNoFloorHeight;
extern const f64 GameObjectNearFloorRadiusDivisor;

extern u8 SuperCarryPlayerRgbPc[2][3];
extern i32 FalconDebKey[2];
STATIC_ASSERT(offsetof(GAMEOBJECT, locator_matrices) + 2 * sizeof(NUMTX) == 0x884);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, points_of_interest) + 2 * sizeof(void *) == 0x1c);
void __cdecl DieRumble(GAMEOBJECT *);
void __cdecl GetTakeOverPos(GAMEOBJECT *, NUVEC *);
extern i16 CharacterFixupIdPc007f18c4;
extern const char BeenKilledPc[11];
extern const char EP3DogfightDiePc[17];
STATIC_ASSERT(offsetof(AREA_GLOBALS_PC, field_00) == 0);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, pickup_positions) == 0x3c);
STATIC_ASSERT(offsetof(GAMEDEBRISENTRY_PC, effect) == 0);
typedef struct kill_zipup_pc_s {
    u8 unknown_00[0x68];
    u16 runtime_flags;
} KILL_ZIPUP_PC;
STATIC_ASSERT(offsetof(KILL_ZIPUP_PC, runtime_flags) == 0x68);
/* A copy of the actual 33-DWORD script cohort (script processor plus takeover link), without extending the
   packet. Held as words so the copy is one block move and the local is not treated as a /GS buffer. */
typedef union TAGSCRIPTSTATE_PC {
    u32 words[0x33];
    struct {
        u32 unknown000[0x2c];
        u8 creature_set;
    } fields;
} TAGSCRIPTSTATE_PC;
STATIC_ASSERT(sizeof(AISCRIPTPROCESS_PC) == 0xc8);
STATIC_ASSERT(offsetof(AIPACKET_PC, takeover_script_link) - offsetof(AIPACKET_PC, script_processor) == 0xc8);
STATIC_ASSERT(sizeof(TAGSCRIPTSTATE_PC) == 0xcc);
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
/* Source-private in gameobjects.c. Forward pointer uses struct tag in saga.h.
 * Tail[1] exposes actual counted traversal; it is NOT owner/allocation count.
 */
typedef struct shadow_platform_entry_pc_s {
    GAMEOBJECT *object;
    u8 unknown_04_14[0x10];
} SHADOW_PLATFORM_ENTRY_PC;
struct shadow_platform_list_pc_s {
    u8 unknown_00_04[4];
    i32 count;
    u8 unknown_08_18[0x10];
    SHADOW_PLATFORM_ENTRY_PC entries[1];
};
STATIC_ASSERT(sizeof(SHADOW_PLATFORM_ENTRY_PC) == 0x14);
STATIC_ASSERT(offsetof(SHADOW_PLATFORM_ENTRY_PC, object) == 0);
STATIC_ASSERT(offsetof(struct shadow_platform_list_pc_s, count) == 4);
STATIC_ASSERT(offsetof(struct shadow_platform_list_pc_s, entries) == 0x18);
typedef struct gameobject_collision_reset_pc_s {
    u8 unknown_000[0x2b0];
    u32 collision_overlap_low;
    u32 collision_overlap_high;
} GAMEOBJECT_COLLISION_RESET_PC;
STATIC_ASSERT(offsetof(GAMEOBJECT_COLLISION_RESET_PC, collision_overlap_low) == 0x2b0);
STATIC_ASSERT(offsetof(GAMEOBJECT_COLLISION_RESET_PC, collision_overlap_high) == 0x2b4);
/* Genuine original call declarations: no placeholder implementations. */
GAMEOBJECT *__cdecl AddCreature(i32 model, i32 mode);
extern i32 complexsockposition_forcesock;
typedef struct GAME_ANTI_NODE_DATA_PC {
    NUVEC position;
    f32 radius, min_y, max_y, extent_x, extent_z;
    u16 angle;
    u8 shape, mode;
} GAME_ANTI_NODE_DATA_PC;
STATIC_ASSERT(sizeof(GAME_ANTI_NODE_DATA_PC) == 0x24);
STATIC_ASSERT(offsetof(GAME_ANTI_NODE_DATA_PC, position) == 0);
STATIC_ASSERT(offsetof(GAME_ANTI_NODE_DATA_PC, radius) == 0xc);
STATIC_ASSERT(offsetof(GAME_ANTI_NODE_DATA_PC, min_y) == 0x10);
STATIC_ASSERT(offsetof(GAME_ANTI_NODE_DATA_PC, max_y) == 0x14);
STATIC_ASSERT(offsetof(GAME_ANTI_NODE_DATA_PC, extent_x) == 0x18);
STATIC_ASSERT(offsetof(GAME_ANTI_NODE_DATA_PC, extent_z) == 0x1c);
STATIC_ASSERT(offsetof(GAME_ANTI_NODE_DATA_PC, angle) == 0x20);
STATIC_ASSERT(offsetof(GAME_ANTI_NODE_DATA_PC, shape) == 0x22);
STATIC_ASSERT(offsetof(GAME_ANTI_NODE_DATA_PC, mode) == 0x23);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_link) == 0x298);
STATIC_ASSERT(offsetof(APIOBJECT_PC, ai_collision_priority_pc) == 0x292);
STATIC_ASSERT(offsetof(APIOBJECT_PC, ai_resolved_collision_priority_pc) == 0x294);
STATIC_ASSERT(offsetof(APIOBJECT_PC, collision_link) == 0x298);
STATIC_ASSERT(offsetof(AIPACKET_PC, antinode_timer) == 0x18c);
STATIC_ASSERT(offsetof(GAMEOBJECT, lighting_packet) == 0x4d8);
STATIC_ASSERT(offsetof(GAMEPAD_PC, rumble) == 0x40);

void __cdecl ClearGameObjects(APIOBJECTSYS_PC *system) {
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

GAMEOBJECT *__cdecl AddGameObject(void) {
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

void __cdecl RemoveGameObject(GAMEOBJECT *object, i32 unused) {
    GAMEOBJECT *other;
    i32 i;
    (void)unused;
    if (object == NULL)
        return;
    other = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++other) {
        // Retail shifts through __allshl, which yields 0 for groups >= 64.
        U64_PAIR(other->collision_contact_mask) &= ~U64_PAIR(object->collision_identity_mask);
        U64_PAIR(other->hostility_mask) &= ~((u64)1 << object->hostility_group);
        U64_PAIR(&other->field_eb0) &= ~((u64)1 << object->hostility_group);
        U64_PAIR(other->ai_opponent_exclusion_mask) &= ~((u64)1 << object->hostility_group);
    }
    if (object->pad != NULL)
        object->pad->runtime_flags &= ~1u;
    DestroySnakeBody(object);
    APIObjectDestroy(WORLD->object_sys, object);
    RecomputeHighGameObject();
    for (i = 0; i < 8; ++i) {
        if (LevGameObject[i] == object)
            LevGameObject[i] = NULL;
    }
}

void __cdecl GameObjectDimensions(GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG_PC *config;
    if (object->use_model_origin > 0 && object->origin_scale_override == 1.0f) {
        config = object->character_data->game_character;
        if (config->collision_origin_joint != -1 &&
            ((CHARACTER_MODEL_LAYERS_PC *)object->character_model)
                    ->points_of_interest[config->collision_origin_joint] != NULL &&
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
    if (GameObjectDimensionsExtraFn != NULL)
        GameObjectDimensionsExtraFn(object);
}

__declspec(noinline) void __cdecl ScaleGameObject(GAMEOBJECT *object) {
    CHARACTERDATA_PC *character = object->character_data;
    object->scaled_radius = character->scaled_radius * object->origin_scale;
    object->collision_radius = object->collision_radius_unscaled * object->origin_scale;
    object->field_b0 = object->collision_radius * object->collision_y_scale;
    object->scaled_height = (character->top - character->bottom) * object->collision_scale;
}

void __cdecl GetTopBot(GAMEOBJECT *object) {
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

void __cdecl SetToLastSafePos(GAMEOBJECT *object) {
    object->start_position = object->last_safe_position;
    object->position = object->last_safe_position;
    object->ai_update_position = object->last_safe_position;
}

i32 __cdecl UnderPlayerControl(GAMEOBJECT *object) {
    if ((object->flags_1fc_word & 0x80) != 0)
        return 1;
    if (object->takeover_source != NULL && (object->takeover_source->flags_1fc_word & 0x80) != 0)
        return 1;
    return 0;
}

__declspec(noinline) GAMEOBJECT *__cdecl FindGameObject(i32 character_id, u32 required_flags, i32 alive_only,
                                                        i32 vehicle_only, i32 non_level_only) {
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

GAMEOBJECT *__cdecl FindNearestGameObject(NUVEC *position, GAMEOBJECT *exclude, u32 required_flags, f32 radius,
                                          f32 extra_radius, i32 animation, i32 character_id, i32 player_index,
                                          f32 *distance_squared, i32 horizontal_only, i32 (*filter)(GAMEOBJECT *)) {
    f32 radius_squared = radius * radius;
    GAMEOBJECT *nearest = NULL;
    f32 nearest_distance = 100000000.0f;
    GAMEOBJECT *object = Obj;
    i32 i;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        f32 x, y, z, distance;
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0 ||
            (object->flags_e20 & 0x20) || (U64_PAIR(&CInfo[object->character_context].flags) & 0x8000))
            continue;
        if (character_id != -1 && (i16)object->id != character_id)
            continue;
        if (player_index >= 0) {
            if (player_index < 2) {
                if (((i8)object->field_280) != player_index)
                    continue;
            } else if (player_index == 99) {
                if (((i8)object->field_280) == -1)
                    continue;
            } else if (player_index == 100) {
                if (((i8)object->field_280) != -1)
                    continue;
            }
        }
        if (exclude && object == exclude)
            continue;
        if (required_flags && (object->character_data->model_flags & required_flags) != required_flags)
            continue;
        if (animation != -1 && object->character_model->model_data_b[animation] == NULL)
            continue;
        if (filter && !filter(object))
            continue;
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
    if (nearest && distance_squared)
        *distance_squared = nearest_distance;
    return nearest;
}

i32 __cdecl GameObjectNearFloor(GAMEOBJECT *object, f32 height, f32 *distance) {
    f32 tolerance;
    f32 floor_distance;

    if (!(height > 0.0f)) {
        height = 0.0f;
    }
    height = (f32)((f64)height * NuTimeBarSlotWidth);
    if ((f64)object->floor_height != GameObjectNearFloorNoFloorHeight) {
        tolerance = height;
        height = (f32)(((f64)object->collision_radius / GameObjectNearFloorRadiusDivisor) * tolerance);
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
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0)
            continue;
        if ((object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0)
            continue;

        dx = camera_position.x - object->position.x;
        dy = camera_position.y - object->position.y;
        dz = camera_position.z - object->position.z;
        object->ai_update_distance = NuFsqrt(dx * dx + dy * dy + dz * dz);
    }
}

void __cdecl GameObjectOrigin(GAMEOBJECT *object) {
    f32 min_y;
    f32 max_y;
    f32 height;
    i32 joint;
    object->flags_1f8 |= 0x100;
    if (object->use_model_origin > 0 && (joint = object->character_data->game_character->model_origin_joint) != -1 &&
        object->character_model != NULL &&
        ((CHARACTER_MODEL_LAYERS_PC *)object->character_model)->points_of_interest[joint] != NULL) {
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
               ((CHARACTER_MODEL_LAYERS_PC *)object->character_model)
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

__declspec(noinline) void __cdecl PortalGameObject(GAMEOBJECT *object, i32 enabled, i32 update_room, i16 room,
                                                   NUGSCN *scene) {
    if (enabled != 0) {
        object->portal_flags = ((object->portal_flags ^ ((update_room & 1) << 23)) & 0x00800000) ^ object->portal_flags;
        object->portal_flags |= 0x00400000;
        if (room == -1) {
            if (scene != NULL)
                room = NuPortalWhichRoom(scene, &object->collision_position);
            else
                room = -1;
        }
        object->portal_room = room;
    } else {
        object->portal_flags &= ~0x00400000;
    }
}

void __cdecl SnapCreaturePos(GAMEOBJECT *object, NUVEC *position, i32 angle, AIPATHINFO_PC *path_info,
                             i32 set_on_surface) {
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

void __cdecl UpdateLastSafePosition(GAMEOBJECT *object) {
    GAMEOBJECT *terrain_object;
    i32 unsafe;
    i8 surface, layer, context;
    i16 platform, shadow_platform;
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
            if (VehicleArea && BonusArea && terrain_object->character_data->game_character->field_028 > 0.0f)
                unsafe = 0;
        } else
            unsafe = 0;
        do {
            if ((object->flags_ef4 & 4) != 0)
                break;
            context = object->character_context;
            if ((U64_PAIR(&CInfo[context].flags) & 0x800000000ULL) != 0)
                break;
            if (object->field_281 == 0 &&
                (context == -1 || (context != LEGOCONTEXT_BEENTAKENOVER && context != LEGOCONTEXT_EATEN &&
                                   context != LEGOCONTEXT_GETIN)) &&
                !VehicleArea) {
                model_flags = object->character_data->model_flags;
                if (((model_flags & 0x2000) == 0 || !(object->character_data->game_character->field_028 > 0.0f)) &&
                    ((model_flags & 0x8000) == 0 || object->jump_variant_state != 1))
                    break;
            }
            if (object->floor_height == 2000000.0f)
                break;
            platform = object->terrain_platform_id;
            if (!((platform == -1 || platform == LevSafePlatID[0] || platform == LevSafePlatID[1]) &&
                  ((shadow_platform = object->supporting_platform_id) == -1 || shadow_platform == LevSafePlatID[0] ||
                   shadow_platform == LevSafePlatID[1])) &&
                (LastSafePosExtraFn == NULL || !LastSafePosExtraFn(object)))
                break;
            if (!(object->lower_position.y > object->floor_height - (f64)0.1f) || unsafe)
                break;
            object->flags_ef4 |= 0x400000;
            object->last_safe_position = object->position;
            object->fall_recovery_timer = 0.0f;
            if ((object->ai_packet.path_info.flags & 1) != 0 &&
                (object->ai_packet.path_info.connection->traversal_flags[1] |
                 object->ai_packet.path_info.connection->traversal_flags[0]) == 0) {
                if (object->takeover_owner != NULL &&
                    (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                    (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER))
                    object->takeover_owner = NULL;
                object->respawn_position = object->last_safe_position;
                object->flags_1fc_word |= 0x2000;
            }
            if ((object->field_280 == 0 || object->field_280 == 1) && object->field_da8 <= 0.0f &&
                GameCam->blend_time >= GameCam->blend_duration && object->sock_index != 0xff) {
                if (object->takeover_owner != NULL &&
                    (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                    (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER))
                    object->takeover_owner = NULL;
                object->camera_saved_position = object->last_safe_position;
            }
            goto safe_position_done;
        } while (0);
        if ((object->flags_1fc & 0x80) != 0 && (layer = object->field_283) != -1 && (TerLayer[layer].flags & 1) != 0 &&
            !NoLayerKill(object)) {
            if (object->fall_recovery_timer <= 0.0f)
                object->fall_hover_height = object->position.y;
            object->fall_recovery_timer = FRAMETIME + object->fall_recovery_timer;
        } else
            object->fall_recovery_timer = 0.0f;
        if ((WORLD->current_level->flags & 0x40000) != 0)
            object->last_safe_position = object->position;
    }
safe_position_done:
    if ((object->flags_ef4 & 0x1000000) == 0 && (object->flags_1fc_word & 0x80) == 0 &&
        (LEGOCONTEXT_EATEN == -1 || object->character_context != LEGOCONTEXT_EATEN) &&
        (object->flags_1fc_word & 0x2000) != 0) {
        if ((object->ai_packet.path_info.flags & 1) != 0) {
            if (object->takeover_owner != NULL &&
                (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER))
                object->takeover_owner = NULL;
        }
        object->last_safe_position = object->respawn_position;
    }
    object->flags_ef4 &= ~4;
}

__declspec(noinline) void __cdecl GameObjectSetCanUse(GAMEOBJECT *object, void *target, u8 action, u8 unused,
                                                      f32 parameter) {
    object->can_use_object = target;
    object->field_f00 = action;
    object->use_action_frames = grapple_attach_frames;
    object->use_action_parameter = parameter;
}

void __cdecl GameObjectRotation(GAMEOBJECT *object, i32 mode) {
    PLAYERCHARACTERCONFIG_PC *game;
    u16 x;
    u16 z;
    u32 flags;
    i32 magnet;
    i32 sliding;
    i32 grapple;
    i32 alignment = 0;
    i32 reflection;
    NUVEC direction;

    if ((object->character_data->game_character->flags_090 & 0x200000) != 0) {
        NUVEC normal;
        NUVEC corner;
        NUVEC next;
        NUVEC after;
        NUVEC first_edge;
        NUVEC second_edge;
        NUVEC points[4];
        const NUVEC *offsets;
        f32 drop = object->position.y - object->start_position.y;
        f32 rate = drop / FRAMETIME;
        f32 height;
        i32 index;
        if (object->id == GameObjectRotationBuggyId) {
            offsets = GAMEOBJECT_ROTATION_BUGGY_OFFSETS;
        } else if (object->id == GameObjectRotationRadioCarId) {
            offsets = GAMEOBJECT_ROTATION_RADIOCAR_OFFSETS;
        } else if (object->id == GameObjectRotationPoliceCarId) {
            offsets = GAMEOBJECT_ROTATION_POLICECAR_OFFSETS;
        } else if (object->id == GameObjectRotationMowerId) {
            offsets = GAMEOBJECT_ROTATION_MOWER_OFFSETS;
        } else {
            offsets = GAMEOBJECT_ROTATION_DEFAULT_OFFSETS;
        }
        for (index = 0; index < 4; ++index) {
            NuVecRotateY(&points[index], (NUVEC *)&offsets[index], object->field_0x276);
            NuVecAdd(&points[index], &points[index], &object->lower_position);
            height = GameShadow(NULL, &points[index], 5.0f, -1);
            if (height == GameObjectNearFloorNoFloorHeight) {
                object->suspension[index].height = object->lower_position.y;
                object->suspension[index].velocity = drop / FRAMETIME;
            } else {
                f64 gravity_delta;
                object->suspension[index].height = drop + object->suspension[index].height;
                gravity_delta = (f64)FRAMETIME * -5.0;
                object->suspension[index].velocity = (f32)(gravity_delta + object->suspension[index].velocity);
                object->suspension[index].height += object->suspension[index].velocity * FRAMETIME;
                if (object->suspension[index].height < height) {
                    object->suspension[index].height = height;
                    object->suspension[index].velocity = rate;
                }
                height = object->lower_position.y - object->bounds_radius * 2.0f;
                if (object->suspension[index].height < height) {
                    object->suspension[index].height = height;
                    object->suspension[index].velocity = rate;
                } else {
                    height = object->bounds_radius * 3.0 + object->lower_position.y;
                    if (object->suspension[index].height > height) {
                        object->suspension[index].height = height;
                        object->suspension[index].velocity = rate;
                    }
                }
            }
            corner.x = points[index].x;
            corner.y = object->suspension[index].height;
            corner.z = points[index].z;
        }
        normal = v000;
        for (index = 0; index < 4;) {
            i32 next_index = (index + 1) % 4;
            i32 after_index = (next_index + 1) % 4;
            corner.x = points[index].x;
            corner.y = object->suspension[index].height;
            corner.z = points[index].z;
            next.x = points[next_index].x;
            next.y = object->suspension[next_index].height;
            next.z = points[next_index].z;
            after.x = points[after_index].x;
            after.y = object->suspension[after_index].height;
            after.z = points[after_index].z;
            NuVecSub(&first_edge, &next, &corner);
            NuVecSub(&second_edge, &after, &corner);
            NuVecCross(&first_edge, &first_edge, &second_edge);
            NuVecAdd(&normal, &normal, &first_edge);
            index++;
        }
        NuVecScale(&normal, &normal, 0.25f);
        if (normal.y > 0.0f) {
            NuVecNorm(&object->suspension_normal, &normal);
        }
    }

    if (0 == ((object->flags_1fc & 4))) {
        if (0 != ((object->character_data->game_character->flags_090 & 0x200000))) {
            FindAnglesZX(&object->suspension_normal, &object->field_1056, &object->field_1058);
        } else if (object->surface_normal.x != 0.0f || object->surface_normal.y != 1.0f ||
                   object->surface_normal.z != 0.0f) {
            FindAnglesZX(&object->surface_normal, &object->field_1056, &object->field_1058);
        } else {
            object->field_1058 = 0;
            object->field_1056 = 0;
        }
    }

    x = object->field_1056;
    temp_zrot = x;
    z = object->field_1058;
    temp_xrot = z;
    game = object->character_data->game_character;
    flags = game->flags_090;
    sliding = (flags & 0x200) != 0 && object->character_context == 0x33;
    magnet = (flags & 0x10000000) != 0 && object->character_context == 0x44;
    grapple = object->character_context == 0x46 && object->field_7b3 == 0;

    if (object->flags_107a == 2 &&
        ((object->character_data->model_flags & 0x100) != 0 || (flags & 0x200000) != 0 || sliding)) {
        f32 hover =
            (object->character_data->model_flags & 0x2000) != 0 ? GetVehicleHoverHeight(object, NULL) : game->field_028;
        if (hover > 0.0f) {
            if (GameObjectNearFloorNoFloorHeight != object->floor_height) {
                f32 limit = 2.5 * hover;
                f32 height = object->collision_min.y - object->floor_height;
                f32 fraction;
                if (height < hover) {
                    fraction = 1.0f;
                } else if (limit > height) {
                    fraction = 1.0f - (height - hover) / (limit - hover);
                } else {
                    fraction = 0.0f;
                }
                x = (u16)((f32)RotDiff(0, temp_zrot) * fraction);
                z = (u16)((f32)RotDiff(0, temp_xrot) * fraction);
            } else {
                x = 0;
                z = 0;
            }
            alignment = 2;
        } else if (object->field_281 != 0 || (object->character_data->game_character->flags_090 & 0x200000) != 0 ||
                   sliding) {
            alignment = 1;
        } else if ((object->character_data->model_flags & 0x100) != 0 &&
                   0.0f == object->character_data->game_character->field_028 &&
                   (object->character_data->game_character->flags_090 & 0x100) != 0 &&
                   GameObjectNearFloorNoFloorHeight != object->floor_height) {
            f32 vertical_gap = object->collision_min.y - object->floor_height;
            if (vertical_gap < object->bounds_half_height)
                alignment = 1;
        }

        if (object->id == GameObjectRotationAtstId && VehicleArea == 0) {
            x = (u16)((f32)RotDiff(0, temp_zrot) * 0.5);
            z = (u16)((f32)RotDiff(0, temp_xrot) * 0.5);
        }
    }

    reflection = 0 != (0x2000 & object->character_data->model_flags) &&
                 object->character_data->game_character->field_028 > 0.0f &&
                 (object->field_283 == 7 || object->field_283 == 0x10) && object->field_280 == 0xff;

    if (mode == 1) {
        object->field_1052 = x;
        object->field_1054 = z;
        if (alignment == 0) {
            object->pitch_angle = object->roll_angle = 0;
        } else if (magnet) {
            if (object->field_7aa == 6 || object->field_7aa == 5) {
                object->pitch_angle = 0;
                object->roll_angle = 0;
            } else {
                object->pitch_angle = object->field_7a4;
                object->roll_angle = object->field_7a8;
            }
        } else if (grapple) {
            GRAPPLE_ROTATION_PC *hook = (GRAPPLE_ROTATION_PC *)object->grapple;
            NuVecSub(&direction, &hook->hook_position, &object->collision_position);
            FindAnglesZX(&direction, &object->pitch_angle, &object->roll_angle);
        } else if (reflection) {
            object->pitch_angle = object->field_105c;
            object->roll_angle = object->field_105e;
        } else if (!(alignment == 2 || object->surface_normal.y > NuTrigTable[0x3000])) {
            object->pitch_angle = object->roll_angle = 0;
        } else {
            object->pitch_angle = x;
            object->roll_angle = z;
        }
    } else if (mode == 2) {
        if (object->field_1052 != object->field_1056) {
            object->field_1052 = SeekRot(object->field_1052, x, 8.0f);
        }
        if (object->field_1054 != object->field_1058) {
            object->field_1054 = SeekRot(object->field_1054, z, 8.0f);
        }
        if (object->flags_107a == 2) {
            f32 speed;
            u16 target_x, target_z;
            if (magnet) {
                if (object->field_7aa == 6 || object->field_7aa == 5) {
                    object->pitch_angle = 0;
                    target_z = 0;
                    object->roll_angle = 0;
                    target_x = 0;
                } else {
                    target_x = object->field_7a4;
                    target_z = object->field_7a8;
                }
            } else if (grapple) {
                GRAPPLE_ROTATION_PC *hook = (GRAPPLE_ROTATION_PC *)object->grapple;
                NuVecSub(&direction, &hook->hook_position, &object->collision_position);
                FindAnglesZX(&direction, &target_x, &target_z);
            } else if (reflection) {
                target_x = object->field_105c;
                target_z = object->field_105e;
            } else if (alignment == 2 || (alignment != 0 && object->surface_normal.y > NuTrigTable[0x3000])) {
                target_x = x;
                target_z = z;
            } else {
                target_z = 0;
                target_x = 0;
            }

            speed = object->id == GameObjectRotationAtstId && VehicleArea == 0 ? 3.0f : 8.0f;
            if (object->pitch_angle != target_x) {
                object->pitch_angle = SeekRot(object->pitch_angle, target_x, speed);
            }
            if (object->roll_angle != target_z) {
                object->roll_angle = SeekRot(object->roll_angle, target_z, speed);
            }
        }
    }
}

void __cdecl PlayerTakeHit(GAMEOBJECT *object, GAMEOBJECT *attacker) {
    i32 state;
    i32 animation;
    f32 duration;
    if ((i8)object->field_280 != -1)
        return;
    if (object->jump_variant_state == 1 && object->character_model->model_data_b[0x3e] != NULL)
        animation = 0x3e;
    else if (object->jump_variant_state != 1 && object->character_model->model_data_b[0x3d] != NULL)
        animation = 0x3d;
    else {
        if (object->portal_flags & 0x08000000) {
            ResetAnimPacket(&object->anim_packet, -1);
            Player_ClearContext(object, 1);
            Player_ResetContexts(object->ai_player_packet_pc);
        }
        return;
    }
    state = object->jump_variant_state;
    if (object->character_context == 0x1b && object->force_target != NULL &&
        object->force_target->character_context == 0x1c)
        object->force_target->character_context = -1;
    object->field_7aa = (i16)animation;
    ResetAnimPacket(&object->anim_packet, -1);
    Player_ClearContext(object, 1);
    Player_ResetContexts(object->ai_player_packet_pc);
    object->character_context = 0x15;
    duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
    object->context_timer = duration;
    if (duration <= 0.0f)
        object->context_timer = 1.0f;
    object->jump_variant_state = state;
    SetFlicker(object, 0.4f);
    if (attacker != NULL) {
        object->facing_angle = object->movement_facing_angle = object->field_0x276 =
            NuAtan2D(attacker->collision_position.x - object->collision_position.x,
                     attacker->collision_position.z - object->collision_position.z);
    }
}

void __cdecl MakeBaddiesForgetAboutParty(i32 check_hostility) {
    GAMEOBJECT *baddies[64];
    GAMEOBJECT *party[64];
    i32 baddie_count = 0;
    i32 party_count = 0;
    GAMEOBJECT **baddie_cursor = baddies;
    GAMEOBJECT **party_cursor = party;
    GAMEOBJECT *object = Obj;
    i32 count = HIGHGAMEOBJECT;
    i32 i, j;
    for (i = 0; i < count; ++i, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0)
            continue;
        if ((object->flags_1f8 & 1) != 0) {
            ++party_count;
            *party_cursor++ = object;
        } else if ((object->flags_1f8 & 4) == 0) {
            ++baddie_count;
            *baddie_cursor++ = object;
        }
    }
    if (check_hostility != 0) {
        for (i = 0; i < baddie_count; ++i) {
            for (j = 0; j < party_count; ++j) {
                if ((U64_PAIR(party[j]->hostility_mask) & ((u64)1 << baddies[i]->hostility_group)) != 0 &&
                    (WORLD->object_sys->hostility_masks[party[j]->hostility_group] &
                     ((u64)1 << baddies[i]->hostility_group)) != 0)
                    return;
            }
        }
    }
    for (i = 0; i < baddie_count; ++i) {
        for (j = 0; j < party_count; ++j) {
            U64_PAIR(party[j]->hostility_mask) &= ~((u64)1 << baddies[i]->hostility_group);
            U64_PAIR(baddies[i]->hostility_mask) &= ~((u64)1 << party[j]->hostility_group);
        }
    }
    object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0)
            continue;
        object->ai_packet.flags &= ~0x5800;
        object->ai_packet.opponent = NULL;
        object->ai_packet.opponent_metric = 1000000000.0f;
        object->ai_packet.nearest_opponent = NULL;
        object->ai_packet.nearest_opponent_metric = 1000000000.0f;
        object->ai_packet.target_dc = NULL;
        object->ai_packet.target_metric_e0 = 1000000000.0f;
        object->ai_packet.target_ec = NULL;
        object->ai_packet.target_metric_f0 = 1000000000.0f;
        object->hostility_mask[0] = 0;
        object->hostility_mask[1] = 0;
        object->field_eb0 = 0;
        object->field_eb4 = 0;
        object->alert_target = NULL;
    }
    party_under_cover = 0;
    alert_obj = NULL;
    alert_timer = 0.0f;
    if (StartupGameFlag1 == 0 || (VADER_ADATA != NULL && WORLD->area == VADER_ADATA) || party_cant_be_under_cover != 0)
        return;
    {
        i32 all_under_cover = 1;
        for (i = 0; i < 8; ++i) {
            GAMEOBJECT *player = (GAMEOBJECT *)Player[i];
            if (player == NULL || (player->flags_1fc_word & 1) == 0 || (player->flags_1fc_word & 0x1000) == 0 ||
                player->dead != 0 || (player->flags_ef3 & 1) != 0)
                continue;
            if (player->helmet == 5 ||
                ((FreePlay != 0 || WORLD->current_level == HUB_LDATA) &&
                 (player->character_data->model_flags & 0x204) != 0 && player->takeover_source == NULL))
                party_under_cover = 1;
            if ((player->character_data->model_flags & 0x80000) == 0)
                all_under_cover = 0;
        }
        if (all_under_cover != 0)
            party_under_cover = 1;
    }
}

i32 CanFightLikeAJedi(GAMEOBJECT *object) {
    if (CharCategory_IsCategory(object, 0) != 0)
        return 1;
    if (object->id == (u16)id_GRIEVOUS)
        return 1;
    if (object->id == (u16)id_BODYGUARD)
        return 1;
    return object->id == (u16)id_IMPERIALGUARD;
}

APIOBJECT_PC *__cdecl GameAPIOBJECTFromObjID(u8 object_id) {
    GAMEOBJECT *object;
    u32 flags;
    if ((i32)object_id < HIGHGAMEOBJECT) {
        object = &Obj[object_id];
        flags = object->flags_1fc_word;
        if ((flags & 1) == 0) {
            return NULL;
        }
        if ((flags & 0x1000) == 0 && object->ai_packet.reset_mode != 3) {
            return NULL;
        }
        if (object->dead != 0 && !(object->field_1010 > 0.0f)) {
            return NULL;
        }
        return (APIOBJECT_PC *)object;
    }
    return NULL;
}

GAMEOBJECT *__cdecl TargetGameObject(GAMEOBJECT *object, NUVEC *position, NUVEC *direction, f32 radius,
                                     f32 range_squared, u32 model_mask, i32 directional, i32 require_drawn,
                                     i32 bolt_id) {
    NUVEC aim = *direction;
    BOLTTYPE_PC *bolt = BoltType_FindByID(bolt_id, WORLD);
    u16 near_angle = TargetDeg_Near;
    u16 mid_angle = TargetDeg_Mid;
    u16 far_angle = TargetDeg_Far;
    f32 near_distance = TargetDist_Near2;
    f32 mid_distance = TargetDist_Mid2;
    f32 min_x, max_x, min_z, max_z;
    i32 arcade;
    GAMEOBJECT *target;
    GAMEOBJECT *best = NULL;
    GAMEOBJECT *previous = NULL;
    i32 i;
    if ((object->flags_1fc & 0x80) != 0 &&
        (WORLD->current_level == DEATHSTAR2BATTLEA_LDATA || WORLD->current_level == ASTEROIDCHASED_LDATA)) {
        near_angle = 0x2aaa;
        mid_angle = 0x1c71;
        far_angle = 0xe38;
        near_distance = 6.25f;
        mid_distance = 100.0f;
    }
    min_x = position->x - radius;
    min_z = position->z - radius;
    max_x = radius + position->x;
    max_z = radius + position->z;
    if (directional != 0) {
        aim = *direction;
        if (bolt != NULL && (bolt->flags_60 & 0x20000) != 0) {
            aim.y = 0.0f;
            NuVecNorm(&aim, &aim);
        }
    }
    arcade = Arcade_GetMode(NULL) == 0;
    target = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++target) {
        i8 context;
        NUVEC delta;
        f32 distance;
        if (target == object || (target->flags_1fc_word & 0x1000) == 0 || target->dead != 0 ||
            (require_drawn != 0 && target->model_draw_result == 0) || (target->flags_ef4 & 1) != 0) {
            continue;
        }
        context = target->character_context;
        if (context == 0x3b || context == 0x39 || context == 0x3c || (U64_PAIR(&CInfo[context].flags) & 0x8000) != 0) {
            continue;
        }
        if (model_mask != 0) {
            if ((target->character_data->model_flags & model_mask) == 0) {
                continue;
            }
        } else if (arcade == 0 && ((target->flags_1f8 ^ object->flags_1f8) & 0x10001) == 0) {
            continue;
        }
        if (context == 0x5f || context == 0x60) {
            continue;
        }
        if (arcade == 0 && (object->flags_1fc & 0x80) != 0 && (target->flags_ef4 & 0x800000) == 0) {
            if ((((u64)1 << target->hostility_group) & WORLD->object_sys->hostility_masks[object->hostility_group]) ==
                0) {
                continue;
            }
        }
        if (target->collision_position.x < min_x || target->collision_position.x > max_x ||
            target->collision_position.z < min_z || target->collision_position.z > max_z) {
            continue;
        }
        if (WORLD->current_level == DEATHSTARRESCUEA_LDATA && target->field_280 == 0xff && target->position.y < -1.5f &&
            GameCam->sock_position.sock_index == 2) {
            continue;
        }
        distance = NuVecDistSqr(&target->collision_position, position, &delta);
        if (distance < range_squared) {
            f32 dot;
            u16 angle;
            if (directional == 0) {
                NuVecRotateY(
                    &aim, &v001,
                    NuAtan2D(target->collision_position.x - position->x, target->collision_position.z - position->z));
            }
            if (bolt != NULL && (bolt->flags_60 & 0x20000) != 0) {
                delta.y = 0.0f;
            }
            NuVecNorm(&delta, &delta);
            dot = NuVecDot(&delta, &aim);
            angle = near_distance > distance && directional != 0 ? near_angle
                                                                 : (mid_distance > distance ? mid_angle : far_angle);
            if (dot > NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff]) {
                if (target == object->collision_target) {
                    previous = target;
                } else {
                    range_squared = distance;
                    best = target;
                }
            }
        }
    }
    return best != NULL ? best : previous;
}

void __cdecl GameObjectToCameraCode(GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG_PC *data;
    u32 mask;

    if (GameTimer.update_count != 0 && object->ai_update_distance < WORLD->current_level->blob_shadow_fade_far) {
        if (object->ai_update_distance > WORLD->current_level->blob_shadow_fade_near) {
            object->shadow_opacity =
                1.0f - (object->ai_update_distance - WORLD->current_level->blob_shadow_fade_near) /
                           (WORLD->current_level->blob_shadow_fade_far - WORLD->current_level->blob_shadow_fade_near);
        } else {
            object->shadow_opacity = 1.0f;
        }
    } else {
        object->shadow_opacity = 0.0f;
    }
    if ((u32)GameTimer.update_count > 0) {
        data = object->character_data->game_character;
        if ((data->flags_094 & 0x10) == 0) {
            if (object->dead != 0 &&
                (mask = apicharsys->char_data[((CHARACTER_MODEL_LAYERS_PC *)object->character_model)->model_id]
                            .game_character->layer_mask_dead) != 0) {
                object->layer_mask = mask;
            } else if (object->ai_update_distance > data->layer_distance_low) {
                object->layer_mask = data->layer_mask_low;
            } else if (object->ai_update_distance > data->layer_distance_medium) {
                object->layer_mask = data->layer_mask_medium;
            } else {
                object->layer_mask = data->layer_mask;
            }
            object->layer_mask |= object->extra_layer_mask;
            object->layer_mask = AdjustLayerBits(object->layer_mask, object);
        }
    }
}

f32 __cdecl GetCharacterGoalSpeed(APIOBJECT_PC *object) {
    if (object == NULL || object->ai == NULL) {
        return 0.0f;
    }
    switch (object->ai->goal_speed_mode) {
        case 0:
            return object->character_data->game_character->run_speed * FRAMETIME;
        case 1:
            return object->character_data->game_character->walk_speed * FRAMETIME;
        case 2:
            return object->character_data->game_character->tiptoe_speed * FRAMETIME;
        default:
            return 0.0f;
    }
}

i32 __cdecl KillGameObject(GAMEOBJECT *object, i32 reason, i32 unused) {
    NUVEC momentum, position;
    i32 debris[4];
    i32 part_debris;
    i32 requested_reason;
    i32 terminal_kill;
    GAMEOBJECT *rider;
    AIGROUP_PC *group;
    GAMEOBJECT *candidate;
    PART_PC *part;
    BOLT *bolt;
    i32 i;
    terminal_kill = 0;
    requested_reason = reason;
    if (reason == 5)
        reason = 4;
    if ((object->flags_1fc_word & 0x80) != 0) {
        ++AreaGlobals_PC.field_00;
        ++LevDeaths;
    }
    NuSound3ClearLoopHold(&object->upper_position, 0);
    NuSound3ClearLoopHold(&object->collision_position, 0);
    NuSound3ClearLoopHold(&object->lower_position, 0);
    Grapple_RemoveDynamic(object);
    Shards_HandleLostObj(WORLD, object);
    part_debris = -1;
    debris[0] = -1;
    debris[1] = -1;
    debris[2] = -1;
    debris[3] = -1;
    if (reason != 4) {
        if ((object->character_data->model_flags & 0x4000000) != 0) {
            debris[0] = 73;
            debris[1] = 74;
            debris[2] = 75;
        } else if (object->character_data->move_fn == Move_CANNON) {
            debris[0] = 75;
            debris[1] = 117;
        } else if (object->id == (u16)VehicleBoltId7f1910Pc) {
            debris[0] = 38;
            part_debris = 0;
        } else if (object->id == (u16)id_CLONEWALKER || object->id == (u16)id_ATST_LOWRES) {
            debris[1] = 130;
            debris[2] = 133;
            debris[3] = 132;
        } else if (object->id == GameObjectRotationAtstId) {
            debris[0] = 44;
        } else if (object->id == (u16)id_ATAT) {
            GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
            NewRumbleAllPlayers(0.7f, 0.0f, 0, 0);
        } else if (object->id == (u16)id_XWING || object->id == (u16)VehicleBoltId7f1a24Pc ||
                   object->id == (u16)VehicleBoltId7f19a4Pc || object->id == (u16)id_TIEFIGHTER ||
                   object->id == (u16)VehicleBoltId7f1a28Pc || object->id == (u16)VehicleBoltId7f196cPc ||
                   object->id == (u16)id_TIEBOMBER || object->id == (u16)VehicleBoltId7f1a2cPc ||
                   object->id == (u16)VehicleBoltId7f195cPc) {
            debris[0] = 34;
            if (WORLD->current_level == DEATHSTARRESCUEE_LDATA && object->field_280 == 0xff) {
                GameCam_Judder(GameCam, qrand() < 32768 ? -0.75f : 0.75f, 2, NULL);
                NewRumbleAllPlayers(1.0f, 0.1f, 0, 0);
            }
        } else if (object->id == (u16)VehicleBoltId7f19a8Pc) {
            debris[0] = 34;
        } else if (object->id == (u16)id_MOUSEDROID) {
            debris[0] = 69;
            part_debris = 1;
        } else if (object->id == id_DRAGBOMB) {
            GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
            NewRumbleAllPlayers(1.0f, 0.1f, 0, 0);
            AddExplosion(&object->collision_position, (f32)((f64)AreaPickupScale * (f64)0.6f), 0.5f, NULL, -1, 0x807);
            if (Cheat_IsOn(5)) {
                debris[0] = 125;
                debris[1] = 126;
            } else {
                debris[0] = 82;
                debris[1] = 83;
                debris[2] = 84;
                part_debris = 3;
            }
            PlaySfx("exp_thermalDet", &object->collision_position);
        } else if (object->id == (u16)CharacterFixupIdPc007f18c4) {
            part_debris = 5;
        } else if (object->id == (u16)VehicleBoltId7f18b4Pc) {
            debris[0] = 130;
            debris[1] = 133;
        }
        if (object->character_context != 43)
            PlayDieSfx(object);
        if ((object->flags_1fc_word & 0x80) != 0)
            DieRumble(object);
        if ((object->character_data->model_flags & 0x2000) != 0) {
            momentum.x = (f32)((f64)object->velocity.x * 0.5);
            momentum.y = (f32)((f64)object->velocity.y * 0.5);
            momentum.z = (f32)((f64)object->velocity.z * 0.5);
        } else {
            momentum = v000;
        }
        for (i = 0; i < 4; ++i) {
            if (debris[i] != -1) {
                if ((object->character_data->model_flags & 0x2000) != 0)
                    AddGameDebrisMomentum(WORLD->game_debris_sys, debris[i], &object->collision_position, &momentum,
                                          &momentum);
                else
                    AddGameDebris(WORLD->game_debris_sys, debris[i], &object->collision_position);
            }
        }
        if (part_debris != -1)
            AddPartDebris(WORLD->part_debris_sys, part_debris, &object->collision_position);
        BlowUpSnakeBody(object);
    } else {
        DestroySnakeBody(object);
    }
    if (object->character_context == 43) {
        object->character_context = -1;
        if ((object->flags_1fc_word & 0x80) != 0 &&
            (WORLD->current_level == HOTHBATTLEA_LDATA || WORLD->current_level == GUNSHIPA_LDATA))
            GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
    }
    if (reason == 4) {
        terminal_kill = 1;
        reason = 2;
    } else if (WORLD->current_level == SPEEDERCHASEA_LDATA && object->ai_packet.creature_set == 2) {
        GIZAIMESSAGE *message = CheckGizAIMessage(gizaimessagesys, "SpeedersKilled", NULL);
        if (message != NULL)
            message->value = (f32)((f64)message->value + 1.0);
        ++speeder_hitpoints_lost;
    }
    if ((object->character_data->model_flags & 0x2000) == 0) {
        object->velocity.x = 0.0f;
        object->velocity.z = 0.0f;
    } else {
        object->velocity.x = (f32)((f64)object->velocity.x * 0.5);
        object->velocity.y = (f32)(0.5 * ((f64)object->velocity.y));
        object->velocity.z = (f32)((f64)object->velocity.z * 0.5);
    }
    if (reason == 3)
        reason = 2;
    object->dead = (u8)reason;
    object->current_hp = 0;
    if (object->character_context == 7 || object->field_e33 == 1)
        object->flags_e20 |= 0x10000;
    else if (object->character_context == 6 || object->field_e33 == 2)
        object->flags_e20 = object->flags_e20 & (~0x10000);
    if (object->helmet != 0)
        LoseHelmet(object, 1, 0);
    rider = object->rider;
    Player_ClearContext(object, 1);
    if (terminal_kill != 0 && NULL != rider)
        KillGameObject(rider, 4, 0);
    ReleaseLever(object);
    Player_ResetContexts(object->ai_player_packet_pc);
    if ((object->flags_1fc_word & 0x80) != 0) {
        if (object->torpedo != NULL) {
            while (object->torpedo->count != 0) {
                AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[71].effect,
                                            &object->torpedo->pickup_positions[object->torpedo->count], 30, 0, 0);
                --object->torpedo->count;
            }
        }
        if (object->dead == 0 || (object->flags_1fc_word & 0x80) == 0 ||
            (object->character_data->model_flags & 0x2000) == 0 ||
            (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0))
            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
        if ((object->flags_1f8 & 0x40000) == 0) {
            if ((object->takeover_owner != NULL &&
                 (object->takeover_owner->rider == NULL || object == object->takeover_owner->rider))) {
                GetTakeOverPos(object->takeover_owner, &position);
                object->start_position = position;
                object->position = position;
                object->ai_update_position = position;
            } else {
                SetToLastSafePos(object);
            }
        }
        GameObjectOrigin(object);
        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
    }
    object->death_timer = 0.0f;
    if (object->dead != 2)
        object->dead = 1;
    if (object->dead != 2)
        object->dead = 1;
    if (1 == object->dead)
        object->start_position = object->position;
    switch ((i8)object->dead) {
        case 2:
            object->death_delay = 0.0f;
            break;
        default:
            object->death_delay = 0.5f;
            break;
    }
    if (object->death_delay <= 0.0f)
        object->movement_lean_angle = 0;
    if (terminal_kill == 0 &&
        AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, (char *)BeenKilledPc))
        AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, &object->ai_packet.script_processor,
                        FRAMETIME);
    group = object->ai_packet.group;
    if (group != NULL) {
        group->member_is_alive &= ~(u32)((u64)1 << object->ai_packet.group_member_index);
        group->rows[object->ai_packet.group_row].is_alive &=
            ~(u8)((u64)1 << (object->ai_packet.group_member_index - object->ai_packet.group_row * group->count_across));
        group = object->ai_packet.group;
        if (group->leader == (APIOBJECT_PC *)object) {
            group->leader = NULL;
            if ((group->flags & 4) != 0) {
                for (i = group->member_count - 1; i >= 0; --i) {
                    if (group->members[i] != NULL) {
                        candidate = (GAMEOBJECT *)group->members[i]->owner;
                        if (object != candidate && (candidate->flags_1fc_word & 1) != 0 &&
                            (candidate->flags_1fc_word & 0x1000) != 0 && candidate->dead == 0) {
                            group->leader = (APIOBJECT_PC *)candidate;
                            break;
                        }
                    }
                }
            } else {
                for (i = 0; i < object->ai_packet.group->member_count; ++i) {
                    group = object->ai_packet.group;
                    if (group->members[i] == NULL)
                        continue;
                    candidate = (GAMEOBJECT *)group->members[i]->owner;
                    if (candidate == NULL) {
                        group->members[i] = NULL;
                    } else if (object != candidate && (candidate->flags_1fc_word & 1) != 0 &&
                               (candidate->flags_1fc_word & 0x1000) != 0 && candidate->dead == 0) {
                        group->leader = (APIOBJECT_PC *)candidate;
                        break;
                    }
                }
            }
        }
    }
    object->ai_packet.flags &= ~0x5000;
    object->ai_packet.nearest_opponent = NULL;
    object->ai_packet.target_dc = NULL;
    object->ai_packet.target_ec = NULL;
    object->ai_packet.target_metric_e0 = 1000000000.0f;
    object->ai_packet.target_metric_f0 = 1000000000.0f;
    object->ai_awareness_mask = 0;
    object->ai_seen_mask = 0;
    object->field_eb4 = object->field_eb0 = 0;
    object->opponent = NULL;
    object->ai_packet.opponent = NULL;
    object->alert_target = NULL;
    object->field_ec4 = 0.0f;
    if (object->reciprocal_target != NULL) {
        object->reciprocal_target->reciprocal_target = NULL;
        object->reciprocal_target = NULL;
    }
    if (object->character_context == 71 && object->grapple != NULL) {
        ((KILL_ZIPUP_PC *)object->grapple)->runtime_flags &= ~0x100;
        object->grapple = NULL;
    }
    Batarangs_CheckLostData(object);
    SpecialMove_ReleaseVictim(object);
    ReleaseEat(object);
    ReleaseBuildIt(object, 0);
    ReleasePush(object);
    candidate = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++candidate) {
        if ((candidate->flags_1fc_word & 1) == 0 || (candidate->flags_1fc_word & 0x1000) == 0 || candidate == object)
            continue;
        if (candidate->character_context == 27 && candidate->force_target == object)
            ReleaseForce(candidate, 0);
        if (candidate->force_target == object)
            candidate->force_target = NULL;
        if (candidate->airborne_collision_target == object)
            candidate->airborne_collision_target = NULL;
    }
    ReleaseForce(object, 0);
    if (object->field_280 == 0xff) {
        bolt = Bolt;
        for (i = 0; i < 32; ++i, ++bolt)
            if (bolt->active != 0 && bolt->owner == object)
                bolt->owner = NULL;
        part = Part;
        for (i = 0; i < MAXPARTS; ++i, ++part)
            if ((part->active & 1) != 0 && part->owner == object)
                part->owner = NULL;
        if (Explosions[0].time < Explosions[0].duration && Explosions[0].owner == object)
            Explosions[0].owner = NULL;
        if (Explosions[1].time < Explosions[1].duration && Explosions[1].owner == object)
            Explosions[1].owner = NULL;
        if (Explosions[2].time < Explosions[2].duration && Explosions[2].owner == object)
            Explosions[2].owner = NULL;
        if (Explosions[3].time < Explosions[3].duration && Explosions[3].owner == object)
            Explosions[3].owner = NULL;
        if (Explosions[4].time < Explosions[4].duration && Explosions[4].owner == object)
            Explosions[4].owner = NULL;
        if (Explosions[5].time < Explosions[5].duration && Explosions[5].owner == object)
            Explosions[5].owner = NULL;
        if (Explosions[6].time < Explosions[6].duration && Explosions[6].owner == object)
            Explosions[6].owner = NULL;
        if (Explosions[7].time < Explosions[7].duration && Explosions[7].owner == object)
            Explosions[7].owner = NULL;
    }
    if (DOGFIGHTA_LDATA != NULL && WorldInfo_CurrentlyActive()->current_level == DOGFIGHTA_LDATA && BonusWinner == -1 &&
        (0x40000 & object->flags_1f8) == 0)
        ResetLevel(WorldInfo_CurrentlyActive(), (char *)EP3DogfightDiePc, 1);
    if (0 != terminal_kill && object->field_280 == 0xff) {
        AICREATURE_PC *creature;
        i32 persistent;
        i32 respawn;
        u32 limit;
        f32 delay;
        if (requested_reason == 5) {
            object->flags_1fc_word &= ~0x1000;
            object->ai_packet.reset_mode = 4;
        } else {
            persistent = object->respawnable;
            respawn = 0;
            creature = NULL;
            if ((object->flags_1f8 & 0x4000) != 0 && (object->flags_1f8 & 0x400) != 0) {
                creature = &WORLD->ai_sys->creatures[object->ai_packet.creature_index];
                respawn = object->respawn_at_origin;
                if (!persistent) {
                    if (creature->max_respawn_count != -1 && object->ai_packet.respawn_locator == NULL)
                        limit =
                            creature->min_respawn_count +
                            ((creature->max_respawn_count - creature->min_respawn_count) * ((i32)Game.difficulty - 1)) /
                                9 +
                            1;
                    else
                        limit = object->ai_packet.respawn_count + 1;
                    if (object->ai_packet.respawn_count < limit) {
                        persistent = 1;
                        respawn = 1;
                    }
                }
            }
            if (!persistent) {
                object->flags_1fc_word &= ~0x1000;
                object->ai_packet.reset_mode = 4;
            } else if (respawn && creature != NULL) {
                object->flags_1fc_word &= ~0x1000;
                object->ai_packet.reset_mode = 1;
                delay = (f32)(1.0 - ((f64)Game.difficulty - 1.0) / 9.0);
                object->ai_packet.spawn_delay = delay;
                if (creature->min_respawn_time > 0.0f && creature->max_respawn_time > 0.0f)
                    object->ai_packet.spawn_delay =
                        creature->max_respawn_time * delay + creature->min_respawn_time * (1.0f - delay);
                else
                    object->ai_packet.spawn_delay =
                        (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.0 * (f64)object->ai_packet.spawn_delay);
            } else {
                object->field_1010 = 1.0f;
            }
        }
    }
    if (object->torpedo != NULL && object->field_280 == 0xff) {
        FreeTorpedoPacket(&object->torpedo);
        object->torpedo = NULL;
    }
    if (VehicleArea != 0 && object->field_280 < 2)
        DebFreeInstantly(&FalconDebKey[object->field_280]);
    return 1;
}

/* Original-TU private globals are handed off to the single data owner. */

void __cdecl ManageGameObjects(void) {
    i32 i, j, area, alive;
    GAMEOBJECT *candidate, *other, *vehicle, *target;
    AICREATURE_PC *creature;
    AISYS_PC *system;
    AIGROUP_PC *group;
    AILOCATOR_PC *locator, *respawn_locator;
    u32 saved_flags, limit;
    i64 mask;
    GAMEOBJECT *object = Obj;
    memset(aicreature_sets_alive, 0, sizeof(aicreature_sets_alive));
    for (i = 0; i < HIGHGAMEOBJECT; ++i) {
        candidate = &object[i];
        if ((candidate->flags_1fc_word & 0x1000) != 0 && candidate->dead == 0 && candidate->ai_packet.creature_set != 0)
            ++aicreature_sets_alive[candidate->ai_packet.creature_set - 1];
    }
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & 1) == 0)
            continue;
        if ((object->flags_1f8 & 0x40000) == 0) {
            if ((object->flags_1fc_word & 0x1000) == 0) {
                if (object->ai_packet.creature_index == 0xff) {
                    if (object->ai_packet.reset_mode == 1) {
                        object->ai_packet.spawn_delay -= FRAMETIME;
                        if (object->ai_packet.spawn_delay <= 0.0f) {
                            object->flags_1fc_word |= 0x1000;
                            object->ai_packet.spawn_delay = 0.0f;
                            object->ai_packet.reset_mode = 2;
                        }
                    } else if (object->ai_packet.reset_mode != 3) {
                        if ((object->flags_1f8 & 0x4000) == 0) {
                            if (object->ai_packet.group != NULL) {
                                alive = 0;
                                for (j = 0; j < 4; ++j) {
                                    if (object->ai_packet.group->rows[j].is_alive != 0) {
                                        alive = 1;
                                        break;
                                    }
                                }
                                if (alive == 0) {
                                    DestroyAIGroup(object->ai_packet.group);
                                } else {
                                    for (j = 0; j < object->ai_packet.group->member_count; ++j) {
                                        if (object->ai_packet.group->members[j] == (APIOBJECT_PC *)object)
                                            object->ai_packet.group->members[j] = NULL;
                                    }
                                }
                            }
                            FreeTorpedoPacket(&object->torpedo);
                            RemoveGameObject(object, 1);
                        } else {
                            AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet,
                                            &object->ai_packet.script_processor, FRAMETIME);
                        }
                    }
                } else {
                    system = WORLD->ai_sys;
                    creature = &system->creatures[object->ai_packet.creature_index];
                    switch (object->ai_packet.reset_mode) {
                        case 0:
                            switch (creature->activation_type) {
                                case 0:
                                    if (Game.difficulty >= creature->activation_difficulty)
                                        ResetAICreature(object, system);
                                    else
                                        object->ai_packet.reset_mode = 4;
                                    break;
                                case 1:
                                    if (creature->activate_area != NULL) {
                                        if (player != NULL) {
                                            area = creature->activate_area - system->areas;
                                            mask = 1 << area;
                                            if ((U64_PAIR(player->ai_area_mask) & mask) != 0) {
                                                if (Game.difficulty >= creature->activation_difficulty) {
                                                    if (creature->count > 1 && creature->start_stagger > 0.0f &&
                                                        object->ai_packet.group_member_index != 0) {
                                                        object->ai_packet.spawn_delay =
                                                            (f32)(u32)object->ai_packet.group_member_index *
                                                            creature->start_stagger;
                                                        object->ai_packet.reset_mode = 1;
                                                    } else {
                                                        ResetAICreature(object, system);
                                                    }
                                                } else {
                                                    object->ai_packet.reset_mode = 4;
                                                }
                                            }
                                        }
                                    } else {
                                        creature->activation_type = 0;
                                    }
                                    break;
                                case 2:
                                    if (Game.difficulty >= creature->activation_difficulty)
                                        AIScriptProcess(system, (APIOBJECT_PC *)object, &object->ai_packet,
                                                        &object->ai_packet.script_processor, FRAMETIME);
                                    break;
                            }
                            break;
                        case 1:
                            group = object->ai_packet.group;
                            if (group != NULL && (group->flags & 2) == 0)
                                break;
                            alive = 1;
                            if (creature->activation_type == 1) {
                                area = creature->activate_area - system->areas;
                                mask = 1 << area;
                                if (player == NULL || (U64_PAIR(player->ai_area_mask) & mask) == 0 ||
                                    Game.difficulty < creature->activation_difficulty)
                                    alive = 0;
                            }
                            if (alive != 0) {
                                object->ai_packet.spawn_delay -= FRAMETIME;
                                if (object->ai_packet.spawn_delay <= 0.0f) {
                                    if (group != NULL) {
                                        locator = object->ai_packet.script_processor.locator;
                                        respawn_locator = object->ai_packet.respawn_locator;
                                        for (j = 0; j < object->ai_packet.group->member_count; ++j) {
                                            if (object->ai_packet.group->members[j] != NULL) {
                                                target = object->ai_packet.group->members[j]->owner;
                                                target->ai_packet.spawn_delay = 0.0f;
                                                target->ai_packet.script_processor.locator = locator;
                                                target->ai_packet.respawn_locator = respawn_locator;
                                                ResetAICreature(target, WORLD->ai_sys);
                                                if (object->ai_packet.group->leader == NULL)
                                                    object->ai_packet.group->leader =
                                                        object->ai_packet.group->members[j];
                                            }
                                        }
                                        object->ai_packet.group->flags &= ~8u;
                                        object->ai_packet.group->flags &= ~4u;
                                    } else {
                                        object->ai_packet.spawn_delay = 0.0f;
                                        if (object->id == (u16)id_PKDROID) {
                                            ResetAICreature(object, WORLD->ai_sys);
                                            SpawnCreatureFromCrate(object, 3.0f, 0.0f);
                                        } else {
                                            ResetAICreature(object, WORLD->ai_sys);
                                        }
                                    }
                                }
                            } else if (creature->count > 1 && creature->start_stagger > 0.0f) {
                                object->ai_packet.spawn_delay =
                                    (f32)(u32)object->ai_packet.group_member_index * creature->start_stagger;
                            }
                            break;
                    }
                }
            } else if (object->dead != 0) {
                if (object->tag_object_callback != NULL) {
                    object->tag_object_callback(object);
                    object->tag_object_callback = NULL;
                }
                if (object->field_1010 > 0.0f) {
                    object->field_1010 -= FRAMETIME;
                    if (object->field_1010 <= 0.0f) {
                        object->field_1010 = 0.0f;
                        other = NULL;
                        if (WORLD->current_level == PODSPRINTA_LDATA)
                            other = GetOtherActivePlayer(object);
                        if (other != NULL) {
                            object->ai_update_position = object->position = object->start_position = other->position;
                            GameObjectOrigin(object);
                            object->velocity = other->velocity;
                            object->field_0x276 = object->facing_angle = object->movement_facing_angle =
                                other->field_0x276;
                        } else {
                            SetToLastSafePos(object);
                            GameObjectOrigin(object);
                            object->velocity.x = object->velocity.z = 0.0f;
                            object->velocity.y = -0.1f;
                            object->field_0x276 = object->facing_angle = object->movement_facing_angle;
                        }
                        saved_flags = object->flags_1fc_bit_13;
                        ResetPlayerMoves(object);
                        object->dead = 0;
                        object->flags_1fc_bit_13 = saved_flags;
                        object->current_hp = object->field_107e;
                        object->shield_hitpoints = object->character_data->game_character->shield_hitpoints;
                        object->saved_hitpoints = 4;
                        object->flags_ef0 |= 0x80;
                        object->character_context = 0xff;
                        if (object->field_280 != 0xff)
                            object->spawn_protection_timer = 2.5f;
                        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
                        vehicle = object->takeover_owner;
                        if (vehicle != NULL && (vehicle->rider == NULL || vehicle->rider == object) &&
                            (vehicle->flags_1fc_word & 0x1000) != 0 && vehicle->character_context != 0x2b &&
                            vehicle->dead == 0) {
                            GetTakeOverPos(vehicle, &object->position);
                            TakeOverGameObject(object, object->takeover_owner, 0, 1);
                        }
                        if ((object->flags_1fc_word & 0x80) != 0 && WORLD->current_level == SPEEDERCHASEA_LDATA &&
                            disable_narrow_socks == 0)
                            GameCam_Blend(GameCam, 0.5f, 0.0f, 0);
                    } else if (WORLD->current_level == PODSPRINTA_LDATA) {
                        other = GetOtherActivePlayer(object);
                        if (other != NULL)
                            object->position = other->position;
                    }
                } else {
                    if ((object->death_timer += FRAMETIME) >= object->death_delay) {
                        object->death_timer = object->death_delay;
                        other = Obj;
                        for (j = 0; j < HIGHGAMEOBJECT; ++j, ++other) {
                            U64_PAIR(other->collision_contact_mask) &= ~U64_PAIR(object->collision_identity_mask);
                            U64_PAIR(other->hostility_mask) &= ~((u64)1 << object->hostility_group);
                            U64_PAIR(&other->field_eb0) &= ~((u64)1 << object->hostility_group);
                            U64_PAIR(other->ai_opponent_exclusion_mask) &= ~((u64)1 << object->hostility_group);
                        }
                        if ((object->flags_1f8 & 0x4000) != 0) {
                            if ((object->flags_1f8 & 0x400) == 0) {
                                object->field_1010 = 2.0f;
                            } else {
                                creature = &WORLD->ai_sys->creatures[object->ai_packet.creature_index];
                                if (object->respawnable) {
                                    if (!object->respawn_at_origin) {
                                        if (object->id == (u16)id_STAP2 && WORLD->current_level == NEGOTIATIONSC_LDATA)
                                            object->field_1010 = 5.0f;
                                        else
                                            object->field_1010 = 1.0f;
                                        continue;
                                    }
                                } else {
                                    if (creature->max_respawn_count != -1 && object->ai_packet.respawn_locator == NULL)
                                        limit = creature->min_respawn_count +
                                                (Game.difficulty - 1) *
                                                    (creature->max_respawn_count - creature->min_respawn_count) / 9 +
                                                1;
                                    else
                                        limit = object->ai_packet.respawn_count + 1;
                                    if (object->ai_packet.respawn_count >= limit) {
                                        object->flags_1fc_word &= ~0x1000;
                                        object->ai_packet.reset_mode = 4;
                                        continue;
                                    }
                                }
                                object->flags_1fc_word &= ~0x1000;
                                object->ai_packet.reset_mode = 1;
                                object->ai_packet.spawn_delay = 1.0 - ((f64)(u32)Game.difficulty - 1.0) / 9.0;
                                object->ai_packet.spawn_delay =
                                    creature->max_respawn_time * object->ai_packet.spawn_delay +
                                    creature->min_respawn_time * (1.0f - object->ai_packet.spawn_delay);
                            }
                        } else if (object->respawnable) {
                            object->field_1010 = 1.0f;
                        } else {
                            if (object->ai_packet.group != NULL) {
                                alive = 0;
                                for (j = 0; j < 4; ++j) {
                                    if (object->ai_packet.group->rows[j].is_alive != 0) {
                                        alive = 1;
                                        break;
                                    }
                                }
                                if (alive == 0) {
                                    DestroyAIGroup(object->ai_packet.group);
                                } else {
                                    for (j = 0; j < object->ai_packet.group->member_count; ++j) {
                                        if (object->ai_packet.group->members[j] == (APIOBJECT_PC *)object)
                                            object->ai_packet.group->members[j] = NULL;
                                    }
                                }
                            }
                            FreeTorpedoPacket(&object->torpedo);
                            RemoveGameObject(object, 1);
                        }
                    }
                }
            }
        } else if (nethost != 0 && (object->flags_1fc_word & 0x1000) == 0 && object->ai_packet.creature_index == 0xff &&
                   object->ai_packet.reset_mode != 1 && object->ai_packet.reset_mode != 3 &&
                   (object->flags_1f8 & 0x4000) != 0) {
            AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet,
                            &object->ai_packet.script_processor, FRAMETIME);
        }
    }
}

void __cdecl UpdateGameObjects(WORLDINFO *world) {
    i32 i;
    i32 setting;
    GAMEOBJECT *object;
    u32 interval;
    PLAYERCHARACTERCONFIG_PC *configuration;
    NUVEC delta;
    f32 distance;
    i32 tier;
    f32 frame_time;
    f32 flicker_decay;
    f32 vertical_displacement;
    f32 vertical_velocity;
    f32 elapsed;
    i32 lighting_start;
    GAMEOBJECT *dagobah_luke;
    NUVEC offset;
    i32 yoda_proximity;
    f32 target;
    i32 result;
    {
        Hub_PadSpeed[0] = Hub_PadSpeed[1] = 0.0f;
        SetPlayer();
        AIFireIntervalMul = 1.0f;
        CarWashHack = NULL;
        if (Cheat_IsOn(0x2c) != 0) {
            setting = adtab[adaptivedifficulty.level][3];
            if (setting == 1)
                AIFireIntervalMul = (f64)AIFireIntervalMul * 0.5;
            else if (setting == -1)
                AIFireIntervalMul = (f64)AIFireIntervalMul * 2.0;
        }
        GhostLightMul = SeekLinearF(GhostLightMul, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5 + 0.75),
                                    (f32)((f64)FRAMETIME * 2.0));
        if (GhostLightMul == GhostLightTargetMul)
            GhostLightTargetMul = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5 + 0.75);
        object = Obj;
        for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
            if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0)
                continue;
            if (0 != (object->flags_ef4 & 8))
                object->ai_elapsed_time = 0.0f;
            object->ai_elapsed_time += FRAMETIME;
            interval = 1;
            if (timebase_updates != 0 && (object->flags_1f8 & 0x400) != 0) {
                if ((object->portal_flags & 0x8000000) != 0 || (0x20 & object->flags_ef4) != 0 ||
                    0 != (U64_PAIR(&CInfo[(i8)object->character_context].flags) & 0x200) ||
                    object->supporting_platform_id != -1 ||
                    (object->rider != NULL && object->character_context == 0x3b)) {
                    object->flags_ef4 |= 0x10;
                    interval = 1;
                } else if (Technos_FindControllingTechno(object) != NULL) {
                    object->flags_ef4 |= 0x10;
                    interval = 1;
                } else if (object->active_trigger_set != NULL && (object->active_trigger_set->flags_low & 2) != 0) {
                    object->flags_ef4 |= 0x10;
                    interval = 1;
                } else if (object->ai_packet.group != NULL && 0 != (object->ai_packet.group->flags & 8)) {
                    object->flags_ef4 |= 0x10;
                    interval = 1;
                } else if (world->rooms_visible_ptr[object->portal_room] == 0 || object->model_draw_result == 0) {
                    interval = 10;
                    object->flags_ef4 &= ~0x10u;
                } else if (object->field_281 == 0) {
                    object->flags_ef4 |= 0x10;
                    interval = 1;
                } else {
                    distance = NuVecDistSqr(&object->collision_position, &GameCam->position, &delta);
                    configuration = object->character_data->game_character;
                    for (tier = 0; tier < 4; ++tier) {
                        if (configuration->ai_update_intervals[tier] == 0)
                            break;
                        if (configuration->ai_update_distances[tier] * configuration->ai_update_distances[tier] <
                            distance) {
                            interval = configuration->ai_update_intervals[tier];
                            break;
                        }
                    }
                    if (interval == 1)
                        object->flags_ef4 |= 0x10;
                    else
                        object->flags_ef4 &= ~0x10u;
                }
            } else {
                object->flags_ef4 |= 0x10;
            }
            if (((u32)object->hostility_group + (u32)GameTimer.update_count) % interval == 0) {
                object->flags_ef4 |= 8;
                if ((object->flags_1f8 & 0x400) != 0 && (object->flags_1f8 & 0x40000) == 0)
                    object->position = object->ai_update_position;
            } else {
                object->flags_ef4 &= ~8u;
            }
        }
    }
    GameAntinode_Update(world->game_antinode_sys);
    GameAIProcess();

    // AI movement precedes the separate player movement pass.
    object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 ||
            (object->flags_1f8 & 0x400) == 0)
            continue;
        object->flags_1fc_word ^= (((object->character_context == 0) << 14) ^ object->flags_1fc_word) & 0x4000;
        flicker_decay = object->vehicle_hit_timer - FRAMETIME;
        object->vehicle_hit_timer = flicker_decay;
        if (flicker_decay < -1.0f)
            SetFlicker(object, 0.0f);
        if (object->timer_d34 > 0.0f)
            object->timer_d34 -= FRAMETIME;
        if (object->field_d54 > 0.0f)
            object->field_d54 -= FRAMETIME;
        if ((object->flags_1f8 & 0x40000) != 0) {
            MovePlayer_NETWORK(object);
        } else if ((object->flags_ef4 & 8) != 0) {
            frame_time = FRAMETIME;
            FRAMETIME = object->ai_elapsed_time;
            object->flags_e20 &= ~0x20;
            if (object->spawn_protection_timer > 0.0f)
                object->spawn_protection_timer -= FRAMETIME;
            object->powerup_time = 0.0f;
            if (object->move_override != NULL)
                object->move_override(object);
            else if (object->movement_spline_position.spline != NULL)
                MovePlayerSpline(object);
            else
                MovePlayer(object);
            if ((object->portal_flags & 0x400000) != 0 && (object->flags_1fc_word & 4) == 0) {
                object->portal_room = -1;
                if ((object->portal_flags & 0x800000) != 0 && world->current_gscn != NULL)
                    object->portal_room = NuPortalWhichRoom(world->current_gscn, &object->collision_position);
            }
            FRAMETIME = frame_time;
        }
        if (object->character_data->game_character->weapon_action == 10) {
            if (object->model_draw_result != 0) {
                if ((object->flags_ef4 & 0x100) == 0 && tieonsfxwait <= 0.0f) {
                    PlaySfx("veh_tie_by", &object->collision_position);
                    tieonsfxwait = ((f64)qrand() * ((f64)(1.0f / 65535.0f))) * 0.5f + 0.5f;
                }
            } else if ((object->flags_ef4 & 0x100) != 0 && tieoffsfxwait <= 0.0f) {
                PlaySfx("veh_tie_by", &object->collision_position);
                tieoffsfxwait = ((f64)qrand() * ((f64)(1.0f / 65535.0f))) * 0.5f + 0.5f;
            }
        }
    }

    // Player movement has its own timers, power-up and controller processing.
    for (i = 0; i < 8; ++i) {
        object = Player[i];
        if (object == NULL || (object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0) {
            continue;
        }
        object->flags_e20 ^= (((VehicleArea != 0 && (object->field_280 == 0 || object->field_280 == 1) &&
                                (object->flags_1fc_word & 0x80) == 0)
                               << 5) ^
                              object->flags_e20) &
                             0x20;
        if (object->hud_icon_timer > 0.0f)
            object->hud_icon_timer -= FRAMETIME;
        if (object->tag_cooldown > 0.0f)
            object->tag_cooldown -= FRAMETIME;
        if (object->spawn_protection_timer > 0.0f)
            object->spawn_protection_timer -= FRAMETIME;
        PowerUp_Update(object);
        if ((object->flags_1fc_word & 0x80) != 0 && (Cheats_CheckFlags(0x1000) != 0 || object->powerup_time > 0.0f))
            RegenerateHearts(object);
        object->flags_ef4 &= ~0x10000000u;
        flicker_decay = 1.0f;
        if (object->vehicle_hit_timer > 0.0f && (object->flags_1fc_word & 0x80) != 0 && Cheat_IsOn(0x2c) != 0) {
            setting = adtab[adaptivedifficulty.level][2];
            if (setting == 1)
                flicker_decay = 2.0f;
            else if (setting == -1)
                flicker_decay = 0.5f;
        }
        object->vehicle_hit_timer -= FRAMETIME * flicker_decay;
        if (object->vehicle_hit_timer < -1.0f)
            SetFlicker(object, 0.0f);
        if (object->timer_d34 > 0.0f)
            object->timer_d34 -= FRAMETIME;
        if (object->field_d54 > 0.0f)
            object->field_d54 -= FRAMETIME;
        object->flags_1fc_word ^= (((object->character_context == 0) << 14) ^ object->flags_1fc_word) & 0x4000;
        if ((object->flags_1f8 & 0x40000) != 0) {
            MovePlayer_NETWORK(object);
        } else {
            if ((object->flags_1fc_word & 0x80) != 0) {
                if (object->pad->bound_pad != NULL) {
                    UpdateRumble(&object->pad->rumble);
                    Player_ToggleCharacter(object, 0, 1);
                }
            } else if (FreePlay != 0) {
                Player_ToggleCharacter(object, 0, 0);
            }
            if (object->move_override != NULL)
                object->move_override(object);
            else if (object->movement_spline_position.spline != NULL)
                MovePlayerSpline(object);
            else
                MovePlayer(object);
        }
        if ((object->portal_flags & 0x400000) != 0 && (object->flags_1fc_word & 4) == 0) {
            object->portal_room = -1;
            if ((object->portal_flags & 0x800000) != 0 && world->current_gscn != NULL)
                object->portal_room = NuPortalWhichRoom(world->current_gscn, &object->collision_position);
        }
    }
    CurrentSpeedOverride = -1000.0f;
    AveragePlayerCurrentSpeedMul();

    // AI-controlled characters use their accumulated AI interval for a full
    // terrain update when their script was processed. Between those updates,
    // the original advances the last resolved velocity and still animates the
    // object every frame.
    object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 ||
            (object->flags_1f8 & 0x400) == 0) {
            object->portal_flags &= ~1;
            if (object->timer_ecc > 0.0f)
                object->timer_ecc -= FRAMETIME;
            continue;
        }
        if ((object->flags_ef4 & 8) != 0) {
            frame_time = FRAMETIME;
            FRAMETIME = object->ai_elapsed_time;
            TerrainPlayer(object);

            object->ai_update_position = object->position;

            vertical_displacement = object->position.y - object->start_position.y;
            elapsed = object->ai_elapsed_time;
            if (vertical_displacement == 0.0f || elapsed == 0.0f)
                vertical_velocity = 0.0f;
            else
                vertical_velocity = vertical_displacement / elapsed;
            object->ai_vertical_velocity = vertical_velocity;
            FRAMETIME = frame_time;
        } else {
            PreResetCode(object);
            PostResetCode(object);
            if ((object->flags_1f8 & 0x40000) == 0) {
                object->position.x += object->velocity.x * FRAMETIME;
                object->position.y += object->ai_vertical_velocity * FRAMETIME;
                object->position.z += object->velocity.z * FRAMETIME;
            } else {
                GameObjectOrigin(object);
            }
        }

        AnimatePlayer(object);
        object->context_target_position = NULL;
        if (world->sock_sys != NULL && (object->portal_flags & 0x4000) != 0) {
            ComplexSockPosition(world->sock_sys, &object->position, (i8)object->sock_position.sock_index,
                                object->sock_position.segment, &object->sock_position);
            ComplexSockAngles(&object->sock_angles);
        } else {
            object->sock_position.segment = -1;
            object->sock_position.sock_index = -1;
        }
        if (WORLD->current_level != SPEEDERCHASEA_LDATA || disable_narrow_socks != 0 ||
            object->id != (u16)id_SPEEDERBIKE || object->dead == 0)
            UpdateLastSafePosition(object);
        object->collision_scale = object->dead == 1 ? 1.0f - object->death_timer / object->death_delay : 1.0f;
        if (object->collision_scale_override != 1000000000.0)
            object->collision_scale = object->collision_scale_override;
        object->origin_scale = object->character_data->model_scale * object->collision_scale;
        ScaleGameObject(object);
        GameObjectDimensions(object);
        AddSurfaceRipples(object);
        if (object->use_model_origin != 0xff) {
            ++object->use_model_origin;
        }
        NuCameraTransformScreenClip(&object->camera_screen_position, &object->collision_position, 1, NULL);
        GameObjectToCameraCode(object);
        object->portal_flags &= ~1;
        if (object->timer_ecc > 0.0f)
            object->timer_ecc -= FRAMETIME;
    }

    CurrentSpeed = 0.0f;
    for (i = 0; i < 8; ++i) {
        object = Player[i];
        if (object == NULL || (object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0) {
            continue;
        }

        if (VehicleArea != 0 || (object->character_data->model_flags & 0x2000) != 0) {
            object->reset_velocity = object->velocity;
            object->pre_terrain_speed = NuFsqrt(object->reset_velocity.x * object->reset_velocity.x +
                                                object->reset_velocity.y * object->reset_velocity.y +
                                                object->reset_velocity.z * object->reset_velocity.z);
        }
        TerrainPlayer(object);
        object->ai_update_position = object->position;
        vertical_displacement = object->position.y - object->start_position.y;
        elapsed = object->ai_elapsed_time;
        if (vertical_displacement == 0.0f || elapsed == 0.0f)
            vertical_velocity = 0.0f;
        else
            vertical_velocity = vertical_displacement / elapsed;
        object->ai_vertical_velocity = vertical_velocity;
        AnimatePlayer(object);
        object->context_target_position = NULL;
        if (VehicleArea != 0 || (object->character_data->model_flags & 0x2000) != 0) {
            object->post_terrain_speed =
                NuFsqrt(object->velocity.x * object->velocity.x + object->velocity.y * object->velocity.y +
                        object->velocity.z * object->velocity.z);
        }
        if (object->previous_sock_position != NULL)
            *object->previous_sock_position = object->sock_position;
        if (world->sock_sys != NULL &&
            ((object->flags_1fc_word & 0x80) != 0 || VehicleArea != 0 || world->current_level == UpdateSockets_LDATA)) {
            ComplexSockPosition(world->sock_sys, &object->position, (i8)object->sock_position.sock_index,
                                object->sock_position.segment, &object->sock_position);
            ComplexSockAngles(&object->sock_angles);
        } else {
            object->sock_position.segment = -1;
            object->sock_position.sock_index = -1;
            if (object->previous_sock_position != NULL) {
                object->previous_sock_position->segment = -1;
                object->previous_sock_position->sock_index = -1;
            }
        }
        if (object == Player[0]) {
            if (object->sock_position.sock_index != -1)
                CurrentSpeed = world->sock_sys->socks[(i8)object->sock_position.sock_index].current_speed;
            BaseCurrentSpeed = CurrentSpeed;
            if (CurrentSpeedOverride != -1000.0f)
                CurrentSpeed = CurrentSpeedOverride;
        }
        UpdateLastSafePosition(object);
        if (object->sock_position.sock_index != -1) {
            plr_lastpos = object->start_position;
            if (object->previous_sock_position != NULL && object->previous_sock_position->sock_index == -1) {
                *object->previous_sock_position = object->sock_position;
            }
        } else {
            plr_lastpos = object->position;
        }
        KeepOnScreen(object);
        Doors_Check(world, object);
        object->collision_scale = object->dead == 1 ? 1.0f - object->death_timer / object->death_delay : 1.0f;
        if (FreePlay != 0 && WORLD->area != NULL && WORLD->area == PODSPRINT_ADATA &&
            object->id != (u16)id_ANAKINSNEWPOD && object->id != (u16)id_ANAKINSNEWPODGREEN &&
            object->id != (u16)id_SEBULBASPOD) {
            object->collision_scale *= (f64)0.55f;
        }
        if (object->collision_scale_override != 1000000000.0)
            object->collision_scale = object->collision_scale_override;
        object->origin_scale = object->character_data->model_scale * object->collision_scale;
        ScaleGameObject(object);
        GameObjectDimensions(object);
        if ((object->flags_1fc_word & 0x80) != 0 && FadeSys.fade == 0.0f && InDoubleScoreZone(object) != 0) {
            DoubleScore |= (i32)(1u << ((u8)object->field_280 & 31));
        }
        UpdateCoinPacket((PLAYER_COIN_PACKET_PC *)object->coin_packet, (object->flags_1fc_word >> 7) & 1,
                         (i8)object->field_280);
        if ((object->flags_1fc_word & 0x80) != 0 && object->dead == 0 && object->character_context != 0x2b &&
            !(object->character_context == 0x0f && object->field_7b3 == 1)) {
            GizmoPickups_Collide(world, object, 1);
        } else {
            ResetCoinPacket(object->coin_packet);
        }
        AddSurfaceRipples(object);
        if ((object->character_data->model_flags & 0x2000) != 0 && object->dead == 0 &&
            object->model_draw_result != 0 && (object->flags_e20 & 0x20) == 0) {
            AddSurfaceDebris(object);
        }
        if (object->use_model_origin != 0xff) {
            ++object->use_model_origin;
        }
        NuCameraTransformScreenClip(&object->camera_screen_position, &object->collision_position, 1, NULL);
        GameObjectToCameraCode(object);
    }

    if (MainFrameCounters.third_frame == -1)
        lighting_start = 0;
    else
        lighting_start = (MainFrameCounters.third_frame * HIGHGAMEOBJECT) / 3;
    dagobah_luke = FindGameObject(id_LUKESKYWALKERDAGOBAH, 0, 1, 1, 0);
    for (i = 0; i < HIGHGAMEOBJECT; ++i) {
        object = &Obj[i];
        if (world->rooms_visible_ptr[object->portal_room] != 0 && (object->flags_1fc_word & 0x1001) == 0x1001) {
            yoda_proximity = StartupGameFlag1 != 0 ? TakeOverYodaSeekDistanceHack(object, dagobah_luke, &offset) : 0;
            target = (object->flags_e20 & 0x80000000) != 0 || yoda_proximity != 0 ? 1.0f : 0.0f;
            object->arrow_blend = SeekLinearF(object->arrow_blend, target, FRAMETIME + FRAMETIME);
            if (object->model_draw_result == 0)
                continue;
            if (i >= lighting_start)
                LightGameObject(object, world->rtl_set);
            if (object->arrow_blend > 0.0f) {
                NUVEC position;
                ADD_GAME_MESSAGE_PC message;
                char text[128];
                i32 joint = object->character_data->game_character->ride_locator;
                f32 height;
                f32 amplitude;
                u16 base;
                u16 phase;
                if (joint != -1 && object->character_model->points_of_interest[joint] != NULL) {
                    position = *(NUVEC *)&object->locator_matrices[joint].m30;
                } else {
                    position = object->upper_position;
                }
                height = (f32)((f64)object->bounds_half_height * 0.5);
                if (height < (f64)0.21f)
                    height = 0.21f;
                position.y = position.y + height;
                amplitude = (f32)((f64)object->bounds_radius * (f64)0.05f);
                base = (u16)((object->hostility_group & 7) << 13);
                position.x =
                    NuTrigTable[((base + (u16)(i32)(NuFmod(GameTimer.time_elapsed, 1.5f) / 1.5 * 65536.0)) >> 1) &
                                0x7fff] *
                        amplitude +
                    position.x;
                position.y =
                    NuTrigTable[((base + (u16)(i32)(NuFmod(GameTimer.time_elapsed, 1.174f) / (f64)1.174f * 65536.0)) >>
                                 1) &
                                0x7fff] *
                        (amplitude * 0.75) +
                    position.y;
                position.z =
                    NuTrigTable[((base + (u16)(i32)(NuFmod(GameTimer.time_elapsed, 1.259f) / (f64)1.259f * 65536.0)) >>
                                 1) &
                                0x7fff] *
                        amplitude +
                    position.z;
                NuStrCpy(text, TagAsciiDownPc);
                message = AddGameMsg_Default;
                message.scale = InteractionArrowScalePc;
                message.text = text;
                message.position = &position;
                phase = (u16)(i32)(((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0) * 65536.0);
                message.alpha = (u8)(i32)(((f64)NuTrigTable[phase >> 1] * 24.0 + 104.0) * object->arrow_blend);
                message.flags = 0x1083;
                message.unknown_4f = 4;
                AddGameMsg(&message);
            }
        }
    }
    CollideGameObjects(world);
    if (do_player_tag != 0) {
        result = 0;
        if (player_tag_to != NULL && player_tag_from != NULL && player_tag_to != player_tag_from &&
            (player_tag_to->flags_1fc_word & 0x1001) == 0x1001 && player_tag_to->dead == 0 &&
            (player_tag_to->tag_flags & 2) == 0 && (player_tag_from->flags_1fc_word & 0x1001) == 0x1001 &&
            player_tag_from->dead == 0 && (2 & player_tag_from->tag_flags) == 0) {
            result = TagCode(player_tag_to, player_tag_from, 0, 0, 1);
            if (result == 1) {
                GameAudio_PlaySfx(0x22, &player_tag_to->collision_position, 0, 0);
                GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
                RememberPlayerIDs(0, Player[0] != NULL ? (i16)((GAMEOBJECT *)Player[0])->id : -1,
                                  Player[1] != NULL ? (i16)((GAMEOBJECT *)Player[1])->id : -1);
                player_tag_to->hud_icon_timer = 2.0f;
                player_tag_from->hud_icon_timer = 2.0f;
            }
        }
        if (result != 2) {
            player_tag_to = NULL;
            player_tag_timer = 0.0f;
            player_tag_from = NULL;
            do_player_tag = 0;
        }
    } else if (player_tag_timer > 0.0f) {
        player_tag_timer -= FRAMETIME;
        if (player_tag_timer < 0.0f || player_tag_to == NULL || player_tag_from == NULL ||
            0x1001 != ((player_tag_to->flags_1fc_word & 0x1001)) || player_tag_to->dead != 0 ||
            (player_tag_to->tag_flags & 2) != 0 || (player_tag_from->flags_1fc_word & 0x1001) != 0x1001 ||
            player_tag_from->dead != 0 || (player_tag_from->tag_flags & 2) != 0) {
            player_tag_to = NULL;
            player_tag_timer = 0.0f;
            player_tag_from = NULL;
            do_player_tag = 0;
        }
    }
    {
        GAMEOBJECT *player0 = Player[0];
        GAMEOBJECT *player1 = Player[1];
        i32 show[2];
        show[1] = show[0] = 0;
        if ((VehicleArea != 0 || (WORLD->area != NULL && (WORLD->area->flags & 0x104) == 4) ||
             (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0)) &&
            player0 != NULL && (player0->flags_1fc_word & 0x80) != 0 && player1 != NULL &&
            (player1->flags_1fc_word & 0x80) != 0) {
            show[1] = show[0] = 1;
        } else {
            if (player0 != NULL && (player0->flags_1fc_word & 0x80) != 0 && (player0->flags_e24 & 8) != 0 &&
                player0->rider != NULL &&
                (player0->id == GameObjectRotationAtstId || player0->id == (u16)id_ATST_LOWRES ||
                 player0->id == (u16)id_ATAT)) {
                show[0] = 2;
            }
            if (player1 != NULL && (player1->flags_1fc_word & 0x80) != 0 && (player1->flags_e24 & 8) != 0 &&
                player1->rider != NULL &&
                (player1->id == GameObjectRotationAtstId || player1->id == (u16)id_ATST_LOWRES ||
                 player1->id == (u16)id_ATAT)) {
                show[1] = 2;
            }
        }
        for (i = 0; i < 2; ++i) {
            NUVEC position;
            ADD_GAME_MESSAGE_PC message;
            u16 phase;
            object = Player[i];
            if (object == NULL || show[i] == 0 || object->dead != 0) {
                continue;
            }
            if (object->hud_icon_timer > 0.0f && VehicleArea == 0) {
                continue;
            }
            if (show[i] == 2) {
                i32 joint = object->character_data->game_character->ride_locator;
                if (joint != -1 && object->character_model->points_of_interest[joint] != NULL) {
                    position = *(NUVEC *)&object->locator_matrices[joint].m30;
                } else {
                    position = object->upper_position;
                }
                position.y = object->bounds_half_height * 0.5 + position.y;
            } else if (world->current_level == DOGFIGHTA_LDATA) {
                position.x = 0.0f;
                position.y = object->character_top * object->origin_scale;
                position.z = 0.0f;
                NuVecMtxRotate(&position, &position, &object->vehicle_orientation);
                NuVecAdd(&position, &position, &object->position);
            } else {
                position.x = object->collision_position.x;
                position.y = object->bounds_half_height * 1.5 + object->collision_position.y;
                position.z = object->collision_position.z;
            }
            message = AddGameMsg_Default;
            message.scale = 0.666f;
            message.text = TagAsciiDownPc;
            message.position = &position;
            message.red = SuperCarryPlayerRgbPc[i][0];
            message.green = SuperCarryPlayerRgbPc[i][1];
            message.blue = SuperCarryPlayerRgbPc[i][2];
            phase = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0);
            message.flags = 0x83;
            message.unknown_4f = 4;
            message.alpha = (u8)(i32)((f64)NuTrigTable[phase >> 1] * 48.0 + 80.0);
            AddGameMsg(&message);
        }
    }
    if (MissionSys != NULL && Mission_Active(MissionSys) != NULL && MissionSys->active_mission != NULL) {
        CheckMissionEnd(MissionSys);
    }
}

/* Preserve existing GAMEOBJECT field declarations/assertions:
 * level_object_index signedWORD1070, flags_1fc BYTE1fc,
 * character_data pointer54 -> game_character pointer24 -> field_028 f32.
 * No new aggregate/type claim in this private source access view.
 */

#include <string.h>

#include <string.h>

GAMEOBJECT *__cdecl AddDynamicCreature(i32 model, NUVEC *position, i32 angle, char *script_name,
                                       AIPATHINFO_PC *path_info, AIGROUP_PC *group, i32 set_on_surface,
                                       NUGSPLINE *spline, NUVEC *spline_offset, i32 spline_mode, i32 creature_set) {
    GAMEOBJECT *object;
    APIOBJECT_PC *api;
    u32 flags;
    NUVEC point, offset;
    u16 yaw, pitch;
    AIROW_PC *row;
    u32 column;
    if (position == NULL && spline == NULL)
        return NULL;
    if (NOAICREATURES != 0 && model != (i16)id_DRAGBOMB)
        return NULL;
    if (Arcade_GetMode(NULL) == 0 || (u32)model > 0x153 || apicharsys->playermodelids[model] == -1)
        return NULL;
    object = AddCreature(model, 0);
    if (object != NULL) {
        api = (APIOBJECT_PC *)object;
        object->flags_1f8 |= 0x400;
        flags = apicharsys->char_data[model].model_flags;
        if ((flags & 0x200) != 0)
            object->flags_1f8 |= 4;
        else if ((flags & 4) != 0)
            object->flags_1f8 |= 1;
        object->flags_1044 |= 1;
        if ((apicharsys->char_data[model].model_flags & 0x1000) != 0)
            object->flags_1044 |= 4;
        api->view_distance = apicharsys->char_data[model].game_character->viewdistance;
        api->hear_distance = apicharsys->char_data[model].game_character->heardistance;
        api->max_view_height = apicharsys->char_data[model].game_character->maxviewheight;
        api->min_view_height = apicharsys->char_data[model].game_character->minviewheight;
        object->ai_packet.flags &= ~0x5000;
        object->portal_flags &= ~0x801;
        object->ai_packet.nearest_opponent = NULL;
        object->ai_packet.antinode_timer = 0.0f;
        object->ai_packet.target_dc = NULL;
        object->ai_packet.target_metric_e0 = 1000000000.0f;
        object->ai_packet.opponent = NULL;
        object->ai_packet.target_ec = NULL;
        object->ai_packet.target_metric_f0 = 1000000000.0f;
        object->ai_awareness_mask = 0;
        object->ai_seen_mask = 0;
        object->field_eb0 = 0;
        object->field_ec4 = 0.0f;
        object->field_eb4 = 0;
        object->timer_ecc = 0.0f;
        object->alert_target = NULL;
        InitPlayerAI(object);
        if (spline != NULL) {
            InitSplinePosition(&object->movement_spline_position, spline, 0.0f, spline_mode);
            PointAlongSpline(object->movement_spline_position.spline, object->movement_spline_position.along, &point,
                             &yaw, &pitch, (i8)object->movement_spline_position.looping);
            object->field_0x276 = object->facing_angle = object->movement_facing_angle = yaw;
            object->pitch_angle = (u16)-pitch;
            if (spline_offset != NULL) {
                object->movement_spline_offset = *spline_offset;
                if (object->movement_spline_offset.x != 0.0f || object->movement_spline_offset.y != 0.0f ||
                    object->movement_spline_offset.z != 0.0f) {
                    NuVecRotateX(&offset, &object->movement_spline_offset, object->pitch_angle);
                    NuVecRotateY(&offset, &offset, object->field_0x276);
                    NuVecAdd(&point, &point, &offset);
                }
            }
            object->position = point;
        } else {
            object->position = *position;
            object->movement_facing_angle = object->facing_angle = object->field_0x276 = (u16)angle;
            if (group != NULL) {
                AddToAIGroup(group, api);
                offset.x = ((object->ai_packet.group_column + 1) / 2) * group->x_spacing;
                if ((object->ai_packet.group_column & 1) != 0)
                    offset.x = -offset.x;
                offset.y = 0.0f;
                offset.z = -group->z_spacing * object->ai_packet.group_row;
                NuVecRotateY(&offset, &offset, angle);
                NuVecAdd(&object->position, &object->position, &offset);
            }
        }
        ResetPlayerMoves(object);
        object->collision_position = object->position;
        object->initial_position = object->collision_position;
        plr_lastpos = object->collision_position;
        object->start_position = object->collision_position;
        object->velocity = v000;
        GetTopBot(object);
        GameObjectDimensions(object);
        ResetRumble(&object->pad->rumble);
        ResetLights(&object->position, &object->lighting_packet, WORLD->rtl_set);
        object->sock_position.sock_index = -1;
        object->sock_position.segment = -1;
        if (WORLD->sock_sys != NULL) {
            if (Door_NextSock != -1 && WORLD->sock_sys->socks[Door_NextSock].valid != 0)
                complexsockposition_forcesock = Door_NextSock;
            ComplexSockPosition(WORLD->sock_sys, &object->position, (i8)object->sock_position.sock_index,
                                (i16)object->sock_position.segment, &object->sock_position);
            ComplexSockAngles(&object->sock_angles);
        }
        NuVecRotateYValZ(&object->facing_direction, 1.0f, object->field_0x276);
        object->model_draw_result = 1;
        object->use_model_origin = 0;
        object->origin_joint_ready = 0;
        object->flags_ef0 &= ~0x40000;
        if (spline == NULL) {
            InitSurfaceInfo(object);
            if (set_on_surface != 0)
                SetObjOnSurface(object, 0);
        }
        PortalGameObject(object, 1, 1, -1, WORLD->current_gscn);
        if (reset_restart != 0) {
            object->current_hp = object->field_107e = object->character_data->game_character->hitpoints;
            object->shield_hitpoints = object->character_data->game_character->shield_hitpoints;
            object->vehicle_secondary_hp = 4;
        }
        object->flags_1fc_word &= ~4;
        object->collision_scale = 1.0f;
        object->dead = 0;
        object->character_context = -1;
        object->effect_flags = 0;
        object->flags_1f8 |= 0x100;
        object->flags_1f8 ^= 0x100;
        memset(&object->ai_packet.path_info, 0, sizeof(object->ai_packet.path_info));
        object->ai_packet.inside_path_node = -1;
        object->ai_packet.current_route = 0xff;
        object->ai_packet.next_route = 0;
        if (spline == NULL) {
            if (path_info != NULL) {
                AISysCharacterSetPath(&object->ai_packet, path_info->path);
                AISysCharacterSetPathCnx(&object->ai_packet, &object->position, path_info->connection,
                                         path_info->direction);
            }
            if (object->ai_packet.path_info.connection == NULL)
                AISysGetCharacterPathPos(WORLD->ai_sys, api, &object->ai_packet, 0xff, (i8)object->field_281);
        }
        AIScriptProcessorInit(WORLD->ai_sys, &object->ai_packet, &object->ai_packet.script_processor, NULL, script_name,
                              NULL, 1, NULL, NULL);
        object->previous_floor_height = 2000000.0f;
        if (object->ai_packet.group != NULL) {
            object->ai_packet.group->member_is_alive |= (u32)((u64)1 << object->ai_packet.group_member_index);
            row = &object->ai_packet.group->rows[object->ai_packet.group_row];
            column = object->ai_packet.group_member_index -
                     object->ai_packet.group_row * object->ai_packet.group->count_across;
            row->is_alive |= (u8)((u64)1 << column);
            if (object->ai_packet.group_column == 0) {
                row->position = object->position;
                row->path_info = object->ai_packet.path_info;
                row->rotation = object->field_0x276;
                row->next_connection = NULL;
                row->flags &= ~2;
            }
        }
        PreResetCode(object);
        PostResetCode(object);
        GameObjectOrigin(object);
        object->ai_update_position = object->last_safe_position = object->position;
        if ((u32)(creature_set - 1) <= 15) {
            object->ai_packet.creature_set = (u8)creature_set;
            ++aicreature_sets_alive[creature_set - 1];
        }
    }
    return object;
}

/* Bitwise absolute value through a float temporary, as retail does. */

#define INTERACTION_GEOMETRY(p) ((GAMEOBJECT *)(p))
// Copies the antinode timer and its 0x8000 flag from one creature's packet to the other's.
#define INTERACTION_COPY_ANTINODE(source, target)                                                                      \
    (target)->ai->antinode_timer = (source)->ai->antinode_timer;                                                       \
    (target)->ai->flags ^= ((source)->ai->flags ^ (target)->ai->flags) & 0x8000
typedef void(__cdecl *CHECK_ANTINODE_FN_PC)(APIOBJECT_PC *, AIANTINODE_PC *, NUVEC *, f32);
/* The genuine callback definitions and initializer remain in the original aisys owner. */
extern CHECK_ANTINODE_FN_PC checkantinodefns[3];

static void CreatureInteractionOrbitPc(APIOBJECT_PC *object, NUVEC *difference, APIOBJECT_PC *centre, f32 radius) {
    f32 squared_distance, fraction;
    i32 angle, turn, limit;
    squared_distance = difference->x * difference->x + difference->z * difference->z;
    if (squared_distance < (f64)radius * radius) {
        difference->x = object->position.x - centre->position.x;
        difference->z = object->position.z - centre->position.z;
        angle = NuAtan2D(difference->x, difference->z);
        difference->x = object->ai->movement_destination.x - centre->position.x;
        difference->z = object->ai->movement_destination.z - centre->position.z;
        turn = NuAngSub(NuAtan2D(difference->x, difference->z), angle);
        fraction = ai_moveradius / radius;
        limit = (i32)((f64)fraction * AIPathAngularScale);
        if (turn > limit)
            turn = limit;
        else if (turn < -limit)
            turn = -limit;
        angle = NuAngAdd(angle, turn);
        difference->x = 0.0f;
        difference->y = 0.0f;
        difference->z = radius;
        NuVecRotateY(difference, difference, angle);
        object->ai->movement_position.x = centre->position.x + difference->x;
        object->ai->movement_position.y = object->ai->movement_destination.y;
        object->ai->movement_position.z = centre->position.z + difference->z;
    }
}
void __cdecl GameCreatureInteraction2D(AISYS_PC *system, i32 object_count, APIOBJECT_PC **objects, i32 *immovable,
                                       f32 delta_time) {
    i32 index, first_index, second_index, first_fixed, second_fixed;
    APIOBJECT_PC *first, *second;
    NUVEC difference;
    f32 radius, minimum;
    for (index = 0; index < object_count; ++index) {
        first = objects[index];
        first->ai->antinode_timer -= delta_time;
        if (first->ai->antinode_timer < 0.0f)
            first->ai->antinode_timer = 0.0f;
    }
    for (first_index = 0; first_index < object_count - 1; ++first_index) {
        first = objects[first_index];
        for (second_index = first_index + 1; second_index < object_count; ++second_index) {
            second = objects[second_index];
            radius = first->ai->mover_height + second->ai->mover_height;
            difference.x = first->ai->movement_position.x - second->ai->movement_position.x;
            if (radius < difference.x || (minimum = -radius) > difference.x)
                continue;
            difference.z = first->ai->movement_position.z - second->ai->movement_position.z;
            if (difference.z > radius || difference.z < minimum)
                continue;
            if (INTERACTION_GEOMETRY(first)->collision_min.y > INTERACTION_GEOMETRY(second)->collision_max.y ||
                INTERACTION_GEOMETRY(second)->collision_min.y > INTERACTION_GEOMETRY(first)->collision_max.y)
                continue;
            if (first->collision_link == second || second->collision_link == first)
                continue;
            if (immovable != NULL) {
                first_fixed = immovable[first_index];
                second_fixed = immovable[second_index];
            } else {
                first_fixed = 0;
                second_fixed = 0;
            }
            if (first->flags_low & 0x80)
                first_fixed = 1;
            if (second->flags_low & 0x80)
                second_fixed = 1;
            if (second->ai_collision_priority_pc < first->ai_collision_priority_pc) {
                first_fixed = 1;
                if (second_fixed)
                    continue;
            } else if (second->ai_collision_priority_pc > first->ai_collision_priority_pc) {
                second_fixed = 1;
                if (first_fixed)
                    continue;
            } else {
                continue;
            }
            if (!(NuFsqrt(difference.x * difference.x + difference.z * difference.z) < radius))
                continue;
            difference.y = 0.0f;
            if (first_fixed) {
                if (first->ai_collision_priority_pc > second->ai_resolved_collision_priority_pc) {
                    second->ai_resolved_collision_priority_pc = first->ai_collision_priority_pc;
                    difference.x = -difference.x;
                    difference.z = -difference.z;
                    CreatureInteractionOrbitPc(second, &difference, first, radius);
                }
            } else if (second_fixed) {
                if (second->ai_collision_priority_pc > first->ai_resolved_collision_priority_pc) {
                    first->ai_resolved_collision_priority_pc = second->ai_collision_priority_pc;
                    CreatureInteractionOrbitPc(first, &difference, second, radius);
                }
            } else {
                first->ai_resolved_collision_priority_pc = second->ai_collision_priority_pc;
                difference.x = -difference.x;
                difference.z = -difference.z;
                CreatureInteractionOrbitPc(second, &difference, first, radius);
            }
            if (first->ai->antinode_timer > antinode_time && second->ai->antinode_timer > antinode_time) {
                if (first->ai->antinode_timer > second->ai->antinode_timer) {
                    INTERACTION_COPY_ANTINODE(first, second);
                } else {
                    INTERACTION_COPY_ANTINODE(second, first);
                }
            } else if (second->ai->antinode_timer > antinode_time) {
                INTERACTION_COPY_ANTINODE(second, first);
            } else if (first->ai->antinode_timer > 0.0f) {
                INTERACTION_COPY_ANTINODE(first, second);
            }
            if ((first->motion_flags ^ second->motion_flags) & 1) {
                first->ai->flags |= 0x4000;
                second->ai->flags |= 0x4000;
            }
        }
    }
    AISysCreatureAntinodeInteraction(system, object_count, objects, immovable);
}
typedef struct OPPONENT_PACKET_PC {
    u8 unknown000[0xd4];
    APIOBJECT_PC *nearest_opponent_object;
    f32 nearest_opponent_metric;
    APIOBJECT_PC *pending_nearest_opponent;
    f32 pending_nearest_metric;
    APIOBJECT_PC *opponent_object;
    f32 opponent_metric;
    APIOBJECT_PC *pending_opponent;
    f32 pending_opponent_metric;
    u8 unknown0f4[0xf4];
    u32 flags;
} OPPONENT_PACKET_PC;
STATIC_ASSERT(offsetof(OPPONENT_PACKET_PC, nearest_opponent_object) == 0xd4);
STATIC_ASSERT(offsetof(OPPONENT_PACKET_PC, pending_nearest_opponent) == 0xdc);
STATIC_ASSERT(offsetof(OPPONENT_PACKET_PC, opponent_object) == 0xe4);
STATIC_ASSERT(offsetof(OPPONENT_PACKET_PC, pending_opponent) == 0xec);
STATIC_ASSERT(offsetof(OPPONENT_PACKET_PC, flags) == 0x1e8);
typedef struct OPPONENT_OBJECT_PC {
    u8 unknown000[0x230];
    f32 view_distance;
    u8 unknown234[0xc];
    f32 visibility_range_extension;
    u8 unknown244[0x64];
    u64 awareness;
} OPPONENT_OBJECT_PC;
STATIC_ASSERT(offsetof(OPPONENT_OBJECT_PC, visibility_range_extension) == 0x240);
STATIC_ASSERT(offsetof(OPPONENT_OBJECT_PC, awareness) == 0x2a8);
typedef struct OPPONENT_GAME_PC {
    u8 unknown000[0xeb0];
    u64 seen;
    u64 exclusion;
    APIOBJECT_PC *alert_target;
    f32 alert_timer;
} OPPONENT_GAME_PC;
STATIC_ASSERT(offsetof(OPPONENT_GAME_PC, seen) == 0xeb0);
STATIC_ASSERT(offsetof(OPPONENT_GAME_PC, exclusion) == 0xeb8);
STATIC_ASSERT(offsetof(OPPONENT_GAME_PC, alert_target) == 0xec0);
STATIC_ASSERT(offsetof(OPPONENT_GAME_PC, alert_timer) == 0xec4);
typedef struct OPPONENT_SYS_PC {
    u8 unknown000[0x1385];
    u8 goody_cursor;
    u16 assignment_flags;
} OPPONENT_SYS_PC;
STATIC_ASSERT(offsetof(OPPONENT_SYS_PC, goody_cursor) == 0x1385);
STATIC_ASSERT(offsetof(OPPONENT_SYS_PC, assignment_flags) == 0x1386);
typedef struct OPPONENT_LOS_PC {
    u32 object_size;
    GAMEOBJECT *objects;
    u64 line_of_sight[64];
} OPPONENT_LOS_PC;
STATIC_ASSERT(offsetof(OPPONENT_LOS_PC, line_of_sight) == 8);
#define OPPONENT_PACKET(p) ((OPPONENT_PACKET_PC *)(p))
#define OPPONENT_OBJECT(p) ((OPPONENT_OBJECT_PC *)(p))
#define OPPONENT_GAME(p) ((OPPONENT_GAME_PC *)(p))
#define OPPONENT_SYS(p) ((OPPONENT_SYS_PC *)(p))
#define OPPONENT_LOS(p) ((OPPONENT_LOS_PC *)(p))

i32 OpponentCanAcquirePc(APIOBJECT_PC *observer, APIOBJECT_PC *target, f32 distance, u64 awareness, f32 dx, f32 dz) {
    f32 facing_dot;
    if (party_under_cover != 0 && ((observer->motion_flags & 0x400) == 0 || (target->motion_flags & 0x400) == 0))
        return 0;
    if (awareness & ((u64)1 << target->object_id))
        return 1;
    if ((OPPONENT_LOS(WORLD->object_sys)->line_of_sight[observer->object_id] & ((u64)1 << target->object_id)) == 0)
        return 0;
    if (observer->flags_low & 0x800)
        return 1;
    facing_dot = ((GAMEOBJECT *)observer->owner)->facing_direction.x * dx +
                 ((GAMEOBJECT *)observer->owner)->facing_direction.z * dz;
    if (facing_dot >= 0.0f)
        return 1;
    if (distance < observer->hear_distance && (((GAMEOBJECT *)target->owner)->portal_flags & 0x800) != 0)
        return 1;
    return 0;
}

void GameCreatureOpponentSelection(AISYS_PC *system, i32 count, APIOBJECT_PC **objects, i32 goody_count,
                                   APIOBJECT_PC **goodies, i32 baddy_count, APIOBJECT_PC **baddies, u64 awareness,
                                   f32 unused) {
    i32 i, j;
    f32 distance;
    i32 complete;
    APIOBJECT_PC *object;
    i32 budget;
    APIOBJECT_PC *goody;
    f32 best_distance;
    APIOBJECT_PC *best_baddy;
    APIOBJECT_PC *baddy;
    i32 have_distance;
    GAMEOBJECT *baddy_object;
    u16 baddy_id;
    GAMEOBJECT *goody_object;
    OPPONENT_PACKET_PC *packet;
    f32 metric;
    u64 bit;
    u64 known;
    NUVEC difference = {0.0f, 0.0f, 0.0f};
    distance = 0.0f;
    budget = 32;
    complete = 0;
    if (system == NULL || goody_count == 0)
        return;
    if (baddy_count == 0)
        complete = 1;
    if (alert_obj != NULL && (alert_obj->flags_1fc_word & 0x1000) != 0 && (alert_obj->flags_1f8 & 5) == 0 &&
        (alert_obj->character_data->game_character->flags_090 & 0x8000) == 0) {
        for (i = 0; i < baddy_count; ++i) {
            object = baddies[i];
            if ((((u64)1 << alert_obj->hostility_group) & OPPONENT_OBJECT(object)->awareness) != 0) {
                OPPONENT_GAME(((GAMEOBJECT *)object->owner))->alert_target = (APIOBJECT_PC *)alert_obj;
                OPPONENT_GAME(((GAMEOBJECT *)object->owner))->alert_timer = 5.0f;
            } else if (WORLD->rooms_visible_ptr[((GAMEOBJECT *)object->owner)->portal_room] != 0) {
                difference.x = alert_pos.x - ((GAMEOBJECT *)object)->collision_position.x;
                if (difference.x < object->hear_distance) {
                    difference.z = alert_pos.z - ((GAMEOBJECT *)object)->collision_position.z;
                    if (difference.z < object->hear_distance) {
                        difference.y = alert_pos.y - ((GAMEOBJECT *)object)->collision_position.y;
                        if (difference.y < object->max_view_height && difference.y > object->min_view_height) {
                            distance = difference.x * difference.x + difference.z * difference.z;
                            if (!(distance < object->hear_distance * object->hear_distance))
                                continue;
                            if ((((u64)1 << alert_obj->hostility_group) &
                                 OPPONENT_LOS(WORLD->object_sys)->line_of_sight[object->object_id]) != 0 ||
                                ((f64)alert_obj->collision_position.x -
                                 (f64)((GAMEOBJECT *)object)->collision_position.x) *
                                            ((f64)alert_obj->collision_position.x -
                                             (f64)((GAMEOBJECT *)object)->collision_position.x) +
                                        ((f64)alert_obj->collision_position.z -
                                         (f64)((GAMEOBJECT *)object)->collision_position.z) *
                                            ((f64)alert_obj->collision_position.z -
                                             (f64)((GAMEOBJECT *)object)->collision_position.z) >
                                    (f64)object->view_distance * (f64)object->view_distance) {
                                OPPONENT_GAME(((GAMEOBJECT *)object->owner))->alert_target = (APIOBJECT_PC *)alert_obj;
                                OPPONENT_OBJECT(object)->awareness |= (u64)1 << alert_obj->hostility_group;
                                awareness |= OPPONENT_OBJECT(object)->awareness;
                                OPPONENT_GAME(((GAMEOBJECT *)object->owner))->alert_timer = 5.0f;
                            }
                        }
                    }
                }
            }
        }
        alert_timer -= FRAMETIME;
        if (alert_timer <= 0.0f)
            alert_obj = NULL;
    }
    if (OPPONENT_SYS(system)->goody_cursor >= goody_count)
        OPPONENT_SYS(system)->goody_cursor = 0;
    // Pending choices survive budget exhaustion. Publish only after a pass
    // finishes without changing the baddies' assignments.
    while (!complete) {
        while (OPPONENT_SYS(system)->goody_cursor < goody_count && budget > 0) {
            best_distance = 1.0e9f;
            goody = goodies[OPPONENT_SYS(system)->goody_cursor];
            best_baddy = NULL;
            i = 0;
            if (baddy_count > 0) {
                budget -= baddy_count;
                do {
                    baddy = baddies[i];
                    have_distance = 0;
                    if (baddy == goody || OPPONENT_PACKET(baddy->ai)->pending_opponent == goody)
                        continue;
                    if (((GAMEOBJECT *)baddy->owner)->flags_ef0 & 2) {
                        OPPONENT_OBJECT(goody)->awareness &= ~((u64)1 << baddy->object_id);
                    } else if (OPPONENT_OBJECT(goody)->awareness & ((u64)1 << baddy->object_id)) {
                        distance = NuVecDist(&baddy->position, &goody->position, &difference);
                        have_distance = 1;
                        if (distance >=
                            OPPONENT_OBJECT(goody)->view_distance + OPPONENT_OBJECT(baddy)->visibility_range_extension)
                            OPPONENT_OBJECT(goody)->awareness &= ~((u64)1 << baddy->object_id);
                    } else if (OPPONENT_LOS(WORLD->object_sys)->line_of_sight[goody->object_id] &
                               ((u64)1 << baddy->object_id)) {
                        distance = NuVecDist(&baddy->position, &goody->position, &difference);
                        have_distance = 1;
                        if (OpponentCanAcquirePc(goody, baddy, distance, awareness, difference.x, difference.z)) {
                            OPPONENT_OBJECT(goody)->awareness |= (u64)1 << baddy->object_id;
                            awareness |= OPPONENT_OBJECT(goody)->awareness;
                        }
                    }
                    if (((GAMEOBJECT *)goody->owner)->flags_ef0 & 2) {
                        OPPONENT_OBJECT(baddy)->awareness &= ~((u64)1 << goody->object_id);
                    } else if (OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->alert_target == goody) {
                        OPPONENT_OBJECT(baddy)->awareness |= (u64)1 << goody->object_id;
                        awareness |= OPPONENT_OBJECT(baddy)->awareness;
                    } else if (OPPONENT_OBJECT(baddy)->awareness & ((u64)1 << goody->object_id)) {
                        if (!have_distance)
                            distance = NuVecDist(&baddy->position, &goody->position, &difference);
                        have_distance = 1;
                        if (distance >=
                            OPPONENT_OBJECT(baddy)->view_distance + OPPONENT_OBJECT(goody)->visibility_range_extension)
                            OPPONENT_OBJECT(baddy)->awareness &= ~((u64)1 << goody->object_id);
                    } else if (OPPONENT_LOS(WORLD->object_sys)->line_of_sight[baddy->object_id] &
                               ((u64)1 << goody->object_id)) {
                        if (!have_distance)
                            distance = NuVecDist(&baddy->position, &goody->position, &difference);
                        have_distance = 1;
                        if (OpponentCanAcquirePc(baddy, goody, distance, awareness, -difference.x, -difference.z)) {
                            OPPONENT_OBJECT(baddy)->awareness |= (u64)1 << goody->object_id;
                            awareness |= OPPONENT_OBJECT(baddy)->awareness;
                        }
                    }
                    if (!(OPPONENT_OBJECT(goody)->awareness & ((u64)1 << baddy->object_id)) &&
                        !(OPPONENT_OBJECT(baddy)->awareness & ((u64)1 << goody->object_id)))
                        continue;
                    if (!have_distance)
                        distance = NuVecDist(&baddy->position, &goody->position, &difference);
                    baddy_object = ((GAMEOBJECT *)baddy->owner);
                    baddy_id = baddy_object->id;
                    if (baddy_id == (u16)id_JAWA || baddy_id == (u16)id_UGNAUGHT) {
                        if (!ZapTarget(((GAMEOBJECT *)goody->owner)))
                            OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->exclusion |= (u64)1 << goody->object_id;
                    } else if ((goody->character_data->model_flags & 0x80000) ||
                               ((((u8 *)&baddy_object->portal_flags)[3] & 1) &&
                                ((GAMEOBJECT *)goody->owner) != player && ((GAMEOBJECT *)goody->owner) != player2) ||
                               (((GAMEOBJECT *)goody->owner)->id == id_DRAGBOMB && baddy_id != (u16)id_ATAT)) {
                        OPPONENT_GAME(baddy_object)->exclusion |= (u64)1 << goody->object_id;
                    }
                    if (!(OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->exclusion & ((u64)1 << goody->object_id))) {
                        if (OPPONENT_PACKET(baddy->ai)->pending_nearest_metric > distance) {
                            OPPONENT_PACKET(baddy->ai)->pending_nearest_opponent = goody;
                            OPPONENT_PACKET(baddy->ai)->pending_nearest_metric = distance;
                        }
                        if (OPPONENT_PACKET(baddy->ai)->opponent_object == goody)
                            metric = distance - OpponentSelectionHysteresisPc;
                        else
                            metric = distance;
                        if (OPPONENT_OBJECT(baddy)->awareness & ((u64)1 << goody->object_id)) {
                            if (OPPONENT_PACKET(baddy->ai)->pending_opponent != NULL) {
                                if ((goody->flags_low & 0x80) &&
                                    (((GAMEOBJECT *)baddy->owner)->portal_flags & 0x40000000)) {
                                    if (!(OPPONENT_PACKET(baddy->ai)->pending_opponent->flags_low & 0x80) ||
                                        metric < OPPONENT_PACKET(baddy->ai)->pending_opponent_metric) {
                                        best_distance = distance;
                                        best_baddy = baddy;
                                    }
                                }
                            } else if (!(OPPONENT_PACKET(baddy->ai)->flags & 0x20000) ||
                                       (OPPONENT_LOS(WORLD->object_sys)->line_of_sight[baddy->object_id] &
                                        ((u64)1 << goody->object_id)) ||
                                       OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->alert_target == goody) {
                                if (metric < best_distance) {
                                    best_distance = distance;
                                    best_baddy = baddy;
                                }
                            }
                        }
                    }
                    if (((GAMEOBJECT *)baddy->owner)->id == (u16)id_JAWA ||
                        ((GAMEOBJECT *)baddy->owner)->id == (u16)id_UGNAUGHT ||
                        (baddy->character_data->model_flags & 0x80000))
                        OPPONENT_GAME(((GAMEOBJECT *)goody->owner))->exclusion |= (u64)1 << baddy->object_id;
                    if (!(OPPONENT_GAME(((GAMEOBJECT *)goody->owner))->exclusion & ((u64)1 << baddy->object_id))) {
                        if (OPPONENT_PACKET(goody->ai)->pending_nearest_metric > distance) {
                            OPPONENT_PACKET(goody->ai)->pending_nearest_opponent = baddy;
                            OPPONENT_PACKET(goody->ai)->pending_nearest_metric = distance;
                        }
                        packet = OPPONENT_PACKET(goody->ai);
                        if (packet->opponent_object == baddy)
                            metric = distance - OpponentSelectionHysteresisPc;
                        else
                            metric = distance;
                        if ((OPPONENT_OBJECT(goody)->awareness & ((u64)1 << baddy->object_id)) &&
                            (!(packet->flags & 0x20000) ||
                             (OPPONENT_LOS(WORLD->object_sys)->line_of_sight[goody->object_id] &
                              ((u64)1 << baddy->object_id)))) {
                            if (metric < packet->pending_opponent_metric ||
                                (packet->pending_opponent != NULL &&
                                 ((GAMEOBJECT *)packet->pending_opponent->owner) != NULL &&
                                 ((GAMEOBJECT *)packet->pending_opponent->owner)->character_context == 0x5a &&
                                 ((GAMEOBJECT *)baddy->owner)->character_context != 0x5a)) {
                                packet->pending_opponent = baddy;
                                OPPONENT_PACKET(goody->ai)->pending_opponent_metric = distance;
                            }
                        }
                    }
                } while (++i < baddy_count);
                {
                    if (best_baddy != NULL && OPPONENT_PACKET(best_baddy->ai)->pending_opponent != goody) {
                        OPPONENT_PACKET(best_baddy->ai)->pending_opponent = goody;
                        OPPONENT_PACKET(best_baddy->ai)->pending_opponent_metric = best_distance;
                        OPPONENT_SYS(system)->assignment_flags |= 2;
                        if (!(OPPONENT_GAME(((GAMEOBJECT *)goody->owner))->exclusion &
                              ((u64)1 << best_baddy->object_id)) &&
                            (OPPONENT_OBJECT(goody)->awareness & ((u64)1 << best_baddy->object_id))) {
                            if (!(OPPONENT_PACKET(goody->ai)->flags & 0x1000) ||
                                OPPONENT_PACKET(goody->ai)->pending_opponent_metric > best_distance) {
                                OPPONENT_PACKET(goody->ai)->pending_opponent = best_baddy;
                                OPPONENT_PACKET(goody->ai)->pending_opponent_metric = best_distance;
                                OPPONENT_PACKET(goody->ai)->flags |= 0x1000;
                            }
                        }
                    }
                }
            }
            ++OPPONENT_SYS(system)->goody_cursor;
            if (OPPONENT_SYS(system)->goody_cursor >= goody_count) {
                OPPONENT_SYS(system)->goody_cursor = 0;
                if (OPPONENT_SYS(system)->assignment_flags & 2) {
                    OPPONENT_SYS(system)->assignment_flags &= ~2u;
                    complete = 0;
                } else {
                    complete = 1;
                }
            }
        }
        if (budget <= 0)
            break;
    }
    if (!complete)
        return;
    OPPONENT_SYS(system)->goody_cursor = 0;
    nbaddies_can_see_players = 0;
    // Losing LOS alone does not erase the seen history; losing awareness does.
    for (i = 0; i < goody_count; ++i) {
        goody = goodies[i];
        for (j = 0; j < baddy_count; ++j) {
            baddy = baddies[j];
            bit = (u64)1 << baddy->object_id;
            known = OPPONENT_OBJECT(goody)->awareness & bit;
            if (known != 0 && (OPPONENT_LOS(WORLD->object_sys)->line_of_sight[goody->object_id] & bit) != 0)
                OPPONENT_GAME(((GAMEOBJECT *)goody->owner))->seen |= bit;
            else if ((OPPONENT_GAME(((GAMEOBJECT *)goody->owner))->seen & bit) != 0 && known == 0)
                OPPONENT_GAME(((GAMEOBJECT *)goody->owner))->seen &= ~bit;
            bit = (u64)1 << goody->object_id;
            known = OPPONENT_OBJECT(baddy)->awareness & bit;
            if (known != 0 && (OPPONENT_LOS(WORLD->object_sys)->line_of_sight[baddy->object_id] & bit) != 0)
                OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->seen |= bit;
            else if ((OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->seen & bit) != 0 && known == 0)
                OPPONENT_GAME(((GAMEOBJECT *)baddy->owner))->seen &= ~bit;
        }
    }
    for (i = 0; i < count; ++i) {
        object = objects[i];
        OPPONENT_PACKET(object->ai)->opponent_object = OPPONENT_PACKET(object->ai)->pending_opponent;
        OPPONENT_PACKET(object->ai)->opponent_metric = OPPONENT_PACKET(object->ai)->pending_opponent_metric;
        OPPONENT_PACKET(object->ai)->flags =
            ((OPPONENT_PACKET(object->ai)->flags >> 1 ^ OPPONENT_PACKET(object->ai)->flags) & 0x800) ^
            OPPONENT_PACKET(object->ai)->flags;
        OPPONENT_PACKET(object->ai)->nearest_opponent_object = OPPONENT_PACKET(object->ai)->pending_nearest_opponent;
        OPPONENT_PACKET(object->ai)->nearest_opponent_metric = OPPONENT_PACKET(object->ai)->pending_nearest_metric;
        OPPONENT_PACKET(object->ai)->pending_nearest_opponent = NULL;
        OPPONENT_PACKET(object->ai)->pending_nearest_metric = 1.0e9f;
        OPPONENT_PACKET(object->ai)->pending_opponent = NULL;
        OPPONENT_PACKET(object->ai)->pending_opponent_metric = 1.0e9f;
        OPPONENT_PACKET(object->ai)->flags &= ~0x1000;
        if (awareness & ((u64)1 << object->object_id))
            OPPONENT_PACKET(object->ai)->flags |= 0x4000;
        else
            OPPONENT_PACKET(object->ai)->flags &= ~0x4000;
        ((GAMEOBJECT *)object->owner)->portal_flags &= ~0x800;
        OPPONENT_GAME(((GAMEOBJECT *)object->owner))->exclusion = 0;
        if ((player != NULL &&
             (OPPONENT_GAME(((GAMEOBJECT *)object->owner))->seen & ((u64)1 << player->hostility_group))) ||
            (player2 != NULL &&
             (OPPONENT_GAME(((GAMEOBJECT *)object->owner))->seen & ((u64)1 << player2->hostility_group))))
            ++nbaddies_can_see_players;
        if (((GAMEOBJECT *)object->owner)->opponent != NULL) {
            OPPONENT_PACKET(object->ai)->opponent_object = (APIOBJECT_PC *)((GAMEOBJECT *)object->owner)->opponent;
            OPPONENT_PACKET(object->ai)->opponent_metric =
                NuVecDist(&OPPONENT_PACKET(object->ai)->opponent_object->position, &object->position, &difference);
            if (OPPONENT_PACKET(OPPONENT_PACKET(object->ai)->opponent_object->ai)->opponent_object == object)
                OPPONENT_PACKET(object->ai)->flags |= 0x800;
            else
                OPPONENT_PACKET(object->ai)->flags &= ~0x800;
        }
    }
}

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

static i32 GameAIWallShufflePc(GAMEOBJECT *object) {
    AIPATHNODE_PC *node;
    AIPATHNODE_PC *other_node;
    u16 angle;
    NUVEC wall_direction, lateral, ray, opposite, movement;
    f32 first_distance, second_distance, distance, scale;

    object->flags_ef4 |= 0x10000;
    other_node =
        &object->ai_packet.path_info.path
             ->nodes[object->ai_packet.path_info.connection->node_indices[object->ai_packet.path_info.direction == 0]];
    node = &object->ai_packet.path_info.path
                ->nodes[object->ai_packet.path_info.connection->node_indices[object->ai_packet.path_info.direction]];
    if (object->character_context == 0x45) {
        angle = object->takeover_start_angle;
        wall_direction.x = 0.0f;
        wall_direction.y = 0.0f;
        wall_direction.z = 1.0f;
        NuVecRotateY(&wall_direction, &wall_direction, angle);
    } else {
        NuVecSub(&lateral, &other_node->position, &node->position);
        lateral.y = 0.0f;
        NuVecNorm(&lateral, &lateral);
        NuVecRotateY(&lateral, &lateral, 0x4000);
        opposite.x = -lateral.x;
        opposite.y = 0.0f;
        opposite.z = -lateral.z;
        ray = lateral;
        if (NewRayCastScaleYMask(&node->position, &ray, 0.0f, 1.0f, 0,
                                 GetSurfaceInfoDefaultMaskPc.word | GetSurfaceInfoRuntimeMaskPc.word | 0x5f) != 0)
            first_distance = NuVecMag(&ray);
        else
            first_distance = 1.0e9f;
        ray = opposite;
        if (NewRayCastScaleYMask(&node->position, &ray, 0.0f, 1.0f, 0,
                                 GetSurfaceInfoDefaultMaskPc.word | GetSurfaceInfoRuntimeMaskPc.word | 0x5f) != 0)
            second_distance = NuVecMag(&ray);
        else
            second_distance = 1.0e9f;
        if (first_distance < 1.0e9 || second_distance < 1.0e9) {
            if (second_distance > first_distance)
                wall_direction = lateral;
            else
                wall_direction = opposite;
        }
        angle = NuAtan2D(wall_direction.x, wall_direction.z);
    }
    movement.x = object->ai_packet.movement_position.x - object->position.x;
    movement.y = 0.0f;
    movement.z = object->ai_packet.movement_position.z - object->position.z;
    distance = NuVecMag(&movement);
    if (distance > ai_moveradius) {
        scale = ai_moveradius / distance;
        scale = (f32)((f64)scale * 2.0);
        NuVecScale(&movement, &movement, scale);
    }
    NuVecRotateY(&movement, &movement, -(i32)angle);
    movement.z = 0.0f;
    NuVecRotateY(&movement, &movement, angle);
    NuVecScale(&wall_direction, &wall_direction, ai_moveradius);
    NuVecAdd(&movement, &movement, &wall_direction);
    object->ai_packet.movement_position.x = object->position.x + movement.x;
    object->ai_packet.movement_position.y = object->position.y;
    object->ai_packet.movement_position.z = object->position.z + movement.z;
    return 1;
}

i32 CheckPosAIArea(AIAREA_PC *area, NUVEC *position, float tolerance) {
    NUVEC local_position;
    if (area == NULL || position == NULL) {
        return 0;
    }

    NuVecSub(&local_position, position, &area->position);
    NuVecRotateY(&local_position, &local_position, -area->rotation);

    return local_position.x + tolerance >= -area->half_width && local_position.y + tolerance >= -0.1f &&
           local_position.z + tolerance >= -area->half_depth && local_position.x - tolerance <= area->half_width &&
           local_position.y - tolerance <= area->height && local_position.z - tolerance <= area->half_depth;
}

i32 GameAIActionParseSpeed(char *name, u8 *speed) {
    if (NuStrICmp(name, "RUN") == 0) {
        *speed = AI_ACTION_SPEED_RUN;
        return 1;
    }
    if (NuStrICmp(name, "WALK") == 0) {
        *speed = AI_ACTION_SPEED_WALK;
        return 1;
    }
    if (NuStrICmp(name, "TIPTOE") == 0) {
        *speed = AI_ACTION_SPEED_TIPTOE;
        return 1;
    }
    return 0;
}

void __movsd(unsigned long *, unsigned long const *, unsigned int);
#pragma intrinsic(__movsd)
void __cdecl GameAIProcess(void) {

    i32 index;
    u16 angle;
    GAMEOBJECT *object;
    f32 frame_time;
    f32 follow_offset;
    i32 all_under_cover;
    APIOBJECT_PC *object_lists[4][64];
    APIOBJECT_PC **neutral_objects;
    APIOBJECT_PC **interactive_objects;
    APIOBJECT_PC **goodies;
    APIOBJECT_PC **baddies;
    i32 neutral_count;
    i32 interactive_count;
    i32 goody_count;
    i32 baddy_count;
    APIOBJECT_PC **neutral_cursor;
    APIOBJECT_PC **goody_cursor;
    u64 awareness;
    NUVEC direction, forward;
    u32 combat_flags;
    u32 saved_ai_flags;
    APIOBJECT_PC **interactive_cursor;
    APIOBJECT_PC **baddy_cursor;
    i32 ground_checks;
    u8 ai_update_flags;
    i32 defend;
    BOLTTYPE_PC *bolt;
    f32 scale;
    f32 dot;
    f32 threshold;
    AIPATHCNX_PC *connection;
    NUVEC position;
    f32 x;
    f32 z;
    f64 radius;
    i32 los_count;
    APIOBJECT_PC *first_player;
    APIOBJECT_PC *second_player;
    f32 hold_time;
    neutral_count = 0;
    interactive_count = 0;
    goody_count = 0;
    baddy_count = 0;
    awareness = 0;
    ai_fighting = 0;
    first_player = NULL;
    second_player = NULL;
    if (player != NULL)
        first_player = (APIOBJECT_PC *)player;
    if (player2 != NULL)
        second_player = (APIOBJECT_PC *)player2;
    AISysProcess(WORLD->ai_sys, first_player, second_player);

    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; ++object, ++index) {
        if (!((object->flags_1fc_word & 0x1000) != 0 && object->dead == 0)) {
            continue;
        }
        object->flags_ef0 &= ~0x8000u;
        object->flags_ef4 &= 0xfffc3fffu;
        object->ai_frame_event = 0;
        if ((object->use_action_frames != 0)) {
            --object->use_action_frames;
        } else if (object->use_action_parameter > 0.0f) {
            object->use_action_parameter -= FRAMETIME;
        } else {
            object->can_use_object = NULL;
            object->field_f00 = 0;
        }
        if (object->field_1088 != 0)
            --object->field_1088;
        if ((object->ai_packet.flags & 0x4000) != 0)
            object->ai_visibility_extension = draw_attention_distance;
        else
            object->ai_visibility_extension = 0.0f;
        if (MiniCutCam != 0 && (object->flags_1fc_word & 0x100) != 0) {
            object->pad->buttons_held &= GAMEPAD_START;
            object->pad->buttons_pressed &= GAMEPAD_START;
        }
        if (object->alert_target != NULL) {
            frame_time = object->field_ec4 - FRAMETIME;
            object->field_ec4 = frame_time;
            if (frame_time <= 0.0f) {
                object->field_ec4 = 0.0f;
                object->alert_target = NULL;
            }
        }
    }
    neutral_objects = object_lists[3];
    interactive_objects = object_lists[2];
    goodies = object_lists[1];
    baddies = object_lists[0];
    interactive_cursor = interactive_objects;
    baddy_cursor = baddies;
    goody_cursor = goodies;
    neutral_cursor = neutral_objects;
    active_neutral_count = 0;
    party_under_cover = 0;
    all_under_cover = 1;
    for (index = 0; index < 8; ++index) {
        object = Player[index];
        if (object != NULL && (object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
            (object->dead == 0 || object->field_1010 > 0.0f) && (object->flags_ef3 & 1) == 0 && StartupGameFlag1 != 0 &&
            (NULL == VADER_ADATA || WORLD->area != VADER_ADATA) && party_cant_be_under_cover == 0) {
            if (object->helmet == 5 ||
                ((FreePlay != 0 || WORLD->current_level == HUB_LDATA) &&
                 (object->character_data->model_flags & 0x204) != 0 && object->takeover_source == NULL)) {
                if (alert_obj == NULL || alert_obj->field_280 == 0xff || (alert_obj->flags_ef3 & 1) != 0)
                    party_under_cover = 1;
            }
            if ((object->character_data->model_flags & 0x80000) == 0)
                all_under_cover = 0;
        }
        party_follow_offsets[index] = 0.5f;
    }
    if (StartupGameFlag1 != 0 && (VADER_ADATA == NULL || WORLD->area != VADER_ADATA) && (all_under_cover != 0) &&
        party_cant_be_under_cover == 0) {
        party_under_cover = 1;
    }
    AITriggerSetSysProcess(WORLD->ai_trigger_set);

    follow_offset = 0.0f * GameAiPartyFollowSpacingPc + 0.0f;
    for (index = 0; index < 8; ++index) {
        if (index < 2 && player == (object = Player[1])) {
            if (index != 0)
                object = Player[0];
        } else {
            object = Player[index];
        }
        if (object != NULL && (object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
            (object->dead == 0 || object->field_1010 > 0.0f) && object->character_context != 0x17 &&
            object->character_context != 0x5a) {
            if (party_under_cover != 0 && (object->ai_packet.flags & 0x4000) != 0)
                party_under_cover = 0;
            if (!(object->camera_screen_position.x > (f64)-0.85f && object->camera_screen_position.x < (f64)0.85f &&
                  object->camera_screen_position.y > (f64)-0.85f && object->camera_screen_position.y < (f64)0.85f) &&
                (object->tag_flags & 2) == 0 && drop_back_in_timer > 0.0f) {
                object->ai_packet.flags &= ~0x1000u;
                object->ai_packet.target_metric_e0 = 1.0e9f;
                object->ai_packet.target_metric_f0 = 1.0e9f;
                object->ai_packet.nearest_opponent = NULL;
                object->ai_packet.target_dc = NULL;
                object->ai_packet.opponent = NULL;
                object->ai_packet.target_ec = NULL;
            } else if ((object->flags_1f8 & 1) != 0) {
                ++baddy_count;
                *baddy_cursor++ = (APIOBJECT_PC *)object;
            } else if ((object->flags_1f8 & 4) != 0) {
                ++neutral_count;
                *neutral_cursor++ = (APIOBJECT_PC *)object;
            } else {
                ++goody_count;
                *goody_cursor++ = (APIOBJECT_PC *)object;
            }
        }
        object = Player[index];
        if (object != NULL && (object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
            (object->dead == 0 || object->field_1010 > 0.0f) && 0 == (object->flags_ef3 & 1) &&
            (object->flags_1fc_word & 0x80) == 0) {
            party_follow_offsets[(i8)object->field_280] = follow_offset;
            follow_offset += GameAiPartyFollowSpacingPc;
        }
    }
    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {

        object->active_trigger_set = NULL;
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0 ||
            object->character_context == 0x3b) {
            goto interaction;
        }

        if (StartupGameFlag0 != 0) {
            ground_checks = object->field_281 != 0 || (object->character_data->model_flags & 0x2000) != 0 ||
                            object->character_context == 0x4b;
        } else if (StartupGameFlag1 != 0) {
            ground_checks = object->field_281 != 0 || (object->character_data->model_flags & 0x2000) != 0;
        } else {
            ground_checks = object->field_281 != 0 || (object->character_data->model_flags & 0x2000) != 0 ||
                            object->character_context == 0x44 || object->character_context == 0x4b;
            if (object->character_context == 0x17) {
                if ((object->ai_update_flags & 8) != 0)
                    AISysUpdateCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet,
                                                ground_checks, object->ai_elapsed_time);
                continue;
            }
        }

        ai_update_flags = object->ai_update_flags;
        if ((object->flags_1f8 & 0x400) != 0) {
            if ((object->flags_1f8 & 0x10000) != 0) {
                ++baddy_count;
                *baddy_cursor++ = (APIOBJECT_PC *)object;
                ++goody_count;
                *goody_cursor++ = (APIOBJECT_PC *)object;
            } else if ((object->flags_1f8 & 1) != 0) {
                ++baddy_count;
                *baddy_cursor++ = (APIOBJECT_PC *)object;
            } else if ((object->flags_1f8 & 4) != 0) {
                ++neutral_count;
                *neutral_cursor++ = (APIOBJECT_PC *)object;
            } else {
                ++goody_count;
                *goody_cursor++ = (APIOBJECT_PC *)object;
            }
        }
        if ((object->flags_1f8 & 0x40000) != 0) {
            if ((object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 &&
                object->context_target_position == NULL &&
                (object->character_data->game_character->flags_090 & 0x8000) == 0) {
                ++interactive_count;
                *interactive_cursor++ = (APIOBJECT_PC *)object;
                object->ai_packet.flags |= 0x2000;
                object->ai_collision_priority_pc = HIGHGAMEOBJECT - object->hostility_group;
                object->ai_resolved_collision_priority_pc = 0;
            } else if (object->field_1010 > 0.0f) {
                object->ai_packet.flags |= 0x2000;
            } else {
                object->ai_packet.flags &= ~0x2000u;
                object->ai_packet.nearest_opponent = NULL;
                object->ai_packet.opponent = NULL;
            }
            if (object->opponent != NULL && (((APIOBJECT_PC *)object->opponent)->flags_low & 1) == 0)
                object->opponent = NULL;
            if ((APIOBJECT_PC *)object->ai_packet.opponent != NULL &&
                ((((APIOBJECT_PC *)object->ai_packet.opponent)->flags_low & 1) == 0 ||
                 (((APIOBJECT_PC *)object->ai_packet.opponent)->ai->flags & 0x2000) == 0)) {
                object->ai_packet.opponent_metric = 1.0e9f;
                object->ai_packet.opponent = NULL;
            }
            if ((object->ai_update_flags & 8) != 0)
                AISysUpdateCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, ground_checks,
                                            object->ai_elapsed_time);
            continue;
        }
        if (object->pad->bound_pad == NULL) {
            object->pad->buttons_released = 0;
            object->pad->left_directions = 0;
            object->pad->previous_left_directions = 0;
            object->pad->right_directions = 0;
            object->pad->previous_right_directions = 0;
            object->pad->ai_input_state = 0;
            object->pad->buttons_held = 0;
            object->pad->buttons_pressed = 0;
        }
        awareness |= (u64)object->hostility_mask[0] | ((u64)object->hostility_mask[1] << 32);
        if ((ai_update_flags & 8) != 0) {
            combat_flags = object->portal_flags;
            combat_flags = (combat_flags & ~0x420u) | ((combat_flags & 0x200u) << 1);
            object->portal_flags = combat_flags;
            object->script_fire_target = NULL;
            if ((combat_flags & 0x8000u) != 0) {
                object->portal_flags = combat_flags & ~0x8000u;
                object->field_e58 = object->field_e4c;
            }
            object->portal_flags &= ~0x30000u;
        }
        if (StartupGameFlag0 != 0 && (object->flags_1fc_word & 0x80) != 0 &&
            object->character_context != LEGOCONTEXT_GRAPPLE) {
            saved_ai_flags = object->ai_packet.capabilities;
            object->ai_packet.capabilities = saved_ai_flags & ~8u;
            AISysProcessCharacter(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, ground_checks,
                                  object->ai_elapsed_time, 0, (ai_update_flags >> 3) & 1);
            object->ai_packet.capabilities = saved_ai_flags;
        } else {
            AISysProcessCharacter(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, ground_checks,
                                  object->ai_elapsed_time, 0, (ai_update_flags >> 3) & 1);
        }
        if (((object->flags_1fc_word & 0x80) == 0)) {
            if (object->character_context == 0x5d)
                SetBallooningHeight(object, object->ai_packet.movement_destination.y);
            if ((WORLD->current_level == JEDI_B_LDATA) && object->id == (u16)id_JANGOFETT &&
                (object->portal_flags & 0x8000000u) != 0 && object->hover_height_override != 1.0e9f &&
                object->jump_variant_state == 0)
                object->jump_variant_state = 1;
            if (object->field_280 != 0xff && object->jump_variant_state != 0 &&
                (object->ai_packet.capabilities & LEGO_AIPATHCNX_R2D2GLIDE) == 0)
                object->jump_variant_state = 0;
            if (object->character_context == 0x4b && object->suit != NULL && (object->suit->flags & 0x10) != 0)
                object->pad->buttons_held |= GAMEPAD_SPECIAL;
            connection = object->ai_packet.path_info.connection;
            if (((connection != NULL ? connection->traversal_flags[object->ai_packet.path_info.direction] &
                                           object->ai_packet.capabilities
                                     : 0) &
                 LEGO_AIPATHCNX_WALLSHUFFLE) != 0) {
                GameAIWallShufflePc(object);
            } else if (object->ai_packet.path_update_flag == 0 && (object->ai_packet.path_info.flags_word & 1) != 0 &&
                       object->respawn_timer > jump_stuck_time) {
                if (object->supporting_platform_id != -1 && connection != NULL &&
                    (connection->original_traversal_flags[0] & LEGO_AIPATHCNX_BLOCKAGE) != 0) {
                    if ((object->character_data->model_flags & 0x88) != 0) {
                        object->pad->buttons_pressed |= GAMEPAD_ACTION;
                        object->respawn_timer = 0.0f;
                        GameObjectSetCanUse(object, NULL, 5, 0, 1.0f);
                    } else if (FreePlay != 0) {
                        hold_time = object->input_toggle_hold_time - FRAMETIME;
                        object->input_toggle_hold_time = hold_time;
                        if (hold_time <= 0.0f) {
                            Player_ToggleCharacter(object, 1, 0);
                            if (object->id == (u16)id_DROIDEKA)
                                Player_ToggleCharacter(object, 1, 0);
                            if (object->model_draw_result != 0)
                                object->input_toggle_hold_time = 0.25f;
                            else
                                object->input_toggle_hold_time = 0.0f;
                        }
                    }
                } else if ((object->ai_packet.capabilities & (LEGO_AIPATHCNX_JUMP | LEGO_AIPATHCNX_R2D2GLIDE)) != 0 &&
                           object->jump_variant_state == 0) {
                    object->pad->buttons_pressed |= GAMEPAD_JUMP;
                    object->respawn_timer = 0.0f;
                } else {
                    object->ai_packet.flags |= 0x400000;
                }
            }
        }
        if (FreePlay != 0 && (object->flags_1f8 & 0x400) == 0) {
            if ((object->flags_1fc_word & 0x80) == 0 && object->character_context != 0x0b) {
                if (drop_in_teleport == 1 && VehicleArea == 0 && (Arcade != 0 || teleport_all_freeplay_modes != 0) &&
                    object->model_draw_result == 0 && (object->tag_flags & 2) == 0 && drop_back_in_timer > 0.0f) {
                    position = player->last_safe_position;
                    if (position.x == player->position.x && position.y == player->position.y &&
                        position.z == player->position.z)
                        position.z += 0.01f;
                    object->start_position = position;
                    object->position = object->start_position;
                    object->velocity = v000;
                    object->field_0x276 = player->field_0x276;
                    ResetPlayerMoves(object);
                    object->respawn_timer = 0.0f;
                    InitSurfaceInfo(object);
                    SetObjOnSurface(object, 1);
                    GameObjectOrigin(object);
                    AddGameDebris(WORLD->game_debris_sys, 0x5c, &object->collision_position);
                } else if ((object->ai_packet.route_flags & 0x10) != 0) {
                    object->ai_packet.current_route = 0xff;
                    object->route_character_id = -1;
                    object->route_frame_state = 0;
                } else if ((0x40 & (object->ai_packet.route_flags)) != 0 &&
                           ((object->ai_packet.route_flags & 0x20) != 0 ||
                            (object->ai_packet.path_info.connection != NULL &&
                             object->ai_packet.path_info.connection->route_mask != 0))) {
                    if ((object->flags_ef0 & 0x40) != 0) {
                        if (object->route_frame_state != object->ai_packet.frame_state) {
                            object->route_character_id = -1;
                            object->route_suit_index = -1;
                            object->flags_ef0 &= ~0x40;
                            object->route_frame_state = 0;
                        }
                    } else if ((object->ai_packet.frame_state & LEGO_AIPATHCNX_DONTTOGGLE) == 0) {
                        if (object->route_character_id == -1 ||
                            object->route_frame_state != object->ai_packet.frame_state ||
                            object->route_character_id == id_DROIDEKA) {
                            object->route_character_id = object->id;
                            object->route_suit_index =
                                object->suit != NULL ? Suit_GetIndex((SUIT_LAYERS_PC *)(object->suit)) : -1;
                            object->input_toggle_hold_time = 0.0f;
                            object->route_frame_state = object->ai_packet.frame_state;
                            object->route_start_index = object->ai_packet.next_route;
                            object->route_search_index = object->ai_packet.next_route;
                        }
                        object->input_toggle_hold_time -= FRAMETIME;
                        if (object->input_toggle_hold_time <= 0.0f) {
                            Player_ToggleCharacter(object, 1, 0);
                            if (object->id == (u16)id_DROIDEKA)
                                Player_ToggleCharacter(object, 1, 0);
                            object->input_toggle_hold_time = object->model_draw_result != 0 ? 0.25f : 0.0f;
                            if (object->id == object->route_character_id &&
                                Suit_GetIndex((SUIT_LAYERS_PC *)(object->suit)) == object->route_suit_index) {
                                Player_ToggleCharacter(object, -1, 0);
                                if (object->id == (u16)id_DROIDEKA)
                                    Player_ToggleCharacter(object, -1, 0);
                                object->route_character_id = object->id;
                                object->route_suit_index = Suit_GetIndex((SUIT_LAYERS_PC *)(object->suit));
                                object->ai_packet.next_route = object->route_search_index;
                                AISysFindRoute(&object->ai_packet);
                                object->route_search_index = object->ai_packet.next_route;
                                if ((object->route_start_index) == (object->route_search_index))
                                    object->flags_ef0 |= 0x40;
                            }
                        }
                    }
                } else {
                    object->route_character_id = -1;
                    object->route_suit_index = -1;
                    object->flags_ef0 &= ~0x40;
                    object->route_frame_state = 0;
                    object->route_search_index = 0;
                }
            }
        } else if ((object->ai_packet.flags & 0x400000) != 0) {
            AISysFindRoute(&object->ai_packet);
        }
        if ((object->flags_1fc_word & 0x180) == 0x80) {
            object->flags_ef0 &= ~4;
            object->flags_1fc_word =
                (object->flags_1fc_word & ~0x200u) | ((object->character_data->model_flags >> 19) & 0x200u);
        } else {
            object->flags_1fc_word &= ~0x200u;
            defend = 0;
            if ((object->combat_flags & 0x80) != 0 && (APIOBJECT_PC *)object->ai_packet.opponent != NULL) {
                bolt = BoltType_FindByID(0, WORLD);
                if (bolt->field_10 * bolt->field_14 > object->ai_packet.opponent_metric) {
                    NuVecSub(&direction, &((GAMEOBJECT *)object->ai_packet.opponent)->collision_position,
                             &object->collision_position);
                    scale = object->ai_packet.opponent_metric == 0.0f ? 0.0f : 1.0f / object->ai_packet.opponent_metric;
                    NuVecScale(&direction, &direction, scale);
                    NuVecRotateY(&forward, &v001, object->movement_facing_angle);
                    dot = NuVecDot(&direction, &forward);
                    angle = object->ai_packet.opponent_metric < 0.5f   ? 0x3aaa
                            : object->ai_packet.opponent_metric < 1.0f ? 0x3000
                                                                       : 0x2555;
                    if (dot > NuTrigTable[angle]) {
                        object->pad->buttons_pressed |= GAMEPAD_ACTION;
                        object->script_fire_target = (GAMEOBJECT *)((APIOBJECT_PC *)object->ai_packet.opponent)->owner;
                    }
                }
                defend = 1;
            }
            if (FreePlay != 0 && (object->flags_1f8 & 0x400) == 0 && object->id == (u16)id_DROIDEKA)
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            if ((object->combat_flags & 2) != 0 &&
                ((object->flags_1f8 & 0x400) != 0 || party_under_cover == 0 || (object->flags_ef3 & 1) != 0) &&
                ((CharCategory_IsCategory(object, 0) != 0 || object->id == (u16)id_GRIEVOUS ||
                  object->id == (u16)id_BODYGUARD || object->id == (u16)id_IMPERIALGUARD) != 0 ||
                 object->id == (u16)id_GAMORREANGUARD)) {
                if ((object->combat_flags & 1) != 0 || (APIOBJECT_PC *)object->ai_packet.opponent != NULL ||
                    (object->field_e22 & 8) != 0)
                    object->timer_ecc = DEFENDTIME;
                if (object->timer_ecc > 0.0f) {
                    defend = 1;
                    if ((object->field_e22 & 9) == 9)
                        object->pad->buttons_pressed |= GAMEPAD_ACTION;
                }
            }
            if ((object->combat_flags4 & 0x20) != 0) {
                defend = 1;
                if ((object->field_e22 & 1) != 0) {
                    object->pad->buttons_pressed |= GAMEPAD_ACTION;
                    object->combat_flags4 &= ~0x20;
                }
            }
            if (0 != (object->combat_flags & 8) &&
                (defend || (object->combat_flags & 0x30) != 0 ||
                 ((object->id == (u16)id_YODA || object->id == (u16)id_YODAGHOST) &&
                  (object->flags_1fc_word & 0x80) == 0 && object->field_280 != 0xff))) {
                if ((object->field_e22 & 1) == 0 && object->character_context != 7 && object->field_e33 == 0)
                    object->pad->buttons_pressed |= GAMEPAD_ACTION;
            } else if ((object->pad->buttons_pressed & GAMEPAD_ACTION) == 0 && (object->field_e22 & 1) != 0 &&
                       object->character_context != 6 && object->field_e33 == 0) {
                object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
            }
        }
        if ((object->character_data->game_character->flags_094_bytes[3] & 0x10) != 0 &&
            (object->flags_1fc_word & 0x80) == 0) {
            threshold = (f32)((f64)object->character_data->game_character->walk_speed - (f64)0.01f);
            if (object->jump_variant_state == 1) {
                if (threshold >= object->pad->input_magnitude) {
                    object->ai_hop_timer += FRAMETIME;
                    if ((f64)object->ai_hop_timer > (f64)0.1f) {
                        object->pad->buttons_pressed |= GAMEPAD_JUMP;
                        object->ai_hop_timer = 0.0f;
                    }
                } else {
                    object->ai_hop_timer = 0.0f;
                }
            } else if (object->pad->input_magnitude > threshold) {
                object->ai_hop_timer += FRAMETIME;
                if (object->ai_hop_timer > 1.0f) {
                    object->pad->buttons_pressed |= GAMEPAD_JUMP;
                    object->ai_hop_timer = 0.0f;
                }
            } else {
                object->ai_hop_timer = 0.0f;
            }
        }
        if ((object->ai_packet.opponent != NULL))
            SetObjAsHeadTarget(object, (GAMEOBJECT *)((APIOBJECT_PC *)object->ai_packet.opponent)->owner, 2, 1.0f, 0.0f,
                               0.0f);
    interaction:
        if ((object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 && object->context_target_position == NULL &&
            (object->character_data->game_character->flags_090 & 0x8000) == 0) {
            ++interactive_count;
            *interactive_cursor++ = (APIOBJECT_PC *)object;
            object->ai_collision_priority_pc = HIGHGAMEOBJECT - object->hostility_group;
            object->ai_resolved_collision_priority_pc = 0;
            if ((object->flags_1fc_word & 2) != 0 || (object->flags_ef0 & 0x80000000u) != 0) {
                object->ai_collision_priority_pc = 0x8000;
            } else if ((object->flags_1fc_word & 0x80) == 0) {
                if ((object->ai_packet.flags & 0x80000000u) != 0)
                    object->ai_collision_priority_pc |= 0x4000;
                if ((object->ai_packet.flags & 0x40000000u) != 0)
                    object->ai_collision_priority_pc |= 0x2000;
                if ((object->ai_packet.navigation_flags & 1) != 0)
                    object->ai_collision_priority_pc |= 0x1000;
                if ((object->pad->runtime_flags & 2) != 0)
                    object->ai_collision_priority_pc |= 0x800;
                if ((object->ai_packet.path_info.flags_word & 1) == 0 || object->ai_packet.path_info.connection == NULL)
                    object->ai_collision_priority_pc |= 0x400;
                if (object->ai_packet.movement_parameter == 0.0f) {
                    x = object->ai_packet.fallback_destination.x - object->position.x;
                    z = object->ai_packet.fallback_destination.z - object->position.z;
                    radius = (f64)object->ai_packet.mover_height + 1.0;
                    if (x * x + z * z < radius * radius)
                        object->ai_collision_priority_pc |= 0x200;
                }
            }
            object->ai_packet.flags |= 0x2000;
            if (object->opponent != NULL && (((APIOBJECT_PC *)object->opponent)->flags_low & 1) == 0)
                object->opponent = NULL;
            if ((APIOBJECT_PC *)object->ai_packet.opponent != NULL &&
                ((((APIOBJECT_PC *)object->ai_packet.opponent)->flags_low & 1) == 0 ||
                 (((APIOBJECT_PC *)object->ai_packet.opponent)->ai->flags & 0x2000) == 0)) {
                object->ai_packet.opponent = NULL;
                object->ai_packet.opponent_metric = 1.0e9f;
            }
            if ((object->flags_ef0 & 0x8000000u) != 0 || debug_ignore_players != 0 || Hub_InMenu() != 0) {
                U64_PAIR(object->ai_opponent_exclusion_mask) |= (u64)1 << player->hostility_group;
                if (player2 != NULL)
                    U64_PAIR(object->ai_opponent_exclusion_mask) |= (u64)1 << player2->hostility_group;
            }
            if ((object->flags_1fc_word & 0x80) != 0 && object->field_281 != 0 &&
                object->pad->input_magnitude > (WALKSTEALTH != 0
                                                    ? object->character_data->game_character->walk_speed
                                                    : object->character_data->game_character->tiptoe_speed))
                object->portal_flags |= 0x800;
        } else if (object->field_1010 > 0.0f) {
            object->ai_packet.flags |= 0x2000;
        } else {
            object->ai_packet.flags &= ~0x2000u;
            object->ai_packet.nearest_opponent = NULL;
            object->ai_packet.opponent = NULL;
        }
    }
    active_goody_count = goody_count;
    active_baddy_count = baddy_count;
    active_neutral_count = neutral_count;
    los_count = baddy_count;
    if (neutral_count > 0) {
        __movsd((unsigned long *)&baddies[baddy_count], (unsigned long *)neutral_objects, neutral_count);
        los_count = neutral_count + baddy_count;
    }
    timetogetlos = GameAiLosDivisorPc != 0 ? (f32)(los_count * los_count) * FRAMETIME / GameAiLosDivisorPc : 1.0e9f;
    APIObjectLOSChecks(WORLD->object_sys, GameAiLosDivisorPc, goody_count, goodies, los_count, baddies,
                       (f32)(u8)(WORLD->current_level->raycast_step));
    if (test_new_interaction != 0)
        GameCreatureInteraction2D(WORLD->ai_sys, interactive_count, interactive_objects, NULL, FRAMETIME);
    else
        LEGO_AISysCreatureInteraction2D(WORLD->ai_sys, interactive_count, interactive_objects, NULL, FRAMETIME);
    if (party_under_cover != 0) {
        for (index = 0; index < 8; ++index) {
            if (((GAMEOBJECT **)Player)[index] != NULL)
                ((GAMEOBJECT **)Player)[index]->ai_packet.flags &= ~0x4000u;
        }
    }
    GameCreatureOpponentSelection(WORLD->ai_sys, interactive_count, interactive_objects, goody_count, goodies,
                                  baddy_count, baddies, awareness, FRAMETIME);
    GizTurrets_OpponentSelection(WORLD->giz_turret_sys, goody_count, goodies, baddy_count, baddies);
    oneAtOnce_MaintainArray();
}

i32 AutoSaveIconPendingPc = 0;

void __cdecl DrawAutoSaveIcon(void) {
    AutoSaveIconPendingPc = 1;
}

/* Native 0x0043fda0, 390 bytes. */
STATIC_ASSERT(offsetof(WORLDINFO, script_processor_count) == 0x2aec);
STATIC_ASSERT(offsetof(WORLDINFO, script_processors) == 0x2af0);
STATIC_ASSERT(offsetof(WORLDINFO, ai_path_cnx_control_sys) == 0x4670);
STATIC_ASSERT(offsetof(WORLDINFO, ai_connection_helpers) == 0x4674);
void __cdecl LevelScriptReStoreProgress(WORLDINFO *, LEVEL_SCRIPT_PROCESS_PC *);
void __cdecl GameAIResetPathConnectionsPc(AISYS_PC *);

extern const char GameAILevelScriptFormatPc[8];

void __cdecl GameAISysReset(AISYS_PC *system) {
    i32 script_index;
    char script_name[32];
    if (system == NULL)
        return;
    AISysSetLevelPath(system, NULL);
    WORLD->script_processor_count = 0;
    for (script_index = 0; script_index < 32; ++script_index) {
        if (script_index != 0)
            sprintf(script_name, GameAILevelScriptFormatPc, script_index);
        else
            sprintf(script_name, AIScriptKeywordLevel);
        if (AIScriptFind(WORLD->ai_sys, script_name, 0, 1, 0) == NULL)
            continue;
        AIScriptProcessorInit(system, NULL, &WORLD->script_processors[WORLD->script_processor_count].processor, NULL,
                              script_name, NULL, 0, NULL, NULL);
        NuStrCpy(WORLD->script_processors[WORLD->script_processor_count].name, script_name);
        LevelScriptReStoreProgress(WORLD, &WORLD->script_processors[WORLD->script_processor_count]);
        ++WORLD->script_processor_count;
    }
    GameAIResetPathConnectionsPc(system);
    AIPathCnxControlSysReset(WORLD->ai_path_cnx_control_sys);
    AIPathCnxHelperSysReset(WORLD, WORLD->ai_connection_helpers);
    InitAICreatures(system);
    ResetAICreatures(system);
    if (WORLD->script_processor_count != 0)
        GizmoSysAddGizmos(WORLD->gizmo_sys, WORLD->giz_flow, WORLD);
}

/* Native 0x0043b7c0, 376 bytes: private EDI-argument helper of frame owner. */
static void GameAIUpdateAreaPc(AIAREA_PC *area) {
    i32 area_index = (i32)(area - WORLD->ai_sys->areas);
    GAMEOBJECT *object = Obj;
    i32 index;
    NUVEC local;
    u64 outside_bit;
    i32 inside_low;
    area->runtime_flags &= ~7u;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0)
            continue;
        if (object->dead != 0 && !(object->field_1010 > 0.0f) && (i8)object->flags_1fc_half >= 0)
            continue;
        NuVecSub(&local, &object->position, &area->position);
        NuVecRotateY(&local, &local, -area->rotation);
        if (local.x >= -area->half_width && (f64)local.y >= (f64)-0.1f && local.z >= -area->half_depth &&
            local.x <= area->half_width && local.y <= area->height && local.z <= area->half_depth) {
            inside_low = (i32)(1u << (area_index & 31));
            object->ai_area_mask[0] |= (u32)inside_low;
            object->ai_area_mask[1] |= (u32)((i64)inside_low >> 32);
            if ((object->flags_1fc_half & 0x80) != 0)
                area->runtime_flags |= 1;
            if ((object->flags_1f8 & 1) != 0)
                area->runtime_flags |= 4;
            else if ((object->flags_1f8 & 4) == 0)
                area->runtime_flags |= 2;
        } else {
            outside_bit = (u64)1 << area_index;
            object->ai_area_mask[0] &= ~(u32)outside_bit;
            object->ai_area_mask[1] &= ~(u32)(outside_bit >> 32);
        }
    }
}

typedef struct ai_frame_world_pc_s {
    u8 unknown_0000[0x2aec];
    i32 processor_count;
    LEVEL_SCRIPT_PROCESS_PC processors[32];
} AI_FRAME_WORLD_PC;
STATIC_ASSERT(offsetof(AI_FRAME_WORLD_PC, processor_count) == 0x2aec);
STATIC_ASSERT(offsetof(AI_FRAME_WORLD_PC, processors) == 0x2af0);
/* Native 0x0043b940, 467 bytes. */
void __cdecl GameAISysStartFrame(AISYS_PC *system) {
    i32 path_index, connection_index, script_index;
    AIPATH_PC *path;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *node;
    AI_FRAME_WORLD_PC *world;
    if (system == NULL || netclient != 0)
        return;
    if (system->path_sys != NULL) {
        for (path_index = 0; path_index < system->path_sys->path_count; ++path_index) {
            memset(system->path_sys->paths[path_index]->updated_node_bits, 0, 0x20);
            PCByteCopy_candidate(system->path_sys->paths[path_index]->previous_inside_node_bits,
                                 system->path_sys->paths[path_index]->inside_node_bits, 0x20);
            memset(system->path_sys->paths[path_index]->inside_node_bits, 0, 0x20);
        }
        for (path_index = 0; path_index < system->path_sys->path_count; ++path_index) {
            for (connection_index = 0; connection_index < system->path_sys->paths[path_index]->connection_count;
                 ++connection_index) {
                path = system->path_sys->paths[path_index];
                connection = &path->connections[connection_index];
                node = &system->path_sys->paths[path_index]->nodes[connection->node_indices[0]];
                if (node->has_special != 0)
                    AIPathNodeUpdatePos(system, system->path_sys->paths[path_index], node);
                path = system->path_sys->paths[path_index];
                node = &path->nodes[connection->node_indices[1]];
                if (node->has_special != 0)
                    AIPathNodeUpdatePos(system, path, node);
            }
        }
    }
    if (system->next_area_check < system->area_count) {
        GameAIUpdateAreaPc(&system->areas[system->next_area_check]);
        ++system->next_area_check;
        if (system->next_area_check >= system->area_count)
            system->next_area_check = 0;
    }
    for (script_index = 0; script_index < ((AI_FRAME_WORLD_PC *)WORLD)->processor_count; ++script_index) {
        world = (AI_FRAME_WORLD_PC *)WORLD;
        if ((world->processors[script_index].processor.flags & 2) == 0)
            AIScriptProcess(system, NULL, NULL, &world->processors[script_index].processor, FRAMETIME);
    }
}
