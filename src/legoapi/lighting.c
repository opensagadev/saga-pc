#include "saga.h"

static void ZeroLightSetupPrivatePc(NUVEC *direction0, NUVEC *colour0, NUVEC *direction1, NUVEC *colour1,
                                    NUVEC *direction2, NUVEC *colour2) {
    PCGlobalLightingState.direction[0] = *direction0;
    PCGlobalLightingState.direction[1] = *direction1;
    PCGlobalLightingState.direction[2] = *direction2;
    PCGlobalLightingState.intensity[0] = *colour0;
    PCGlobalLightingState.intensity[1] = *colour1;
    PCGlobalLightingState.intensity[2] = *colour2;
    NuRndrSetDirectionalLightsPS(direction0, colour0, direction1, colour1, direction2, colour2);
}
void __cdecl SetZeroLights(void) {
    NUVEC black = {0, 0, 0};
    ZeroLightSetupPrivatePc(&LightingUnitDirectionPc, &black, &LightingUnitDirectionPc, &black,
                            &LightingUnitDirectionPc, &black);
    PCGlobalLightingState.ambient = APIObjectZeroMomentumPC;
    NuRndrSetAmbientLightPS(&APIObjectZeroMomentumPC);
}

void __cdecl FindAndSetLights(NUVEC *position, f32 scale, void *set) {
    RTL_DATA_PC lights;
    rtlResetEx(&lights, 1);
    rtlDynamicMasterEnable(0);
    rtlApplySetScale(set, &lights, position, NULL, -1, scale);
    rtlDynamicMasterEnable(1);
    rtlSetLights(&lights);
}
void __cdecl ResetLights(NUVEC *position, RTL_DATA_PC *packet, void *set) {
    rtlResetEx(packet, 1);
    if (position != NULL)
        rtlApplySetScale(set, packet, position, NULL, -1, 1.0f);
}

typedef struct startup_object_light_pc_s {
    u8 unknown_000[0x1fc];
    u32 flags;
    u8 unknown_200[0xc04];
    i32 dynamic_light_id;
    u8 unknown_e08[0x2d0];
} STARTUP_OBJECT_LIGHT_PC;
STATIC_ASSERT(sizeof(STARTUP_OBJECT_LIGHT_PC) == sizeof(GAMEOBJECT));
STATIC_ASSERT(offsetof(STARTUP_OBJECT_LIGHT_PC, flags) == 0x1fc);
STATIC_ASSERT(offsetof(STARTUP_OBJECT_LIGHT_PC, dynamic_light_id) == 0xe04);
STATIC_ASSERT(offsetof(NULNKHDR, id) == 0xc);
i32 __cdecl rtlDynamicAlloc(void);
i32 __cdecl rtlDynamicSetType(i32 id, i32 type);
void __cdecl InitGameObjectLights(void) {
    STARTUP_OBJECT_LIGHT_PC *objects = (STARTUP_OBJECT_LIGHT_PC *)Obj;
    i32 index;
    for (index = 0; index < 64; ++index)
        objects[index].dynamic_light_id = -1;
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        if ((objects[index].flags & 0x1001) == 0x1001) {
            objects[index].dynamic_light_id = rtlDynamicAlloc();
            if (objects[index].dynamic_light_id != -1) {
                rtlDynamicSetType(objects[index].dynamic_light_id, 2);
                rtlDynamicEnable(objects[index].dynamic_light_id, 0);
            }
        }
    }
}

typedef struct OBJECT_LIGHT_STATE_GAME_PC {
    NUVEC ambient;
    NUVEC intensity[3];
    NUVEC direction[3];
} OBJECT_LIGHT_STATE_GAME_PC;
STATIC_ASSERT(offsetof(OBJECT_LIGHT_STATE_GAME_PC, intensity) == 0xc);
STATIC_ASSERT(offsetof(OBJECT_LIGHT_STATE_GAME_PC, direction) == 0x30);
typedef struct GAME_LIGHT_VIEW_PC {
    u8 unknown000[0x285];
    i8 floor_surface;
    u8 unknown286[0x4d8 - 0x286];
    RTL_DATA_PC light_data;
    OBJECT_LIGHT_STATE_GAME_PC lighting_state;
} GAME_LIGHT_VIEW_PC;
STATIC_ASSERT(offsetof(GAME_LIGHT_VIEW_PC, floor_surface) == 0x285);
STATIC_ASSERT(offsetof(GAME_LIGHT_VIEW_PC, light_data) == 0x4d8);
STATIC_ASSERT(offsetof(GAME_LIGHT_VIEW_PC, lighting_state) == 0x61c);
STATIC_ASSERT(sizeof(OBJECT_LIGHT_STATE_GAME_PC) == 84);
void __cdecl LightGameObject(GAMEOBJECT *object, void *set) {
    GAME_LIGHT_VIEW_PC *view = (GAME_LIGHT_VIEW_PC *)object;
    RTL_DATA_PC *target = &view->light_data;
    OBJECT_LIGHT_STATE_GAME_PC *current = &view->lighting_state;
    f32 surface_fade;
    i32 index;
    rtlResetEx(target, 1);
    rtlApplySetScale(set, target, &object->collision_position, NULL, -1, 1.0f);
    surface_fade = object->floor_height != 2000000.0f && (TerSurface[view->floor_surface].flags & 8) != 0 ? 1.0f : 0.0f;
    if (object->flags_ef0 & 0x80)
        object->surface_flag8_scale_pc = surface_fade;
    else
        object->surface_flag8_scale_pc =
            SeekLinearF(object->surface_flag8_scale_pc, surface_fade, (f32)((f64)FRAMETIME * 3.0));
    if (object->flags_ef0 & 0x80)
        current->ambient = target->ambient;
    else {
        current->ambient.x = SeekValF(current->ambient.x, target->ambient.x, 5.0f);
        current->ambient.y = SeekValF(current->ambient.y, target->ambient.y, 5.0f);
        current->ambient.z = SeekValF(current->ambient.z, target->ambient.z, 5.0f);
    }
    for (index = 0; index < 3; ++index) {
        if (object->flags_ef0 & 0x80) {
            current->intensity[index] = target->intensity_vectors[index];
            current->direction[index] = target->direction[index];
        } else {
            current->intensity[index].x =
                SeekValF(current->intensity[index].x, target->intensity_vectors[index].x, 5.0f);
            current->intensity[index].y =
                SeekValF(current->intensity[index].y, target->intensity_vectors[index].y, 5.0f);
            current->intensity[index].z =
                SeekValF(current->intensity[index].z, target->intensity_vectors[index].z, 5.0f);
            current->direction[index].x = SeekValF(current->direction[index].x, target->direction[index].x, 5.0f);
            current->direction[index].y = SeekValF(current->direction[index].y, target->direction[index].y, 5.0f);
            current->direction[index].z = SeekValF(current->direction[index].z, target->direction[index].z, 5.0f);
        }
        if (current->direction[index].x != 0.0f || current->direction[index].y != 0.0f ||
            current->direction[index].z != 0.0f)
            NuVecNorm(&current->direction[index], &current->direction[index]);
        else
            current->direction[index] = target->direction[index];
    }
    object->flags_ef0 &= ~0x80u;
}
