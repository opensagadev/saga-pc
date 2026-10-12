#include "saga.h"

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

i32 HothBattle_StartNewWave() {
    i32 type;
    if (melee.field_0x2 != 0) {
        switch (melee.field_0x1) {
            case 1:
                melee.waves[0].field_0x18 = 9;
                melee.waves[0].field_0x19 = 9;
                melee.waves[0].character_id = id_ATST_LOWRES;
                NuStrCpy(melee.waves[0].name, "Probe");
                melee.creature_count = 1;
                if (g_lowEndLevelBehaviour != 0) {
                    melee.waves[0].field_0x18 = 5;
                    melee.waves[0].field_0x19 = 5;
                }
                break;
            case 2:
                melee.waves[0].field_0x18 = 9;
                melee.waves[0].field_0x19 = 9;
                melee.waves[0].character_id = id_ATST_LOWRES;
                NuStrCpy(melee.waves[0].name, "rider");
                melee.creature_count = 1;
                if (g_lowEndLevelBehaviour != 0) {
                    melee.waves[0].field_0x18 = 5;
                    melee.waves[0].field_0x19 = 5;
                }
                break;
            case 3:
                melee.waves[0].field_0x18 = 2;
                melee.waves[0].field_0x19 = 2;
                melee.waves[0].character_id = id_ATAT;
                NuStrCpy(melee.waves[0].name, "ATAT");
                melee.creature_count = 1;
                break;
            case 4:
                melee.waves[0].field_0x18 = 3;
                melee.waves[0].field_0x19 = 3;
                melee.waves[0].character_id = id_ATST_LOWRES;
                NuStrCpy(melee.waves[0].name, "Probe");
                melee.waves[1].field_0x18 = 5;
                melee.waves[1].field_0x19 = 5;
                melee.waves[1].character_id = id_ATST_LOWRES;
                NuStrCpy(melee.waves[1].name, "rider");
                melee.waves[2].field_0x18 = 1;
                melee.waves[2].field_0x19 = 1;
                melee.waves[2].character_id = id_ATAT;
                NuStrCpy(melee.waves[2].name, "ATAT");
                melee.creature_count = 3;
                if (g_lowEndLevelBehaviour != 0) {
                    melee.waves[0].field_0x18 = 2;
                    melee.waves[0].field_0x19 = 2;
                    melee.waves[1].field_0x18 = 2;
                    melee.waves[1].field_0x19 = 2;
                }
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
