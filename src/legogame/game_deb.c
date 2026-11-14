#include "saga.h"
#include <string.h>
#include <stdio.h>

/* Retail unit 0x42f4a0-0x430117: definitions in retail address order. */

typedef struct startup_object_light_pc_s {
    u8 unknown_000[0x1fc]; // 0x0000
    u32 flags;             // 0x01fc
    u8 unknown_200[0xc04]; // 0x0200
    i32 dynamic_light_id;  // 0x0e04
    u8 unknown_e08[0x2d0]; // 0x0e08
} STARTUP_OBJECT_LIGHT_PC; // size 0x10d8

STATIC_ASSERT(sizeof(STARTUP_OBJECT_LIGHT_PC) == sizeof(GAMEOBJECT));
STATIC_ASSERT(offsetof(NULNKHDR, id) == 0xc);

typedef struct OBJECT_LIGHT_STATE_GAME_PC {
    NUVEC ambient;            // 0x00
    NUVEC intensity[3];       // 0x0c
    NUVEC direction[3];       // 0x30
} OBJECT_LIGHT_STATE_GAME_PC; // size 0x54

typedef struct GAME_LIGHT_VIEW_PC {
    u8 unknown000[0x285];                      // 0x000
    i8 floor_surface;                          // 0x285
    u8 unknown286[0x4d8 - 0x286];              // 0x286
    RTL_DATA_PC light_data;                    // 0x4d8
    OBJECT_LIGHT_STATE_GAME_PC lighting_state; // 0x61c
} GAME_LIGHT_VIEW_PC;                          // size 0x670

typedef struct BURN_OUT_PC {
    i32 active;     // 0x00
    NUVEC position; // 0x04
    f32 field_10;   // 0x10
    f32 field_14;   // 0x14
    f32 field_18;   // 0x18
    f32 field_1c;   // 0x1c
    f32 field_20;   // 0x20
} BURN_OUT_PC;      // size 0x24

typedef struct BURN_SET_PC {
    BURN_PARAMETERS_PC parameters;      // 0x000
    BURN_PARAMETERS_PC parameters_copy; // 0x054
    i32 field_a8;                       // 0x0a8
    i32 field_ac;                       // 0x0ac
    i32 field_b0;                       // 0x0b0
    i32 field_b4;                       // 0x0b4
    f32 field_b8;                       // 0x0b8
    f32 field_bc;                       // 0x0bc
    f32 field_c0;                       // 0x0c0
    f32 field_c4;                       // 0x0c4
    f32 field_c8;                       // 0x0c8
    f32 field_cc;                       // 0x0cc
    BURN_OUT_PC burnouts[32];           // 0x0d0
    i32 active_count;                   // 0x550
    i32 selected_index;                 // 0x554
    f32 field_558;                      // 0x558
    f32 field_55c;                      // 0x55c
    i32 field_560;                      // 0x560
} BURN_SET_PC;                          // size 0x564

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

typedef struct LIGHTING_LOAD_WORLD_PC {
    u8 unknown_000[0x104];          // 0x0000
    VARIPTR buffer;                 // 0x0104
    VARIPTR end_buffer;             // 0x0108
    u8 unknown_10c[0x2aac - 0x10c]; // 0x010c
    BURN_SET_PC *burnset;           // 0x2aac
    u8 unknown_2ab0[4];             // 0x2ab0
    RTL_SET_PC *rtl_set;            // 0x2ab4
} LIGHTING_LOAD_WORLD_PC;           // size 0x2ab8

STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, specular_direction) == 0x54);
STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, specular_direction_missing) == 0x60);
STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, unknown_64) == 0x64);
STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, specular_intensity_present) == 0x74);
STATIC_ASSERT(offsetof(NUCAMERA, mtx) == 0);
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
    u8 unknown_00[0x50]; // 0x00
    u32 flags;           // 0x50
} BURNOUT_CUT_PREFIX_PC; // size 0x54

STATIC_ASSERT(offsetof(WORLDINFO, config_file) == 0x80);

typedef struct ripple_effect_pc_s {
    union {
        u32 start_color;
        u8 start_rgba[4];
    };

    union {
        u32 end_color;
        u8 end_rgba[4];
    };

    f32 lifetime;
    f32 initial_size;
    f32 end_size;
    char texture_name[16];
    NUMTL *material;
} RIPPLE_EFFECT_PC;

typedef struct world_ripple_view_pc_s {
    u8 unknown_0000[0x46f4];
    RIPPLE_EFFECT_PC *effects;
    i32 count;

    union {
        struct {
            i32 water_effect;
            i32 sabre_effect;
        };

        i32 effect_of_type[2]; /* indexed by LookupRippleEffectIndex */
    };
} WORLD_RIPPLE_VIEW_PC;

typedef struct ripple_contact_timer_view_pc_s {
    u8 unknown_0000[0xdc0];
    f32 timer;
} RIPPLE_CONTACT_TIMER_VIEW_PC;

#define RIPPLE_WORLD ((WORLD_RIPPLE_VIEW_PC *)WORLD)

typedef struct ripple_material_reader_pc_s {
    u8 unknown_0000[0x70];
    f32 opacity;
} RIPPLE_MATERIAL_READER_PC;

void __cdecl DebrisKillPlayers(void) {
    i32 i;
    GAMEOBJECT *player;
    DebrisPreCheckCollisions(&GameCam->position, 50.0f);
    for (i = 0; i < 8; i++) {
        player = Player[i];
        if (player != NULL && (player->flags_1fc_word & 0x1000) != 0 && player->dead == 0 &&
            player->vehicle_hit_timer <= 0.0f && !(player->spawn_protection_timer > 0.0f) &&
            (player->flags_ef0 & 0x400000) == 0 && CannotKill(player) == 0 && Player_HasInvincibility(player) == 0 &&
            (player->character_data->game_character->flags_090 & 0x4008000) == 0) {
            if (DebrisCollisionCheckScaleY(&player->collision_position, player->collision_radius,
                                           player->collision_y_scale) != -1) {
                ObjHitObj(NULL, player, 1, 0, 0, 1);
            }
            if (DebrisTorusCollisionCheckScaleY(&player->collision_position, player->collision_radius,
                                                player->collision_y_scale) != -1) {
                ObjHitObj(NULL, player, 1, 0, 0, 1);
            }
        }
    }
}

void __cdecl AddSurfaceDebris(GAMEOBJECT *object) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i8 layer;
    i32 effect;
    i32 count;
    f32 speed;
    f32 run_speed;
    f32 rate;
    f32 fraction;
    f32 emission_rate;
    f32 height_scale;
    f32 height;
    f32 minimum_rate;
    f64 dx;
    f64 dy;
    f64 dz;
    NUVEC current;
    NUVEC previous;
    NUVEC position;
    if (object->character_context == 0x2b) {
        return;
    }
    layer = object->field_283;
    if (layer != -1 && (TerLayer[layer].flags & 1)) {
        return;
    }
    if (object->floor_height == 2000000.0f) {
        return;
    }
    if ((PODRACE_ADATA != NULL && world->area == PODRACE_ADATA) || world->current_level == PODSPRINTA_LDATA) {
        PodDust(world, object);
        return;
    }
    height_scale = 1.0f;
    height = 0.0f;
    minimum_rate = 0.0f;
    if (object->id == (u16)id_LANDSPEEDER) {
        effect = 102;
        speed = object->horizontal_velocity_magnitude;
        run_speed = object->character_data->game_character->run_speed;
        minimum_rate = 0.0f;
        rate = 20.0f;
    } else if (WORLD->area != NULL && WORLD->area == HOTHBATTLE_ADATA) {
        effect = 86;
        speed = object->horizontal_velocity_magnitude;
        run_speed = object->character_data->game_character->run_speed;
        minimum_rate = 10.0f;
        rate = 100.0f;
        height = 3.0f;
    } else if ((BONUS_GUNSHIP_ADATA != NULL && world->area == BONUS_GUNSHIP_ADATA) ||
               (GUNSHIP_ADATA != NULL && world->area == GUNSHIP_ADATA)) {
        effect = 141;
        speed = object->horizontal_velocity_magnitude;
        run_speed = object->character_data->game_character->run_speed;
        rate = GunshipDustEmissionRatePc;
    } else {
        return;
    }
    if (world->game_debris_sys->entries[effect].effect == -1 || run_speed <= 0.0f) {
        return;
    }
    if (height > 0.0f) {
        height_scale = 1.0f - (object->collision_min.y - object->floor_height) * (1.0f / height);
        if (height_scale <= 0.0f) {
            return;
        }
        if (height_scale > 1.0f) {
            height_scale = 1.0f;
        }
    }
    emission_rate = (speed / run_speed) * rate;
    if (emission_rate < minimum_rate) {
        emission_rate = minimum_rate;
    }
    emission_rate *= height_scale;
    current.x = object->position.x;
    current.y = object->floor_height;
    current.z = object->position.z;
    if (object->previous_floor_height != 2000000.0f) {
        previous.x = object->start_position.x;
        previous.y = object->previous_floor_height;
        previous.z = object->start_position.z;
    } else {
        previous = current;
    }
    count = ParticlesPerSecond(emission_rate, FRAMETIME);
    if (count <= 0) {
        return;
    }
    dx = current.x - previous.x;
    dy = current.y - previous.y;
    dz = current.z - previous.z;
    do {
        fraction = qrand() * (1.0f / 65535.0f);
        position.x = previous.x + dx * fraction;
        position.y = previous.y + dy * fraction;
        position.z = previous.z + dz * fraction;
        AddVariableShotDebrisEffect(world->game_debris_sys->entries[effect].effect, &position, 1, 0, 0);
    } while (--count > 0);
}

void __cdecl AddWaterSplash(GAMEOBJECT *object, NUVEC *position) {
    AddGameDebrisXYZ(WORLD->game_debris_sys, 13, position->x, object->water_height, position->z);
    PlaySfx((char *)TerrainWaterJumpSoundNamePc, &object->collision_position);
    NewRumble(object->pad->bound_pad, 0.5f, 0);
    NewBuzzFrames(object->pad->bound_pad, 1, 0);
}

void AddSlamDebris(GAMEOBJECT *object) {
    f32 radius;
    f32 strength;
    i32 damage;
    i32 flags;
    struct EXPLOSION *explosion;
    NUVEC position;
    FindSlamOrigin(object, &position, NULL);
    flags = 0x17;
    if ((object->flags_1fc & 0x80) != 0 && Cheat_IsOn(0x16)) {
        radius = 1.0f;
        flags = 0x37;
        damage = 2;
        strength = 0.5f;
    } else {
        radius = 0.6f;
        damage = 1;
        strength = 0.3f;
    }
    explosion = AddExplosion(&position, radius, strength, object, object->slam_effect, flags);
    if (explosion != NULL) {
        explosion->field_0x32 = damage;
    }
}

void __cdecl InitRippleMtl(char *name, NUMTL **result, VARIPTR *buffer, VARIPTR *end) {
    char filename[64] = "stuff\\";
    i32 texture;
    if (name != NULL) {
        NuStrCat(filename, name);
        buffer->addr = (buffer->addr + 15) & ~15u;
        NuStrCat(filename, ".tex");
        texture = NuTexRead(filename, buffer, *end);
        if (texture != 0) {
            *result = NuMtlCreateEx3D(1, 1);
            (*result)->attribs.unknown_1_1_2 = 0;
            (*result)->attribs.unknown_1_4_8 = 0;
            ((RIPPLE_MATERIAL_READER_PC *)*result)->opacity = 1.0f;
            (*result)->attribs.z_mode = 1;
            (*result)->attribs.alpha_mode = 2;
            (*result)->attribs.filter_mode = 1;
            (*result)->attribs.unknown_2_1_2 = 2;
            (*result)->texture_id = (i16)texture;
            NuMtlUpdate(*result);
            return;
        }
    }
    *result = NULL;
}

void __cdecl BuildRippleMtx(NUMTX *matrix, NUVEC *normal, NUVEC *position, u16 unused_x, u16 unused_y) {
    (void)unused_x;
    (void)unused_y;
    FindAnglesXY(normal, NULL, NULL);
    NuMtxSetRotationX(matrix, temp_zrot);
    NuMtxRotateY(matrix, temp_yrot);
    NuMtxTranslate(matrix, position);
}

void __cdecl BurnoutApply(i32 view) {
    BURN_PARAMETERS_PC parameters;
    NUVEC average;
    parameters.field_00 = 0;
    memset(&parameters.field_04, 0, 0x50);
    if ((CutStopInfo == NULL || (((BURNOUT_CUT_PREFIX_PC *)CutStopInfo)->flags & 0x80) == 0) &&
        WORLD->page_unknown != 0) {
        edrtlCalculateBurnoutEx((BURN_SET_PC *)WORLD->page_unknown, &parameters, (NUVEC *)&global_camera.mtx.m30,
                                FRAMETIME);
    }
    if (parameters.field_14 > 0.0f) {
        NuRndrSetBurnoutParametersPc(view, &parameters);
    }
    if ((Players_AveragePos(&average, NULL) != 0 || netclient != 0) && PCShadowCurrentLight->enabled != 0) {
        NuRndrSetShadowCoefficientPc(0, WORLD->shadow_editor->field_24);
        NuRndrSetShadowCoefficientPc(1, WORLD->shadow_editor->field_28);
        NuRndrSetShadowCoefficientPc(2, WORLD->shadow_editor->field_2c);
        NuDeferredShadingRender();
    }
}

static f32 RippleRandomRadiusPc = 0.15f;

void __cdecl AddSurfaceRipples(GAMEOBJECT *object) {
    RIPPLE_CONTACT_TIMER_VIEW_PC *contact_timer = (RIPPLE_CONTACT_TIMER_VIEW_PC *)object;
    NUVEC position;
    NUVEC normal;
    NUVEC seed;
    NUVEC velocity;
    NUMTX matrix;
    f32 rate;
    f32 size;
    f32 swap_y;
    f32 swap_x;
    i32 count;
    i32 i;
    if (RIPPLE_WORLD->effects == NULL || object->dead != 0) {
        return;
    }
    if (object->contact_count == 1) {
        if (object->contact_material[0] == 13 || object->contact_material[0] == 12) {
            rate = object->pad->input_magnitude / object->character_data->game_character->run_speed *
                   ForceFieldRippleRatePc;
            if (rate < .5f) {
                rate = .5f;
            }
            count = ParticlesPerSecond(rate, FRAMETIME);
            if (count > 0) {
                position = object->contact_position;
                normal = object->contact_normal;
                BuildRippleMtx(&matrix, &normal, &position, 0, 0);
                seed = normal;
                seed.x = FRAMETIME * seed.x;
                seed.y = FRAMETIME * seed.y;
                seed.z = FRAMETIME * seed.z;
                swap_y = seed.y;
                seed.y = -seed.z;
                swap_x = seed.x;
                seed.x = swap_y;
                seed.z = -swap_x;
                for (i = 0; i < count; ++i) {
                    u16 angle = (u16)qrand();
                    velocity = seed;
                    VecRotateAxis(&velocity, angle, &normal);
                    AddRipple(RippleSetPc, &matrix, RIPPLE_WORLD->effects[RIPPLE_WORLD->sabre_effect].initial_size,
                              RIPPLE_WORLD->effects[RIPPLE_WORLD->sabre_effect].end_size,
                              RIPPLE_WORLD->effects[RIPPLE_WORLD->sabre_effect].lifetime, 0.0f,
                              RIPPLE_WORLD->effects[RIPPLE_WORLD->sabre_effect].start_color,
                              RIPPLE_WORLD->effects[RIPPLE_WORLD->sabre_effect].end_color, 15,
                              RIPPLE_WORLD->effects[RIPPLE_WORLD->sabre_effect].material, &velocity);
                }
            }
            /* The native sound position remains unwritten when no particles are emitted. */
            if (contact_timer->timer <= 0) {
                PlaySfx((char *)RippleForceFieldSoundNamePc, &position);
                contact_timer->timer = RippleSfxCooldownPc;
            }
        }
        if (contact_timer->timer > 0) {
            contact_timer->timer -= FRAMETIME;
        }
    } else {
        if (object->water_intersects == 0 || object->model_draw_result == 0) {
            return;
        }
        rate = (f32)(object->pad->input_magnitude / object->character_data->game_character->run_speed * 20.0);
        if (rate < 1.0f) {
            rate = 1.0f;
        }
        count = ParticlesPerSecond(rate, FRAMETIME);
        if (count <= 0) {
            return;
        }
        position.x = 0.0f;
        position.y = 0.0f;
        position.z = 0.0f;
        BuildRippleMtx(&matrix, &v010, &position, 0, 0);
        size = (f32)(object->bounds_radius * .5);
        for (i = 0; i < count; ++i) {
            matrix.m30 = (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f) - .5) * RipplePerturbationPc +
                               object->collision_position.x);
            matrix.m31 = (f32)(object->water_height + (f64).001f);
            matrix.m32 = (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f) - .5) * RipplePerturbationPc +
                               object->collision_position.z);
            RippleRandomRadiusPc = object->bounds_radius + object->bounds_radius + object->bounds_radius;
            AddRipple(RippleSetPc, &matrix, size, RippleRandomRadiusPc,
                      RIPPLE_WORLD->effects[RIPPLE_WORLD->water_effect].lifetime, 0.0f,
                      RIPPLE_WORLD->effects[RIPPLE_WORLD->water_effect].start_color,
                      RIPPLE_WORLD->effects[RIPPLE_WORLD->water_effect].end_color, 7,
                      RIPPLE_WORLD->effects[RIPPLE_WORLD->water_effect].material, NULL);
        }
    }
}
