#include "saga.h"
#include <stdio.h>

/* Retail unit 0x5999e0-0x59a595 (legoapi/minikits.c): definitions in retail address order. */

STATIC_ASSERT(sizeof(VARIPTR) == 4);
STATIC_ASSERT(sizeof(*Char_MiniKit) == 4);
extern f32 KITPOS2YPc;

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
#endif

void __cdecl MiniKits_Init(VARIPTR *buffer, VARIPTR *end) {
    (void)end;
    buffer->addr = (buffer->addr + 3) & ~3u;
    Char_MiniKit = (i32 *)buffer->void_ptr;
    buffer->addr += (u32)CHARCOUNT * 4u;
}

void __cdecl MiniKit_Load(MINIKIT_PC *minikit, i32 id, VARIPTR *buffer, VARIPTR *end, void *unused) {
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

void __cdecl MiniKit_InitPieces(MINIKIT_PC *minikit, i32 count, VARIPTR *buffer, VARIPTR *end) {
    i32 direction_counts[6];
    i32 index, direction;
    char name[64];
    (void)end;
    if (minikit->scene != NULL) {
        for (direction = 0; direction < 6; ++direction) {
            direction_counts[direction] = 0;
        }
        buffer->addr = (buffer->addr + 3) & ~3u;
        minikit->pieces = (MINIKIT_PIECE_PC *)buffer->void_ptr;
        minikit->piece_count = 0;
        for (index = 0; index < count; ++index) {
            for (direction = 0; direction < 6; ++direction) {
                sprintf(name, "%s_%s_%i", CDataList[minikit->model_id].name, MiniKitDirectionNamesPc[direction], index);
                if (NuSpecialFind(minikit->scene, &minikit->pieces[minikit->piece_count].special, name, 1) != 0) {
                    minikit->pieces[minikit->piece_count].matrix =
                        *NuSpecialGetInstanceMtx(&minikit->pieces[minikit->piece_count].special);
                    minikit->pieces[minikit->piece_count].direction = (u8)direction;
                    minikit->pieces[minikit->piece_count].direction_index = (u8)direction_counts[direction]++;
                    ++minikit->piece_count;
                }
            }
        }
        sprintf(name, "%s_shadow", CDataList[minikit->model_id].name);
        NuSpecialFind(minikit->scene, &minikit->shadow, name, 1);
        if (minikit->piece_count > 0) {
            buffer->addr += minikit->piece_count * sizeof(MINIKIT_PIECE_PC);
        } else {
            minikit->pieces = NULL;
        }
    } else {
        minikit->area_index = -1;
    }
}

void __cdecl CharMiniKit_Draw(i32 id, NUMTX *matrix, i32 reflection_axis, f32 reflection_plane, f32 reflection_height) {
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
        NUMTX draw_matrix, reflected;
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

void __cdecl CharacterMiniKits_Load(COLLECTION_BUILD_PC *collection, WORLDINFO *world, VARIPTR *buffer, VARIPTR *end) {
    WORLD_MINIKIT_READER_PC *view = (WORLD_MINIKIT_READER_PC *)world;
    i32 i, character_id;
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
    view->pieces_by_id = (MINIKIT_PC **)buffer->void_ptr;
    table_size = CHARCOUNT * sizeof(MINIKIT_PC *);
    memset(view->pieces_by_id, 0, table_size);
    buffer->addr += table_size;
    for (i = 0; i < collection->count; ++i) {
        character_id = collection->list[i].id;
        if ((GCDataList[character_id].flags_094 & 0x400) != 0 || APICharacterLoaded(character_id) == NULL) {
            continue;
        }
        view->pieces_by_id[character_id] = (MINIKIT_PC *)buffer->void_ptr;
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

void __cdecl CharacterMiniKits_Dump(WORLDINFO *world) {
    WORLD_MINIKIT_READER_PC *view = (WORLD_MINIKIT_READER_PC *)world;
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
    if (world->area != NULL && (world->area->flags & 5) == 5) {
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

/* Complete native0059a1e0..0059a414. */
void __cdecl DrawMiniKitCount(f32 time, f32 scale, i32 amount, i32 total) {
    i32 model = ChallengeMode != 0 ? CharKitModelId_PC : MiniKitModelId_PC;
    f32 factor, x, y;
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
        DrawPanel3DObjectNoAlpha(x, y + PANEL_MINIKITY, 1.0f, size, size, size, pitch, angle, 0,
                                 &world->level_objects[model].special, 2);
    }
    sprintf(text, "%i/%i", amount, total);
    scale *= PANEL_MINIKITCOUNTSCALE;
    Text3D(text, x, y + PANEL_MINIKITCOUNTY, 1.0f, scale, scale, scale, 0, 255, 0, 127);
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
            if (AreaGlobals_PC.progress_flags > AreaGlobals_PC.area_phase) {
                AreaGlobals_PC.progress_flags = AreaGlobals_PC.area_phase;
            }
        } else if ((i16)message->icon == CharKitModelId_PC && AreaGlobals_PC.field_20 < AreaGlobals_PC.field_1c) {
            ++AreaGlobals_PC.field_20;
            if (AreaGlobals_PC.field_20 == AreaGlobals_PC.field_1c && AreaGlobals_PC.field_1c >= 10) {
                EndChallenge(2, 1);
            }
        }
    }
    GameAudio_PlaySfx(0x26, NULL, 0, 0);
    NewRumbleAllPlayers(0.6f, 0.0f, 0, 0);
    GameCam_Judder(GameCam, -0.2f, 0, NULL);
}

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
