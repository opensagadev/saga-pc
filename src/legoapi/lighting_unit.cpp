#include "saga.h"
#include <string.h>
#include <stdio.h>

/* Retail unit 0x595110-0x5970b0 (legoapi/lighting.c): definitions in retail address order. */

typedef struct startup_object_light_pc_s {
    u8 unknown_000[0x1fc];
    u32 flags;
    u8 unknown_200[0xc04];
    i32 dynamic_light_id;
    u8 unknown_e08[0x2d0];
} STARTUP_OBJECT_LIGHT_PC;
extern "C" {
    i32 __cdecl rtlDynamicAlloc(void);
    i32 __cdecl rtlDynamicSetType(i32 id, i32 type);
}
typedef struct OBJECT_LIGHT_STATE_GAME_PC {
    NUVEC ambient;
    NUVEC intensity[3];
    NUVEC direction[3];
} OBJECT_LIGHT_STATE_GAME_PC;
typedef struct GAME_LIGHT_VIEW_PC {
    u8 unknown000[0x285];
    i8 floor_surface;
    u8 unknown286[0x4d8 - 0x286];
    RTL_DATA_PC light_data;
    OBJECT_LIGHT_STATE_GAME_PC lighting_state;
} GAME_LIGHT_VIEW_PC;
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
typedef struct LIGHTING_LOAD_WORLD_PC {
    u8 unknown_000[0x104];
    VARIPTR buffer, end_buffer;
    u8 unknown_10c[0x2aac - 0x10c];
    BURN_SET_PC *burnset;
    u8 unknown_2ab0[4];
    RTL_SET_PC *rtl_set;
} LIGHTING_LOAD_WORLD_PC;
extern "C" {
    BURN_SET_PC *__cdecl edrtlBurnoutLoad(char *, VARIPTR *, i32);
    void __cdecl NuMtxSetOrthoBlend(NUMTX *, f32, f32, f32, f32, f32, f32);
    extern NUVEC APIObjectZeroMomentumPC;
    extern f32 ShadowLightSplitScalePc, ShadowLightWarpPc, ShadowLightNearOffsetPc;
    extern f32 ShadowLightBlendPowerPc, ShadowLightFallbackHeightPc, ShadowLightEyeDistancePc;
    extern f32 ShadowLightFocusHeightPc;
    extern NUVEC ShadowLightUpPc;
    extern i32 ShadowLightSplitCountPc;
}
struct SHADOW_PLAYER_BOUNDS_PC {
    u8 unknown_000[0x5fc];
    NUVEC center;
    f32 radius;
};
extern "C" NUCAMERA *__cdecl edmainGetCameraPc(void);
extern "C" void __cdecl NuLightSpotFadeSet(u32 fade);

extern "C" {

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
}

void PC_SPECIAL_SHADOW_LIGHT::LookAtPc(NUVEC *eye, NUVEC *target, NUVEC *up) {
    f32 direction_y = target->y - eye->y;
    f32 direction_z = target->z - eye->z;
    shadow_direction_pc.x = target->x - eye->x;
    shadow_direction_pc.y = direction_y;
    shadow_direction_pc.z = direction_z;
    light_direction_w_pc = 0.0f;
    NuVecNorm(&shadow_direction_pc, &shadow_direction_pc);
    light_direction_w_pc = 1.0f;
    f32 position_y = eye->y;
    f32 position_z = eye->z;
    light_position_pc.x = eye->x;
    light_position_pc.y = position_y;
    light_position_pc.z = position_z;
    light_position_pc.w = 1.0f;
    NuMtxLookAtD3D(&view, eye, target, up);
}

void PC_SPECIAL_SHADOW_LIGHT::SetParameterfPc(i32 parameter, f32 value) {
    switch (parameter) {
        case 0:
            parameter_0 = value;
            break;
        case 1:
            parameter_1 = value;
            break;
        case 2:
            parameter_2 = value;
            break;
        case 3:
            parameter_3 = value;
            break;
        case 6:
        case 7:
            render_sets[0].parameter_110 = value;
            break;
        case 8:
            render_sets[1].parameter_110 = value;
            break;
        case 9:
        case 10:
        case 11:
            render_sets[0].parameter_104 = value;
            break;
        case 15:
        case 16:
            render_sets[1].parameter_104 = value;
            break;
        case 12:
        case 13:
        case 14:
            render_sets[0].parameter_100 = value;
            break;
        case 17:
        case 18:
            render_sets[1].parameter_100 = value;
            break;
        case 19:
            parameter_19 = value;
            break;
        case 20:
            parameter_20 = value;
            break;
    }
}
extern "C" {

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
        if (scale != 1.0f) {
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
        } else {
            selected_colours = PanelLightingColoursPc;
            selected_ambient = &PanelLightingAmbientPc;
        }
        ZeroLightSetupPrivatePc(&PanelLightingDirectionsPc[0], &selected_colours[0], &PanelLightingDirectionsPc[1],
                                &selected_colours[1], &PanelLightingDirectionsPc[2], &selected_colours[2]);
        PCGlobalLightingState.ambient = *selected_ambient;
        NuRndrSetAmbientLightPS(selected_ambient);
    }

    void __cdecl ResetLights(NUVEC *position, RTL_DATA_PC *packet, void *set) {
        rtlResetEx(packet, 1);
        if (position != NULL)
            rtlApplySetScale(set, packet, position, NULL, -1, 1.0f);
    }

    char *__cdecl rtlGetEnvPath(void) {
        return "";
    }

    char *__cdecl rtlGetEnvSceneName(void) {
        VARIPTR address;
        address.void_ptr = WORLD;
        address.addr += offsetof(WORLDINFO, config_file);
        return address.char_ptr;
    }

    RTL_SET_PC *__cdecl rtlGetEnvSet(void) {
        return (RTL_SET_PC *)WORLD->rtl_set;
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

    void __cdecl LoadLights(void *world_ptr, char *path) {
        LIGHTING_LOAD_WORLD_PC *world = (LIGHTING_LOAD_WORLD_PC *)world_ptr;
        char filename[256];
        sprintf(filename, "%s.rtl", path);
        world->rtl_set = rtlLoadSet(filename, &world->buffer, world->end_buffer.addr);
        sprintf(filename, "%s.bur", path);
        world->burnset = edrtlBurnoutLoad(filename, &world->buffer, world->end_buffer.addr);
    }

    void __cdecl InitGameObjectLights(void) {
        STARTUP_OBJECT_LIGHT_PC *objects;
        i32 index;
        for (index = 0; index < 64; ++index)
            ((STARTUP_OBJECT_LIGHT_PC *)Obj)[index].dynamic_light_id = -1;
        objects = (STARTUP_OBJECT_LIGHT_PC *)Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index) {
            if ((objects[index].flags & 1) && (objects[index].flags & 0x1000)) {
                objects[index].dynamic_light_id = rtlDynamicAlloc();
                if (objects[index].dynamic_light_id != -1) {
                    rtlDynamicSetType(objects[index].dynamic_light_id, 2);
                    rtlDynamicEnable(objects[index].dynamic_light_id, 0);
                }
            }
        }
    }

    void __cdecl FreeGameObjectLights(void) {
        STARTUP_OBJECT_LIGHT_PC *objects = (STARTUP_OBJECT_LIGHT_PC *)Obj;
        i32 i;
        for (i = 0; i < HIGHGAMEOBJECT; ++i) {
            if ((objects[i].flags & 1) && (objects[i].flags & 0x1000) && objects[i].dynamic_light_id != -1) {
                rtlDynamicFree(objects[i].dynamic_light_id);
                objects[i].dynamic_light_id = -1;
            }
        }
    }

    void __cdecl LightGameObject(GAMEOBJECT *object, void *set) {
        GAME_LIGHT_VIEW_PC *view = (GAME_LIGHT_VIEW_PC *)object;
        f32 surface_fade;
        i32 index;
        rtlResetEx(&view->light_data, 1);
        rtlApplySetScale(set, &view->light_data, &object->collision_position, NULL, -1, 1.0f);
        surface_fade =
            object->floor_height != 2000000.0f && (TerSurface[view->floor_surface].flags & 8) != 0 ? 1.0f : 0.0f;
        if (object->flags_ef0 & 0x80)
            object->surface_flag8_scale_pc = surface_fade;
        else
            object->surface_flag8_scale_pc =
                SeekLinearF(object->surface_flag8_scale_pc, surface_fade, (f32)((f64)FRAMETIME * 3.0));
        if (object->flags_ef0 & 0x80)
            view->lighting_state.ambient = view->light_data.ambient;
        else {
            view->lighting_state.ambient.x = SeekValF(view->lighting_state.ambient.x, view->light_data.ambient.x, 5.0f);
            view->lighting_state.ambient.y = SeekValF(view->lighting_state.ambient.y, view->light_data.ambient.y, 5.0f);
            view->lighting_state.ambient.z = SeekValF(view->lighting_state.ambient.z, view->light_data.ambient.z, 5.0f);
        }
        for (index = 0; index < 3; ++index) {
            if (object->flags_ef0 & 0x80) {
                view->lighting_state.intensity[index] = view->light_data.intensity_vectors[index];
                view->lighting_state.direction[index] = view->light_data.direction[index];
            } else {
                view->lighting_state.intensity[index].x = SeekValF(view->lighting_state.intensity[index].x,
                                                                   view->light_data.intensity_vectors[index].x, 5.0f);
                view->lighting_state.intensity[index].y = SeekValF(view->lighting_state.intensity[index].y,
                                                                   view->light_data.intensity_vectors[index].y, 5.0f);
                view->lighting_state.intensity[index].z = SeekValF(view->lighting_state.intensity[index].z,
                                                                   view->light_data.intensity_vectors[index].z, 5.0f);
                view->lighting_state.direction[index].x =
                    SeekValF(view->lighting_state.direction[index].x, view->light_data.direction[index].x, 5.0f);
                view->lighting_state.direction[index].y =
                    SeekValF(view->lighting_state.direction[index].y, view->light_data.direction[index].y, 5.0f);
                view->lighting_state.direction[index].z =
                    SeekValF(view->lighting_state.direction[index].z, view->light_data.direction[index].z, 5.0f);
            }
            if (view->lighting_state.direction[index].x != 0.0f || view->lighting_state.direction[index].y != 0.0f ||
                view->lighting_state.direction[index].z != 0.0f)
                NuVecNorm(&view->lighting_state.direction[index], &view->lighting_state.direction[index]);
            else
                view->lighting_state.direction[index] = view->light_data.direction[index];
        }
        object->flags_ef0 &= ~0x80u;
    }

    void __cdecl SetCreatureLights(APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        GAME_LIGHT_VIEW_PC *view = (GAME_LIGHT_VIEW_PC *)owner;
        NUVEC ambient, intensity[3], direction[3];
        PLAYERCHARACTERCONFIG_PC *character;
        f32 red, green, blue;
        i32 index;
        if (Cheats_CheckFlags(1) != 0) {
            SetZeroLights();
            return;
        }
        if (owner->dead != 0) {
            SetLights(&view->light_data.intensity_vectors[0], &view->light_data.direction[0],
                      &view->light_data.intensity_vectors[1], &view->light_data.direction[1],
                      &view->light_data.intensity_vectors[2], &view->light_data.direction[2],
                      &view->light_data.ambient);
            return;
        }
        red = green = blue = 1.0f;
        ambient = view->lighting_state.ambient;
        intensity[0] = view->lighting_state.intensity[0];
        direction[0] = view->lighting_state.direction[0];
        intensity[1] = view->lighting_state.intensity[1];
        direction[1] = view->lighting_state.direction[1];
        intensity[2] = view->lighting_state.intensity[2];
        direction[2] = view->lighting_state.direction[2];
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
        } else if ((owner->character_data->game_character->flags_090 & 0x8000) && !(owner->flags_1fc_word & 0x80)) {
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
            red = green = blue =
                (f32)(NuTrigTable[((u32)(angle >> 1)) & 0x7fff] * ((f64)owner->arrow_blend * 0.5) + 1.0);
        } else if (Lighting_FlashRedOnLastHeart != 0 && (owner->flags_1fc_word & 0x80) && owner->current_hp == 1 &&
                   (u8)owner->field_107e > 1 && owner->vehicle_hit_timer < -0.5f) {
            i32 angle = (i32)((1.0 - (-owner->vehicle_hit_timer)) * 2.0 * 65536.0 + 16384.0);
            f32 flash = (f32)((1.0 - NuTrigTable[((u32)(angle >> 1)) & 0x7fff]) * 0.5 * (f64)0.333f);
            red = (f32)(((f64)Lighting_RedFlashRedPc - 1.0) * flash + 1.0);
            green = (f32)(((f64)Lighting_RedFlashGreenPc - 1.0) * flash + 1.0);
            blue = (f32)(((f64)Lighting_RedFlashBluePc - 1.0) * flash + 1.0);
        } else if (owner->spawn_protection_timer > 0.0f && owner->field_280 != 0xff) {
            u16 angle = (u16)(i32)((f64)NuFmod((f32)(2.5 - owner->spawn_protection_timer), 0.5f) * 2.0 * 65536.0);
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
            if (red > 2.0f)
                red = 2.0f;
            green *= GhostLightMul;
            if (green > 2.0f)
                green = 2.0f;
            blue *= GhostLightMul;
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

    void __cdecl InitShadowLights(void) {
        struct PC_SPECIAL_SHADOW_LIGHT *light;
        i32 enabled;
        i32 i;

        PCShadowCurrentLight = NuDynamicLightCreate();
        NuDynamicLightSetParameteri(PCShadowCurrentLight, 5, 2);
        NuDynamicLightSetParameterf(PCShadowCurrentLight, 0, 0.85f);
        NuDynamicLightSetParameterf(PCShadowCurrentLight, 1, 3.5f);
        NuDynamicLightSetParameterf(PCShadowCurrentLight, 2, 0.4f);
        NuDynamicLightSetParameterf(PCShadowCurrentLight, 3, 0.2f);
        NuDynamicLightSetParameterf(PCShadowCurrentLight, 14, 0.0001f);
        NuDynamicLightSetParameterf(PCShadowCurrentLight, 11, 0.0001f);
        NuDynamicLightSetParameterf(PCShadowCurrentLight, 18, 0.0001f);
        NuDynamicLightSetParameterf(PCShadowCurrentLight, 16, 0.0001f);
        light = PCShadowCurrentLight;
        light->parameter_4 = 0;
        NuMtxSetOrthoBlend(&light->projection, -60.0f, 60.0f, -60.0f, 60.0f, 0.0f, 300.0f);

        PCShadowSecondaryLight = NuDynamicLightCreate();
        NuDynamicLightSetParameteri(PCShadowSecondaryLight, 5, 2);
        NuDynamicLightSetParameterf(PCShadowSecondaryLight, 0, 0.25f);
        NuDynamicLightSetParameterf(PCShadowSecondaryLight, 1, 3.5f);
        NuDynamicLightSetParameterf(PCShadowSecondaryLight, 2, 0.85f);
        NuDynamicLightSetParameterf(PCShadowSecondaryLight, 3, 0.2f);
        NuDynamicLightSetParameterf(PCShadowSecondaryLight, 14, 0.0001f);
        NuDynamicLightSetParameterf(PCShadowSecondaryLight, 11, 0.0001f);
        light = PCShadowSecondaryLight;
        light->parameter_4 = 0;
        NuMtxSetOrthoBlend(&light->projection, -60.0f, 60.0f, -60.0f, 60.0f, 0.0f, 300.0f);

        light = PCShadowSecondaryLight;
        enabled = 1;
        if (enabled != 0 && light->enabled == 0) {
            for (i = 0; i < light->render_set_count; ++i) {
                light->render_sets[i].shadow_lists_pc[0].material_last = light->render_sets[i].shadow_lists_pc[0].first;
                light->render_sets[i].shadow_lists_pc[1].material_last = light->render_sets[i].shadow_lists_pc[1].first;
            }
        }
        light->enabled = enabled;
    }
}

/* Native private EDI=center, stack=radius pointer. */
extern "C" i32 __cdecl ShadowAveragePlayerBoundsPc(NUVEC *center, f32 *radius) {
    *center = APIObjectZeroMomentumPC;
    *radius = -1.0f;
    i32 count = 0;
    for (i32 i = 0; i < 2; ++i) {
        if (Player[i] != NULL && (static_cast<GAMEOBJECT *>(Player[i])->flags_1fc & 0x80) != 0) {
            NuVecAdd(center, center, &reinterpret_cast<SHADOW_PLAYER_BOUNDS_PC *>(Player[i])->center);
            *radius = *radius > reinterpret_cast<SHADOW_PLAYER_BOUNDS_PC *>(Player[i])->radius
                          ? *radius
                          : reinterpret_cast<SHADOW_PLAYER_BOUNDS_PC *>(Player[i])->radius;
            ++count;
        }
    }
    if (count > 0)
        NuVecScale(center, center, 1.0f / count);
    return count;
}

extern "C" void __cdecl RenderShadowLights(i32 paused) {
    if (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod == 2 && PCShadowCurrentLight != NULL &&
        PCShadowCurrentLight->enabled != 0) {
        i32 previous = ShadowSetClipStatePc(1, 1);
        ShadowMapResetPc();
        ShadowMapEnablePc(PCShadowCurrentLight);
        ShadowResetClipResultsPc();
        NuRndrBeginScene(1);
        currentScene.dyn_light_count = 1;
        currentScene.dyn_light_ptr = PCShadowCurrentLight;
        currentScene.unknown_40 = 0;
        if (WORLD->current_gscn != NULL)
            PCShadowCurrentLight->RenderSceneShadowPc(WORLD->current_gscn);
        ShadowSetClipStatePc(0, previous);
        NuRndrEndScene();
        ShadowMapResetPc();
    }
}

void __cdecl EnableShadowMapRendering(i32 unused) {
    if (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod == 2)
        ShadowMapEnablePc(PCShadowCurrentLight);
}

void __cdecl ResetShadowMapRendering(void) {
    ShadowMapResetPc();
}

extern "C" void __cdecl SetSpotLightMode(void) {
    NuLightSpotFadeSet(0x80808080);
}

// Append to the current shadowmap.cpp owner; real existing definitions above.
void __cdecl ResetShadowMapRenderingFn(void) {
    ResetShadowMapRendering();
}

void __cdecl EnableShadowMapRenderingFn(void) {
    EnableShadowMapRendering(0);
}
extern "C" {
#pragma data_seg(".data$ShadowLightParametersPc")
    f32 ShadowLightSplitScalePc = 0.9f, ShadowLightWarpPc = 5.0f;
    NUVEC ShadowLightUpPc = {0.0f, 1.0f, 0.0f};
    f32 ShadowLightNearOffsetPc = 1.5f, ShadowLightBlendPowerPc = 3.0f, ShadowLightFallbackHeightPc = 5.0f;
    f32 ShadowLightEyeDistancePc = 20.0f, ShadowLightFocusHeightPc = -2.0f;
#pragma data_seg()
#pragma bss_seg(".bss$ShadowLightSplitCountPc")
    i32 ShadowLightSplitCountPc = 0;
#pragma bss_seg(".bss$EdMainCurrentCameraPc")
    NUCAMERA *EdMainCurrentCameraPc = 0;
#pragma bss_seg()
}

extern "C" void __cdecl SetShadowLight(void) {
    NUVEC center, target, eye, secondary_eye, direction, camera_position;
    __declspec(align(16)) NUVEC4 view_direction;
    NUCAMERA *camera;
    f32 secondary_strength;
    if (CutStopInfo != NULL) {
        NUCAMERA *current = NuCameraGetCam();
        center.x = (f64)current->mtx.m20 * 2.0 + current->mtx.m30;
        center.y = (f64)current->mtx.m21 * 2.0 + current->mtx.m31;
        center.z = (f64)current->mtx.m22 * 2.0 + current->mtx.m32;
        ShadowLightFocusHeightPc = -2.0f;
    } else {
        if (Players_AveragePos(&center, NULL) == 0)
            return;
        ShadowLightFocusHeightPc = WORLD->shadow_editor->field_30;
    }
    if (WORLD->shadow_editor == NULL)
        return;
    target.x = center.x;
    target.y = ShadowLightFocusHeightPc + center.y;
    target.z = center.z;
    i32 alternate =
        WORLD->area != NULL && (WORLD->area->flags & 0x100) != 0 && (WORLD->current_level->flags & 0x400) == 0;
    if (CutStopInfo != NULL) {
        direction = WORLD->shadow_editor->direction;
        secondary_strength = WORLD->shadow_editor->field_0c;
        eye = center;
        eye.x -= ShadowLightEyeDistancePc * direction.x;
        eye.y -= ShadowLightEyeDistancePc * direction.y;
        eye.z -= ShadowLightEyeDistancePc * direction.z;
        secondary_eye = center;
        secondary_eye.x += (f64)ShadowLightEyeDistancePc * 2.0;
        secondary_eye.y += (f64)ShadowLightEyeDistancePc * 10.0;
        secondary_eye.z += (f64)ShadowLightEyeDistancePc * 2.0;
    } else if (ShadowAveragePlayerBoundsPc(&direction, &secondary_strength) != 0 && secondary_strength > 0.0f) {
        NuVecAdd(&direction, &direction, &WORLD->shadow_editor->direction);
        NuVecNorm(&direction, &direction);
        eye = center;
        eye.x -= ShadowLightEyeDistancePc * direction.x;
        eye.y -= ShadowLightEyeDistancePc * direction.y;
        eye.z -= ShadowLightEyeDistancePc * direction.z;
        secondary_eye = center;
        secondary_eye.x += (ShadowLightEyeDistancePc + ShadowLightEyeDistancePc);
        secondary_eye.y += (f64)ShadowLightEyeDistancePc * 10.0;
        secondary_eye.z += (ShadowLightEyeDistancePc + ShadowLightEyeDistancePc);
    } else {
        eye = GameCam->position;
        eye.x = (f64)eye.x - 1.2000000476837158;
        eye.y = ShadowLightFallbackHeightPc;
        eye.z = (f64)eye.z - 1.5;
        secondary_eye = eye;
    }
    if (alternate)
        secondary_strength = 0.5f;
    if (CutStopInfo != NULL) {
        f32 forward_y = GameCam->render_mtx.m21;
        f32 forward_z = GameCam->render_mtx.m22;
        view_direction.x = GameCam->render_mtx.m20;
        view_direction.y = forward_y;
        view_direction.z = forward_z;
        view_direction.w = 1.0f;
        camera_position = *(NUVEC *)&GameCam->render_mtx.m30;
        camera = pNuCam;
    } else if (editor_active != 0) {
        NUCAMERA *current = NuCameraGetCam();
        f32 forward_y = current->mtx.m21;
        f32 forward_z = current->mtx.m22;
        view_direction.x = current->mtx.m20;
        view_direction.y = forward_y;
        view_direction.z = forward_z;
        view_direction.w = 1.0f;
        camera_position = *(NUVEC *)&NuCameraGetCam()->mtx.m30;
        camera = edmainGetCameraPc();
    } else {
        NuVecSub((NUVEC *)&view_direction, &GameCam->target, &GameCam->position);
        camera_position = GameCam->position;
        camera = pNuCam;
    }
    NuVecNorm((NUVEC *)&view_direction, (NUVEC *)&view_direction);
    view_direction.w = 1.0f;
    f64 weight = 1.0 < 1.0 + (f64)view_direction.y ? 1.0 : 1.0 + (f64)view_direction.y;
    f32 distance = NuVecDist(&camera_position, &target, NULL);
    f32 range = WORLD->shadow_editor->field_20;
    f32 weight32 = weight;
    f32 interpolation = NuPow(weight32, ShadowLightBlendPowerPc);
    if (alternate)
        range = 16.0f;
    f32 splits[3];
    i32 split_count;
    if (CutStopInfo != NULL) {
        splits[0] = pNuCam->near_clip;
        split_count = 3;
        splits[1] = pNuCam->near_clip + ShadowLightNearOffsetPc;
        ShadowLightSplitCountPc = split_count;
        splits[2] = distance + (range - distance) * interpolation;
    } else {
        splits[0] = pNuCam->near_clip;
        split_count = 2;
        ShadowLightSplitCountPc = 1;
        splits[1] = distance + (range - distance) * interpolation;
    }
    f32 parameter = WORLD->shadow_editor->field_0c;
    if (alternate)
        parameter = secondary_strength;
    PCShadowCurrentLight->parameter_0 = parameter;
    parameter = WORLD->shadow_editor->field_10;
    PCShadowCurrentLight->parameter_1 = parameter;
    parameter = WORLD->shadow_editor->field_14;
    PCShadowCurrentLight->parameter_2 = parameter;
    parameter = WORLD->shadow_editor->field_1c;
    PCShadowCurrentLight->render_sets[0].parameter_100 = parameter;
    parameter = WORLD->shadow_editor->field_18;
    PCShadowCurrentLight->render_sets[0].parameter_104 = parameter;
    PCShadowCurrentLight->render_sets[0].parameter_110 = ShadowLightWarpPc;
    parameter = WORLD->shadow_editor->field_1c;
    PCShadowCurrentLight->render_sets[1].parameter_100 = parameter;
    parameter = WORLD->shadow_editor->field_18;
    PCShadowCurrentLight->render_sets[1].parameter_104 = parameter;
    PCShadowCurrentLight->render_sets[1].parameter_110 = ShadowLightWarpPc;
    parameter = splits[split_count - 1] * ShadowLightSplitScalePc;
    PCShadowCurrentLight->parameter_19 = parameter;
    parameter = splits[split_count - 1];
    PCShadowCurrentLight->parameter_20 = parameter;
    PCShadowCurrentLight->LookAtPc(&eye, &center, &ShadowLightUpPc);
    PCShadowCurrentLight->SetupCustomCameraFrustumPc(camera, splits, split_count);
    PCShadowSecondaryLight->parameter_0 = secondary_strength;
    PCShadowSecondaryLight->LookAtPc(&secondary_eye, &center, &ShadowLightUpPc);
    PCShadowSecondaryLight->SetupCustomCameraFrustumPc(camera, splits, split_count);
}
