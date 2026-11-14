#include "saga.h"
#include <string.h>

extern const f64 GameObjectNearFloorNoFloorHeight;

typedef struct world_conveyor_pc_s {
    u8 unknown_00[0x128];
    LEVELDATA_PC *current_level;
    u8 unknown_12c[0x5044];
    void *conveyor;
} WORLD_CONVEYOR_PC;

void SurfaceMaskOn(u32 *surface_mask) {
    *surface_mask = 0x20;
}

void SurfaceMaskOff(u32 *surface_mask) {
    *surface_mask = 0;
}

void Surfaces_Reset(void) {
    i32 i;
    LAYER_HOVERIGNORE = 0x10;
    SURFACEBITS_DUST = 0;
    SURFACEBITS_NODUST = 0;
    for (i = 0; i < 32; ++i) {
        u16 surface_bit = (u16)(1 << i);
        if (TerSurface[i].flags & 4) {
            SURFACEBITS_DUST |= surface_bit;
        } else {
            SURFACEBITS_NODUST |= surface_bit;
        }
    }
}

void GetSurfaceInfo(GAMEOBJECT *object, i32 update_fields, f32 floor_height) {
    i32 raw_surface;
    i32 surface_index;
    i8 material;
    i8 roof_surface;
    u8 material_mask;
    void (*extra_reflect)(GAMEOBJECT *);

    if (floor_height != GameObjectNearFloorNoFloorHeight) {
        raw_surface = ShadowInfo();
        surface_index = raw_surface;
        if (surface_index < 0 || surface_index >= 32) {
            surface_index = 0;
        }
        object->reflection_enabled = 2;
        if ((object->character_data->game_character->flags_090 & 0x8000u) == 0 &&
            (TerSurface[surface_index].flags & 2u) != 0) {
            object->reflection_height = floor_height;
        } else {
            object->reflection_height = 2000000.0f;
        }
        object->terrain_platform_id = (i16)NewShadowOnPlatform();
    } else {
        raw_surface = 0;
        surface_index = 0;
        object->reflection_height = 2000000.0f;
        object->reflection_enabled = 0;
        object->terrain_platform_id = -1;
    }

    if (update_fields != 0) {
        object->raw_surface_id_pc = (u8)raw_surface;
        object->jump_animation_state = (u8)surface_index;
        if (object->floor_height == GameObjectNearFloorNoFloorHeight) {
            object->surface_normal = v010;
        } else {
            object->surface_normal = ShadNorm;
        }
    }

    object->water_height = EShadY;
    if (object->water_height != GameObjectNearFloorNoFloorHeight) {
        material = (i8)EShadowInfo();
        object->field_283 = material;
        if (VehicleArea != 0 && BonusArea != 0 && (material == 7 || material == 16)) {
            object->water_height = 2000000.0f;
            object->field_105e = 0;
            object->field_105c = 0;
            object->field_283 = -1;
        } else {
            if (material < 0 || material >= 17) {
                object->field_283 = 0;
            }
            material_mask = GetSurfaceInfoDefaultMaskPc.low | GetSurfaceInfoRuntimeMaskPc.low | 0x40u;
            object->field_283 = (i8)((u8)object->field_283 & (u8)~material_mask);
            FindAnglesZX(&GetSurfaceInfoWaterNormalPc, NULL, NULL);
            object->field_105c = temp_zrot;
            object->field_105e = temp_xrot;
        }
    } else {
        object->field_105e = 0;
        object->field_105c = 0;
        object->field_283 = -1;
    }

    object->roof_height_pc = GetSurfaceInfoRoofHeightPc;
    if (object->roof_height_pc != GameObjectNearFloorNoFloorHeight) {
        roof_surface = (i8)ShadowRoofInfo();
        object->roof_surface_id_pc = (u8)roof_surface;
        if (roof_surface < 0 || roof_surface >= 32) {
            object->roof_surface_id_pc = 0;
        }
    } else {
        object->roof_surface_id_pc = 0xff;
    }

    extra_reflect = SurfaceInfo_ExtraReflectFn;
    if (extra_reflect != NULL) {
        extra_reflect(object);
    }
}

i32 SetObjOnSurface(GAMEOBJECT *object, i32 mode) {
    f32 y;
    i32 placed;

    placed = 0;
    if (object->floor_height != GameObjectNearFloorNoFloorHeight &&
        (WorldInfo_CurrentlyActive()->current_level->flags & 0x40000u) == 0) {
        if (mode == 2) {
            y = (f64)object->collision_min.y - (f64)object->character_bottom * object->origin_scale +
                0.10000000149011612;
            object->position.y = y;
            object->initial_position.y = y;
            object->collision_position.y = y;
        }
        if (mode == 0 || !((f64)object->position.y + (f64)object->character_bottom * object->origin_scale >
                           (f64)object->floor_height + 0.10000000149011612)) {
            y = (f64)object->floor_height - (f64)object->character_bottom * object->origin_scale + 0.10000000149011612;
            object->position.y = y;
            object->initial_position.y = y;
            object->collision_position.y = y;
            placed = 1;
        }
        object->velocity.y = -1.0f;
    }
    return placed;
}

i32 Conveyor_AdjustSpeed(NUVEC *velocity) {
    WORLD_CONVEYOR_PC *world = (WORLD_CONVEYOR_PC *)WorldInfo_CurrentlyActive();
    if (world != NULL && world->conveyor != NULL) {
        velocity->x += world->current_level->conveyor_x_speed;
        velocity->z += world->current_level->conveyor_z_speed;
    }
    return 0;
}

i32 UnderWater(GAMEOBJECT *object) {
    return (object->field_283 == 1 || object->field_283 == 9) && object->collision_position.y <= object->water_height;
}

i32 IntersectWater(GAMEOBJECT *object) {
    if (object->field_283 != 1 && object->field_283 != 9) {
        return 0;
    }
    if (object->collision_max.y < object->water_height) {
        return 0;
    }
    if (object->collision_min.y > object->water_height) {
        return 0;
    }
    return 1;
}

void Surface_Deflect(NUVEC *normal, NUVEC *movement, NUVEC *result, i32 mode) {
    f32 correction;
    correction = (-movement->x) * normal->x + (-movement->y) * normal->y + (-movement->z) * normal->z;
    if (mode == 2) {
        correction = correction + FRAMETIME * (f64)0.015f;
    } else if (mode == 1) {
        correction = correction - FRAMETIME * (f64)0.006f;
    }
    result->x = movement->x + normal->x * correction;
    result->y = movement->y + normal->y * correction;
    result->z = movement->z + normal->z * correction;
}

void InitSurfaceInfo(GAMEOBJECT *object) {
    void (*extra_reflect)(GAMEOBJECT *);

    object->floor_height = GameShadow(object, &object->position, 5.0f, -1);
    if (object->floor_height != GameObjectNearFloorNoFloorHeight) {
        GetSurfaceInfo(object, 1, object->floor_height);
        if (TerSurface[(i8)object->jump_animation_state].flags & 8) {
            object->surface_flag8_scale_pc = 1.0f;
        } else {
            object->surface_flag8_scale_pc = 0.0f;
        }
        FindAnglesZX(&object->surface_normal, NULL, NULL);
        object->field_1056 = temp_zrot;
        object->field_1052 = temp_zrot;
        object->field_1058 = temp_xrot;
        object->field_1054 = temp_xrot;
    } else {
        GetSurfaceInfo(object, 0, object->floor_height);
        object->surface_flag8_scale_pc = 0.0f;
        object->jump_animation_state = 0;
        object->raw_surface_id_pc = 0;
        object->surface_normal = v010;
    }

    object->unknown_0286 =
        (object->field_283 == 1 || object->field_283 == 9) && object->collision_position.y <= object->water_height;
    object->water_intersects = (object->field_283 == 1 || object->field_283 == 9) &&
                               object->water_height <= object->collision_max.y &&
                               object->water_height >= object->collision_min.y;

    object->water_height = LevelConfigFarDistance;
    object->roof_height_pc = LevelConfigFarDistance;
    object->field_283 = -1;
    object->roof_surface_id_pc = 0xff;
    GetSurfaceInfo(object, 1, object->floor_height);

    if (!(object->character_data->game_character->flags_090 & 0x8000) &&
        object->reflection_height == GameObjectNearFloorNoFloorHeight && object->terrain_platform_id != -1) {
        f32 reflection = FindReflectionNoPlatforms(&object->position);
        if (reflection != GameObjectNearFloorNoFloorHeight) {
            object->reflection_height = reflection;
            object->reflection_enabled = 2;
        }
    }

    extra_reflect = SurfaceInfo_ExtraReflectFn;
    if (extra_reflect != NULL) {
        extra_reflect(object);
    }
}

void fun_00572ac0(void) {
}
