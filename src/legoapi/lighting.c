#include "saga.h"
#include <string.h>

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

void __cdecl ConfigureComplexShadow(GAMEOBJECT *object) {
    NUVEC direction;
    f32 distance;
    if (object != NULL) {
        direction.x = 65536.0f;
        direction.y = object->collision_position.y;
        direction.z = (f32)((f64)object->floor_height - (f64)0.1f);
    } else {
        direction.x = 0.0f;
        direction.y = (f32)(-(f64)ComplexShadowBasePc * 0.25);
        direction.z = 0.0f;
    }
    distance = VehicleArea != 0 ? 150.0f : 30.0f;
    NuRndrShadowDirCol(&direction, 0x17000000u, distance, distance);
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

STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, specular_direction) == 0x54);
STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, specular_direction_missing) == 0x60);
STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, unknown_64) == 0x64);
STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, specular_intensity_present) == 0x74);
STATIC_ASSERT(offsetof(NUCAMERA, mtx) == 0);
void __cdecl SetLights(NUVEC *colour0, NUVEC *direction0, NUVEC *colour1, NUVEC *direction1, NUVEC *colour2,
                       NUVEC *direction2, NUVEC *ambient) {
    ZeroLightSetupPrivatePc(direction0, colour0, direction1, colour1, direction2, colour2);
    PCGlobalLightingState.ambient = *ambient;
    NuRndrSetAmbientLightPS(ambient);
}
void __cdecl SetLights_RTLDATA(RTL_DATA_PC *data, f32 scale) {
    RTL_DATA_PC scaled;
    if (scale == 1.0f) {
        rtlSetLights(data);
        return;
    }
    scaled = *data;
    scaled.intensity_vectors[0].x *= scale;
    scaled.intensity_vectors[0].y *= scale;
    scaled.intensity_vectors[0].z *= scale;
    scaled.intensity_vectors[1].x *= scale;
    scaled.intensity_vectors[1].y *= scale;
    scaled.intensity_vectors[1].z *= scale;
    scaled.intensity_vectors[2].x *= scale;
    scaled.intensity_vectors[2].y *= scale;
    scaled.intensity_vectors[2].z *= scale;
    scaled.ambient.x *= scale;
    scaled.ambient.y *= scale;
    scaled.ambient.z *= scale;
    rtlSetLights(&scaled);
}
void __cdecl SetLevelLights(void *set, f32 scale) {
    f32 old_specular = rtlSpecularValue(&LevelLightingDataPc);
    NUVEC4 ambient;
    rtlResetEx(&LevelLightingDataPc, 1);
    rtlApplySetScale(set, &LevelLightingDataPc, (NUVEC *)&global_camera.mtx.m30, NULL, 0x10, 1.0f);
    if (rtlSpecularValue(&LevelLightingDataPc) == 0.0f)
        rtlSetSpecularValue(&LevelLightingDataPc, old_specular);
    SetLights_RTLDATA(&LevelLightingDataPc, scale);
    rtlSetSpecularLight(&LevelLightingDataPc);
    ambient.x = LevelLightingDataPc.ambient.x;
    ambient.y = LevelLightingDataPc.ambient.y;
    ambient.z = LevelLightingDataPc.ambient.z;
    ambient.w = 128.0f;
    NuRndrSetAmbientLightSpecular(&ambient);
}
void __cdecl SetPanelLights(f32 intensity) {
    f32 scale = (f32)((f64)intensity * 0.5);
    NUVEC colours[3], ambient;
    NUVEC *selected_colours, *selected_ambient;
    if (scale == 1.0f) {
        selected_colours = PanelLightingColoursPc;
        selected_ambient = &PanelLightingAmbientPc;
    } else {
        colours[0].x = PanelLightingColoursPc[0].x * scale;
        colours[0].y = PanelLightingColoursPc[0].y * scale;
        colours[0].z = PanelLightingColoursPc[0].z * scale;
        colours[1].x = PanelLightingColoursPc[1].x * scale;
        colours[1].y = PanelLightingColoursPc[1].y * scale;
        colours[1].z = PanelLightingColoursPc[1].z * scale;
        colours[2].x = PanelLightingColoursPc[2].x * scale;
        colours[2].y = PanelLightingColoursPc[2].y * scale;
        colours[2].z = PanelLightingColoursPc[2].z * scale;
        ambient.x = PanelLightingAmbientPc.x * scale;
        ambient.y = PanelLightingAmbientPc.y * scale;
        ambient.z = PanelLightingAmbientPc.z * scale;
        selected_colours = colours;
        selected_ambient = &ambient;
    }
    ZeroLightSetupPrivatePc(&PanelLightingDirectionsPc[0], &selected_colours[0], &PanelLightingDirectionsPc[1],
                            &selected_colours[1], &PanelLightingDirectionsPc[2], &selected_colours[2]);
    PCGlobalLightingState.ambient = *selected_ambient;
    NuRndrSetAmbientLightPS(selected_ambient);
}
void __cdecl FreeGameObjectLights(void) {
    STARTUP_OBJECT_LIGHT_PC *objects = (STARTUP_OBJECT_LIGHT_PC *)Obj;
    i32 i;
    for (i = 0; i < HIGHGAMEOBJECT; ++i) {
        if ((objects[i].flags & 0x1001) == 0x1001 && objects[i].dynamic_light_id != -1) {
            rtlDynamicFree(objects[i].dynamic_light_id);
            objects[i].dynamic_light_id = -1;
        }
    }
}

STATIC_ASSERT(offsetof(APIOBJECT_PC, owner) == 0);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, hud_icon_timer) == 0xd60);
STATIC_ASSERT(offsetof(GAMEOBJECT, surface_flag8_scale_pc) == 0xd6c);
STATIC_ASSERT(offsetof(GAMEOBJECT, arrow_blend) == 0xdd4);
STATIC_ASSERT(offsetof(GAMEOBJECT, spawn_protection_timer) == 0x1000);
STATIC_ASSERT(offsetof(GAMEOBJECT, vehicle_hit_timer) == 0x1018);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107e) == 0x107e);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_094_bytes) == 0x94);
void __cdecl SetCreatureLights(APIOBJECT_PC *object) {
    GAMEOBJECT *owner = object->owner;
    GAME_LIGHT_VIEW_PC *view = (GAME_LIGHT_VIEW_PC *)owner;
    NUVEC ambient, intensity[3], direction[3];
    PLAYERCHARACTERCONFIG_PC *character;
    f32 red = 1.0f, green = 1.0f, blue = 1.0f;
    i32 index;
    if (Cheats_CheckFlags(1) != 0) {
        SetZeroLights();
        return;
    }
    if (owner->dead != 0) {
        SetLights(&view->light_data.intensity_vectors[0], &view->light_data.direction[0],
                  &view->light_data.intensity_vectors[1], &view->light_data.direction[1],
                  &view->light_data.intensity_vectors[2], &view->light_data.direction[2], &view->light_data.ambient);
        return;
    }
    ambient = view->lighting_state.ambient;
    intensity[0] = view->lighting_state.intensity[0];
    intensity[1] = view->lighting_state.intensity[1];
    intensity[2] = view->lighting_state.intensity[2];
    direction[0] = view->lighting_state.direction[0];
    direction[1] = view->lighting_state.direction[1];
    direction[2] = view->lighting_state.direction[2];
    character = owner->character_data->game_character;
    if (owner->vehicle_hit_timer > 0.0f) {
        f32 flash = (f32)(owner->vehicle_hit_timer / (f64)0.4f);
        red = (f32)(((f64)Lighting_RedFlashRedPc - 1.0) * flash + 1.0);
        green = (f32)(((f64)Lighting_RedFlashGreenPc - 1.0) * flash + 1.0);
        blue = (f32)(((f64)Lighting_RedFlashBluePc - 1.0) * flash + 1.0);
    } else if (Lighting_HighlightFlash != 0 && (owner->flags_1fc_word & 0x80) && owner->hud_icon_timer > 0.0f &&
               (owner->hud_icon_timer >= 2.0f || (f64)NuFmod(owner->hud_icon_timer, 0.4f) >= (f64)0.2f)) {
        if (owner->field_280 == 1) {
            red = 1.7f;
            green = 2.0f;
            blue = 1.4f;
        } else {
            red = 1.4f;
            green = 1.85f;
            blue = 2.0f;
        }
    } else if ((character->flags_090 & 0x8000) && !(owner->flags_1fc_word & 0x80)) {
        red = 1.4f;
        green = 1.85f;
        blue = 2.0f;
    } else if (Lighting_BlueFlickerFn != NULL && Lighting_BlueFlickerFn(owner) != 0) {
        f32 flicker = qrand() < 0x8000 ? 1.0f : 0.25f;
        character = owner->character_data->game_character;
        if (character->flags_094_bytes[2] & 1) {
            red = green = blue = flicker + flicker;
        } else {
            red = Lighting_BlueFlickerRedPc * flicker;
            green = Lighting_BlueFlickerGreenPc * flicker;
            blue = Lighting_BlueFlickerBluePc * flicker;
        }
    } else if (owner->arrow_blend > 0.0f) {
        i32 angle = (i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0);
        red = green = blue = (f32)(NuTrigTable[((u32)(angle >> 1)) & 0x7fff] * ((f64)owner->arrow_blend * 0.5) + 1.0);
    } else if (Lighting_FlashRedOnLastHeart != 0 && (owner->flags_1fc_word & 0x80) && owner->current_hp == 1 &&
               (u8)owner->field_107e > 1 && owner->vehicle_hit_timer < -0.5f) {
        i32 angle = (i32)((1.0 + owner->vehicle_hit_timer) * 2.0 * 65536.0 + 16384.0);
        f32 flash = (f32)((1.0 - NuTrigTable[((u32)(angle >> 1)) & 0x7fff]) * 0.5 * (f64)0.333f);
        red = (f32)(((f64)Lighting_RedFlashRedPc - 1.0) * flash + 1.0);
        green = (f32)(((f64)Lighting_RedFlashGreenPc - 1.0) * flash + 1.0);
        blue = (f32)(((f64)Lighting_RedFlashBluePc - 1.0) * flash + 1.0);
    } else if (owner->spawn_protection_timer > 0.0f && owner->field_280 != 0xff) {
        f32 elapsed = (f32)(2.5 - owner->spawn_protection_timer);
        u16 angle = (u16)(i32)((f64)NuFmod(elapsed, 0.5f) * 2.0 * 65536.0);
        red = green = blue = NuTrigTable[angle >> 1] + 1.0f;
    }
    if (owner->surface_flag8_scale_pc > 0.0f) {
        f32 scale = (f32)((f64)owner->surface_flag8_scale_pc * LightingSurfaceFadeSlopePc + 1.0);
        red *= scale;
        green *= scale;
        blue *= scale;
    }
    character = owner->character_data->game_character;
    if (character->flags_090 & 0x8000) {
        red *= GhostLightMul;
        green *= GhostLightMul;
        blue *= GhostLightMul;
        if (red > 2.0f)
            red = 2.0f;
        if (green > 2.0f)
            green = 2.0f;
        if (blue > 2.0f)
            blue = 2.0f;
    }
    if (red != 1.0f) {
        ambient.x *= red;
        for (index = 0; index < 3; ++index)
            intensity[index].x *= red;
    }
    if (green != 1.0f) {
        ambient.y *= green;
        for (index = 0; index < 3; ++index)
            intensity[index].y *= green;
    }
    if (blue != 1.0f) {
        ambient.z *= blue;
        for (index = 0; index < 3; ++index)
            intensity[index].z *= blue;
    }
    SetLights(&intensity[0], &direction[0], &intensity[1], &direction[1], &intensity[2], &direction[2], &ambient);
}

/* Native cut resource access prefix; sole resource owner remains the cut loader. */
typedef struct BURNOUT_CUT_PREFIX_PC {
    u8 unknown_00[0x50];
    u32 flags;
} BURNOUT_CUT_PREFIX_PC;
STATIC_ASSERT(offsetof(BURNOUT_CUT_PREFIX_PC, flags) == 0x50);
void __cdecl NuRndrSetBurnoutParametersPc(i32 view, const BURN_PARAMETERS_PC *parameters) {
    currentScene.burnout_parameters_pc = *parameters;
}
void __cdecl NuRndrSetShadowCoefficientPc(i32 index, f32 value) {
    switch (index) {
        case 0:
            currentScene.shadow_coefficients_pc[0] = value;
            break;
        case 1:
            currentScene.shadow_coefficients_pc[1] = value;
            break;
        case 2:
            currentScene.shadow_coefficients_pc[2] = value;
            break;
        default:
            AnimationDiagnosticNoOpPc();
            break;
    }
}
void __cdecl NuRndrEnableDeferredPc(void) {
    currentScene.unknown_48 = 1;
}
void __cdecl BurnoutApply(i32 view) {
    BURN_PARAMETERS_PC parameters;
    NUVEC average;
    parameters.field_00 = 0;
    memset(&parameters.field_04, 0, 0x50);
    if ((CutStopInfo == NULL || (((BURNOUT_CUT_PREFIX_PC *)CutStopInfo)->flags & 0x80) == 0) &&
        WORLD->page_unknown != 0)
        edrtlCalculateBurnoutEx((BURN_SET_PC *)WORLD->page_unknown, &parameters, (NUVEC *)&global_camera.mtx.m30,
                                FRAMETIME);
    if (parameters.field_14 > 0.0f)
        NuRndrSetBurnoutParametersPc(view, &parameters);
    if ((Players_AveragePos(&average, NULL) != 0 || netclient != 0) && PCShadowCurrentLight->enabled != 0) {
        NuRndrSetShadowCoefficientPc(0, WORLD->shadow_editor->field_24);
        NuRndrSetShadowCoefficientPc(1, WORLD->shadow_editor->field_28);
        NuRndrSetShadowCoefficientPc(2, WORLD->shadow_editor->field_2c);
        NuRndrEnableDeferredPc();
    }
}

STATIC_ASSERT(offsetof(WORLDINFO, config_file) == 0x80);

char *__cdecl rtlGetEnvPath(void) {
    return "";
}

char *__cdecl rtlGetEnvSceneName(void) {
    VARIPTR address;
    address.void_ptr = WORLD;
    address.addr += offsetof(WORLDINFO, config_file);
    return address.char_ptr;
}
