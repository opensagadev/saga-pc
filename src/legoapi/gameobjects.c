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
    CHARACTER_MODEL_LAYERS_PC *model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
    PLAYERCHARACTERCONFIG_PC *config;
    i32 joint;
    object->flags_1f8 |= 0x100;
    if (object->use_model_origin != 0) {
        config = object->character_data->game_character;
        joint = config->model_origin_joint;
        if (joint != -1 && model != NULL && model->points_of_interest[joint] != NULL) {
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
            goto origin_ready;
        }
        joint = config->collision_origin_joint;
        if (object->origin_scale_override == 1.0f && joint != -1 && model->points_of_interest[joint] != NULL &&
            object->origin_joint_ready != 0) {
            object->collision_position.x = object->locator_matrices[joint].m30;
            object->collision_position.y = object->locator_matrices[joint].m31;
            object->collision_position.z = object->locator_matrices[joint].m32;
            goto origin_ready;
        }
    }
    object->flags_1f8 ^= 0x100;
    object->collision_position.x = object->position.x + object->previous_velocity.x * FRAMETIME;
    object->collision_position.y =
        (f32)((f64)(object->position.y + object->previous_velocity.y * FRAMETIME) +
              (f64)((object->character_bottom + object->character_top) * object->origin_scale) * 0.5);
    object->collision_position.z = object->position.z + object->previous_velocity.z * FRAMETIME;
origin_ready:
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
    APIOBJECT_PC *result;

    if (GetNamedGameObjectCallback == NULL) {
        return NULL;
    }
    result = GetNamedGameObjectCallback(sys, name);
    if (result == NULL) {
        return NULL;
    }
    return (GAMEOBJECT *)result->owner;
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
        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
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

void __cdecl GameObjectToCameraCode(GAMEOBJECT *object) {
    f32 distance;
    LEVELDATA_PC *level;
    i32 far_distance;
    u32 near_distance;
    PLAYERCHARACTERCONFIG_PC *data;
    u32 mask;

    if (GameTimer.update_count == 0) {
        object->shadow_opacity = 0.0f;
        return;
    }
    distance = object->ai_update_distance;
    level = WORLD->current_level;
    far_distance = level->blob_shadow_fade_far;
    if (distance < (f32)far_distance) {
        near_distance = level->blob_shadow_fade_near;
        if (distance > (f32)near_distance) {
            object->shadow_opacity = 1.0f - (distance - (f32)near_distance) / (f32)(i32)(far_distance - near_distance);
        } else {
            object->shadow_opacity = 1.0f;
        }
    } else {
        object->shadow_opacity = 0.0f;
    }
    data = object->character_data->game_character;
    if ((data->flags_094 & 0x10) != 0) {
        return;
    }
    if (object->dead != 0) {
        i16 model_id = ((CHARACTER_MODEL_LAYERS_PC *)object->character_model)->model_id;
        PLAYERCHARACTERCONFIG_PC *model_data = apicharsys->char_data[model_id].game_character;
        mask = model_data->layer_mask_dead;
        if (mask != 0) {
            goto apply_layers;
        }
    }
    if (distance > data->layer_distance_low) {
        mask = data->layer_mask_low;
    } else if (distance > data->layer_distance_medium) {
        mask = data->layer_mask_medium;
    } else {
        mask = data->layer_mask;
    }
apply_layers:
    object->layer_mask = mask | object->extra_layer_mask;
    object->layer_mask = AdjustLayerBits(object->layer_mask, object);
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

static i32 GetBladeJoints(GAMEOBJECT *object, i32 blade, i32 joints[2], i32 *streak_blade) {
    i32 local_blade;
    PLAYERCHARACTERCONFIG_PC *data;
    if (object->origin_joint_ready == 0 || (object->flags_e20 & 0x08000000) == 0) {
        return 0;
    }
    if (streak_blade == NULL) {
        streak_blade = &local_blade;
    }
    *streak_blade = blade;
    data = object->character_data->game_character;
    joints[0] = data->streak_joints[blade][0];
    if (joints[0] == -1 ||
        ((CHARACTER_MODEL_LAYERS_PC *)object->character_model)->points_of_interest[joints[0]] == NULL) {
        return 0;
    }
    data = object->character_data->game_character;
    joints[1] = data->streak_joints[*streak_blade][1];
    if (joints[1] == -1 ||
        ((CHARACTER_MODEL_LAYERS_PC *)object->character_model)->points_of_interest[joints[1]] == NULL) {
        return 0;
    }
    return 1;
}

static void LightSabreStreakCode(GAMEOBJECT *object, i32 blade, i32 effect) {
    i8 context;
    i32 joints[2];
    i32 streak_blade;
    CHARACTER_MODEL_LAYERS_PC *model;
    PLAYERCHARACTERCONFIG_PC *data;
    i32 joint_a;
    i32 joint_b;
    NUVEC points[3];
    NUVEC difference;
    NUVEC minimum;
    NUVEC maximum;
    f32 length;
    f32 extent;
    GAMEOBJECT *nearest;
    i32 nearest_point;
    f32 nearest_distance;
    i32 i;
    if (object->weapon_scale < 1.0f)
        return;
    context = object->character_context;
    if ((context == 0 && (object->action_movement_state == 4 || object->action_movement_state == 2)) || context == 4 ||
        context == 16 || (context == 13 && object->id != id_THEEMPEROR) || context == 14 ||
        (context == 31 && object->field_7b3 == 1) ||
        ((object->flags_1fc_word & 0x80) != 0 && Cheat_PowerUpActive((i8)object->field_280))) {
        object->sabre_flags |= 2;
    }
    if (object->id == id_IMPERIALGUARD)
        object->sabre_flags &= ~3;
    if (object->sabre_flags == 0)
        return;
    if (!GetBladeJoints(object, blade, joints, &streak_blade))
        return;
    model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
    data = object->character_data->game_character;
    joint_a = joints[0];
    joint_b = joints[1];
    object->blade_states[blade] = -1;
    points[0].x = object->locator_matrices[joint_a].m30;
    points[0].y = object->locator_matrices[joint_a].m31;
    points[0].z = object->locator_matrices[joint_a].m32;
    points[1].x = object->locator_matrices[joint_b].m30;
    points[1].y = object->locator_matrices[joint_b].m31;
    points[1].z = object->locator_matrices[joint_b].m32;
    if ((object->sabre_flags & 2) != 0) {
        i32 colour;
        if (object->field_280 != 0xff && Cheat_IsOn(25))
            colour = 0;
        else if (object->field_280 != 0xff && Player_HasPurpleForce(object))
            colour = 3;
        else if (object->id == id_GRIEVOUS && (blade == 3 || blade == 0))
            colour = 2;
        else if (AnakinGreenSabre(object) || (object->id == id_BOB && (object->flags_ef0 & 0x200) != 0))
            colour = 1;
        else
            colour = GCDataList[(i16)object->id].blade_colour;
        object->blade_states[blade] = colour;
        if (object->model_draw_result != 0) {
            const u8 *rgb = BladeTab[colour].colour;
            const u32 packed = 0xff000000u | rgb[0] | (rgb[1] << 8) | (rgb[2] << 16);
            STUBBED_NAMED("LightSabre: emit direct streak PC5a3730");
            if (object->reflection_enabled != 0 && object->reflection_height != 2000000.0f) {
                f32 plane = object->reflection_height;
                NUVEC reflected[2];
                if (WORLD->current_level->reflection_y != 2000000.0f)
                    plane = WORLD->current_level->reflection_y;
                reflected[0] = points[0];
                reflected[1] = points[1];
                reflected[0].y = plane - (reflected[0].y - plane);
                reflected[1].y = plane - (reflected[1].y - plane);
                STUBBED_NAMED("LightSabre: emit reflected streak PC5a3730");
            }
        }
    }
    if ((object->sabre_flags & 7) == 0)
        return;
    if (object->field_280 != 0xff && Cheat_IsOn(25))
        effect = 1;
    else if (object->field_280 != 0xff && Player_HasPurpleForce(object))
        effect = 4;
    else if (object->id == id_GRIEVOUS)
        effect = (blade == 3 || blade == 0) ? 3 : 2;
    if (data->weapon_action == 12 && context == 5 && object->combo_stage == 2 && object->field_7aa == 52 &&
        model->points_of_interest[4] != NULL) {
        points[0].x = object->locator_matrices[4].m30;
        points[0].y = object->locator_matrices[4].m31;
        points[0].z = object->locator_matrices[4].m32;
        NuVecAdd(&points[1], &points[0], &object->collision_position);
        NuVecScale(&points[1], &points[1], 0.5f);
    }
    length = NuVecDist(&points[0], &points[1], &difference);
    points[2].x = difference.x * 0.5f + points[1].x;
    points[2].y = difference.y * 0.5f + points[1].y;
    points[2].z = difference.z * 0.5f + points[1].z;
    object->sabre_collision_radius = length * 0.25f;
    extent = length * 1.5f;
    minimum.x = points[2].x - extent;
    minimum.y = points[2].y - extent;
    minimum.z = points[2].z - extent;
    maximum.x = points[2].x + extent;
    maximum.y = points[2].y + extent;
    maximum.z = points[2].z + extent;
    if ((object->sabre_flags & 4) != 0) {
        NUVEC direction;
        NuVecRotateY(&direction, &v001, (u16)(object->field_0x276 + (context == 16 ? 0x8000 : 0)));
        NuVecScale(&difference, &direction, 0.2f);
        NuVecAdd(&points[0], &object->collision_position, &difference);
        NuVecScale(&difference, &direction, 0.5f);
        NuVecAdd(&points[1], &object->collision_position, &difference);
        NuVecScale(&difference, &direction, 0.35f);
        NuVecAdd(&points[2], &object->collision_position, &difference);
        if (context == 13) {
            points[0].y = (f32)((f64)object->lower_position.y +
                                (f64)(object->upper_position.y - object->lower_position.y) * (f64)0.333f);
            points[1].y = points[2].y = points[0].y;
        }
        length = NuVecDist(&points[0], &points[1], &difference);
        points[2].x = difference.x * 0.5f + points[1].x;
        points[2].y = difference.y * 0.5f + points[1].y;
        points[2].z = difference.z * 0.5f + points[1].z;
        object->sabre_collision_radius = length * 0.25f;
        extent = length * 1.5f;
        minimum.x = points[2].x - extent;
        minimum.y = points[2].y - extent;
        minimum.z = points[2].z - extent;
        maximum.x = points[2].x + extent;
        maximum.y = points[2].y + extent;
        maximum.z = points[2].z + extent;
    }
    if ((object->sabre_flags & 5) == 0)
        goto particle_contact;
    nearest = NULL;
    nearest_point = 0;
    nearest_distance = 1000000.0f;
    for (i = 0; i < HIGHGAMEOBJECT; ++i) {
        GAMEOBJECT *target = &Obj[i];
        PLAYERCHARACTERCONFIG_PC *target_data;
        i32 point;
        if (target == object || (target->flags_1fc_word & 0x1001) != 0x1001 || target->use_model_origin <= 1 ||
            target->dead != 0 || (target->character_context & 0xfd) == 57 || target->character_context == 60 ||
            (CInfo[target->character_context].flags & 0x8000) != 0)
            continue;
        target_data = target->character_data->game_character;
        if ((target_data->flags_090 & 0x8000) != 0 || target->collision_min.x > maximum.x ||
            target->collision_max.x < minimum.x || target->collision_min.z > maximum.z ||
            target->collision_max.z < minimum.z || target->collision_min.y > maximum.y ||
            target->collision_max.y < minimum.y)
            continue;
        for (point = 2; point >= 0; --point) {
            f32 distance;
            if (!SphereSphereOverlapScaleY(&target->collision_position, target->bounds_radius,
                                           target->bounds_half_height, &points[point], object->sabre_collision_radius,
                                           object->sabre_collision_radius))
                continue;
            if ((object->sabre_flags & 1) != 0) {
                if ((target->flags_1fc_word & 2) == 0 && (target_data->flags_090 & 0x1040) == 0) {
                    NUVEC reaction;
                    f32 maximum_speed = target_data->run_speed;
                    f32 magnitude_squared;
                    NuVecSub(&reaction, &target->collision_position, &object->collision_position);
                    if (reaction.x == 0.0f && reaction.z == 0.0f) {
                        reaction.x = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) - 0.5);
                        reaction.z = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) - 0.5);
                    }
                    NuVecNorm(&reaction, &reaction);
                    target->movement_direction_x = target_data->walk_speed * reaction.x + target->velocity.x;
                    target->movement_direction_z = target_data->walk_speed * reaction.z + target->velocity.z;
                    magnitude_squared = target->movement_direction_x * target->movement_direction_x +
                                        target->movement_direction_z * target->movement_direction_z;
                    if (magnitude_squared > maximum_speed * maximum_speed) {
                        f32 scale = maximum_speed / NuFsqrt(magnitude_squared);
                        target->movement_direction_x *= scale;
                        target->movement_direction_z *= scale;
                    }
                }
                STUBBED_NAMED("LightSabre: AddGameDebrisRot PC62c680");
                if (SaberSparksTimer <= 0.0f && object->anim_blending == 0 && object->character_context != 5 &&
                    object->character_context != 16 && object->character_context != 13 &&
                    object->character_context != 14 && (CInfo[object->character_context].flags & 0x04000000) == 0 &&
                    ((CInfo[object->character_context].flags & 0x08000000) == 0 ||
                     (object->context_variant_flags & 0x20000) == 0) &&
                    target->character_context != 5 && target->character_context != 16 &&
                    target->character_context != 13 && target->character_context != 14 &&
                    (CInfo[target->character_context].flags & 0x04000000) == 0 &&
                    ((CInfo[target->character_context].flags & 0x08000000) == 0 ||
                     (target->context_variant_flags & 0x20000) == 0)) {
                    PlaySfx((char *)SaberSparksName, &points[point]);
                    SaberSparksTimer = (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f)) * (f64)0.3f + (f64)0.3f);
                    qrand();
                    STUBBED_NAMED("LightSabre: source randomized rumble PC54a850");
                    qrand();
                    STUBBED_NAMED("LightSabre: target randomized rumble PC54a850");
                }
            }
            distance = NuVecDistSqr(&object->collision_position, &target->collision_position, NULL);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest_point = point;
                nearest = target;
            }
        }
    }
    if (nearest != NULL && (object->sabre_flags & 4) != 0) {
        STUBBED_NAMED("LightSabre: closest-target debris PC62c560");
        if (!CannotKill(nearest)) {
            i32 damage = object->sabre_damage;
            if (damage != 0 && (nearest->flags_1fc_word & 0x80) != 0) {
                damage = (object->flags_1fc_word & 0x80) != 0 && Player_HasDoubleWeaponDamage(object) ? 2 : 1;
            }
            STUBBED_NAMED("LightSabre: apply ObjHitObj damage PC438230");
        } else {
            STUBBED_NAMED("LightSabre: source rumble PC54a850");
            STUBBED_NAMED("LightSabre: target rumble PC54a850");
        }
        return;
    }
    if ((object->sabre_flags & 4) != 0 &&
        ((object->flags_1fc_word & 0x80) != 0 || (object->portal_flags & 0x40) != 0 || object->field_f00 == 5)) {
        if ((object->flags_1fc_word & 0x80) == 0 && object->model_draw_result == 0) {
            if (object->blowup_target != NULL) {
                STUBBED_NAMED("LightSabre: GizmoBlowupBlowup PC567800");
                STUBBED_NAMED("LightSabre: AlertSurroundingCreatures PC4399e0");
            }
            return;
        }
        STUBBED_NAMED("LightSabre: GizmoBlowUp_Hit pointer-dependent hit/debris/rumble branch PC569c70");
    }
particle_contact:
    if ((object->sabre_flags & 6) != 0) {
        PART_PC *part = HitParts(object, points, 3, object->sabre_collision_radius, &minimum, &maximum, 0x08000008);
        if (part != NULL) {
            if ((part->flags & 8) == 0) {
                KillPart(part, 0);
            } else {
                STUBBED_NAMED("LightSabre: DeflectPart(part,object,5.0,-2.5,0,effect) PC42c500");
            }
            STUBBED_NAMED("LightSabre: AlertSurroundingCreatures PC4399e0");
        }
    }
}

static void AfterAnimationSabreCode(GAMEOBJECT *object, i32 effect) {
    PLAYERCHARACTERCONFIG_PC *data = object->character_data->game_character;
    i32 reset_streak = 0;
    u8 streak;
    u8 damage;
    u8 blade_flags[2];
    i32 blade;
    if ((data->weapon_action == 3 || object->id == (u16)id_GRIEVOUS || object->id == (u16)id_DOOKU) &&
        object->character_context == 0 && object->action_movement_state == 3) {
        object->sabre_flags |= 2;
    }
    if (object->id == (u16)id_GRIEVOUS && (object->character_context == 12 || object->character_context == 24) &&
        object->field_7aa == 26) {
        reset_streak = 1;
        object->sabre_flags |= 2;
    } else if (object->id == (u16)id_BODYGUARD &&
               (object->character_context == 12 || object->character_context == 24) &&
               (object->field_7aa == 26 || object->field_7aa == 27)) {
        object->sabre_flags |= 2;
    }
    streak = object->sabre_flags & 2;
    damage = object->sabre_flags & 4;
    blade_flags[0] = blade_flags[1] = object->sabre_flags;
    if (object->id == (u16)id_GRIEVOUS) {
        i32 first = -1;
        i32 second = -1;
        if (damage != 0) {
            if (object->character_context == 13) {
                first = 2;
                second = 0;
            } else if (object->character_context == 16) {
                first = 3;
                second = 2;
            } else if (object->character_context == 5) {
                switch (object->field_7aa) {
                    case 46:
                        second = 0;
                        break;
                    case 47:
                    case 48:
                        second = 3;
                        break;
                    case 49:
                    case 50:
                    case 51:
                    case 52:
                        first = 2;
                        second = 0;
                        break;
                    case 53:
                        first = 1;
                        second = 0;
                        break;
                    case 54:
                    case 55:
                        first = 3;
                        second = 2;
                        break;
                    case 56:
                    case 57:
                    case 58:
                    case 59:
                        second = 2;
                        break;
                }
            }
        }
        for (blade = 0; blade < 4; ++blade) {
            if (blade == 2 && reset_streak != 0) {
                streak = 0;
            }
            object->sabre_flags = streak | 1;
            if (damage != 0 && (blade == first || blade == second)) {
                object->sabre_flags |= damage;
            }
            if (object->character_context == 16) {
                if (blade < 2) {
                    object->sabre_flags &= ~2;
                } else {
                    object->sabre_flags |= 2;
                }
            }
            LightSabreStreakCode(object, blade, effect);
        }
    } else if (object->id == (u16)id_DARTHMAUL) {
        LightSabreStreakCode(object, 0, effect);
        object->sabre_flags = streak | 1;
        LightSabreStreakCode(object, 1, effect);
    } else if (object->id == (u16)id_BODYGUARD) {
        if (object->character_context == 5 && damage != 0) {
            if (object->combo_stage == 2) {
                blade_flags[0] &= ~4;
                blade_flags[1] |= 4;
            } else {
                blade_flags[0] |= 4;
                blade_flags[1] &= ~4;
            }
        } else if (object->character_context == 0 && object->action_movement_state == 4) {
            blade_flags[0] |= 2;
            blade_flags[1] |= 2;
        }
        for (blade = 0; blade < 2; ++blade) {
            object->sabre_flags = blade_flags[blade];
            LightSabreStreakCode(object, blade, effect);
        }
    } else {
        LightSabreStreakCode(object, 0, effect);
    }
}

void __cdecl GameObjectStuffAfterAnimation(void) {
    GAMEOBJECT *object = Obj;
    i32 i;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        i32 drawn;
        CHARACTER_MODEL_LAYERS_PC *model;
        PLAYERCHARACTERCONFIG_PC *data;
        if ((object->flags_1fc_word & 0x1001) != 0x1001 || object->dead != 0 || object->origin_joint_ready == 0)
            continue;
        drawn = object->model_draw_result != 0;
        model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
        data = object->character_data->game_character;
        if (drawn || (object->layer_mask & 4) != 0) {
            if (object->script_fire_target != NULL && (object->script_fire_target->flags_1fc_word & 1) == 0)
                object->script_fire_target = NULL;
            if (object->quick_shoot_bolt_id != -1) {
                if (object->id == (u16)id_BASKETCANNON) {
                    NUMTX matrix;
                    NUVEC velocity;
                    i32 joint = drawn ? data->first_shoot_joint : -1;
                    STUBBED_NAMED("AfterAnimation: GameAudio_PlaySfxById PC5478a0");
                    if (joint != -1 && model->points_of_interest[joint] != NULL)
                        matrix = object->locator_matrices[joint];
                    else {
                        matrix = object->transform;
                        matrix.m30 = object->collision_position.x;
                        matrix.m31 = object->collision_position.y;
                        matrix.m32 = object->collision_position.z;
                    }
                    velocity.x = 0.0f;
                    velocity.y = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.0);
                    velocity.z = (f32)(-((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.0 + 3.0));
                    NuVecMtxRotate(&velocity, &velocity, &matrix);
                    STUBBED_NAMED(
                        "AfterAnimation: basketball descriptor/AddPart requiring genuine callbacks PC4b1c58..4b1d0c");
                } else {
                    STUBBED_NAMED("AfterAnimation: Bolt_Shoot PC583af0");
                    if (object->quick_shoot_bolt_id == 29)
                        object->context_variant_flags |= 0x40;
                }
            }
            if ((object->flags_e20 & 0x04000000) != 0)
                STUBBED_NAMED("AfterAnimation: Torpedo_Shoot PC4d3690");
        }
        if (drawn && ((object->character_data->model_flags & 8) != 0 || object->id == (u16)id_BODYGUARD ||
                      object->id == (u16)id_IMPERIALGUARD)) {
            i32 effect = object->blade_index == -1 ? -1 : BladeTab[object->blade_index].hit_effect;
            AfterAnimationSabreCode(object, effect);
        }
        if (object->debris_joint != -1)
            STUBBED_NAMED("AfterAnimation: AddGameDebris effect138/139 at locator translation PC62c560");
        if (object->character_context == 38) {
            CHARACTER_ANIMATION_LOCATOR_PC *animation = model->animations[object->field_7aa];
            if (animation != NULL && animation->locator != -1 &&
                ((object->context_variant_flags & 0x40) == 0 || object->debris_joint != -1)) {
                NUVEC points[2];
                i32 joint = animation->locator;
                points[0].x = object->locator_matrices[joint].m30;
                points[0].y = object->locator_matrices[joint].m31;
                points[0].z = object->locator_matrices[joint].m32;
                points[1] = points[0];
                points[0].y = (f32)((f64)points[0].y + (f64)0.075f);
                points[1].y = (f32)((f64)points[1].y - (f64)0.075f);
                STUBBED_NAMED("AfterAnimation: emit locator streak PC5a3730");
            }
        }
        if (object->slam_effect != -1)
            STUBBED_NAMED("AfterAnimation: AddSlamDebris PC42f9d0");
        if (object->batarang_release != 0)
            STUBBED_NAMED("AfterAnimation: Batarang_Release PC404620");
        if ((object->flags_e20 & 0x00800000) != 0)
            STUBBED_NAMED("AfterAnimation: SuperCarry_Throw PC5980c0");
        if ((object->flags_e20 & 0x10) != 0) {
            NUMTX matrix;
            NUVEC velocity;
            i32 joint = drawn ? data->rocket_joint : -1;
            if (joint != -1 && model->points_of_interest[joint] != NULL)
                matrix = object->locator_matrices[joint];
            else {
                NuMtxSetRotationX(&matrix, 0x1555);
                NuMtxRotateY(&matrix, (u16)(object->field_0x276 + 0x8000));
                matrix.m30 = object->collision_position.x;
                matrix.m31 = object->collision_position.y;
                matrix.m32 = object->collision_position.z;
            }
            velocity.x = 0.0f;
            velocity.y = 0.0f;
            velocity.z = -1.0f;
            NuVecMtxRotate(&velocity, &velocity, &matrix);
            STUBBED_NAMED("AfterAnimation: rocket descriptor/BoltSys direction/TargetGameObject/AddPart requiring "
                          "genuine callbacks PC4b1f97..4b2126");
        }
        if ((object->flags_e24 & 0x4000) != 0)
            STUBBED_NAMED("AfterAnimation: ThermalDetonator_Throw PC4cfbb0");
        if ((object->flags_e20 & 0x02000000) != 0 && object->force_target != NULL)
            STUBBED_NAMED("AfterAnimation: GetZapOrigin pointer-dependent NuLgtLaser PC4a6050/6018c0");
        if (drawn && (object->character_data->model_flags & 8) != 0 && object->weapon_scale == 1.0f) {
            i32 blade;
            for (blade = 0; blade < 4; ++blade) {
                i32 joints[2];
                if (GetBladeJoints(object, blade, joints, NULL) != 0) {
                    STUBBED_NAMED("AfterAnimation: continuous sabre particles and real raycast-dependent rub action "
                                  "PC4b223b..4b261d");
                }
            }
        }
        if (drawn && object->id == (u16)id_LANDSPEEDER)
            STUBBED_NAMED("AfterAnimation: Landspeeder three real debris effects PC4b2636..4b283a");
        if (VehicleArea != 0 && object->field_280 <= 1)
            STUBBED_NAMED("AfterAnimation: Falcon debris-handle create/orient/free PC4b283a..4b290d");
        if (object->powerup_time > 0.0f && object->dead == 0) {
            NUVEC position;
            f32 radius = (f32)((f64)object->bounds_radius * (0.5 + (f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5));
            u16 angle = (u16)qrand();
            position.x = object->position.x + radius * NuTrigTable[angle >> 1];
            position.y =
                (f32)((f64)(object->position.y + object->character_bottom * object->origin_scale) + (f64)0.05f);
            position.z = object->position.z + radius * NuTrigTable[((u16)(angle + 0x4000) >> 1) & 0x7fff];
            STUBBED_NAMED("AfterAnimation: PowerUp_Particles PC46a8d0");
        }
    }
}

typedef struct kill_zipup_pc_s {
    u8 unknown_00[0x68];
    u16 runtime_flags;
} KILL_ZIPUP_PC;
STATIC_ASSERT(offsetof(KILL_ZIPUP_PC, runtime_flags) == 0x68);

i32 __cdecl KillGameObject(GAMEOBJECT *object, i32 requested_reason, i32 unused) {
    i32 reason = requested_reason == 5 ? 4 : requested_reason;
    i32 terminal_kill = 0;
    GAMEOBJECT *rider;
    AIGROUP_PC *group;
    GAMEOBJECT *candidate;
    i32 i;
    if ((object->flags_1fc_word & 0x80) != 0) {
        STUBBED_NAMED("KillGameObject: increment AreaGlobals first value PC951244 and LevDeaths PC87b2f0");
    }
    STUBBED_NAMED("KillGameObject: NuSound3ClearLoopHold upper position PC698a20");
    STUBBED_NAMED("KillGameObject: NuSound3ClearLoopHold collision position PC698a20");
    STUBBED_NAMED("KillGameObject: NuSound3ClearLoopHold lower position PC698a20");
    STUBBED_NAMED("KillGameObject: Grapple_RemoveDynamic PC54e7d0");
    STUBBED_NAMED("KillGameObject: Shards_HandleLostObj PC4c0c80");
    if (reason != 4) {
        STUBBED_NAMED(
            "KillGameObject: PC identity-selected death debris/explosion/audio/rumble/snake actions PC436582..436996");
    } else {
        STUBBED_NAMED("KillGameObject: snake body destroy action PC597350");
    }
    if (object->character_context == 43) {
        object->character_context = -1;
        if ((object->flags_1fc_word & 0x80) != 0 &&
            (WORLD->current_level == HOTHBATTLEA_LDATA || WORLD->current_level == GUNSHIPA_LDATA))
            STUBBED_NAMED("KillGameObject: half-second GameCam_Blend PC54a3e0");
    }
    if (reason == 4) {
        terminal_kill = 1;
        reason = 2;
    } else if (WORLD->current_level == SPEEDERCHASEA_LDATA && object->ai_packet.creature_set == 2) {
        STUBBED_NAMED("KillGameObject: SpeedersKilled message and lost hitpoint increment PC436a30..436a55");
    }
    if ((object->character_data->model_flags & 0x2000) != 0) {
        object->velocity.x = (f32)((f64)object->velocity.x * 0.5);
        object->velocity.y = (f32)((f64)object->velocity.y * 0.5);
        object->velocity.z = (f32)((f64)object->velocity.z * 0.5);
    } else {
        object->velocity.x = 0.0f;
        object->velocity.z = 0.0f;
    }
    object->current_hp = 0;
    object->dead = (u8)(reason == 3 ? 2 : reason);
    if (object->character_context == 7 || object->field_e33 == 1)
        object->flags_e20 |= 0x10000;
    else if (object->character_context == 6 || object->field_e33 == 2)
        object->flags_e20 &= ~0x10000;
    if (object->helmet != 0)
        STUBBED_NAMED("KillGameObject: LoseHelmet PC433aa0");
    rider = object->rider;
    STUBBED_NAMED("KillGameObject: Player_ClearContext PC55e4a0");
    if (terminal_kill != 0 && rider != NULL)
        KillGameObject(rider, 4, 0);
    STUBBED_NAMED("KillGameObject: ReleaseLever PC48ffc0");
    STUBBED_NAMED("KillGameObject: Player_ResetContexts embedded packet6c4 PC55e4e0");
    if ((object->flags_1fc_word & 0x80) != 0) {
        if (object->torpedo != NULL) {
            while (object->torpedo->count != 0) {
                STUBBED_NAMED("KillGameObject: emit torpedo debris at actual position PC684c90");
                --object->torpedo->count;
            }
        }
        if (object->dead == 0 || (object->character_data->model_flags & 0x2000) == 0 ||
            (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0))
            STUBBED_NAMED("KillGameObject: one-second GameCam_Blend PC54a3e0");
        if ((object->flags_1f8 & 0x40000) == 0) {
            if (object->takeover_owner != NULL &&
                (object->takeover_owner->rider == NULL || object->takeover_owner->rider == object)) {
                STUBBED_NAMED(
                    "KillGameObject: real GetTakeOverPos output-dependent position/start/AI position writes PC4871a0");
            } else {
                STUBBED_NAMED("KillGameObject: SetToLastSafePos PC55cd80");
            }
        }
        GameObjectOrigin(object);
        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
    }
    object->death_timer = 0.0f;
    if (object->dead != 2)
        object->dead = 1;
    if (object->dead == 1)
        object->start_position = object->position;
    object->death_delay = object->dead == 2 ? 0.0f : 0.5f;
    if (object->death_delay <= 0.0f)
        object->movement_lean_angle = 0;
    if (terminal_kill == 0)
        STUBBED_NAMED("KillGameObject: real BeenKilled script predicate/process using same packet2c8 PC52c790/52cc90");
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
                        if (candidate != object && (candidate->flags_1fc_word & 0x1001) == 0x1001 &&
                            candidate->dead == 0) {
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
                    } else if (candidate != object && (candidate->flags_1fc_word & 0x1001) == 0x1001 &&
                               candidate->dead == 0) {
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
    object->field_eb0 = object->field_eb4 = 0;
    object->opponent = NULL;
    object->ai_packet.opponent = NULL;
    object->field_ec0 = 0;
    object->field_ec4 = 0.0f;
    if (object->reciprocal_target != NULL) {
        object->reciprocal_target->reciprocal_target = NULL;
        object->reciprocal_target = NULL;
    }
    if (object->character_context == 71 && object->grapple != NULL) {
        ((KILL_ZIPUP_PC *)object->grapple)->runtime_flags &= ~0x100;
        object->grapple = NULL;
    }
    STUBBED_NAMED("KillGameObject: Batarangs_CheckLostData PC404e30");
    STUBBED_NAMED("KillGameObject: SpecialMove_ReleaseVictim PC58b0f0");
    STUBBED_NAMED("KillGameObject: ReleaseEat PC49c4e0");
    STUBBED_NAMED("KillGameObject: ReleaseBuildIt PC58ff10");
    STUBBED_NAMED("KillGameObject: ReleasePush PC59cb00");
    candidate = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++candidate) {
        if ((candidate->flags_1fc_word & 0x1001) != 0x1001 || candidate == object)
            continue;
        if (candidate->character_context == 27 && candidate->force_target == object)
            STUBBED_NAMED("KillGameObject: ReleaseForce target PC4a25b0");
        if (candidate->force_target == object)
            candidate->force_target = NULL;
        if (candidate->airborne_collision_target == object)
            candidate->airborne_collision_target = NULL;
    }
    STUBBED_NAMED("KillGameObject: ReleaseForce source PC4a25b0");
    if (object->field_280 == 0xff) {
        for (i = 0; i < 32; ++i)
            if (Bolt[i].active != 0 && Bolt[i].owner == object)
                Bolt[i].owner = NULL;
        for (i = 0; i < MAXPARTS; ++i)
            if ((Part[i].active & 1) != 0 && Part[i].owner == object)
                Part[i].owner = NULL;
        STUBBED_NAMED("KillGameObject: explosion owner cleanup (eight records38) PC43700c..43711e");
    }
    if (DOGFIGHTA_LDATA != NULL && WorldInfo_CurrentlyActive()->current_level == DOGFIGHTA_LDATA && BonusWinner == -1 &&
        (object->flags_1f8 & 0x40000) == 0)
        STUBBED_NAMED("KillGameObject: ResetLevel EP3_DOGFIGHT_DIE PC57a620");
    if (terminal_kill != 0 && object->field_280 == 0xff) {
        i32 remove = requested_reason == 5;
        AICREATURE_PC *creature = NULL;
        i32 persistent = (object->portal_flags & 0x100000) != 0;
        i32 respawn = 0;
        if (!remove) {
            u32 limit = object->portal_flags;
            i32 respawn_flag = 0;
            if ((object->flags_1f8 & 0x4400) == 0x4400) {
                creature = &WORLD->ai_sys->creatures[object->ai_packet.creature_index];
                respawn_flag = (object->portal_flags & 0x200000) != 0;
                if (!persistent) {
                    limit = object->ai_packet.respawn_count + 1;
                    if (creature->max_respawn_count != -1 && object->ai_packet.respawn_locator == NULL)
                        limit =
                            creature->min_respawn_count +
                            ((creature->max_respawn_count - creature->min_respawn_count) * ((i32)Game.difficulty - 1)) /
                                9 +
                            1;
                }
            }
            if (!persistent && object->ai_packet.respawn_count >= limit)
                remove = 1;
            else
                respawn = creature != NULL && (!persistent || respawn_flag);
        }
        if (remove) {
            object->flags_1fc_word &= ~0x1000;
            object->ai_packet.reset_mode = 4;
        } else if (respawn && creature != NULL) {
            object->flags_1fc_word &= ~0x1000;
            object->ai_packet.reset_mode = 1;
            object->ai_packet.spawn_delay = (f32)(1.0 - ((f64)Game.difficulty - 1.0) / 9.0);
            if (creature->min_respawn_time > 0.0f && creature->max_respawn_time > 0.0f)
                object->ai_packet.spawn_delay = creature->max_respawn_time * object->ai_packet.spawn_delay +
                                                creature->min_respawn_time * (1.0f - object->ai_packet.spawn_delay);
            else
                object->ai_packet.spawn_delay =
                    (f32)((f64)object->ai_packet.spawn_delay * ((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.0));
        } else
            object->field_1010 = 1.0f;
    }
    if (object->torpedo != NULL && object->field_280 == 0xff) {
        STUBBED_NAMED("KillGameObject: FreeTorpedoPacket PC4d22f0");
        object->torpedo = NULL;
    }
    if (VehicleArea != 0 && object->field_280 < 2)
        STUBBED_NAMED("KillGameObject: DebFreeInstantly Falcon key PC684400");
    return 1;
}
