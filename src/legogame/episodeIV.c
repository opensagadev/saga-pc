#include "saga.h"
#include <string.h>
#include <stdio.h>
struct panel_movement_s;

// Kept as an assert, not a layout comment: the declaration count of this unit steers
// BlockadeRunnerDEjectCreature (docs/cleanup-audit.md, section 6).

i32 MosEisleyC_PastBarrier(GAMEOBJECT *object) {
    if (WORLD->current_level == MOSEISLEYC_LDATA) {
        return object->position.z > 5.85f;
    }
    return 0;
}

void MosEisleyE_Update(WORLDINFO *unused0) {
    if ((texanimbits & 2) == 0 && LevGizmo[0] != NULL && LevGizmo[0]->object != NULL && GizForce_Complete(LevGizmo[0]->object)) {
        texanimbits |= 2;
    }
}

void DeathStarEscapeC_Reset(WORLDINFO *unused0) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "RescuedDroids", NULL);
}

typedef struct TORPEDO_MUSIC_FLAGS_VIEW { // size 0x2
    u8 count;                             // 0x0
    u8 flags;                             // 0x1
} TORPEDO_MUSIC_FLAGS_VIEW;

// Kept as an assert, not a layout comment: the declaration count of this unit steers
// BlockadeRunnerDEjectCreature (docs/cleanup-audit.md, section 6).

i32 DeathStarShieldDown(void) {
    GIZMOBLOWUP_PC *blowup;
    GAMEOBJECT *player;
    TORPEDO_MUSIC_FLAGS_VIEW *packet;
    if (LevGizmo[0] == NULL) {
        LevGizmo[0] = GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, "bigbang1");
    }
    if (LevGizmo[0] == NULL || LevGizmo[0]->object == NULL) {
        return 0;
    }
    blowup = LevGizmo[0]->object;
    if ((blowup->status_flags & 1) != 0) {
        return 1;
    }
    if ((blowup->status_flags & 0x800000) == 0) {
        return 0;
    }
    player = Player[0];
    if (player != NULL &&
        (WORLD->current_level != DEATHSTARBATTLED_LDATA || ObjInNarrowSock(player, WORLD->sock_sys, WORLD->level_idx) == 0)) {
        player = Player[0];
        packet = (TORPEDO_MUSIC_FLAGS_VIEW *)player->torpedo;
        if (packet != NULL && (packet->count > 0 || (packet->flags & 2) != 0)) {
            return 1;
        }
    }
    player = Player[1];
    if (player != NULL &&
        (WORLD->current_level != DEATHSTARBATTLED_LDATA || ObjInNarrowSock(player, WORLD->sock_sys, WORLD->level_idx) == 0)) {
        player = Player[1];
        packet = (TORPEDO_MUSIC_FLAGS_VIEW *)player->torpedo;
        if (packet != NULL && (packet->count > 0 || (packet->flags & 2) != 0)) {
            return 1;
        }
    }
    return 0;
}

void DeathStarEscapeB_Init(WORLDINFO *world) {
    NUINSTANIM *animation[2];
    void *data[2];
    i32 index;
    DeathStarEscapeBNetPacket = SetLevelHack(4);
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "WindowClean", NULL);
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "lift_up", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "lift_down", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "sponge", 1);
    fakeanimendframe[0] = 0.0f;
    for (index = 0; index < 2; ++index) {
        animation[index] = NuSpecialGetInstAnim(&LevHSpecial[index]);
        if (animation[index] != NULL) {
            data[index] = LevHSpecial[index].scene->instance_animation_data[animation[index]->animation_index];
            if (data[index] != NULL) {
                fakeanimendframe[0] = (f32)((f64)NuAnimEndFrameOld(data[index]) + (f64)fakeanimendframe[0]);
            }
        }
    }
}

void DeathStarEscapeB_Update(WORLDINFO *world) {
    NUINSTANIM *animation[2] = {NULL, NULL};
    void *data[2] = {NULL, NULL};
    i32 index;
    if (netclient == 0) {
        DeathStarEscapeBNetPacket[0] = (i8)LevAIMessage[0]->value;
    }
    if (LevGameObject[0] == NULL && DeathStarEscapeBNetPacket[0] == 0) {
        LevGameObject[0] = GetNamedGameObject(world->ai_sys, "WASHER_1");
    }
    if (LevGameObject[1] == NULL && DeathStarEscapeBNetPacket[0] == 0) {
        LevGameObject[1] = GetNamedGameObject(world->ai_sys, "WASHER_2");
    }
    for (index = 0; index < 2; ++index) {
        animation[index] = NuSpecialGetInstAnim(&LevHSpecial[index]);
        if (animation[index] != NULL) {
            data[index] = LevHSpecial[index].scene->instance_animation_data[animation[index]->animation_index];
        }
    }
    if (data[0] != NULL && data[1] != NULL) {
        if (NuSpecialGetVisibilityFn(&LevHSpecial[0]) != 0) {
            fakeanimframe[0] = animation[0]->ltime;
            return;
        }
        if (NuSpecialGetVisibilityFn(&LevHSpecial[1]) != 0) {
            fakeanimframe[0] = (f32)((f64)NuAnimEndFrameOld(data[0]) + (f64)animation[1]->ltime);
            return;
        }
        fakeanimframe[0] = 0.0f;
    }
}

i16 dat_007f1a44 = -1;
/* Episode IV FixUpLevels callbacks (Mos Eisley, Blockade Runner, Death Star rescue and escape). */
i32 test_tb = 1;
extern i16 dat_007f1a3c;
extern i16 dat_007f1a40;

void MosEisleyB_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "land_speeder_terrain1", 1);
    LevGizObst[0] = GizObstacle_FindByName(world->giz_obstacle_sys, "Wash");
}

void MosEisleyE_Init(WORLDINFO *world) {
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force1");
}

void MosEisleyE_Reset(WORLDINFO *world) {
    texanimbits &= ~2;
}

void BlockadeRunnerB_Init(WORLDINFO *world) {
    LevBlowUp[0] = GizmoBlowUp_FindByName(world, "thermo_041");
}

void BlockadeRunnerB_Update(WORLDINFO *world) {
    if (LevBlowUp[0] != NULL && InStory() != 0) {
        LevBlowUp[0]->status_flags &= ~0x800000;
    }
}

void BlockadeRunnerC_Init(WORLDINFO *world) {
    LevGizObst[0] = GizObstacle_FindByName(world->giz_obstacle_sys, "obstacle8");
}

static void BlockadeRunnerDKillEjectedCreature(PART_PC *part, i32 reason) {
    i32 i;
    for (i = 0; i < 8; i++) {
        if (LevGamePart[i] == part) {
            if (LevGameObject[i] != NULL) {
                KillGameObject(LevGameObject[i], 4, 0);
                LevGameObject[i] = NULL;
            }
            LevGamePart[i] = NULL;
        }
    }
}

static NUVEC BlockadeRunnerDEjectVec[2] = {
    {13.0f, 1.6f, 7.93f},
    {13.0f, 1.6f, 6.55f}
};
static NUVEC BlockadeRunnerDEjectMom = {-0.5f, 0.0f, 0.0f};

static i32 BlockadeRunnerDDrawCreature(PART_PC *part) {
    return 0;
}

static void BlockadeRunnerDEjectCreature(i32 eject_index) {
    i32 models[4];
    NUMTX matrix;
    ADDPART params;
    i32 i;
    i32 model;
    models[0] = dat_007f1a3c;
    models[1] = dat_007f1a40;
    models[2] = dat_007f1a3c;
    models[3] = dat_007f1a44;
    if ((u32)eject_index > 1) {
        return;
    }
    for (i = 0; i < 8; i++) {
        if (LevGamePart[i] != NULL) {
            continue;
        }
        NuMtxSetTranslation(&matrix, &BlockadeRunnerDEjectVec[eject_index]);
        model = models[qrand() / 0x4000];
        LevGameObject[i] = AddDynamicCreature(model, &BlockadeRunnerDEjectVec[eject_index], 0, "default", NULL, NULL, 0, NULL, NULL, 0, 0);
        if (LevGameObject[i] != NULL) {
            LevGameObject[i]->flags_ef4 = (LevGameObject[i]->flags_ef4 & ~APIOBJECT_NO_TIME_BASED_UPDATE) | ((test_tb & 1) << 5);
            LevGameObject[i]->flags_ef0 |= APIOBJECT_DONT_MOVE;
            LevGameObject[i]->ai_packet.animation_override_from = 0xe9;
            LevGameObject[i]->ai_packet.animation_override_to = 5;
            LevGameObject[i]->flags_ef0 |= 0x4000000;
            params = Default_ADDPART;
            params.matrix = &matrix;
            params.velocity = &BlockadeRunnerDEjectMom;
            params.unknown_14 = 0.15f;
            params.unknown_18 = 0.15f;
            params.gravity = 0.0f;
            params.special = &WORLD->level_objects[254].special;
            params.flags = 0x8698;
            params.collision_fn = PartCollide_3D;
            params.kill_fn = BlockadeRunnerDKillEjectedCreature;
            params.draw_fn = BlockadeRunnerDDrawCreature;
            params.time_step = FRAMETIME;
            LevGamePart[i] = AddPart(&params);
            LevGamePart[i]->time_remaining = 30.0f;
            if (LevGamePart[i] == NULL) {
                KillGameObject(LevGameObject[i], 4, 0);
                LevGameObject[i] = NULL;
            }
            return;
        }
    }
}

void BlockadeRunnerD_Reset(WORLDINFO *world) {
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "obstacle15");
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "obstacle14");
    ((u8 *)LevFlag)[0] = 0;
    ((u8 *)LevFlag)[1] = 0;
}

void BlockadeRunnerD_Update(WORLDINFO *world) {
    i32 i;
    if (netclient != 0) {
        return;
    }
    for (i = 0; i < 8; i++) {
        if (LevGameObject[i] == NULL || LevGamePart[i] == NULL) {
            continue;
        }
        if ((LevGamePart[i]->active & 1) != 0) {
            LevGameObject[i]->flags_107a = 5;
            LevGameObject[i]->vehicle_orientation = LevGamePart[i]->transform;
            LevGameObject[i]->position = LevGameObject[i]->vehicle_orientation.translation;
            LevGameObject[i]->ai_update_position = LevGameObject[i]->position;
        }
        if ((LevGamePart[i]->active & 1) == 0 || LevGamePart[i]->kill_fn != BlockadeRunnerDKillEjectedCreature) {
            KillGameObject(LevGameObject[i], 4, 0);
            LevGameObject[i] = NULL;
        }
    }
    for (i = 0; i < 2; i++) {
        if (GizmoGetOutput(world->gizmo_sys, LevGizmo[i], 1, 1) != 0) {
            if (((u8 *)LevFlag)[i] == 0) {
                ((u8 *)LevFlag)[i] = 1;
                BlockadeRunnerDEjectCreature(i);
            }
        } else {
            ((u8 *)LevFlag)[i] = 0;
        }
    }
}

void DeathStarRescueB_Init(WORLDINFO *world) {
    u8 *flags = (u8 *)LevFlag;
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "reactor_1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "reactor_2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "reactor_3", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], "reactor_4", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[4], "reactor_5", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[5], "reactor_6", 1);
    flags[5] = 0;
    flags[4] = 0;
    flags[3] = 0;
    flags[2] = 0;
    flags[1] = 0;
    flags[0] = 0;
    LevSfxId[0] = GetSfxId("env_tractorbeam_lp");
    LevSfxId[1] = GetSfxId("env_tractorbeam_off");
}

void DeathStarRescueB_Update(WORLDINFO *world) {
    u8 *flags = (u8 *)LevFlag;
    i32 i;
    for (i = 0; i < 6; i++) {
        if (flags[i] == 0) {
            NUVEC *position = NuSpecialGetDrawPos(&LevHSpecial[i]);
            NUINSTANIM *animation = NuSpecialGetInstAnim(&LevHSpecial[i]);
            if (animation != NULL) {
                f64 frame = animation->ltime;
                u16 index = animation->animation_index;
                i32 finished = frame >= NuAnimEndFrameOld(world->current_gscn->instance_animation_data[index]);
                PlaySfxByIdAndSetVolume(LevSfxId[0], position, 0.25f);
                if (finished != 0) {
                    PlaySfxByIdAndSetVolume(LevSfxId[1], NULL, 0.4f);
                    flags[i] = 1;
                }
            }
        }
    }
}

void DeathStarEscapeB_Draw(WORLDINFO *world) {
    NUMTX matrix;
    if (LevGameObject[0] != NULL && DeathStarEscapeBNetPacket[0] == 0) {
        matrix = LevGameObject[0]->locator_matrices[1];
        NuSpecialDrawAt(&LevHSpecial[2], &matrix);
    }
    if (LevGameObject[1] != NULL && DeathStarEscapeBNetPacket[0] == 0) {
        matrix = LevGameObject[1]->locator_matrices[1];
        NuSpecialDrawAt(&LevHSpecial[2], &matrix);
    }
}

void DeathStarEscapeA_Update(WORLDINFO *world) {
    if (GameCam->sock == 2) {
        NUVEC position;
        position.x = (f32)((f64)qrand() * PlayerRandomScale * 2.0 + 36.5);
        position.y = -5.5f;
        position.z = (f32)((f64)qrand() * PlayerRandomScale * 2.0 + 8.5);
        LevChatterSfx("Dianoga_Groan", &position);
    }
}

void DeathStarEscapeC_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "door_push", 1);
    LevPathCnx[0] =
        AIPAthFindPathCnx(world->ai_sys, world->ai_sys->path_sys->active_path, "droid_rescue_a", "droid_rescue_b", &LevPathCnxDir);
}

void DeathStarEscapeC_Update(WORLDINFO *world) {
    NUVEC *position;
    if (LevPathCnx[0] == NULL) {
        return;
    }
    if (NuSpecialGetVisibilityFn(&LevHSpecial[2]) != 0 && (position = NuSpecialGetDrawPos(&LevHSpecial[2])) != NULL &&
        position->x < 77.25f) {
        LevPathCnx[0]->traversal_flags[0] &= ~0x80000000;
        LevPathCnx[0]->traversal_flags[1] &= ~0x80000000;
        if (LevAIMessage[0] != NULL) {
            LevAIMessage[0]->value = 1.0f;
        }
    } else {
        LevPathCnx[0]->traversal_flags[0] |= 0x80000000;
        LevPathCnx[0]->traversal_flags[1] |= 0x80000000;
    }
}

void DeathStarEscapeD_Update(WORLDINFO *world) {
    NUVEC position;
    position.x = (f32)((f64)qrand() * PlayerRandomScale * 4.0 + 32.5);
    position.y = -5.5f;
    position.z = (f32)((f64)qrand() * PlayerRandomScale * 4.0 + 12.5);
    LevChatterSfx("Dianoga_Groan", &position);
}

void LevelStreaming_DoorOverride(WORLDINFO *world, LEVELDATA *level, f32 radius, f32 *distance);

void MosEisleyD_AlwaysUpdate(WORLDINFO *world) {
    LevelStreaming_DoorOverride(world, dat_0087b1c0, 7.5f, NULL);
}

void DeathStarRescueB_AlwaysUpdate(WORLDINFO *world) {
    LevelStreaming_DoorOverride(world, dat_0087b1d0, 8.5f, NULL);
}

void DeathStarRescueC_AlwaysUpdate(WORLDINFO *world) {
    LevelStreaming_DoorOverride(world, DEATHSTARRESCUEE_LDATA, 8.5f, NULL);
}

void DeathStarEscapeB_AlwaysUpdate(WORLDINFO *world) {
    NUVEC position;
    SOCKPOSITION socket_position;
    if (Players_AveragePos(&position, &socket_position) && socket_position.sock_index != -1) {
        if (DEATHSTARESCAPEA_LDATA != NULL && socket_position.sock_index < 6) {
            other_level_override = DEATHSTARESCAPEA_LDATA->index;
        } else if (DEATHSTARESCAPEC_LDATA != NULL && socket_position.sock_index > 6) {
            other_level_override = DEATHSTARESCAPEC_LDATA->index;
        }
    }
}

/* Death Star battle midtro level slot (never assigned by FixUpLevels). */
LEVELDATA *DEATHSTARBATTLEMIDTRO_LDATA = NULL;

void DeathStarBattleC_AlwaysUpdate(WORLDINFO *world) {
    if (FreePlay == 0) {
        if (DEATHSTARBATTLEMIDTRO_LDATA != NULL) {
            other_level_override = DEATHSTARBATTLEMIDTRO_LDATA->index;
        }
    } else {
        if (DEATHSTARBATTLEMIDTRO_LDATA != NULL) {
            other_level_override = DEATHSTARBATTLEMIDTRO_LDATA->index;
        }
    }
}

typedef struct trenchrun_s { // size 0x20
    GAMEOBJECT *objects[3];  // 0x00
    NUVEC position;          // 0x0c
    i32 midtro_played;       // 0x18
    f32 spawn_timer;         // 0x1c
} TRENCHRUN;

/* Death Star trench run state. */
static TRENCHRUN trenchrun;
i8 *DeathStarEscapeBNetPacket = 0;

void DeathStarBattleDInit(WORLDINFO *world) {
}

void DeathStarBattleDReset(WORLDINFO *world) {
    memset(&trenchrun, 0, sizeof(trenchrun));
}

void DeathStarBattleDDraw(WORLDINFO *world) {
}

void fun_004e5eb0(void) {
}

/* Asteroid field. */

/* Rotating rock at the end of AsteroidChase C. */

/* Mutable tuning values. */
f32 TrenchLaneOffset = 10.0f;
f32 TrenchSeekX = 4.0f;
f32 TrenchSeekY = 0.5f;
f32 TrenchSeekZ = 0.5f;
f32 TrenchRollScale = 5000.0f;
f32 TrenchMaxHeightMove = 5.0f;

void TrenchMove(GAMEOBJECT *object) {
    NUVEC target;
    f32 phase;
    f32 height_difference;
    i32 roll;
    i32 target_roll;
    GAMEPAD *pad;
    f32 input_magnitude;
    u16 input_angle;
    object->previous_floor_height = object->floor_height;
    pad = object->pad;
    object->start_position = object->position;
    object->initial_position = object->collision_position;
    object->unknown_282 = object->unknown_281;
    input_magnitude = pad->input_magnitude;
    input_angle = pad->input_angle;
    pad->previous_input_magnitude = input_magnitude;
    pad->previous_input_angle = input_angle;
    object->previous_movement_angle = object->unknown_276;
    object->flags_ef0 &= ~0x4000;
    object->flags_107a = 0;
    if (object->model_draw_result != 0) {
        object->unknown_f10 = 0.0f;
    } else {
        object->unknown_f10 = FRAMETIME + object->unknown_f10;
    }
    if (object->character_context == CHARACTER_CONTEXT_DOOMED) {
        object->turn_braking += FRAMETIME;
    }
    if (object->character_context != CHARACTER_CONTEXT_TURN) {
        if ((object->movement_facing_angle > 0x8000) != (player->movement_facing_angle > 0x8000)) {
            StartTurn(object);
        } else if (player->character_context == CHARACTER_CONTEXT_TURN && player->context_timer < 0.5 * TURNTIME) {
            StartTurn(object);
        } else if (object->character_context != CHARACTER_CONTEXT_LOOP && player->character_context == CHARACTER_CONTEXT_LOOP &&
                   player->context_timer < 0.75 * VehicleLoopTime) {
            object->character_context = player->character_context;
            object->unknown_7aa = ACTION_IDLE;
            object->context_duration = VehicleLoopTime;
            object->context_timer = VehicleLoopTime;
        }
    }
    if (object->character_context == CHARACTER_CONTEXT_TURN) {
        phase = object->context_timer / TURNTIME;
        if (object->movement_facing_angle <= 0x8000) {
            target.x = (1.0f - phase) * TrenchLaneOffset + phase * -TrenchLaneOffset;
        } else {
            target.x = (1.0f - phase) * -TrenchLaneOffset + phase * TrenchLaneOffset;
        }
        target.x = trenchrun.position.x + target.x;
    } else if (object->movement_facing_angle > 0x8000) {
        target.x = trenchrun.position.x + TrenchLaneOffset;
    } else {
        target.x = trenchrun.position.x - TrenchLaneOffset;
    }
    object->position.x = SeekValF(object->position.x, target.x, TrenchSeekX);
    target.y = VehicleTurnOrLoopOffset(object) + (object->movement_spline_offset.y + trenchrun.position.y);
    height_difference = target.y - object->position.y;
    if (height_difference > TrenchMaxHeightMove) {
        target.y = TrenchMaxHeightMove + object->position.y;
    }
    object->position.y = SeekValF(object->position.y, target.y, TrenchSeekY);
    target.z = object->movement_spline_offset.z + trenchrun.position.z;
    object->position.z = SeekValF(object->position.z, target.z, TrenchSeekZ);
    NuVecSub(&object->velocity, &object->position, &object->start_position);
    NuVecScale(&object->velocity, &object->velocity, 1.0f / FRAMETIME);
    if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_BANKING) != 0) {
        if (object->character_context != CHARACTER_CONTEXT_TURN && object->character_context != CHARACTER_CONTEXT_LOOP &&
            object->character_context != CHARACTER_CONTEXT_BARREL_ROLL) {
            roll = (i32)(object->velocity.z * TrenchRollScale);
        } else {
            roll = 0;
        }
        if (player->movement_facing_angle < 0x8000) {
            roll = (i32)(0u - (u32)roll);
        }
        if (roll < -0x10000) {
            roll = -0x10000;
        } else if (roll > 0x10000) {
            roll = 0x10000;
        }
        target_roll = roll / 4;
        if (target_roll < -0x2000) {
            target_roll = -0x2000;
        } else if (target_roll > 0x2000) {
            target_roll = 0x2000;
        }
        object->movement_lean_angle = SeekRot(object->movement_lean_angle, target_roll, 8.0f);
    }
    APIObjectVelocities(object);
    GameObjectOrigin(object);
}

/* Mutable tuning values. */
f32 TrenchBaseHeight = -10.5f;
f32 TrenchVerticalOffset = 1.0f;
f32 TrenchLateralOffset = 2.0f;
f32 TrenchSpawnDistance = 20.0f;
f32 TrenchSpawnHeight = 40.0f;

static void TrenchKilledCallback(GAMEOBJECT *object) {
    i32 index;
    for (index = 0; index < 3; ++index) {
        if (trenchrun.objects[index] == object) {
            trenchrun.objects[index] = NULL;
            return;
        }
    }
}

void DeathStarBattleDUpdate(WORLDINFO *world) {
    GAMEOBJECT *trench_player;
    GAMEOBJECT *ship;
    NUVEC spawn;
    NUVEC offset;
    f32 lower;
    f32 upper;
    i32 index;
    i32 model;
    i32 left_present;
    i32 free_play;
    if (netclient == 0) {
        trench_player = player;
        trenchrun.position.x = trench_player->collision_position.x;
        trenchrun.position.y = TrenchBaseHeight;
        if (trench_player->collision_position.x < -500.0) {
            free_play = FreePlay;
            trenchrun.position.y = TrenchBaseHeight + TrenchSpawnHeight;
            if (free_play == 0 && trenchrun.midtro_played == 0) {
                for (index = 0; index < 3; ++index) {
                    if (trenchrun.objects[index] != NULL) {
                        KillGameObject(trenchrun.objects[index], 4, 0);
                        trenchrun.objects[index] = NULL;
                    }
                }
                NewCutScene(NULL, world->cutscene_sys, "deathstarbattle_midtro_ingame", 1);
                trench_player = player;
                trenchrun.midtro_played = 1;
            }
        }
        left_present = trenchrun.objects[0] != NULL;
        trenchrun.position.z = trench_player->collision_position.z;
        lower = left_present ? 62.0f : 60.0f;
        upper = trenchrun.objects[2] != NULL ? 70.0f : 72.0f;
        if (trenchrun.position.z < lower) {
            trenchrun.position.z = lower;
        } else if (upper < trenchrun.position.z) {
            trenchrun.position.z = upper;
        }
        if (aicreature_sets_alive[0] == 0) {
            trenchrun.spawn_timer -= FRAMETIME;
            if (trenchrun.spawn_timer <= 0.0f) {
                trenchrun.spawn_timer = 5.0f;
                for (index = 0; index < 3; ++index) {
                    spawn.x = player->collision_position.x + TrenchSpawnDistance;
                    spawn.y = player->collision_position.y + TrenchSpawnHeight;
                    spawn.z = player->collision_position.z;
                    offset.x = 0.0f;
                    offset.y = 0.0f;
                    offset.z = 0.0f;
                    switch (index) {
                        case 0:
                            offset.z = TrenchLateralOffset;
                            break;
                        case 1:
                            offset.y = TrenchVerticalOffset;
                            break;
                        case 2:
                            offset.z = -TrenchLateralOffset;
                            break;
                    }
                    NuVecAdd(&spawn, &spawn, &offset);
                    if (trenchrun.midtro_played == 0 && index == 1) {
                        model = dat_007f196c;
                    } else {
                        model = id_TIEFIGHTER;
                    }
                    ship =
                        AddDynamicCreature(model, &spawn, 0xc000, "TrenchBaddie", &player->ai_packet.path_info, NULL, 0, NULL, NULL, 0, 1);
                    if (ship != NULL) {
                        ship->flags_ef8_word |= 4;
                        ship->tag_object_callback = TrenchKilledCallback;
                        ship->move_override = TrenchMove;
                        trenchrun.objects[index] = ship;
                        ship->movement_spline_offset = offset;
                    }
                }
            }
        }
    }
    if (((u8 *)LevFlag)[4] != 0) {
        return;
    }
    if (NuSpecialGetVisibilityFn(&LevHSpecial[0])) {
        PlaySfx("ffield", NuSpecialGetDrawPos(&LevHSpecial[0]));
    } else {
        PlaySfx("ffieldoff", NuSpecialGetDrawPos(&LevHSpecial[0]));
        ((u8 *)LevFlag)[4] = 1;
    }
}
