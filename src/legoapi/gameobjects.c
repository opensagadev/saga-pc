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
                    {
                        ADDPART_PC part = Default_ADDPART;
                        part.matrix = &matrix;
                        part.velocity = &velocity;
                        part.part_id = 90;
                        part.gravity = -10.0f;
                        part.flags = 0x10;
                        part.field_14 = 0.05f;
                        part.field_18 = 0.05f;
                        part.special = &WORLD->level_objects[90].special;
                        part.update_fn = PartUpdate_Basketball;
                        part.impact_fn = PartImpact_Basketball;
                        part.stop_fn = PartStop_Flickerer;
                        part.draw_fn = PartDraw_Flickerer;
                        part.time_step = FRAMETIME;
                        AddPart(&part);
                    }
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
            {
                ADDPART_PC part = Default_ADDPART;
                i32 player = (object->flags_1fc_word & 0x80) != 0;
                NUVEC direction;
                f32 range;
                PART_PC *created;
                part.matrix = &matrix;
                part.velocity = &velocity;
                part.field_14 = 0.1f;
                part.field_18 = 0.1f;
                part.special = &WORLD->level_objects[232].special;
                part.part_id = 232;
                part.flags = player ? 0xc11b : 0xc31b;
                part.owner = object;
                part.hostility_group = object->hostility_group;
                part.kill_fn = BobaRocket_Kill;
                part.move_fn = BobaRocket_Move;
                part.deflect_fn = BobaRocket_Deflect;
                part.collision_fn = PartCollide_3D;
                part.time_step = FRAMETIME;
                part.gravity = 0.0f;
                part.field_a4 = player ? (f32)(4.0 + 2.0 * ((f64)qrand() * (f64)(1.0f / 65535.0f))) : 10.0f;
                BoltSys->shoot_direction(object, &direction);
                range = part.field_a4 * rocket_speed;
                part.recipient = TargetGameObject(object, &object->collision_position, &direction, range, range * range,
                                                  0, 1, 0, -1);
                created = AddPart(&part);
                if (created != NULL)
                    created->force_flags = (u16)ObjHitObj_Flags(object);
            }
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

i32 __cdecl NoLayerKill(GAMEOBJECT *object) {
    return (object->character_data->game_character->flags_094 & 0x800000) != 0 && object->field_283 == 6;
}

void __cdecl GetTakeOverPos(GAMEOBJECT *object, NUVEC *position) {
    i32 locator = object->character_data->game_character->ride_locator;
    CHARACTER_MODEL_LAYERS_PC *model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
    if (locator != -1 && model->points_of_interest[locator] != NULL) {
        NUMTX *matrix = &object->locator_matrices[locator];
        *position = matrix->translation;
    } else {
        position->x = object->position.x;
        position->y = object->upper_position.y;
        position->z = object->position.z;
    }
}

/* A copy of the actual 33-DWORD script cohort, without extending the packet. */
typedef struct TAGSCRIPTSTATE_PC {
    AISCRIPTPROCESS_PC processor;
    void *script_link;
} TAGSCRIPTSTATE_PC;
STATIC_ASSERT(sizeof(AISCRIPTPROCESS_PC) == 0xc8);
STATIC_ASSERT(offsetof(TAGSCRIPTSTATE_PC, script_link) == 0xc8);
STATIC_ASSERT(sizeof(TAGSCRIPTSTATE_PC) == 0xcc);

static void TakeOver_SetAction(GAMEOBJECT *rider, GAMEOBJECT *vehicle) {
    i32 action;
    CHARACTER_MODEL_LAYERS_PC *model = (CHARACTER_MODEL_LAYERS_PC *)rider->character_model;
    if (vehicle->id == GameObjectRotationBuggyId) {
        action = 0x70;
    } else if (vehicle->id == id_GYROCOPTER) {
        action = 0x83;
    } else if (StartupGameFlag1 == 0) {
        rider->field_7aa = 0x6c;
        return;
    } else if (vehicle->character_data->game_character->weapon_action == 12) {
        action = CurrentAnim(&vehicle->anim_packet) == 3 || CurrentAnim(&vehicle->anim_packet) == 0x17 ? 0xc4 : 0xc3;
    } else if (vehicle->id == id_BANTHA || vehicle->id == id_BOMARRMONK) {
        action = 0x70;
    } else if (vehicle->id == id_DEWBACK) {
        action = 0x83;
    } else if (vehicle->id == id_LANDSPEEDER || vehicle->id == id_FLASHSPEEDER) {
        action = 0x84;
    } else if (vehicle->id == id_TAUNTAUN) {
        action = 0x86;
    } else if (vehicle->id == id_SPEEDERBIKE) {
        action = 0xc0;
    } else if (vehicle->id == id_HEAVYREPEATINGCANNON || vehicle->id == id_BIGGUN) {
        action = 0xc1;
    } else if (vehicle->id == id_TROOPERCANNON) {
        action = model->action_animations[0xc2] != NULL ? 0xc2 : 0xc1;
    } else if (vehicle->id == id_STAP2) {
        action = 0xc1;
    } else if (vehicle->id == id_CLONEWALKER) {
        action = 0xc0;
    } else {
        rider->field_7aa = 0x6c;
        return;
    }
    rider->field_7aa = model->action_animations[action] != NULL ? (i16)action : 0x6c;
}

/* Original-TU private globals are handed off to the single data owner. */

static GAMEOBJECT *Tag_FindGameObject_TRANSFER(GAMEOBJECT *object) {
    f32 nearest_distance = object->character_context == 0x17 ? 1.44f : 0.48999998f;
    i32 count = Tag_Mode == 3 ? HIGHGAMEOBJECT : 8;
    GAMEOBJECT *nearest = NULL;
    i32 index;
    for (index = 0; index < count; ++index) {
        GAMEOBJECT *candidate = Tag_Mode == 3 ? &Obj[index] : Player[index];
        NUVEC direction;
        f32 distance;
        if (candidate == NULL || (candidate->flags_1fc_word & 0x1001) != 0x1001 || candidate == object ||
            candidate->dead != 0 || (candidate->tag_flags & 2) != 0) {
            continue;
        }
        if (candidate->character_context == 0x17 || candidate->character_context == 0x3d ||
            (CInfo[candidate->character_context].flags & 0x8000) != 0 ||
            ((candidate->flags_ef4 & 2) != 0 && object->id != id_LUKESKYWALKERDAGOBAH)) {
            continue;
        }
        distance = NuVecDistSqr(&object->position, &candidate->position, &direction);
        if (distance < nearest_distance) {
            NuVecRotateY(&direction, &direction, -(i32)object->field_0x276);
            if (direction.z < 0.0f) {
                nearest_distance = distance;
                nearest = candidate;
            }
        }
    }
    return nearest;
}

static i32 Tag_FindGameObject_CYCLICAL(GAMEOBJECT *object) {
    i32 remaining = HIGHGAMEOBJECT - 1;
    i32 index = object->hostility_group;
    while (remaining > 0) {
        GAMEOBJECT *candidate;
        if (++index == HIGHGAMEOBJECT) {
            index = 0;
        }
        --remaining;
        if (remaining == 0) {
            break;
        }
        candidate = &Obj[index];
        if (candidate != object && (candidate->flags_1fc_word & 0x1001) == 0x1001 && candidate->dead == 0 &&
            candidate->field_280 != 0xff && (candidate->tag_flags & 2) == 0 && (candidate->flags_ef4 & 2) == 0 &&
            (CInfo[candidate->character_context].flags & 0x800) == 0) {
            return (i8)candidate->field_280;
        }
    }
    return -1;
}

i32 __cdecl TagCode(GAMEOBJECT *source, GAMEOBJECT *destination, i32 takeover, i32 blend_camera, i32 unused) {
    GAMEPAD_PC *source_pad;
    GAMEPAD_PC *destination_pad;
    f32 source_da8;
    i8 source_hp, destination_hp;
    u32 source_player, destination_player;
    TAGSCRIPTSTATE_PC source_script, destination_script;
    u8 source_set = 0, destination_set = 0;
    u32 source_ai = 0, destination_ai = 0;
    void *pointer;
    u8 player_index;
    f32 timer;
    u32 source_flags, mask;
    void(__cdecl * callback)(GAMEOBJECT *);
    (void)unused;
    if (CUTSKIPLOCK != 0) {
        return 0;
    }
    source_pad = source->pad;
    destination_pad = destination->pad;
    source_da8 = source->field_da8;
    source_hp = source->current_hp;
    destination_hp = destination->current_hp;
    source_player = source->flags_1fc_word & 0x80;
    destination_player = destination->flags_1fc_word & 0x80;
    if (takeover != 0) {
        source_script.processor = source->ai_packet.script_processor;
        source_script.script_link = source->ai_packet.takeover_script_link;
        destination_script.processor = destination->ai_packet.script_processor;
        destination_script.script_link = destination->ai_packet.takeover_script_link;
        source_set = source->ai_packet.creature_set;
        destination_set = destination->ai_packet.creature_set;
        source_ai = source->flags_1fc_word & 0x100;
        destination_ai = destination->flags_1fc_word & 0x100;
    }
    STUBBED_NAMED("TagCode: ResetForceGlow source player packet PC4a4ca0");
    STUBBED_NAMED("TagCode: ResetForceGlow destination player packet PC4a4ca0");
    destination->pad = source_pad;
    source->pad = destination_pad;
    source->flags_1fc_word = (source->flags_1fc_word & ~0x80u) | destination_player;
    destination->flags_1fc_word = (destination->flags_1fc_word & ~0x80u) | source_player;
    source->field_da8 = destination->field_da8;
    destination->field_da8 = source_da8;
    pointer = source->coin_packet;
    source->coin_packet = destination->coin_packet;
    destination->coin_packet = pointer;
    player_index = source->field_280;
    source->field_280 = destination->field_280;
    destination->field_280 = player_index;
    timer = source->powerup_time;
    source->powerup_time = destination->powerup_time;
    destination->powerup_time = timer;
    pointer = source->gizforce_los_info;
    source->gizforce_los_info = destination->gizforce_los_info;
    destination->gizforce_los_info = pointer;
    STUBBED_NAMED("TagCode: real two GizForce_ResetLOS calls PC466820, no Android task operations");
    mask = takeover != 0 ? 0x146fd : 0x46f8;
    source_flags = source->flags_1f8;
    source->flags_1f8 = (source_flags & ~mask) | (destination->flags_1f8 & mask);
    destination->flags_1f8 = (destination->flags_1f8 & ~mask) | (source_flags & mask);
    callback = source->tag_object_callback;
    source->tag_object_callback = destination->tag_object_callback;
    destination->tag_object_callback = callback;
    timer = source->spawn_protection_timer;
    source->spawn_protection_timer = destination->spawn_protection_timer;
    destination->spawn_protection_timer = timer;
    STUBBED_NAMED("TagCode: AICreatureResumeScript source PC415000");
    if (takeover != 0) {
        if (source->field_280 != 0xff) {
            Player[(i8)source->field_280] = source;
        }
        if (destination->field_280 != 0xff) {
            Player[(i8)destination->field_280] = destination;
        }
    } else {
        source->hud_icon_timer = 0.0f;
        source->input_toggle_hold_time = TOGGLEHOLDTIME;
        source->flags_ef0 = (source->flags_ef0 & ~0x180000u) | 0x80;
        source->tag_reset_timer = 0.0f;
        source->pause_context_state = 0.0f;
        destination->hud_icon_timer = 2.0f;
        destination->input_toggle_hold_time = TOGGLEHOLDTIME;
        destination->flags_ef0 |= 0x80;
        STUBBED_NAMED("TagCode: NewBuzzFrames destination pad 2,0 PC54a8d0");
        if (source->field_280 != 0xff) {
            Player[(i8)source->field_280] = source;
            if (source->rider != NULL) {
                source->field_107e = source->character_data->game_character->hitpoints;
            } else if (WORLD->current_level != VADERC_LDATA) {
                source->field_107e = DEFAULT_PLAYERHITPOINTS;
            }
        } else {
            source->field_107e = source->character_data->game_character->hitpoints;
        }
        if (destination->field_280 != 0xff) {
            Player[(i8)destination->field_280] = destination;
            if (destination->rider != NULL) {
                destination->field_107e = destination->character_data->game_character->hitpoints;
            } else if (WORLD->current_level != VADERC_LDATA) {
                destination->field_107e = DEFAULT_PLAYERHITPOINTS;
            }
            STUBBED_NAMED("TagCode: GameCam_Blend GameCam,0.5f,0,1 PC54a3e0");
        } else {
            destination->field_107e = destination->character_data->game_character->hitpoints;
        }
        destination->current_hp = source_hp;
        if ((i32)source_hp > (i32)destination->field_107e) {
            destination->current_hp = (i8)destination->field_107e;
        }
        if (destination->current_hp == 0 && destination->field_107e != 0) {
            destination->current_hp = 1;
        }
        if (WORLD->current_level == VADERC_LDATA || (source->flags_1fc_word & 0x80) != 0) {
            source->current_hp = destination_hp;
        } else if ((source->character_data->model_flags & 0x20) != 0) {
            source->current_hp = (i8)source->saved_hitpoints;
        } else {
            source->current_hp = (i8)source->field_107e;
        }
    }
    destination->flags_ef0 &= ~0x180000u;
    destination->tag_reset_timer = 0.0f;
    destination->pause_context_state = 0.0f;
    destination->tag_cooldown = 1.0f;
    if (source == player_tag_to && destination != player_tag_from) {
        player_tag_to = NULL;
        player_tag_timer = 0.0f;
        player_tag_from = NULL;
        do_player_tag = 0;
    }
    if (takeover != 0) {
        if (AIScriptSetBaseScriptStateByName(&destination->ai_packet.script_processor,
                                             (char *)TakeOverState_JustBeenTakenOver) != 0) {
            AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)destination, &destination->ai_packet,
                            &destination->ai_packet.script_processor, FRAMETIME);
        }
        source->ai_packet.script_processor = destination_script.processor;
        source->ai_packet.takeover_script_link = destination_script.script_link;
        destination->ai_packet.script_processor = source_script.processor;
        destination->ai_packet.takeover_script_link = source_script.script_link;
        source->ai_packet.creature_set = source_set;
        destination->ai_packet.creature_set = destination_set;
        STUBBED_NAMED("TagCode: AICreatureResumeScript source PC415000");
        source->flags_1fc_word = (source->flags_1fc_word & ~0x100u) | destination_ai;
        destination->flags_1fc_word = (destination->flags_1fc_word & ~0x100u) | source_ai;
        STUBBED_NAMED("TagCode: ReleaseForce source,0 PC4a25b0");
        STUBBED_NAMED("TagCode: ReleaseBuildIt source,0 PC58ff10");
        STUBBED_NAMED("TagCode: ReleasePush source PC59cb00");
        STUBBED_NAMED("TagCode: Player_ResetContexts source player packet PC55e4e0");
        if ((source->portal_flags & 0x10) == 0) {
            STUBBED_NAMED("TagCode: SetWeaponIn source PC58d8e0");
        }
        source->rider = destination;
        destination->rider = source;
        source->takeover_target = NULL;
        destination->takeover_target = NULL;
        source->takeover_owner = destination;
        source->character_context = 0x3b;
        TakeOver_SetAction(source, source->rider);
        source->takeover_timer = 1.0f;
        source->external_force = source->position;
        if (blend_camera != 0) {
            STUBBED_NAMED("TagCode: GameCam_Blend GameCam,0.5f,0,1 PC54a3e0");
        }
        if (destination->character_data->game_character->field_028 > 0.0f) {
            destination->velocity.y -= (f64)source->character_data->game_character->jump_speed * 0.5;
        }
    }
    return 1;
}

/* Appended after tag-draft.c and get-pos.c in the original-TU scratch group. */
static void SetRiderPos(GAMEOBJECT *object, NUVEC *position) {
    if (object->rider != NULL) {
        if (position != NULL) {
            object->position = *position;
        } else {
            GetTakeOverPos(object->rider, &object->position);
        }
        object->pitch_angle = 0;
        object->roll_angle = 0;
        object->field_0x276 = object->rider->facing_angle;
        object->facing_angle = object->rider->facing_angle;
        object->movement_facing_angle = object->rider->facing_angle;
    }
    GameObjectOrigin(object);
}

i32 __cdecl TakeOverYoda(GAMEOBJECT *rider, GAMEOBJECT *vehicle, i32 blend_camera, i32 unused) {
    (void)unused;
    STUBBED_NAMED("TakeOverYoda: ReleaseForce PC4a25b0");
    STUBBED_NAMED("TakeOverYoda: ReleaseBuildIt PC58ff10");
    STUBBED_NAMED("TakeOverYoda: ReleasePush PC59cb00");
    STUBBED_NAMED("TakeOverYoda: Player_ResetContexts PC55e4e0");
    if ((rider->portal_flags & 0x10) == 0) {
        STUBBED_NAMED("TakeOverYoda: SetWeaponIn PC58d8e0");
    }
    rider->rider = vehicle;
    vehicle->rider = rider;
    rider->takeover_target = NULL;
    vehicle->takeover_target = NULL;
    rider->takeover_owner = vehicle;
    rider->character_context = 0x3b;
    TakeOver_SetAction(rider, rider->rider);
    rider->takeover_timer = 1.0f;
    rider->external_force = rider->position;
    if (blend_camera != 0) {
        STUBBED_NAMED("TakeOverYoda: GameCam_Blend GameCam,0.5f,0,1 PC54a3e0");
    }
    STUBBED_NAMED("TakeOverYoda: NewBuzz vehicle pad,0.1f,0 PC54a8a0");
    STUBBED_NAMED("TakeOverYoda: GameCam_Judder GameCam,0.1f,0,NULL PC549de0");
    STUBBED_NAMED("TakeOverYoda: Hint_SetComplete 0x28c PC576960");
    return 1;
}

void __cdecl TakeOver2GetIn(GAMEOBJECT *rider, GAMEOBJECT *vehicle) {
    i32 i;
    CHARACTER_MODEL_LAYERS_PC *model;
    for (i = 0; i < HIGHGAMEOBJECT; ++i) {
        GAMEOBJECT *object = &Obj[i];
        if ((object->flags_1fc_word & 0x1001) == 0x1001 && object->character_context == 0x3c &&
            object->force_target == vehicle) {
            return;
        }
    }
    if (rider == NULL || (rider->flags_1fc_word & 1) == 0 || vehicle == NULL || (vehicle->flags_1fc_word & 1) == 0 ||
        rider->rider != NULL || vehicle->rider != NULL) {
        return;
    }
    rider->character_context = 0x3c;
    model = (CHARACTER_MODEL_LAYERS_PC *)rider->character_model;
    if (model->action_animations[6] != NULL) {
        ResetAnimPacket(&rider->anim_packet, -1);
        rider->field_7aa = 6;
        rider->context_duration = AnimDuration((i16)rider->id, 6, 0.0f, 0.0f, 1);
    } else {
        rider->field_7aa = 5;
        rider->context_duration = 0.5f;
    }
    if (vehicle->id == id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
        NUVEC position;
        GetTakeOverPos(vehicle, &position);
        rider->context_duration *= 2.0f * SpeederChaseATATInOutMul(&rider->position, &position);
    }
    rider->context_timer = 0.0f;
    rider->external_force = rider->position;
    rider->takeover_start_angle = rider->field_0x276;
    rider->force_target = vehicle;
    rider->takeover_target = NULL;
    vehicle->takeover_target = NULL;
    STUBBED_NAMED("TakeOver2GetIn: PlayJumpSfx rider,0 PC547f30");
}

void __cdecl TakeOverGameObject2(GAMEOBJECT *rider, GAMEOBJECT *vehicle, i32 blend_camera) {
    i32 result;
    if (rider == NULL || (rider->flags_1fc_word & 1) == 0 || vehicle == NULL || (vehicle->flags_1fc_word & 1) == 0 ||
        rider->rider != NULL || vehicle->rider != NULL) {
        return;
    }
    if ((rider->flags_ef4 & 2) != 0) {
        result = TakeOverYoda(rider, vehicle, blend_camera, 1);
    } else {
        if (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0 &&
            (rider->flags_1fc_word & 0x80) != 0) {
            blend_camera = 1;
        }
        STUBBED_NAMED("TakeOverGameObject2: RegisterTakeOverObject vehicle PC59f990");
        result = TagCode(rider, vehicle, 1, blend_camera, 1);
    }
    if (result == 2) {
        rider->tag_flags |= 0xc;
        rider->tag_cooldown = 1.0f;
        rider->pending_tag_target = vehicle;
    }
}

void __cdecl TakeOverGameObject(GAMEOBJECT *rider, GAMEOBJECT *vehicle, i32 blend_camera, i32 immediate) {
    if (immediate != 0) {
        TakeOverGameObject2(rider, vehicle, blend_camera);
    } else {
        TakeOver2GetIn(rider, vehicle);
    }
}

void __cdecl ReleaseTakeOver(GAMEOBJECT *object, i32 unused) {
    GAMEOBJECT *rider = object->rider;
    TAGSCRIPTSTATE_PC rider_script, object_script;
    u8 rider_set, object_set;
    u32 rider_flag, object_flag;
    PLAYERCHARACTERCONFIG_PC *character;
    (void)unused;
    if (rider == NULL) {
        return;
    }
    if (object->character_context == 0x3b) {
        GAMEOBJECT *vehicle = rider;
        rider = object;
        object = vehicle;
    }
    rider_script.processor = rider->ai_packet.script_processor;
    rider_script.script_link = rider->ai_packet.takeover_script_link;
    object_script.processor = object->ai_packet.script_processor;
    object_script.script_link = object->ai_packet.takeover_script_link;
    rider_set = rider->ai_packet.creature_set;
    object_set = object->ai_packet.creature_set;
    rider_flag = rider->flags_1fc_word & 0x100;
    object_flag = object->flags_1fc_word & 0x100;
    if ((rider->flags_ef4 & 2) == 0 && TagCode(rider, object, 1, 0, 0) == 0) {
        return;
    }
    STUBBED_NAMED("ReleaseTakeOver: reviewed 1-byte empty cross-character callback PC486f20, names rider/object "
                  "character_data->name");
    rider->ai_packet.script_processor = object_script.processor;
    rider->ai_packet.takeover_script_link = object_script.script_link;
    object->ai_packet.script_processor = rider_script.processor;
    object->ai_packet.takeover_script_link = rider_script.script_link;
    rider->ai_packet.creature_set = rider_set;
    object->ai_packet.creature_set = object_set;
    STUBBED_NAMED("ReleaseTakeOver: AICreatureResumeScript object PC415000");
    rider->flags_1fc_word = (rider->flags_1fc_word & ~0x100u) | object_flag;
    object->flags_1fc_word = (object->flags_1fc_word & ~0x100u) | rider_flag;
    if (rider->rider != NULL) {
        GetTakeOverPos(rider->rider, &rider->position);
        rider->pitch_angle = 0;
        rider->roll_angle = 0;
        rider->field_0x276 = rider->rider->facing_angle;
        rider->facing_angle = rider->rider->facing_angle;
        rider->movement_facing_angle = rider->rider->facing_angle;
    }
    GameObjectOrigin(rider);
    rider->rider = NULL;
    rider->character_context = -1;
    if (object->id == id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
        f32 multiplier = SpeederChaseATATInOutMul(&rider->position, &SpeederChaseATATExitLandPos);
        f32 duration = SpeederChaseATATExitHeight * multiplier;
        f32 speed = SpeederChaseATATExitSpeed * multiplier;
        StartBigJump(rider, &SpeederChaseATATExitLandPos, 0, speed, duration, 0);
        STUBBED_NAMED("ReleaseTakeOver: GameCam_Blend GameCam,2.0f,0,1 PC54a3e0");
    } else {
        Buck_StartRiderJump(rider, object);
    }
    character = object->character_data->game_character;
    if (character->field_028 > 0.0f) {
        object->velocity.y -= (f64)rider->velocity.y * 0.5;
    } else if ((character->flags_094 & 0x20000) != 0) {
        Buck_Start(object, rider);
    }
    object->rider = NULL;
    rider->flags_e20 &= 0x7fffffff;
    object->flags_e20 &= 0x7fffffff;
    rider->ai_packet.path_info.flags_word &= ~1u;
    object->ai_update_position = object->position;
    if (rider->field_280 != 0xff || object->field_280 != 0xff) {
        STUBBED_NAMED("ReleaseTakeOver: GameCam_Blend GameCam,0.3f,0,1 PC54a3e0");
    }
    if (AIScriptSetBaseScriptStateByName(&rider->ai_packet.script_processor, (char *)TakeOverState_ReleasedTakeOver) !=
        0) {
        AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)rider, &rider->ai_packet, &rider->ai_packet.script_processor,
                        FRAMETIME);
    }
    if (AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, (char *)TakeOverState_ReleasedTakeOver) !=
        0) {
        AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, &object->ai_packet.script_processor,
                        FRAMETIME);
    }
}

void __cdecl ReleaseAllTakeOvers(void) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        if (Player[index] != NULL) {
            ReleaseTakeOver(Player[index], 0);
        }
    }
}

void __cdecl Tag_Check(GAMEOBJECT *object) {
    GAMEOBJECT *target = NULL;
    i32 result;
    if (Arcade != 0 || (object->tag_flags & 1) != 0 || object->character_context == 0x51) {
        if ((object->tag_flags & 1) != 0 || object->character_context == 0x51) {
            object->hud_icon_timer = 0.0f;
        }
        object->flags_ef0 &= ~0x180000u;
        object->tag_reset_timer = 0.0f;
        object->pause_context_state = 0.0f;
        if (Arcade == 0) {
            return;
        }
    }
    if (object->field_280 == 0xff) {
        return;
    }
    if ((object->tag_flags & 4) != 0) {
        object->tag_cooldown -= FRAMETIME;
        if (object->tag_cooldown > 0.0f && object->pending_tag_target != NULL) {
            target = object->pending_tag_target;
            if ((target->flags_1fc_word & 0x1001) != 0x1001) {
                object->tag_flags &= ~4u;
                object->pending_tag_target = NULL;
                goto draw_icon;
            }
            result = (object->flags_ef4 & 2) != 0 ? TakeOverYoda(object, target, 0, 1)
                                                  : TagCode(object, target, (object->tag_flags >> 3) & 1, 0, 1);
            if (result != 0) {
                goto tagged_result;
            }
        }
        object->tag_flags &= ~4u;
        goto draw_icon;
    }
    object->tag_player_index = -1;
    if ((object->flags_1fc_word & 0x80) == 0 || object->dead != 0 ||
        (CInfo[object->character_context].flags & 0x800) != 0 || (object->flags_1fc_word & 0x100) != 0 ||
        !(object->tag_cooldown <= 0.0f)) {
        goto draw_icon;
    }
    if (VehicleArea != 0) {
        if ((object->pad->buttons_pressed & GAMEPAD_TAG) != 0) {
            object->hud_icon_timer = 2.0f;
        }
        goto draw_icon;
    }
    if ((object->pad->buttons_pressed & GAMEPAD_TAG) == 0) {
        goto draw_icon;
    }
    if (CUTSKIPLOCK != 0) {
        object->hud_icon_timer = 2.0f;
        object->input_toggle_hold_time = TOGGLEHOLDTIME;
        goto no_target;
    }
    if (Arcade != 0) {
        goto no_target;
    }
    if (Tag_Mode == 1 || Tag_Mode == 3) {
        if (object->character_context == -1 || object->character_context == 0x17 ||
            (CInfo[object->character_context].flags & 4) != 0 ||
            (CInfo[object->character_context].parameter & 0x10) != 0) {
            target = Tag_FindGameObject_TRANSFER(object);
        }
        if (Tag_Mode == 1) {
            if (target != NULL && target->field_280 != 0xff) {
                object->tag_player_index = (i8)target->field_280;
            }
            goto select_player;
        }
        if (target == NULL) {
            goto no_target;
        }
        if (InCollectList_Index((i16)target->id, NULL, 0) == -1) {
            STUBBED_NAMED("Tag_Check: GameAudio_PlaySfx 0x32,&collision_position,0,0 PC547930");
            STUBBED_NAMED("Tag_Check: NewRumble object pad,0.5f,0 PC54a850");
            if (AIScriptSetBaseScriptStateByName(&target->ai_packet.script_processor,
                                                 (char *)TakeOverState_MapRunAway) != 0) {
                AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)target, &target->ai_packet,
                                &target->ai_packet.script_processor, FRAMETIME);
            }
            goto no_target;
        }
        goto attempt_tag;
    }
    object->tag_player_index = (i8)Tag_FindGameObject_CYCLICAL(object);
select_player:
    if (object->tag_player_index < 0) {
        goto no_target;
    }
    target = Player[object->tag_player_index];
    if (target == NULL) {
        goto no_target;
    }
attempt_tag:
    if ((target->flags_ef4 & 2) != 0) {
        TakeOver2GetIn(target, object);
        goto draw_icon;
    }
    if ((object->flags_1fc_word & 0x80) != 0 && (target->flags_1fc_word & 0x80) != 0) {
        if (player_tag_timer > 0.0f && object == player_tag_to && target == player_tag_from) {
            do_player_tag = 1;
        } else {
            player_tag_to = target;
            player_tag_from = object;
            player_tag_timer = 0.5f;
        }
        goto draw_icon;
    }
    result = TagCode(object, target, 0, 0, 1);
    if (result == 2) {
        object->tag_flags = (object->tag_flags | 4) & ~8u;
        object->tag_cooldown = 1.0f;
        object->pending_tag_target = target;
        goto draw_icon;
    }
tagged_result:
    if (result != 1) {
        goto draw_icon;
    }
    object->tag_flags &= ~4u;
    STUBBED_NAMED("Tag_Check: GameAudio_PlaySfx 0x21,&collision_position,0,0 PC547930");
    if (target->model_draw_result != 0) {
        STUBBED_NAMED("Tag_Check: GameCam_Blend GameCam,0.5f,0,1 PC54a3e0");
    } else {
        STUBBED_NAMED("Tag_Check: GameCam_Reset GameCam PC54a2f0");
    }
    STUBBED_NAMED("Tag_Check: RememberPlayerIDs 0,Player0id-or-minus1,Player1id-or-minus1 PC55e1b0");
    if (Tag_Mode == 1 || Tag_Mode == 3) {
        STUBBED_NAMED("Tag_Check: Tag_NewTransfer object,target PC59fe30");
    }
    if (object->field_280 < 2 && target->field_280 < 2) {
        STUBBED_NAMED("Tag_Check: ResetTimer &JoinInTimer,0 PC548fb0");
    }
    goto draw_icon;
no_target:
    if (object->rider == NULL) {
        object->hud_icon_timer = 2.0f;
    }
draw_icon:
    if (Tag_DrawIconFn != NULL) {
        Tag_DrawIconFn(object);
    }
}

void __cdecl TakeOverCode(GAMEOBJECT *object, i32 tag_pressed) {
    GAMEOBJECT *target;
    if (object->character_context == 0x3c) {
        f32 fraction;
        NUVEC destination;
        f32 height;
        f64 phase;
        i32 difference;
        u16 angle;
        target = object->force_target;
        if (target == NULL) {
            object->character_context = -1;
            return;
        }
        object->context_timer += FRAMETIME;
        if (object->context_timer >= object->context_duration) {
            object->character_context = -1;
            if ((object->flags_1fc_word & 0x80) != 0) {
                STUBBED_NAMED("TakeOverCode: Hint_SetComplete selected target ID/movement callback and "
                              "StartupGameFlag1 branch PC576960");
            }
            STUBBED_NAMED("TakeOverCode: WORLDgrabber+46ec and grab-control IDs select PlaySfx env_crane_in PC681f40 "
                          "or PlayLandSfx object,0,0 PC548020");
            STUBBED_NAMED("TakeOverCode: NewBuzz object pad,0.1f,0 PC54a8a0");
            TakeOverGameObject2(object, object->force_target, 1);
            return;
        }
        fraction = object->context_timer / object->context_duration;
        GetTakeOverPos(target, &destination);
        object->position.x = (destination.x - object->external_force.x) * fraction + object->external_force.x;
        object->position.y = (destination.y - object->external_force.y) * fraction + object->external_force.y;
        object->position.z = (destination.z - object->external_force.z) * fraction + object->external_force.z;
        target = object->force_target;
        height = target->bounds_half_height;
        if (object->field_7aa == 6 || object->field_7aa == 0xc9) {
            f32 jump_height = object->character_data->game_character->jump_height;
            if (jump_height > height) {
                height = jump_height;
            }
        }
        if (target->id == id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
            height *= TakeOverATATJumpScale * SpeederChaseATATInOutMul(&destination, &object->external_force);
        }
        phase = (f64)fraction * 32768.0;
        object->position.y += height * NuTrigTable[((i32)phase >> 1) & 0x7fff];
        target = object->force_target;
        difference = RotDiff(object->takeover_start_angle, target->field_0x276);
        angle = (u16)(i32)((f64)object->takeover_start_angle +
                           (f64)difference *
                               (1.0 - ((f64)NuTrigTable[((i32)(phase + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5));
        object->field_0x276 = angle;
        object->movement_facing_angle = angle;
        object->facing_angle = angle;
        return;
    }
    object->tag_flags &= ~1u;
    target = object->rider;
    if (target != NULL) {
        if (object->character_context == 0x3b) {
            if (!(players_cannot_exit_speeder != 0 && target->id == id_SPEEDERBIKE)) {
                u32 buttons = target->pad->buttons_pressed;
                if ((object->flags_ef4 & 2) != 0) {
                    buttons &= ~GAMEPAD_JUMP;
                }
                if (target->character_context != 0x2a && (buttons & (GAMEPAD_JUMP | GAMEPAD_TAG)) != 0) {
                    ReleaseTakeOver(object, 1);
                    if ((object->flags_1fc_word & 0x80) != 0) {
                        STUBBED_NAMED("TakeOverCode: Hint_SetComplete StartupGameFlag1?0x25e:5 PC576960");
                    }
                    return;
                }
            }
            SetRiderPos(object, NULL);
            object->takeover_timer = 1.0f;
        } else if (object->character_data->game_character->weapon_action == 12) {
            i32 animation =
                CurrentAnim(&object->anim_packet) == 3 || CurrentAnim(&object->anim_packet) == 0x17 ? 0xc4 : 0xc3;
            CHARACTER_MODEL_LAYERS_PC *model;
            target = object->rider;
            model = (CHARACTER_MODEL_LAYERS_PC *)target->character_model;
            if (model->action_animations[animation] != NULL) {
                target->field_7aa = (i16)animation;
            }
        }
        return;
    }
    if (object->takeover_timer >= 0.0f) {
        object->takeover_timer -= FRAMETIME;
        return;
    }
    if (MiniCutCam != 0 || (object->flags_1fc_word & 0x80) == 0 || object->dead != 0 ||
        !(object->takeover_timer <= 0.0f) || object->character_context == 0x2b ||
        (object->character_context != -1 && (object->character_context != 0 || object->action_movement_state == 3 ||
                                             object->action_movement_state == 4)) ||
        object->field_281 == 0) {
        return;
    }
    {
        f32 nearest_distance = 9.0f;
        GAMEOBJECT *nearest = NULL;
        i32 index;
        for (index = 0; index < HIGHGAMEOBJECT; ++index) {
            GAMEOBJECT *candidate = &Obj[index];
            f32 dx, dz, range, range_squared, distance;
            if ((candidate->flags_1fc_word & 0x1001) != 0x1001 || candidate->dead != 0 ||
                candidate->field_280 != 0xff || (CInfo[candidate->character_context].flags & 0x8000) != 0 ||
                candidate->rider != NULL || (candidate->character_data->game_character->flags_090 & 0x40) == 0 ||
                ((candidate->character_data->model_flags & 0x40000000) != 0 &&
                 (candidate->character_context == 0x17 || candidate->character_context == 0x3e)) ||
                (candidate->tag_flags & 2) != 0) {
                continue;
            }
            if (candidate->id == id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
                if (object->jump_animation_state == 0x13) {
                    nearest = candidate;
                }
                continue;
            }
            if (object->collision_min.y > candidate->collision_max.y ||
                candidate->collision_min.y > object->collision_max.y) {
                continue;
            }
            range = (f64)candidate->bounds_radius * 2.0 + object->bounds_radius;
            dx = candidate->collision_position.x - object->collision_position.x;
            dz = candidate->collision_position.z - object->collision_position.z;
            if (!(object->facing_direction.x * dx + object->facing_direction.z * dz > 0.0f)) {
                continue;
            }
            distance = dx * dx + dz * dz;
            range_squared = range * range;
            if (range_squared > distance && nearest_distance > distance) {
                nearest_distance = distance;
                nearest = candidate;
            }
        }
        if (nearest != NULL) {
            if (tag_pressed != 0 || (nearest->id == id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA)) {
                TakeOver2GetIn(object, nearest);
            } else {
                object->tag_flags |= 1;
                nearest->flags_e20 |= 0x80000000;
            }
        }
    }
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

void __cdecl RemoveGameObject(GAMEOBJECT *object, i32 unused) {
    i32 i;
    (void)unused;
    if (object == NULL)
        return;
    for (i = 0; i < HIGHGAMEOBJECT; ++i) {
        u64 bit = object->hostility_group < 64 ? ((u64)1 << object->hostility_group) : 0;
        u32 low = ~(u32)bit, high = ~(u32)(bit >> 32);
        Obj[i].collision_contact_mask[0] &= ~object->collision_identity_mask[0];
        Obj[i].collision_contact_mask[1] &= ~object->collision_identity_mask[1];
        Obj[i].hostility_mask[0] &= low;
        Obj[i].hostility_mask[1] &= high;
        Obj[i].field_eb0 &= low;
        Obj[i].field_eb4 &= high;
        Obj[i].ai_opponent_exclusion_mask[0] &= low;
        Obj[i].ai_opponent_exclusion_mask[1] &= high;
    }
    if (object->pad != NULL)
        object->pad->runtime_flags &= ~1u;
    STUBBED_NAMED("RemoveGameObject: DestroySnakeBody PC597350");
    STUBBED_NAMED("RemoveGameObject: APIObjectDestroy WORLD->object_sys,object PC62a910");
    RecomputeHighGameObject();
    for (i = 0; i < 8; ++i) {
        if (LevGameObject[i] == object)
            LevGameObject[i] = NULL;
    }
}

void __cdecl DeactivateGameObject(GAMEOBJECT *object) {
    if (object == NULL)
        return;
    if (object->rider != NULL) {
        if ((object->flags_1f8 & 0x4000) != 0)
            ReleaseTakeOver(object, 1);
        else
            KillGameObject(object->rider, 4, 0);
    }
    object->flags_1fc_word &= ~0x1000u;
    if (object->ai_packet.creature_index != 0xff &&
        AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, (char *)GameObjectState_InActive) != 0) {
        object->ai_packet.reset_mode = 0;
        WORLD->ai_sys->creatures[object->ai_packet.creature_index].activation_type = 2;
    } else {
        object->ai_packet.reset_mode = 4;
    }
}

void __cdecl ManageGameObjects(void) {
    i32 i, j, area;
    GAMEOBJECT *candidate, *other, *vehicle, *target;
    AICREATURE_PC *creature;
    AISYS_PC *system;
    AIGROUP_PC *group;
    AILOCATOR_PC *locator, *respawn_locator;
    APIOBJECT_PC *member;
    u16 saved_flags;
    u32 contact_low, contact_high, low, high, limit;
    u64 bit;
    i64 mask;
    f32 blend;
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
        if ((object->flags_1f8 & 0x40000) != 0) {
            if (nethost != 0 && (object->flags_1fc_word & 0x1000) == 0 && object->ai_packet.creature_index == 0xff &&
                (object->ai_packet.reset_mode & ~2) != 1 && (object->flags_1f8 & 0x4000) != 0)
                AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet,
                                &object->ai_packet.script_processor, FRAMETIME);
            continue;
        }
        if ((object->flags_1fc_word & 0x1000) != 0) {
            if (object->dead == 0)
                continue;
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
                        object->start_position = other->position;
                        object->position = other->position;
                        object->ai_update_position = other->position;
                        GameObjectOrigin(object);
                        object->velocity = other->velocity;
                        object->movement_facing_angle = other->field_0x276;
                        object->facing_angle = other->field_0x276;
                        object->field_0x276 = other->field_0x276;
                    } else {
                        SetToLastSafePos(object);
                        GameObjectOrigin(object);
                        object->velocity.x = object->velocity.z = 0.0f;
                        object->velocity.y = -0.1f;
                        object->facing_angle = object->movement_facing_angle;
                        object->field_0x276 = object->movement_facing_angle;
                    }
                    saved_flags = object->flags_1fc_word & 0x2000;
                    STUBBED_NAMED("ManageGameObjects: ResetPlayerMoves PC41af70");
                    object->dead = 0;
                    object->flags_1fc_word = (object->flags_1fc_word & ~0x2000) | saved_flags;
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
                    if ((object->flags_1fc_word & 0x80) && WORLD->current_level == SPEEDERCHASEA_LDATA &&
                        disable_narrow_socks == 0)
                        STUBBED_NAMED("ManageGameObjects: GameCam_Blend GameCam,0.5f,0.0f,0 PC54a3e0");
                } else if (WORLD->current_level == PODSPRINTA_LDATA) {
                    other = GetOtherActivePlayer(object);
                    if (other != NULL)
                        object->position = other->position;
                }
                continue;
            }
            object->death_timer += FRAMETIME;
            if (!(object->death_timer >= object->death_delay))
                continue;
            object->death_timer = object->death_delay;
            contact_low = ~object->collision_identity_mask[0];
            contact_high = ~object->collision_identity_mask[1];
            bit = object->hostility_group < 64 ? ((u64)1 << object->hostility_group) : 0;
            low = ~(u32)bit, high = ~(u32)(bit >> 32);
            for (j = 0; j < HIGHGAMEOBJECT; ++j) {
                Obj[j].collision_contact_mask[0] &= contact_low;
                Obj[j].collision_contact_mask[1] &= contact_high;
                Obj[j].hostility_mask[0] &= low;
                Obj[j].hostility_mask[1] &= high;
                Obj[j].field_eb0 &= low;
                Obj[j].field_eb4 &= high;
                Obj[j].ai_opponent_exclusion_mask[0] &= low;
                Obj[j].ai_opponent_exclusion_mask[1] &= high;
            }
            if ((object->flags_1f8 & 0x4000) != 0) {
                if ((object->flags_1f8 & 0x400) != 0) {
                    creature = &WORLD->ai_sys->creatures[object->ai_packet.creature_index];
                    if ((object->portal_flags & 0x100000) != 0) {
                        if ((object->portal_flags & 0x200000) == 0) {
                            object->field_1010 =
                                object->id == id_STAP2 && WORLD->current_level == NEGOTIATIONSC_LDATA ? 5.0f : 1.0f;
                            continue;
                        }
                    } else {
                        limit = object->ai_packet.respawn_count + 1;
                        if (creature->max_respawn_count != -1 && object->ai_packet.respawn_locator == NULL)
                            limit = creature->min_respawn_count +
                                    (Game.difficulty - 1) *
                                        (creature->max_respawn_count - creature->min_respawn_count) / 9 +
                                    1;
                        if (limit <= object->ai_packet.respawn_count) {
                            object->flags_1fc_word &= ~0x1000;
                            object->ai_packet.reset_mode = 4;
                            continue;
                        }
                    }
                    object->flags_1fc_word &= ~0x1000;
                    object->ai_packet.reset_mode = 1;
                    blend = 1.0 - ((f64)(u32)Game.difficulty - 1.0) / 9.0;
                    object->ai_packet.spawn_delay =
                        creature->max_respawn_time * blend + creature->min_respawn_time * (1.0f - blend);
                } else {
                    object->field_1010 = 2.0f;
                }
                continue;
            }
            if ((object->portal_flags & 0x100000) != 0) {
                object->field_1010 = 1.0f;
                continue;
            }
            goto remove_object;
        }
        if (object->ai_packet.creature_index != 0xff) {
            system = WORLD->ai_sys;
            creature = &system->creatures[object->ai_packet.creature_index];
            if (object->ai_packet.reset_mode == 0) {
                switch (creature->activation_type) {
                    case 0:
                        if (Game.difficulty >= creature->activation_difficulty)
                            STUBBED_NAMED("ManageGameObjects: ResetAICreature object/system PC41c120");
                        else
                            object->ai_packet.reset_mode = 4;
                        break;
                    case 1:
                        if (creature->activate_area == NULL) {
                            creature->activation_type = 0;
                        } else {
                            area = creature->activate_area - system->areas;
                            mask = (i32)(1u << (area & 31));
                            if (player != NULL &&
                                (((u64)player->ai_area_mask[0] | ((u64)player->ai_area_mask[1] << 32)) & mask) != 0) {
                                if (Game.difficulty < creature->activation_difficulty) {
                                    object->ai_packet.reset_mode = 4;
                                } else if (creature->count > 1 && creature->start_stagger > 0.0f &&
                                           object->ai_packet.group_member_index != 0) {
                                    object->ai_packet.reset_mode = 1;
                                    object->ai_packet.spawn_delay =
                                        (f32)(u32)object->ai_packet.group_member_index * creature->start_stagger;
                                } else {
                                    STUBBED_NAMED("ManageGameObjects: ResetAICreature object/system PC41c120");
                                }
                            }
                        }
                        break;
                    case 2:
                        if (Game.difficulty >= creature->activation_difficulty)
                            AIScriptProcess(system, (APIOBJECT_PC *)object, &object->ai_packet,
                                            &object->ai_packet.script_processor, FRAMETIME);
                        break;
                }
            } else if (object->ai_packet.reset_mode == 1) {
                group = object->ai_packet.group;
                if (group != NULL && !(group->flags & 2))
                    continue;
                if (creature->activation_type == 1) {
                    area = creature->activate_area - system->areas;
                    mask = (i32)(1u << (area & 31));
                    if (player == NULL ||
                        (((u64)player->ai_area_mask[0] | ((u64)player->ai_area_mask[1] << 32)) & mask) == 0 ||
                        Game.difficulty < creature->activation_difficulty) {
                        if (creature->count > 1 && creature->start_stagger > 0.0f)
                            object->ai_packet.spawn_delay =
                                (f32)(u32)object->ai_packet.group_member_index * creature->start_stagger;
                        continue;
                    }
                }
                object->ai_packet.spawn_delay -= FRAMETIME;
                if (!(object->ai_packet.spawn_delay <= 0.0f))
                    continue;
                if (group != NULL) {
                    locator = object->ai_packet.script_processor.locator;
                    respawn_locator = object->ai_packet.respawn_locator;
                    for (j = 0; j < group->member_count; ++j) {
                        member = group->members[j];
                        if (member != NULL) {
                            target = member->owner;
                            target->ai_packet.spawn_delay = 0.0f;
                            target->ai_packet.script_processor.locator = locator;
                            target->ai_packet.respawn_locator = respawn_locator;
                            STUBBED_NAMED("ManageGameObjects: ResetAICreature group member PC41c120");
                            group = object->ai_packet.group;
                            if (group->leader == NULL)
                                group->leader = group->members[j];
                        }
                        group = object->ai_packet.group;
                    }
                    group->flags &= ~8u;
                    object->ai_packet.group->flags &= ~4u;
                } else {
                    object->ai_packet.spawn_delay = 0.0f;
                    if (object->id == id_PKDROID) {
                        STUBBED_NAMED("ManageGameObjects: ResetAICreature object/system PC41c120");
                        STUBBED_NAMED("ManageGameObjects: SpawnCreatureFromCrate object,3.0f,0.0f PC4302e0");
                    } else {
                        STUBBED_NAMED("ManageGameObjects: ResetAICreature object/WORLD->ai_sys PC41c120");
                    }
                }
            }
            continue;
        }
        if (object->ai_packet.reset_mode == 1) {
            object->ai_packet.spawn_delay -= FRAMETIME;
            if (object->ai_packet.spawn_delay <= 0.0f) {
                object->ai_packet.reset_mode = 2;
                object->ai_packet.spawn_delay = 0.0f;
                object->flags_1fc_word |= 0x1000;
            }
            continue;
        }
        if (object->ai_packet.reset_mode == 3)
            continue;
        if ((object->flags_1f8 & 0x4000) != 0) {
            AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet,
                            &object->ai_packet.script_processor, FRAMETIME);
            continue;
        }
    remove_object:;
        group = object->ai_packet.group;
        if (group != NULL) {
            if (group->rows[0].is_alive == 0 && group->rows[1].is_alive == 0 && group->rows[2].is_alive == 0 &&
                group->rows[3].is_alive == 0) {
                STUBBED_NAMED("ManageGameObjects: DestroyAIGroup PC5181e0");
            } else {
                for (j = 0; j < group->member_count; ++j) {
                    if (group->members[j] == (APIOBJECT_PC *)object)
                        group->members[j] = NULL;
                }
            }
        }
        STUBBED_NAMED("ManageGameObjects: FreeTorpedoPacket PC4d22f0");
        RemoveGameObject(object, 1);
    }
}
