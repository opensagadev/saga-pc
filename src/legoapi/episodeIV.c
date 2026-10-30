#include "saga.h"
#include <string.h>
#include <stdio.h>
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

static __forceinline void AsteroidChaseD_SetTurretTarget(i32 index, NUVEC *position) {
    if (StarDestroyerTurrets[index] != NULL && StarDestroyerTurrets[index]->field_0xe4 == NULL) {
        StarDestroyerTurrets[index]->field_0xe4 = position;
        StarDestroyerTurrets[index]->field_0x12c = 2;
    }
}

/* PC 0x0088a310 / 0x0088a314. */
i32 lastPlaying = 0;
NUSPECIALHANDLE specialIcon = {0};
extern i32 turret_gizmotype_id;
void __cdecl DrawMeleeTargetsNumber(i16 *targets, u8 *counts, i32 count, u8 reset, NUSPECIALHANDLE *special);

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

void __cdecl AsteroidChaseD_Panel(WORLDINFO *world) {
    i16 targets = -1;
    DrawMeleeTargetsNumber(&targets, &turretAliveCount, 1, 0, &specialIcon);
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

void CloudCityEscapeC_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "gas_1_animin", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "gas_2_animin", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "gas_3_animin", 1);
}

/* Native 0x004e5170, 204 bytes. */
void __cdecl DeathStarEscapeB_Init(WORLDINFO *world) {
    NUINSTANIM_TIME_VIEW *animation[2];
    void *data;
    i32 index;
    DeathStarEscapeBNetPacketPc = (i8 *)SetLevelHack(4);
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, DeathStarEscapeBWindowCleanPc, NULL);
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], (char *)DeathStarEscapeBLiftUpPc, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], (char *)DeathStarEscapeBLiftDownPc, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], (char *)DeathStarEscapeBSpongePc, 1);
    fakeanimendframe[0] = 0.0f;
    for (index = 0; index < 2; ++index) {
        animation[index] = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&LevHSpecial[index]);
        if (animation[index] != NULL) {
            data = ((NUGSCN *)LevHSpecial[index].scene)->instance_animation_data[animation[index]->animation_index];
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
        DeathStarEscapeBNetPacketPc[0] = (i8)LevAIMessage[0]->value;
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
        for (i = 1; i <= 3; ++i) {
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

/* PC 0x0088a554 / 0x0088a55c / 0x00800214 / 0x0080021c: Falcon spotlight phases. */
f32 spotLightA_yrot[2] = {0};
f32 spotLightA_zrot[2] = {0};
f32 spotLightB_yrot[2] = {0.5f, 0.5f};
f32 spotLightB_zrot[2] = {0.5f, 0.5f};

#define FALCON_SLOT(object) ((i8)(object)->field_280)
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

ASTEROID_PC asteroids[128] = {0};
i32 nasteroids = 0;
/* PC 0x00800290. */
i32 maxrotspd[3] = {0x1555, 0x38e, 0x16c};
FINAL_ASTEROID_PC finalAsteroid = {0};
ASTEROIDC_NETPACKET_PC *asteroidc_netpacket = NULL;
GIZMOBLOWUP_PC *classicBlowups[8] = {0};
u8 drawLights = 0;
NUMTX *mtxOrig = NULL;

static void AsteroidChase_FindPopBits(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "small_pop_bit1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "small_pop_bit2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "small_pop_bit3", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], "small_pop_bit4", 1);
}

static __forceinline void AsteroidSetAngVec(NUANGVEC *out, NUANG x, NUANG y, NUANG z) {
    out->x = x;
    out->y = y;
    out->z = z;
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

void __cdecl AsteroidChaseA_Init(WORLDINFO *world) {
    AsteroidChase_FindPopBits(world);
}

void __cdecl AsteroidChaseA_Reset(WORLDINFO *world) {
    Asteroids_Reset(world);
}

void __cdecl AsteroidChaseA_Update(WORLDINFO *world) {
    Asteroids_Update();
}

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

#define PLAYER_OBJ(i) ((GAMEOBJECT *)Player[i])
void __cdecl AsteroidChaseB_Update(WORLDINFO *world) {
    i32 i;
    Asteroids_Update();
    for (i = 0; i < 4; i++) {
        if (classicBlowups[i] != NULL && (classicBlowups[i]->status_flags & 1) != 0) {
            if ((((u8 *)LevFlag)[0] & (1 << i)) == 0) {
                ((u8 *)LevFlag)[0] |= (u8)(1 << i);
                AddMiscPickups(&classicBlowups[i]->position, -1, -1, 1);
            }
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
    f32 phase, target_x, target_y, height_difference;
    i32 roll, target_roll;
    i32 heading_right;
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
        heading_right = object->movement_facing_angle > 0x8000;
        phase = object->context_timer / TURNTIME;
        target_x = phase * (heading_right ? TrenchLaneOffsetPc : -TrenchLaneOffsetPc) +
                   (1.0f - phase) * (heading_right ? -TrenchLaneOffsetPc : TrenchLaneOffsetPc);
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
    object->position.z =
        SeekValF(object->position.z, object->movement_spline_offset.z + trenchrun.position.z, TrenchSeekZPc);
    NuVecSub(&object->velocity, &object->position, &object->start_position);
    NuVecScale(&object->velocity, &object->velocity, 1.0f / FRAMETIME);
    if ((object->character_data->game_character->flags_090 & 1) != 0) {
        roll = 0;
        if (object->character_context != 0x2a && object->character_context != 0x36 && object->character_context != 0x3a)
            roll = (i32)(object->velocity.z * TrenchRollScalePc);
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
    f32 base_height, lower, upper;
    i32 index, model, left_present, free_play;
    if (netclient == 0) {
        trench_player = player;
        trenchrun.position.x = trench_player->collision_position.x;
        base_height = TrenchBaseHeightPc;
        trenchrun.position.y = base_height;
        if (trench_player->collision_position.x < -500.0) {
            free_play = FreePlay;
            trenchrun.position.y = base_height + TrenchSpawnHeightPc;
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
        if (lower > trenchrun.position.z)
            trenchrun.position.z = lower;
        else if (upper < trenchrun.position.z)
            trenchrun.position.z = upper;
        if (aicreature_sets_alive[0] == 0) {
            trenchrun.spawn_timer -= FRAMETIME;
            if (trenchrun.spawn_timer <= 0.0f) {
                trenchrun.spawn_timer = 5.0f;
                for (index = 0; index < 3; ++index) {
                    if (index != 0)
                        trench_player = player;
                    spawn.x = trench_player->collision_position.x + TrenchSpawnDistancePc;
                    spawn.y = trench_player->collision_position.y + TrenchSpawnHeightPc;
                    spawn.z = trench_player->collision_position.z;
                    offset.x = 0.0f;
                    offset.y = 0.0f;
                    offset.z = 0.0f;
                    if (index == 0)
                        offset.z = TrenchLateralOffsetPc;
                    else if (index == 1)
                        offset.y = TrenchVerticalOffsetPc;
                    else if (index == 2)
                        offset.z = -TrenchLateralOffsetPc;
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
