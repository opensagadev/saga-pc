#include "saga.h"
#include <string.h>

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

STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, specular_direction) == 0x54);
STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, specular_direction_missing) == 0x60);
STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, unknown_64) == 0x64);
STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, specular_intensity_present) == 0x74);
STATIC_ASSERT(offsetof(NUCAMERA, mtx) == 0);

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

/* Native cut resource access prefix; sole resource owner remains the cut loader. */
typedef struct BURNOUT_CUT_PREFIX_PC {
    u8 unknown_00[0x50];
    u32 flags;
} BURNOUT_CUT_PREFIX_PC;
STATIC_ASSERT(offsetof(BURNOUT_CUT_PREFIX_PC, flags) == 0x50);
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
