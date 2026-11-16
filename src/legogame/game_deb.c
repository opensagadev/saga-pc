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
    if (object->player.character_context == CHARACTER_CONTEXT_DOOMED) {
        return;
    }
    layer = object->ground_layer;
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
    NewRumble(object->player.pad->bound_pad, 0.5f, 0);
    NewBuzzFrames(object->player.pad->bound_pad, 1, 0);
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
    explosion = AddExplosion(&position, radius, strength, object, object->player.slam_effect, flags);
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
            (*result)->attribs.u_address = NUMTL_ADDRESS_WRAP;
            (*result)->attribs.v_address = NUMTL_ADDRESS_WRAP;
            (*result)->opacity = 1.0f;
            (*result)->attribs.z_mode = NUMTL_ZMODE_READ_ONLY;
            (*result)->attribs.alpha_mode = NUMTL_ALPHA_ADDITIVE;
            (*result)->attribs.filter_mode = 1;
            (*result)->attribs.lighting = NUMTL_LIGHTING_NONE;
            (*result)->texture_id = (i16)texture;
            NuMtlUpdate(*result);
            return;
        }
    }
    *result = NULL;
}

void BuildRippleMtx(NUMTX *matrix, NUVEC *normal, NUVEC *position, u16 unused_x, u16 unused_y) {
    FindAnglesXY(normal, NULL, NULL);
    NuMtxSetRotationX(matrix, temp_zrot);
    NuMtxRotateY(matrix, temp_yrot);
    NuMtxTranslate(matrix, position);
}

void BurnoutApply(i32 view) {
    BURN_PARAMETERS parameters;
    NUVEC average;
    parameters.enabled = 0;
    memset(&parameters.start_angle, 0, 0x50);
    if ((CutStopInfo == NULL || (CutStopInfo->flags & 0x80) == 0) && WORLD->burnset != NULL) {
        edrtlCalculateBurnoutEx(WORLD->burnset, &parameters, &global_camera.mtx.translation, FRAMETIME);
    }
    if (parameters.intensity > 0.0f) {
        NuRndrSetBurnoutParameters(view, &parameters);
    }
    if ((Players_AveragePos(&average, NULL) != 0 || netclient != 0) && PCShadowCurrentLight->enabled != 0) {
        NuRndrSetShadowCoefficient(0, WORLD->shadow_editor->coefficient_0);
        NuRndrSetShadowCoefficient(1, WORLD->shadow_editor->coefficient_1);
        NuRndrSetShadowCoefficient(2, WORLD->shadow_editor->coefficient_2);
        NuDeferredShadingRender();
    }
}

f32 GunshipDustEmissionRate = 75.0f;
static f32 RippleRandomRadius = 0.15f;
f32 RipplePerturbation = 0.15f;
f32 RippleSfxCooldown = 0.5f;
f32 ForceFieldRippleRate = 100.0f;

void AddSurfaceRipples(GAMEOBJECT *object) {
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
    if (WORLD->ripple_effects == NULL || object->dead != 0) {
        return;
    }
    if (object->contact_count == 1) {
        if (object->contact_material[0] == 13 || object->contact_material[0] == 12) {
            rate = object->player.pad->input_magnitude / object->character_data->game_character->run_speed * ForceFieldRippleRate;
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
                    AddRipple(RippleSet, &matrix, WORLD->ripple_effects[WORLD->sabre_ripple_effect].initial_size,
                              WORLD->ripple_effects[WORLD->sabre_ripple_effect].end_size,
                              WORLD->ripple_effects[WORLD->sabre_ripple_effect].lifetime, 0.0f,
                              WORLD->ripple_effects[WORLD->sabre_ripple_effect].start_color,
                              WORLD->ripple_effects[WORLD->sabre_ripple_effect].end_color, 15,
                              WORLD->ripple_effects[WORLD->sabre_ripple_effect].material, &velocity);
                }
            }
            /* The sound position stays unwritten when no particles are emitted. */
            if (object->player.sabre_contact_sfx_timer <= 0) {
                PlaySfx("SabFField", &position);
                object->player.sabre_contact_sfx_timer = RippleSfxCooldown;
            }
        }
        if (object->player.sabre_contact_sfx_timer > 0) {
            object->player.sabre_contact_sfx_timer -= FRAMETIME;
        }
    } else {
        if (object->water_intersects == 0 || object->model_draw_result == 0) {
            return;
        }
        rate = (f32)(object->player.pad->input_magnitude / object->character_data->game_character->run_speed * 20.0);
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
            AddRipple(RippleSet, &matrix, size, RippleRandomRadius, WORLD->ripple_effects[WORLD->water_ripple_effect].lifetime, 0.0f,
                      WORLD->ripple_effects[WORLD->water_ripple_effect].start_color,
                      WORLD->ripple_effects[WORLD->water_ripple_effect].end_color, 7,
                      WORLD->ripple_effects[WORLD->water_ripple_effect].material, NULL);
        }
    }
}
