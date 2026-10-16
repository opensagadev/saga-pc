#include "saga.h"
typedef struct customiser_names_pc_s {
    u8 unknown_00[0x6c];
    i16 character_ids[2];
    char display_names[2][128];
} CUSTOMISER_NAMES_PC;
STATIC_ASSERT(offsetof(CUSTOMISER_NAMES_PC, character_ids) == 0x6c);
STATIC_ASSERT(offsetof(CUSTOMISER_NAMES_PC, display_names) == 0x70);
STATIC_ASSERT(sizeof(CUSTOMISER_NAMES_PC) == 0x170);
STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, name_id) == 0);
void __cdecl Customiser_InitNames(CUSTOMISER_LAYERS_PC *customiser) {
    CUSTOMISER_NAMES_PC *view = (CUSTOMISER_NAMES_PC *)customiser;
    i32 index;
    if (customiser == NULL)
        return;
    for (index = 0; index < 2; ++index) {
        if (view->character_ids[index] != -1) {
            i32 text = CDataList[view->character_ids[index]].name_id;
            if (text != -1) {
                NuStrCpy(view->display_names[index], TTab[text]);
                TTab[text] = view->display_names[index];
            }
        }
    }
}

typedef struct customiser_anims_load_pc_s {
    u8 unknown_00[0x6c];
    i16 character_ids[2];
    u8 unknown_70[0x198];
    i32 model_texture_ids[18];
    u8 unknown_250[0x818];
    u16 *animation_ids_to_load;
} CUSTOMISER_ANIMS_LOAD_PC;
STATIC_ASSERT(offsetof(CUSTOMISER_ANIMS_LOAD_PC, character_ids) == 0x6c);
STATIC_ASSERT(offsetof(CUSTOMISER_ANIMS_LOAD_PC, model_texture_ids) == 0x208);
STATIC_ASSERT(offsetof(CUSTOMISER_ANIMS_LOAD_PC, animation_ids_to_load) == 0xa68);
STATIC_ASSERT(sizeof(CHARACTERANIM_CONFIG_PC) == 0x4c);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, flags) == 4);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, action_id) == 8);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, animations) == 0x10);
void __cdecl Customiser_SetAnimsToLoad(CUSTOMISER_LAYERS_PC *input, i32 enabled) {
    CUSTOMISER_ANIMS_LOAD_PC *customiser = (CUSTOMISER_ANIMS_LOAD_PC *)input;
    i32 character;
    if (customiser == NULL)
        return;
    for (character = 0; character < 2; ++character) {
        CHARACTERANIM_CONFIG_PC *animation = CDataList[customiser->character_ids[character]].animations;
        if (animation == NULL)
            continue;
        for (; animation->name != NULL; ++animation) {
            const u16 *animation_ids;
            if (enabled == 0) {
                animation->flags &= ~0x8000u;
                continue;
            }
            animation->flags |= 0x8000u;
            animation_ids = customiser->animation_ids_to_load;
            if (animation_ids == NULL)
                continue;
            for (; *animation_ids != 0xffff; ++animation_ids) {
                if (*animation_ids == (u16)animation->action_id) {
                    animation->flags &= ~0x8000u;
                    break;
                }
            }
        }
    }
}
void __cdecl Customiser_ResetModelTextureIDs(CUSTOMISER_LAYERS_PC *input) {
    CUSTOMISER_ANIMS_LOAD_PC *customiser = (CUSTOMISER_ANIMS_LOAD_PC *)input;
    if (customiser == NULL)
        return;
    customiser->model_texture_ids[0] = 0;
    customiser->model_texture_ids[1] = 0;
    customiser->model_texture_ids[2] = 0;
    customiser->model_texture_ids[3] = 0;
    customiser->model_texture_ids[4] = 0;
    customiser->model_texture_ids[5] = 0;
    customiser->model_texture_ids[6] = 0;
    customiser->model_texture_ids[7] = 0;
    customiser->model_texture_ids[8] = 0;
    customiser->model_texture_ids[9] = 0;
    customiser->model_texture_ids[10] = 0;
    customiser->model_texture_ids[11] = 0;
    customiser->model_texture_ids[12] = 0;
    customiser->model_texture_ids[13] = 0;
    customiser->model_texture_ids[14] = 0;
    customiser->model_texture_ids[15] = 0;
    customiser->model_texture_ids[16] = 0;
    customiser->model_texture_ids[17] = 0;
}

#include <string.h>
typedef struct custom_piece_config_pc_s {
    char *name;
    i16 character_id;
    i16 icon_character_id;
    i16 weapon_model;
    i16 unknown_indices_0a[3];
    i8 field_10;
    i8 collection_type;
    u16 availability_flags;
    u32 model_flags;
    u32 gameplay_flags;
    u8 unknown_1c[12];
} CUSTOM_PIECE_CONFIG_PC;
STATIC_ASSERT(sizeof(CUSTOM_PIECE_CONFIG_PC) == 0x28);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_CONFIG_PC, name) == 0);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_CONFIG_PC, character_id) == 4);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_CONFIG_PC, icon_character_id) == 6);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_CONFIG_PC, weapon_model) == 8);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_CONFIG_PC, unknown_indices_0a) == 0xa);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_CONFIG_PC, field_10) == 0x10);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_CONFIG_PC, collection_type) == 0x11);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_CONFIG_PC, availability_flags) == 0x12);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_CONFIG_PC, model_flags) == 0x14);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_CONFIG_PC, gameplay_flags) == 0x18);
typedef struct custom_category_config_pc_s {
    char *name;
    u8 uses_special;
    i8 material_tag;
    u8 material_indices[2];
    char *scene_file;
} CUSTOM_CATEGORY_CONFIG_PC;
STATIC_ASSERT(sizeof(CUSTOM_CATEGORY_CONFIG_PC) == 12);
STATIC_ASSERT(offsetof(CUSTOM_CATEGORY_CONFIG_PC, name) == 0);
STATIC_ASSERT(offsetof(CUSTOM_CATEGORY_CONFIG_PC, uses_special) == 4);
STATIC_ASSERT(offsetof(CUSTOM_CATEGORY_CONFIG_PC, material_tag) == 5);
STATIC_ASSERT(offsetof(CUSTOM_CATEGORY_CONFIG_PC, material_indices) == 6);
STATIC_ASSERT(offsetof(CUSTOM_CATEGORY_CONFIG_PC, scene_file) == 8);
typedef struct custom_save_config_pc_s {
    u16 pieces[9];
    u8 unknown_12[0x26];
    u16 secondary_pieces[9];
    u8 unknown_4a[0x26];
} CUSTOM_SAVE_CONFIG_PC;
STATIC_ASSERT(offsetof(CUSTOM_SAVE_CONFIG_PC, pieces) == 0);
STATIC_ASSERT(offsetof(CUSTOM_SAVE_CONFIG_PC, secondary_pieces) == 0x38);
STATIC_ASSERT(sizeof(CUSTOM_SAVE_CONFIG_PC) == 0x70);
typedef struct customiser_config_pc_s {
    CUSTOM_CATEGORY_CONFIG_PC *categories[9];
    CUSTOM_PIECE_CONFIG_PC *piece_sets[9];
    i32 piece_counts[9];
    i16 character_ids[2];
    u8 unknown_70[0x100];
    i32(__cdecl *piece_available)(CUSTOMPIECE_LAYERS_PC *);
    CUSTOM_SAVE_CONFIG_PC *save;
    u8 unknown_178[0xd8];
    i8 locator_indices[9];
    u8 unknown_259[3];
    i8 layer_indices[9];
    u8 unknown_265[0x803];
    u16 *animation_ids_to_load;
    u8 unknown_a6c[0x1bc];
    u16 default_pieces[2][10];
    f32 locator_x_offsets[9];
    f32 locator_y_offsets[9];
} CUSTOMISER_CONFIG_PC;
STATIC_ASSERT(sizeof(CUSTOMISER_CONFIG_PC) == 0xc98);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, categories) == 0);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, piece_sets) == 0x24);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, piece_counts) == 0x48);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, character_ids) == 0x6c);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, piece_available) == 0x170);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, save) == 0x174);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, locator_indices) == 0x250);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, layer_indices) == 0x25c);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, animation_ids_to_load) == 0xa68);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, default_pieces) == 0xc28);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, locator_x_offsets) == 0xc50);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, locator_y_offsets) == 0xc74);
STATIC_ASSERT(offsetof(NUFPAR_PC, word_buffer_ptr) == 0x510);
static CUSTOM_CATEGORY_CONFIG_PC CustomiserSetDataPrivatePc[9] = {
    {"hat_hair", 1, -1, {1, 0}, "chars\\weirdo\\all\\hat_hair_all.gsc"},
    {"head", 1, -1, {0, 0}, "chars\\weirdo\\all\\head_all.gsc"},
    {NULL, 0, -1, {0, 0}, NULL},
    {"arm", 0, 2, {1, 0}, NULL},
    {"hand", 0, 4, {0, 0}, NULL},
    {"cape", 0, 3, {2, 0}, NULL},
    {"body", 0, 1, {3, 0}, NULL},
    {"under_pants", 0, 6, {1, 0}, NULL},
    {"leg", 0, 5, {2, 0}, NULL}};
i32 __cdecl CharVariant_Find(char *name);
i32 __cdecl Game_100PercentComplete(void);
void __cdecl Customiser_Dump(void) {
}
static i32 CustomiserFindSetFromNamePrivatePc(char *name) {
    if (NuStrICmp(name, "hathair") == 0)
        return 0;
    if (NuStrICmp(name, "head") == 0)
        return 1;
    if (NuStrICmp(name, "cape") == 0)
        return 5;
    if (NuStrICmp(name, "body") == 0)
        return 6;
    if (NuStrICmp(name, "arms") == 0)
        return 3;
    if (NuStrICmp(name, "hands") == 0)
        return 4;
    if (NuStrICmp(name, "weapon") == 0)
        return 2;
    if (NuStrICmp(name, "underpants") == 0)
        return 7;
    if (NuStrICmp(name, "legs") == 0)
        return 8;
    return -1;
}
static i32 __cdecl CustomiserPieceAvailableDefaultPrivatePc(CUSTOMPIECE_LAYERS_PC *input) {
    CUSTOM_PIECE_CONFIG_PC *piece = (CUSTOM_PIECE_CONFIG_PC *)input;
    if ((piece->availability_flags & 0x180) != 0 && Game_100PercentComplete() == 0)
        return 0;
    return 1;
}
void __cdecl Customiser_CopyDefaultPiecesToSave(CUSTOMISER_LAYERS_PC *input, void *save_input) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOM_SAVE_CONFIG_PC *save = (CUSTOM_SAVE_CONFIG_PC *)save_input;
    if (customiser == NULL)
        return;
    if (save == NULL) {
        save = customiser->save;
        if (save == NULL)
            return;
    }
    save->pieces[0] = customiser->default_pieces[0][0];
    save->pieces[1] = customiser->default_pieces[0][1];
    save->pieces[2] = customiser->default_pieces[0][2];
    save->pieces[3] = customiser->default_pieces[0][3];
    save->pieces[4] = customiser->default_pieces[0][4];
    save->pieces[5] = customiser->default_pieces[0][5];
    save->pieces[6] = customiser->default_pieces[0][6];
    save->pieces[7] = customiser->default_pieces[0][7];
    save->pieces[8] = customiser->default_pieces[0][8];
    save->secondary_pieces[0] = customiser->default_pieces[1][0];
    save->secondary_pieces[1] = customiser->default_pieces[1][1];
    save->secondary_pieces[2] = customiser->default_pieces[1][2];
    save->secondary_pieces[3] = customiser->default_pieces[1][3];
    save->secondary_pieces[4] = customiser->default_pieces[1][4];
    save->secondary_pieces[5] = customiser->default_pieces[1][5];
    save->secondary_pieces[6] = customiser->default_pieces[1][6];
    save->secondary_pieces[7] = customiser->default_pieces[1][7];
    save->secondary_pieces[8] = customiser->default_pieces[1][8];
}
CUSTOMISER_LAYERS_PC *__cdecl Customiser_Configure(char *filename, VARIPTR *buffer, VARIPTR *buffer_end,
                                                   i32 first_character, i32 second_character,
                                                   i32 (*available)(CUSTOMPIECE_LAYERS_PC *),
                                                   void (*configure_piece)(CUSTOMPIECE_LAYERS_PC *, NUFPAR_PC *),
                                                   i32 (*weapon_from_name)(char *), void *save_input, i16 *animations) {
    NUFPAR_PC *parser;
    CUSTOMISER_CONFIG_PC *customiser;
    CUSTOM_SAVE_CONFIG_PC *save = (CUSTOM_SAVE_CONFIG_PC *)save_input;
    i32 category, index;
    i32 locators[9], layers[9];
    f32 x_offsets[9] = {0}, y_offsets[9] = {0};
    u8 categories[440];
    CUSTOM_PIECE_CONFIG_PC pieces[440];
    char *names;
    i32 count = 0;
    (void)buffer_end;
    if (first_character == -1 && second_character == -1)
        return NULL;
    if (Game_Customiser != NULL) {
        Customiser_Dump();
        return NULL;
    }
    parser = NuFParCreate(filename);
    if (parser == NULL)
        return NULL;

    for (category = 0; category < 9; ++category) {
        locators[category] = -1;
        layers[category] = -1;
    }
    memset(pieces, 0, sizeof(pieces));
    names = buffer->char_ptr;
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0)
            break;
        category = CustomiserFindSetFromNamePrivatePc(parser->word_buffer_ptr);
        if (category != -1 && NuFParGetWord(parser) != 0) {
            CUSTOM_PIECE_CONFIG_PC *piece;
            categories[count] = category;
            piece = &pieces[count];
            piece->name = names;
            piece->field_10 = -1;
            piece->collection_type = -1;
            piece->availability_flags = 0;
            piece->model_flags = 0;
            piece->gameplay_flags = 0;
            piece->weapon_model = -1;
            piece->character_id = -1;
            piece->icon_character_id = -1;
            piece->unknown_indices_0a[0] = -1;
            piece->unknown_indices_0a[1] = -1;
            piece->unknown_indices_0a[2] = -1;
            NuStrCpy(names, parser->word_buffer_ptr);
            names += NuStrLen(parser->word_buffer_ptr) + 1;
            NuStrLen(parser->word_buffer_ptr);
            if (CustomiserSetDataPrivatePc[category].name == NULL && weapon_from_name != NULL)
                piece->weapon_model = weapon_from_name(piece->name);
            while (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "from") == 0) {
                    if (NuFParGetWord(parser) != 0)
                        piece->character_id = CharIDFromName(parser->word_buffer_ptr);
                } else if (NuStrICmp(parser->word_buffer_ptr, "icon_from") == 0) {
                    if (NuFParGetWord(parser) != 0)
                        piece->icon_character_id = CharIDFromName(parser->word_buffer_ptr);
                } else if (NuStrICmp(parser->word_buffer_ptr, "from_variant") == 0) {
                    if (NuFParGetWord(parser) != 0)
                        piece->collection_type = CharVariant_Find(parser->word_buffer_ptr);
                } else if (NuStrICmp(parser->word_buffer_ptr, "no_hat") == 0) {
                    piece->availability_flags |= 1;
                } else if (NuStrICmp(parser->word_buffer_ptr, "hat_off") == 0) {
                    piece->availability_flags |= 0x20;
                } else if (NuStrICmp(parser->word_buffer_ptr, "cape_off") == 0) {
                    piece->availability_flags |= 0x40;
                } else if (NuStrICmp(parser->word_buffer_ptr, "helmet") == 0) {
                    piece->availability_flags |= 2;
                } else if (NuStrICmp(parser->word_buffer_ptr, "big_head") == 0) {
                    piece->availability_flags |= 4;
                } else if (NuStrICmp(parser->word_buffer_ptr, "big_hat") == 0) {
                    piece->availability_flags |= 8;
                } else if (NuStrICmp(parser->word_buffer_ptr, "not_demo") == 0) {
                    piece->availability_flags |= 0x10;
                } else if (NuStrICmp(parser->word_buffer_ptr, "100_percent") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        i32 side = NuAToI(parser->word_buffer_ptr);
                        if (side == 1)
                            piece->availability_flags |= 0x80;
                        else if (side == 2)
                            piece->availability_flags |= 0x100;
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "default") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        i32 side = NuAToI(parser->word_buffer_ptr);
                        if (side == 1)
                            piece->availability_flags |= 0x200;
                        else if (side == 2)
                            piece->availability_flags |= 0x400;
                    }
                } else if (configure_piece != NULL) {
                    configure_piece((CUSTOMPIECE_LAYERS_PC *)piece, parser);
                }
            }
            ++count;
        } else if (NuStrICmp(parser->word_buffer_ptr, "locator") == 0) {
            if (NuFParGetWord(parser) != 0) {
                category = CustomiserFindSetFromNamePrivatePc(parser->word_buffer_ptr);
                if (category != -1) {
                    i32 locator = NuFParGetInt(parser);
                    locators[category] = (u32)locator < 16 ? locator : -1;
                    if ((u32)locator < 16) {
                        while (NuFParGetWord(parser) != 0) {
                            if (NuStrICmp(parser->word_buffer_ptr, "xoffset") == 0)
                                x_offsets[category] = NuFParGetFloat(parser);
                            else if (NuStrICmp(parser->word_buffer_ptr, "yoffset") == 0)
                                y_offsets[category] = NuFParGetFloat(parser);
                        }
                    }
                }
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "layer") == 0) {
            if (NuFParGetWord(parser) != 0) {
                category = CustomiserFindSetFromNamePrivatePc(parser->word_buffer_ptr);
                if (category != -1) {
                    i32 layer = NuFParGetInt(parser);
                    layers[category] = (u32)layer < 32 ? layer : -1;
                }
            }
        }
        if (count >= 440)
            break;
    }
    NuFParDestroy(parser);
    if (count == 0)
        return NULL;

    buffer->addr = ((u32)names + 3) & ~3u;
    customiser = (CUSTOMISER_CONFIG_PC *)buffer->void_ptr;
    memset(customiser, 0, sizeof(*customiser));
    buffer->addr = (buffer->addr + sizeof(*customiser) + 3) & ~3u;
    customiser->character_ids[0] = first_character;
    customiser->character_ids[1] = second_character;
    customiser->piece_available = available != NULL ? available : CustomiserPieceAvailableDefaultPrivatePc;
    customiser->save = save;
    customiser->animation_ids_to_load = (u16 *)animations;
    for (category = 0; category < 9; ++category) {
        CUSTOM_PIECE_CONFIG_PC *output = (CUSTOM_PIECE_CONFIG_PC *)buffer->void_ptr;
        customiser->categories[category] = &CustomiserSetDataPrivatePc[category];
        customiser->piece_sets[category] = output;
        customiser->piece_counts[category] = 0;
        customiser->locator_indices[category] = locators[category];
        customiser->layer_indices[category] = layers[category];
        customiser->locator_x_offsets[category] = x_offsets[category];
        customiser->locator_y_offsets[category] = y_offsets[category];
        for (index = 0; index < count; ++index) {
            if (categories[index] == category) {
                PCByteCopy_candidate(output, &pieces[index], sizeof(*output));
                ++customiser->piece_counts[category];
                ++output;
            }
        }
        if (customiser->piece_counts[category] > 0)
            buffer->addr = ((u32)output + 3) & ~3u;
    }
    if (customiser->save == NULL) {
        customiser->save = (CUSTOM_SAVE_CONFIG_PC *)buffer->void_ptr;
        buffer->addr = (buffer->addr + 0x73) & ~3u;
    }
    for (category = 0; category < 9; ++category) {
        i32 first_found = 0, second_found = 0;
        const i32 piece_count = customiser->piece_counts[category];
        for (index = 0; index < piece_count && !(first_found && second_found); ++index) {
            const u16 flags = customiser->piece_sets[category][index].availability_flags;
            if ((flags & 0x200) != 0) {
                customiser->default_pieces[0][category] = index;
                first_found = 1;
            }
            if ((flags & 0x400) != 0) {
                customiser->default_pieces[1][category] = index;
                second_found = 1;
            }
        }
    }
    Customiser_CopyDefaultPiecesToSave((CUSTOMISER_LAYERS_PC *)customiser, NULL);
    Game_Customiser = customiser;
    return (CUSTOMISER_LAYERS_PC *)customiser;
}

typedef struct customiser_game_setting_pc_s {
    char *name;
    u32 model_flags;
    u32 gameplay_flags;
} CUSTOMISER_GAME_SETTING_PC;
STATIC_ASSERT(sizeof(CUSTOMISER_GAME_SETTING_PC) == 12);
STATIC_ASSERT(offsetof(CUSTOMISER_GAME_SETTING_PC, name) == 0);
STATIC_ASSERT(offsetof(CUSTOMISER_GAME_SETTING_PC, model_flags) == 4);
STATIC_ASSERT(offsetof(CUSTOMISER_GAME_SETTING_PC, gameplay_flags) == 8);
static CUSTOMISER_GAME_SETTING_PC CustomiserGameSettingsPrivatePc[] = {{"bountyhunter", 0x1000000, 0},
                                                                       {"jedi", 8, 0},
                                                                       {"sith", 0xc, 2},
                                                                       {"blaster", 0x100080, 0x40000000},
                                                                       {"alreadygothat", 0, 0x10},
                                                                       {"stormtrooperhelmet", 0, 0x40000},
                                                                       {NULL, 0, 0}};
i32 __cdecl Collection_GotAnyOfType(i32 type, u32 flags);
void __cdecl PermanentCustomPieceConfigPc(CUSTOMPIECE_LAYERS_PC *input, NUFPAR_PC *parser) {
    CUSTOM_PIECE_CONFIG_PC *piece = (CUSTOM_PIECE_CONFIG_PC *)input;
    CUSTOMISER_GAME_SETTING_PC *setting;
    for (setting = CustomiserGameSettingsPrivatePc; setting->name != NULL; ++setting) {
        if (NuStrICmp(parser->word_buffer_ptr, setting->name) == 0) {
            piece->model_flags |= setting->model_flags;
            piece->gameplay_flags |= setting->gameplay_flags;
            break;
        }
    }
}
i32 __cdecl PermanentCustomPieceAvailablePc(CUSTOMPIECE_LAYERS_PC *input) {
    CUSTOM_PIECE_CONFIG_PC *piece = (CUSTOM_PIECE_CONFIG_PC *)input;
    if (GAMEDEMO != 0) {
        if ((piece->availability_flags & 0x10) != 0)
            return 0;
    } else {
        u32 required_flags;
        if ((piece->availability_flags & 0x180) != 0 && Game_100PercentComplete() == 0)
            return 0;
        if (piece->character_id != -1) {
            if (InCollectList_Index(piece->character_id, NULL, 0) == -1)
                return 1;
            if (Collection_Got(piece->character_id) == 0)
                return 0;
        }
        required_flags = piece->model_flags & 0xc;
        if (required_flags != 0 && Collection_GotAnyOfType(-1, required_flags) == 0)
            return 0;
        if (piece->collection_type != -1 && Collection_GotAnyOfType(piece->collection_type, 0) == 0)
            return 0;
    }
    return 1;
}

void __cdecl Customiser_GetActiveWeirdoIndex(i32 *index, i32 *count) {
    if (MissionMenuPacketPc.active_player[0] != 0) {
        if (MissionMenuPacketPc.active_player[1] == 0) {
            *index = 0;
            *count = 1;
            return;
        }
    } else if (MissionMenuPacketPc.active_player[1] != 0) {
        *index = 1;
        *count = 1;
        return;
    }
    *index = 0;
    *count = 2;
}

void CustomiserMenu_End() {
    LevLock[1] = 1;
    LevTime[1] = 2.0f;
    MenuRememberCursor(&GameMenu[GameMenuLevel]);
    MenuReset();
    ResetTimer(&JoinInTimer, 0.0f);
    CustomiseMode[0] = 2;
    CustomiseMode[1] = 2;
    if (GAMEDEMO != 0) {
        GAMEDEMO = 2;
        NewLData = HUB_LDATA;
        NextArea_FreePlay = 1;
        FreePlay = 1;
        makeplayerlist_freeplay = 1;
        makefreeplaymodellist = 1;
    }
}

void __cdecl Detonators_Reset(void) {
    memset(Detonator, 0, sizeof(Detonator));
}

static __inline i32 DetonatorPlacementSlot(GAMEOBJECT *object) {
    i32 count = 0;
    i32 available = -1;
    DetonatorConsiderPlacementSlot(0, object, count, available);
    DetonatorConsiderPlacementSlot(1, object, count, available);
    DetonatorConsiderPlacementSlot(2, object, count, available);
    if (count == 3)
        return -1;
    DetonatorConsiderPlacementSlot(3, object, count, available);
    if (count == 3)
        return -1;
    DetonatorConsiderPlacementSlot(4, object, count, available);
    if (count == 3)
        return -1;
    DetonatorConsiderPlacementSlot(5, object, count, available);
    if (count == 3)
        return -1;
    DetonatorConsiderPlacementSlot(6, object, count, available);
    if (count == 3)
        return -1;
    DetonatorConsiderPlacementSlot(7, object, count, available);
    if (count == 3)
        return -1;
    DetonatorConsiderPlacementSlot(8, object, count, available);
    if (count == 3)
        return -1;
    DetonatorConsiderPlacementSlot(9, object, count, available);
    if (count == 3)
        return -1;
    return available;
}

typedef struct custom_accessory_resource_pc_s {
    NUGSCN *scene;
    NUSPECIALHANDLE special;
    i32 original_texture_id;
    i32 texture_id;
    i32 material_index;
    CHARACTER_MODEL_LAYERS_PC *character_model;
} CUSTOM_ACCESSORY_RESOURCE_PC;
STATIC_ASSERT(sizeof(CUSTOM_ACCESSORY_RESOURCE_PC) == 0x20);
STATIC_ASSERT(offsetof(CUSTOM_ACCESSORY_RESOURCE_PC, scene) == 0);
STATIC_ASSERT(offsetof(CUSTOM_ACCESSORY_RESOURCE_PC, special) == 4);
STATIC_ASSERT(offsetof(CUSTOM_ACCESSORY_RESOURCE_PC, original_texture_id) == 0x10);
STATIC_ASSERT(offsetof(CUSTOM_ACCESSORY_RESOURCE_PC, texture_id) == 0x14);
STATIC_ASSERT(offsetof(CUSTOM_ACCESSORY_RESOURCE_PC, material_index) == 0x18);
STATIC_ASSERT(offsetof(CUSTOM_ACCESSORY_RESOURCE_PC, character_model) == 0x1c);
typedef struct custom_accessory_hierarchy_pc_s {
    u8 unknown_00[0xc];
    NUMTL **materials;
    i32 material_count;
} CUSTOM_ACCESSORY_HIERARCHY_PC;
STATIC_ASSERT(offsetof(CUSTOM_ACCESSORY_HIERARCHY_PC, materials) == 0xc);
STATIC_ASSERT(offsetof(CUSTOM_ACCESSORY_HIERARCHY_PC, material_count) == 0x10);
typedef struct custom_accessory_material_pc_s {
    u8 unknown_00[0x74];
    i16 texture_id;
    u8 unknown_76[0x24];
    u8 material_tag;
} CUSTOM_ACCESSORY_MATERIAL_PC;
STATIC_ASSERT(offsetof(CUSTOM_ACCESSORY_MATERIAL_PC, texture_id) == 0x74);
STATIC_ASSERT(offsetof(CUSTOM_ACCESSORY_MATERIAL_PC, material_tag) == 0x9a);
STATIC_ASSERT(offsetof(APICHARSYS, models) == 0x18);
STATIC_ASSERT(offsetof(APICHARSYS, playermodelids) == 0x1c);
STATIC_ASSERT(sizeof(CHARACTER_MODEL_LAYERS_PC) == 0x54);
extern i32 Customiser_AccessoriesLoaded;
void __cdecl Customiser_LoadAccessories(CUSTOMISER_LAYERS_PC *input, APICHARACTERMODELLIST *models) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 side, category, material_index;
    CHARACTER_MODEL_LAYERS_PC *model;
    CUSTOM_ACCESSORY_RESOURCE_PC *resource;
    char piece_name[0x80], path[0x80];
    if (customiser == NULL)
        return;
    Customiser_AccessoriesLoaded = 1;
    for (; models->model_id != -1; ++models) {
        side = 0;
        if (models->model_id != customiser->character_ids[0]) {
            if (models->model_id != customiser->character_ids[1])
                continue;
            side = 1;
        }
        model = &apicharsys->models[apicharsys->playermodelids[models->model_id]];
        for (category = 0; category < 9; ++category) {
            u16 piece_index;
            resource = (CUSTOM_ACCESSORY_RESOURCE_PC *)&KillPartsAccessoriesPc[side][category];
            memset(resource, 0, sizeof(*resource));
            if (customiser->categories[category]->name == NULL || customiser->piece_counts[category] <= 0)
                continue;
            resource->character_model = model;
            piece_index = side == 0 ? customiser->save->pieces[category] : customiser->save->secondary_pieces[category];
            NuStrCpy(piece_name, customiser->piece_sets[category][piece_index].name);
            NuStrCpy(path, "chars\\weirdo\\");
            NuStrCat(path, customiser->categories[category]->name);
            NuStrCat(path, "\\");
            NuStrCat(path, piece_name);
            if (customiser->categories[category]->uses_special != 0) {
                NuStrCat(path, ".gsc");
                resource->scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, path);
                if (resource->scene != NULL)
                    NuSpecialFind(resource->scene, &resource->special, piece_name, 1);
            } else {
                if (customiser->categories[category]->material_tag == -1)
                    continue;
                NuStrCat(path, ".pnt");
                resource->texture_id = NuTexReadPc(path, &characterbuffer_ptr, characterbuffer_end);
                if (resource->texture_id == 0)
                    continue;
                for (material_index = 0;
                     material_index < ((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)->material_count;
                     ++material_index) {
                    NUMTL *material = ((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)->materials[material_index];
                    if (((CUSTOM_ACCESSORY_MATERIAL_PC *)material)->material_tag !=
                        customiser->categories[category]->material_tag)
                        continue;
                    resource->material_index = material_index;
                    resource->original_texture_id =
                        ((CUSTOM_ACCESSORY_MATERIAL_PC *)((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)
                             ->materials[material_index])
                            ->texture_id;
                    ((CUSTOM_ACCESSORY_MATERIAL_PC *)((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)
                         ->materials[material_index])
                        ->texture_id = (i16)resource->texture_id;
                    NuMtlUpdate(((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)->materials[material_index]);
                }
            }
        }
    }
}

STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, piece_sets) == 0x24);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, piece_counts) == 0x48);
STATIC_ASSERT(offsetof(CUSTOMISER_CONFIG_PC, save) == 0x174);
STATIC_ASSERT(sizeof(CUSTOM_PIECE_CONFIG_PC) == 0x28);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_CONFIG_PC, name) == 0);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_CONFIG_PC, availability_flags) == 0x12);
STATIC_ASSERT(sizeof(((CUSTOM_PIECE_CONFIG_PC *)0)->availability_flags) == 2);
STATIC_ASSERT(offsetof(CUSTOM_SAVE_CONFIG_PC, pieces) == 0);
STATIC_ASSERT(offsetof(CUSTOM_SAVE_CONFIG_PC, secondary_pieces) == 0x38);
STATIC_ASSERT(sizeof(((CUSTOM_SAVE_CONFIG_PC *)0)->pieces[0]) == 2);

void __cdecl Customiser_Set100PercentPieces(CUSTOMISER_LAYERS_PC *input) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 category, index;
    if (customiser == NULL || customiser->save == NULL)
        return;
    for (category = 0; category < 9; ++category) {
        for (index = 0; index < customiser->piece_counts[category]; ++index) {
            if ((customiser->piece_sets[category][index].availability_flags & 0x80) != 0)
                customiser->save->pieces[category] = (u16)index;
            if ((customiser->piece_sets[category][index].availability_flags & 0x100) != 0)
                customiser->save->secondary_pieces[category] = (u16)index;
        }
    }
}

CUSTOMPIECE_LAYERS_PC *__cdecl Customiser_FindPieceByName(CUSTOMISER_LAYERS_PC *input, char *name, i32 *category,
                                                          i32 *index) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 category_index, piece_index;
    if (customiser != NULL) {
        for (category_index = 0; category_index < 9; ++category_index) {
            for (piece_index = 0; piece_index < customiser->piece_counts[category_index]; ++piece_index) {
                if (NuStrICmp(name, customiser->piece_sets[category_index][piece_index].name) == 0) {
                    if (category != NULL)
                        *category = category_index;
                    if (index != NULL)
                        *index = piece_index;
                    return (CUSTOMPIECE_LAYERS_PC *)&customiser->piece_sets[category_index][piece_index];
                }
            }
        }
    }
    return NULL;
}

void __cdecl Customiser_DumpAccessories(void *input) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 side, category;
    if (customiser == NULL || Customiser_AccessoriesLoaded != 1)
        return;
    for (side = 0; side < 2; ++side) {
        if (apicharsys->playermodelids[customiser->character_ids[side]] == -1)
            continue;
        for (category = 0; category < 9; ++category) {
            CUSTOM_ACCESSORY_RESOURCE_PC *resource =
                (CUSTOM_ACCESSORY_RESOURCE_PC *)&KillPartsAccessoriesPc[side][category];
            if (customiser->categories[category]->name == NULL)
                continue;
            if (resource->scene != NULL) {
                NuGScnRemove(resource->scene);
                resource->scene = NULL;
            } else if (resource->texture_id != 0) {
                ((CUSTOM_ACCESSORY_MATERIAL_PC *)((CUSTOM_ACCESSORY_HIERARCHY_PC *)resource->character_model->hierarchy)
                     ->materials[resource->material_index])
                    ->texture_id = (i16)resource->original_texture_id;
                NuMtlUpdate(((CUSTOM_ACCESSORY_HIERARCHY_PC *)resource->character_model->hierarchy)
                                ->materials[resource->material_index]);
                NuTexDestroy(resource->texture_id);
            }
        }
    }
}

STATIC_ASSERT(offsetof(WORLDINFO, customiser_resources) == 0x5120);
STATIC_ASSERT(offsetof(WORLDINFO, customiser_shared_scenes) == 0x5144);

void __cdecl Customiser_DumpAll(void *input, WORLDINFO *world) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 category, piece;
    if (customiser == NULL)
        return;
    for (category = 0; category < 9; ++category) {
        CUSTOM_ACCESSORY_RESOURCE_PC *resources = (CUSTOM_ACCESSORY_RESOURCE_PC *)world->customiser_resources[category];
        if (resources == NULL)
            continue;
        for (piece = 0; piece < customiser->piece_counts[category]; ++piece) {
            CUSTOM_ACCESSORY_RESOURCE_PC *resource = &resources[piece];
            if (resource->scene != NULL) {
                if (resource->scene != world->customiser_shared_scenes[category])
                    NuGScnRemove(resource->scene);
                resource->scene = NULL;
            } else if (resource->texture_id != 0) {
                NuTexDestroy(resource->texture_id);
            }
        }
        if (world->customiser_shared_scenes[category] != NULL) {
            NuGScnRemove(world->customiser_shared_scenes[category]);
            world->customiser_shared_scenes[category] = NULL;
        }
    }
}

void __cdecl Customiser_RestoreModelTextureIDs(void *input) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOMISER_ANIMS_LOAD_PC *textures = (CUSTOMISER_ANIMS_LOAD_PC *)input;
    i32 side, category, material;
    if (customiser == NULL)
        return;
    for (side = 0; side < 2; ++side) {
        CHARACTER_MODEL_LAYERS_PC *model = APICharacterLoaded(customiser->character_ids[side]);
        if (model == NULL)
            continue;
        for (category = 0; category < 9; ++category) {
            if (textures->model_texture_ids[side * 9 + category] == 0)
                continue;
            for (material = 0; material < ((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)->material_count;
                 ++material) {
                NUMTL *entry = ((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)->materials[material];
                if (((CUSTOM_ACCESSORY_MATERIAL_PC *)entry)->material_tag ==
                    customiser->categories[category]->material_tag) {
                    ((CUSTOM_ACCESSORY_MATERIAL_PC *)((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)
                         ->materials[material])
                        ->texture_id = (i16)textures->model_texture_ids[side * 9 + category];
                    NuMtlUpdate(((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)->materials[material]);
                }
            }
        }
    }
}

/* Private integration fragment for src/legoapi/customise.c, after its existing
 * CUSTOMISER_CONFIG_PC / CUSTOM_SAVE_CONFIG_PC definitions.
 * Native 0x592d00, 450 bytes, cdecl(customiser, object, matrices).
 */
void __cdecl CustomiserDrawAccessoriesPc(CUSTOMISER_LAYERS_PC *input, GAMEOBJECT *object, NUMTX *matrices) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 joint = object->character_data->game_character->helmet_locator;
    i32 side;
    i32 category;
    const u16 *pieces;
    KILLPARTS_ACCESSORY_PC *resource;
    NUMTX matrix;
    NUMTX reflection;
    if (customiser == NULL || joint == -1 || object->character_model->points_of_interest[joint] == NULL)
        return;
    side = object->id != (u16)customiser->character_ids[0];
    for (category = 0; category < 9; ++category) {
        if (customiser->piece_counts[category] <= 0 || category == 2)
            continue;
        if (category == 0) {
            pieces = side == 0 ? customiser->save->pieces : customiser->save->secondary_pieces;
            if ((customiser->piece_sets[0][pieces[0]].availability_flags & 0x20) != 0 ||
                (customiser->piece_sets[1][pieces[1]].availability_flags & 1) != 0 || object->helmet != 0)
                continue;
        }
        resource = &KillPartsAccessoriesPc[side][category];
        if (NuSpecialExistsFn(&resource->special) == 0)
            continue;
        if (matrices != NULL)
            matrix = matrices[joint];
        else
            matrix = object->locator_matrices[joint];
        NuSpecialDrawAt(&resource->special, &matrix);
        if ((u8)object->field_107c != 0 &&
            MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                             WORLD->current_level->reflection_y, &reflection) != 0) {
            NuRndrStartReflectionRender(0);
            NuSpecialDrawAt(&resource->special, &reflection);
            NuRndrEndReflectionRender();
        }
    }
}

/* Append after existing CUSTOM_ACCESSORY_* types in src/legoapi/customise.c.
 * Retail 005935f0/179; no callees. Uses established local view types. */
void __cdecl Customiser_SaveModelTextureIDs(void *input, CHARACTER_MODEL_LAYERS_PC *model) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOMISER_ANIMS_LOAD_PC *textures = (CUSTOMISER_ANIMS_LOAD_PC *)input;
    i32 side, category, material;
    if (customiser == NULL || model == NULL)
        return;
    for (side = 0; side < 2; ++side) {
        if (customiser->character_ids[side] != model->model_id)
            continue;
        for (category = 0; category < 9; ++category) {
            for (material = 0; material < ((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)->material_count;
                 ++material) {
                NUMTL *entry = ((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)->materials[material];
                if (((CUSTOM_ACCESSORY_MATERIAL_PC *)entry)->material_tag ==
                    customiser->categories[category]->material_tag)
                    textures->model_texture_ids[side * 9 + category] = entry->texture_id;
            }
        }
    }
}

/* Source-private views for customise.c. Config/piece/category types already defined there. */
typedef struct custom_piece_anim_pc_s {
    f32 duration, elapsed, hold_time;
    u16 start_angle, target_angle, current_angle;
    u8 unknown_12[2];
} CUSTOM_PIECE_ANIM_PC;
STATIC_ASSERT(sizeof(CUSTOM_PIECE_ANIM_PC) == 0x14);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_ANIM_PC, elapsed) == 4);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_ANIM_PC, hold_time) == 8);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_ANIM_PC, start_angle) == 0xc);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_ANIM_PC, target_angle) == 0xe);
STATIC_ASSERT(offsetof(CUSTOM_PIECE_ANIM_PC, current_angle) == 0x10);
/* Native customiser packet allocation stride is 0x48; canonical API view
 * currently covers offsets 0..0x43. Preserve its unknown four-byte tail. */
typedef struct customiser_anim_packet_pc_s {
    ANIMPACKET_PC packet;
    u8 unknown_44[4];
} CUSTOMISER_ANIM_PACKET_PC;
STATIC_ASSERT(sizeof(ANIMPACKET_PC) == 0x44);
STATIC_ASSERT(sizeof(CUSTOMISER_ANIM_PACKET_PC) == 0x48);
STATIC_ASSERT(offsetof(CUSTOMISER_ANIM_PACKET_PC, packet) == 0);
STATIC_ASSERT(offsetof(CUSTOMISER_ANIM_PACKET_PC, unknown_44) == 0x44);
typedef struct customiser_update_pc_s {
    u8 unknown_00[0x178];
    CUSTOMISER_ANIM_PACKET_PC animation_packets[2];
    u8 unknown_208[0x60];
    NUMTX joint_matrices[2][16];
    u8 unknown_a68[4];
    u8 animation_active[2], animation_state[2];
    f32 animation_values[2];
} CUSTOMISER_UPDATE_PC;
STATIC_ASSERT(offsetof(CUSTOMISER_UPDATE_PC, animation_packets) == 0x178);
STATIC_ASSERT(offsetof(CUSTOMISER_UPDATE_PC, joint_matrices) == 0x268);
STATIC_ASSERT(offsetof(CUSTOMISER_UPDATE_PC, animation_active) == 0xa6c);
STATIC_ASSERT(offsetof(CUSTOMISER_UPDATE_PC, animation_state) == 0xa6e);
STATIC_ASSERT(offsetof(CUSTOMISER_UPDATE_PC, animation_values) == 0xa70);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, animation_index) == 0x36);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, previous_animation) == 0x38);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, requested_animation) == 0x3a);
typedef struct custom_resource_update_pc_s {
    u8 unknown_00[0x14];
    u32 texture_id;
    u8 unknown_18[8];
} CUSTOM_RESOURCE_UPDATE_PC;
STATIC_ASSERT(offsetof(CUSTOM_RESOURCE_UPDATE_PC, texture_id) == 0x14);
STATIC_ASSERT(sizeof(CUSTOM_RESOURCE_UPDATE_PC) == 0x20);
typedef struct custom_hierarchy_update_pc_s {
    u8 unknown_00[0xc];
    NUMTL **materials;
    i32 material_count;
} CUSTOM_HIERARCHY_UPDATE_PC;
STATIC_ASSERT(offsetof(CUSTOM_HIERARCHY_UPDATE_PC, materials) == 0xc);
STATIC_ASSERT(offsetof(CUSTOM_HIERARCHY_UPDATE_PC, material_count) == 0x10);
typedef struct custom_material_update_pc_s {
    u8 unknown_00[0x9a];
    u8 material_tag;
} CUSTOM_MATERIAL_UPDATE_PC;
STATIC_ASSERT(offsetof(CUSTOM_MATERIAL_UPDATE_PC, material_tag) == 0x9a);
STATIC_ASSERT(offsetof(WORLDINFO, customiser_resources) == 0x5120);
STATIC_ASSERT(offsetof(NUMTL, texture_id) == 0x74);

static CUSTOM_PIECE_ANIM_PC CustomiseArmsAnimPc[2] = {0}, CustomiseLegsAnimPc[2] = {0}, CustomiseHeadAnimPc[2] = {0};
static u16 CustomiseTiltXPc[2] = {0}, CustomiseBobPc[2] = {0}, CustomiseTiltZPc[2] = {0}, CustomiseRotYPc[2] = {0},
           CustomiseYRotPc[2] = {0};
static f32 CustomiseMenuTimePc[2] = {0};
static i32 CustomiseMenuYPc[2] = {0};
static NUVEC CustomiseNoHatPosPc[2] = {0};
static f32 CustomiseNameAlphaPc = 0;
i32 customiser_quit = 0;
static f32 CustomiseHeadOffsetPc = 0.21f;
/* Canonical existing InitAsciiLeftPc, Game.customiser_start and AddGameMsg_Default are reused. */

static void UpdateCustomPieceAnim(CUSTOM_PIECE_ANIM_PC *anim, u16 minimum, u16 maximum) {
    i32 difference, index;
    f64 blend;
    if (anim->elapsed < anim->duration) {
        anim->elapsed += FRAMETIME;
        if (anim->elapsed >= anim->duration) {
            anim->elapsed = anim->duration;
            anim->hold_time = (f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5 + 0.5;
        }
        difference = RotDiff(anim->start_angle, anim->target_angle);
        index = (i32)((f64)anim->elapsed / anim->duration * 32768.0 + 16384.0);
        blend = 1.0 - ((f64)NuTrigTable[(index >> 1) & 0x7fff] + 1.0) * 0.5;
        anim->current_angle = (i32)((f64)difference * blend + anim->start_angle);
    } else {
        anim->hold_time -= FRAMETIME;
        if (anim->hold_time <= 0.0f) {
            anim->start_angle = anim->current_angle;
            difference = RotDiff(minimum, maximum);
            anim->target_angle = (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * difference + minimum);
            anim->elapsed = 0.0f;
            anim->duration = (f64)qrand() * (f64)(1.0f / 65535.0f) + 1.0;
        }
    }
}

void __cdecl Customiser_Update(CUSTOMISER_LAYERS_PC *input, WORLDINFO *world) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOMISER_UPDATE_PC *runtime = (CUSTOMISER_UPDATE_PC *)input;
    /* Native conversions must all happen before the NULL test. */
    const f32 frame_time = FRAMETIME;
    const i32 rotation_step = (i32)(10922.0 * frame_time);
    const i32 tilt_x_step = (i32)(12379.0 * frame_time);
    const i32 tilt_z_step = (i32)(10376.0 * frame_time);
    const i32 bob_step = (i32)(20206.0 * frame_time);
    i32 active_side, active_count, preview_side, colour, side, category;
    ADD_GAME_MESSAGE_PC message;
    if (customiser == NULL)
        return;
    Customiser_GetActiveWeirdoIndex(&active_side, &active_count);
    preview_side = -1;
    if (active_count == 1)
        preview_side = MissionMenuPacketPc.customise_other_player[active_side] != 0 ? active_side == 0 : active_side;
    colour = menu_flash == 0 ? 255 : 191;
    message = AddGameMsg_Default;
    for (side = 0; side < 2; ++side) {
        const i32 control_side = active_count == 2                           ? side
                                 : MissionMenuPacketPc.active_player[0] != 0 ? 0
                                 : MissionMenuPacketPc.active_player[1] != 0 ? 1
                                                                             : -1;
        const u16 *selection = Game.customiser_start[side].pieces;
        CUSTOM_PIECE_CONFIG_PC *head, *torso, *weapon;
        CHARACTER_MODEL_LAYERS_PC *model;
        ANIMPACKET_PC *packet;
        i32 saber_colour, intro;
        u16 angle;
        f32 no_hat;
        CustomiseRotYPc[side] += rotation_step;
        CustomiseTiltXPc[side] += tilt_x_step;
        CustomiseTiltZPc[side] += tilt_z_step;
        CustomiseBobPc[side] += bob_step;
        UpdateCustomPieceAnim(&CustomiseHeadAnimPc[side], 0xd556, 0x2aaa);
        UpdateCustomPieceAnim(&CustomiseArmsAnimPc[side], 0x1555, 0x4000);
        UpdateCustomPieceAnim(&CustomiseLegsAnimPc[side], 0xe000, 0x4000);
        head = &customiser->piece_sets[0][selection[0]];
        torso = &customiser->piece_sets[1][selection[1]];
        no_hat = 0.0f;
        if ((torso->availability_flags & 1) != 0) {
            if ((torso->availability_flags & 4) != 0)
                no_hat = (head->availability_flags & 8) != 0 ? CustomiseHeadOffsetPc
                                                             : (f64)CustomiseHeadOffsetPc * (f64)0.8f;
            else
                no_hat = (head->availability_flags & 8) != 0 ? (f64)CustomiseHeadOffsetPc * (f64)0.8f
                                                             : (f64)CustomiseHeadOffsetPc * (f64)0.6f;
        }
        CustomiseNoHatPosPc[side].y = SeekValF(CustomiseNoHatPosPc[side].y, no_hat, 10.0f);
        CustomiseMenuTimePc[side] += FRAMETIME;
        model = APICharacterLoaded(customiser->character_ids[side]);
        if (model == NULL)
            continue;
        weapon = &customiser->piece_sets[2][selection[2]];
        saber_colour = LightSabre_ColourFromObj(weapon->weapon_model, NULL);
        if (runtime->animation_state[side] > 0)
            --runtime->animation_state[side];
        packet = &runtime->animation_packets[side].packet;
        intro = saber_colour == -1 ? 0x62 : 0x61;
        if (runtime->animation_active[side] == 0 && model->model_data_b[intro] != NULL) {
            if (runtime->animation_values[side] == 0.0f) {
                runtime->animation_values[side] = AnimDuration(model->model_id, intro, 0.0f, 0.0f, 1);
            } else {
                runtime->animation_values[side] -= FRAMETIME;
                if (runtime->animation_values[side] <= 0.0f)
                    runtime->animation_active[side] = 1;
            }
        } else {
            runtime->animation_active[side] = 1;
        }
        packet->previous_animation = packet->animation_index;
        if (saber_colour == -1)
            packet->requested_animation = runtime->animation_active[side] == 0 ? 0x62 : 0x63;
        else
            packet->requested_animation = runtime->animation_active[side] == 0 ? 0x61 : 0xbe;
        UpdateAnimPacket(model, packet, (f64)FRAMETIME * 30.0, 0.0f, FRAMETIME, 0.0f);
        angle = CustomiseYRotPc[side] + 0xc000;
        for (category = 0; category < 9; ++category) {
            i32 locator;
            CUSTOM_CATEGORY_CONFIG_PC *category_data;
            CUSTOM_RESOURCE_UPDATE_PC *resources;
            i32 piece_index, material;
            if (CustomiseMenuYPc[side] == category && GetMenuID() == 12 && customiser_quit == 0 &&
                (active_count == 2 || side == preview_side) && control_side != -1 &&
                (locator = customiser->locator_indices[category]) != -1 && model->points_of_interest[locator] != NULL) {
                const NUMTX *joint = &runtime->joint_matrices[side][locator];
                NUVEC position;
                const f32 offset = customiser->locator_x_offsets[category];
                position.x = joint->m30;
                position.y = joint->m31;
                position.z = joint->m32;
                message.text = InitAsciiLeftPc;
                message.unknown_4f = 2;
                if (offset != 0.0f) {
                    position.x += offset * NuTrigTable[(angle >> 1) & 0x7fff];
                    position.z += offset * NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
                }
                position.y += customiser->locator_y_offsets[category];
                message.red = colour;
                message.green = colour;
                message.blue = colour;
                message.position = &position;
                message.scale = 1.5f;
                message.alpha = 0x60;
                message.flags = 0x87;
                AddGameMsg(&message);
            }
            resources = (CUSTOM_RESOURCE_UPDATE_PC *)world->customiser_resources[category];
            if (resources == NULL || customiser->piece_counts[category] <= 0)
                continue;
            category_data = customiser->categories[category];
            if (category_data->uses_special != 0 || category_data->material_tag == -1)
                continue;
            piece_index = selection[category];
            if (resources[piece_index].texture_id == 0)
                continue;
            for (material = 0; material < ((CUSTOM_HIERARCHY_UPDATE_PC *)model->hierarchy)->material_count;
                 ++material) {
                NUMTL *entry = ((CUSTOM_HIERARCHY_UPDATE_PC *)model->hierarchy)->materials[material];
                if (((CUSTOM_MATERIAL_UPDATE_PC *)entry)->material_tag == category_data->material_tag) {
                    entry->texture_id = (u16)resources[piece_index].texture_id;
                    NuMtlUpdate(entry);
                }
            }
        }
    }
    {
        const f32 target = GetMenuID() == 12 && customiser_quit == 0 ? 1.0f : 0.0f;
        CustomiseNameAlphaPc = SeekLinearF(CustomiseNameAlphaPc, target, 1.5 * FRAMETIME);
    }
}

/* Append to customise.c after existing update/runtime types and state. */
#include <stdio.h>
extern NUVEC CustomisePos[2];
static f32 CustomiseNameBoardTMulPc[2] = {0}, CustomiseNameBoardMulPc[2] = {0}, CustomiseNameLetterBlipScalePc[2] = {0};
static i32 CustomiserUseBigScenesPc = 1;
void __cdecl Customiser_SetNameAndIcon(CUSTOMISER_LAYERS_PC *, i32);

/* 00592940 / 291 */
i32 __cdecl Customiser_NextPieceRight(CUSTOMISER_LAYERS_PC *input, i32 index, i32 count, i32 unused, i32 category) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 attempts = 0;
    (void)unused;
    if (customiser != NULL) {
        while (attempts < count) {
            i32 available = 1;
            ++index;
            if (index >= count)
                index -= count;
            if (category != 2) {
                WORLDINFO *world = WorldInfo_CurrentlyActive();
                if (world != NULL && HUB_ADATA != NULL && world->area == HUB_ADATA) {
                    CUSTOM_ACCESSORY_RESOURCE_PC *resources =
                        (CUSTOM_ACCESSORY_RESOURCE_PC *)world->customiser_resources[category];
                    if (resources == NULL)
                        available = 0;
                    else if (customiser->categories[category]->uses_special != 0)
                        available = NuSpecialExistsFn(&resources[index].special) != 0;
                    else
                        available = resources[index].texture_id != 0;
                }
            }
            if (available != 0 &&
                ((customiser->piece_sets[category][index].availability_flags & 0x180) == 0 ||
                 Game_100PercentComplete() != 0) &&
                customiser->piece_available((CUSTOMPIECE_LAYERS_PC *)&customiser->piece_sets[category][index]) != 0)
                break;
            ++attempts;
        }
    }
    return index;
}

/* 00424040 / 775 */
void __cdecl Customiser_Init(CUSTOMISER_LAYERS_PC *input) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOMISER_UPDATE_PC *runtime = (CUSTOMISER_UPDATE_PC *)input;
    i32 side, category;
    if (customiser == NULL)
        return;
    for (side = 0; side < 2; ++side) {
        ResetAnimPacket(&runtime->animation_packets[side].packet, -1);
        runtime->animation_values[side] = 0.0f;
        runtime->animation_active[side] = 0;
        runtime->animation_state[side] = 2;
        CustomiseMode[side] = 2;
    }
    if (WORLD->camera_splines[18] != NULL) {
        for (side = 0; side < 2; ++side) {
            NUVEC *points = WORLD->camera_splines[18]->pts;
            CustomisePos[side] = points[side * 2];
            CustomiseYRotPc[side] =
                NuAtan2D(points[side * 2 + 1].x - CustomisePos[side].x, points[side * 2 + 1].z - CustomisePos[side].z);
            CustomiseHeadAnimPc[side].current_angle = 0;
            CustomiseHeadAnimPc[side].hold_time = (f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5 * 0.5;
            CustomiseHeadAnimPc[side].start_angle = 0;
            CustomiseHeadAnimPc[side].target_angle = 0;
            CustomiseHeadAnimPc[side].elapsed = 0.0f;
            CustomiseHeadAnimPc[side].duration = 0.0f;
            CustomiseArmsAnimPc[side].current_angle = 0;
            CustomiseArmsAnimPc[side].hold_time = (f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5 * 0.5;
            CustomiseArmsAnimPc[side].start_angle = 0;
            CustomiseArmsAnimPc[side].target_angle = 0;
            CustomiseArmsAnimPc[side].elapsed = 0.0f;
            CustomiseArmsAnimPc[side].duration = 0.0f;
            CustomiseLegsAnimPc[side].current_angle = 0;
            CustomiseLegsAnimPc[side].hold_time = (f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5 * 0.5;
            CustomiseLegsAnimPc[side].start_angle = 0;
            CustomiseLegsAnimPc[side].target_angle = 0;
            CustomiseLegsAnimPc[side].elapsed = 0.0f;
            CustomiseLegsAnimPc[side].duration = 0.0f;
        }
        CustomiseRotYPc[0] = qrand();
        CustomiseRotYPc[1] = CustomiseRotYPc[0] + 0x4000;
        CustomiseTiltXPc[0] = qrand();
        CustomiseTiltXPc[1] = CustomiseTiltXPc[0] + 0x8000;
        CustomiseTiltZPc[0] = qrand();
        CustomiseTiltZPc[1] = CustomiseTiltZPc[0] + 0x8000;
        CustomiseBobPc[0] = qrand();
        CustomiseBobPc[1] = CustomiseBobPc[0] + 0x8000;
    }
    Customiser_SetNameAndIcon(input, -1);
    CustomiseNameAlphaPc = 0.0f;
    for (side = 0; side < 2; ++side) {
        for (category = 0; category < 9; ++category) {
            if (customiser->piece_counts[category] > 0)
                Game.customiser_start[side].pieces[category] =
                    (u16)Customiser_NextPieceRight(input, (u16)Game.customiser_start[side].pieces[category] - 1,
                                                   customiser->piece_counts[category], side, category);
        }
    }
    CustomiseNameBoardTMulPc[1] = 0.0f;
    CustomiseNameBoardMulPc[0] = 0.0f;
    CustomiseNameLetterBlipScalePc[1] = 1.0f;
    CustomiseNameLetterBlipScalePc[0] = 1.0f;
}

/* 005931a0 / 811. Retail ignored extra arguments remain explicit calls. */
void __cdecl Customiser_LoadAll(CUSTOMISER_LAYERS_PC *input, WORLDINFO *world) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    char converted_path[0x80];
    char piece_name[0x80];
    char path[0x80];
    void *texture_pack;
    i32 category, piece;
    if (customiser == NULL)
        return;
    Customiser_AccessoriesLoaded = 2;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    for (category = 0; category < 9; ++category) {
        if (customiser->piece_counts[category] > 0 && customiser->categories[category]->name != NULL) {
            i32 bytes = customiser->piece_counts[category] * sizeof(CUSTOM_ACCESSORY_RESOURCE_PC);
            world->customiser_resources[category] = world->giz_buffer.void_ptr;
            memset(world->customiser_resources[category], 0, bytes);
            world->giz_buffer.addr += bytes;
        } else {
            world->customiser_resources[category] = NULL;
        }
    }
    texture_pack = CHARPAK != 0
                       ? NuFilePakLoad("chars\\weirdo\\all_textures.fpk", &world->giz_buffer, world->buffer_end, 0x20)
                       : NULL;
    for (category = 0; category < 9; ++category) {
        CUSTOM_ACCESSORY_RESOURCE_PC *resources = (CUSTOM_ACCESSORY_RESOURCE_PC *)world->customiser_resources[category];
        world->customiser_shared_scenes[category] = NULL;
        if (resources == NULL)
            continue;
        if (CustomiserUseBigScenesPc != 0 && customiser->categories[category]->uses_special != 0 &&
            customiser->categories[category]->scene_file != NULL) {
            world->customiser_shared_scenes[category] =
                NuGScnRead(&world->giz_buffer, world->buffer_end, customiser->categories[category]->scene_file);
            ((void(__cdecl *)(char *))Customiser_Dump)(customiser->categories[category]->scene_file);
        }
        for (piece = 0; piece < customiser->piece_counts[category]; ++piece) {
            CUSTOM_ACCESSORY_RESOURCE_PC *resource = &resources[piece];
            NuStrCpy(piece_name, customiser->piece_sets[category][piece].name);
            NuStrCpy(path, "chars\\weirdo\\");
            NuStrCat(path, customiser->categories[category]->name);
            NuStrCat(path, "\\");
            NuStrCat(path, piece_name);
            if (customiser->categories[category]->uses_special != 0) {
                if (world->customiser_shared_scenes[category] != NULL) {
                    resource->scene = world->customiser_shared_scenes[category];
                } else {
                    NuStrCat(path, ".gsc");
                    world->giz_buffer.addr = (world->giz_buffer.addr + 0x3f) & ~0x3fu;
                    resource->scene = NuGScnRead(&world->giz_buffer, world->buffer_end, path);
                    ((void(__cdecl *)(char *))Customiser_Dump)(path);
                }
                if (resource->scene != NULL)
                    NuSpecialFind(resource->scene, &resource->special, piece_name, 1);
            } else if (customiser->categories[category]->material_tag != -1) {
                NuStrCat(path, ".pnt");
                if (texture_pack != NULL) {
                    i32 item, size;
                    void *data;
                    ((i32(__cdecl *)(char *, char *, i32))NuFileExtConvert)(converted_path, path, 0x80);
                    item = NuFilePakGetItem(texture_pack, converted_path);
                    if (item != -1 && NuFilePakGetItemInfo(texture_pack, item, &data, &size) != 0) {
                        data = NuPtrBlockFix(data);
                        resource->texture_id = NuTexCreateNative((NUTEXDIM_PC *)data);
                        ((void(__cdecl *)(char *))Customiser_Dump)(path);
                    }
                }
                if (resource->texture_id == 0) {
                    resource->texture_id = NuTexReadPc(path, &world->giz_buffer, world->buffer_end);
                    ((void(__cdecl *)(char *))Customiser_Dump)(path);
                }
            }
        }
    }
}

/* 004243e0 / 273; static native callee receives customiser in EDI. */
static u32 CustomiserPreviewLayerMaskPc(CUSTOMISER_CONFIG_PC *customiser, const u16 *selection, i32 selected,
                                        PLAYERCHARACTERCONFIG_PC *character) {
    u32 mask = 0;
    i32 category;
    for (category = 0; category < 9; ++category) {
        if (category != 5 && customiser->layer_indices[category] != -1)
            mask |= 1u << customiser->layer_indices[category];
    }
    if (mask == 0)
        mask = 1;
    if (character->cape_layer != -1) {
        mask |= 1u << character->cape_layer;
        if ((selected == -1 || GetMenuID() != 12 || customiser_quit != 0 || selected != 5) &&
            (customiser->piece_sets[5][selection[5]].availability_flags & 0x40) != 0)
            mask &= ~(1u << character->cape_layer);
    }
    return mask;
}

i32 __cdecl GameDrawCharacterModel(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *, NUMTX *, NUMTX *, NUMTX *, NUMTX *,
                                   GAMEOBJECT *, u32);
/* 00425fa0 / 1087 */
void __cdecl Customiser_Draw3D(CUSTOMISER_LAYERS_PC *input) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOMISER_UPDATE_PC *runtime = (CUSTOMISER_UPDATE_PC *)input;
    i32 active_side, active_count, preview_side, side;
    if (customiser == NULL || world->camera_splines[18] == NULL)
        return;
    Customiser_GetActiveWeirdoIndex(&active_side, &active_count);
    preview_side = -1;
    if (active_count == 1)
        preview_side = MissionMenuPacketPc.customise_other_player[active_side] != 0 ? active_side == 0 : active_side;
    for (side = 0; side < 2; ++side) {
        i32 character_id = customiser->character_ids[side];
        CHARACTER_MODEL_LAYERS_PC *model = APICharacterLoaded(character_id);
        PLAYERCHARACTERCONFIG_PC *character;
        i32 helmet, selected, category;
        const u16 *selection = Game.customiser_start[side].pieces;
        RTL_DATA_PC light_data;
        NUMTX matrix;
        u32 mask;
        if (model == NULL || runtime->animation_state[side] > 0)
            continue;
        character = &GCDataList[character_id];
        helmet = character->helmet_locator;
        if (helmet != -1 && model->points_of_interest[helmet] == NULL)
            helmet = -1;
        selected = CustomiseMenuYPc[side];
        rtlResetEx(&light_data, 1);
        rtlApplySetScale(world->rtl_set, &light_data, &CustomisePos[side], NULL, -1, 1.0f);
        SetLights_RTLDATA(&light_data, 1.0f);
        mask = CustomiserPreviewLayerMaskPc(customiser, selection, selected, character);
        NuMtxSetRotationY(&matrix, CustomiseYRotPc[side] + 0x8000);
        NuMtxTranslate(&matrix, &CustomisePos[side]);
        if (GameDrawCharacterModel(model, &runtime->animation_packets[side].packet, &matrix, NULL, NULL,
                                   runtime->joint_matrices[side], NULL, mask) == 0)
            continue;
        if (helmet != -1) {
            for (category = 0; category < 9; ++category) {
                CUSTOM_ACCESSORY_RESOURCE_PC *resources =
                    (CUSTOM_ACCESSORY_RESOURCE_PC *)world->customiser_resources[category];
                CUSTOM_ACCESSORY_RESOURCE_PC *resource;
                CUSTOM_PIECE_CONFIG_PC *piece;
                if (resources == NULL || customiser->piece_counts[category] <= 0 ||
                    customiser->categories[category]->uses_special == 0)
                    continue;
                resource = &resources[selection[category]];
                if (NuSpecialExistsFn(&resource->special) == 0)
                    continue;
                piece = &customiser->piece_sets[category][selection[category]];
                if (GetMenuID() == 12 && customiser_quit == 0 && category == selected &&
                    (active_count == 2 || side == preview_side)) {
                    NuFmod(CustomiseMenuTimePc[side], 0.3f);
                } else if (category == 0 && (piece->availability_flags & 0x20) != 0) {
                    continue;
                }
                matrix = runtime->joint_matrices[side][helmet];
                if (category == 0)
                    NuMtxPreTranslate(&matrix, &CustomiseNoHatPosPc[side]);
                NuSpecialDrawAt(&resource->special, &matrix);
            }
        }
        if (customiser->piece_counts[2] > 0) {
            i32 hand = character->weapon_joints[0];
            CUSTOM_PIECE_CONFIG_PC *weapon;
            LEVEL_OBJECT_RUNTIME_PC *object;
            if (hand == -1 || model->points_of_interest[hand] == NULL)
                continue;
            weapon = &customiser->piece_sets[2][selection[2]];
            if (weapon->weapon_model == -1)
                continue;
            object = &world->level_objects[weapon->weapon_model];
            if (object->active != 0) {
                i32 blade;
                NUSPECIALHANDLE *special;
                matrix = runtime->joint_matrices[side][hand];
                NuSpecialDrawAt(&object->special, &matrix);
                if (LightSabre_ColourFromObj(weapon->weapon_model, &blade) != -1) {
                    if (blade != -1) {
                        special = &world->level_objects[blade].special;
                        if (special != NULL)
                            NuSpecialDrawAt(special, &matrix);
                    }
                    special = &world->level_objects[17].special;
                    if (special != NULL)
                        NuSpecialDrawAt(special, &matrix);
                }
            }
        }
    }
    SetLevelLights(world->rtl_set, 1.0f);
}

/* 00593920 / 120; retail does not check the customiser/save arguments. */
i32 __cdecl Customiser_GetIcon(CUSTOMISER_LAYERS_PC *input, const void *save_input, i32 unused) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    const CUSTOM_SAVE_CONFIG_PC *save = (const CUSTOM_SAVE_CONFIG_PC *)save_input;
    CUSTOM_PIECE_CONFIG_PC *hat = &customiser->piece_sets[0][save->pieces[0]];
    CUSTOM_PIECE_CONFIG_PC *head;
    i32 character;
    (void)unused;
    if ((hat->availability_flags & 0x20) == 0) {
        head = &customiser->piece_sets[1][save->pieces[1]];
        if ((head->availability_flags & 1) == 0) {
            character = hat->icon_character_id;
            if (character != -1)
                return CDataList[character].icon_model;
            character = hat->character_id;
            if (character != -1)
                return CDataList[character].icon_model;
        }
    }
    head = &customiser->piece_sets[1][save->pieces[1]];
    character = head->icon_character_id;
    if (character != -1)
        return CDataList[character].icon_model;
    character = head->character_id;
    if (character != -1)
        return CDataList[character].icon_model;
    return PCArcadeUnpairedIconModel;
}

extern i16 TextIdPc880fa0;
/* 00423c30 / 874. Native contains class/name-part accumulation even though
 * the final displayed name is the generic stranger label. */
void __cdecl Customiser_SetNameAndIcon(CUSTOMISER_LAYERS_PC *input, i32 index) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOMISER_NAMES_PC *names = (CUSTOMISER_NAMES_PC *)input;
    i32 best = -1;
    i32 side, category, group;
    if (customiser == NULL)
        return;
    for (side = 0; side < 2; ++side) {
        i32 counts[11] = {0};
        CUSTOM_PIECE_CONFIG_PC *first = NULL;
        CUSTOM_PIECE_CONFIG_PC *second = NULL;
        CUSTOM_PIECE_CONFIG_PC *third = NULL;
        const u16 *selection = Game.customiser_start[side].pieces;
        if (index != -1 && side != index)
            continue;
        names->display_names[side][0] = 0;
        for (category = 0; category < 9; ++category) {
            CUSTOM_PIECE_CONFIG_PC *piece;
            if (customiser->piece_counts[category] <= 0)
                continue;
            if (category == 0 && (customiser->piece_sets[1][selection[1]].availability_flags & 1) != 0)
                continue;
            if (category == 1 && (customiser->piece_sets[0][selection[0]].availability_flags & 2) != 0 &&
                (customiser->piece_sets[1][selection[1]].availability_flags & 1) == 0)
                continue;
            piece = &customiser->piece_sets[category][selection[category]];
            if (piece->field_10 != -1)
                counts[piece->field_10] += (i8)customiser->categories[category]->material_indices[0];
            if (first == NULL && piece->unknown_indices_0a[0] != 0)
                first = piece;
            else if (second == NULL && piece->unknown_indices_0a[1] != 0)
                second = piece;
            else if (third == NULL && piece->unknown_indices_0a[2] != 0)
                third = piece;
        }
        for (group = 0; group < 11; ++group) {
            if (counts[group] >= 4 && (best == -1 || counts[group] > counts[best]))
                best = group;
        }
        sprintf(names->display_names[side], "%s %i", TTab[TextIdPc880fa0], side + 1);
        CDataList[customiser->character_ids[side]].icon_model =
            (i16)Customiser_GetIcon(input, &Game.customiser_start[side], side);
    }
}

static NUVEC CustomiseScreenPosPc[2] = {0};
/* 004243b0 / 42 */
void __cdecl Customiser_TransformToPanel(CUSTOMISER_LAYERS_PC *unused) {
    (void)unused;
    NuCameraTransformScreenClip(&CustomiseScreenPosPc[0], &CustomisePos[0], 1, NULL);
    NuCameraTransformScreenClip(&CustomiseScreenPosPc[1], &CustomisePos[1], 1, NULL);
}

void __cdecl Customiser_Reset(CUSTOMISER_LAYERS_PC *customiser) {
    CUSTOMISER_UPDATE_PC *runtime = (CUSTOMISER_UPDATE_PC *)customiser;
    i32 side;
    if (customiser == NULL)
        return;
    for (side = 0; side < 2; ++side) {
        CustomiseMenuTimePc[side] = 0.0f;
        ResetAnimPacket(&runtime->animation_packets[side].packet, -1);
        runtime->animation_active[side] = 0;
        runtime->animation_values[side] = 0.0f;
        runtime->animation_state[side] = 2;
    }
}
