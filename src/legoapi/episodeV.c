#include <stdio.h>
#include "saga.h"
#pragma data_seg(".data$rocket_lock_distance")
static f32 rocket_lock_distance = 1.0f;
#pragma data_seg()

/* Retail unit 0x4e5f10-0x4e9f30 (legoapi/world/levels/episodeV.cpp): definitions in retail address order. */
#include <string.h>

void __cdecl BobaRocket_Deflect(PART_PC *part) {
    part->flags = (part->flags & ~0x4000u) | 0x80;
    NewPartRotation(part);
}

void __cdecl BobaRocket_Kill(PART_PC *part, i32 reason) {
    EXPLOSION_PC *explosion = Detonate(&part->position, 0);
    GAMEOBJECT *owner;
    (void)reason;
    if (explosion == NULL || Arcade == 0) {
        return;
    }
    owner = part->owner;
    if (owner != NULL && (owner == Player[0] || owner == Player[1]) && (owner->flags_1fc_word & 1) != 0 &&
        (owner->flags_1fc_word & 0x1000) != 0 && (owner->field_280 == 0 || owner->field_280 == 1)) {
        explosion->flags |= 0x10000;
        explosion->owner = owner;
    }
}

u16 __cdecl SeekRot(u16 current, u16 target, f32 rate);
f32 __cdecl SeekValF(f32 current, f32 target, f32 rate);

void __cdecl BobaRocket_Move(PART_PC *part, f32 elapsed) {
    NUVEC delta;
    NUVEC next;
    NUVEC scale;
    GAMEOBJECT *recipient = part->recipient;
    if ((part->active & 0x80) != 0) {
        i32 yaw;
        i32 pitch;
        NuVecSub(&delta, &part->guided_target, &part->position);
        yaw = NuAtan2D(delta.x, delta.z);
        NuVecRotateY(&delta, &delta, -yaw);
        pitch = -NuAtan2D(delta.y, delta.z);
        part->rotation_x = SeekRot(part->rotation_x, (u16)pitch, 8.0f);
        part->rotation_y = SeekRot(part->rotation_y, (u16)yaw, 8.0f);
        part->field_124[3] = (i32)SeekValF((f32)part->field_124[3], 0.0f, 1.0f);
    } else {
        if (recipient != NULL) {
            f32 horizontal_squared;
            i32 yaw;
            i32 pitch;
            NuVecSub(&delta, &recipient->collision_position, &part->position);
            horizontal_squared = delta.x * delta.x + delta.z * delta.z;
            yaw = NuAtan2D(delta.x, delta.z);
            NuVecRotateY(&delta, &delta, -yaw);
            delta.y = (f32)((f64)recipient->collision_position.y + 0.5 - (f64)part->position.y);
            pitch = -NuAtan2D(delta.y, delta.z);
            part->rotation_x = SeekRot(part->rotation_x, (u16)pitch, 1.0f);
            part->rotation_y = SeekRot(part->rotation_y, (u16)yaw, 4.0f);
            if (horizontal_squared < rocket_lock_distance * rocket_lock_distance) {
                part->active |= 0x80;
                NuVecSub(&delta, &recipient->collision_position, &part->position);
                NuVecScale(&delta, &delta, 5.0f);
                NuVecAdd(&part->guided_target, &part->position, &delta);
                part->recipient = NULL;
                horizontal_squared += delta.y * delta.y;
                horizontal_squared = NuFsqrt(horizontal_squared);
                part->time_remaining = horizontal_squared / rocket_speed;
            }
        }
        part->field_124[3] = 60000;
    }
    part->rotation_z += (i32)((f32)part->field_124[3] * FRAMETIME);
    part->velocity.x = 0.0f;
    part->velocity.y = 0.0f;
    part->velocity.z = rocket_speed;
    NuVecRotateX(&part->velocity, &part->velocity, part->rotation_x);
    NuVecRotateY(&part->velocity, &part->velocity, part->rotation_y);
    next.x = part->velocity.x * elapsed + part->position.x;
    next.y = part->velocity.y * elapsed + part->position.y + part->gravity * elapsed;
    next.z = part->velocity.z * elapsed + part->position.z;
    NuMtxSetRotationX(&part->transform, NuAngAdd(part->rotation_x, 0x4000));
    NuMtxRotateY(&part->transform, part->rotation_y);
    NuMtxPreRotateY(&part->transform, part->rotation_z);
    NuMtxTranslate(&part->transform, &next);
    if (part->scale_time < (f64)0.2f) {
        scale.x = scale.y = scale.z = (f32)((f64)part->scale_time / (f64)0.2f);
        NuMtxPreScale(&part->transform, &scale);
    }
    if ((part->active & 0x80) != 0) {
        i32 count = ParticlesPerFrame(1.0f, FRAMETIME);
        delta.x = -part->velocity.x;
        delta.y = -part->velocity.y;
        delta.z = -part->velocity.z;
        AddGameDebrisMom(WORLD->game_debris_sys, 11, &next, count, &delta);
    }
}

/* Episode V FixUpLevels callbacks (Hoth escape, Dagobah, Cloud City trap). */
GAMEOBJECT *Vader_obj = NULL;
static GIZAIMESSAGE *Vader_ai_message = NULL;
AILOCATOR_PC *HothEscapeBLocatorPc = NULL;
GAMEOBJECT *HothEscapeBSnowmobPc = NULL;

extern i32 gizbuildit_gizmotype_id;
GIZMO_PC *__cdecl GizmoFindByData(void *, i32, void *);
i32 __cdecl GizBuildIt_AtEnd(void *);
void __cdecl GizBuildIt_KillParts(GIZBUILDIT_s *);
void __cdecl GizBuildit_SetVisibility(GIZBUILDIT_s *, i32);
struct panel_movement_pc_s;
struct panel_movement_pc_s *__cdecl GizPanel_FindByName(WORLDINFO *, char *);
typedef struct CLOUDCITY_PANEL_FLAGS_VIEW_PC {
    u8 unknown_00[0x62];
    u8 flags;
} CLOUDCITY_PANEL_FLAGS_VIEW_PC;
typedef struct CLOUDCITY_LEVEL_FLAG_VIEW_PC {
    u8 started;
} CLOUDCITY_LEVEL_FLAG_VIEW_PC;
extern i32 turret_gizmotype_id;
void __cdecl DrawMeleeTargetsNumber(i16 *targets, u8 *counts, i32 count, u8 reset, NUSPECIALHANDLE *special);
#define FALCON_SLOT(object) ((i8)(object)->field_280)
/* PC 0x00889660: asteroid field (original episodeV.cpp). */
typedef struct ASTEROID_PC {
    NUSPECIALHANDLE special;
    GIZMOBLOWUP_PC *blowup;
    i16 rotation_speed_x, rotation_speed_y, rotation_speed_z;
    u8 activated;
} ASTEROID_PC;
/* PC 0x0088a268: rotating rock at the end of AsteroidChase C. */
typedef struct FINAL_ASTEROID_PC {
    NUSPECIALHANDLE special;
    GIZMOBLOWUP_PC *blowups[8];
    i16 blowup_count;
    i16 rotation_speed_x, rotation_speed_y, rotation_speed_z;
    i16 rotation_x, rotation_y, rotation_z;
} FINAL_ASTEROID_PC;
typedef struct ASTEROIDC_NETPACKET_PC {
    u16 rotation_x, rotation_y, rotation_z;
} ASTEROIDC_NETPACKET_PC;
#define PLAYER_OBJ(i) ((GAMEOBJECT *)Player[i])
void __cdecl LevelStreaming_DoorOverride(WORLDINFO *world, LEVELDATA_PC *level, f32 radius, f32 *distance);
extern u32 LEGO_AIPATHCNX_BIGJUMP;
#define INCREMENT_MINIKIT_COUNTER_FOR(world, average, gizmo_index, counter, camera_distance, camera_pitch, camera_yaw) \
    do {                                                                                                               \
        GIZMO_PC *gizmo = LevGizmo[gizmo_index];                                                                       \
        if (gizmo != NULL && gizmo->object != NULL) {                                                                  \
            GIZMOPICKUP_PC *pickup = (GIZMOPICKUP_PC *)gizmo->object;                                                  \
            if ((pickup->flags & 0x48) == 0 && *(counter) < 10) {                                                      \
                ++*(counter);                                                                                          \
                if (*(counter) == 10) {                                                                                \
                    NUVEC camera;                                                                                      \
                    NUVEC message_pos;                                                                                 \
                    camera.x = 0.0f;                                                                                   \
                    camera.y = 0.0f;                                                                                   \
                    camera.z = camera_distance;                                                                        \
                    NuVecRotateX(&camera, &camera, camera_pitch);                                                      \
                    NuVecRotateY(&camera, &camera, camera_yaw);                                                        \
                    NuVecAdd(&camera, &camera, &pickup->position);                                                     \
                    GameCameraMakeMiniCut2(&camera, &pickup->position, 0, 0.0f, 4.0f, 0.0f, 0.0f, 0, 0, 0);            \
                    GizmoActivate((world)->gizmo_sys, LevGizmo[gizmo_index], 1, 1);                                    \
                    AddGameMsgCount(&message_pos, *(counter), 10, 200, 100, 30, 0.75f);                                \
                } else {                                                                                               \
                    AddGameMsgCount(average, *(counter), 10, 200, 100, 30, 0.75f);                                     \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)
/* PC level-script and antinode views retain the surrounding canonical layouts.
 */
typedef struct HOTH_WORLD_SCRIPT_PC {
    u8 reserved[0x2b00];
    AISCRIPTPROCESS_PC processor;
} HOTH_WORLD_SCRIPT_PC;
typedef struct HOTH_ANTINODE_PC {
    u8 reserved[0x50];
    i64 excluded_character_types;
} HOTH_ANTINODE_PC;
void __cdecl DrawMeleeTargetsRowsPc(i16 *targets, char *defeated, f32 *values, i32 count);

static __forceinline void AsteroidChaseD_SetTurretTarget(i32 index, NUVEC *position) {
    if (StarDestroyerTurrets[index] != NULL && StarDestroyerTurrets[index]->opponent == NULL) {
        StarDestroyerTurrets[index]->opponent = position;
        StarDestroyerTurrets[index]->opponent_type = 2;
    }
}
static __forceinline void AsteroidSetAngVec(NUANGVEC *out, NUANG x, NUANG y, NUANG z) {
    out->x = x;
    out->y = y;
    out->z = z;
}
TROOPERCANNON_PC troopercannons[4] = {0};

static void __cdecl KilledTrooperCannon(GAMEOBJECT *object) {
    i32 index;
    if (netclient != 0)
        return;
    for (index = 0; index < 4; ++index) {
        if (troopercannons[index].object == object) {
            DeactivateCharacter(troopercannons[index].character_name);
            GizBuildIt_SetToStart(troopercannons[index].buildit, 0, 0);
            GizmoActivate(WORLD->gizmo_sys,
                          GizmoFindByData(WORLD->gizmo_sys, gizbuildit_gizmotype_id, troopercannons[index].buildit), 1,
                          1);
            GizBuildit_SetVisibility(troopercannons[index].buildit, 1);
            troopercannons[index].rebuilding = 1;
            WORLD->level_progress->destroyed_trooper_cannon_mask |= 1u << index;
            return;
        }
    }
}

void __cdecl InitTrooperCannons(WORLDINFO *unused) {
    (void)unused;
    memset(troopercannons, 0, sizeof(troopercannons));
}
const char TrooperCannonBuilditFormatPc[17] = "trooper_cannon%d";
const char TrooperCannonCharacterFormatPc[17] = "troopercannon_%d";
const char TrooperCannonBaseFormatPc[7] = "BASE%d";
const char TrooperCannonAINamePc[14] = "CannonTrooper";

void __cdecl ResetTrooperCannons(WORLDINFO *world, i32 trooper_id) {
    i32 number, index = 0;
    char name[32];
    if (troopercannons_beenReset != 0 && (netclient == 0 || troopercannons[0].object != NULL))
        return;
    for (number = 1; number < 5; ++number) {
        TROOPERCANNON_PC *cannon = &troopercannons[index];
        GAMEOBJECT *trooper;
        memset(cannon, 0, sizeof(*cannon));
        sprintf(name, TrooperCannonBuilditFormatPc, number);
        cannon->buildit = (GIZBUILDIT_s *)GizBuildIt_Find(world, name);
        if (cannon->buildit != NULL) {
            sprintf(name, TrooperCannonCharacterFormatPc, number);
            cannon->object = GetNamedGameObject(world->ai_sys, name);
            if (cannon->object != NULL)
                NuStrCpy(cannon->character_name, name);
        }
        sprintf(name, TrooperCannonBaseFormatPc, number);
        cannon->base = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, name);
        if (netclient != 0) {
            if (cannon->object != NULL)
                ++index;
            continue;
        }
        if (cannon->buildit != NULL && cannon->object != NULL) {
            if ((world->level_progress->destroyed_trooper_cannon_mask & (1u << index)) == 0) {
                if (cannon->base != NULL) {
                    GizmoSetVisibility(world->gizmo_sys, cannon->base, 1, 1);
                    GizObstacle_PlayBackwards((GIZOBSTACLE *)cannon->base->object);
                }
                GizBuildIt_Finish(cannon->buildit);
                GizBuildit_SetVisibility(cannon->buildit, 0);
                cannon->rebuilding = 0;
                ActivateCharacter(name, NULL, 0);
                AddGameDebris(world->game_debris_sys, 0x5c, &cannon->object->collision_position);
                trooper = AddDynamicCreature(trooper_id, &cannon->object->collision_position,
                                             cannon->object->field_0x276, (char *)TrooperCannonAINamePc,
                                             &cannon->object->ai_packet.path_info, NULL, 0, NULL, NULL, 0, 0);
                if (trooper != NULL)
                    TakeOverGameObject(trooper, cannon->object, 0, 1);
            } else {
                GizBuildIt_SetToStart(cannon->buildit, 0, 0);
                cannon->rebuilding = 1;
            }
        }
        ++index;
    }
    troopercannons_beenReset = 1;
}

void __cdecl UpdateTrooperCannons(WORLDINFO *unused) {
    i32 index;
    (void)unused;
    for (index = 0; index < 4; ++index) {
        TROOPERCANNON_PC *cannon = &troopercannons[index];
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
        if (cannon->object == NULL || netclient != 0)
            break;
        if (cannon->object->rider != NULL) {
            if (cannon->object->rider->tag_object_callback == NULL)
                cannon->object->rider->tag_object_callback = KilledTrooperCannon;
        } else if (cannon->object->tag_object_callback == NULL) {
            cannon->object->tag_object_callback = KilledTrooperCannon;
        }
    }
}

void __cdecl HothEscapeA_Init(WORLDINFO *world) {
    memset(troopercannons, 0, sizeof(troopercannons));
    troopercannons_beenReset = 0;
}

void __cdecl HothEscapeA_Reset(WORLDINFO *world) {
}

void __cdecl HothEscapeA_Update(WORLDINFO *world) {
    ResetTrooperCannons(world, id_SNOWTROOPER);
    UpdateTrooperCannons(world);
}

void __cdecl HothEscapeB_Init(WORLDINFO *world) {
    if (netclient == 0) {
        HothEscapeBLocatorPc = AIPathFindLocator(world->ai_sys, "snow_mob");
        HothEscapeBSnowmobPc = GetNamedGameObject(world->ai_sys, "snowmob_1");
    }
    memset(troopercannons, 0, sizeof(troopercannons));
    troopercannons_beenReset = 0;
}

void __cdecl HothEscapeB_Reset(WORLDINFO *world) {
    HothEscapeBLocatorPc = AIPathFindLocator(world->ai_sys, "snow_mob");
    HothEscapeBSnowmobPc = GetNamedGameObject(world->ai_sys, "snowmob_1");
    TerSurface[9].movement_scale = TerSurface[17].movement_scale;
    TerSurface[9].flags = TerSurface[17].flags & ~2u;
}

void __cdecl HothEscapeB_Update(WORLDINFO *world) {
    ResetTrooperCannons(world, id_SNOWTROOPER);
    UpdateTrooperCannons(world);
    if (netclient == 0 && HothEscapeBLocatorPc != NULL && HothEscapeBSnowmobPc != NULL)
        HothEscapeBLocatorPc->position = HothEscapeBSnowmobPc->locator_matrices[1].translation;
}

void __cdecl HothEscapeC_Init(WORLDINFO *world) {
    memset(troopercannons, 0, sizeof(troopercannons));
    troopercannons_beenReset = 0;
}

void __cdecl HothEscapeC_Reset(WORLDINFO *world) {
}

void __cdecl HothEscapeC_Update(WORLDINFO *world) {
    ResetTrooperCannons(world, id_SNOWTROOPER);
    UpdateTrooperCannons(world);
}

void __cdecl HothEscapeC_AlwaysUpdate(WORLDINFO *world) {
    LevelStreaming_DoorOverride(world, LevelFixupSlotPc0087b220, 7.5f, NULL);
}

void __cdecl HothEscapeD_Init(WORLDINFO *world) {
    memset(troopercannons, 0, sizeof(troopercannons));
    troopercannons_beenReset = 0;
}

void __cdecl HothEscapeD_Reset(WORLDINFO *world) {
}

void __cdecl HothEscapeD_Update(WORLDINFO *world) {
    ResetTrooperCannons(world, id_SNOWTROOPER);
    UpdateTrooperCannons(world);
}

static void AsteroidChase_FindPopBits(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "small_pop_bit1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "small_pop_bit2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "small_pop_bit3", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], "small_pop_bit4", 1);
}

void Asteroid_PartKill(PART_PC *part, i32 reason) {
    AddGameDebris(WORLD->game_debris_sys, 0x5d, &part->position);
    AddGameDebris(WORLD->game_debris_sys, 0x5e, &part->position);
    AddGameDebris(WORLD->game_debris_sys, 0x5f, &part->position);
    AddPartDebris(WORLD->part_debris_sys, 4, &part->position);
    if (part->field_206 == 1) {
        i32 type;
        if (reason == 4)
            type = 0;
        else if (reason == 5)
            type = 1;
        else
            return;
        AddMiscPickups(&part->position, type, 1000, 0);
    } else {
        AddMiscPickups(&part->position, -1, 0, 1);
    }
}

void __cdecl Asteroid_AddParts(GIZMOBLOWUP_PC *blowup) {
    i32 special_indices[4] = {0, -1, -1, -1};
    i32 index;
    i32 part_count;
    NUSPECIALHANDLE *special;
    NUANGVEC rotation;
    NUMTX matrix;
    NUVEC velocity;
    NUVEC centre;
    ADDPART_PC params;
    PART_PC *part;
    part_count = qrand() / 0x4000 + 1;
    for (index = 1; index < part_count; index++)
        special_indices[index] = qrand() / (0xffff / 3 + 1) + 1;
    for (index = 0; index < part_count; index++) {
        special = &LevHSpecial[special_indices[index]];
        if (NuSpecialExistsFn(special) == 0)
            continue;
        AsteroidSetAngVec(&rotation, qrand(), qrand(), qrand());
        NuMtxSetRotateXYZVU0(&matrix, &rotation);
        NuMtxTranslate(&matrix, &blowup->position);
        velocity.x = velocity.y = 0.0f;
        velocity.z = (f32)(qrand() * PlayerRandomScalePc * NuTimeBarColumnStep + 2.0);
        NuVecRotateY(&velocity, &velocity, qrand());
        params = Default_ADDPART;
        params.matrix = &matrix;
        params.velocity = &velocity;
        NuSpecialGetRadius(special, &centre, &params.field_14);
        params.field_18 = params.field_14;
        params.gravity = 0.0f;
        params.special = special;
        params.flags = 0x8000193;
        if (index == 0)
            params.flags = 0x800019b;
        params.time_step = FRAMETIME;
        params.collision_fn = (PART_ACTION_PC)PartCollide_3D;
        params.kill_fn = (PART_KILL_PC)Asteroid_PartKill;
        params.field_a4 = (f32)(qrand() * PlayerRandomScalePc * NuTimeBarColumnStep + 7.0);
        part = AddPart(&params);
        if (part != NULL)
            part->field_206 = (index != 0) + 1;
    }
}
ASTEROID_PC asteroids[128] = {0};
i32 nasteroids = 0;
/* PC 0x00800290. */
i32 maxrotspd[3] = {0x1555, 0x38e, 0x16c};

void __cdecl Asteroids_Reset(WORLDINFO *world) {
    NUSPECIALHANDLE specials[128];
    ASTEROID_PC *asteroid;
    GIZMOBLOWUP_PC *blowup;
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
    if (count == 0)
        return;
    for (i = 0; i < count; i++) {
        for (j = 0; j < world->gizmo_blowup_type_count; j++)
            NuSpecialCompare(&world->gizmo_blowup_types[j].animated_special, &specials[i]);
        if (NuSpecialGetVisibilityFn(&specials[i]) != 0) {
            asteroid = &asteroids[nasteroids];
            asteroid->special = specials[i];
            name = NuSpecialGetName(&asteroid->special);
            type = 0;
            if (name != NULL) {
                if (NuStrIStr(name, "asteroid_a") != NULL)
                    type = 0;
                else if (NuStrIStr(name, "asteroid_pop") != NULL)
                    type = 0;
                else if (NuStrIStr(name, "asteroid_b") != NULL)
                    type = 1;
                else if (NuStrIStr(name, "asteroid_c") != NULL)
                    type = 2;
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
            if (NuStrIStr(name, "asteroid_a") != NULL)
                kind = 0;
            else if (NuStrIStr(name, "asteroid_pop") != NULL)
                kind = 0;
            else if (NuStrIStr(name, "asteroid_mid") != NULL)
                kind = 1;
            else
                continue;
        }
        asteroid->blowup = blowup;
        max_speed = maxrotspd[kind];
        asteroid->rotation_speed_x = (i16)(max_speed - qrand() / (0xffff / (max_speed * 2) + 1));
        asteroid->rotation_speed_y = (i16)(max_speed - qrand() / (0xffff / (max_speed * 2) + 1));
        asteroid->rotation_speed_z = (i16)(max_speed - qrand() / (0xffff / (max_speed * 2) + 1));
        nasteroids++;
    }
}

void __cdecl Asteroids_Update(void) {
    ASTEROID_PC *asteroid;
    NUMTX *matrix;
    i32 i;
    asteroid = asteroids;
    for (i = 0; i < nasteroids; i++, asteroid++) {
        if (asteroid->blowup != NULL) {
            if ((asteroid->blowup->status_flags & 0x4000) != 0 && (asteroid->blowup->status_flags & 1) == 0) {
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
/* PC 0x0088a554 / 0x0088a55c / 0x00800214 / 0x0080021c: Falcon spotlight phases. */
f32 spotLightA_yrot[2] = {0};
f32 spotLightA_zrot[2] = {0};
f32 spotLightB_yrot[2] = {0.5f, 0.5f};
f32 spotLightB_zrot[2] = {0.5f, 0.5f};

void __cdecl DrawFalconSpotLights(GAMEOBJECT *object) {
    NUMTX matrix;
    if ((object->field_280 != 0 && object->field_280 != 1) || (i16)object->id != VehicleBoltId7f19a4Pc ||
        WORLD->level_objects[0x127].active == 0)
        return;
    if (object->character_model->points_of_interest[4] != NULL) {
        matrix = object->locator_matrices[4];
        NuSpecialDrawAt(&WORLD->level_objects[0x127].special, &matrix);
    }
    spotLightA_yrot[FALCON_SLOT(object)] += FRAMETIME / 5.0f;
    if (spotLightA_yrot[FALCON_SLOT(object)] > 1.0f)
        spotLightA_yrot[FALCON_SLOT(object)] -= 1.0f;
    spotLightA_zrot[FALCON_SLOT(object)] += FRAMETIME / 5.0f;
    if (spotLightA_zrot[FALCON_SLOT(object)] > 1.0f)
        spotLightA_zrot[FALCON_SLOT(object)] -= 1.0f;
    if (object->character_model->points_of_interest[5] != NULL) {
        matrix = object->locator_matrices[5];
        NuSpecialDrawAt(&WORLD->level_objects[0x127].special, &matrix);
    }
    spotLightB_yrot[FALCON_SLOT(object)] += FRAMETIME / 5.0f;
    if (spotLightB_yrot[FALCON_SLOT(object)] > 1.0f)
        spotLightB_yrot[FALCON_SLOT(object)] -= 1.0f;
    spotLightB_zrot[FALCON_SLOT(object)] += FRAMETIME / 5.0f;
    if (spotLightB_zrot[FALCON_SLOT(object)] > 1.0f)
        spotLightB_zrot[FALCON_SLOT(object)] -= 1.0f;
}

void __cdecl AsteroidChaseA_Init(WORLDINFO *world) {
    AsteroidChase_FindPopBits(world);
}

void __cdecl AsteroidChaseA_Reset(WORLDINFO *world) {
    Asteroids_Reset(world);
}

void __cdecl AsteroidChaseA_Update(WORLDINFO *world) {
    Asteroids_Update();
}
GIZMOBLOWUP_PC *classicBlowups[8] = {0};
u8 drawLights = 0;

void __cdecl AsteroidChaseB_Init(WORLDINFO *world) {
    GIZMOBLOWUP_PC *blowup;
    i32 count;
    i32 i;
    AsteroidChase_FindPopBits(world);
    memset(classicBlowups, 0, sizeof(classicBlowups));
    count = 0;
    for (i = 0; i < world->gizmo_blowup_count && count < 8; i++) {
        blowup = &world->gizmo_blowups[i];
        if (NuStrIStr(blowup->name, "classic") != NULL)
            classicBlowups[count++] = blowup;
    }
    ((u8 *)LevFlag)[0] = 0;
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

void __cdecl AsteroidChaseB_Reset(WORLDINFO *world) {
    Asteroids_Reset(world);
}

void __cdecl AsteroidChaseB_Update(WORLDINFO *world) {
    i32 i;
    Asteroids_Update();
    for (i = 0; i < 4; i++) {
        if (classicBlowups[i] != NULL && (classicBlowups[i]->status_flags & 1) != 0 &&
            (((u8 *)LevFlag)[0] & (1 << i)) == 0) {
            ((u8 *)LevFlag)[0] |= (u8)(1 << i);
            AddMiscPickups(&classicBlowups[i]->position, -1, -1, 1);
        } else if (classicBlowups[i] != NULL && (classicBlowups[i]->status_flags & 1) == 0 &&
                   (((u8 *)LevFlag)[0] & (1 << i)) != 0) {
            ((u8 *)LevFlag)[0] &= ~(u8)(1 << i);
        }
    }
    for (i = 0; i < 2; i++) {
        if (PLAYER_OBJ(i) != NULL && (i16)PLAYER_OBJ(i)->id == VehicleBoltId7f19a4Pc && LevArea[0] != NULL &&
            (*(u64 *)PLAYER_OBJ(i)->ai_area_mask & (u64)(i64)(1 << (LevArea[0] - world->ai_sys->areas))) != 0) {
            if (LevTime[i] >= 2.0f) {
                LevTime[i] = 2.0f;
                PLAYER_OBJ(i)->layer_mask |= 8;
                DrawFalconSpotLights(Player[i]);
                drawLights = 1;
            } else {
                LevTime[i] = FRAMETIME + LevTime[i];
            }
        } else if (PLAYER_OBJ(i) != NULL && (i16)PLAYER_OBJ(i)->id == VehicleBoltId7f19a4Pc) {
            if (LevTime[i] > 0.0f) {
                LevTime[i] -= FRAMETIME;
            } else {
                PLAYER_OBJ(i)->layer_mask &= ~8;
                drawLights = 0;
                LevTime[i] = 0.0f;
            }
        } else {
            LevTime[i] = 0.0f;
        }
    }
}

void __cdecl AsteroidChaseB_Draw(WORLDINFO *world) {
    i32 i;
    for (i = 0; i < 2; i++) {
        if (drawLights != 0 && Player[i] != NULL)
            DrawFalconSpotLights(Player[i]);
    }
}
FINAL_ASTEROID_PC finalAsteroid = {0};
ASTEROIDC_NETPACKET_PC *asteroidc_netpacket = NULL;

void __cdecl AsteroidChaseC_Init(WORLDINFO *world) {
    GIZMOBLOWUP_PC *blowup;
    i32 i;
    asteroidc_netpacket = (ASTEROIDC_NETPACKET_PC *)SetLevelHack(8);
    memset(&finalAsteroid, 0, sizeof(finalAsteroid));
    AsteroidChase_FindPopBits(world);
    NuSpecialFind(world->current_gscn, &finalAsteroid.special, "blockrock", 1);
    finalAsteroid.rotation_speed_x = (i16)(4551.0 - qrand() / (65535.0 / (4551.0 * 2.0) + 1.0));
    finalAsteroid.rotation_speed_y = (i16)(4551.0 - qrand() / (65535.0 / (4551.0 * 2.0) + 1.0));
    finalAsteroid.rotation_speed_z = (i16)(4551.0 - qrand() / (65535.0 / (4551.0 * 2.0) + 1.0));
    if (finalAsteroid.rotation_speed_x > -910 && finalAsteroid.rotation_speed_x < 910)
        finalAsteroid.rotation_speed_x = 0x555;
    if (finalAsteroid.rotation_speed_y > -910 && finalAsteroid.rotation_speed_y < 910)
        finalAsteroid.rotation_speed_y = 0x555;
    if (finalAsteroid.rotation_speed_z > -910 && finalAsteroid.rotation_speed_z < 910)
        finalAsteroid.rotation_speed_z = 0x555;
    for (i = 0; i < world->gizmo_blowup_count; i++) {
        if (finalAsteroid.blowup_count >= 8)
            break;
        blowup = &world->gizmo_blowups[i];
        if (NuStrIStr(blowup->name, "targ") != NULL)
            finalAsteroid.blowups[finalAsteroid.blowup_count++] = blowup;
    }
}

void __cdecl AsteroidChaseC_Reset(WORLDINFO *world) {
    Asteroids_Reset(world);
}
NUMTX *mtxOrig = NULL;

void __cdecl AsteroidChaseC_Update(WORLDINFO *world) {
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
        NuMtxRotateY(&matrix, (i16)finalAsteroid.rotation_y);
        NuMtxPreRotateX(&matrix, (i16)finalAsteroid.rotation_x);
        NuMtxPreRotateY(&matrix, (i16)finalAsteroid.rotation_z);
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
/* PC 0x0088a310 / 0x0088a314. */
i32 lastPlaying = 0;
NUSPECIALHANDLE specialIcon = {0};

void __cdecl AsteroidChaseD_Init(WORLDINFO *world) {
    i16 targets = -1;
    char name[16];
    GIZMO_PC *gizmo;
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
        gizmo = (GIZMO_PC *)GizmoFindByName(world->gizmo_sys, turret_gizmotype_id, name);
        if (gizmo != NULL)
            StarDestroyerTurrets[index] = (GIZTURRET_s *)gizmo->object;
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
        if (StarDestroyerTurrets[index] != NULL && (StarDestroyerTurrets[index]->flags & 0x20) != 0) {
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

void __cdecl AsteroidChaseD_Panel(WORLDINFO *world) {
    i16 targets = -1;
    DrawMeleeTargetsNumber(&targets, &turretAliveCount, 1, 0, &specialIcon);
}
static const char CloudCityTrapAForceNamePc[7] = "force3";

void __cdecl CloudCityTrapA_Init(WORLDINFO *world) {
    if (netclient == 0)
        memset(troopercannons, 0, sizeof(troopercannons));
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, EpisodeVI_ShowHearts, NULL);
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, (char *)CloudCityTrapAForceNamePc);
}

void __cdecl CloudCityTrapA_Reset(WORLDINFO *world) {
    if (netclient == 0)
        troopercannons_beenReset = 0;
}

void CloudCityTrapA_Update(WORLDINFO *world) {
    struct gizmo_pc_s *gizmo;
    static NUVEC pos = {33.0f, -0.75f, -3.0f};
    if (netclient == 0) {
        ResetTrooperCannons(world, id_SNOWTROOPER);
        UpdateTrooperCannons(world);
    }
    if (LevGameObject[0] == NULL)
        LevGameObject[0] = FindGameObject(FreePlayId007f199cPc, 1, 1, 1, 0);
    if (netclient == 0) {
        if (LevAIMessage[0] != NULL && LevAIMessage[0]->value == 1.0f)
            DrawBossHitPoints(LevGameObject[0]);
        else
            DrawBossHitPoints(NULL);
    }
    gizmo = LevGizmo[0];
    if (gizmo != NULL && gizmo->object != NULL &&
        ((GIZFORCE_s *)(gizmo->object))->anim_set->state == GAMEANIMSET_STATE_AT_END)
        AIAntinodeCreateSingleFrame(&pos, 0.5f);
}

void __cdecl CloudCityTrapB_Init(WORLDINFO *world) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    LevAIMessage[1] = CheckGizAIMessage(gizaimessagesys, "TrapBEndFight", NULL);
    LevGameObject[0] = FindGameObject(FreePlayId007f199cPc, 1, 1, 1, 0);
}

void CloudCityTrapB_Update(WORLDINFO *unused0) {
    struct gameobject_s *vader;
    if (LevGameObject[0] == NULL)
        LevGameObject[0] = FindGameObject(FreePlayId007f199cPc, 1, 1, 1, 0);
    if (netclient != 0)
        return;
    if (LevAIMessage[0] != NULL && LevAIMessage[0]->value == 1.0f)
        DrawBossHitPoints(LevGameObject[0]);
    else
        DrawBossHitPoints(NULL);
    if (LevAIMessage[1] == NULL || LevAIMessage[1]->value != 1.0f || LevGameObject[0] == NULL)
        return;
    vader = LevGameObject[0];
    if (vader->dead != 0 || vader->current_hp == 0) {
        if (FreePlay != 0)
            CompleteLevel(WORLD);
        else if (LevelFixupSlotPc0087b250 != NULL)
            GoToNewLevel(LevelFixupSlotPc0087b250->index);
    }
}

void CloudCityTrapC_Reset(WORLDINFO *world) {
    struct gizmo_pc_s *gizmo;
    LevGameObject[0] = FindGameObject(FreePlayId007f199cPc, 1, 1, 0, 0);
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    LevPathCnx[0] =
        AIPAthFindPathCnx(world->ai_sys, world->ai_sys->path_sys->active_path, "gap1_a", "gap1_b", &LevPathCnxDir);
    gizmo = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "obstacle3");
    LevGizmo[0] = gizmo;
    if (gizmo != NULL && gizmo->object != NULL)
        LevGizObst[0] = ((GIZOBSTACLE *)(gizmo->object));
    gizmo = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "???");
    LevGizmo[1] = gizmo;
    if (gizmo != NULL && gizmo->object != NULL)
        LevGizObst[1] = ((GIZOBSTACLE *)(gizmo->object));
}

void __cdecl CloudCityTrapC_Update(WORLDINFO *world) {
    u32 active = 0;
    if (netclient != 0 || LevPathCnx[0] == NULL)
        return;
    ((AIPATHCNX_PC *)LevPathCnx[0])->traversal_flags[LevPathCnxDir] &=
        ~(LEGO_AIPATHCNX_BIGJUMP | LEGO_AIPATHCNX_R2D2GLIDE | 0x80000000);
    if ((LevGizObst[1] != NULL && LevGizObst[1]->anim_set->state != 0 && player->collision_position.z < -20.0f) ||
        player->collision_position.z < -21.0f)
        active = LEGO_AIPATHCNX_BIGJUMP;
    if (LevGizObst[0] != NULL && LevGizObst[0]->anim_set->state != 0)
        active |= LEGO_AIPATHCNX_R2D2GLIDE;
    if (active == 0)
        active = 0x80000000;
    ((AIPATHCNX_PC *)LevPathCnx[0])->traversal_flags[LevPathCnxDir] |= active;
}

void __cdecl CloudCityTrapC_Panel(WORLDINFO *world) {
    if (netclient == 0 && LevGameObject[0] != NULL && LevAIMessage[0] != NULL) {
        if (LevAIMessage[0]->value == 1.0f)
            DrawBossHitPoints(LevGameObject[0]);
        else
            DrawBossHitPoints(NULL);
    }
}
struct panel_movement_pc_s *LevGizPanel = NULL;
static const char CloudCityEscapePanelNamePc[8] = "panel5";
static const char CloudCityEscapeGas3Pc[8] = "gas_3";
static const char CloudCityEscapeGas2Pc[8] = "gas_2";
static const char CloudCityEscapeGas1Pc[8] = "gas_1";
static const char CloudCityEscapeFightMessagePc[20] = "BobaFightStarted";

void __cdecl CloudCityEscapeA_Init(WORLDINFO *world) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, (char *)CloudCityEscapeFightMessagePc, NULL);
    ((CLOUDCITY_LEVEL_FLAG_VIEW_PC *)LevFlag)->started = 0;
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], (char *)CloudCityEscapeGas1Pc, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], (char *)CloudCityEscapeGas2Pc, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], (char *)CloudCityEscapeGas3Pc, 1);
    LevGizPanel = GizPanel_FindByName(world, (char *)CloudCityEscapePanelNamePc);
}
static const char CloudCityEscapeBuilditNamePc[12] = "buildit2";
static const char CloudCityEscapeBuiltMessagePc[12] = "Built_C3PO";

void __cdecl CloudCityEscapeA_Reset(WORLDINFO *world) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, (char *)CloudCityEscapeFightMessagePc, NULL);
    LevAIMessage[1] = CheckGizAIMessage(gizaimessagesys, (char *)CloudCityEscapeBuiltMessagePc, NULL);
    LevGizmo[0] =
        (GIZMO_PC *)GizmoFindByName(world->gizmo_sys, gizbuildit_gizmotype_id, (char *)CloudCityEscapeBuilditNamePc);
}
static const char CloudCityEscapeDeskSfxPc[20] = "env_ctrl_desk_on";
static const char CloudCityEscapeSteamSfxPc[16] = "env_steam_lp";

void __cdecl CloudCityEscapeA_Update(WORLDINFO *world) {
    i32 i;
    NUINSTANIM *animation;
    if (((CLOUDCITY_LEVEL_FLAG_VIEW_PC *)LevFlag)->started == 0 && NuSpecialGetVisibilityFn(&LevHSpecial[0]) != 0) {
        PlaySfx((char *)CloudCityEscapeDeskSfxPc, NuSpecialGetDrawPos(&LevHSpecial[0]));
        ((CLOUDCITY_LEVEL_FLAG_VIEW_PC *)LevFlag)->started = 1;
    }
    TerSurface[14].flags = 0x2002;
    if (LevGizPanel == NULL || (((CLOUDCITY_PANEL_FLAGS_VIEW_PC *)LevGizPanel)->flags & 2) == 0) {
        for (i = 1; i <= 3; ++i) {
            if (NuSpecialExistsFn(&LevHSpecial[i])) {
                animation = (NUINSTANIM *)NuSpecialGetInstAnim(&LevHSpecial[i]);
                if (animation != NULL && animation->ltime > 1.0f) {
                    PlaySfx((char *)CloudCityEscapeSteamSfxPc, NuSpecialGetDrawPos(&LevHSpecial[i]));
                    TerSurface[14].flags |= 0x4042;
                }
            }
        }
    }
    if (LevGizmo[0] != NULL && LevAIMessage[1] != NULL && LevAIMessage[1]->value == 0.0f &&
        LevGizmo[0]->object != NULL && ((GIZBUILDIT_PC *)LevGizmo[0]->object)->build_state == 2)
        LevAIMessage[1]->value = 1.0f;
}

void __cdecl CloudCityEscapeA_Panel(WORLDINFO *world) {
    GAMEOBJECT *boba;
    if (netclient == 0) {
        boba = FindGameObject(id_BOBAFETT, 1, 1, 1, 0);
        if (boba != NULL && LevAIMessage[0] != NULL && LevAIMessage[0]->value == 1.0f)
            DrawBossHitPoints(boba);
        else if (LevAIMessage[0] != NULL && LevAIMessage[0]->value == 0.0f)
            DrawBossHitPoints(NULL);
    }
}

void CloudCityEscapeC_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "gas_1_animin", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "gas_2_animin", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "gas_3_animin", 1);
}

void __cdecl CloudCityEscapeC_Update(WORLDINFO *world) {
    i32 i;
    TerSurface[14].flags = 0x2002;
    for (i = 0; i < 3; i++) {
        if (NuSpecialExistsFn(&LevHSpecial[i]) != 0) {
            NUINSTANIM *animation = (NUINSTANIM *)NuSpecialGetInstAnim(&LevHSpecial[i]);
            if (animation != NULL && animation->ltime == 1.0f) {
                PlaySfx("env_steam_lp", NuSpecialGetDrawPos(&LevHSpecial[i]));
                TerSurface[14].flags |= 0x4042;
            }
        }
    }
}

i32 __cdecl TakeOverYodaSeekDistanceHack(GAMEOBJECT *object, GAMEOBJECT *luke, NUVEC *offset) {
    if ((object->flags_ef4 & 2) == 0 || dagobah_training == 0 || object->rider != NULL || luke == NULL ||
        (luke->flags_1fc_word & 0x80) == 0 ||
        !((f64)NuVecDistSqr(&object->position, &luke->position, offset) < (f64)(0.7f * 0.7f)))
        return 0;
    NuVecRotateY(offset, offset, -(i32)luke->field_0x276);
    return offset->z > 0.0f;
}

void __cdecl DagobahB_Init(WORLDINFO *world) {
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

void __cdecl DagobahE_Init(WORLDINFO *world) {
    GIZAIMESSAGE *completed = CheckGizAIMessage(gizaimessagesys, "CompletedTraining", NULL);
    DOOR_s *door;
    if (FreePlay == 0 && completed != NULL && completed->value == 0.0f) {
        dagobah_training = 1;
        door = (DOOR_s *)Door_FindByName(world, "door_e_to_b");
        if (door != NULL)
            door->flags |= 4;
        door = (DOOR_s *)Door_FindByName(world, "door_b_to_e");
        if (door != NULL)
            door->flags |= 4;
    } else {
        dagobah_training = 0;
        door = (DOOR_s *)Door_FindByName(world, "door_e_to_b");
        if (door != NULL)
            door->flags &= ~4;
        door = (DOOR_s *)Door_FindByName(world, "door_b_to_e");
        if (door != NULL)
            door->flags &= ~4;
    }
    SetGizAIMessage(gizaimessagesys, "DagobahTraining", (f32)dagobah_training, NULL);
    SetGizAIMessage(gizaimessagesys, NULL, 1.0f, completed);
}

void __cdecl DagobahC_Init(WORLDINFO *world) {
    Vader_obj = FindGameObject(FreePlayId007f199cPc, 1, 1, 0, 0);
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force20");
}

void __cdecl DagobahC_Panel(WORLDINFO *world) {
    if (netclient == 0) {
        Vader_ai_message = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
        if (Vader_obj != NULL && Vader_ai_message != NULL && Vader_ai_message->value == 1.0f)
            DrawBossHitPoints(Vader_obj);
    }
}

void __cdecl ResetMinikitCounter(void) {
    minikitCounter_C = 0;
    minikitCounter_A = 0;
}

void __cdecl IncrementMinikitCounter(GAMEOBJECT *unused) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    NUVEC average;
    Players_AveragePos(&average, NULL);
    if (world->current_level == HOTHBATTLEA_LDATA)
        INCREMENT_MINIKIT_COUNTER_FOR(world, &average, 0, &minikitCounter_A, 10.0f, -0xe38, -0x2000);
    else if (world->current_level == LevelFixupSlotPc0087b204)
        INCREMENT_MINIKIT_COUNTER_FOR(world, &average, 1, &minikitCounter_C, 25.0f, -0x1555, -0x871c);
}
static const char HothBattleCGreenBoltPc[16] = "trooper_green";
static const char HothBattleCRedBoltPc[12] = "trooper_red";
static const char HothBattleCMinifig11Pc[12] = "minifig_1_1";
static const char HothBattleCMinifig12Pc[12] = "minifig_1_2";
static const char HothBattleCMinifig13Pc[12] = "minifig_1_3";
static const char HothBattleCMinifig21Pc[12] = "minifig_2_1";
static const char HothBattleCMinifig22Pc[12] = "minifig_2_2";
static const char HothBattleCMinifig23Pc[12] = "minifig_2_3";
static const char HothBattleAPickupPc[8] = "m_pup3";

void __cdecl HothBattleA_Init(WORLDINFO *world) {
    i32 count;
    LevGizmo[0] = (GIZMO_PC *)GizmoFindByName(world->gizmo_sys, gizmopickup_typeid, (char *)HothBattleAPickupPc);
    GizmoSetVisibility(world->gizmo_sys, LevGizmo[0], 0, 1);
    gunship_bolts[1] = BoltType_FindIDByName((char *)HothBattleCGreenBoltPc, world);
    gunship_bolts[0] = BoltType_FindIDByName((char *)HothBattleCRedBoltPc, world);
    trooper_side[0] = 0;
    trooper_side[1] = 1;
    InitMiniSnowTroopers(world, 2, 32, 0);
    count = NuSpecialFind(world->current_gscn, &LevHSpecial[0], (char *)HothBattleCMinifig11Pc, 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[1], (char *)HothBattleCMinifig12Pc, 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[2], (char *)HothBattleCMinifig13Pc, 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[3], (char *)HothBattleCMinifig21Pc, 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[4], (char *)HothBattleCMinifig22Pc, 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[5], (char *)HothBattleCMinifig23Pc, 1);
    if (count == 6)
        hothtroopers = LevHSpecial;
}

void __cdecl HothBattleA_Reset(WORLDINFO *world) {
    GIZMO_PC *gizmo = LevGizmo[0];
    GIZMOPICKUP_PC *pickup;
    if (gizmo == NULL || gizmo->object == NULL)
        return;
    pickup = (GIZMOPICKUP_PC *)gizmo->object;
    if (minikitCounter_A == 10 && (pickup->flags & 8) == 0) {
        GizmoActivate(world->gizmo_sys, gizmo, 1, 1);
        return;
    }
    GizmoSetVisibility(world->gizmo_sys, gizmo, 0, 1);
}

void __cdecl HothBattleA_Update(WORLDINFO *world) {
    UpdateMiniSnowTroopers(world);
}

void __cdecl HothBattleA_DrawPc(WORLDINFO *world) {
    DrawMiniSnowTroopers(world);
}

void HothBattleB_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "snow_ball_1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "snow_ball_2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "snow_ball_3", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], "snow_ball_4", 1);
}
static const char HothBattleCBombMessagePc[20] = "BombGen_ATAT_Killed";
static const char HothBattleCIceBPc[8] = "ice_b";
static const char HothBattleCIceAPc[8] = "ice_a";
static const char HothBattleCObstaclePc[12] = "obstacle3";
static const char HothBattleCPickupPc[8] = "m_pup7";

void __cdecl HothBattleC_Init(WORLDINFO *world) {
    i32 direction;
    i32 count;
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, (char *)HothBattleCBombMessagePc, NULL);
    LevPathCnx[0] = AIPAthFindPathCnx(world->ai_sys, world->ai_sys->path_sys->active_path, (char *)HothBattleCIceAPc,
                                      (char *)HothBattleCIceBPc, &direction);
    LevGizmo[0] = (GIZMO_PC *)GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, (char *)HothBattleCObstaclePc);
    LevGizmo[1] = (GIZMO_PC *)GizmoFindByName(world->gizmo_sys, gizmopickup_typeid, (char *)HothBattleCPickupPc);
    NuSpecialSetVisibility(&LevHSpecial[10], 0);
    gunship_bolts[1] = BoltType_FindIDByName((char *)HothBattleCGreenBoltPc, world);
    gunship_bolts[0] = BoltType_FindIDByName((char *)HothBattleCRedBoltPc, world);
    trooper_side[0] = 0;
    trooper_side[1] = 1;
    InitMiniSnowTroopers(world, 2, 32, 0);
    count = NuSpecialFind(world->current_gscn, &LevHSpecial[0], (char *)HothBattleCMinifig11Pc, 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[1], (char *)HothBattleCMinifig12Pc, 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[2], (char *)HothBattleCMinifig13Pc, 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[3], (char *)HothBattleCMinifig21Pc, 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[4], (char *)HothBattleCMinifig22Pc, 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[5], (char *)HothBattleCMinifig23Pc, 1);
    if (count == 6)
        hothtroopers = LevHSpecial;
}

void HothBattleC_Reset(WORLDINFO *world) {
    struct gizmo_pc_s *gizmo;
    struct GIZMOPICKUP_PC *pickup;
    GIZMO_PC *bomb_generator = GizmoFindByName(world->gizmo_sys, bombgen_gizmotype_id, "bomb_generator1");
    if (bomb_generator != NULL && bomb_generator->object != NULL) {
        HothBattleC_BombGenerator = ((GIZBOMBGEN_s *)(bomb_generator->object));
    }

    gizmo = LevGizmo[1];
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }

    pickup = ((GIZMOPICKUP_PC *)(gizmo->object));
    if (minikitCounter_C == 10 && (pickup->flags & 8) == 0) {
        GizmoActivate(world->gizmo_sys, gizmo, 1, 1);
        return;
    }
    GizmoSetVisibility(world->gizmo_sys, gizmo, 0, 1);
}

void HothBattleC_Update(WORLDINFO *world) {
    if (netclient == 0 && HothBattleC_BombGenerator != NULL && !HothBattleC_BombGenerator->active &&
        LevAIMessage[0] != NULL && LevAIMessage[0]->value > 0.0f) {
        HothBattleC_BombGenerator->active = 1;
    }
    UpdateMiniSnowTroopers(world);
}

void __cdecl HothBattleC_DrawPc(WORLDINFO *world) {
    DrawMiniSnowTroopers(world);
}

void __cdecl HothBattleE_DrawPc(WORLDINFO *world) {
    DrawMiniSnowTroopers(world);
}
f32 HothBattleEPanelAlphaPc[16] = {0};
HOTHBATTLEE_NETPACKET_PC *HothBattleENetPacketPc = NULL;

void __cdecl HothBattleE_PanelPc(WORLDINFO *world) {
    i16 targets[16];
    char defeated[16];
    i32 count = 0, index;
    if (MiniCutCam != 0)
        return;
    if (netclient != 0) {
        DrawMeleeTargetsRowsPc(HothBattleENetPacketPc->targets, (char *)HothBattleENetPacketPc->defeated,
                               HothBattleENetPacketPc->alpha, HothBattleENetPacketPc->count);
        return;
    }
    switch (melee.current_wave) {
        case 1:
        case 2:
            for (index = 0; index < melee.waves[0].initial_count; ++index) {
                if (index == (melee.waves[0].initial_count >> 1) + 1) {
                    targets[count] = -1;
                    defeated[count++] = 0;
                }
                targets[count] = melee.waves[0].character_id;
                defeated[count++] = index >= melee.waves[0].remaining_count;
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
            defeated[count++] = melee.waves[2].remaining_count == 0;
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
        if (targets[index] != -1 && defeated[index] != 0)
            HothBattleEPanelAlphaPc[index] = SeekLinearF(HothBattleEPanelAlphaPc[index], 0.4f, 0.1f);
        else
            HothBattleEPanelAlphaPc[index] = 1.0f;
    }
    DrawMeleeTargetsRowsPc(targets, defeated, HothBattleEPanelAlphaPc, count);
}

AILOCATOR_PC *getSpawnLocator(float clip_radius, char *name) {
    AILOCATORSET_PC *locator_set = AIPathFindLocatorSet(WORLD->ai_sys, name);
    if (locator_set == NULL) {
        return NULL;
    }

    AILocatorSet_CheckLocatorsStillAssigned(WORLD->ai_sys, locator_set);
    {
        AILOCATOR_PC *locators[32];
        i32 i = 0;
        i32 locator_count = 0;
        for (; i < locator_set->locator_count && locator_count < 32; ++i) {
            if (locator_set->assigned[i] == 0xff) {
                locators[locator_count++] = &WORLD->ai_sys->locators[locator_set->locator_entries[i]];
            }
        }

        return LocalGetRandomLocator(locators, locator_count, clip_radius, &player->collision_position, 1000000000.0f,
                                     0, 1000000000.0f, 1000000000.0f);
    }
}

void __cdecl HothBattle_ManageBackgroundCreatures(void) {
    AILOCATORSET_PC *set = AIPathFindLocatorSet(WORLD->ai_sys, "spawn");
    u8 wave_types = 0;
    i32 type, last, index;
    f32 radius;
    AILOCATOR_PC *spawn, *entry;
    GAMEOBJECT *object;
    if (NOAICREATURES != 0)
        return;
    for (type = 0; type < melee.creature_count; ++type) {
        i16 id = melee.waves[type].character_id;
        if (id == VehicleBoltId7f18b4Pc)
            wave_types |= 1;
        else if (id == VehicleBoltId7f19a8Pc)
            wave_types |= 2;
        else if (id == id_ATST_LOWRES)
            wave_types |= 4;
        else if (id == id_ATAT)
            wave_types |= 8;
    }
    radius = apicharsys->char_data[VehicleBoltId7f18b4Pc].collision_radius;
    while (aicreature_sets_alive[0] < 5 && (wave_types & 1) == 0) {
        spawn = getSpawnLocator(radius, "spawn");
        if (spawn == NULL)
            return;
        object = AddDynamicCreature(VehicleBoltId7f18b4Pc, &spawn->position, spawn->rotation, "Probe",
                                    &spawn->path_info, NULL, 1, NULL, NULL, 0, 1);
        if (object == NULL)
            break;
        object->portal_flags |= 0x10000000;
        for (index = 0; index < set->locator_count; ++index) {
            entry = &WORLD->ai_sys->locators[set->locator_entries[index]];
            if (entry == spawn) {
                object->ai_packet.locator = entry;
                set->assigned[index] = object->hostility_group;
                break;
            }
        }
    }
    last = -1;
    for (index = 5; index >= 0; --index) {
        object = melee.background_creatures[index];
        if (object == NULL)
            continue;
        if ((object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 && object->ai_packet.creature_set == 3) {
            if (last == -1)
                last = index;
        } else if (last == -1)
            melee.background_creatures[index] = NULL;
        else {
            melee.background_creatures[index] = melee.background_creatures[last];
            melee.background_creatures[last--] = NULL;
        }
    }
    radius = apicharsys->char_data[id_ATST_LOWRES].collision_radius * 2.0f;
    while (aicreature_sets_alive[2] < 6 && (wave_types & 4) == 0) {
        spawn = getSpawnLocator(radius, "spawn");
        if (spawn == NULL)
            return;
        object = AddDynamicCreature(id_ATST_LOWRES, &spawn->position, spawn->rotation, "rider", &spawn->path_info, NULL,
                                    1, NULL, NULL, 0, 3);
        melee.background_creatures[++last] = object;
        if (object == NULL)
            return;
        object->portal_flags |= 0x10000000;
        for (index = 0; index < set->locator_count; ++index) {
            entry = &WORLD->ai_sys->locators[set->locator_entries[index]];
            if (entry == spawn) {
                object->ai_packet.locator = entry;
                set->assigned[index] = object->hostility_group;
                break;
            }
        }
    }
}

i32 __cdecl SpawnMeleeCreatureType(i32 type) {
    i32 index, slot, attempts;
    f32 radius = apicharsys->char_data[melee.waves[type].character_id].collision_radius;
    AILOCATOR_PC *locator;
    GAMEOBJECT *object;
    NUVEC offset;
    for (index = 0; index < melee.waves[type].active_count; ++index) {
        if (melee.waves[type].character_id != id_ATAT && melee.waves[type].creatures[index] != NULL &&
            melee.waves[type].creatures[index]->field_f10 > 5.0f) {
            KillGameObject(melee.waves[type].creatures[index], 4, 0);
            melee.waves[type].creatures[index] = NULL;
            --melee.waves[type].active_count;
        }
    }
    while (melee.waves[type].remaining_count > melee.waves[type].active_count) {
        if (melee.waves[type].active_count >= 4)
            break;
        slot = -1;
        for (index = 0; index < 4 && slot == -1; ++index) {
            if (melee.waves[type].creatures[index] == NULL)
                slot = index;
        }
        if (slot == -1)
            return 0;
        if (melee.waves[type].character_id == id_ATAT && melee.waves[type].active_count == 0) {
            object = GetNamedGameObject(WORLD->ai_sys, "ATAT_EDIT");
            melee.waves[type].creatures[slot] = object;
            if (object != NULL && object->dead == 0 && (object->flags_1fc_word & 0x1000) != 0) {
                ++melee.waves[type].active_count;
                continue;
            }
            melee.waves[type].creatures[slot] = NULL;
        }
        locator = NULL;
        for (attempts = 50; attempts > 0; --attempts) {
            locator = getSpawnLocator(5.0f, "spawn");
            if (locator != NULL) {
                if (OnOrInsidePlane(&locator->position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal, NULL, 0.5f,
                                    NULL) == 0 ||
                    OnOrInsidePlane(&locator->position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal, NULL, 0.5f,
                                    NULL) == 0) {
                    if (OnOrInsidePlane(&locator->position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal, NULL, 2.5f,
                                        NULL) != 0 ||
                        OnOrInsidePlane(&locator->position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal, NULL, 2.5f,
                                        NULL) != 0)
                        break;
                }
                locator = NULL;
            }
        }
        if (locator == NULL)
            return 0;
        offset.y = offset.z = 0.0f;
        offset.x = (f32)(((f64)NuFloatRand((NURAND *)&GAMERAND) + 1.0) * radius * 2.0);
        NuVecRotateY(&offset, &offset, qrand());
        NuVecAdd(&offset, &offset, &locator->position);
        if (melee.waves[type].character_id == id_ATAT)
            object = AddDynamicCreature(melee.waves[type].character_id, &locator->position, locator->rotation,
                                        melee.waves[type].name, &locator->path_info, NULL, 1, NULL, NULL, 0, 0);
        else
            object = AddDynamicCreature(melee.waves[type].character_id, &offset, locator->rotation,
                                        melee.waves[type].name, &locator->path_info, NULL, 1, NULL, NULL, 0, 0);
        if (object == NULL)
            return 0;
        melee.waves[type].creatures[slot] = object;
        ++melee.waves[type].active_count;
        object->ai_packet.locator = locator;
    }
    return 1;
}

void __cdecl HothBattleE_Init(WORLDINFO *world) {
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
    if (count == 6)
        hothtroopers = LevHSpecial;
    if (netclient == 0)
        HothBattle_ManageBackgroundCreatures();
    HothBattleENetPacketPc = (HOTHBATTLEE_NETPACKET_PC *)SetLevelHack(0x58);
}

i32 HothBattle_StartNewWave() {
    i32 type;
    if (melee.field_0x2 != 0) {
        switch (melee.field_0x1) {
            case 1:
                melee.waves[0].character_id = VehicleBoltId7f18b4Pc;
                melee.waves[0].field_0x18 = 11;
                melee.waves[0].field_0x19 = 11;
                NuStrCpy(melee.waves[0].name, "Probe");
                melee.creature_count = 1;
                break;
            case 2:
                melee.waves[0].character_id = id_ATST_LOWRES;
                melee.waves[0].field_0x18 = 11;
                melee.waves[0].field_0x19 = 11;
                NuStrCpy(melee.waves[0].name, "rider");
                melee.creature_count = 1;
                break;
            case 3:
                melee.waves[0].character_id = id_ATAT;
                melee.waves[0].field_0x18 = 2;
                melee.waves[0].field_0x19 = 2;
                NuStrCpy(melee.waves[0].name, "ATAT");
                melee.creature_count = 1;
                break;
            case 4:
                melee.waves[0].character_id = VehicleBoltId7f18b4Pc;
                melee.waves[0].field_0x18 = 4;
                melee.waves[0].field_0x19 = 4;
                NuStrCpy(melee.waves[0].name, "Probe");
                melee.waves[1].character_id = id_ATST_LOWRES;
                melee.waves[1].field_0x18 = 6;
                melee.waves[1].field_0x19 = 6;
                NuStrCpy(melee.waves[1].name, "rider");
                melee.waves[2].character_id = id_ATAT;
                melee.waves[2].field_0x18 = 1;
                melee.waves[2].field_0x19 = 1;
                NuStrCpy(melee.waves[2].name, "ATAT");
                melee.creature_count = 3;
                break;
        }
    }
    melee.field_0x2 = 0;
    if (MiniCutCam != 0)
        return 0;
    for (type = 0; type < melee.creature_count; ++type)
        SpawnMeleeCreatureType(type);
    return 1;
}

void __cdecl HothBattleE_UpdateWave(void) {
    i32 pending = 0, type, wave_index, index;
    char state[16];
    GAMEOBJECT *object;
    HOTHBATTLE_MELEE_WAVE_s *wave;
    HOTH_ANTINODE_PC *node;
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
                if (FreePlay != 0)
                    CompleteLevel(WORLD);
                else
                    GoToNewLevel(LevelFixupSlotPc0087b210->index);
            }
            melee.wave_delay = 0.0f;
            sprintf(state, "CamCut_%d", melee.next_wave);
            if (AIScriptSetBaseScriptStateByName(&((HOTH_WORLD_SCRIPT_PC *)WORLD)->processor, state) != 0)
                AIScriptProcess(WORLD->ai_sys, NULL, NULL, &((HOTH_WORLD_SCRIPT_PC *)WORLD)->processor, FRAMETIME);
            melee.transition_phase = 0;
            melee.wave_delay = 0.0f;
            melee.initialize_wave = 1;
            pending = 1;
        } else if (melee.transition_phase > 0) {
            if (MiniCutCam == 0) {
                if (melee.transition_phase == 1) {
                    object = Obj;
                    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
                        if ((object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
                            object->id != (u16)id_ATAT && object->field_280 == 0xff &&
                            (object->character_data->model_flags & 4) != 0)
                            KillGameObject(object, 4, 0);
                    }
                    for (index = 0; index < 6; ++index)
                        melee.background_creatures[index] = NULL;
                    melee.transition_phase = 2;
                }
                if (HothBattle_StartNewWave() == 0)
                    return;
                melee.transition_phase = -1;
                melee.current_wave = melee.next_wave;
                pending = 1;
            }
        } else if (melee.transition_phase == 0 && MiniCutCam != 0)
            melee.transition_phase = 1;
    }
    for (type = 0; type < melee.creature_count; ++type)
        SpawnMeleeCreatureType(type);
    HothBattle_ManageBackgroundCreatures();
    if (melee.current_wave == 3 && melee.waves[0].creatures[0] != NULL && melee.waves[0].creatures[1] != NULL) {
        radius = (f32)((f64)melee.waves[0].creatures[type]->bounds_radius * 3.0);
        node = (HOTH_ANTINODE_PC *)AIAntinodeCreateSingleFrame(&melee.waves[0].creatures[0]->position, radius);
        node->excluded_character_types = (i64)(i32) ~(1u << (melee.waves[0].creatures[1]->hostility_group & 31));
        radius = (f32)((f64)melee.waves[0].creatures[type]->bounds_radius * 3.0);
        node = (HOTH_ANTINODE_PC *)AIAntinodeCreateSingleFrame(&melee.waves[0].creatures[1]->position, radius);
        node->excluded_character_types = (i64)(i32) ~(1u << (melee.waves[0].creatures[0]->hostility_group & 31));
    }
    for (wave_index = 0; wave_index < 4; ++wave_index) {
        wave = &melee.waves[wave_index];
        count = 0;
        for (index = 0; index < 4; ++index) {
            object = wave->creatures[index];
            if (object != NULL) {
                if ((object->flags_1fc_word & 0x1000) == 0 || object->dead != 0) {
                    --wave->active_count;
                    --wave->remaining_count;
                    wave->creatures[index] = NULL;
                    PlaySfxAndSetPitch("TrueJedi_100pc", NULL, 1.5f);
                } else
                    ++count;
            }
        }
        if (wave->active_count != count)
            wave->active_count = (u8)count;
        if (wave->remaining_count != 0)
            pending = 1;
    }
    if ((pending | MiniCutCam) == 0 && NOAICREATURES == 0 && melee.current_wave == melee.next_wave)
        ++melee.next_wave;
}

void __cdecl HothBattleE_Update(WORLDINFO *world) {
    UpdateMiniSnowTroopers(world);
    if (netclient == 0)
        HothBattleE_UpdateWave();
}
