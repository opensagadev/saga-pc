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

#include <stdio.h>
typedef struct BURN_PARAMETERS_PC {
    i32 field_00;
    f32 field_04;
    f32 field_08;
    f32 field_0c;
    f32 field_10;
    f32 field_14;
    i32 field_18;
    f32 field_1c;
    f32 field_20;
    f32 field_24;
    i32 field_28;
    f32 field_2c;
    f32 field_30;
    f32 field_34;
    f32 field_38;
    f32 field_3c;
    f32 field_40;
    f32 field_44;
    f32 field_48;
    f32 field_4c;
    f32 field_50;
} BURN_PARAMETERS_PC;
typedef struct BURN_OUT_PC {
    i32 active;
    NUVEC position;
    f32 field_10, field_14, field_18, field_1c, field_20;
} BURN_OUT_PC;
typedef struct BURN_SET_PC {
    BURN_PARAMETERS_PC parameters, parameters_copy;
    i32 field_a8, field_ac, field_b0, field_b4;
    f32 field_b8, field_bc, field_c0, field_c4, field_c8, field_cc;
    BURN_OUT_PC burnouts[32];
    i32 active_count, selected_index;
    f32 field_558, field_55c;
    i32 field_560;
} BURN_SET_PC;
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_00) == 0x0);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_00) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_04) == 0x4);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_04) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_08) == 0x8);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_08) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_0c) == 0xc);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_0c) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_10) == 0x10);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_10) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_14) == 0x14);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_14) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_18) == 0x18);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_18) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_1c) == 0x1c);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_1c) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_20) == 0x20);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_20) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_24) == 0x24);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_24) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_28) == 0x28);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_28) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_2c) == 0x2c);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_2c) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_30) == 0x30);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_30) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_34) == 0x34);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_34) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_38) == 0x38);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_38) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_3c) == 0x3c);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_3c) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_40) == 0x40);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_40) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_44) == 0x44);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_44) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_48) == 0x48);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_48) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_4c) == 0x4c);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_4c) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_50) == 0x50);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_50) == 4);
STATIC_ASSERT(sizeof(BURN_PARAMETERS_PC) == 0x54);
STATIC_ASSERT(sizeof(BURN_OUT_PC) == 0x24);
STATIC_ASSERT(sizeof(BURN_SET_PC) == 0x564);
STATIC_ASSERT(offsetof(BURN_OUT_PC, active) == 0x0);
STATIC_ASSERT(offsetof(BURN_OUT_PC, position) == 0x4);
STATIC_ASSERT(offsetof(BURN_OUT_PC, field_10) == 0x10);
STATIC_ASSERT(offsetof(BURN_OUT_PC, field_14) == 0x14);
STATIC_ASSERT(offsetof(BURN_OUT_PC, field_18) == 0x18);
STATIC_ASSERT(offsetof(BURN_OUT_PC, field_1c) == 0x1c);
STATIC_ASSERT(offsetof(BURN_OUT_PC, field_20) == 0x20);
STATIC_ASSERT(offsetof(BURN_SET_PC, parameters) == 0x0);
STATIC_ASSERT(offsetof(BURN_SET_PC, parameters_copy) == 0x54);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_a8) == 0xa8);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_ac) == 0xac);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_b0) == 0xb0);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_b4) == 0xb4);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_b8) == 0xb8);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_bc) == 0xbc);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_c0) == 0xc0);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_c4) == 0xc4);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_c8) == 0xc8);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_cc) == 0xcc);
STATIC_ASSERT(offsetof(BURN_SET_PC, burnouts) == 0xd0);
STATIC_ASSERT(offsetof(BURN_SET_PC, active_count) == 0x550);
STATIC_ASSERT(offsetof(BURN_SET_PC, selected_index) == 0x554);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_558) == 0x558);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_55c) == 0x55c);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_560) == 0x560);

typedef struct LIGHTING_LOAD_WORLD_PC {
    u8 unknown_000[0x104];
    VARIPTR buffer, end_buffer;
    u8 unknown_10c[0x2aac - 0x10c];
    BURN_SET_PC *burnset;
    u8 unknown_2ab0[4];
    RTL_SET_PC *rtl_set;
} LIGHTING_LOAD_WORLD_PC;
STATIC_ASSERT(offsetof(LIGHTING_LOAD_WORLD_PC, buffer) == 0x104);
STATIC_ASSERT(offsetof(LIGHTING_LOAD_WORLD_PC, end_buffer) == 0x108);
STATIC_ASSERT(offsetof(LIGHTING_LOAD_WORLD_PC, burnset) == 0x2aac);
STATIC_ASSERT(offsetof(LIGHTING_LOAD_WORLD_PC, rtl_set) == 0x2ab4);
BURN_SET_PC *__cdecl edrtlBurnoutLoad(char *, VARIPTR *, i32);
void __cdecl LoadLights(void *world_ptr, char *path) {
    LIGHTING_LOAD_WORLD_PC *world = world_ptr;
    char filename[256];
    sprintf(filename, "%s.rtl", path);
    world->rtl_set = rtlLoadSet(filename, &world->buffer, world->end_buffer.addr);
    sprintf(filename, "%s.bur", path);
    world->burnset = edrtlBurnoutLoad(filename, &world->buffer, world->end_buffer.addr);
}
