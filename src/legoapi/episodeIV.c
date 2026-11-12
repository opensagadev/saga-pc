#include "saga.h"
#include <string.h>
#include <stdio.h>
struct panel_movement_pc_s;
typedef struct CLOUDCITY_PANEL_FLAGS_VIEW_PC {
    u8 unknown_00[0x62];
    u8 flags;
} CLOUDCITY_PANEL_FLAGS_VIEW_PC;
STATIC_ASSERT(offsetof(CLOUDCITY_PANEL_FLAGS_VIEW_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, build_state) == 0x6d);
typedef struct CLOUDCITY_LEVEL_FLAG_VIEW_PC {
    u8 started;
} CLOUDCITY_LEVEL_FLAG_VIEW_PC;
STATIC_ASSERT(offsetof(CLOUDCITY_LEVEL_FLAG_VIEW_PC, started) == 0);

extern LEVELDATA_PC *MOSEISLEYC_LDATA;
i32 __cdecl MosEisleyC_PastBarrier(GAMEOBJECT *object) {
    if (WORLD->current_level == MOSEISLEYC_LDATA)
        return object->position.z > 5.85f;
    return 0;
}

void MosEisleyE_Update(WORLDINFO *unused0) {
    if ((texanimbits & 2) == 0 && LevGizmo[0] != NULL && LevGizmo[0]->object != NULL &&
        GizForce_Complete(((GIZFORCE_s *)(LevGizmo[0]->object))))
        texanimbits |= 2;
}

void DeathStarEscapeC_Reset(WORLDINFO *unused0) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "RescuedDroids", NULL);
}

typedef struct TORPEDO_MUSIC_FLAGS_VIEW_PC {
    u8 count, flags;
} TORPEDO_MUSIC_FLAGS_VIEW_PC;
STATIC_ASSERT(offsetof(TORPEDO_MUSIC_FLAGS_VIEW_PC, count) == 0);
STATIC_ASSERT(offsetof(TORPEDO_MUSIC_FLAGS_VIEW_PC, flags) == 1);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, status_flags) == 0x94);
STATIC_ASSERT(offsetof(GAMEOBJECT, torpedo) == 0xcc4);
STATIC_ASSERT(offsetof(WORLDINFO, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(WORLDINFO, sock_sys) == 0x2988);
STATIC_ASSERT(offsetof(WORLDINFO, level_idx) == 0x11c);

i32 __cdecl DeathStarShieldDown(void) {
    GIZMOBLOWUP_PC *blowup;
    GAMEOBJECT *player;
    TORPEDO_MUSIC_FLAGS_VIEW_PC *packet;
    if (LevGizmo[0] == NULL)
        LevGizmo[0] = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, "bigbang1");
    if (LevGizmo[0] == NULL || LevGizmo[0]->object == NULL)
        return 0;
    blowup = (GIZMOBLOWUP_PC *)LevGizmo[0]->object;
    if ((blowup->status_flags & 1) != 0)
        return 1;
    if ((blowup->status_flags & 0x800000) == 0)
        return 0;
    player = Player[0];
    if (player != NULL && (WORLD->current_level != DEATHSTARBATTLED_LDATA ||
                           ObjInNarrowSock(player, WORLD->sock_sys, WORLD->level_idx) == 0)) {
        player = Player[0];
        packet = (TORPEDO_MUSIC_FLAGS_VIEW_PC *)player->torpedo;
        if (packet != NULL && (packet->count > 0 || (packet->flags & 2) != 0))
            return 1;
    }
    player = Player[1];
    if (player != NULL && (WORLD->current_level != DEATHSTARBATTLED_LDATA ||
                           ObjInNarrowSock(player, WORLD->sock_sys, WORLD->level_idx) == 0)) {
        player = Player[1];
        packet = (TORPEDO_MUSIC_FLAGS_VIEW_PC *)player->torpedo;
        if (packet != NULL && (packet->count > 0 || (packet->flags & 2) != 0))
            return 1;
    }
    return 0;
}

/* Native 0x004e5170, 204 bytes. */
void __cdecl DeathStarEscapeB_Init(WORLDINFO *world) {
    NUINSTANIM *animation[2];
    void *data[2];
    i32 index;
    DeathStarEscapeBNetPacketPc = (i8 *)SetLevelHack(4);
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, DeathStarEscapeBWindowCleanPc, NULL);
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], (char *)DeathStarEscapeBLiftUpPc, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], (char *)DeathStarEscapeBLiftDownPc, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], (char *)DeathStarEscapeBSpongePc, 1);
    fakeanimendframe[0] = 0.0f;
    for (index = 0; index < 2; ++index) {
        animation[index] = (NUINSTANIM *)NuSpecialGetInstAnim(&LevHSpecial[index]);
        if (animation[index] != NULL) {
            data[index] =
                ((NUGSCN *)LevHSpecial[index].scene)->instance_animation_data[animation[index]->animation_index];
            if (data[index] != NULL)
                fakeanimendframe[0] = (f32)((f64)NuAnimEndFrameOld(data[index]) + (f64)fakeanimendframe[0]);
        }
    }
}

/* Native 0x004e5240, 307 bytes. */
void __cdecl DeathStarEscapeB_Update(WORLDINFO *world) {
    NUINSTANIM *animation[2] = {NULL, NULL};
    void *data[2] = {NULL, NULL};
    i32 index;
    if (netclient == 0)
        DeathStarEscapeBNetPacketPc[0] = (i8)LevAIMessage[0]->value;
    if (LevGameObject[0] == NULL && DeathStarEscapeBNetPacketPc[0] == 0)
        LevGameObject[0] = GetNamedGameObject(world->ai_sys, (char *)DeathStarEscapeBWasher1Pc);
    if (LevGameObject[1] == NULL && DeathStarEscapeBNetPacketPc[0] == 0)
        LevGameObject[1] = GetNamedGameObject(world->ai_sys, (char *)DeathStarEscapeBWasher2Pc);
    for (index = 0; index < 2; ++index) {
        animation[index] = (NUINSTANIM *)NuSpecialGetInstAnim(&LevHSpecial[index]);
        if (animation[index] != NULL)
            data[index] =
                ((NUGSCN *)LevHSpecial[index].scene)->instance_animation_data[animation[index]->animation_index];
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

/* Episode IV FixUpLevels callbacks (Mos Eisley, Blockade Runner, Death Star rescue and escape). */
i32 test_tb = 1;
extern i16 CharacterFixupIdPc007f1a3c;
extern i16 CharacterFixupIdPc007f1a40;
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet.animation_override_from) == 0x3ee);

void __cdecl MosEisleyB_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "land_speeder_terrain1", 1);
    LevGizObst[0] = GizObstacle_FindByName(world->giz_obstacle_sys, "Wash");
}

void __cdecl MosEisleyE_Init(WORLDINFO *world) {
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force1");
}

void __cdecl MosEisleyE_Reset(WORLDINFO *world) {
    texanimbits &= ~2;
}

void __cdecl BlockadeRunnerB_Init(WORLDINFO *world) {
    LevBlowUp[0] = GizmoBlowUp_FindByName(world, "thermo_041");
}

void __cdecl BlockadeRunnerB_Update(WORLDINFO *world) {
    if (LevBlowUp[0] != NULL && InStory() != 0)
        LevBlowUp[0]->status_flags &= ~0x800000;
}

void __cdecl BlockadeRunnerC_Init(WORLDINFO *world) {
    LevGizObst[0] = GizObstacle_FindByName(world->giz_obstacle_sys, "obstacle8");
}

static void BlockadeRunnerDKillEjectedCreaturePc(PART_PC *part, i32 reason) {
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

static NUVEC BlockadeRunnerDEjectVecPc[2] = {{13.0f, 1.6f, 7.93f}, {13.0f, 1.6f, 6.55f}};
static NUVEC BlockadeRunnerDEjectMomPc = {-0.5f, 0.0f, 0.0f};

static i32 BlockadeRunnerDDrawCreaturePc(PART_PC *part) {
    return 0;
}

static void BlockadeRunnerDEjectCreaturePc(i32 eject_index) {
    i32 models[4];
    NUMTX matrix;
    ADDPART_PC params;
    i32 i;
    i32 model;
    models[0] = CharacterFixupIdPc007f1a3c;
    models[1] = CharacterFixupIdPc007f1a40;
    models[2] = CharacterFixupIdPc007f1a3c;
    models[3] = FreePlayId007f1a44Pc;
    if ((u32)eject_index > 1)
        return;
    for (i = 0; i < 8; i++) {
        if (LevGamePart[i] != NULL)
            continue;
        NuMtxSetTranslation(&matrix, &BlockadeRunnerDEjectVecPc[eject_index]);
        model = models[qrand() / 0x4000];
        LevGameObject[i] = AddDynamicCreature(model, &BlockadeRunnerDEjectVecPc[eject_index], 0, "default", NULL, NULL,
                                              0, NULL, NULL, 0, 0);
        if (LevGameObject[i] != NULL) {
            LevGameObject[i]->flags_ef4 = (LevGameObject[i]->flags_ef4 & ~0x20) | ((test_tb & 1) << 5);
            LevGameObject[i]->flags_ef0 |= 0x10;
            LevGameObject[i]->ai_packet.animation_override_from = 0xe9;
            LevGameObject[i]->ai_packet.animation_override_to = 5;
            LevGameObject[i]->flags_ef0 |= 0x4000000;
            params = Default_ADDPART;
            params.matrix = &matrix;
            params.velocity = &BlockadeRunnerDEjectMomPc;
            params.field_14 = 0.15f;
            params.field_18 = 0.15f;
            params.gravity = 0.0f;
            params.special = &WORLD->level_objects[254].special;
            params.flags = 0x8698;
            params.collision_fn = PartCollide_3D;
            params.kill_fn = BlockadeRunnerDKillEjectedCreaturePc;
            params.draw_fn = BlockadeRunnerDDrawCreaturePc;
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

void __cdecl BlockadeRunnerD_Reset(WORLDINFO *world) {
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "obstacle15");
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "obstacle14");
    ((u8 *)LevFlag)[0] = 0;
    ((u8 *)LevFlag)[1] = 0;
}

void __cdecl BlockadeRunnerD_Update(WORLDINFO *world) {
    i32 i;
    if (netclient != 0)
        return;
    for (i = 0; i < 8; i++) {
        if (LevGameObject[i] == NULL || LevGamePart[i] == NULL)
            continue;
        if ((LevGamePart[i]->active & 1) != 0) {
            LevGameObject[i]->flags_107a = 5;
            LevGameObject[i]->vehicle_orientation = LevGamePart[i]->transform;
            LevGameObject[i]->position = LevGameObject[i]->vehicle_orientation.translation;
            LevGameObject[i]->ai_update_position = LevGameObject[i]->position;
        }
        if ((LevGamePart[i]->active & 1) == 0 || LevGamePart[i]->kill_fn != BlockadeRunnerDKillEjectedCreaturePc) {
            KillGameObject(LevGameObject[i], 4, 0);
            LevGameObject[i] = NULL;
        }
    }
    for (i = 0; i < 2; i++) {
        if (GizmoGetOutput(world->gizmo_sys, LevGizmo[i], 1, 1) != 0) {
            if (((u8 *)LevFlag)[i] == 0) {
                ((u8 *)LevFlag)[i] = 1;
                BlockadeRunnerDEjectCreaturePc(i);
            }
        } else {
            ((u8 *)LevFlag)[i] = 0;
        }
    }
}

void __cdecl DeathStarRescueB_Init(WORLDINFO *world) {
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

void __cdecl DeathStarRescueB_Update(WORLDINFO *world) {
    u8 *flags = (u8 *)LevFlag;
    i32 i;
    for (i = 0; i < 6; i++) {
        if (flags[i] == 0) {
            NUVEC *position = NuSpecialGetDrawPos(&LevHSpecial[i]);
            NUINSTANIM *animation = (NUINSTANIM *)NuSpecialGetInstAnim(&LevHSpecial[i]);
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

void __cdecl DeathStarEscapeB_Draw(WORLDINFO *world) {
    NUMTX matrix;
    if (LevGameObject[0] != NULL && DeathStarEscapeBNetPacketPc[0] == 0) {
        matrix = LevGameObject[0]->locator_matrices[1];
        NuSpecialDrawAt(&LevHSpecial[2], &matrix);
    }
    if (LevGameObject[1] != NULL && DeathStarEscapeBNetPacketPc[0] == 0) {
        matrix = LevGameObject[1]->locator_matrices[1];
        NuSpecialDrawAt(&LevHSpecial[2], &matrix);
    }
}

void __cdecl DeathStarEscapeA_Update(WORLDINFO *world) {
    if (GameCam->sock == 2) {
        NUVEC position;
        position.x = (f32)((f64)qrand() * PlayerRandomScalePc * 2.0 + 36.5);
        position.y = -5.5f;
        position.z = (f32)((f64)qrand() * PlayerRandomScalePc * 2.0 + 8.5);
        LevChatterSfx("Dianoga_Groan", &position);
    }
}

void __cdecl DeathStarEscapeC_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "door_push", 1);
    LevPathCnx[0] = AIPAthFindPathCnx(world->ai_sys, world->ai_sys->path_sys->active_path, "droid_rescue_a",
                                      "droid_rescue_b", &LevPathCnxDir);
}

void __cdecl DeathStarEscapeC_Update(WORLDINFO *world) {
    NUVEC *position;
    if (LevPathCnx[0] == NULL)
        return;
    if (NuSpecialGetVisibilityFn(&LevHSpecial[2]) != 0 && (position = NuSpecialGetDrawPos(&LevHSpecial[2])) != NULL &&
        position->x < 77.25f) {
        ((AIPATHCNX_PC *)LevPathCnx[0])->traversal_flags[0] &= ~0x80000000;
        ((AIPATHCNX_PC *)LevPathCnx[0])->traversal_flags[1] &= ~0x80000000;
        if (LevAIMessage[0] != NULL)
            LevAIMessage[0]->value = 1.0f;
    } else {
        ((AIPATHCNX_PC *)LevPathCnx[0])->traversal_flags[0] |= 0x80000000;
        ((AIPATHCNX_PC *)LevPathCnx[0])->traversal_flags[1] |= 0x80000000;
    }
}

void __cdecl DeathStarEscapeD_Update(WORLDINFO *world) {
    NUVEC position;
    position.x = (f32)((f64)qrand() * PlayerRandomScalePc * 4.0 + 32.5);
    position.y = -5.5f;
    position.z = (f32)((f64)qrand() * PlayerRandomScalePc * 4.0 + 12.5);
    LevChatterSfx("Dianoga_Groan", &position);
}

void __cdecl LevelStreaming_DoorOverride(WORLDINFO *world, LEVELDATA_PC *level, f32 radius, f32 *distance);

void __cdecl MosEisleyD_AlwaysUpdate(WORLDINFO *world) {
    LevelStreaming_DoorOverride(world, LevelFixupSlotPc0087b1c0, 7.5f, NULL);
}

void __cdecl DeathStarRescueB_AlwaysUpdate(WORLDINFO *world) {
    LevelStreaming_DoorOverride(world, LevelFixupSlotPc0087b1d0, 8.5f, NULL);
}

void __cdecl DeathStarRescueC_AlwaysUpdate(WORLDINFO *world) {
    LevelStreaming_DoorOverride(world, DEATHSTARRESCUEE_LDATA, 8.5f, NULL);
}

void __cdecl DeathStarEscapeB_AlwaysUpdate(WORLDINFO *world) {
    NUVEC position;
    SOCKPOSITION_PC socket_position;
    if (Players_AveragePos(&position, &socket_position) && socket_position.sock_index != -1) {
        if (DEATHSTARESCAPEA_LDATA != NULL && socket_position.sock_index < 6)
            other_level_override = DEATHSTARESCAPEA_LDATA->index;
        else if (DEATHSTARESCAPEC_LDATA != NULL && socket_position.sock_index > 6)
            other_level_override = DEATHSTARESCAPEC_LDATA->index;
    }
}

/* PC 0x0087b1f8: Death Star battle midtro level slot (never assigned by FixUpLevels). */
LEVELDATA_PC *DEATHSTARBATTLEMIDTRO_LDATA = NULL;

void __cdecl DeathStarBattleC_AlwaysUpdate(WORLDINFO *world) {
    if (FreePlay == 0) {
        if (DEATHSTARBATTLEMIDTRO_LDATA != NULL)
            other_level_override = DEATHSTARBATTLEMIDTRO_LDATA->index;
    } else {
        if (DEATHSTARBATTLEMIDTRO_LDATA != NULL)
            other_level_override = DEATHSTARBATTLEMIDTRO_LDATA->index;
    }
}

typedef struct trenchrun_pc_s {
    GAMEOBJECT *objects[3];
    NUVEC position;
    i32 midtro_played;
    f32 spawn_timer;
} TRENCHRUN_PC;
STATIC_ASSERT(sizeof(TRENCHRUN_PC) == 0x20);

/* PC 0x00889638: Death Star trench run state. */
static TRENCHRUN_PC trenchrun;

void __cdecl DeathStarBattleDInit(WORLDINFO *world) {
}

void __cdecl DeathStarBattleDReset(WORLDINFO *world) {
    memset(&trenchrun, 0, sizeof(trenchrun));
}

void __cdecl DeathStarBattleDDraw(WORLDINFO *world) {
}

/* PC 0x00889660: asteroid field (original episodeV.cpp). */
typedef struct ASTEROID_PC {
    NUSPECIALHANDLE special;
    GIZMOBLOWUP_PC *blowup;
    i16 rotation_speed_x, rotation_speed_y, rotation_speed_z;
    u8 activated;
} ASTEROID_PC;
STATIC_ASSERT(sizeof(ASTEROID_PC) == 0x18);
STATIC_ASSERT(offsetof(ASTEROID_PC, activated) == 0x16);

/* PC 0x0088a268: rotating rock at the end of AsteroidChase C. */
typedef struct FINAL_ASTEROID_PC {
    NUSPECIALHANDLE special;
    GIZMOBLOWUP_PC *blowups[8];
    i16 blowup_count;
    i16 rotation_speed_x, rotation_speed_y, rotation_speed_z;
    i16 rotation_x, rotation_y, rotation_z;
} FINAL_ASTEROID_PC;
STATIC_ASSERT(sizeof(FINAL_ASTEROID_PC) == 0x3c);
STATIC_ASSERT(offsetof(FINAL_ASTEROID_PC, rotation_x) == 0x34);

typedef struct ASTEROIDC_NETPACKET_PC {
    u16 rotation_x, rotation_y, rotation_z;
} ASTEROIDC_NETPACKET_PC;

STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, model_rotation_x) == 0xe8);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, score) == 0xa0);
STATIC_ASSERT(offsetof(GAMEOBJECT, layer_mask) == 0x1048);

/* Native-observed mutable scalar providers; Native allocation extents unknown. */
#pragma data_seg(".data$TrenchLaneOffsetPc")
f32 TrenchLaneOffsetPc = 10.0f;
#pragma data_seg(".data$TrenchSeekXPc")
f32 TrenchSeekXPc = 4.0f;
#pragma data_seg(".data$TrenchSeekYPc")
f32 TrenchSeekYPc = 0.5f;
#pragma data_seg(".data$TrenchSeekZPc")
f32 TrenchSeekZPc = 0.5f;
#pragma data_seg(".data$TrenchRollScalePc")
f32 TrenchRollScalePc = 5000.0f;
#pragma data_seg(".data$TrenchMaxHeightMovePc")
f32 TrenchMaxHeightMovePc = 5.0f;
#pragma data_seg()

void __cdecl TrenchMove(GAMEOBJECT *object) {
    f32 phase, target_x, target_y, target_z, height_difference;
    i32 roll, target_roll;
    GAMEPAD_PC *pad;
    f32 input_magnitude;
    u16 input_angle;
    object->previous_floor_height = object->floor_height;
    pad = object->pad;
    object->start_position = object->position;
    object->initial_position = object->collision_position;
    object->field_282 = object->field_281;
    input_magnitude = pad->input_magnitude;
    input_angle = pad->input_angle;
    pad->previous_input_magnitude = input_magnitude;
    pad->previous_input_angle = input_angle;
    object->flags_ef0 &= ~0x4000;
    object->previous_movement_angle = object->field_0x276;
    object->flags_107a = 0;
    if (object->model_draw_result != 0)
        object->field_f10 = 0.0f;
    else
        object->field_f10 = FRAMETIME + object->field_f10;
    if (object->character_context == 0x2b)
        object->turn_braking += FRAMETIME;
    if (object->character_context != 0x2a) {
        if ((object->movement_facing_angle > 0x8000) != (player->movement_facing_angle > 0x8000)) {
            StartTurn(object);
        } else if (player->character_context == 0x2a && player->context_timer < 0.5 * TURNTIME) {
            StartTurn(object);
        } else if (object->character_context != 0x36 && player->character_context == 0x36 &&
                   player->context_timer < 0.75 * VehicleLoopTimePc) {
            object->character_context = player->character_context;
            object->field_7aa = 1;
            object->context_duration = VehicleLoopTimePc;
            object->context_timer = VehicleLoopTimePc;
        }
    }
    if (object->character_context == 0x2a) {
        phase = object->context_timer / TURNTIME;
        if (object->movement_facing_angle > 0x8000)
            target_x = phase * TrenchLaneOffsetPc + (1.0f - phase) * -TrenchLaneOffsetPc;
        else
            target_x = phase * -TrenchLaneOffsetPc + (1.0f - phase) * TrenchLaneOffsetPc;
        target_x = trenchrun.position.x + target_x;
    } else if (object->movement_facing_angle > 0x8000) {
        target_x = trenchrun.position.x + TrenchLaneOffsetPc;
    } else {
        target_x = trenchrun.position.x - TrenchLaneOffsetPc;
    }
    object->position.x = SeekValF(object->position.x, target_x, TrenchSeekXPc);
    target_y = VehicleTurnOrLoopOffset(object) + (object->movement_spline_offset.y + trenchrun.position.y);
    height_difference = target_y - object->position.y;
    if (height_difference > TrenchMaxHeightMovePc)
        target_y = TrenchMaxHeightMovePc + object->position.y;
    object->position.y = SeekValF(object->position.y, target_y, TrenchSeekYPc);
    target_z = object->movement_spline_offset.z + trenchrun.position.z;
    object->position.z = SeekValF(object->position.z, target_z, TrenchSeekZPc);
    NuVecSub(&object->velocity, &object->position, &object->start_position);
    NuVecScale(&object->velocity, &object->velocity, 1.0f / FRAMETIME);
    if ((object->character_data->game_character->flags_090 & 1) != 0) {
        if (object->character_context != 0x2a && object->character_context != 0x36 && object->character_context != 0x3a)
            roll = (i32)(object->velocity.z * TrenchRollScalePc);
        else
            roll = 0;
        if (player->movement_facing_angle < 0x8000)
            roll = (i32)(0u - (u32)roll);
        if (roll < -0x10000)
            roll = -0x10000;
        else if (roll > 0x10000)
            roll = 0x10000;
        target_roll = roll / 4;
        if (target_roll < -0x2000)
            target_roll = -0x2000;
        else if (target_roll > 0x2000)
            target_roll = 0x2000;
        object->movement_lean_angle = SeekRot(object->movement_lean_angle, target_roll, 8.0f);
    }
    APIObjectVelocities(object);
    GameObjectOrigin(object);
}

/* Native-observed mutable scalar minima; original allocation extents unknown. */
#pragma data_seg(".data$TrenchBaseHeightPc")
f32 TrenchBaseHeightPc = -10.5f;
#pragma data_seg(".data$TrenchVerticalOffsetPc")
f32 TrenchVerticalOffsetPc = 1.0f;
#pragma data_seg(".data$TrenchLateralOffsetPc")
f32 TrenchLateralOffsetPc = 2.0f;
#pragma data_seg(".data$TrenchSpawnDistancePc")
f32 TrenchSpawnDistancePc = 20.0f;
#pragma data_seg(".data$TrenchSpawnHeightPc")
f32 TrenchSpawnHeightPc = 40.0f;
#pragma data_seg()
STATIC_ASSERT(offsetof(TRENCHRUN_PC, objects) == 0);
STATIC_ASSERT(offsetof(TRENCHRUN_PC, position) == 0xc);
STATIC_ASSERT(offsetof(TRENCHRUN_PC, midtro_played) == 0x18);
STATIC_ASSERT(offsetof(TRENCHRUN_PC, spawn_timer) == 0x1c);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, path_info) == 0x420);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef8_word) == 0xef8);
STATIC_ASSERT(offsetof(GAMEOBJECT, tag_object_callback) == 0xea8);
STATIC_ASSERT(offsetof(GAMEOBJECT, move_override) == 0x10b0);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_spline_offset) == 0xe84);

/* Native 0x004e5b70..0x004e5b94, complete 36-byte cdecl callback. */
static void __cdecl TrenchKilledCallback(GAMEOBJECT *object) {
    i32 index;
    for (index = 0; index < 3; ++index) {
        if (trenchrun.objects[index] == object) {
            trenchrun.objects[index] = NULL;
            return;
        }
    }
}

void __cdecl DeathStarBattleDUpdate(WORLDINFO *world) {
    GAMEOBJECT *trench_player, *ship;
    NUVEC spawn, offset;
    f32 lower, upper;
    i32 index, model, left_present, free_play;
    if (netclient == 0) {
        trench_player = player;
        trenchrun.position.x = trench_player->collision_position.x;
        trenchrun.position.y = TrenchBaseHeightPc;
        if (trench_player->collision_position.x < -500.0) {
            free_play = FreePlay;
            trenchrun.position.y = TrenchBaseHeightPc + TrenchSpawnHeightPc;
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
        if (trenchrun.position.z < lower)
            trenchrun.position.z = lower;
        else if (upper < trenchrun.position.z)
            trenchrun.position.z = upper;
        if (aicreature_sets_alive[0] == 0) {
            trenchrun.spawn_timer -= FRAMETIME;
            if (trenchrun.spawn_timer <= 0.0f) {
                trenchrun.spawn_timer = 5.0f;
                for (index = 0; index < 3; ++index) {
                    spawn.x = player->collision_position.x + TrenchSpawnDistancePc;
                    spawn.y = player->collision_position.y + TrenchSpawnHeightPc;
                    spawn.z = player->collision_position.z;
                    offset.x = 0.0f;
                    offset.y = 0.0f;
                    offset.z = 0.0f;
                    switch (index) {
                        case 0:
                            offset.z = TrenchLateralOffsetPc;
                            break;
                        case 1:
                            offset.y = TrenchVerticalOffsetPc;
                            break;
                        case 2:
                            offset.z = -TrenchLateralOffsetPc;
                            break;
                    }
                    NuVecAdd(&spawn, &spawn, &offset);
                    if (trenchrun.midtro_played == 0 && index == 1)
                        model = VehicleBoltId7f196cPc;
                    else
                        model = id_TIEFIGHTER;
                    ship = AddDynamicCreature(model, &spawn, 0xc000, "TrenchBaddie", &player->ai_packet.path_info, NULL,
                                              0, NULL, NULL, 0, 1);
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
    if (((u8 *)LevFlag)[4] != 0)
        return;
    if (NuSpecialGetVisibilityFn(&LevHSpecial[0])) {
        PlaySfx((char *)EpisodeVI_ffield, NuSpecialGetDrawPos(&LevHSpecial[0]));
    } else {
        PlaySfx((char *)EpisodeVI_ffieldoff, NuSpecialGetDrawPos(&LevHSpecial[0]));
        ((u8 *)LevFlag)[4] = 1;
    }
}
