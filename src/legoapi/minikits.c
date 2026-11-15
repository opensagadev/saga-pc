#include "saga.h"
#include <stdio.h>

extern f32 KITPOS2YPc;

#ifdef __cplusplus
#endif

void MiniKits_Init(VARIPTR *buffer, VARIPTR *end) {
    (void)end;
    buffer->addr = (buffer->addr + 3) & ~3u;
    Char_MiniKit = buffer->void_ptr;
    buffer->addr += (u32)CHARCOUNT * 4u;
}

void MiniKit_Load(MINIKIT_PC *minikit, i32 id, VARIPTR *buffer, VARIPTR *end, void *unused) {
    char path[256];
    (void)unused;
    minikit->scene = NULL;
    minikit->pieces = NULL;
    minikit->piece_count = 0;
    minikit->area_index = -1;
    minikit->model_id = (i16)id;
    if (id == -1) {
        return;
    }
    NuStrCpy(path, "chars\\minikits\\");
    NuStrCat(path, CDataList[id].name);
    NuStrCat(path, "\\");
    NuStrCat(path, CDataList[id].name);
    NuStrCat(path, ".gsc");
    buffer->addr = (buffer->addr + 3) & ~3u;
    minikit->scene = NuGScnRead(buffer, *end, path);
}

static const char *MiniKitDirectionNamesPc[6] = {"NegX", "PosX", "NegY", "PosY", "NegZ", "PosZ"};

void MiniKit_InitPieces(MINIKIT_PC *minikit, i32 count, VARIPTR *buffer, VARIPTR *end) {
    i32 direction_counts[6];
    i32 index;
    i32 direction;
    char name[64];
    (void)end;
    if (minikit->scene != NULL) {
        for (direction = 0; direction < 6; ++direction) {
            direction_counts[direction] = 0;
        }
        buffer->addr = (buffer->addr + 3) & ~3u;
        minikit->pieces = buffer->void_ptr;
        minikit->piece_count = 0;
        for (index = 0; index < count; ++index) {
            for (direction = 0; direction < 6; ++direction) {
                sprintf(name, "%s_%s_%i", CDataList[minikit->model_id].name, MiniKitDirectionNamesPc[direction], index);
                if (NuSpecialFind(minikit->scene, &minikit->pieces[minikit->piece_count].special, name, 1) != 0) {
                    minikit->pieces[minikit->piece_count].matrix = *NuSpecialGetInstanceMtx(&minikit->pieces[minikit->piece_count].special);
                    minikit->pieces[minikit->piece_count].direction = (u8)direction;
                    minikit->pieces[minikit->piece_count].direction_index = (u8)direction_counts[direction]++;
                    ++minikit->piece_count;
                }
            }
        }
        sprintf(name, "%s_shadow", CDataList[minikit->model_id].name);
        NuSpecialFind(minikit->scene, &minikit->shadow, name, 1);
        if (minikit->piece_count > 0) {
            buffer->addr += minikit->piece_count * sizeof(MINIKIT_PIECE);
        } else {
            minikit->pieces = NULL;
        }
    } else {
        minikit->area_index = -1;
    }
}

void CharMiniKit_Draw(i32 id, NUMTX *matrix, i32 reflection_axis, f32 reflection_plane, f32 reflection_height) {
    MINIKIT_PC *kit;
    i32 i;
    if (Char_MiniKit == NULL) {
        return;
    }
    kit = (MINIKIT_PC *)Char_MiniKit[id];
    if (kit == NULL) {
        return;
    }
    for (i = 0; i < kit->piece_count; ++i) {
        NUMTX draw_matrix;
        NUMTX reflected;
        if (NuSpecialExistsFn(&kit->pieces[i].special) == 0) {
            continue;
        }
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

void CharacterMiniKits_Load(COLLECTION_BUILD_PC *collection, WORLDINFO *world, VARIPTR *buffer, VARIPTR *end) {
    WORLD_MINIKIT_READER *view = (WORLD_MINIKIT_READER *)world;
    i32 i;
    i32 character_id;
    u32 table_size;
    char path[256];
    if (Char_MiniKit == NULL) {
        return;
    }
    minikits_savecharacterbufferptr = characterbuffer_ptr;
    for (i = 0; i < CHARCOUNT; ++i) {
        Char_MiniKit[i] = 0;
    }
    buffer->addr = (buffer->addr + 3) & ~3u;
    view->pieces_by_id = buffer->void_ptr;
    table_size = CHARCOUNT * sizeof(MINIKIT_PC *);
    memset(view->pieces_by_id, 0, table_size);
    buffer->addr += table_size;
    for (i = 0; i < collection->count; ++i) {
        character_id = collection->list[i].id;
        if ((GCDataList[character_id].flags_094 & 0x400) != 0 || APICharacterLoaded(character_id) == NULL) {
            continue;
        }
        view->pieces_by_id[character_id] = buffer->void_ptr;
        buffer->addr += 0x18;
        NuStrCpy(path, "chars\\minikits\\");
        NuStrCat(path, CDataList[character_id].name);
        NuStrCat(path, "\\");
        NuStrCat(path, CDataList[character_id].name);
        NuStrCat(path, ".gsc");
        buffer->addr = (buffer->addr + 3) & ~3u;
        if (i >= 18) {
            view->pieces_by_id[character_id]->scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, path);
        } else {
            view->pieces_by_id[character_id]->scene = NuGScnRead(buffer, *end, path);
        }
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

void CharacterMiniKits_Dump(WORLDINFO *world) {
    WORLD_MINIKIT_READER *view = (WORLD_MINIKIT_READER *)world;
    i32 i;
    if (view->minikit.scene != NULL) {
        NuGScnRemove(view->minikit.scene);
        view->minikit.scene = NULL;
        view->minikit.pieces = NULL;
        view->minikit.piece_count = 0;
    }
    if (view->pieces_by_id == NULL) {
        return;
    }
    if (world->area != NULL && (world->area->flags & (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA)) == 5) {
        if (Char_MiniKit == NULL) {
            return;
        }
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

void CollectAllMiniKits(AREASAVE_s *save) {
    i32 i;
    if (save == NULL) {
        return;
    }
    for (i = 0; i < AREACOUNT; ++i) {
        save[i].minikit_count = (ADataList[i].flags & AREAFLAG_MINIKIT) != 0 ? 10 : 0;
        save[i].minikit_complete = 1;
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

void DrawMiniKitCount(f32 time, f32 scale, i32 amount, i32 total) {
    i32 model = ChallengeMode != 0 ? CharKitModelId : MiniKitModelId;
    f32 factor;
    f32 x;
    f32 y;
    WORLDINFO *world;
    char text[32];
    if (model == -1 || time <= 0.0f) {
        return;
    }
    factor = NuTrigTable[((i32)((f64)time * 16384.0) >> 1) & 0x7fff];
    x = (KITPOSX - KITPOS2X) * factor + KITPOS2X;
    y = (KITPOSY - KITPOS2YPc) * factor + KITPOS2YPc;
    world = WorldInfo_CurrentlyActive();
    if (world->level_objects[model].active != 0) {
        u16 angle = (u16)(i32)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
        u16 pitch = (u16)(i32)((f64)NU_SIN_LUT(angle * 2) * 1820.0);
        f32 size = PANEL_MINIKITSCALE * scale;
        DrawPanel3DObjectNoAlpha(x, y + PANEL_MINIKITY, 1.0f, size, size, size, pitch, angle, 0, &world->level_objects[model].special, 2);
    }
    sprintf(text, "%i/%i", amount, total);
    scale *= PANEL_MINIKITCOUNTSCALE;
    Text3D(text, x, y + PANEL_MINIKITCOUNTY, 1.0f, scale, scale, scale, 0, 255, 0, 127);
}

void MiniKit_GameMsg_Update(GAME_MESSAGE_PC *message) {
    message->parameter_e2 = (u16)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    message->parameter_e0 = (u16)(NU_SIN_LUT(message->parameter_e2 * 2) * 1820.0);
}

void MiniKit_GameMsg_End(GAME_MESSAGE_PC *message) {
    MiniKitScale = 2.0f;
    if (WorldInfo_CurrentlyActive()->area != NULL && (i16)message->icon != -1) {
        if ((i16)message->icon == MiniKitModelId) {
            ++AreaGlobals.progress_flags;
            if (AreaGlobals.progress_flags > AreaGlobals.area_phase) {
                AreaGlobals.progress_flags = AreaGlobals.area_phase;
            }
        } else if ((i16)message->icon == CharKitModelId && AreaGlobals.unknown_20 < AreaGlobals.unknown_1c) {
            ++AreaGlobals.unknown_20;
            if (AreaGlobals.unknown_20 == AreaGlobals.unknown_1c && AreaGlobals.unknown_1c >= 10) {
                EndChallenge(2, 1);
            }
        }
    }
    GameAudio_PlaySfx(0x26, NULL, 0, 0);
    NewRumbleAllPlayers(0.6f, 0.0f, 0, 0);
    GameCam_Judder(GameCam, -0.2f, 0, NULL);
}
