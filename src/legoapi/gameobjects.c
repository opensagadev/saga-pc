#include "saga.h"

typedef struct grapple_rotation_pc_s {
    u8 unknown_00[0x30];
    NUVEC hook_position;
} GRAPPLE_ROTATION_PC;
STATIC_ASSERT(offsetof(GRAPPLE_ROTATION_PC, hook_position) == 0x30);

extern const f64 GameObjectNearFloorNoFloorHeight;
extern const f64 GameObjectNearFloorRadiusDivisor;

i32 __cdecl GameObjectNearFloor(GAMEOBJECT *object, f32 height, f32 *distance) {
    f32 tolerance;
    f32 radius_tolerance;
    f32 floor_distance;

    if (height < 0.0f) {
        height = 0.0f;
    }
    tolerance = (f32)((f64)height * NuTimeBarSlotWidth);

    if ((f64)object->floor_height == GameObjectNearFloorNoFloorHeight) {
        if (distance != NULL) {
            *distance = LevelConfigFarDistance;
        }
        return 0;
    }

    radius_tolerance = (f32)(((f64)object->collision_radius / GameObjectNearFloorRadiusDivisor) * tolerance);
    if (radius_tolerance > tolerance) {
        tolerance = radius_tolerance;
    }

    floor_distance = object->collision_min.y - object->floor_height;
    if (distance != NULL) {
        *distance = floor_distance;
    }
    return tolerance > floor_distance;
}

i16 __cdecl NuPortalWhichRoom(NUGSCN *scene, NUVEC *position);

void __cdecl PortalGameObject(GAMEOBJECT *object, i32 enabled, i32 update_room, i16 room, NUGSCN *scene) {
    if (enabled != 0) {
        object->portal_flags = ((object->portal_flags ^ ((update_room & 1) << 23)) & 0x00800000) ^ object->portal_flags;
        object->portal_flags |= 0x00400000;
        if (room != -1) {
            object->portal_room = room;
        } else if (scene != NULL) {
            object->portal_room = NuPortalWhichRoom(scene, &object->collision_position);
        } else {
            object->portal_room = -1;
        }
    } else {
        object->portal_flags &= ~0x00400000;
    }
}

extern u16 GameObjectRotationBuggyId;
extern u16 GameObjectRotationRadioCarId;
extern u16 GameObjectRotationPoliceCarId;
extern u16 GameObjectRotationMowerId;
extern u16 GameObjectRotationAtstId;
extern i32 VehicleArea;
extern u16 temp_xrot;
extern u16 temp_zrot;
extern i32 disable_narrow_socks;

extern const f64 VehicleHoverHeightSeparationScale;
extern i32 getvehiclehoverheight_hothbattlehack;

f32 __cdecl GetVehicleHoverHeight(GAMEOBJECT *object, f32 *separation_offset) {
    GAMEOBJECT *player0 = (GAMEOBJECT *)Player[0];
    GAMEOBJECT *player1 = (GAMEOBJECT *)Player[1];
    WORLDINFO *world = WORLD;
    LEVELDATA_PC *level = world->current_level;
    f32 height = object->hover_height_override;
    i32 separate;

    if ((f64)height == MoveSplineRunSpeedSentinel) {
        height = level->hover_height;
        if (height == 0.0f) {
            height = object->character_data->game_character->field_028;
            if (object->id == id_SPEEDERBIKE && level == SPEEDERCHASEA_LDATA && disable_narrow_socks != 0) {
                height = (f32)((f64)height * NuQFntHalfScale);
            }
        }
    }

    if (level == SPEEDERCHASEA_LDATA) {
        separate = disable_narrow_socks == 0 && object->field_280 != 0xff &&
                   object->movement_spline_position.spline == NULL && player0 != NULL &&
                   player0->takeover_source != NULL && player0->id == id_SPEEDERBIKE && player1 != NULL &&
                   player1->takeover_source != NULL && player0->id == player1->id;
    } else {
        separate = (object->flags_1fc & 0x80) != 0 && player0 != NULL &&
                   (player0->character_data->model_flags & 0x2000) != 0 &&
                   (((player0->flags_1fc & 0x80) != 0) || player0->character_context == 0x24) && player1 != NULL &&
                   (player1->character_data->model_flags & 0x2000) != 0 &&
                   (((player1->flags_1fc & 0x80) != 0) || player1->character_context == 0x24);
    }

    if (separate) {
        f32 combined_height =
            (f32)((f64)(player0->bounds_half_height + player1->bounds_half_height) * VehicleHoverHeightSeparationScale);
        f32 combined_radius = player0->bounds_radius + player1->bounds_radius;
        f32 inner_distance = combined_radius * combined_radius;
        f32 outer_radius = (f32)((f64)combined_radius * NuTimeBarColumnStep);
        f32 outer_distance = outer_radius * outer_radius;
        f32 distance = NuVecXZDistSqr(&player0->position, &player1->position, NULL);
        f32 blend;
        f32 offset;
        f32 adjusted_height;
        f32 correction = 0.0f;

        if (distance < inner_distance) {
            blend = 1.0f;
        } else if (distance < outer_distance) {
            blend = 1.0f - (distance - inner_distance) / (outer_distance - inner_distance);
        } else {
            blend = 0.0f;
        }

        offset = blend * combined_height;
        adjusted_height = height - offset;
        if (adjusted_height < offset) {
            correction = offset - adjusted_height;
        }
        if (player0 == object) {
            adjusted_height = height + offset;
            if (separation_offset != NULL) {
                *separation_offset = offset;
            }
        } else if (separation_offset != NULL) {
            *separation_offset = -offset;
        }
        height = adjusted_height + correction;
        level = world->current_level;
    } else if (separation_offset != NULL) {
        *separation_offset = 0.0f;
    }

    if (level == HOTHBATTLEA_LDATA && (TerLayer[object->field_283].flags & 1) != 0) {
        height = 0.0f;
        getvehiclehoverheight_hothbattlehack = 1;
    } else {
        getvehiclehoverheight_hothbattlehack = 0;
    }
    return height;
}

u16 __cdecl SeekRot(u16 current, u16 target, f32 speed);

void __cdecl FindAnglesZX(NUVEC *normal, u16 *x_rotation, u16 *z_rotation) {
    NUANG x_angle = NuAtan2D(normal->z, normal->y);
    NUVEC rotated;
    NUANG z_angle;

    if (x_rotation != NULL) {
        *x_rotation = x_angle;
    }
    temp_zrot = x_angle;
    NuVecRotateX(&rotated, normal, (NUANG)(0 - x_angle));
    z_angle = (NUANG)(0 - NuAtan2D(rotated.x, rotated.y));
    if (z_rotation != NULL) {
        *z_rotation = z_angle;
    }
    temp_xrot = z_angle;
}

void __cdecl GameObjectRotation(GAMEOBJECT *object, i32 mode) {
    PLAYERCHARACTERCONFIG_PC *game = object->character_data->game_character;
    u32 flags = game->flags_090;
    u16 x;
    u16 z;
    i32 sliding;
    i32 magnet;
    i32 grapple;
    i32 alignment = 0;
    i32 reflection;

    if ((flags & 0x200000) != 0) {
        STUBBED_NAMED("GameObjectRotation: choose vehicle suspension offsets, query four terrain points, update "
                      "suspension springs, and reconstruct suspension normal (PC terrain helper at 0x5506f0)");
    }

    if ((object->flags_1fc & 4) == 0) {
        if ((flags & 0x200000) != 0) {
            FindAnglesZX(&object->suspension_normal, &object->field_1056, &object->field_1058);
        } else if (object->surface_normal.x != 0.0f || object->surface_normal.y != 1.0f ||
                   object->surface_normal.z != 0.0f) {
            FindAnglesZX(&object->surface_normal, &object->field_1056, &object->field_1058);
        } else {
            object->field_1056 = 0;
            object->field_1058 = 0;
        }
    }

    temp_xrot = object->field_1058;
    temp_zrot = object->field_1056;
    x = object->field_1056;
    z = object->field_1058;
    game = object->character_data->game_character;
    flags = game->flags_090;
    sliding = (flags & 0x200) != 0 && object->character_context == 0x33;
    magnet = !sliding && (flags & 0x10000000) != 0 && object->character_context == 0x44;
    grapple = !sliding && object->character_context == 0x46 && object->field_7a3 == 0;

    if (object->flags_107a == 2 &&
        ((object->character_data->model_flags & 0x100) != 0 || (flags & 0x200000) != 0 || sliding)) {
        f32 hover =
            (object->character_data->model_flags & 0x2000) != 0 ? GetVehicleHoverHeight(object, NULL) : game->field_028;
        if (hover > 0.0f) {
            alignment = 2;
            if ((f64)object->floor_height == GameObjectNearFloorNoFloorHeight) {
                x = 0;
                z = 0;
            } else {
                f32 height = object->collision_min.y - object->floor_height;
                f32 fraction;
                if (hover > height) {
                    fraction = 1.0f;
                } else if (2.5f * hover > height) {
                    fraction = 1.0f - (height - hover) / (2.5f * hover - hover);
                } else {
                    fraction = 0.0f;
                }
                x = (u16)((f32)RotDiff(0, temp_zrot) * fraction);
                z = (u16)((f32)RotDiff(0, temp_xrot) * fraction);
            }
        } else {
            alignment = 1;
            if (object->field_281 == 0 && (flags & 0x200000) == 0 && !sliding) {
                alignment = 0;
                if ((object->character_data->model_flags & 0x100) != 0 && game->field_028 == 0.0f &&
                    (flags & 0x100) != 0 && (f64)object->floor_height != GameObjectNearFloorNoFloorHeight) {
                    alignment = object->bounds_half_height > object->collision_min.y - object->floor_height;
                }
            }
        }

        if (object->id == GameObjectRotationAtstId && VehicleArea == 0) {
            x = (u16)((f32)RotDiff(0, temp_zrot) * 0.5f);
            z = (u16)((f32)RotDiff(0, temp_xrot) * 0.5f);
        }
    }

    reflection = (object->character_data->model_flags & 0x2000) != 0 && game->field_028 > 0.0f &&
                 (object->field_283 == 0x10 || object->field_283 == 7) && object->field_280 == 0xff;

    if (mode == 1) {
        object->field_1052 = x;
        object->field_1054 = z;
        if (alignment == 0) {
            object->pitch_angle = object->roll_angle = 0;
        } else if (magnet) {
            if ((u16)(object->field_7aa - 5) <= 1) {
                object->pitch_angle = object->roll_angle = 0;
            } else {
                object->pitch_angle = object->field_7a4;
                object->roll_angle = object->field_7a8;
            }
        } else if (grapple) {
            NUVEC direction;
            GRAPPLE_ROTATION_PC *hook = (GRAPPLE_ROTATION_PC *)object->grapple;
            NuVecSub(&direction, &hook->hook_position, &object->collision_position);
            FindAnglesZX(&direction, &object->pitch_angle, &object->roll_angle);
        } else if (reflection) {
            object->pitch_angle = object->reflection_x;
            object->roll_angle = object->reflection_z;
        } else if (alignment == 2 || object->surface_normal.y > NuTrigTable[0x3000]) {
            object->pitch_angle = x;
            object->roll_angle = z;
        } else {
            object->pitch_angle = object->roll_angle = 0;
        }
    } else if (mode == 2) {
        if (object->field_1052 != object->field_1056) {
            object->field_1052 = SeekRot(object->field_1052, x, 8.0f);
        }
        if (object->field_1054 != object->field_1058) {
            object->field_1054 = SeekRot(object->field_1054, z, 8.0f);
        }
        if (object->flags_107a != 2) {
            return;
        }

        if (magnet) {
            if ((u16)(object->field_7aa - 5) <= 1) {
                object->pitch_angle = 0;
                object->roll_angle = 0;
                x = 0;
                z = 0;
            } else {
                x = object->field_7a4;
                z = object->field_7a8;
            }
        } else if (grapple) {
            NUVEC direction;
            GRAPPLE_ROTATION_PC *hook = (GRAPPLE_ROTATION_PC *)object->grapple;
            NuVecSub(&direction, &hook->hook_position, &object->collision_position);
            FindAnglesZX(&direction, &x, &z);
        } else if (reflection) {
            x = object->reflection_x;
            z = object->reflection_z;
        } else if (alignment != 2 && (alignment == 0 || object->surface_normal.y <= NuTrigTable[0x3000])) {
            x = 0;
            z = 0;
        }

        {
            f32 speed = object->id == GameObjectRotationAtstId && VehicleArea == 0 ? 3.0f : 8.0f;
            if (object->pitch_angle != x) {
                object->pitch_angle = SeekRot(object->pitch_angle, x, speed);
            }
            if (object->roll_angle != z) {
                object->roll_angle = SeekRot(object->roll_angle, z, speed);
            }
        }
    }
}

void __cdecl GameObjectOrigin(GAMEOBJECT *object) {
    f32 x;
    f32 z;
    object->flags_1f8 |= 0x100;
    if (object->use_model_origin != 0) {
        STUBBED_NAMED(GameObjectOrigin_ModelJointSelection);
    } else {
        object->flags_1f8 ^= 0x100;
        object->collision_position.x = object->position.x + object->previous_velocity.x * FRAMETIME;
        object->collision_position.y = object->position.y + object->previous_velocity.y * FRAMETIME;
        object->collision_position.z = object->position.z + object->previous_velocity.z * FRAMETIME;
        object->collision_position.y +=
            (object->character_bottom + object->character_top) * object->origin_scale * 0.5f;
    }
    x = object->position.x + object->previous_velocity.x * FRAMETIME;
    z = object->position.z + object->previous_velocity.z * FRAMETIME;
    object->collision_min.x = object->collision_position.x - object->bounds_radius;
    object->collision_max.x = object->collision_position.x + object->bounds_radius;
    object->collision_min.z = object->collision_position.z - object->bounds_radius;
    object->collision_max.z = object->collision_position.z + object->bounds_radius;
    object->collision_min.y = object->collision_position.y - object->bounds_half_height;
    object->collision_max.y = object->collision_position.y + object->bounds_half_height;
    object->upper_position.x = x;
    object->upper_position.y = object->collision_max.y;
    object->upper_position.z = z;
    object->lower_position.x = x;
    object->lower_position.y = object->collision_min.y;
    object->lower_position.z = z;
    object->collision_origin.x = x;
    object->collision_origin.y = object->collision_min.y + object->previous_velocity.y * FRAMETIME;
    object->collision_origin.z = z;
    object->ai_terrain_origin = object->collision_origin;
    {
        u32 context_flags = CInfo[object->character_context].flags;
        if ((context_flags & 0x80000) != 0 && (object->context_variant_flags & 0x800) != 0) {
            object->ai_terrain_origin.y += object->collision_max.y - object->collision_min.y;
        } else if ((context_flags & 0x100000) != 0) {
            object->ai_terrain_origin = object->position;
        } else {
            object->ai_terrain_origin.y -= object->terrain_origin_floor_offset;
            if (object->ai_terrain_origin.y < object->floor_height &&
                object->floor_height < object->collision_origin.y + object->scaled_height) {
                object->ai_terrain_origin.y = object->floor_height;
            }
        }
    }
}

GAMEOBJECT *__cdecl FindGameObject(i32 character_id, u32 required_flags, i32 alive_only, i32 vehicle_only,
                                   i32 non_level_only) {
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
        if (alive_only != 0 && (object->dead != 0 || (CInfo[object->character_context].flags & 0x8000) != 0)) {
            continue;
        }
        if (non_level_only != 0 && object->level_object_index != -1) {
            continue;
        }
        return object;
    }
    return NULL;
}

GAMEOBJECT *__cdecl GetNamedGameObject(void *sys, char *name) {
    GAMEOBJECT **result;

    if (GetNamedGameObjectCallback == NULL) {
        return NULL;
    }
    result = GetNamedGameObjectCallback(sys, name);
    if (result == NULL) {
        return NULL;
    }
    return *result;
}

void __cdecl MakeBaddiesForgetAboutParty(i32 check_hostility) {
    GAMEOBJECT *baddies[64];
    GAMEOBJECT *party[64];
    i32 party_count = 0;
    i32 baddie_count = 0;
    GAMEOBJECT *object = Obj;
    i32 count = HIGHGAMEOBJECT;
    i32 i, j;
    for (i = 0; i < count; ++i, ++object) {
        if ((object->flags_1fc_word & 0x1001) != 0x1001 || object->dead != 0)
            continue;
        if ((object->flags_1f8 & 1) != 0)
            party[party_count++] = object;
        else if ((object->flags_1f8 & 4) == 0)
            baddies[baddie_count++] = object;
    }
    if (check_hostility != 0) {
        for (i = 0; i < baddie_count; ++i) {
            u64 mask = (u64)1 << baddies[i]->hostility_group;
            for (j = 0; j < party_count; ++j) {
                GAMEOBJECT *member = party[j];
                u64 hostile = (u64)member->hostility_mask[0] | ((u64)member->hostility_mask[1] << 32);
                if ((hostile & mask) != 0) {
                    u32 *defaults = WORLD->object_sys->hostility_masks[member->hostility_group];
                    if ((((u64)defaults[0] | ((u64)defaults[1] << 32)) & mask) != 0)
                        return;
                }
            }
        }
    }
    for (i = 0; i < baddie_count; ++i) {
        GAMEOBJECT *baddie = baddies[i];
        for (j = 0; j < party_count; ++j) {
            GAMEOBJECT *member = party[j];
            u64 mask = ~((u64)1 << baddie->hostility_group);
            u64 other_mask;
            member->hostility_mask[0] &= (u32)mask;
            member->hostility_mask[1] &= (u32)(mask >> 32);
            other_mask = ~((u64)1 << member->hostility_group);
            baddie->hostility_mask[0] &= (u32)other_mask;
            baddie->hostility_mask[1] &= (u32)(other_mask >> 32);
        }
    }
    object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & 0x1001) != 0x1001 || object->dead != 0)
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
        object->hostility_mask[0] = object->hostility_mask[1] = 0;
        object->field_eb0 = 0;
        object->field_eb4 = 0;
        object->field_ec0 = 0;
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
            if (player == NULL || (player->flags_1fc_word & 0x1001) != 0x1001 || player->dead != 0 ||
                (player->flags_ef0 & 0x01000000) != 0)
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

void __cdecl ScaleGameObject(GAMEOBJECT *object) {
    f32 scale = object->origin_scale;
    CHARACTERDATA_PC *character = object->character_data;
    object->scaled_radius = character->scaled_radius * scale;
    object->collision_radius = object->collision_radius_unscaled * scale;
    object->field_b0 = object->collision_radius * object->collision_y_scale;
    object->scaled_height = (character->top - character->bottom) * object->collision_scale;
}

void __cdecl GetTopBot(GAMEOBJECT *object) {
    CHARACTERDATA_PC *character = object->character_data;
    f32 bottom = character->bottom;
    f32 top = character->top;
    object->character_bottom = bottom;
    object->character_top = top;
    if (object->collision_radius_unscaled == 0.0f) {
        object->collision_y_scale = 0.0f;
    } else {
        object->collision_y_scale = (f32)((f64)(top - bottom) / ((f64)object->collision_radius_unscaled * 2.0));
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
    STUBBED_NAMED("SnapCreaturePos: initialize surface information (PC 0x5728a0)");
    if (set_on_surface != 0) {
        STUBBED_NAMED("SnapCreaturePos: set object on surface (PC 0x572660)");
    }
    if (path_info != NULL) {
        object->ai_packet.path_info = *path_info;
    } else {
        STUBBED_NAMED("SnapCreaturePos: query character path position (PC 0x52f800)");
    }
}

APIOBJECT_PC *__cdecl GameAPIOBJECTFromObjID(u8 object_id) {
    GAMEOBJECT *object;
    if ((i32)object_id >= HIGHGAMEOBJECT) {
        return NULL;
    }
    object = &Obj[object_id];
    if ((object->flags_1fc & 1) == 0) {
        return NULL;
    }
    if ((object->flags_1fc_word & 0x1000) == 0 && object->ai_packet.reset_mode != 3) {
        return NULL;
    }
    if (object->dead != 0 && !(object->field_1010 > 0.0f)) {
        return NULL;
    }
    return (APIOBJECT_PC *)object;
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
    f32 nearest_distance = range_squared;
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
    max_x = position->x + radius;
    min_z = position->z - radius;
    max_z = position->z + radius;
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
        if (context == 0x39 || context == 0x3b || context == 0x3c || (CInfo[context].flags & 0x8000) != 0) {
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
            u32 *mask = WORLD->object_sys->hostility_masks[object->hostility_group];
            u64 hostility = (u64)mask[0] | ((u64)mask[1] << 32);
            if ((hostility & ((u64)1 << target->hostility_group)) == 0) {
                continue;
            }
        }
        if (target->collision_position.x < min_x || target->collision_position.x > max_x ||
            target->collision_position.z < min_z || target->collision_position.z > max_z) {
            continue;
        }
        if (WORLD->current_level == DEATHSTARRESCUEA_LDATA && target->field_280 == 0xff && target->position.y < -1.5f &&
            GameCam->sock == 2) {
            continue;
        }
        distance = NuVecDistSqr(&target->collision_position, position, &delta);
        if (nearest_distance > distance) {
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
                if (object->collision_target == target) {
                    previous = target;
                } else {
                    nearest_distance = distance;
                    best = target;
                }
            }
        }
    }
    return best != NULL ? best : previous;
}
