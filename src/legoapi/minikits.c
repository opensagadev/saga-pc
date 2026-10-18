#include "saga.h"

static const char HothBattleCBombMessagePc[20] = "BombGen_ATAT_Killed";
static const char HothBattleCIceBPc[8] = "ice_b";
static const char HothBattleCIceAPc[8] = "ice_a";
static const char HothBattleCObstaclePc[12] = "obstacle3";
static const char HothBattleCPickupPc[8] = "m_pup7";
static const char HothBattleCGreenBoltPc[16] = "trooper_green";
static const char HothBattleCRedBoltPc[12] = "trooper_red";
static const char HothBattleCMinifig11Pc[12] = "minifig_1_1";
static const char HothBattleCMinifig12Pc[12] = "minifig_1_2";
static const char HothBattleCMinifig13Pc[12] = "minifig_1_3";
static const char HothBattleCMinifig21Pc[12] = "minifig_2_1";
static const char HothBattleCMinifig22Pc[12] = "minifig_2_2";
static const char HothBattleCMinifig23Pc[12] = "minifig_2_3";

void __cdecl ResetMinikitCounter(void) {
    minikitCounter_C = 0;
    minikitCounter_A = 0;
}

void HothBattleB_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "snow_ball_1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "snow_ball_2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "snow_ball_3", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], "snow_ball_4", 1);
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

i32 HothBattle_StartNewWave() {
    i32 type;
    if (melee.field_0x2 != 0) {
        switch (melee.field_0x1) {
            case 1:
                melee.waves[0].field_0x18 = 11;
                melee.waves[0].field_0x19 = 11;
                melee.waves[0].character_id = VehicleBoltId7f18b4Pc;
                NuStrCpy(melee.waves[0].name, "Probe");
                melee.creature_count = 1;
                break;
            case 2:
                melee.waves[0].field_0x18 = 11;
                melee.waves[0].field_0x19 = 11;
                melee.waves[0].character_id = id_ATST_LOWRES;
                NuStrCpy(melee.waves[0].name, "rider");
                melee.creature_count = 1;
                break;
            case 3:
                melee.waves[0].field_0x18 = 2;
                melee.waves[0].field_0x19 = 2;
                melee.waves[0].character_id = id_ATAT;
                NuStrCpy(melee.waves[0].name, "ATAT");
                melee.creature_count = 1;
                break;
            case 4:
                melee.waves[0].field_0x18 = 4;
                melee.waves[0].field_0x19 = 4;
                melee.waves[0].character_id = VehicleBoltId7f18b4Pc;
                NuStrCpy(melee.waves[0].name, "Probe");
                melee.waves[1].field_0x18 = 6;
                melee.waves[1].field_0x19 = 6;
                melee.waves[1].character_id = id_ATST_LOWRES;
                NuStrCpy(melee.waves[1].name, "rider");
                melee.waves[2].field_0x18 = 1;
                melee.waves[2].field_0x19 = 1;
                melee.waves[2].character_id = id_ATAT;
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

void DeathStar2BattleFire_Draw(WORLDINFO *world) {
    u16 yaw = 0;
    u16 pitch = 0;
    NUVEC point = v000;
    NUMTX matrix;
    NUVEC difference;
    NUVEC average;
    SPLINEPOS position;
    if (Paused != 0)
        return;

    position = fireSplinePos;
    while (position.normalized_position > 0.0f) {
        NuMtxSetIdentity(&matrix);
        PointAlongSpline(LevelCodeSpline[0], position.normalized_position, &point, &yaw, &pitch, 0);
        Players_AveragePos(&average, NULL);
        difference.x = average.x - position.position.x;
        difference.y = average.y - position.position.y;
        difference.z = average.z - position.position.z;
        if (NuVecMagSqr(&difference) < fire_clip_dist)
            AddVariableShotDebrisEffectTimed1(world->game_debris_sys->entries[121].effect, &position.position, 33,
                                              FRAMETIME, pitch, yaw, NULL);
        MoveSplinePosition(&position, -40.0f);
    }

    position = fireBackPos;
    while (position.normalized_position < fireSplinePos.normalized_position) {
        NuMtxSetIdentity(&matrix);
        PointAlongSpline(LevelCodeSpline[0], position.normalized_position, &point, &yaw, &pitch, 0);
        Players_AveragePos(&average, NULL);
        difference.x = average.x - position.position.x;
        difference.y = average.y - position.position.y;
        difference.z = average.z - position.position.z;
        if (NuVecMagSqr(&difference) < fire_clip_dist)
            AddVariableShotDebrisEffectTimed1(world->game_debris_sys->entries[121].effect, &position.position, 67,
                                              FRAMETIME, pitch, yaw, NULL);
        MoveSplinePosition(&position, 40.0f);
    }

    position = fireBackPos;
    while (position.normalized_position > 0.0f) {
        NuMtxSetIdentity(&matrix);
        PointAlongSpline(LevelCodeSpline[0], position.normalized_position, &point, &yaw, &pitch, 0);
        Players_AveragePos(&average, NULL);
        difference.x = average.x - position.position.x;
        difference.y = average.y - position.position.y;
        difference.z = average.z - position.position.z;
        if (NuVecMagSqr(&difference) < fire_clip_dist)
            AddVariableShotDebrisEffectTimed1(world->game_debris_sys->entries[121].effect, &position.position, 33,
                                              FRAMETIME, pitch, yaw, NULL);
        MoveSplinePosition(&position, -40.0f);
    }
}

void __cdecl MiniKit_GameMsg_Update(GAME_MESSAGE_PC *message) {
    i32 angle = (i32)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    message->parameter_e2 = (u16)angle;
    message->parameter_e0 = (u16)(i32)((f64)NuTrigTable[(u16)angle & 0x7fff] * 1820.0);
}

void __cdecl MiniKit_GameMsg_End(GAME_MESSAGE_PC *message) {
    MiniKitScale = 2.0f;
    if (WorldInfo_CurrentlyActive()->area != NULL && (i16)message->icon != -1) {
        if ((i16)message->icon == MiniKitModelId_PC) {
            ++AreaGlobals_PC.progress_flags;
            if (AreaGlobals_PC.progress_flags > AreaGlobals_PC.area_phase)
                AreaGlobals_PC.progress_flags = AreaGlobals_PC.area_phase;
        } else if ((i16)message->icon == CharKitModelId_PC && AreaGlobals_PC.field_20 < AreaGlobals_PC.field_1c) {
            ++AreaGlobals_PC.field_20;
            if (AreaGlobals_PC.field_20 == AreaGlobals_PC.field_1c && AreaGlobals_PC.field_1c >= 10)
                EndChallenge(2, 1);
        }
    }
    GameAudio_PlaySfx(0x26, NULL, 0, 0);
    NewRumbleAllPlayers(0.6f, 0.0f, 0, 0.0f);
    GameCam_Judder(GameCam, -0.2f, 0, NULL);
}

void __cdecl CollectMinikit(NUVEC *position, char *name, i32 flags) {
    ADD_GAME_MESSAGE_PC message;
    NUVEC target;
    i32 area_id;
    (void)flags;
    if (WORLD->level_objects[0xce].active == 0)
        return;
    area_id = WORLD->area_index;
    target.x = KITPOSX;
    target.y = KITPOSY + PANEL_MINIKITY;
    target.z = 1.0f;
    message = AddGameMsg_Default;
    message.position = position;
    message.target_position = &target;
    message.scale = AreaPickupScale;
    message.target_scale = PANEL_MINIKITSCALE;
    message.flags = 0x2112d;
    message.duration = 1.0f;
    message.icon = 0xce;
    message.special = &WORLD->level_objects[0xce].special;
    message.tick_fn = MiniKit_GameMsg_Update;
    message.end_fn = MiniKit_GameMsg_End;
    message.allow_temporary = 1;
    AddGameMsg(&message);
    DrawMiniKitTime = 2.0f;
    if (area_id != -1 && Game.area_save[area_id].minikit_count < 10) {
        if (NuStrLen(name) <= 8)
            NuStrCpy(NewMiniPiece[AreaGlobals_PC.new_minikit_count].name, name);
        else
            NewMiniPiece[AreaGlobals_PC.new_minikit_count].name[0] = 0;
        NewMiniPiece[AreaGlobals_PC.new_minikit_count].level = (i16)WORLD->level_idx;
        ++AreaGlobals_PC.new_minikit_count;
        ++AreaGlobals_PC.area_phase;
        if (GAMEDEMO != 0) {
            if (AreaGlobals_PC.area_phase == 1)
                Hint_SetHintFromId(0x26d, 0, 0);
            else if (AreaGlobals_PC.area_phase == 10)
                Hint_SetHintFromId(0x26e, 0, 0);
        }
    }
    AddGameDebris(WORLD->game_debris_sys, 0x13, position);
    GameAudio_PlaySfxById(GetSfxId(PickupTypeLiteral758c14), position, 3, 0);
}

typedef struct world_minikit_reader_pc_s {
    u8 unknown_0000[0x2968];
    MINIKIT_PC minikit;
    u8 unknown_2980[4];
    MINIKIT_PC **pieces_by_id;
    u8 unknown_2988[0x158];
    u8 *hub_minikits;
} WORLD_MINIKIT_READER_PC;
STATIC_ASSERT(offsetof(WORLD_MINIKIT_READER_PC, minikit) == 0x2968);
STATIC_ASSERT(offsetof(WORLD_MINIKIT_READER_PC, pieces_by_id) == 0x2984);
STATIC_ASSERT(offsetof(WORLD_MINIKIT_READER_PC, hub_minikits) == 0x2ae0);
STATIC_ASSERT(offsetof(AREA_DATA_PC, minikit_id) == 0x8a);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, name) == 0xc);
STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
#ifdef __cplusplus
extern "C" {
#endif
    void __cdecl MiniKit_Load(MINIKIT_PC *, i32, VARIPTR *, VARIPTR *, void *);
    void __cdecl MiniKit_InitPieces(MINIKIT_PC *, i32, VARIPTR *, VARIPTR *);
    void __cdecl Hub_LoadAndFixUpMiniKits(WORLDINFO *, VARIPTR *, VARIPTR *);
    void __cdecl CharacterMiniKits_Load(COLLECTION_BUILD_PC *, WORLDINFO *, VARIPTR *, VARIPTR *);
#ifdef __cplusplus
}
#endif

void __cdecl CharacterMiniKits_Load(COLLECTION_BUILD_PC *collection, WORLDINFO *world, VARIPTR *buffer, VARIPTR *end) {
    WORLD_MINIKIT_READER_PC *view = (WORLD_MINIKIT_READER_PC *)world;
    i32 i, character_id;
    MINIKIT_PC *minikit;
    char path[256];
    if (Char_MiniKit == NULL)
        return;
    minikits_savecharacterbufferptr = characterbuffer_ptr;
    for (i = 0; i < CHARCOUNT; ++i)
        Char_MiniKit[i] = 0;
    buffer->addr = (buffer->addr + 3) & ~3u;
    view->pieces_by_id = (MINIKIT_PC **)buffer->void_ptr;
    memset(view->pieces_by_id, 0, CHARCOUNT * 4);
    buffer->addr += CHARCOUNT * 4;
    for (i = 0; i < collection->count; ++i) {
        character_id = collection->list[i].id;
        if ((GCDataList[character_id].flags_094 & 0x400) != 0 || APICharacterLoaded(character_id) == NULL)
            continue;
        view->pieces_by_id[character_id] = (MINIKIT_PC *)buffer->void_ptr;
        buffer->addr += 0x18;
        NuStrCpy(path, "chars\\minikits\\");
        NuStrCat(path, CDataList[character_id].name);
        NuStrCat(path, "\\");
        NuStrCat(path, CDataList[character_id].name);
        NuStrCat(path, ".gsc");
        buffer->addr = (buffer->addr + 3) & ~3u;
        if (i < 18)
            view->pieces_by_id[character_id]->scene = NuGScnRead(buffer, *end, path);
        else
            view->pieces_by_id[character_id]->scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, path);
        minikit = view->pieces_by_id[character_id];
        minikit->model_id = (i16)character_id;
        if (minikit->scene == NULL) {
            view->pieces_by_id[character_id] = NULL;
            buffer->addr -= 0x18;
        } else {
            MiniKit_InitPieces(minikit, 10, buffer, end);
            ((MINIKIT_PC **)Char_MiniKit)[character_id] = view->pieces_by_id[character_id];
        }
    }
}

void __cdecl CharMiniKit_Draw(i32 id, NUMTX *matrix, i32 reflection_axis, f32 reflection_plane, f32 reflection_height) {
    MINIKIT_PC *kit;
    i32 i;
    if (Char_MiniKit == NULL)
        return;
    kit = (MINIKIT_PC *)Char_MiniKit[id];
    if (kit == NULL)
        return;
    for (i = 0; i < kit->piece_count; ++i) {
        MINIKIT_PIECE_PC *piece = &kit->pieces[i];
        NUMTX draw_matrix, reflected;
        if (NuSpecialExistsFn(&piece->special) == 0)
            continue;
        draw_matrix = piece->matrix;
        NuMtxMul(&draw_matrix, &draw_matrix, matrix);
        NuSpecialDrawAt(&piece->special, &draw_matrix);
        if (reflection_axis != 0 && reflection_plane != 0.0f &&
            MatrixReflection(&draw_matrix, reflection_axis, reflection_plane, reflection_height, &reflected) != 0) {
            NuRndrStartReflectionRender(0);
            NuSpecialDrawAt(&piece->special, &reflected);
            NuRndrEndReflectionRender();
        }
    }
}

void __cdecl CharacterMiniKits_Dump(WORLDINFO *world) {
    WORLD_MINIKIT_READER_PC *view = (WORLD_MINIKIT_READER_PC *)world;
    i32 i;
    if (view->minikit.scene != NULL) {
        NuGScnRemove(view->minikit.scene);
        view->minikit.scene = NULL;
        view->minikit.pieces = NULL;
        view->minikit.piece_count = 0;
    }
    if (view->pieces_by_id == NULL)
        return;
    if (world->area != NULL && (world->area->flags & 5) == 5) {
        if (Char_MiniKit == NULL)
            return;
        for (i = 0; i < CHARCOUNT; ++i) {
            MINIKIT_PC *kit = view->pieces_by_id[i];
            if (kit != NULL && kit->scene != NULL) {
                NuGScnRemove(kit->scene);
                view->pieces_by_id[i]->scene = NULL;
            }
            Char_MiniKit[i] = 0;
        }
        characterbuffer_ptr = minikits_savecharacterbufferptr;
        return;
    }
    for (i = 0; i < AREACOUNT; ++i) {
        MINIKIT_PC *kit = view->pieces_by_id[i];
        if (kit != NULL && kit->scene != NULL) {
            NuGScnRemove(kit->scene);
            view->pieces_by_id[i]->scene = NULL;
        }
    }
}

i32 AllMiniKitsDone(AREASAVE_s *save) {
    i32 i;
    if (save == NULL) {
        return 1;
    }

    for (i = 0; i < AREACOUNT; ++i, ++save) {
        if ((ADataList[i].flags & AREAFLAG_MINIKIT) != 0 && save->minikit_complete == SAVE_INCOMPLETE) {
            return 0;
        }
    }
    return 1;
}

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

void __cdecl HothBattleC_DrawPc(WORLDINFO *world) {
    DrawMiniSnowTroopers(world);
}

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

f32 HothBattleEPanelAlphaPc[16] = {0};
HOTHBATTLEE_NETPACKET_PC *HothBattleENetPacketPc = NULL;
f32 MeleeRowsScalePc = 0.8f;
f32 MeleeRowsYPc = 0.475f;
f32 MeleeRowsSpacingPc = 0.15f;
f32 MeleeRowsYOffsetPc = -0.175f;
extern f32 HudBossIconScalePc;

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
STATIC_ASSERT(offsetof(HOTH_WORLD_SCRIPT_PC, processor) == 0x2b00);
STATIC_ASSERT(offsetof(HOTH_ANTINODE_PC, excluded_character_types) == 0x50);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet.locator) == 0x36c);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet.creature_set) == 0x378);
STATIC_ASSERT(offsetof(GAMEOBJECT, hostility_group) == 0x28d);
STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_radius) == 0x1dc);

void __cdecl DrawMeleeTargetsRowsPc(i16 *targets, char *defeated, f32 *values, i32 count) {
    i32 row_counts[3] = {0, 0, 0};
    i32 row = 0, i;
    f32 size, spacing, offset, x, y;
    if (FadeSys.fade != 0.0f || count <= 0)
        return;
    for (i = 0; i < count; ++i) {
        if (targets[i] == -1)
            ++row;
        else
            ++row_counts[row];
    }
    size = MeleeRowsScalePc * HudBossIconScalePc;
    spacing = MeleeRowsSpacingPc * MeleeRowsScalePc;
    offset = MeleeRowsScalePc * MeleeRowsYOffsetPc;
    x = (f32) - ((row_counts[0] - 1) * (f64)spacing * 0.5);
    y = MeleeRowsYPc - offset;
    row = 0;
    for (i = 0; i < count; ++i) {
        if (targets[i] == -1) {
            i32 previous_row = row++;
            ++i;
            y += offset;
            x = (f32) - (row_counts[row] * (f64)spacing * 0.5);
            if (((row_counts[row] - row_counts[previous_row] % 2) + row) & 1) {
            } else {
                x = (f32)((f64)spacing * 0.5 + x);
            }
        }
        DrawCharIcon(targets[i], x, y, 0.0f, size, 0xa7, values[i], values[i], 1, NULL);
        x += spacing;
    }
}

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

void __cdecl HothBattleE_DrawPc(WORLDINFO *world) {
    DrawMiniSnowTroopers(world);
}

void __cdecl HothBattleE_Update(WORLDINFO *world) {
    UpdateMiniSnowTroopers(world);
    if (netclient == 0)
        HothBattleE_UpdateWave();
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
