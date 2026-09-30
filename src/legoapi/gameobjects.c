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
        if ((object->character_data->model_flags & 0x2000) != 0) {
            STUBBED_NAMED("GameObjectRotation: vehicle-specific hover-height query and dependent ground alignment (PC "
                          "helper 0x498590)");
        } else if (game->field_028 > 0.0f) {
            f32 hover = game->field_028;
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
    STUBBED_NAMED(GameObjectOrigin_ContextTerrainAdjustment);
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
