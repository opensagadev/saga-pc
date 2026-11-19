#include <stdio.h>
#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/gizmos/object/gizpanel.h"
#include "gameapi/ai/aisys/gameaiscript.h"
#include "gameapi/edptl.h"
#include "globals.h"
#include "legoapi/actions/character/legoapi_tag.h"
#include "legoapi/actions/character/speederchase.h"
#include "legoapi/ai/core/legoai.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizbuildit.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/gizmos/object/gizbuildits.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/gizmos/traps/gizbombgen.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/minikits.h"
#include "legoapi/items/collect/thermaldetonators.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/misc/buffers.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/startup_dats.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeV.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nuvec.h"

#include <string.h>
#include "nu2api/nusound/nusound.h"
#include "nu2api/nustencilshadows_PC.h"

/* Episode V FixUpLevels callbacks (Hoth escape, Dagobah, Cloud City trap). */
GAMEOBJECT *Vader_obj = NULL;
static GIZAIMESSAGE *Vader_ai_message = NULL;
struct GIZBOMBGEN_s *HothBattleC_BombGenerator = 0;
AILOCATOR *HothEscapeBLocator = NULL;
GAMEOBJECT *HothEscapeBSnowmob = NULL;

/* Asteroid field. */

/* Rotating rock at the end of AsteroidChase C. */

static void AsteroidChaseD_SetTurretTarget(i32 index, NUVEC *position) {
    if (StarDestroyerTurrets[index] != NULL && StarDestroyerTurrets[index]->opponent == NULL) {
        StarDestroyerTurrets[index]->opponent = position;
        StarDestroyerTurrets[index]->opponent_type = 2;
    }
}

static void AsteroidSetAngVec(NUANGVEC *out, NUANG x, NUANG y, NUANG z) {
    out->x = x;
    out->y = y;
    out->z = z;
}

TROOPERCANNON troopercannons[4] = {0};

i32 netclient = 0;

static void KilledTrooperCannon(GAMEOBJECT *object) {
    i32 index;
    if (netclient != 0) {
        return;
    }
    for (index = 0; index < 4; ++index) {
        if (troopercannons[index].object == object) {
            DeactivateCharacter(troopercannons[index].character_name);
            GizBuildIt_SetToStart(troopercannons[index].buildit, 0, 0);
            GizmoActivate(WORLD->gizmo_sys, GizmoFindByData(WORLD->gizmo_sys, gizbuildit_gizmotype_id, troopercannons[index].buildit), 1,
                          1);
            GizBuildit_SetVisibility(troopercannons[index].buildit, 1);
            troopercannons[index].rebuilding = 1;
            WORLD->level_progress->destroyed_trooper_cannon_mask |= 1u << index;
            return;
        }
    }
}

void InitTrooperCannons(WORLDINFO *unused) {
    memset(troopercannons, 0, sizeof(troopercannons));
}

void ResetTrooperCannons(WORLDINFO *world, i32 trooper_id) {
    i32 cannon;
    i32 index = 0;
    char name[32];
    GAMEOBJECT *trooper;
    if (troopercannons_beenReset != 0 && (netclient == 0 || troopercannons[0].object != NULL)) {
        return;
    }
    for (cannon = 0; cannon < 4; ++cannon) {
        memset(&troopercannons[index], 0, sizeof(troopercannons[index]));
        sprintf(name, "trooper_cannon%d", cannon + 1);
        troopercannons[index].buildit = GizBuildIt_Find(world, name);
        if (troopercannons[index].buildit != NULL) {
            sprintf(name, "troopercannon_%d", cannon + 1);
            troopercannons[index].object = GetNamedGameObject(world->ai_sys, name);
            if (troopercannons[index].object != NULL) {
                NuStrCpy(troopercannons[index].character_name, name);
            }
        }
        sprintf(name, "BASE%d", cannon + 1);
        troopercannons[index].base = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, name);
        if (netclient != 0) {
            if (troopercannons[index].object != NULL) {
                ++index;
            }
            continue;
        }
        if (troopercannons[index].buildit != NULL && troopercannons[index].object != NULL) {
            if ((world->level_progress->destroyed_trooper_cannon_mask & (1u << cannon)) == 0) {
                if (troopercannons[index].base != NULL) {
                    GizmoSetVisibility(world->gizmo_sys, troopercannons[index].base, 1, 1);
                    GizObstacle_PlayBackwards(troopercannons[index].base->object);
                }
                GizBuildIt_Finish(troopercannons[index].buildit);
                GizBuildit_SetVisibility(troopercannons[index].buildit, 0);
                troopercannons[cannon].rebuilding = 0;
                ActivateCharacter(name, NULL, 0);
                AddGameDebris(world->game_debris_sys, 0x5c, &troopercannons[index].object->collision_position);
                trooper = AddDynamicCreature(trooper_id, &troopercannons[index].object->collision_position,
                                             troopercannons[index].object->movement_angle, "CannonTrooper",
                                             &troopercannons[index].object->ai_packet.path_info, NULL, 0, NULL, NULL, 0, 0);
                if (trooper != NULL) {
                    TakeOverGameObject(trooper, troopercannons[index].object, 0, 1);
                }
            } else {
                GizBuildIt_SetToStart(troopercannons[index].buildit, 0, 0);
                troopercannons[cannon].rebuilding = 1;
            }
        }
        ++index;
    }
    troopercannons_beenReset = 1;
}

void UpdateTrooperCannons(WORLDINFO *unused) {
    i32 index;
    for (index = 0; index < 4; ++index) {
        TROOPERCANNON *cannon = &troopercannons[index];
        if (cannon->buildit != NULL && netclient == 0) {
            if (cannon->object != NULL) {
                if (GizBuildIt_AtEnd(cannon->buildit) && cannon->rebuilding != 0) {
                    ActivateCharacter(cannon->character_name, NULL, 0);
                    GizBuildit_SetVisibility(cannon->buildit, 0);
                    cannon->rebuilding = 0;
                    WORLD->level_progress->destroyed_trooper_cannon_mask &= ~(1u << index);
                }
            } else if (GizBuildIt_AtEnd(cannon->buildit)) {
                GizBuildIt_KillParts(cannon->buildit);
                GizBuildIt_SetToStart(cannon->buildit, 0, 0);
                GizBuildit_SetVisibility(cannon->buildit, 0);
            }
        }
        if (cannon->object == NULL || netclient != 0) {
            break;
        }
        if (cannon->object->player.rider != NULL) {
            if (cannon->object->player.rider->tag_object_callback == NULL) {
                cannon->object->player.rider->tag_object_callback = KilledTrooperCannon;
            }
        } else if (cannon->object->tag_object_callback == NULL) {
            cannon->object->tag_object_callback = KilledTrooperCannon;
        }
    }
}

void HothEscapeA_Init(WORLDINFO *world) {
    memset(troopercannons, 0, sizeof(troopercannons));
    troopercannons_beenReset = 0;
}

void HothEscapeA_Reset(WORLDINFO *world) {
}

void HothEscapeA_Update(WORLDINFO *world) {
    ResetTrooperCannons(world, id_SNOWTROOPER);
    UpdateTrooperCannons(world);
}

void HothEscapeB_Init(WORLDINFO *world) {
    if (netclient == 0) {
        HothEscapeBLocator = AIPathFindLocator(world->ai_sys, "snow_mob");
        HothEscapeBSnowmob = GetNamedGameObject(world->ai_sys, "snowmob_1");
    }
    memset(troopercannons, 0, sizeof(troopercannons));
    troopercannons_beenReset = 0;
}

void HothEscapeB_Reset(WORLDINFO *world) {
    HothEscapeBLocator = AIPathFindLocator(world->ai_sys, "snow_mob");
    HothEscapeBSnowmob = GetNamedGameObject(world->ai_sys, "snowmob_1");
    TerSurface[9].movement_scale = TerSurface[17].movement_scale;
    TerSurface[9].flags = TerSurface[17].flags & ~TERSURFACE_REFLECTIVE;
}

void HothEscapeB_Update(WORLDINFO *world) {
    ResetTrooperCannons(world, id_SNOWTROOPER);
    UpdateTrooperCannons(world);
    if (netclient == 0 && HothEscapeBLocator != NULL && HothEscapeBSnowmob != NULL) {
        HothEscapeBLocator->position = HothEscapeBSnowmob->player.locator_matrices[1].translation;
    }
}

void HothEscapeC_Init(WORLDINFO *world) {
    memset(troopercannons, 0, sizeof(troopercannons));
    troopercannons_beenReset = 0;
}

void HothEscapeC_Reset(WORLDINFO *world) {
}

void HothEscapeC_Update(WORLDINFO *world) {
    ResetTrooperCannons(world, id_SNOWTROOPER);
    UpdateTrooperCannons(world);
}

void HothEscapeC_AlwaysUpdate(WORLDINFO *world) {
    LevelStreaming_DoorOverride(world, dat_0087b220, 7.5f, NULL);
}

void HothEscapeD_Init(WORLDINFO *world) {
    memset(troopercannons, 0, sizeof(troopercannons));
    troopercannons_beenReset = 0;
}

void HothEscapeD_Reset(WORLDINFO *world) {
}

void HothEscapeD_Update(WORLDINFO *world) {
    ResetTrooperCannons(world, id_SNOWTROOPER);
    UpdateTrooperCannons(world);
}

static void AsteroidChase_FindPopBits(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "small_pop_bit1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "small_pop_bit2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "small_pop_bit3", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], "small_pop_bit4", 1);
}

void Asteroid_PartKill(PART *part, i32 reason) {
    AddGameDebris(WORLD->game_debris_sys, 0x5d, &part->position);
    AddGameDebris(WORLD->game_debris_sys, 0x5e, &part->position);
    AddGameDebris(WORLD->game_debris_sys, 0x5f, &part->position);
    AddPartDebris(WORLD->part_debris_sys, 4, &part->position);
    if (part->player_mask == 1) {
        i32 type;
        if (reason == 4) {
            type = 0;
        } else if (reason == 5) {
            type = 1;
        } else {
            return;
        }
        AddMiscPickups(&part->position, type, 1000, 0);
    } else {
        AddMiscPickups(&part->position, -1, 0, 1);
    }
}

void Asteroid_AddParts(GIZMOBLOWUP *blowup) {
    i32 special_indices[4] = {0, -1, -1, -1};
    i32 index;
    i32 part_count;
    nuhspecial_s *special;
    NUANGVEC rotation;
    NUMTX matrix;
    NUVEC velocity;
    NUVEC centre;
    ADDPART params;
    PART *part;
    part_count = qrand() / 0x4000 + 1;
    for (index = 1; index < part_count; index++) {
        special_indices[index] = qrand() / (0xffff / 3 + 1) + 1;
    }
    for (index = 0; index < part_count; index++) {
        special = &LevHSpecial[special_indices[index]];
        if (NuSpecialExistsFn(special) == 0) {
            continue;
        }
        AsteroidSetAngVec(&rotation, qrand(), qrand(), qrand());
        NuMtxSetRotateXYZVU0(&matrix, &rotation);
        NuMtxTranslate(&matrix, &blowup->position);
        velocity.x = velocity.y = 0.0f;
        velocity.z = (f32)(qrand() * PlayerRandomScale * NuTimeBarColumnStep + 2.0);
        NuVecRotateY(&velocity, &velocity, qrand());
        params = Default_ADDPART;
        params.matrix = &matrix;
        params.velocity = &velocity;
        NuSpecialGetRadius(special, &centre, &params.start_radius);
        params.end_radius = params.start_radius;
        params.gravity = 0.0f;
        params.special = special;
        params.flags = 0x8000193;
        if (index == 0) {
            params.flags = 0x800019b;
        }
        params.time_step = FRAMETIME;
        params.collision_fn = (PART_ACTION)PartCollide_3D;
        params.kill_fn = (PART_KILL)Asteroid_PartKill;
        params.fade_time = (f32)(qrand() * PlayerRandomScale * NuTimeBarColumnStep + 7.0);
        part = AddPart(&params);
        if (part != NULL) {
            part->player_mask = (index != 0) + 1;
        }
    }
}

ASTEROID asteroids[128] = {0};
i32 nasteroids = 0;
u8 minikitCounter_A = 0;
u8 minikitCounter_C = 0;
nuhspecial_s escape[8] = {0};
i32 maxrotspd[3] = {0x1555, 0x38e, 0x16c};
WORLDINFO *WORLD = &WorldInfo[0];

void Asteroids_Reset(WORLDINFO *world) {
    nuhspecial_s specials[128];
    ASTEROID *asteroid;
    GIZMOBLOWUP *blowup;
    char *name;
    i32 count;
    i32 i;
    i32 j;
    i32 type;
    i32 kind;
    i32 max_speed;
    memset(asteroids, 0, sizeof(asteroids));
    nasteroids = 0;
    count = NuSpecialFindMulti(world->current_gscn, specials, "asteroid", 128, 0);
    if (count == 0) {
        return;
    }
    for (i = 0; i < count; i++) {
        for (j = 0; j < world->gizmo_blowup_type_count; j++) {
            NuSpecialCompare(&world->gizmo_blowup_types[j].animated_special, &specials[i]);
        }
        if (NuSpecialGetVisibilityFn(&specials[i]) != 0) {
            asteroid = &asteroids[nasteroids];
            asteroid->special = specials[i];
            name = NuSpecialGetName(&asteroid->special);
            type = 0;
            if (name != NULL) {
                if (NuStrIStr(name, "asteroid_a") != NULL) {
                    type = 0;
                } else if (NuStrIStr(name, "asteroid_pop") != NULL) {
                    type = 0;
                } else if (NuStrIStr(name, "asteroid_b") != NULL) {
                    type = 1;
                } else if (NuStrIStr(name, "asteroid_c") != NULL) {
                    type = 2;
                }
            }
            max_speed = maxrotspd[type];
            asteroid->rotation_speed_x = (i16)(max_speed - qrand() / (0xffff / (max_speed * 2) + 1));
            asteroid->rotation_speed_y = (i16)(max_speed - qrand() / (0xffff / (max_speed * 2) + 1));
            nasteroids++;
        }
    }
    for (i = 0; i < world->gizmo_blowup_count; i++) {
        blowup = &world->gizmo_blowups[i];
        asteroid = &asteroids[nasteroids];
        name = blowup->name;
        kind = 0;
        if (name != NULL) {
            if (NuStrIStr(name, "asteroid_a") != NULL) {
                kind = 0;
            } else if (NuStrIStr(name, "asteroid_pop") != NULL) {
                kind = 0;
            } else if (NuStrIStr(name, "asteroid_mid") != NULL) {
                kind = 1;
            } else {
                continue;
            }
        }
        asteroid->blowup = blowup;
        max_speed = maxrotspd[kind];
        asteroid->rotation_speed_x = (i16)(max_speed - qrand() / (0xffff / (max_speed * 2) + 1));
        asteroid->rotation_speed_y = (i16)(max_speed - qrand() / (0xffff / (max_speed * 2) + 1));
        asteroid->rotation_speed_z = (i16)(max_speed - qrand() / (0xffff / (max_speed * 2) + 1));
        nasteroids++;
    }
}

void Asteroids_Update(void) {
    ASTEROID *asteroid;
    NUMTX *matrix;
    i32 i;
    asteroid = asteroids;
    for (i = 0; i < nasteroids; i++, asteroid++) {
        if (asteroid->blowup != NULL) {
            if ((asteroid->blowup->status_flags & BLOWUP_STATUS_VISIBLE) != 0 &&
                (asteroid->blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) == 0) {
                asteroid->activated = 0;
                asteroid->blowup->model_rotation_x += (u16)(asteroid->rotation_speed_x * FRAMETIME);
                asteroid->blowup->model_rotation_y += (u16)(asteroid->rotation_speed_y * FRAMETIME);
                asteroid->blowup->model_rotation_z += (u16)(asteroid->rotation_speed_z * FRAMETIME);
                asteroid->blowup->status_flags |= 0x10000;
                GizmoBlowupUpdateMatrix(asteroid->blowup);
                continue;
            }
        } else if (NuSpecialGetVisibilityFn(&asteroid->special) != 0) {
            asteroid->activated = 0;
            matrix = NuSpecialGetDrawMtx(&asteroid->special);
            if (matrix != NULL) {
                NuMtxPreRotateX(matrix, (i32)(asteroid->rotation_speed_x * FRAMETIME));
                NuMtxPreRotateY(matrix, (i32)(asteroid->rotation_speed_y * FRAMETIME));
                NuSpecialUpdate(&asteroid->special);
                continue;
            }
        }
        if (asteroid->activated == 0) {
            asteroid->activated = 1;
            if (asteroid->blowup != NULL) {
                asteroid->blowup->score = 0;
                Asteroid_AddParts(asteroid->blowup);
            }
        }
    }
}

/* Falcon spotlight phases. */
f32 spotLightA_yrot[2] = {0};
f32 spotLightA_zrot[2] = {0};
i16 id_PROBEDROID = -1;
i16 id_ATST_LOWRES = -1;
i16 id_BOBAFETT = -1;
i16 id_ATAT = -1;
i16 id_MILLENNIUMFALCON = -1;
i16 id_SPEEDERBIKESNOW = -1;
f32 spotLightB_yrot[2] = {0.5f, 0.5f};
f32 spotLightB_zrot[2] = {0.5f, 0.5f};
i8 melee_wavePhase = -1;

void DrawFalconSpotLights(GAMEOBJECT *object) {
    NUMTX matrix;
    if ((object->player_slot != 0 && object->player_slot != 1) || (i16)object->id != id_MILLENNIUMFALCON ||
        WORLD->level_objects[0x127].active == 0) {
        return;
    }
    if (object->character_model->points_of_interest[4] != NULL) {
        matrix = object->player.locator_matrices[4];
        NuSpecialDrawAt(&WORLD->level_objects[0x127].special, &matrix);
    }
    spotLightA_yrot[object->player_index] += FRAMETIME / 5.0f;
    if (spotLightA_yrot[object->player_index] > 1.0f) {
        spotLightA_yrot[object->player_index] -= 1.0f;
    }
    spotLightA_zrot[object->player_index] += FRAMETIME / 5.0f;
    if (spotLightA_zrot[object->player_index] > 1.0f) {
        spotLightA_zrot[object->player_index] -= 1.0f;
    }
    if (object->character_model->points_of_interest[5] != NULL) {
        matrix = object->player.locator_matrices[5];
        NuSpecialDrawAt(&WORLD->level_objects[0x127].special, &matrix);
    }
    spotLightB_yrot[object->player_index] += FRAMETIME / 5.0f;
    if (spotLightB_yrot[object->player_index] > 1.0f) {
        spotLightB_yrot[object->player_index] -= 1.0f;
    }
    spotLightB_zrot[object->player_index] += FRAMETIME / 5.0f;
    if (spotLightB_zrot[object->player_index] > 1.0f) {
        spotLightB_zrot[object->player_index] -= 1.0f;
    }
}

void AsteroidChaseA_Init(WORLDINFO *world) {
    AsteroidChase_FindPopBits(world);
}

void AsteroidChaseA_Reset(WORLDINFO *world) {
    Asteroids_Reset(world);
}

void AsteroidChaseA_Update(WORLDINFO *world) {
    Asteroids_Update();
}

GIZMOBLOWUP *classicBlowups[8] = {0};
u8 drawLights = 0;
u8 turretAliveCount = 0;

void AsteroidChaseB_Init(WORLDINFO *world) {
    GIZMOBLOWUP *blowup;
    i32 count;
    i32 i;
    AsteroidChase_FindPopBits(world);
    memset(classicBlowups, 0, sizeof(classicBlowups));
    count = 0;
    for (i = 0; i < world->gizmo_blowup_count && count < 8; i++) {
        blowup = &world->gizmo_blowups[i];
        if (NuStrIStr(blowup->name, "classic") != NULL) {
            classicBlowups[count++] = blowup;
        }
    }
    LevFlag.bytes[0] = 0;
    LevArea[0] = AISysFindArea(WORLD->ai_sys, "In_Cave");
    drawLights = 0;
    LevTime[0] = 0.0f;
    LevTime[1] = 0.0f;
    spotLightA_yrot[1] = 0.0f;
    spotLightA_yrot[0] = 0.0f;
    spotLightA_zrot[1] = 0.0f;
    spotLightA_zrot[0] = 0.0f;
    spotLightB_yrot[1] = 0.5f;
    spotLightB_yrot[0] = 0.5f;
    spotLightB_zrot[1] = 0.5f;
    spotLightB_zrot[0] = 0.5f;
}

void AsteroidChaseB_Reset(WORLDINFO *world) {
    Asteroids_Reset(world);
}

void AsteroidChaseB_Update(WORLDINFO *world) {
    i32 i;
    Asteroids_Update();
    for (i = 0; i < 4; i++) {
        if (classicBlowups[i] != NULL && (classicBlowups[i]->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0 &&
            (LevFlag.bytes[0] & (1 << i)) == 0) {
            LevFlag.bytes[0] |= (u8)(1 << i);
            AddMiscPickups(&classicBlowups[i]->position, -1, -1, 1);
        } else if (classicBlowups[i] != NULL && (classicBlowups[i]->status_flags & BLOWUP_STATUS_BLOWN_UP) == 0 &&
                   (LevFlag.bytes[0] & (1 << i)) != 0) {
            LevFlag.bytes[0] &= ~(u8)(1 << i);
        }
    }
    for (i = 0; i < 2; i++) {
        if (Player[i] != NULL && (i16)Player[i]->id == id_MILLENNIUMFALCON && LevArea[0] != NULL &&
            (*(u64 *)Player[i]->ai_area_mask & (u64)(i64)(1 << (LevArea[0] - world->ai_sys->areas))) != 0) {
            if (LevTime[i] >= 2.0f) {
                LevTime[i] = 2.0f;
                Player[i]->layer_mask |= 8;
                DrawFalconSpotLights(Player[i]);
                drawLights = 1;
            } else {
                LevTime[i] = FRAMETIME + LevTime[i];
            }
        } else if (Player[i] != NULL && (i16)Player[i]->id == id_MILLENNIUMFALCON) {
            if (LevTime[i] > 0.0f) {
                LevTime[i] -= FRAMETIME;
            } else {
                Player[i]->layer_mask &= ~8;
                drawLights = 0;
                LevTime[i] = 0.0f;
            }
        } else {
            LevTime[i] = 0.0f;
        }
    }
}

void AsteroidChaseB_Draw(WORLDINFO *world) {
    i32 i;
    for (i = 0; i < 2; i++) {
        if (drawLights != 0 && Player[i] != NULL) {
            DrawFalconSpotLights(Player[i]);
        }
    }
}

FINAL_ASTEROID finalAsteroid = {0};
ASTEROIDC_NETPACKET *asteroidc_netpacket = NULL;

void AsteroidChaseC_Init(WORLDINFO *world) {
    GIZMOBLOWUP *blowup;
    i32 i;
    asteroidc_netpacket = SetLevelHack(8);
    memset(&finalAsteroid, 0, sizeof(finalAsteroid));
    AsteroidChase_FindPopBits(world);
    NuSpecialFind(world->current_gscn, &finalAsteroid.special, "blockrock", 1);
    finalAsteroid.rotation_speed_x = (i16)(4551.0 - qrand() / (65535.0 / (4551.0 * 2.0) + 1.0));
    finalAsteroid.rotation_speed_y = (i16)(4551.0 - qrand() / (65535.0 / (4551.0 * 2.0) + 1.0));
    finalAsteroid.rotation_speed_z = (i16)(4551.0 - qrand() / (65535.0 / (4551.0 * 2.0) + 1.0));
    if (finalAsteroid.rotation_speed_x > -910 && finalAsteroid.rotation_speed_x < 910) {
        finalAsteroid.rotation_speed_x = 0x555;
    }
    if (finalAsteroid.rotation_speed_y > -910 && finalAsteroid.rotation_speed_y < 910) {
        finalAsteroid.rotation_speed_y = 0x555;
    }
    if (finalAsteroid.rotation_speed_z > -910 && finalAsteroid.rotation_speed_z < 910) {
        finalAsteroid.rotation_speed_z = 0x555;
    }
    for (i = 0; i < world->gizmo_blowup_count; i++) {
        if (finalAsteroid.blowup_count >= 8) {
            break;
        }
        blowup = &world->gizmo_blowups[i];
        if (NuStrIStr(blowup->name, "targ") != NULL) {
            finalAsteroid.blowups[finalAsteroid.blowup_count++] = blowup;
        }
    }
}

void AsteroidChaseC_Reset(WORLDINFO *world) {
    Asteroids_Reset(world);
}

NUMTX *mtxOrig = NULL;
f32 melee_waveDelay = 0;
i32 dagobah_training = 0;

i32 nethost = 0;

void AsteroidChaseC_Update(WORLDINFO *world) {
    NUMTX matrix;
    NUVEC position;
    i32 i;
    Asteroids_Update();
    if (netclient == 0) {
        finalAsteroid.rotation_x += (i16)(FRAMETIME * finalAsteroid.rotation_speed_x);
        finalAsteroid.rotation_y += (i16)(FRAMETIME * finalAsteroid.rotation_speed_y);
        finalAsteroid.rotation_z += (i16)(FRAMETIME * finalAsteroid.rotation_speed_z);
        if (nethost != 0) {
            asteroidc_netpacket->rotation_x = finalAsteroid.rotation_x;
            asteroidc_netpacket->rotation_y = finalAsteroid.rotation_y;
            asteroidc_netpacket->rotation_z = finalAsteroid.rotation_z;
        }
    } else {
        finalAsteroid.rotation_x = SeekRot(finalAsteroid.rotation_x, asteroidc_netpacket->rotation_x, 7.0f);
        finalAsteroid.rotation_y = SeekRot(finalAsteroid.rotation_y, asteroidc_netpacket->rotation_y, 7.0f);
        finalAsteroid.rotation_z = SeekRot(finalAsteroid.rotation_z, asteroidc_netpacket->rotation_z, 7.0f);
    }
    mtxOrig = NuSpecialGetMtx(&finalAsteroid.special);
    if (mtxOrig != NULL) {
        matrix = *mtxOrig;
        NuMtxGetTranslation(&matrix, &position);
        NuMtxRotateY(&matrix, finalAsteroid.rotation_y);
        NuMtxPreRotateX(&matrix, finalAsteroid.rotation_x);
        NuMtxPreRotateY(&matrix, finalAsteroid.rotation_z);
        matrix.m30 = position.x;
        matrix.m31 = position.y;
        matrix.m32 = position.z;
        NuSpecialSetDrawMtx(&finalAsteroid.special, &matrix);
        NuSpecialUpdate(&finalAsteroid.special);
    }
    for (i = 0; i < finalAsteroid.blowup_count; i++) {
        if (finalAsteroid.blowups[i] != NULL) {
            finalAsteroid.blowups[i]->model_rotation_x = finalAsteroid.rotation_x;
            finalAsteroid.blowups[i]->model_rotation_y = finalAsteroid.rotation_y;
            finalAsteroid.blowups[i]->model_rotation_z = finalAsteroid.rotation_z;
            finalAsteroid.blowups[i]->status_flags |= 0x10000;
            GizmoBlowupUpdateMatrix(finalAsteroid.blowups[i]);
        }
    }
}

i32 lastPlaying = 0;
nuhspecial_s specialIcon = {0};
HOTHBATTLE_MELEE_s melee = {0};
struct GIZTURRET_s *StarDestroyerTurrets[16] = {0};

void AsteroidChaseD_Init(WORLDINFO *world) {
    i16 targets = -1;
    char name[16];
    GIZMO *gizmo;
    i32 index;
    NuSpecialFind(vehicle_scene, &specialIcon, "Gun_Turret_icon", 1);
    turretAliveCount = 0;
    lastPlaying = 0;
    memset(escape, 0, sizeof(escape));
    memset(escape, 0, sizeof(escape) / 2);
    NuSpecialFind(WORLD->current_gscn, &escape[0], "rebelcruiser2", 0);
    NuSpecialFind(WORLD->current_gscn, &escape[1], "transport3", 0);
    NuSpecialFind(WORLD->current_gscn, &escape[2], "transport2", 0);
    NuSpecialFind(WORLD->current_gscn, &escape[4], "transport1", 0);
    NuSpecialFind(WORLD->current_gscn, &escape[5], "rebelcruiser1", 0);
    NuSpecialFind(WORLD->current_gscn, &escape[6], "transport4", 0);
    for (index = 0; index < 16; index++) {
        sprintf(name, "turret%d", index + 1);
        gizmo = GizmoFindByName(world->gizmo_sys, turret_gizmotype_id, name);
        if (gizmo != NULL) {
            StarDestroyerTurrets[index] = gizmo->object;
        }
        if (StarDestroyerTurrets[index] != NULL) {
            turretAliveCount++;
            StarDestroyerTurrets[index]->fire_interval = 0.5f;
        } else {
            StarDestroyerTurrets[index] = NULL;
        }
    }
    DrawMeleeTargetsNumber(&targets, &turretAliveCount, 1, 1, NULL);
    melee_wavePhase = 0;
}

void AsteroidChaseD_Update(WORLDINFO *unused0) {
    struct nuvec_s *position;
    i32 index;
    if (melee_wavePhase == 0 && MiniCutCam != 0) {
        melee_wavePhase = 1;
    }
    NuSpecialFind(WORLD->current_gscn, &escape[0], "rebelcruiser2", 0);
    NuSpecialFind(WORLD->current_gscn, &escape[1], "transport3", 0);
    NuSpecialFind(WORLD->current_gscn, &escape[2], "transport2", 0);
    NuSpecialFind(WORLD->current_gscn, &escape[4], "transport1", 0);
    NuSpecialFind(WORLD->current_gscn, &escape[5], "rebelcruiser1", 0);
    NuSpecialFind(WORLD->current_gscn, &escape[6], "transport4", 0);
    position = NuSpecialGetDrawPos(&escape[6]);
    for (index = 0; index < 2; ++index) {
        AsteroidChaseD_SetTurretTarget(index, position);
    }
    position = NuSpecialGetDrawPos(&escape[0]);
    for (index = 2; index < 6; ++index) {
        AsteroidChaseD_SetTurretTarget(index, position);
    }
    position = NuSpecialGetDrawPos(&escape[1]);
    for (index = 6; index < 8; ++index) {
        AsteroidChaseD_SetTurretTarget(index, position);
    }
    position = NuSpecialGetDrawPos(&escape[5]);
    for (index = 10; index < 14; ++index) {
        AsteroidChaseD_SetTurretTarget(index, position);
    }
    position = NuSpecialGetDrawPos(&escape[4]);
    for (index = 14; index < 16; ++index) {
        AsteroidChaseD_SetTurretTarget(index, position);
    }
    if (melee_wavePhase == 1 && MiniCutCam == 0) {
        i32 index;
        for (index = 0; index < 16; ++index) {
            if (StarDestroyerTurrets[index] != NULL) {
                StarDestroyerTurrets[index]->fire_interval = 2.0f;
            }
        }
        melee_wavePhase = 2;
    }
    for (index = 0; index < 16; ++index) {
        if (StarDestroyerTurrets[index] != NULL && (StarDestroyerTurrets[index]->flags & TURRET_FLAG_UPDATE_DISABLED) != 0) {
            --turretAliveCount;
            StarDestroyerTurrets[index] = NULL;
        }
    }
    if (melee_waveDelay > 0.0f) {
        melee_waveDelay -= FRAMETIME;
    }
    if (turretAliveCount == 0 && netclient == 0) {
        LevTime[0] += FRAMETIME;
        if (melee_waveDelay <= 0.0f && LevTime[0] >= 12.0f) {
            if (FreePlay == 0) {
                GoToNewLevel(ASTEROIDCHASEMITRO_LDATA->index);
            } else {
                GoToNewLevel(ASTEROIDCHASEA_LDATA->index);
            }
        }
    }
}

void AsteroidChaseD_Panel(WORLDINFO *world) {
    i16 targets = -1;
    DrawMeleeTargetsNumber(&targets, &turretAliveCount, 1, 0, &specialIcon);
}

void CloudCityTrapA_Init(WORLDINFO *world) {
    if (netclient == 0) {
        memset(troopercannons, 0, sizeof(troopercannons));
    }
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force3");
}

void CloudCityTrapA_Reset(WORLDINFO *world) {
    if (netclient == 0) {
        troopercannons_beenReset = 0;
    }
}

void CloudCityTrapA_Update(WORLDINFO *world) {
    struct gizmo_s *gizmo;
    static NUVEC pos = {33.0f, -0.75f, -3.0f};
    if (netclient == 0) {
        ResetTrooperCannons(world, id_SNOWTROOPER);
        UpdateTrooperCannons(world);
    }
    if (LevGameObject[0] == NULL) {
        LevGameObject[0] = FindGameObject(id_DARTHVADER, APIOBJECT_SIDE_BADDIE, 1, 1, 0);
    }
    if (netclient == 0) {
        if (LevAIMessage[0] != NULL && LevAIMessage[0]->value == 1.0f) {
            DrawBossHitPoints(LevGameObject[0]);
        } else {
            DrawBossHitPoints(NULL);
        }
    }
    gizmo = LevGizmo[0];
    if (gizmo != NULL && gizmo->object != NULL && gizmo->force->anim_set->state == GAMEANIMSET_STATE_AT_END) {
        AIAntinodeCreateSingleFrame(&pos, 0.5f);
    }
}

void CloudCityTrapB_Init(WORLDINFO *world) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    LevAIMessage[1] = CheckGizAIMessage(gizaimessagesys, "TrapBEndFight", NULL);
    LevGameObject[0] = FindGameObject(id_DARTHVADER, APIOBJECT_SIDE_BADDIE, 1, 1, 0);
}

void CloudCityTrapB_Update(WORLDINFO *unused0) {
    struct gameobject_s *vader;
    if (LevGameObject[0] == NULL) {
        LevGameObject[0] = FindGameObject(id_DARTHVADER, APIOBJECT_SIDE_BADDIE, 1, 1, 0);
    }
    if (netclient != 0) {
        return;
    }
    if (LevAIMessage[0] != NULL && LevAIMessage[0]->value == 1.0f) {
        DrawBossHitPoints(LevGameObject[0]);
    } else {
        DrawBossHitPoints(NULL);
    }
    if (LevAIMessage[1] == NULL || LevAIMessage[1]->value != 1.0f || LevGameObject[0] == NULL) {
        return;
    }
    vader = LevGameObject[0];
    if (vader->dead != 0 || vader->current_hp == 0) {
        if (FreePlay != 0) {
            CompleteLevel(WORLD);
        } else if (dat_0087b250 != NULL) {
            GoToNewLevel(dat_0087b250->index);
        }
    }
}

void CloudCityTrapC_Reset(WORLDINFO *world) {
    struct gizmo_s *gizmo;
    LevGameObject[0] = FindGameObject(id_DARTHVADER, APIOBJECT_SIDE_BADDIE, 1, 0, 0);
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    LevPathCnx[0] = AIPAthFindPathCnx(world->ai_sys, world->ai_sys->path_sys->active_path, "gap1_a", "gap1_b", &LevPathCnxDir);
    gizmo = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "obstacle3");
    LevGizmo[0] = gizmo;
    if (gizmo != NULL && gizmo->object != NULL) {
        LevGizObst[0] = gizmo->object;
    }
    gizmo = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "???");
    LevGizmo[1] = gizmo;
    if (gizmo != NULL && gizmo->object != NULL) {
        LevGizObst[1] = gizmo->object;
    }
}

i32 LEGO_AIPATHCNX_R2D2GLIDE = 0;
u32 LEGO_AIPATHCNX_BIGJUMP = 0;

void CloudCityTrapC_Update(WORLDINFO *world) {
    u32 active = 0;
    if (netclient != 0 || LevPathCnx[0] == NULL) {
        return;
    }
    LevPathCnx[0]->traversal_flags[LevPathCnxDir] &= ~(LEGO_AIPATHCNX_BIGJUMP | LEGO_AIPATHCNX_R2D2GLIDE | AIPATHCNX_TEMPORARY_BLOCK);
    if ((LevGizObst[1] != NULL && LevGizObst[1]->anim_set->state != GAMEANIMSET_STATE_AT_START && player->collision_position.z < -20.0f) ||
        player->collision_position.z < -21.0f) {
        active = LEGO_AIPATHCNX_BIGJUMP;
    }
    if (LevGizObst[0] != NULL && LevGizObst[0]->anim_set->state != GAMEANIMSET_STATE_AT_START) {
        active |= LEGO_AIPATHCNX_R2D2GLIDE;
    }
    if (active == 0) {
        active = 0x80000000;
    }
    LevPathCnx[0]->traversal_flags[LevPathCnxDir] |= active;
}

void CloudCityTrapC_Panel(WORLDINFO *world) {
    if (netclient == 0 && LevGameObject[0] != NULL && LevAIMessage[0] != NULL) {
        if (LevAIMessage[0]->value == 1.0f) {
            DrawBossHitPoints(LevGameObject[0]);
        } else {
            DrawBossHitPoints(NULL);
        }
    }
}

u8 aicreature_sets_alive[16] = {0};
GAMEOBJECT *LevGameObject[8] = {0};
struct panel_movement_s *LevGizPanel = NULL;
static const char CloudCityEscapePanelName[8] = "panel5";
static const char CloudCityEscapeGas3[8] = "gas_3";
static const char CloudCityEscapeGas2[8] = "gas_2";
static const char CloudCityEscapeGas1[8] = "gas_1";
static const char CloudCityEscapeFightMessage[20] = "BobaFightStarted";

void CloudCityEscapeA_Init(WORLDINFO *world) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, (char *)CloudCityEscapeFightMessage, NULL);
    LevFlag.cloud_city.started = 0;
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], (char *)CloudCityEscapeGas1, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], (char *)CloudCityEscapeGas2, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], (char *)CloudCityEscapeGas3, 1);
    LevGizPanel = GizPanel_FindByName(world, (char *)CloudCityEscapePanelName);
}

static const char CloudCityEscapeBuilditName[12] = "buildit2";
static const char CloudCityEscapeBuiltMessage[12] = "Built_C3PO";

void CloudCityEscapeA_Reset(WORLDINFO *world) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, (char *)CloudCityEscapeFightMessage, NULL);
    LevAIMessage[1] = CheckGizAIMessage(gizaimessagesys, (char *)CloudCityEscapeBuiltMessage, NULL);
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, gizbuildit_gizmotype_id, (char *)CloudCityEscapeBuilditName);
}

static const char CloudCityEscapeDeskSfx[20] = "env_ctrl_desk_on";
static const char CloudCityEscapeSteamSfx[16] = "env_steam_lp";

void CloudCityEscapeA_Update(WORLDINFO *world) {
    i32 i;
    NUINSTANIM *animation;
    if (LevFlag.cloud_city.started == 0 && NuSpecialGetVisibilityFn(&LevHSpecial[0]) != 0) {
        PlaySfx((char *)CloudCityEscapeDeskSfx, NuSpecialGetDrawPos(&LevHSpecial[0]));
        LevFlag.cloud_city.started = 1;
    }
    TerSurface[14].flags = 0x2002;
    if (LevGizPanel == NULL || (LevGizPanel->flags & 2) == 0) {
        for (i = 1; i <= 3; ++i) {
            if (NuSpecialExistsFn(&LevHSpecial[i])) {
                animation = NuSpecialGetInstAnim(&LevHSpecial[i]);
                if (animation != NULL && animation->ltime > 1.0f) {
                    PlaySfx((char *)CloudCityEscapeSteamSfx, NuSpecialGetDrawPos(&LevHSpecial[i]));
                    TerSurface[14].flags |= 0x4042;
                }
            }
        }
    }
    if (LevGizmo[0] != NULL && LevAIMessage[1] != NULL && LevAIMessage[1]->value == 0.0f && LevGizmo[0]->object != NULL &&
        LevGizmo[0]->buildit->build_state == GIZBUILDIT_BUILD_BUILT) {
        LevAIMessage[1]->value = 1.0f;
    }
}

void CloudCityEscapeA_Panel(WORLDINFO *world) {
    GAMEOBJECT *boba;
    if (netclient == 0) {
        boba = FindGameObject(id_BOBAFETT, APIOBJECT_SIDE_BADDIE, 1, 1, 0);
        if (boba != NULL && LevAIMessage[0] != NULL && LevAIMessage[0]->value == 1.0f) {
            DrawBossHitPoints(boba);
        } else if (LevAIMessage[0] != NULL && LevAIMessage[0]->value == 0.0f) {
            DrawBossHitPoints(NULL);
        }
    }
}

void CloudCityEscapeC_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "gas_1_animin", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "gas_2_animin", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "gas_3_animin", 1);
}

void CloudCityEscapeC_Update(WORLDINFO *world) {
    i32 i;
    TerSurface[14].flags = 0x2002;
    for (i = 0; i < 3; i++) {
        if (NuSpecialExistsFn(&LevHSpecial[i]) != 0) {
            NUINSTANIM *animation = NuSpecialGetInstAnim(&LevHSpecial[i]);
            if (animation != NULL && animation->ltime == 1.0f) {
                PlaySfx("env_steam_lp", NuSpecialGetDrawPos(&LevHSpecial[i]));
                TerSurface[14].flags |= 0x4042;
            }
        }
    }
}

i32 TakeOverYodaSeekDistanceHack(GAMEOBJECT *object, GAMEOBJECT *luke, NUVEC *offset) {
    if ((object->flags_ef4 & APIOBJECT_CAN_BE_CARRIED) == 0 || dagobah_training == 0 || object->player.rider != NULL || luke == NULL ||
        (luke->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 ||
        !((f64)NuVecDistSqr(&object->position, &luke->position, offset) < (f64)(0.7f * 0.7f))) {
        return 0;
    }
    NuVecRotateY(offset, offset, -luke->movement_angle);
    return offset->z > 0.0f;
}

void DagobahB_Init(WORLDINFO *world) {
    dagobah_training = 0;
}

void DagobahB_Reset(WORLDINFO *world) {
    LevSafePlatID[1] = -1;
    LevSafePlatID[0] = -1;

    if (NuSpecialFind(world->current_gscn, &LevHSpecial[0], "pad_2_base_2", 1) != 0) {
        if (world->terrain != NULL) {
            LevSafePlatID[0] = FindPlatInst(NuSpecialGetInstanceix(&LevHSpecial[0]));
        }
    }

    if (NuSpecialFind(world->current_gscn, &LevHSpecial[1], "pad_4_base_2", 1) != 0) {
        if (world->terrain != NULL) {
            LevSafePlatID[1] = FindPlatInst(NuSpecialGetInstanceix(&LevHSpecial[1]));
        }
    }
}

void DagobahE_Init(WORLDINFO *world) {
    GIZAIMESSAGE *completed = CheckGizAIMessage(gizaimessagesys, "CompletedTraining", NULL);
    DOOR_s *door;
    if (FreePlay == 0 && completed != NULL && completed->value == 0.0f) {
        dagobah_training = 1;
        door = Door_FindByName(world, "door_e_to_b");
        if (door != NULL) {
            door->flags |= DOOR_FLAG_DO_NOT_USE;
        }
        door = Door_FindByName(world, "door_b_to_e");
        if (door != NULL) {
            door->flags |= DOOR_FLAG_DO_NOT_USE;
        }
    } else {
        dagobah_training = 0;
        door = Door_FindByName(world, "door_e_to_b");
        if (door != NULL) {
            door->flags &= ~DOOR_FLAG_DO_NOT_USE;
        }
        door = Door_FindByName(world, "door_b_to_e");
        if (door != NULL) {
            door->flags &= ~DOOR_FLAG_DO_NOT_USE;
        }
    }
    SetGizAIMessage(gizaimessagesys, "DagobahTraining", (f32)dagobah_training, NULL);
    SetGizAIMessage(gizaimessagesys, NULL, 1.0f, completed);
}

void DagobahC_Init(WORLDINFO *world) {
    Vader_obj = FindGameObject(id_DARTHVADER, APIOBJECT_SIDE_BADDIE, 1, 0, 0);
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force20");
}

void DagobahC_Panel(WORLDINFO *world) {
    if (netclient == 0) {
        Vader_ai_message = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
        if (Vader_obj != NULL && Vader_ai_message != NULL && Vader_ai_message->value == 1.0f) {
            DrawBossHitPoints(Vader_obj);
        }
    }
}

void ResetMinikitCounter(void) {
    minikitCounter_C = 0;
    minikitCounter_A = 0;
}

void IncrementMinikitCounter(GAMEOBJECT *unused) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    NUVEC average;
    Players_AveragePos(&average, NULL);
    if (world->current_level == HOTHBATTLEA_LDATA) {
        {
            GIZMO *gizmo = LevGizmo[0];
            if (gizmo != NULL && gizmo->object != NULL) {
                GIZMOPICKUP *pickup = gizmo->pickup;
                if ((pickup->flags & 0x48) == 0 && minikitCounter_A < 10) {
                    ++minikitCounter_A;
                    if (minikitCounter_A == 10) {
                        NUVEC camera;
                        NUVEC message_pos;
                        camera.x = 0.0f;
                        camera.y = 0.0f;
                        camera.z = 10.0f;
                        NuVecRotateX(&camera, &camera, -0xe38);
                        NuVecRotateY(&camera, &camera, -0x2000);
                        NuVecAdd(&camera, &camera, &pickup->position);
                        GameCameraMakeMiniCut2(&camera, &pickup->position, 0, 0.0f, 4.0f, 0.0f, 0.0f, 0, 0, 0);
                        GizmoActivate(world->gizmo_sys, LevGizmo[0], 1, 1);
                        AddGameMsgCount(&message_pos, minikitCounter_A, 10, 200, 100, 30, 0.75f);
                    } else {
                        AddGameMsgCount(&average, minikitCounter_A, 10, 200, 100, 30, 0.75f);
                    }
                }
            }
        }
    } else if (world->current_level == dat_0087b204) {
        {
            GIZMO *gizmo = LevGizmo[1];
            if (gizmo != NULL && gizmo->object != NULL) {
                GIZMOPICKUP *pickup = gizmo->pickup;
                if ((pickup->flags & 0x48) == 0 && minikitCounter_C < 10) {
                    ++minikitCounter_C;
                    if (minikitCounter_C == 10) {
                        NUVEC camera;
                        NUVEC message_pos;
                        camera.x = 0.0f;
                        camera.y = 0.0f;
                        camera.z = 25.0f;
                        NuVecRotateX(&camera, &camera, -0x1555);
                        NuVecRotateY(&camera, &camera, -0x871c);
                        NuVecAdd(&camera, &camera, &pickup->position);
                        GameCameraMakeMiniCut2(&camera, &pickup->position, 0, 0.0f, 4.0f, 0.0f, 0.0f, 0, 0, 0);
                        GizmoActivate(world->gizmo_sys, LevGizmo[1], 1, 1);
                        AddGameMsgCount(&message_pos, minikitCounter_C, 10, 200, 100, 30, 0.75f);
                    } else {
                        AddGameMsgCount(&average, minikitCounter_C, 10, 200, 100, 30, 0.75f);
                    }
                }
            }
        }
    }
}

static const char HothBattleCGreenBolt[16] = "trooper_green";
static const char HothBattleAPickup[8] = "m_pup3";

void HothBattleA_Init(WORLDINFO *world) {
    i32 count;
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, gizmopickup_typeid, (char *)HothBattleAPickup);
    GizmoSetVisibility(world->gizmo_sys, LevGizmo[0], 0, 1);
    gunship_bolts[1] = BoltType_FindIDByName((char *)HothBattleCGreenBolt, world);
    gunship_bolts[0] = BoltType_FindIDByName("trooper_red", world);
    trooper_side[0] = 0;
    trooper_side[1] = 1;
    InitMiniSnowTroopers(world, 2, 32, 0);
    count = NuSpecialFind(world->current_gscn, &LevHSpecial[0], "minifig_1_1", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[1], "minifig_1_2", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[2], "minifig_1_3", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[3], "minifig_2_1", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[4], "minifig_2_2", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[5], "minifig_2_3", 1);
    if (count == 6) {
        hothtroopers = LevHSpecial;
    }
}

void HothBattleA_Reset(WORLDINFO *world) {
    GIZMO *gizmo = LevGizmo[0];
    GIZMOPICKUP *pickup;
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }
    pickup = gizmo->object;
    if (minikitCounter_A == 10 && (pickup->flags & GIZMOPICKUP_STATE_COLLECTED) == 0) {
        GizmoActivate(world->gizmo_sys, gizmo, 1, 1);
        return;
    }
    GizmoSetVisibility(world->gizmo_sys, gizmo, 0, 1);
}

void HothBattleA_Update(WORLDINFO *world) {
    UpdateMiniSnowTroopers(world);
}

void HothBattleA_Draw(WORLDINFO *world) {
    DrawMiniSnowTroopers(world);
}

void HothBattleB_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "snow_ball_1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "snow_ball_2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "snow_ball_3", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], "snow_ball_4", 1);
}

static const char HothBattleCIceB[8] = "ice_b";
static const char HothBattleCIceA[8] = "ice_a";
static const char HothBattleCObstacle[12] = "obstacle3";
static const char HothBattleCPickup[8] = "m_pup7";

void HothBattleC_Init(WORLDINFO *world) {
    i32 direction;
    i32 count;
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "BombGen_ATAT_Killed", NULL);
    LevPathCnx[0] = AIPAthFindPathCnx(world->ai_sys, world->ai_sys->path_sys->active_path, (char *)HothBattleCIceA, (char *)HothBattleCIceB,
                                      &direction);
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, (char *)HothBattleCObstacle);
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, gizmopickup_typeid, (char *)HothBattleCPickup);
    NuSpecialSetVisibility(&LevHSpecial[10], 0);
    gunship_bolts[1] = BoltType_FindIDByName((char *)HothBattleCGreenBolt, world);
    gunship_bolts[0] = BoltType_FindIDByName("trooper_red", world);
    trooper_side[0] = 0;
    trooper_side[1] = 1;
    InitMiniSnowTroopers(world, 2, 32, 0);
    count = NuSpecialFind(world->current_gscn, &LevHSpecial[0], "minifig_1_1", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[1], "minifig_1_2", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[2], "minifig_1_3", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[3], "minifig_2_1", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[4], "minifig_2_2", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[5], "minifig_2_3", 1);
    if (count == 6) {
        hothtroopers = LevHSpecial;
    }
}

void HothBattleC_Reset(WORLDINFO *world) {
    struct gizmo_s *gizmo;
    struct GIZMOPICKUP *pickup;
    GIZMO *bomb_generator = GizmoFindByName(world->gizmo_sys, bombgen_gizmotype_id, "bomb_generator1");
    if (bomb_generator != NULL && bomb_generator->object != NULL) {
        HothBattleC_BombGenerator = bomb_generator->object;
    }

    gizmo = LevGizmo[1];
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }

    pickup = gizmo->object;
    if (minikitCounter_C == 10 && (pickup->flags & GIZMOPICKUP_STATE_COLLECTED) == 0) {
        GizmoActivate(world->gizmo_sys, gizmo, 1, 1);
        return;
    }
    GizmoSetVisibility(world->gizmo_sys, gizmo, 0, 1);
}

void HothBattleC_Update(WORLDINFO *world) {
    if (netclient == 0 && HothBattleC_BombGenerator != NULL && !HothBattleC_BombGenerator->active && LevAIMessage[0] != NULL &&
        LevAIMessage[0]->value > 0.0f) {
        HothBattleC_BombGenerator->active = 1;
    }
    UpdateMiniSnowTroopers(world);
}

void HothBattleC_Draw(WORLDINFO *world) {
    DrawMiniSnowTroopers(world);
}

void fun_004e8d20(HOTHBATTLE_MELEE_s *battle) {
    if (battle != NULL) {
        battle->current_wave = 0;
        battle->next_wave = 0;
        battle->initialize_wave = 1;
        battle->transition_phase = -1;
        battle->wave_delay = 0.0f;
    }
}

void HothBattleE_Draw(WORLDINFO *world) {
    DrawMiniSnowTroopers(world);
}

f32 HothBattleEPanelAlpha[16] = {0};
u8 troopercannons_beenReset = 0;
HOTHBATTLEE_NETPACKET *HothBattleENetPacket = NULL;

void HothBattleE_Panel(WORLDINFO *world) {
    i16 targets[16];
    char defeated[16];
    i32 count;
    i32 index;
    if (MiniCutCam != 0) {
        return;
    }
    count = 0;
    if (netclient == 0) {
        switch (melee.current_wave) {
            case 1:
            case 2:
                for (index = 0; index < melee.waves[0].initial_count; ++index) {
                    if (index == (melee.waves[0].initial_count >> 1) + 1) {
                        targets[count] = -1;
                        defeated[count++] = 0;
                    }
                    defeated[count] = index >= melee.waves[0].remaining_count;
                    targets[count++] = melee.waves[0].character_id;
                }
                break;
            case 3:
                for (index = 0; index < melee.waves[0].initial_count; ++index) {
                    targets[count] = melee.waves[0].character_id;
                    defeated[count++] = index >= melee.waves[0].remaining_count;
                }
                break;
            case 4:
                targets[count] = melee.waves[2].character_id;
                defeated[count++] = melee.waves[2].remaining_count <= 0;
                for (index = 0; index < melee.waves[0].initial_count; ++index) {
                    targets[count] = melee.waves[0].character_id;
                    defeated[count++] = index >= melee.waves[0].remaining_count;
                }
                targets[count++] = -1;
                for (index = 0; index < melee.waves[1].initial_count; ++index) {
                    targets[count] = melee.waves[1].character_id;
                    defeated[count++] = index >= melee.waves[1].remaining_count;
                }
                break;
        }
        for (index = 0; index < count; ++index) {
            if (targets[index] != -1 && defeated[index] != 0) {
                HothBattleEPanelAlpha[index] = SeekLinearF(HothBattleEPanelAlpha[index], 0.4f, 0.1f);
            } else {
                HothBattleEPanelAlpha[index] = 1.0f;
            }
        }
        DrawMeleeTargetsRows(targets, defeated, HothBattleEPanelAlpha, count);
    } else {
        DrawMeleeTargetsRows(HothBattleENetPacket->targets, (char *)HothBattleENetPacket->defeated, HothBattleENetPacket->alpha,
                             HothBattleENetPacket->count);
    }
}

i32 fun_004e8fc0(GAMEOBJECT *object) {
    i32 wave;
    i32 i;
    for (wave = 0; wave < 4; ++wave) {
        for (i = 0; i < 4; ++i) {
            if (object == melee.waves[wave].creatures[i]) {
                return 1;
            }
        }
    }
    return 0;
}

AILOCATOR *getSpawnLocator(float clip_radius, char *name) {
    AILOCATOR *locators[32];
    i32 locator_count = 0;
    i32 i;
    AILOCATORSET *locator_set = AIPathFindLocatorSet(WORLD->ai_sys, name);
    if (locator_set != NULL) {
        AILocatorSet_CheckLocatorsStillAssigned(WORLD->ai_sys, locator_set);
        for (i = 0; i < locator_set->locator_count && locator_count < 32; ++i) {
            if (locator_set->assigned[i] == 0xff) {
                locators[locator_count++] = &WORLD->ai_sys->locators[locator_set->locator_entries[i]];
            }
        }
        return LocalGetRandomLocator(locators, locator_count, clip_radius, &player->collision_position, 1000000000.0f, 0, 1000000000.0f,
                                     1000000000.0f);
    }
    return NULL;
}

void HothBattle_ManageBackgroundCreatures(void) {
    u8 wave_types = 0;
    AILOCATORSET *set = AIPathFindLocatorSet(WORLD->ai_sys, "spawn");
    i32 type;
    i32 last;
    i32 index;
    f32 radius;
    AILOCATOR *spawn;
    AILOCATOR *entry;
    GAMEOBJECT *object;
    if (NOAICREATURES != 0) {
        return;
    }
    for (type = 0; type < melee.creature_count; ++type) {
        i16 id = melee.waves[type].character_id;
        if (id == id_PROBEDROID) {
            wave_types |= 1;
        } else if (id == id_SPEEDERBIKESNOW) {
            wave_types |= 2;
        } else if (id == id_ATST_LOWRES) {
            wave_types |= 4;
        } else if (id == id_ATAT) {
            wave_types |= 8;
        }
    }
    radius = apicharsys->char_data[id_PROBEDROID].collision_radius;
    while (aicreature_sets_alive[0] < 5 && (wave_types & 1) == 0) {
        spawn = getSpawnLocator(radius, "spawn");
        if (spawn == NULL) {
            return;
        }
        object =
            AddDynamicCreature(id_PROBEDROID, &spawn->position, spawn->rotation, "Probe", &spawn->path_info, NULL, 1, NULL, NULL, 0, 1);
        if (object == NULL) {
            break;
        }
        object->portal_flags |= APIOBJECT_CAN_SHOOT_OBSTRUCTIONS;
        for (index = 0; index < set->locator_count; ++index) {
            entry = &WORLD->ai_sys->locators[set->locator_entries[index]];
            if (entry == spawn) {
                object->ai_packet.locator = entry;
                set->assigned[index] = object->object_id;
                break;
            }
        }
    }
    last = -1;
    for (index = 5; index >= 0; --index) {
        object = melee.background_creatures[index];
        if (object == NULL) {
            continue;
        }
        if ((object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && object->dead == 0 && object->ai_packet.creature_set == 3) {
            if (last == -1) {
                last = index;
            }
        } else if (last == -1) {
            melee.background_creatures[index] = NULL;
        } else {
            melee.background_creatures[index] = melee.background_creatures[last];
            melee.background_creatures[last--] = NULL;
        }
    }
    radius = apicharsys->char_data[id_ATST_LOWRES].collision_radius * 2.0f;
    while (aicreature_sets_alive[2] < 6 && (wave_types & 4) == 0) {
        spawn = getSpawnLocator(radius, "spawn");
        if (spawn == NULL) {
            return;
        }
        object =
            AddDynamicCreature(id_ATST_LOWRES, &spawn->position, spawn->rotation, "rider", &spawn->path_info, NULL, 1, NULL, NULL, 0, 3);
        melee.background_creatures[++last] = object;
        if (object == NULL) {
            return;
        }
        object->portal_flags |= APIOBJECT_CAN_SHOOT_OBSTRUCTIONS;
        for (index = 0; index < set->locator_count; ++index) {
            entry = &WORLD->ai_sys->locators[set->locator_entries[index]];
            if (entry == spawn) {
                object->ai_packet.locator = entry;
                set->assigned[index] = object->object_id;
                break;
            }
        }
    }
}

__declspec(align(16)) KEEP_SCREEN_PLANE PlayPlane[6] = {0};

i32 SpawnMeleeCreatureType(i32 type) {
    i32 index;
    i32 slot;
    i32 attempts;
    f32 radius = apicharsys->char_data[melee.waves[type].character_id].collision_radius;
    AILOCATOR *locator;
    GAMEOBJECT *object;
    NUVEC offset;
    for (index = 0; index < melee.waves[type].active_count; ++index) {
        if (melee.waves[type].character_id != id_ATAT && melee.waves[type].creatures[index] != NULL &&
            melee.waves[type].creatures[index]->offscreen_time > 5.0f) {
            KillGameObject(melee.waves[type].creatures[index], KILL_REASON_REMOVED, 0);
            melee.waves[type].creatures[index] = NULL;
            --melee.waves[type].active_count;
        }
    }
    while (melee.waves[type].remaining_count > melee.waves[type].active_count) {
        if (melee.waves[type].active_count >= 4) {
            break;
        }
        slot = -1;
        for (index = 0; index < 4 && slot == -1; ++index) {
            if (melee.waves[type].creatures[index] == NULL) {
                slot = index;
            }
        }
        if (slot == -1) {
            return 0;
        }
        if (melee.waves[type].character_id == id_ATAT && melee.waves[type].active_count == 0) {
            object = GetNamedGameObject(WORLD->ai_sys, "ATAT_EDIT");
            melee.waves[type].creatures[slot] = object;
            if (object != NULL && object->dead == 0 && (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0) {
                ++melee.waves[type].active_count;
                continue;
            }
            melee.waves[type].creatures[slot] = NULL;
        }
        locator = NULL;
        for (attempts = 50; attempts > 0; --attempts) {
            locator = getSpawnLocator(5.0f, "spawn");
            if (locator != NULL) {
                if (OnOrInsidePlane(&locator->position, &PlayPlane[1].point, &PlayPlane[1].normal, NULL, 0.5f, NULL) == 0 ||
                    OnOrInsidePlane(&locator->position, &PlayPlane[2].point, &PlayPlane[2].normal, NULL, 0.5f, NULL) == 0) {
                    if (OnOrInsidePlane(&locator->position, &PlayPlane[1].point, &PlayPlane[1].normal, NULL, 2.5f, NULL) != 0 ||
                        OnOrInsidePlane(&locator->position, &PlayPlane[2].point, &PlayPlane[2].normal, NULL, 2.5f, NULL) != 0) {
                        break;
                    }
                }
                locator = NULL;
            }
        }
        if (locator == NULL) {
            return 0;
        }
        offset.y = offset.z = 0.0f;
        offset.x = (f32)(((f64)NuFloatRand(&GAMERAND) + 1.0) * radius * 2.0);
        NuVecRotateY(&offset, &offset, qrand());
        NuVecAdd(&offset, &offset, &locator->position);
        if (melee.waves[type].character_id == id_ATAT) {
            object = AddDynamicCreature(melee.waves[type].character_id, &locator->position, locator->rotation, melee.waves[type].name,
                                        &locator->path_info, NULL, 1, NULL, NULL, 0, 0);
        } else {
            object = AddDynamicCreature(melee.waves[type].character_id, &offset, locator->rotation, melee.waves[type].name,
                                        &locator->path_info, NULL, 1, NULL, NULL, 0, 0);
        }
        if (object == NULL) {
            return 0;
        }
        melee.waves[type].creatures[slot] = object;
        ++melee.waves[type].active_count;
        object->ai_packet.locator = locator;
    }
    return 1;
}

void HothBattleE_Init(WORLDINFO *world) {
    i32 count;
    gunship_bolts[1] = (i16)BoltType_FindIDByName("trooper_green", world);
    gunship_bolts[0] = (i16)BoltType_FindIDByName("trooper_red", world);
    trooper_side[0] = 0;
    trooper_side[1] = 0;
    trooper_side[2] = 0;
    trooper_side[3] = 0;
    trooper_side[4] = 0;
    trooper_side[5] = 1;
    trooper_side[6] = 1;
    trooper_side[7] = 1;
    trooper_side[8] = 1;
    trooper_side[9] = 1;
    InitMiniSnowTroopers(world, 10, 32, 0);
    memset(melee.waves, 0, sizeof(melee) - offsetof(HOTHBATTLE_MELEE_s, waves));
    melee.wave_delay = 0.0f;
    melee.current_wave = 0;
    melee.next_wave = 0;
    melee.initialize_wave = 1;
    melee.transition_phase = -1;
    count = NuSpecialFind(world->current_gscn, &LevHSpecial[0], "minifig_1_1", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[1], "minifig_1_2", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[2], "minifig_1_3", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[3], "minifig_2_1", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[4], "minifig_2_2", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[5], "minifig_2_3", 1);
    if (count == 6) {
        hothtroopers = LevHSpecial;
    }
    if (netclient == 0) {
        HothBattle_ManageBackgroundCreatures();
    }
    HothBattleENetPacket = SetLevelHack(0x58);
}

i32 HothBattle_StartNewWave() {
    i32 type;
    if (melee.restart != 0) {
        switch (melee.phase) {
            case 1:
                melee.waves[0].character_id = id_PROBEDROID;
                melee.waves[0].creature_type_a = 11;
                melee.waves[0].creature_type_b = 11;
                NuStrCpy(melee.waves[0].name, "Probe");
                melee.creature_count = 1;
                break;
            case 2:
                melee.waves[0].character_id = id_ATST_LOWRES;
                melee.waves[0].creature_type_a = 11;
                melee.waves[0].creature_type_b = 11;
                NuStrCpy(melee.waves[0].name, "rider");
                melee.creature_count = 1;
                break;
            case 3:
                melee.waves[0].character_id = id_ATAT;
                melee.waves[0].creature_type_a = 2;
                melee.waves[0].creature_type_b = 2;
                NuStrCpy(melee.waves[0].name, "ATAT");
                melee.creature_count = 1;
                break;
            case 4:
                melee.waves[0].character_id = id_PROBEDROID;
                melee.waves[0].creature_type_a = 4;
                melee.waves[0].creature_type_b = 4;
                NuStrCpy(melee.waves[0].name, "Probe");
                melee.waves[1].character_id = id_ATST_LOWRES;
                melee.waves[1].creature_type_a = 6;
                melee.waves[1].creature_type_b = 6;
                NuStrCpy(melee.waves[1].name, "rider");
                melee.waves[2].character_id = id_ATAT;
                melee.waves[2].creature_type_a = 1;
                melee.waves[2].creature_type_b = 1;
                NuStrCpy(melee.waves[2].name, "ATAT");
                melee.creature_count = 3;
                break;
        }
    }
    melee.restart = 0;
    if (MiniCutCam != 0) {
        return 0;
    }
    for (type = 0; type < melee.creature_count; ++type) {
        SpawnMeleeCreatureType(type);
    }
    return 1;
}

void HothBattleE_UpdateWave(void) {
    i32 pending = 0;
    i32 type;
    i32 wave_index;
    i32 index;
    char state[16];
    GAMEOBJECT *object;
    HOTH_ANTINODE *node;
    f32 radius;
    i32 count;
    if (melee.current_wave != melee.next_wave) {
        if (melee.transition_phase == -1) {
            if (melee.wave_delay < 5.0f && melee.next_wave != 1) {
                melee.wave_delay += FRAMETIME;
                return;
            }
            if (melee.next_wave == 5) {
                memset(melee.waves, 0, sizeof(melee) - offsetof(HOTHBATTLE_MELEE_s, waves));
                if (FreePlay != 0) {
                    CompleteLevel(WORLD);
                } else {
                    GoToNewLevel(dat_0087b210->index);
                }
            }
            melee.wave_delay = 0.0f;
            sprintf(state, "CamCut_%d", melee.next_wave);
            if (AIScriptSetBaseScriptStateByName(&((HOTH_WORLD_SCRIPT *)WORLD)->processor, state) != 0) {
                AIScriptProcess(WORLD->ai_sys, NULL, NULL, &((HOTH_WORLD_SCRIPT *)WORLD)->processor, FRAMETIME);
            }
            melee.transition_phase = 0;
            melee.wave_delay = 0.0f;
            melee.initialize_wave = 1;
            pending = 1;
        } else if (melee.transition_phase == 0) {
            if (MiniCutCam != 0) {
                melee.transition_phase = 1;
            }
        } else if (melee.transition_phase > 0) {
            if (MiniCutCam == 0) {
                if (melee.transition_phase == 1) {
                    object = Obj;
                    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
                        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 &&
                            (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && object->id != (u16)id_ATAT &&
                            object->player_index == -1 && (object->character_data->model_flags & CHARACTER_MODEL_BADDIE) != 0) {
                            KillGameObject(object, KILL_REASON_REMOVED, 0);
                        }
                    }
                    memset(melee.background_creatures, 0, sizeof(melee.background_creatures));
                    melee.transition_phase = 2;
                }
                if (HothBattle_StartNewWave() == 0) {
                    return;
                }
                melee.transition_phase = -1;
                melee.current_wave = melee.next_wave;
                pending = 1;
            }
        }
    }
    for (type = 0; type < melee.creature_count; ++type) {
        SpawnMeleeCreatureType(type);
    }
    HothBattle_ManageBackgroundCreatures();
    if (melee.current_wave == 3 && melee.waves[0].creatures[0] != NULL && melee.waves[0].creatures[1] != NULL) {
        radius = (f32)((f64)melee.waves[0].creatures[type]->bounds_radius * 3.0);
        node = (HOTH_ANTINODE *)AIAntinodeCreateSingleFrame(&melee.waves[0].creatures[0]->position, radius);
        node->excluded_character_types = (i32) ~(1u << melee.waves[0].creatures[1]->object_id);
        radius = (f32)((f64)melee.waves[0].creatures[type]->bounds_radius * 3.0);
        node = (HOTH_ANTINODE *)AIAntinodeCreateSingleFrame(&melee.waves[0].creatures[1]->position, radius);
        node->excluded_character_types = (i32) ~(1u << melee.waves[0].creatures[0]->object_id);
    }
    for (wave_index = 0; wave_index < 4; ++wave_index) {
        count = 0;
        for (index = 0; index < 4; ++index) {
            object = melee.waves[wave_index].creatures[index];
            if (object != NULL) {
                if ((object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && object->dead == 0) {
                    ++count;
                } else {
                    --melee.waves[wave_index].active_count;
                    --melee.waves[wave_index].remaining_count;
                    melee.waves[wave_index].creatures[index] = NULL;
                    PlaySfxAndSetPitch("TrueJedi_100pc", NULL, 1.5f);
                }
            }
        }
        if (melee.waves[wave_index].active_count != count) {
            melee.waves[wave_index].active_count = (u8)count;
        }
        if (melee.waves[wave_index].remaining_count > 0) {
            pending = 1;
        }
    }
    if (MiniCutCam == 0 && pending == 0 && NOAICREATURES == 0 && melee.next_wave == melee.current_wave) {
        ++melee.next_wave;
    }
}

void HothBattleE_Update(WORLDINFO *world) {
    UpdateMiniSnowTroopers(world);
    if (netclient == 0) {
        HothBattleE_UpdateWave();
    }
}
