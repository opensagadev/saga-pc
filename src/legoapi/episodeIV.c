#include "saga.h"
#include <string.h>
extern i32 gizbuildit_gizmotype_id;
struct panel_movement_pc_s;
struct panel_movement_pc_s *__cdecl GizPanel_FindByName(WORLDINFO *, char *);
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
struct panel_movement_pc_s *LevGizPanel = NULL;
static const char CloudCityEscapePanelNamePc[8] = "panel5";
static const char CloudCityEscapeGas3Pc[8] = "gas_3";
static const char CloudCityEscapeGas2Pc[8] = "gas_2";
static const char CloudCityEscapeGas1Pc[8] = "gas_1";
static const char CloudCityEscapeFightMessagePc[20] = "BobaFightStarted";
static const char CloudCityEscapeBuilditNamePc[12] = "buildit2";
static const char CloudCityEscapeBuiltMessagePc[12] = "Built_C3PO";
static const char CloudCityEscapeDeskSfxPc[20] = "env_ctrl_desk_on";
static const char CloudCityEscapeSteamSfxPc[16] = "env_steam_lp";

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
        if (packet != NULL && (packet->count != 0 || (packet->flags & 2) != 0))
            return 1;
    }
    player = Player[1];
    if (player != NULL && (WORLD->current_level != DEATHSTARBATTLED_LDATA ||
                           ObjInNarrowSock(player, WORLD->sock_sys, WORLD->level_idx) == 0)) {
        player = Player[1];
        packet = (TORPEDO_MUSIC_FLAGS_VIEW_PC *)player->torpedo;
        if (packet != NULL && (packet->count != 0 || (packet->flags & 2) != 0))
            return 1;
    }
    return 0;
}

void Asteroid_PartKill(PART_PC *part, i32 reason) {
    AddGameDebris(WORLD->game_debris_sys, 0x5d, &part->position);
    AddGameDebris(WORLD->game_debris_sys, 0x5e, &part->position);
    AddGameDebris(WORLD->game_debris_sys, 0x5f, &part->position);
    AddPartDebris(WORLD->part_debris_sys, 4, &part->position);
    if (part->field_206 == 1) {
        i32 type;
        switch (reason) {
            case 4:
                type = 0;
                break;
            case 5:
                type = 1;
                break;
            default:
                return;
        }
        AddMiscPickups(&part->position, type, 1000, 0);
    } else {
        AddMiscPickups(&part->position, -1, 0, 1);
    }
}

static __forceinline void AsteroidChaseD_SetTurretTarget(i32 index, NUVEC *position) {
    GIZTURRET_s *turret = StarDestroyerTurrets[index];
    if (turret != NULL && turret->field_0xe4 == NULL) {
        turret->field_0xe4 = position;
        turret->field_0x12c = 2;
    }
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

static void DrawFalconSpotLights(GAMEOBJECT *object) {
    NUMTX matrix;
    if ((u8)(object->field_280) > 1 || object->id != id_MILLENNIUMFALCON || WORLD->level_objects[0x127].active == 0)
        return;
    if (object->character_model->points_of_interest[4] != NULL) {
        matrix = object->locator_matrices[4];
        NuSpecialDrawAt(&WORLD->level_objects[0x127].special, &matrix);
    }
    spotLightA_yrot[object->field_280] += FRAMETIME / 5.0f;
    if (spotLightA_yrot[object->field_280] > 1.0f)
        spotLightA_yrot[object->field_280] -= 1.0f;
    spotLightA_zrot[object->field_280] += FRAMETIME / 5.0f;
    if (spotLightA_zrot[object->field_280] > 1.0f)
        spotLightA_zrot[object->field_280] -= 1.0f;
    if (object->character_model->points_of_interest[5] != NULL) {
        matrix = object->locator_matrices[5];
        NuSpecialDrawAt(&WORLD->level_objects[0x127].special, &matrix);
    }
    spotLightB_yrot[object->field_280] += FRAMETIME / 5.0f;
    if (spotLightB_yrot[object->field_280] > 1.0f)
        spotLightB_yrot[object->field_280] -= 1.0f;
    spotLightB_zrot[object->field_280] += FRAMETIME / 5.0f;
    if (spotLightB_zrot[object->field_280] > 1.0f)
        spotLightB_zrot[object->field_280] -= 1.0f;
}

void CloudCityEscapeC_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "gas_1_animin", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "gas_2_animin", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "gas_3_animin", 1);
}

/* Native 0x004e5170, 204 bytes. */
void __cdecl DeathStarEscapeB_Init(WORLDINFO *world) {
    NUINSTANIM_TIME_VIEW *animation;
    void *data;
    i32 index;
    DeathStarEscapeBNetPacketPc = (u8 *)SetLevelHack(4);
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, DeathStarEscapeBWindowCleanPc, NULL);
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], (char *)DeathStarEscapeBLiftUpPc, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], (char *)DeathStarEscapeBLiftDownPc, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], (char *)DeathStarEscapeBSpongePc, 1);
    fakeanimendframe[0] = 0.0f;
    for (index = 0; index < 2; ++index) {
        animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&LevHSpecial[index]);
        if (animation != NULL) {
            data = ((NUGSCN *)LevHSpecial[index].scene)->instance_animation_data[animation->animation_index];
            if (data != NULL)
                fakeanimendframe[0] = (f32)((f64)NuAnimEndFrameOld(data) + (f64)fakeanimendframe[0]);
        }
    }
}

/* Native 0x004e5240, 307 bytes. */
void __cdecl DeathStarEscapeB_Update(WORLDINFO *world) {
    NUINSTANIM_TIME_VIEW *animation[2] = {NULL, NULL};
    void *data[2] = {NULL, NULL};
    i32 index;
    if (netclient == 0)
        DeathStarEscapeBNetPacketPc[0] = (u8)(i32)LevAIMessage[0]->value;
    if (LevGameObject[0] == NULL && DeathStarEscapeBNetPacketPc[0] == 0)
        LevGameObject[0] = GetNamedGameObject(world->ai_sys, (char *)DeathStarEscapeBWasher1Pc);
    if (LevGameObject[1] == NULL && DeathStarEscapeBNetPacketPc[0] == 0)
        LevGameObject[1] = GetNamedGameObject(world->ai_sys, (char *)DeathStarEscapeBWasher2Pc);
    for (index = 0; index < 2; ++index) {
        animation[index] = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&LevHSpecial[index]);
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

void __cdecl CloudCityEscapeA_Init(WORLDINFO *world) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, (char *)CloudCityEscapeFightMessagePc, NULL);
    ((CLOUDCITY_LEVEL_FLAG_VIEW_PC *)LevFlag)->started = 0;
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], (char *)CloudCityEscapeGas1Pc, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], (char *)CloudCityEscapeGas2Pc, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], (char *)CloudCityEscapeGas3Pc, 1);
    LevGizPanel = GizPanel_FindByName(world, (char *)CloudCityEscapePanelNamePc);
}
void __cdecl CloudCityEscapeA_Reset(WORLDINFO *world) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, (char *)CloudCityEscapeFightMessagePc, NULL);
    LevAIMessage[1] = CheckGizAIMessage(gizaimessagesys, (char *)CloudCityEscapeBuiltMessagePc, NULL);
    LevGizmo[0] =
        (GIZMO_PC *)GizmoFindByName(world->gizmo_sys, gizbuildit_gizmotype_id, (char *)CloudCityEscapeBuilditNamePc);
}
void __cdecl CloudCityEscapeA_Update(WORLDINFO *world) {
    i32 i;
    NUINSTANIM_TIME_VIEW *animation;
    if (((CLOUDCITY_LEVEL_FLAG_VIEW_PC *)LevFlag)->started == 0 && NuSpecialGetVisibilityFn(&LevHSpecial[0]) != 0) {
        PlaySfx((char *)CloudCityEscapeDeskSfxPc, NuSpecialGetDrawPos(&LevHSpecial[0]));
        ((CLOUDCITY_LEVEL_FLAG_VIEW_PC *)LevFlag)->started = 1;
    }
    TerSurface[14].flags = 0x2002;
    if (LevGizPanel == NULL || (((CLOUDCITY_PANEL_FLAGS_VIEW_PC *)LevGizPanel)->flags & 2) == 0) {
        for (i = 1; i < 4; ++i) {
            if (NuSpecialExistsFn(&LevHSpecial[i])) {
                animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&LevHSpecial[i]);
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

/* Episode IV FixUpLevels callbacks (Mos Eisley, Blockade Runner, Death Star rescue and escape). */
i32 test_tb = 1;
extern i16 CharacterFixupIdPc007f1a3c;
extern i16 CharacterFixupIdPc007f1a40;
void __cdecl LevChatterSfx(char *name, NUVEC *position) {
    if (TerrainChatterWaitPc <= 0.0f && qrand() < 0x400) {
        PlaySfx(name, position);
        TerrainChatterWaitPc = (f32)((f64)qrand() * PlayerRandomScalePc * 2.0 + NuTimeBarColumnStep);
    }
}
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

static i32 BlockadeRunnerDDrawCreaturePc(PART_PC *part) {
    return 0;
}

static NUVEC BlockadeRunnerDEjectMomPc = {-0.5f, 0.0f, 0.0f};
static NUVEC BlockadeRunnerDEjectVecPc[2] = {{13.0f, 1.6f, 7.93f}, {13.0f, 1.6f, 6.55f}};

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
            NUINSTANIM_TIME_VIEW *animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&LevHSpecial[i]);
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
