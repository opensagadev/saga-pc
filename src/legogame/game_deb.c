#include "gameapi/edptl.h"
#include "globals.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmos/fx/edgizshadowmachine.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/fx/explosions.h"
#include "legoapi/items/fx/ripples.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeI.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nuvport_gen.h"
#include <string.h>
#include <stdio.h>

typedef struct BURN_SET {            // size 0x564
    BURN_PARAMETERS parameters;      // 0x000
    BURN_PARAMETERS parameters_copy; // 0x054
    i32 unknown_a8;                  // 0x0a8
    i32 field_ac;                    // 0x0ac
    i32 unknown_b0;                  // 0x0b0
    i32 unknown_b4;                  // 0x0b4
    f32 unknown_b8;                  // 0x0b8
    f32 field_bc;                    // 0x0bc
    f32 unknown_c0;                  // 0x0c0
    f32 unknown_c4;                  // 0x0c4
    f32 unknown_c8;                  // 0x0c8
    f32 field_cc;                    // 0x0cc
    BURN_OUT burnouts[32];           // 0x0d0
    i32 active_count;                // 0x550
    i32 selected_index;              // 0x554
    f32 unknown_558;                 // 0x558
    f32 unknown_55c;                 // 0x55c
    i32 unknown_560;                 // 0x560
} BURN_SET;

/* Cut resource access prefix; the cut loader owns the resource. */
typedef struct BURNOUT_CUT_PREFIX { // size 0x54
    u8 unknown_00[0x50];            // 0x00
    u32 flags;                      // 0x50
} BURNOUT_CUT_PREFIX;

#define RIPPLE_WORLD ((WORLD_RIPPLE_VIEW *)WORLD)

void DebrisKillPlayers(void) {
    i32 i;
    GAMEOBJECT *player;
    DebrisPreCheckCollisions(&GameCam->position, 50.0f);
    for (i = 0; i < 8; i++) {
        player = Player[i];
        if (player != NULL && (player->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && player->dead == 0 &&
            player->vehicle_hit_timer <= 0.0f && !(player->spawn_protection_timer > 0.0f) && (player->flags_ef0 & 0x400000) == 0 &&
            CannotKill(player) == 0 && Player_HasInvincibility(player) == 0 &&
            (player->character_data->game_character->flags_090 & (CHARACTER_RUNTIME_GHOST | CHARACTER_RUNTIME_HAZARD_PROTECTION)) == 0) {
            if (DebrisCollisionCheckScaleY(&player->collision_position, player->collision_radius, player->collision_y_scale) != -1) {
                ObjHitObj(NULL, player, 1, 0, 0, 1);
            }
            if (DebrisTorusCollisionCheckScaleY(&player->collision_position, player->collision_radius, player->collision_y_scale) != -1) {
                ObjHitObj(NULL, player, 1, 0, 0, 1);
            }
        }
    }
}

void AddSurfaceDebris(GAMEOBJECT *object) {
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
    if (object->character_context == CHARACTER_CONTEXT_DOOMED) {
        return;
    }
    layer = object->unknown_283;
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
        rate = GunshipDustEmissionRate;
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

void AddWaterSplash(GAMEOBJECT *object, NUVEC *position) {
    AddGameDebrisXYZ(WORLD->game_debris_sys, 13, position->x, object->water_height, position->z);
    PlaySfx("FS_WaterJump", &object->collision_position);
    NewRumble(object->pad->bound_pad, 0.5f, 0);
    NewBuzzFrames(object->pad->bound_pad, 1, 0);
}

void AddSlamDebris(GAMEOBJECT *object) {
    f32 radius;
    f32 strength;
    i32 damage;
    i32 flags;
    EXPLOSION *explosion;
    NUVEC position;
    FindSlamOrigin(object, &position, NULL);
    flags = 0x17;
    if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && Cheat_IsOn(0x16)) {
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
        explosion->damage = damage;
    }
}

void InitRippleMtl(char *name, NUMTL **result, VARIPTR *buffer, VARIPTR *end) {
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
            ((RIPPLE_MATERIAL_READER *)*result)->opacity = 1.0f;
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

void BuildRippleMtx(NUMTX *matrix, NUVEC *normal, NUVEC *position, u16 unused_x, u16 unused_y) {
    (void)unused_x;
    (void)unused_y;
    FindAnglesXY(normal, NULL, NULL);
    NuMtxSetRotationX(matrix, temp_zrot);
    NuMtxRotateY(matrix, temp_yrot);
    NuMtxTranslate(matrix, position);
}

void BurnoutApply(i32 view) {
    BURN_PARAMETERS parameters;
    NUVEC average;
    parameters.unknown_00 = 0;
    memset(&parameters.unknown_04, 0, 0x50);
    if ((CutStopInfo == NULL || (((BURNOUT_CUT_PREFIX *)CutStopInfo)->flags & 0x80) == 0) && WORLD->page_unknown != 0) {
        edrtlCalculateBurnoutEx((BURN_SET *)WORLD->page_unknown, &parameters, (NUVEC *)&global_camera.mtx.m30, FRAMETIME);
    }
    if (parameters.unknown_14 > 0.0f) {
        NuRndrSetBurnoutParameters(view, &parameters);
    }
    if ((Players_AveragePos(&average, NULL) != 0 || netclient != 0) && PCShadowCurrentLight->enabled != 0) {
        NuRndrSetShadowCoefficient(0, WORLD->shadow_editor->unknown_24);
        NuRndrSetShadowCoefficient(1, WORLD->shadow_editor->unknown_28);
        NuRndrSetShadowCoefficient(2, WORLD->shadow_editor->unknown_2c);
        NuDeferredShadingRender();
    }
}

f32 GunshipDustEmissionRate = 75.0f;
static f32 RippleRandomRadius = 0.15f;
f32 RipplePerturbation = 0.15f;
f32 RippleSfxCooldown = 0.5f;
f32 ForceFieldRippleRate = 100.0f;

void AddSurfaceRipples(GAMEOBJECT *object) {
    RIPPLE_CONTACT_TIMER_VIEW *contact_timer = (RIPPLE_CONTACT_TIMER_VIEW *)object;
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
            rate = object->pad->input_magnitude / object->character_data->game_character->run_speed * ForceFieldRippleRate;
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
                    AddRipple(RippleSet, &matrix, RIPPLE_WORLD->effects[RIPPLE_WORLD->sabre_effect].initial_size,
                              RIPPLE_WORLD->effects[RIPPLE_WORLD->sabre_effect].end_size,
                              RIPPLE_WORLD->effects[RIPPLE_WORLD->sabre_effect].lifetime, 0.0f,
                              RIPPLE_WORLD->effects[RIPPLE_WORLD->sabre_effect].start_color,
                              RIPPLE_WORLD->effects[RIPPLE_WORLD->sabre_effect].end_color, 15,
                              RIPPLE_WORLD->effects[RIPPLE_WORLD->sabre_effect].material, &velocity);
                }
            }
            /* The sound position stays unwritten when no particles are emitted. */
            if (contact_timer->timer <= 0) {
                PlaySfx("SabFField", &position);
                contact_timer->timer = RippleSfxCooldown;
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
            matrix.m30 = (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f) - .5) * RipplePerturbation + object->collision_position.x);
            matrix.m31 = (f32)(object->water_height + (f64).001f);
            matrix.m32 = (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f) - .5) * RipplePerturbation + object->collision_position.z);
            RippleRandomRadius = object->bounds_radius + object->bounds_radius + object->bounds_radius;
            AddRipple(RippleSet, &matrix, size, RippleRandomRadius, RIPPLE_WORLD->effects[RIPPLE_WORLD->water_effect].lifetime, 0.0f,
                      RIPPLE_WORLD->effects[RIPPLE_WORLD->water_effect].start_color,
                      RIPPLE_WORLD->effects[RIPPLE_WORLD->water_effect].end_color, 7,
                      RIPPLE_WORLD->effects[RIPPLE_WORLD->water_effect].material, NULL);
        }
    }
}
