#include "saga.h"

void DeathStar2BattleFire_Draw(WORLDINFO *world) {
    u16 yaw = 0;
    u16 pitch = 0;
    NUVEC point = v000;
    NUMTX matrix;
    NUVEC difference;
    NUVEC average;
    SPLINEPOS position;
    f32 distance;
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
        distance = NuVecMagSqr(&difference);
        if (distance < fire_clip_dist)
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
        distance = NuVecMagSqr(&difference);
        if (distance < fire_clip_dist)
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
        distance = NuVecMagSqr(&difference);
        if (distance < fire_clip_dist)
            AddVariableShotDebrisEffectTimed1(world->game_debris_sys->entries[121].effect, &position.position, 33,
                                              FRAMETIME, pitch, yaw, NULL);
        MoveSplinePosition(&position, -40.0f);
    }
}

void __cdecl MiniKit_GameMsg_Update(GAME_MESSAGE_PC *message) {
    message->parameter_e2 = (u16)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    message->parameter_e0 = (u16)(NU_SIN_LUT(message->parameter_e2 * 2) * 1820.0);
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
    NewRumbleAllPlayers(0.6f, 0.0f, 0, 0);
    GameCam_Judder(GameCam, -0.2f, 0, NULL);
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
    u32 table_size;
    char path[256];
    if (Char_MiniKit == NULL)
        return;
    minikits_savecharacterbufferptr = characterbuffer_ptr;
    for (i = 0; i < CHARCOUNT; ++i)
        Char_MiniKit[i] = 0;
    buffer->addr = (buffer->addr + 3) & ~3u;
    view->pieces_by_id = (MINIKIT_PC **)buffer->void_ptr;
    table_size = CHARCOUNT * sizeof(MINIKIT_PC *);
    memset(view->pieces_by_id, 0, table_size);
    buffer->addr += table_size;
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
        if (i >= 18)
            view->pieces_by_id[character_id]->scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, path);
        else
            view->pieces_by_id[character_id]->scene = NuGScnRead(buffer, *end, path);
        view->pieces_by_id[character_id]->model_id = (i16)character_id;
        if (view->pieces_by_id[character_id]->scene != NULL) {
            MiniKit_InitPieces(view->pieces_by_id[character_id], 10, buffer, end);
            ((MINIKIT_PC **)Char_MiniKit)[character_id] = view->pieces_by_id[character_id];
        } else {
            view->pieces_by_id[character_id] = NULL;
            buffer->addr -= 0x18;
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
        NUMTX draw_matrix, reflected;
        if (NuSpecialExistsFn(&kit->pieces[i].special) == 0)
            continue;
        draw_matrix = kit->pieces[i].matrix;
        NuMtxMul(&draw_matrix, &draw_matrix, matrix);
        NuSpecialDrawAt(&kit->pieces[i].special, &draw_matrix);
        if (reflection_axis != 0 && reflection_plane != 2000000.0f &&
            MatrixReflection(&draw_matrix, reflection_axis, reflection_plane, reflection_height, &reflected) != 0) {
            NuRndrStartReflectionRender(0);
            NuSpecialDrawAt(&kit->pieces[i].special, &reflected);
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
            if (view->pieces_by_id[i] != NULL && view->pieces_by_id[i]->scene != NULL) {
                NuGScnRemove(view->pieces_by_id[i]->scene);
                view->pieces_by_id[i]->scene = NULL;
            }
            Char_MiniKit[i] = 0;
        }
        characterbuffer_ptr = minikits_savecharacterbufferptr;
        return;
    }
    for (i = 0; i < AREACOUNT; ++i) {
        if (view->pieces_by_id[i] != NULL && view->pieces_by_id[i]->scene != NULL) {
            NuGScnRemove(view->pieces_by_id[i]->scene);
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
    i32 row_counts[3];
    i32 row, i;
    f32 size, spacing, offset, x, y;
    if (FadeSys.fade != 0.0f || count <= 0)
        return;
    memset(row_counts, 0, sizeof(row_counts));
    row = 0;
    for (i = 0; i < count; ++i) {
        if (targets[i] == -1)
            ++row;
        else
            ++row_counts[row];
    }
    size = HudBossIconScalePc * MeleeRowsScalePc;
    spacing = MeleeRowsSpacingPc * MeleeRowsScalePc;
    offset = MeleeRowsScalePc * MeleeRowsYOffsetPc;
    x = (f32) - ((row_counts[0] - 1) * (f64)spacing * 0.5);
    y = MeleeRowsYPc - offset;
    row = 0;
    for (i = 0; i < count; ++i) {
        if (targets[i] == -1) {
            ++row;
            ++i;
            y += offset;
            x = (f32) - (row_counts[row] * (f64)spacing * 0.5);
            if ((((row_counts[row] - row_counts[row - 1] % 2) + row) & 1) == 0)
                x = (f32)((f64)spacing * 0.5 + x);
        }
        DrawCharIcon(targets[i], x, y, 0.0f, size, 0xa7, values[i], values[i], 1, NULL);
        x += spacing;
    }
}

void __cdecl DrawMeleeTargetsAlphaPc(i16 *targets, u8 *dimmed, f32 *values, i32 count) {
    f32 size, spacing, offset, x;
    i32 i;
    if (FadeSys.fade != 0.0f || count <= 0)
        return;
    size = HudBossIconScalePc * MeleeRowsScalePc;
    spacing = (f32)((f64)MeleeRowsSpacingPc * 0.5 * (f64)MeleeRowsScalePc);
    offset = (f32)((f64)MeleeRowsYOffsetPc * 0.5 * (f64)MeleeRowsScalePc);
    x = (f32)((f64)(1 - count) * (f64)spacing * 0.5);
    for (i = 0; i < count; ++i) {
        f32 y = (i & 1) != 0 ? offset + MeleeRowsYPc : MeleeRowsYPc - offset;
        if (values != NULL) {
            DrawCharIcon(targets[i], x, y, 0.0f, size, 0xa7, values[i], values[i], 1, NULL);
        } else {
            f32 alpha = dimmed[i] != 0 ? 0.25f : 1.0f;
            DrawCharIcon(targets[i], x, y, 0.0f, size, 0xa7, alpha, alpha, 1, NULL);
        }
        x += spacing;
    }
}
