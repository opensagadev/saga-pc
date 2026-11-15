#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "globals.h"
#include "legoapi/actions/character/snake.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/gizmos/transport/gizportal.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuportal.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

extern const f64 GameObjectNearFloorNoFloorHeight;

/* A copy of the 33-DWORD script cohort (script processor plus takeover link), without extending the
   packet. Held as words so the copy is one block move and the local is not treated as a /GS buffer. */

/* Bitfield view of flags_1fc: bit 7 marks a player-controlled object, bit 8 an AI-driven one. */

/* Shadow platform entries. Tail[1] holds the counted traversal, not an allocation count.
 */
typedef struct shadow_platform_entry_pc_s { // size 0x14
    GAMEOBJECT *object;                     // 0x00
    u8 unknown_04_14[0x10];                 // 0x04
} SHADOW_PLATFORM_ENTRY;

struct shadow_platform_list_pc_s {    // size 0x2c
    u8 unknown_00_04[4];              // 0x00
    i32 count;                        // 0x04
    u8 unknown_08_18[0x10];           // 0x08
    SHADOW_PLATFORM_ENTRY entries[1]; // 0x18
};

typedef struct gameobject_collision_reset_s { // size 0x2b8
    u8 unknown_000[0x2b0];                    // 0x000
    u32 collision_overlap_low;                // 0x2b0
    u32 collision_overlap_high;               // 0x2b4
} GAMEOBJECT_COLLISION_RESET;

void ClearGameObjects(APIOBJECTSYS *system) {
    memset(Obj, 0, sizeof(GAMEOBJECT) * 64);
    APIObjectDestroyAll(system);
    HIGHGAMEOBJECT = 0;
}

static void RecomputeHighGameObject(void) {
    i32 index;
    HIGHGAMEOBJECT = 0;
    for (index = 0; index < 64; ++index) {
        if ((Obj[index].flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0) {
            HIGHGAMEOBJECT = index + 1;
        }
    }
}

GAMEOBJECT *AddGameObject(void) {
    GAMEOBJECT *object = APIObjectCreate(WORLD->object_sys);
    u64 identity;
    if (object != NULL) {
        object->flags_1fc_word |= APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER;
        object->origin_scale = 1.0f;
        object->collision_scale = 1.0f;
        object->reflection_height = 2000000.0f;
        object->sock_index = 0xff;
        object->movement_mode = 2;
        identity = (u64)1 << object->hostility_group;
        object->collision_identity_mask[0] = (u32)identity;
        object->roof_height = 2000000.0f;
        object->collision_identity_mask[1] = (u32)(identity >> 32);
        object->water_height = 2000000.0f;
        object->layer_mask = 1;
        object->floor_height = 2000000.0f;
        object->roof_surface_id = 0xff;
        object->unknown_283 = -1;
        object->jump_animation_state = 0xff;
        RecomputeHighGameObject();
        ((APIOBJECT *)object)->owner = object;
        object->ai_packet.owner = object;
        ((APIOBJECT *)object)->ai = &object->ai_packet;
        ((GAMEOBJECT_COLLISION_RESET *)object)->collision_overlap_low = 0;
        ((GAMEOBJECT_COLLISION_RESET *)object)->collision_overlap_high = 0;
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
        U64_PAIR(&other->unknown_eb0) &= ~((u64)1 << object->hostility_group);
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

i32 HIGHGAMEOBJECT = 0;
void (*GameObjectDimensionsExtraFn)(GAMEOBJECT *) = 0;

void GameObjectDimensions(GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG *config;
    if (object->use_model_origin > 0 && object->origin_scale_override == 1.0f) {
        config = object->character_data->game_character;
        if (config->collision_origin_joint != -1 && object->character_model->points_of_interest[config->collision_origin_joint] != NULL &&
            object->origin_joint_ready != 0) {
            f32 radius = object->character_data->game_character->collision_origin_radius * object->collision_scale;
            object->bounds_half_height = radius;
            object->bounds_radius = radius;
            goto extra;
        }
    }
    object->bounds_radius = object->collision_radius;
    object->bounds_half_height = object->unknown_b0;
extra:
    if (GameObjectDimensionsExtraFn != NULL) {
        GameObjectDimensionsExtraFn(object);
    }
}

void ScaleGameObject(GAMEOBJECT *object) {
    CHARACTERDATA *character = object->character_data;
    object->scaled_radius = character->scaled_radius * object->origin_scale;
    object->collision_radius = object->collision_radius_unscaled * object->origin_scale;
    object->unknown_b0 = object->collision_radius * object->collision_y_scale;
    object->scaled_height = (character->top - character->bottom) * object->collision_scale;
}

void GetTopBot(GAMEOBJECT *object) {
    CHARACTERDATA *character = object->character_data;
    object->character_bottom = character->bottom;
    object->character_top = character->top;
    if (object->collision_radius_unscaled == 0.0f) {
        object->collision_y_scale = 0.0f;
    } else {
        object->collision_y_scale =
            (f32)((f64)(object->character_top - object->character_bottom) / ((f64)object->collision_radius_unscaled * 2.0));
    }
}

void SetToLastSafePos(GAMEOBJECT *object) {
    object->start_position = object->last_safe_position;
    object->position = object->last_safe_position;
    object->ai_update_position = object->last_safe_position;
}

i32 UnderPlayerControl(GAMEOBJECT *object) {
    if ((object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        return 1;
    }
    if (object->takeover_source != NULL && (object->takeover_source->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        return 1;
    }
    return 0;
}

GAMEOBJECT *FindGameObject(i32 character_id, u32 required_flags, i32 alive_only, i32 vehicle_only, i32 non_level_only) {
    GAMEOBJECT *object = Obj;
    i32 i;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0) {
            continue;
        }
        if (vehicle_only != 0 && (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0) {
            continue;
        }
        if (required_flags != 0 && (object->flags_1f8 & required_flags) != required_flags) {
            continue;
        }
        if (character_id != -1 && (i16)object->id != character_id) {
            continue;
        }
        if (alive_only != 0 && (object->dead != 0 || (U64_PAIR(&CInfo[object->character_context].flags) & 0x8000) != 0)) {
            continue;
        }
        if (non_level_only != 0 && object->level_object_index != -1) {
            continue;
        }
        return object;
    }
    return NULL;
}

GAMEOBJECT *FindNearestGameObject(NUVEC *position, GAMEOBJECT *exclude, u32 required_flags, f32 radius, f32 extra_radius, i32 animation,
                                  i32 character_id, i32 player_index, f32 *distance_squared, i32 horizontal_only,
                                  i32 (*filter)(GAMEOBJECT *)) {
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
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 ||
            object->dead != 0 || (object->flags_e20 & 0x20) || (U64_PAIR(&CInfo[object->character_context].flags) & 0x8000)) {
            continue;
        }
        if (character_id != -1 && (i16)object->id != character_id) {
            continue;
        }
        if (player_index >= 0) {
            if (player_index < 2) {
                if ((object->player_index) != player_index) {
                    continue;
                }
            } else if (player_index == 99) {
                if ((object->player_index) == -1) {
                    continue;
                }
            } else if (player_index == 100) {
                if ((object->player_index) != -1) {
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

f32 FRAMETIME = 0;

void GameObjectOrigin(GAMEOBJECT *object) {
    f32 min_y;
    f32 max_y;
    f32 height;
    i32 joint;
    object->flags_1f8 |= 0x100;
    if (object->use_model_origin > 0 && (joint = object->character_data->game_character->model_origin_joint) != -1 &&
        object->character_model != NULL && object->character_model->points_of_interest[joint] != NULL) {
        if ((object->flags_e24 & 8) != 0) {
            object->collision_position.x = object->locator_matrices[joint].m30 + object->previous_velocity.x * FRAMETIME;
            object->collision_position.y = (f32)((f64)(object->locator_matrices[joint].m31 + object->previous_velocity.y * FRAMETIME) +
                                                 (f64)((object->character_bottom + object->character_top) * object->origin_scale) * 0.5);
            object->collision_position.z = object->locator_matrices[joint].m32 + object->previous_velocity.z * FRAMETIME;
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
               object->character_model->points_of_interest[object->character_data->game_character->collision_origin_joint] != NULL &&
               object->origin_joint_ready != 0) {
        object->collision_position =
            *(NUVEC *)&object->locator_matrices[object->character_data->game_character->collision_origin_joint].m30;
    } else {
        object->flags_1f8 ^= 0x100;
        object->collision_position.x = object->position.x + object->previous_velocity.x * FRAMETIME;
        object->collision_position.y = (f32)((f64)(object->position.y + object->previous_velocity.y * FRAMETIME) +
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
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x80000) != 0 && (object->context_variant_flags & 0x800) != 0) {
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

NUVEC plr_lastpos = {0};

void SnapCreaturePos(GAMEOBJECT *object, NUVEC *position, i32 angle, AIPATHINFO *path_info, i32 set_on_surface) {
    object->position = *position;
    object->unknown_276 = (u16)angle;
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
        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT *)object, &object->ai_packet, 0xff, 1);
    }
}

LAST_SAFE_POS_EXTRA LastSafePosExtraFn = 0;

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
    if (!object->dead && (LEGOCONTEXT_DOOMED == -1 || object->character_context != LEGOCONTEXT_DOOMED) && (object->flags_1fc & 4) == 0) {
        object->previous_position = object->position;
        terrain_object =
            object->takeover_source != NULL && LEGOCONTEXT_BEENTAKENOVER != -1 && object->character_context == LEGOCONTEXT_BEENTAKENOVER
                ? object->takeover_source
                : object;
        surface = terrain_object->jump_animation_state;
        if ((surface != -1 && (TerSurface[surface].flags & 0xc041) != 0) ||
            ((layer = terrain_object->unknown_283) != -1 && ((TerLayer[layer].flags & 1) != 0 || layer == 9))) {
            unsafe = 1;
            if (VehicleArea && BonusArea && terrain_object->character_data->game_character->unknown_028 > 0.0f) {
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
            if (object->unknown_281 == 0 &&
                (context == CHARACTER_CONTEXT_NONE ||
                 (context != LEGOCONTEXT_BEENTAKENOVER && context != LEGOCONTEXT_EATEN && context != LEGOCONTEXT_GETIN)) &&
                !VehicleArea) {
                model_flags = object->character_data->model_flags;
                if (((model_flags & CHARACTER_MODEL_VEHICLE) == 0 || !(object->character_data->game_character->unknown_028 > 0.0f)) &&
                    ((model_flags & CHARACTER_MODEL_JETPACK) == 0 || object->jump_variant_state != 1)) {
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
            if ((object->ai_packet.path_info.flags & 1) != 0 && (object->ai_packet.path_info.connection->traversal_flags[1] |
                                                                 object->ai_packet.path_info.connection->traversal_flags[0]) == 0) {
                if (object->takeover_owner != NULL && (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                    (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER)) {
                    object->takeover_owner = NULL;
                }
                object->respawn_position = object->last_safe_position;
                object->flags_1fc_word |= 0x2000;
            }
            if ((object->unknown_280 == 0 || object->unknown_280 == 1) && object->unknown_da8 <= 0.0f &&
                GameCam->blend_time >= GameCam->blend_duration && object->sock_index != 0xff) {
                if (object->takeover_owner != NULL && (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                    (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER)) {
                    object->takeover_owner = NULL;
                }
                object->camera_saved_position = object->last_safe_position;
            }
            goto safe_position_done;
        } while (0);
        if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && (layer = object->unknown_283) != -1 &&
            (TerLayer[layer].flags & 1) != 0 && !NoLayerKill(object)) {
            if (object->fall_recovery_timer <= 0.0f) {
                object->fall_hover_height = object->position.y;
            }
            object->fall_recovery_timer = FRAMETIME + object->fall_recovery_timer;
        } else {
            object->fall_recovery_timer = 0.0f;
        }
        if ((WORLD->current_level->flags & LEVEL_IN_SPACE) != 0) {
            object->last_safe_position = object->position;
        }
    }
safe_position_done:
    if ((object->flags_ef4 & APIOBJECT_IGNORE_LAST_SAFE_PATH_POS) == 0 && (object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 &&
        (LEGOCONTEXT_EATEN == -1 || object->character_context != LEGOCONTEXT_EATEN) && (object->flags_1fc_word & 0x2000) != 0) {
        if ((object->ai_packet.path_info.flags & 1) != 0) {
            if (object->takeover_owner != NULL && (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER)) {
                object->takeover_owner = NULL;
            }
        }
        object->last_safe_position = object->respawn_position;
    }
    object->flags_ef4 &= ~4;
}

u8 grapple_attach_frames = 5;
TERRAIN_LAYER TerLayer[17] = {
    {0x00000000, 0x0000ffff, 0x40000000},
    {0x00000000, 0x0000ffff, 0x40000000},
    {0x00000000, 0x0000ffff, 0x3f800000},
    {0x00000001, 0x0000ffff, 0x3f800000},
    {0x00000000, 0x0000ffff, 0x3f800000},
    {0x00000020, 0x0000ffff, 0x3f800000},
    {0x00000001, 0x0000ffff, 0x3f800000},
    {0x00000000, 0x0000ffff, 0x3f800000},
    {0x00000000, 0x0000ffff, 0x3f800000},
    {0x00000000, 0x0000ffff, 0x3f800000},
    {0x00000000, 0x0000ffff, 0x3f800000},
    {0x00000000, 0x0000ffff, 0x3f800000},
    {0x00000000, 0x0000ffff, 0x3f800000},
    {0x00000000, 0x0000ffff, 0x3f800000},
    {0x00000000, 0x0000ffff, 0x3f800000},
    {0x00000000, 0x0000ffff, 0x3f800000},
    {0x00000000, 0x0000ffff, 0x00000000},
};

void GameObjectSetCanUse(GAMEOBJECT *object, void *target, u8 action, u8 unused, f32 parameter) {
    object->can_use_object = target;
    object->unknown_f00 = action;
    object->use_action_frames = grapple_attach_frames;
    object->use_action_parameter = parameter;
}

/* Bitwise absolute value through a float temporary. */

// Copies the antinode timer and its 0x8000 flag from one creature's packet to the other's.
typedef void (*CHECK_ANTINODE_FN)(APIOBJECT *, AIANTINODE *, NUVEC *, f32);
extern CHECK_ANTINODE_FN checkantinodefns[3];

typedef struct OPPONENT_PACKET_PC {      // size 0x1ec
    u8 unknown_000[0xd4];                // 0x000
    APIOBJECT *nearest_opponent_object;  // 0x0d4
    f32 nearest_opponent_metric;         // 0x0d8
    APIOBJECT *pending_nearest_opponent; // 0x0dc
    f32 pending_nearest_metric;          // 0x0e0
    APIOBJECT *opponent_object;          // 0x0e4
    f32 opponent_metric;                 // 0x0e8
    APIOBJECT *pending_opponent;         // 0x0ec
    f32 pending_opponent_metric;         // 0x0f0
    u8 unknown_0f4[0xf4];                // 0x0f4
    u32 flags;                           // 0x1e8
} OPPONENT_PACKET_PC;

typedef struct OPPONENT_GAME_PC { // size 0xec8
    u8 unknown_000[0xeb0];        // 0x000
    u64 seen;                     // 0xeb0
    u64 exclusion;                // 0xeb8
    APIOBJECT *alert_target;      // 0xec0
    f32 alert_timer;              // 0xec4
} OPPONENT_GAME_PC;

typedef struct OPPONENT_LOS_PC { // size 0x208
    u32 object_size;             // 0x000
    GAMEOBJECT *objects;         // 0x004
    u64 line_of_sight[64];       // 0x008
} OPPONENT_LOS_PC;

typedef struct ai_frame_world_s {        // size 0x4670
    u8 unknown_0000[0x2aec];             // 0x0000
    i32 processor_count;                 // 0x2aec
    LEVEL_SCRIPT_PROCESS processors[32]; // 0x2af0
} AI_FRAME_WORLD;
