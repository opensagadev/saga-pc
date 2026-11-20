#include "gamelib/util/Utilities.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

extern const f64 GameObjectNearFloorNoFloorHeight;

void SurfaceMaskOn(u32 *surface_mask) {
    *surface_mask = 0x20;
}

void SurfaceMaskOff(u32 *surface_mask) {
    *surface_mask = 0;
}

u16 SURFACEBITS_DUST = 0;
u16 SURFACEBITS_NODUST = 0;
PC_SURFACE_MASK GetSurfaceInfoRuntimeMask = {0};

void Surfaces_Reset(void) {
    i32 i;
    LAYER_HOVERIGNORE = 0x10;
    SURFACEBITS_DUST = 0;
    SURFACEBITS_NODUST = 0;
    for (i = 0; i < 32; ++i) {
        u16 surface_bit = (u16)(1 << i);
        if (TerSurface[i].flags & TERSURFACE_DUST) {
            SURFACEBITS_DUST |= surface_bit;
        } else {
            SURFACEBITS_NODUST |= surface_bit;
        }
    }
}

void (*SurfaceInfo_ExtraReflectFn)(GAMEOBJECT *object) = 0;

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
        if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_GHOST) == 0 &&
            (TerSurface[surface_index].flags & TERSURFACE_REFLECTIVE) != 0) {
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
        object->player.raw_surface_id = (u8)raw_surface;
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
        object->ground_layer = material;
        if (VehicleArea != 0 && BonusArea != 0 && (material == 7 || material == 16)) {
            object->water_height = 2000000.0f;
            object->tilt_target_roll = 0;
            object->tilt_target_pitch = 0;
            object->ground_layer = -1;
        } else {
            if (material < 0 || material >= 17) {
                object->ground_layer = 0;
            }
            material_mask = GetSurfaceInfoDefaultMask.low | GetSurfaceInfoRuntimeMask.low | 0x40u;
            object->ground_layer = (i8)((u8)object->ground_layer & (u8)~material_mask);
            FindAnglesZX(&GetSurfaceInfoWaterNormal, NULL, NULL);
            object->tilt_target_pitch = temp_zrot;
            object->tilt_target_roll = temp_xrot;
        }
    } else {
        object->tilt_target_roll = 0;
        object->tilt_target_pitch = 0;
        object->ground_layer = -1;
    }

    object->roof_height = GetSurfaceInfoRoofHeight;
    if (object->roof_height != GameObjectNearFloorNoFloorHeight) {
        roof_surface = (i8)ShadowRoofInfo();
        object->roof_surface_id = (u8)roof_surface;
        if (roof_surface < 0 || roof_surface >= 32) {
            object->roof_surface_id = 0;
        }
    } else {
        object->roof_surface_id = 0xff;
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
        (WorldInfo_CurrentlyActive()->current_level->flags & LEVEL_IN_SPACE) == 0) {
        if (mode == 2) {
            y = (f64)object->collision_min.y - (f64)object->character_bottom * object->origin_scale + 0.10000000149011612;
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
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    if (world != NULL && world->belt_active != 0) {
        velocity->x += world->current_level->conveyor_x_speed;
        velocity->z += world->current_level->conveyor_z_speed;
    }
    return 0;
}

i32 UnderWater(GAMEOBJECT *object) {
    return (object->ground_layer == 1 || object->ground_layer == 9) && object->collision_position.y <= object->water_height;
}

i32 IntersectWater(GAMEOBJECT *object) {
    if (object->ground_layer != 1 && object->ground_layer != 9) {
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
            object->player.surface_flag8_scale = 1.0f;
        } else {
            object->player.surface_flag8_scale = 0.0f;
        }
        FindAnglesZX(&object->surface_normal, NULL, NULL);
        object->surface_pitch = temp_zrot;
        object->shadow_pitch = temp_zrot;
        object->surface_roll = temp_xrot;
        object->shadow_roll = temp_xrot;
    } else {
        GetSurfaceInfo(object, 0, object->floor_height);
        object->player.surface_flag8_scale = 0.0f;
        object->jump_animation_state = 0;
        object->player.raw_surface_id = 0;
        object->surface_normal = v010;
    }

    object->in_water = (object->ground_layer == 1 || object->ground_layer == 9) && object->collision_position.y <= object->water_height;
    object->water_intersects = (object->ground_layer == 1 || object->ground_layer == 9) &&
                               object->water_height <= object->collision_max.y && object->water_height >= object->collision_min.y;

    object->water_height = LevelConfigFarDistance;
    object->roof_height = LevelConfigFarDistance;
    object->ground_layer = -1;
    object->roof_surface_id = 0xff;
    GetSurfaceInfo(object, 1, object->floor_height);

    if (!(object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_GHOST) &&
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
