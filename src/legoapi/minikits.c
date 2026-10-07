#include "saga.h"

void __cdecl ResetMinikitCounter(void) {
    minikitCounter_C = 0;
    minikitCounter_A = 0;
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
