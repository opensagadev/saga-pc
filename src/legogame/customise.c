#include <stdio.h>
#include <string.h>
#include "saga.h"

typedef struct customiser_names_pc_s {
    u8 unknown_00[0x6c];
    i16 character_ids[2];
    char display_names[2][128];
} CUSTOMISER_NAMES_PC;

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

typedef struct custom_category_config_pc_s {
    char *name;
    u8 uses_special;
    i8 material_tag;
    u8 material_indices[2];
    char *scene_file;
} CUSTOM_CATEGORY_CONFIG_PC;

typedef struct custom_save_config_pc_s {
    u16 pieces[9];
    u8 unknown_12[0x26];
    u16 secondary_pieces[9];
    u8 unknown_4a[0x26];
} CUSTOM_SAVE_CONFIG_PC;

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

typedef struct customiser_game_setting_pc_s {
    char *name;
    u32 model_flags;
    u32 gameplay_flags;
} CUSTOMISER_GAME_SETTING_PC;

typedef struct custom_accessory_resource_pc_s {
    NUGSCN *scene;
    NUSPECIALHANDLE special;
    i32 original_texture_id;
    i32 texture_id;
    i32 material_index;
    CHARACTER_MODEL_LAYERS_PC *character_model;
} CUSTOM_ACCESSORY_RESOURCE_PC;

/* Source-private views for customise.c. Config/piece/category types already defined there. */
typedef struct custom_piece_anim_pc_s {
    f32 duration;
    f32 elapsed;
    f32 hold_time;
    u16 start_angle;
    u16 target_angle;
    u16 current_angle;
    u8 unknown_12[2];
} CUSTOM_PIECE_ANIM_PC;

/* Native customiser packet allocation stride is 0x48; canonical API view
 * currently covers offsets 0..0x43. Preserve its unknown four-byte tail. */
typedef struct customiser_anim_packet_pc_s {
    ANIMPACKET_PC packet;
    u8 unknown_44[4];
} CUSTOMISER_ANIM_PACKET_PC;

typedef struct customiser_update_pc_s {
    u8 unknown_00[0x178];
    CUSTOMISER_ANIM_PACKET_PC animation_packets[2];
    u8 unknown_208[0x60];
    NUMTX joint_matrices[2][16];
    u8 unknown_a68[4];
    u8 animation_active[2];
    u8 animation_state[2];
    f32 animation_values[2];
} CUSTOMISER_UPDATE_PC;

typedef struct custom_resource_update_pc_s {
    u8 unknown_00[0x14];
    u32 texture_id;
    u8 unknown_18[8];
} CUSTOM_RESOURCE_UPDATE_PC;

typedef struct custom_hierarchy_update_pc_s {
    u8 unknown_00[0xc];
    NUMTL **materials;
    i32 material_count;
} CUSTOM_HIERARCHY_UPDATE_PC;

typedef struct custom_material_update_pc_s {
    u8 unknown_00[0x9a];
    u8 material_tag;
} CUSTOM_MATERIAL_UPDATE_PC;

extern NUVEC CustomisePos[2];
i32 __cdecl Customiser_NextPieceRight(CUSTOMISER_LAYERS_PC *input, i32 index, i32 count, i32 unused, i32 category);
i32 __cdecl GameDrawCharacterModel(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *, NUMTX *, NUMTX *, NUMTX *, NUMTX *,
                                   GAMEOBJECT *, u32);
i32 __cdecl Customiser_GetIcon(CUSTOMISER_LAYERS_PC *input, const void *save_input, i32 unused);
extern i16 TextIdPc880fa0;
/* Native customiser menu drawing and private matrix helpers; append to customise.c. */
extern i16 TextIdPc880750;
extern i16 TextIdPc880754;
extern i16 TextIdPc880758;
extern i16 TextIdPc880760;
extern i16 TextIdPc880858;
extern i16 TextIdPc880894;
extern i16 TextIdPc880898;
void __cdecl Hub_ClearStats(void);
void __cdecl Text_FillInExtendedSaveInfo(void);
i32 __cdecl TriggerAutoSave(void);

/* 424750/92. Can share the fuller Customiser canonical type once integrated. */
typedef struct hub_customiser_menu_reader_pc_s {
    u8 unknown_00[0x6c];
    i16 character_ids[2];
    u8 unknown_70[0x142];
    i16 first_animation;
    u8 unknown_1b4[0x46];
    i16 second_animation;
} HUB_CUSTOMISER_MENU_READER_PC;

static void CustomiserMtxSetRotateZPc(NUMTX *matrix, i32 angle) {
    const f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    const f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    matrix->m00 = cosine;
    matrix->m01 = sine;
    matrix->m02 = 0.0f;
    matrix->m03 = 0.0f;
    matrix->m10 = -sine;
    matrix->m11 = cosine;
    matrix->m12 = 0.0f;
    matrix->m13 = 0.0f;
    matrix->m20 = ANI3_EVAL_IDENTITY_MATRIX.m20;
    matrix->m21 = ANI3_EVAL_IDENTITY_MATRIX.m21;
    matrix->m22 = ANI3_EVAL_IDENTITY_MATRIX.m22;
    matrix->m23 = ANI3_EVAL_IDENTITY_MATRIX.m23;
    matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
}

static void CustomiserMtxRotateXPc(NUMTX *matrix, i32 angle) {
    const f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    const f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    const f32 a = matrix->m01;
    const f32 b = matrix->m11;
    const f32 c = matrix->m21;
    const f32 d = matrix->m31;
    matrix->m01 = (a * cosine) - (matrix->m02 * sine);
    matrix->m02 = (matrix->m02 * cosine) + (a * sine);
    matrix->m11 = (b * cosine) - (matrix->m12 * sine);
    matrix->m12 = (matrix->m12 * cosine) + (b * sine);
    matrix->m21 = (c * cosine) - (matrix->m22 * sine);
    matrix->m22 = (c * sine) + (matrix->m22 * cosine);
    matrix->m31 = (d * cosine) - (matrix->m32 * sine);
    matrix->m32 = (matrix->m32 * cosine) + (d * sine);
}

static void CustomiserMtxRotateYPc(NUMTX *matrix, i32 angle) {
    const f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    const f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    const f32 a = matrix->m00;
    const f32 b = matrix->m10;
    const f32 c = matrix->m20;
    const f32 d = matrix->m30;
    matrix->m00 = a * cosine + matrix->m02 * sine;
    matrix->m02 = matrix->m02 * cosine - a * sine;
    matrix->m10 = matrix->m12 * sine + b * cosine;
    matrix->m12 = matrix->m12 * cosine - b * sine;
    matrix->m20 = c * cosine + matrix->m22 * sine;
    matrix->m22 = matrix->m22 * cosine - c * sine;
    matrix->m30 = matrix->m32 * sine + d * cosine;
    matrix->m32 = matrix->m32 * cosine - d * sine;
}

static i32 __cdecl Customise_FindLetter(char character) {
    i32 letter;
    for (letter = 0; letter < 38; ++letter) {
        if (CustomiseLetter[letter] == character) {
            break;
        }
    }
    if (letter >= 38) {
        return 0;
    }
    return letter;
}

void __cdecl FinishWeirdoNames(i32 which) {
    char *name;
    i32 length;
    if (which != 1) {
        name = Game.customiser_start[0].name;
        NuStrUpr(name, name);
        for (length = NuStrLen(name); length < 15; ++length) {
            name[length] = ' ';
        }
        name[15] = '\0';
    }
    if (which == 0) {
        return;
    }
    name = Game.customiser_start[1].name;
    NuStrUpr(name, name);
    for (length = NuStrLen(name); length < 15; ++length) {
        name[length] = ' ';
    }
    name[15] = '\0';
}

static void UpdateCustomPieceAnim(CUSTOM_PIECE_ANIM_PC *anim, u16 minimum, u16 maximum) {
    i32 index;
    f64 difference;
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
        anim->current_angle = (i32)(difference * blend + anim->start_angle);
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

static CUSTOMISER_GAME_SETTING_PC CustomiserGameSettingsPrivatePc[] = {{"bountyhunter", 0x1000000, 0},
                                                                       {"jedi", 8, 0},
                                                                       {"sith", 0xc, 2},
                                                                       {"blaster", 0x100080, 0x40000000},
                                                                       {"alreadygothat", 0, 0x10},
                                                                       {"stormtrooperhelmet", 0, 0x40000},
                                                                       {NULL, 0, 0}};

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

void __cdecl Customiser_InitNames(CUSTOMISER_LAYERS_PC *customiser) {
    CUSTOMISER_NAMES_PC *view = (CUSTOMISER_NAMES_PC *)customiser;
    i32 index;
    if (customiser == NULL) {
        return;
    }
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

/* 00423c30 / 874. Native contains class/name-part accumulation even though
 * the final displayed name is the generic stranger label. */
void __cdecl Customiser_SetNameAndIcon(CUSTOMISER_LAYERS_PC *input, i32 index) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOMISER_NAMES_PC *names = (CUSTOMISER_NAMES_PC *)input;
    i32 best = -1;
    i32 side;
    i32 category;
    i32 group;
    if (customiser == NULL) {
        return;
    }
    for (side = 0; side < 2; ++side) {
        i32 counts[11];

        struct {
            char name[64];
            CUSTOM_PIECE_CONFIG_PC *piece;
        } parts[2];

        CUSTOM_PIECE_CONFIG_PC *third;
        const u16 *selection = Game.customiser_start[side].pieces;
        if (index != -1 && side != index) {
            continue;
        }
        names->display_names[side][0] = 0;
        third = NULL;
        parts[1].piece = NULL;
        parts[0].piece = NULL;
        for (group = 0; group < 11; ++group) {
            counts[group] = 0;
        }

        for (category = 0; category < 9; ++category) {
            CUSTOM_PIECE_CONFIG_PC *piece;
            if (customiser->piece_counts[category] <= 0) {
                continue;
            }
            if (category == 0 && (customiser->piece_sets[1][selection[1]].availability_flags & 1) != 0) {
                continue;
            }
            if (category == 1 && (customiser->piece_sets[0][selection[0]].availability_flags & 2) != 0 &&
                (customiser->piece_sets[1][selection[1]].availability_flags & 1) == 0) {
                continue;
            }
            piece = &customiser->piece_sets[category][selection[category]];
            if (piece->field_10 != -1) {
                counts[piece->field_10] += (i8)customiser->categories[category]->material_indices[0];
            }
            if (parts[0].piece == NULL && piece->unknown_indices_0a[0] != 0) {
                parts[0].piece = piece;
            } else if (parts[1].piece == NULL && piece->unknown_indices_0a[1] != 0) {
                parts[1].piece = piece;
            } else if (third == NULL && piece->unknown_indices_0a[2] != 0) {
                third = piece;
            }
        }
        for (group = 0; group < 11; ++group) {
            if (counts[group] >= 4 && (best == -1 || counts[group] > counts[best])) {
                best = group;
            }
        }
        sprintf(names->display_names[side], "%s %i", TTab[TextIdPc880fa0], side + 1);
        CDataList[customiser->character_ids[side]].icon_model =
            (i16)Customiser_GetIcon(input, &Game.customiser_start[side], side);
    }
}

i32 __cdecl PermanentCustomPieceAvailablePc(CUSTOMPIECE_LAYERS_PC *input) {
    CUSTOM_PIECE_CONFIG_PC *piece = (CUSTOM_PIECE_CONFIG_PC *)input;
    if (GAMEDEMO != 0) {
        if ((piece->availability_flags & 0x10) != 0) {
            return 0;
        }
    } else {
        u32 required_flags;
        if ((piece->availability_flags & 0x180) != 0 && Game_100PercentComplete() == 0) {
            return 0;
        }
        if (piece->character_id != -1) {
            if (InCollectList_Index(piece->character_id, NULL, 0) == -1) {
                return 1;
            }
            if (Collection_Got(piece->character_id) == 0) {
                return 0;
            }
        }
        required_flags = piece->model_flags & 0xc;
        if (required_flags != 0 && Collection_GotAnyOfType(-1, required_flags) == 0) {
            return 0;
        }
        if (piece->collection_type != -1 && Collection_GotAnyOfType(piece->collection_type, 0) == 0) {
            return 0;
        }
    }
    return 1;
}

static CUSTOM_PIECE_ANIM_PC CustomiseArmsAnimPc[2] = {0};
static CUSTOM_PIECE_ANIM_PC CustomiseLegsAnimPc[2] = {0};
static CUSTOM_PIECE_ANIM_PC CustomiseHeadAnimPc[2] = {0};
static u16 CustomiseTiltXPc[2] = {0};
static u16 CustomiseBobPc[2] = {0};
static u16 CustomiseTiltZPc[2] = {0};
static u16 CustomiseRotYPc[2] = {0};
static u16 CustomiseYRotPc[2] = {0};
static f32 CustomiseNameAlphaPc = 0;
static f32 CustomiseNameBoardTMulPc[2] = {0};
static f32 CustomiseNameBoardMulPc[2] = {0};
static f32 CustomiseNameLetterBlipScalePc[2] = {0};

/* 00424040 / 775 */
void __cdecl Customiser_Init(CUSTOMISER_LAYERS_PC *input) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOMISER_UPDATE_PC *runtime = (CUSTOMISER_UPDATE_PC *)input;
    i32 side;
    i32 category;
    if (customiser == NULL) {
        return;
    }
    for (side = 0; side < 2; ++side) {
        ResetAnimPacket(&runtime->animation_packets[side].packet, -1);
        runtime->animation_active[side] = 0;
        runtime->animation_values[side] = 0.0f;
        runtime->animation_state[side] = 2;
        CustomiseMode[side] = 2;
    }
    if (WORLD->camera_splines[18] != NULL) {
        for (side = 0; side < 2; ++side) {
            NUVEC *next;
            CustomisePos[side] = WORLD->camera_splines[18]->pts[side * 2];
            next = &WORLD->camera_splines[18]->pts[side * 2 + 1];
            CustomiseYRotPc[side] = NuAtan2D(next->x - CustomisePos[side].x, next->z - CustomisePos[side].z);
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
            if (customiser->piece_counts[category] > 0) {
                Game.customiser_start[side].pieces[category] =
                    (u16)Customiser_NextPieceRight(input, Game.customiser_start[side].pieces[category] - 1,
                                                   customiser->piece_counts[category], side, category);
            }
        }
    }
    CustomiseNameBoardTMulPc[1] = 0.0f;
    CustomiseNameBoardMulPc[0] = 0.0f;
    CustomiseNameLetterBlipScalePc[1] = 1.0f;
    CustomiseNameLetterBlipScalePc[0] = 1.0f;
}

static f32 CustomiseMenuTimePc[2] = {0};

void __cdecl Customiser_Reset(CUSTOMISER_LAYERS_PC *customiser) {
    CUSTOMISER_UPDATE_PC *runtime = (CUSTOMISER_UPDATE_PC *)customiser;
    i32 side;
    if (customiser == NULL) {
        return;
    }
    for (side = 0; side < 2; ++side) {
        CustomiseMenuTimePc[side] = 0.0f;
        ResetAnimPacket(&runtime->animation_packets[side].packet, -1);
        runtime->animation_active[side] = 0;
        runtime->animation_values[side] = 0.0f;
        runtime->animation_state[side] = 2;
    }
}

static NUVEC CustomiseScreenPosPc[2] = {0};

/* 004243b0 / 42 */
void __cdecl Customiser_TransformToPanel(CUSTOMISER_LAYERS_PC *unused) {
    (void)unused;
    NuCameraTransformScreenClip(&CustomiseScreenPosPc[0], &CustomisePos[0], 1, NULL);
    NuCameraTransformScreenClip(&CustomiseScreenPosPc[1], &CustomisePos[1], 1, NULL);
}

i32 customiser_quit = 0;

/* 004243e0 / 273; static native callee receives customiser in EDI. */
static u32 CustomiserPreviewLayerMaskPc(CUSTOMISER_CONFIG_PC *customiser, const u16 *selection, i32 selected,
                                        PLAYERCHARACTERCONFIG_PC *character) {
    u32 mask = 0;
    i32 category;
    for (category = 0; category < 9; ++category) {
        if (category != 5 && customiser->layer_indices[category] != -1) {
            mask |= 1u << customiser->layer_indices[category];
        }
    }
    if (mask == 0) {
        mask = 1;
    }
    if (character->cape_layer != -1) {
        mask |= 1u << character->cape_layer;
        if ((selected == -1 || GetMenuID() != 12 || customiser_quit != 0 || selected != 5) &&
            (customiser->piece_sets[5][selection[5]].availability_flags & 0x40) != 0) {
            mask &= ~(1u << character->cape_layer);
        }
    }
    return mask;
}

/* 00424500 / 491: applies both saved customisations to their characters'
 * layer masks, model and gameplay flags, weapon and blade colour. */
void __cdecl Customiser_SetUpCharacterData(CUSTOMISER_LAYERS_PC *input) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    u32 side;
    i32 category;
    if (customiser == NULL) {
        return;
    }
    for (side = 0; side < 2; ++side) {
        const u16 *selection = Game.customiser_start[side].pieces;
        CHARACTERDATA_PC *character = &CDataList[customiser->character_ids[side]];
        PLAYERCHARACTERCONFIG_PC *config = character->game_character;
        u32 mask;
        config->flags_090 = 0;
        mask = CustomiserPreviewLayerMaskPc(customiser, selection, -1, config);
        config->layer_mask_special = mask;
        config->layer_mask = mask;
        config->layer_mask_medium = mask;
        config->layer_mask_low = mask;
        config->layer_mask_dead = mask;
        character->model_flags &= 3;
        for (category = 0; category < 9; ++category) {
            if (customiser->piece_counts[category] > 0) {
                if (category != 0 || (customiser->piece_sets[1][selection[1]].availability_flags & 1) == 0) {
                    character->model_flags |= customiser->piece_sets[category][selection[category]].model_flags;
                }
                if ((character->model_flags & 0x80) != 0) {
                    character->model_flags |= 0x10000000;
                }
                config->flags_090 |= customiser->piece_sets[category][selection[category]].gameplay_flags;
            }
        }
        if ((customiser->piece_sets[1][selection[1]].availability_flags & 1) != 0) {
            config->flags_090 |= 0x10;
        }
        config->weapon_model = customiser->piece_sets[2][selection[2]].weapon_model;
        config->blade_colour = (i8)LightSabre_ColourFromObj(config->weapon_model, NULL);
    }
    Customiser_SetNameAndIcon(input, -1);
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

i32 __cdecl Customiser_MenuAvailable(CUSTOMISER_LAYERS_PC *customiser) {
    HUB_CUSTOMISER_MENU_READER_PC *c = (HUB_CUSTOMISER_MENU_READER_PC *)customiser;
    if (c == NULL || APICharacterLoaded(c->character_ids[0]) == NULL) {
        return 0;
    }
    if (c->first_animation != 190 && c->first_animation != 99) {
        return 0;
    }
    if (APICharacterLoaded(c->character_ids[1]) == NULL) {
        return 0;
    }
    return c->second_animation == 190 || c->second_animation == 99;
}

i32 __cdecl Customise_GetToggleString(i32 index) {
    i32 mode = CustomiseMode[index];
    if (mode == 1) {
        return TextIdPc880890;
    }
    if (mode != 0) {
        do {
            mode = (mode + 1) % 3;
        } while (mode == 0);
    }
    return TextIdPc880780;
}

static i32 CustomiseMenuYPc[2] = {0};
static i32 CustomiseNameXPc[2] = {0};
static i32 CustomiseNameYPc[2] = {0};
static const char CustomiserBadWordReplacementPc[9] = "Stranger";

/* 00424810..004252bc: PC control returns its quit decision to the wrapper. */
i32 __cdecl CustomiserMenu_Update(void) {
    i32 quit = 0;
    i32 cancel_name = 0;
    i32 toggle = 0;
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)CharacterCustomiser;
    i32 active_index;
    i32 active_count;
    i32 select[2];
    i32 cancel;
    i32 player;
    if (customiser == NULL) {
        quit = 1;
    }
    Customiser_GetActiveWeirdoIndex(&active_index, &active_count);
    BlipD[0] = BlipD[1] = BlipU[0] = BlipU[1] = BlipR[0] = BlipR[1] = BlipL[0] = BlipL[1] = 0.0f;
    cancel = 0;

    for (player = 0; player < 2; ++player) {
        GAMEPAD_RUNTIME_INPUT_PC *pad = (GAMEPAD_RUNTIME_INPUT_PC *)&GamePadStorage[player];
        select[player] = 0;
        CustomiseNameBoardTMulPc[player] = CustomiseMode[player] == 1 ? 1.0f : 0.0f;
        CustomiseNameBoardMulPc[player] =
            SeekLinearF(CustomiseNameBoardMulPc[player], CustomiseNameBoardTMulPc[player], (f32)((f64)FRAMETIME * 4.0));
        CustomiseNameLetterBlipScalePc[player] =
            SeekLinearF(CustomiseNameLetterBlipScalePc[player], 1.0f, (f32)((f64)FRAMETIME * 5.0));
        if (MissionMenuPacketPc.active_player[player] != 0) {
            if ((pad->pressed & GAMEPAD_MENUSELECT) != 0) {
                select[player] = 1;
            } else if ((pad->pressed & GAMEPAD_MENUCANCEL) != 0) {
                cancel = 1;
            }
        }
    }
    if (cancel != 0 && CustomiseMode[0] != 1 && CustomiseMode[1] != 1) {
        if (GAMEDEMO != 0 && MissionMenuPacketPc.active_player[0] != 0 && MissionMenuPacketPc.active_player[1] != 0) {
            if ((((GAMEPAD_RUNTIME_INPUT_PC *)&GamePadStorage[0])->pressed & GAMEPAD_MENUCANCEL) != 0) {
                MissionMenuPacketPc.selected[0] = MissionMenuPacketPc.selected[0] == 0;
            }
            if ((((GAMEPAD_RUNTIME_INPUT_PC *)&GamePadStorage[1])->pressed & GAMEPAD_MENUCANCEL) != 0) {
                MissionMenuPacketPc.selected[1] = MissionMenuPacketPc.selected[1] == 0;
            }
            if (MissionMenuPacketPc.selected[0] != 0 && MissionMenuPacketPc.selected[1] != 0) {
                quit = 1;
            }
        } else {
            quit = 1;
        }
    } else if ((select[0] != 0 || select[1] != 0) && active_count == 1 && CustomiseMode[0] != 1 &&
               CustomiseMode[1] != 1) {
        toggle = 1;
    } else {
        quit = 0;
        for (player = 0; player < 2; ++player) {
            u32 pressed;
            u32 held;
            i32 side;
            i32 old_mode;
            i32 new_mode = -1;
            i32 randomise = 0;
            if (MissionMenuPacketPc.active_player[player] == 0) {
                continue;
            }
            held = ((GAMEPAD_RUNTIME_INPUT_PC *)&GamePadStorage[player])->left_held |
                   ((GAMEPAD_RUNTIME_INPUT_PC *)&GamePadStorage[player])->held;
            pressed = ((GAMEPAD_RUNTIME_INPUT_PC *)&GamePadStorage[player])->pressed |
                      ((GAMEPAD_RUNTIME_INPUT_PC *)&GamePadStorage[player])->left_pressed;

            if (active_count == 1 && MissionMenuPacketPc.customise_other_player[player] != 0) {
                side = player == 0;
            } else {
                side = player;
            }
            old_mode = CustomiseMode[side];
            if ((pressed & GAMEPAD_ACTION) != 0 && old_mode != 1) {
                randomise = 1;
            } else if ((pressed & GAMEPAD_TAG) != 0) {
                new_mode = 1;
            } else if ((pressed & GAMEPAD_SPECIAL) != 0) {
                new_mode = old_mode;
                do {
                    new_mode = (new_mode + 1) % 3;
                } while (new_mode == 0);
                cancel_name = 1;
                GameAudio_PlaySfx(0x30, NULL, 0, 0);
            } else if (select[player] != 0 && (CustomiseMode[0] == 1 || CustomiseMode[1] == 1)) {
                new_mode = old_mode;
                do {
                    new_mode = (new_mode + 1) % 3;
                } while (new_mode == 0);
                GameAudio_PlaySfx(0x30, NULL, 0, 0);
            } else {
                f32 repeat_delay = old_mode == 1 ? 0.1f : 0.25f;
                i32 category = CustomiseMenuYPc[side];
                i32 piece_count = customiser->piece_counts[category];
                i32 old_piece = Game.customiser_start[side].pieces[category];
                u16 *slot = &Game.customiser_start[side].pieces[category];
                i32 old_category = category;
                i32 next_piece = old_piece;
                i32 up;
                i32 up_held;
                i32 down;
                i32 down_held;
                i32 left;
                i32 left_held;
                i32 right;
                i32 right_held;
                up_held = held & GAMEPAD_DUP;
                down_held = held & GAMEPAD_DDOWN;
                down = pressed & GAMEPAD_DDOWN;
                left_held = held & GAMEPAD_DLEFT;
                left = pressed & GAMEPAD_DLEFT;
                right = pressed & GAMEPAD_DRIGHT;
                up = pressed & GAMEPAD_DUP;
                right_held = held & GAMEPAD_DRIGHT;
                if (up != 0 && down != 0) {
                    up = down = 0;
                }
                if (left != 0 && right != 0) {
                    left = right = 0;
                }
                if (up_held != 0 && down_held != 0) {
                    up_held = down_held = 0;
                }
                if (left_held != 0 && right_held != 0) {
                    left_held = right_held = 0;
                }
                MenuRepeat(&up_held, &up, &uprepeattime[player], &uprepeatcount[player], 0.1f, FRAMETIME);
                MenuRepeat(&down_held, &down, &downrepeattime[player], &downrepeatcount[player], 0.1f, FRAMETIME);
                MenuRepeat(&left_held, &left, &leftrepeattime[player], &leftrepeatcount[player], repeat_delay,
                           FRAMETIME);
                MenuRepeat(&right_held, &right, &rightrepeattime[player], &rightrepeatcount[player], repeat_delay,
                           FRAMETIME);
                if (CustomiseMode[side] == 1) {
                    i32 old_x = CustomiseNameXPc[side];
                    i32 old_y = CustomiseNameYPc[side];
                    if (left != 0) {
                        if (old_x > 0) {
                            Game.customiser_start[side].name[old_x] = CustomiseLetter[old_y];
                            CustomiseNameXPc[side] = old_x - 1;
                            CustomiseNameYPc[side] =
                                Customise_FindLetter(Game.customiser_start[side].name[CustomiseNameXPc[side]]);
                        }
                    } else if (right != 0) {
                        i32 x;
                        NuStrLen(Game.customiser_start[side].name);
                        x = CustomiseNameXPc[side];
                        if (x < 14) {
                            Game.customiser_start[side].name[x] = CustomiseLetter[CustomiseNameYPc[side]];
                            CustomiseNameXPc[side] = x + 1;
                            CustomiseNameYPc[side] = Customise_FindLetter(Game.customiser_start[side].name[x + 1]);
                        }
                    } else if (up != 0) {
                        CustomiseNameYPc[side] = old_y + 1;
                        if (CustomiseNameYPc[side] == 38) {
                            CustomiseNameYPc[side] = 0;
                        }
                    } else if (down != 0) {
                        CustomiseNameYPc[side] = old_y - 1;
                        if (CustomiseNameYPc[side] == -1) {
                            CustomiseNameYPc[side] = 37;
                        }
                    }
                    if (CustomiseNameXPc[side] != old_x || CustomiseNameYPc[side] != old_y) {
                        GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                        CustomiseNameLetterBlipScalePc[side] = 1.5f;
                    }
                } else {
                    if (up_held != 0 && category > 0) {
                        BlipU[player] = 1.0f;
                    } else if (down_held != 0 && category < 8) {
                        BlipD[player] = 1.0f;
                    }
                    if (left_held != 0) {
                        BlipL[player] = 1.0f;
                    } else if (right_held != 0) {
                        BlipR[player] = 1.0f;
                    }
                    if (up != 0) {
                        if (category > 0) {
                            --category;
                        }
                    } else if (down != 0) {
                        if (category < 8) {
                            ++category;
                        }
                    } else if (left != 0) {
                        next_piece = Customiser_NextPieceLeft((CUSTOMISER_LAYERS_PC *)customiser, old_piece,
                                                              piece_count, side, category);
                    } else if (right != 0) {
                        next_piece = Customiser_NextPieceRight((CUSTOMISER_LAYERS_PC *)customiser, old_piece,
                                                               piece_count, side, category);
                    }
                    *slot = (u16)next_piece;
                    CustomiseMenuYPc[side] = category;
                    if (next_piece != old_piece || category != old_category) {
                        GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                        CustomiseMenuTimePc[side] = 0.0f;
                    }
                    if (next_piece != old_piece) {
                        Customiser_SetNameAndIcon((CUSTOMISER_LAYERS_PC *)customiser, side);
                    }
                }
            }
            if (new_mode != -1) {
                CustomiseMode[side] = new_mode;
                if (new_mode == 1) {
                    i32 length;
                    NuStrCpy(Game.customiser_start[side].name + 16, Game.customiser_start[side].name);
                    length = NuStrLen(Game.customiser_start[side].name);
                    if (length < 15) {
                        Game.customiser_start[side].name[length] = ' ';
                    }
                    CustomiseNameYPc[side] =
                        Customise_FindLetter(Game.customiser_start[side].name[CustomiseNameXPc[side]]);
                    CustomiseNameLetterBlipScalePc[side] = 1.0f;
                } else if (old_mode == 1) {
                    if (cancel_name == 0) {
                        i32 last = 14;
                        Game.customiser_start[side].name[CustomiseNameXPc[side]] =
                            CustomiseLetter[CustomiseNameYPc[side]];
                        if (Game.customiser_start[side].name[last] == ' ') {
                            do {
                                --last;
                            } while (Game.customiser_start[side].name[last] == ' ');
                        }
                        if (last < 0) {
                            i16 text_id = side == 0 ? TextIdPc880894 : TextIdPc880898;
                            NuStrCpy(Game.customiser_start[side].name, TTab[text_id]);
                            FinishWeirdoNames(side);
                        } else {
                            Game.customiser_start[side].name[last + 1] = 0;
                        }
                        if (NuStringFilterBadWords((u8 *)Game.customiser_start[side].name,
                                                   (u8 *)Game.customiser_start[side].name,
                                                   (u8 *)CustomiserBadWordReplacementPc) != 0) {
                            NuStrCpy(Game.customiser_start[side].name, Game.customiser_start[side].name + 16);
                        }
                    } else {
                        NuStrCpy(Game.customiser_start[side].name, Game.customiser_start[side].name + 16);
                    }
                }
                Game.customiser_start[side].use_saved_name = 1;
            }
            if (randomise != 0) {
                NUVEC *position = &CustomisePos[side];
                NUVEC debris_position;
                NUMTX matrix;
                i32 category;
                i32 angle;
                PlaySfx((char *)GameAudioName00753354Pc, position);
                NewBuzzFrames(((GAMEPAD_RUNTIME_INPUT_PC *)&GamePadStorage[player])->bound_pad, 1, 0);
                for (category = 0; category < 9; ++category) {
                    if (customiser->piece_counts[category] > 0) {
                        i32 piece;
                        do {
                            piece = qrand() / (65535 / customiser->piece_counts[category] + 1);
                        } while (customiser->piece_available(
                                     (CUSTOMPIECE_LAYERS_PC *)&customiser->piece_sets[category][piece]) == 0);
                        Game.customiser_start[side].pieces[category] = (u16)piece;
                    }
                }
                Customiser_SetNameAndIcon((CUSTOMISER_LAYERS_PC *)customiser, side);
                debris_position.x = 0.0f;
                debris_position.y = 0.21f;
                debris_position.z = 0.0f;
                angle = (i32)((f64)NuTrigTable[CustomiseTiltZPc[side] >> 1] * 910.0);
                CustomiserMtxSetRotateZPc(&matrix, angle);
                angle = (i32)((f64)NuTrigTable[CustomiseTiltXPc[side] >> 1] * 910.0);
                CustomiserMtxRotateXPc(&matrix, angle);
                angle = (i32)((f64)NuTrigTable[CustomiseRotYPc[side] >> 1] * 10922.0);
                CustomiserMtxRotateYPc(&matrix, angle);
                NuVecMtxRotate(&debris_position, &debris_position, &matrix);
                NuVecAdd(&debris_position, &debris_position, position);
                AddGameDebris(WORLD->game_debris_sys, 0x7b, &debris_position);
                AddGameDebris(WORLD->game_debris_sys, 0x5c, &debris_position);
            }
        }
    }
    if (quit != 0) {
        return 1;
    }
    if (toggle != 0) {
        GAMEOBJECT *object;
        MissionMenuPacketPc.customise_other_player[active_index] =
            MissionMenuPacketPc.customise_other_player[active_index] == 0;
        GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
        object = Player[active_index];
        NewBuzzFrames(object->pad->bound_pad, 1, 0);
    }
    return 0;
}

void CustomiserMenu_End() {
    LevLock[1] = 1;
    LevTime[1] = 0.6f;
    MenuRememberCursor(&GameMenu[GameMenuLevel]);
    MenuReset();
    ResetTimer(&JoinInTimer, 0.0f);
    CustomiseMode[0] = 2;
    CustomiseMode[1] = 2;
    if (GAMEDEMO != 0) {
        NewLData = HUB_LDATA;
        NextArea_FreePlay = 1;
        FreePlay = 1;
        makeplayerlist_freeplay = 1;
        makefreeplaymodellist = 1;
        GAMEDEMO = 2;
    }
}

static f32 CustomiseNameLetterScalePc = 0.5f;
static f32 CustomiseNameGapPc = 0.05249999836087227f;
static f32 CustomiseNameSideGapPc = 0.05999999865889549f;
static f32 CustomiseNameArrowScalePc = 0.6000000238418579f;
static f32 CustomiseNameArrowOffsetPc = 0.10000000149011612f;
static f32 CustomiseNameBracketOffsetXPc = 0.009999999776482582f;
static f32 CustomiseNameBracketOffsetYPc = 0.007499999832361937f;

static void CustomiserDrawNameEditorPc(CUSTOMISER_LAYERS_PC *input) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 side;
    i32 active_count;
    i32 index;
    char name[64];
    if (customiser == NULL || CustomiseNameAlphaPc <= 0.0f) {
        return;
    }
    Customiser_GetActiveWeirdoIndex(&side, &active_count);
    if (active_count == 1 && MissionMenuPacketPc.customise_other_player[side] != 0) {
        side = side == 0;
    }
    for (index = 0; index < active_count; ++index, ++side) {
        const i32 red = side == 0 ? 0 : 127;
        const i32 green = side == 0 ? 191 : 255;
        const i32 blue = side == 0 ? 255 : 0;
        f32 x;
        f32 alpha;
        i32 icon;
        if (CustomiseMode[side] == 1) {
            char letter[2];
            f32 gap = CustomiseNameGapPc;
            i32 length;
            i32 column;
            letter[1] = 0;
            if (Game.options_save.widescreen != 0) {
                gap = (f32)(gap * 0.800000011920929);
            }
            for (length = 15; length > 0; --length) {
                if (Game.customiser_start[side].name[length - 1] != ' ') {
                    break;
                }
            }
            x = (f32)(CustomiseScreenPosPc[side].x - gap * 7.0);
            if (active_count == 2) {
                x += side == 0 ? -CustomiseNameSideGapPc : CustomiseNameSideGapPc;
            }
            for (column = 0; column < 15; ++column) {
                f32 scale;
                i32 letter_red;
                i32 letter_green;
                i32 letter_blue;
                if (column == CustomiseNameXPc[side]) {
                    const i32 colour = menu_flash == 0 ? 255 : 191;
                    letter[0] = CustomiseLetter[CustomiseNameYPc[side]];
                    Text3D((char *)InitAsciiUpPc, x, (f32)(CustomiseNameArrowOffsetPc - (f64)0.6499999761581421f), 1.0f,
                           CustomiseNameArrowScalePc, CustomiseNameArrowScalePc, CustomiseNameArrowScalePc, 0, colour,
                           colour, colour);
                    Text3D((char *)TagAsciiDownPc, x, (f32)(-(f64)0.6499999761581421f - CustomiseNameArrowOffsetPc),
                           1.0f, CustomiseNameArrowScalePc, CustomiseNameArrowScalePc, CustomiseNameArrowScalePc, 0,
                           colour, colour, colour);
                    scale = CustomiseNameLetterBlipScalePc[side];
                    letter_red = letter_blue = 255;
                    letter_green = menu_flash == 0 ? 255 : 0;
                } else {
                    letter[0] = Game.customiser_start[side].name[column];
                    if (column > CustomiseNameXPc[side] && column >= length && letter[0] == ' ') {
                        letter[0] = '.';
                    }
                    scale = 1.0f;
                    letter_red = red;
                    letter_green = green;
                    letter_blue = blue;
                }
                Text3D(letter, x, -0.6499999761581421f, 1.0f, scale * CustomiseNameLetterScalePc, (f32)(scale * 0.5),
                       0.5f, 0, letter_red, letter_green, letter_blue);
                if (column == 0) {
                    Text3D("[", (x - gap) + CustomiseNameBracketOffsetXPc,
                           (f32)(CustomiseNameBracketOffsetYPc - (f64)0.6499999761581421f), 1.0f, 0.5f,
                           0.6000000238418579f, 0.5f, 0, red, green, blue);
                } else if (column == 14) {
                    Text3D("]", (x + gap) - CustomiseNameBracketOffsetXPc,
                           (f32)(CustomiseNameBracketOffsetYPc - (f64)0.6499999761581421f), 1.0f, 0.5f,
                           0.6000000238418579f, 0.5f, 0, red, green, blue);
                }
                x += gap;
            }
        } else {
            GameObj_GetNamePc(customiser->character_ids[side], NULL, name);
            SmartTextEx(name, CustomiseScreenPosPc[side].x, -0.6499999761581421f, 1.0f, 0.5f, 0.5f, 0.5f, 0, red, green,
                        blue, 0.75f, 2, NULL, 0, (i32)(CustomiseNameAlphaPc * 128.0));
        }
        x = CustomiseScreenPosPc[side].x;
        alpha = (1.0f - CustomiseNameBoardMulPc[side]) * CustomiseNameAlphaPc;
        DrawPanel3DObject(x, -0.47999998927116394f, 1.0f, MissionPlayerIconSizePc, MissionPlayerIconSizePc,
                          MissionPlayerIconSizePc, 0, 0, 0, &WORLD->level_objects[167].special, 0, alpha);
        icon = Customiser_GetIcon(input, &Game.customiser_start[side], index);
        if (icon != -1) {
            if (icon != PCArcadeUnpairedIconModel) {
                ++icon;
            }
            DrawPanel3DObject(x, -0.47999998927116394f, 1.0f, MissionPlayerIconSizePc, MissionPlayerIconSizePc,
                              MissionPlayerIconSizePc, 0, 0, 0, &WORLD->level_objects[icon].special, 0, alpha);
        }
    }
}

static NUVEC CustomiseNoHatPosPc[2] = {0};
static f32 CustomiseHeadOffsetPc = 0.21f;

void __cdecl Customiser_Update(CUSTOMISER_LAYERS_PC *input, WORLDINFO *world) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOMISER_UPDATE_PC *runtime = (CUSTOMISER_UPDATE_PC *)input;
    /* Native conversions must all happen before the NULL test. */
    const i32 rotation_step = (i32)(10922.0 * FRAMETIME);
    const i32 tilt_x_step = (i32)(12379.0 * FRAMETIME);
    const i32 tilt_z_step = (i32)(10376.0 * FRAMETIME);
    const i32 bob_step = (i32)(20206.0 * FRAMETIME);
    i32 active_side;
    i32 active_count;
    i32 preview_side;
    i32 colour;
    i32 side;
    i32 category;
    ADD_GAME_MESSAGE_PC message;
    if (customiser == NULL) {
        return;
    }
    Customiser_GetActiveWeirdoIndex(&active_side, &active_count);
    preview_side = -1;
    if (active_count == 1) {
        preview_side = MissionMenuPacketPc.customise_other_player[active_side] != 0 ? active_side == 0 : active_side;
    }
    message = AddGameMsg_Default;
    colour = menu_flash == 0 ? 255 : 191;
    for (side = 0; side < 2; ++side) {
        const i32 control_side = active_count == 2                           ? side
                                 : MissionMenuPacketPc.active_player[0] != 0 ? 0
                                 : MissionMenuPacketPc.active_player[1] != 0 ? 1
                                                                             : -1;
        u32 torso_flags;
        CHARACTER_MODEL_LAYERS_PC *model;
        ANIMPACKET_PC *packet;
        i32 saber_colour;
        i32 intro;
        u16 angle;
        f32 no_hat;
        CustomiseRotYPc[side] += rotation_step;
        CustomiseTiltXPc[side] += tilt_x_step;
        CustomiseTiltZPc[side] += tilt_z_step;
        CustomiseBobPc[side] += bob_step;
        UpdateCustomPieceAnim(&CustomiseHeadAnimPc[side], 0xd556, 0x2aaa);
        UpdateCustomPieceAnim(&CustomiseArmsAnimPc[side], 0x1555, 0x4000);
        UpdateCustomPieceAnim(&CustomiseLegsAnimPc[side], 0xe000, 0x4000);
        torso_flags = customiser->piece_sets[1][Game.customiser_start[side].pieces[1]].availability_flags;
        if ((torso_flags & 1) != 0) {
            if ((torso_flags & 4) != 0) {
                if ((customiser->piece_sets[0][Game.customiser_start[side].pieces[0]].availability_flags & 8) != 0) {
                    no_hat = CustomiseHeadOffsetPc;
                } else {
                    no_hat = (f64)CustomiseHeadOffsetPc * (f64)0.8f;
                }
            } else {
                if ((customiser->piece_sets[0][Game.customiser_start[side].pieces[0]].availability_flags & 8) != 0) {
                    no_hat = (f64)CustomiseHeadOffsetPc * (f64)0.8f;
                } else {
                    no_hat = (f64)CustomiseHeadOffsetPc * (f64)0.6f;
                }
            }
        } else {
            no_hat = 0.0f;
        }

        CustomiseNoHatPosPc[side].y = SeekValF(CustomiseNoHatPosPc[side].y, no_hat, 10.0f);
        CustomiseMenuTimePc[side] += FRAMETIME;
        model = APICharacterLoaded(customiser->character_ids[side]);
        if (model == NULL) {
            continue;
        }
        saber_colour = LightSabre_ColourFromObj(
            customiser->piece_sets[2][Game.customiser_start[side].pieces[2]].weapon_model, NULL);
        if (runtime->animation_state[side] > 0) {
            --runtime->animation_state[side];
        }
        packet = &runtime->animation_packets[side].packet;
        if (runtime->animation_active[side] == 0 &&
            model->model_data_b[intro = saber_colour == -1 ? 0x62 : 0x61] != NULL) {
            if (runtime->animation_values[side] == 0.0f) {
                runtime->animation_values[side] = AnimDuration(model->model_id, intro, 0.0f, 0.0f, 1);
            } else {
                runtime->animation_values[side] -= FRAMETIME;
                if (runtime->animation_values[side] <= 0.0f) {
                    runtime->animation_active[side] = 1;
                }
            }
        } else {
            runtime->animation_active[side] = 1;
        }
        packet->previous_animation = packet->animation_index;
        if (saber_colour != -1) {
            packet->requested_animation = runtime->animation_active[side] == 0 ? 0x61 : 0xbe;
        } else {
            packet->requested_animation = runtime->animation_active[side] == 0 ? 0x62 : 0x63;
        }
        UpdateAnimPacket(model, packet, (f64)FRAMETIME * 30.0, 0.0f, FRAMETIME, 0.0f);
        angle = CustomiseYRotPc[side] + 0xc000;
        for (category = 0; category < 9; ++category) {
            i32 locator;
            CUSTOM_RESOURCE_UPDATE_PC *resources;
            i32 material;
            if (category == CustomiseMenuYPc[side] && GetMenuID() == 12 && customiser_quit == 0 &&
                (active_count == 2 || side == preview_side) && control_side != -1 &&
                (locator = customiser->locator_indices[category]) != -1 && model->points_of_interest[locator] != NULL) {
                NUVEC position;
                position = *(NUVEC *)&runtime->joint_matrices[side][locator].m30;
                message.text = InitAsciiLeftPc;
                message.unknown_4f = 2;
                if (customiser->locator_x_offsets[category] != 0.0f) {
                    position.x += NuTrigTable[(angle >> 1) & 0x7fff] * customiser->locator_x_offsets[category];
                    position.z +=
                        NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * customiser->locator_x_offsets[category];
                }
                position.y = customiser->locator_y_offsets[category] + position.y;

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
            if (resources == NULL || customiser->piece_counts[category] <= 0) {
                continue;
            }
            if (customiser->categories[category]->uses_special != 0 ||
                customiser->categories[category]->material_tag == -1) {
                continue;
            }
            resources += Game.customiser_start[side].pieces[category];
            if (resources->texture_id == 0) {
                continue;
            }
            for (material = 0; material < ((CUSTOM_HIERARCHY_UPDATE_PC *)model->hierarchy)->material_count;
                 ++material) {
                if (((CUSTOM_MATERIAL_UPDATE_PC *)((CUSTOM_HIERARCHY_UPDATE_PC *)model->hierarchy)->materials[material])
                        ->material_tag == customiser->categories[category]->material_tag) {
                    ((CUSTOM_HIERARCHY_UPDATE_PC *)model->hierarchy)->materials[material]->texture_id =
                        (u16)resources->texture_id;
                    NuMtlUpdate(((CUSTOM_HIERARCHY_UPDATE_PC *)model->hierarchy)->materials[material]);
                }
            }
        }
    }
    {
        const f32 target = GetMenuID() == 12 && customiser_quit == 0 ? 1.0f : 0.0f;
        CustomiseNameAlphaPc = SeekLinearF(CustomiseNameAlphaPc, target, 1.5 * FRAMETIME);
    }
}

/* 00425fa0 / 1087 */
void __cdecl Customiser_Draw3D(CUSTOMISER_LAYERS_PC *input) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOMISER_UPDATE_PC *runtime = (CUSTOMISER_UPDATE_PC *)input;
    i32 active_side;
    i32 active_count;
    i32 preview_side;
    i32 side;
    if (customiser == NULL || world->camera_splines[18] == NULL) {
        return;
    }
    Customiser_GetActiveWeirdoIndex(&active_side, &active_count);
    preview_side = -1;
    if (active_count == 1) {
        preview_side = MissionMenuPacketPc.customise_other_player[active_side] != 0 ? active_side == 0 : active_side;
    }
    for (side = 0; side < 2; ++side) {
        i32 character_id = customiser->character_ids[side];
        CHARACTER_MODEL_LAYERS_PC *model = APICharacterLoaded(character_id);
        PLAYERCHARACTERCONFIG_PC *character;
        i32 helmet;
        i32 selected;
        i32 category;
        RTL_DATA_PC light_data;
        NUMTX matrix;
        u32 mask;
        if (model == NULL || runtime->animation_state[side] > 0) {
            continue;
        }
        character = &GCDataList[character_id];
        helmet = character->helmet_locator;
        if (helmet != -1 && model->points_of_interest[helmet] == NULL) {
            helmet = -1;
        }
        selected = CustomiseMenuYPc[side];
        rtlResetEx(&light_data, 1);
        rtlApplySetScale(world->rtl_set, &light_data, &CustomisePos[side], NULL, -1, 1.0f);
        SetLights_RTLDATA(&light_data, 1.0f);
        mask = CustomiserPreviewLayerMaskPc(customiser, Game.customiser_start[side].pieces, selected, character);
        NuMtxSetRotationY(&matrix, CustomiseYRotPc[side] + 0x8000);
        NuMtxTranslate(&matrix, &CustomisePos[side]);
        if (GameDrawCharacterModel(model, &runtime->animation_packets[side].packet, &matrix, NULL, NULL,
                                   runtime->joint_matrices[side], NULL, mask) == 0) {
            continue;
        }
        if (helmet != -1) {
            for (category = 0; category < 9; ++category) {
                CUSTOM_ACCESSORY_RESOURCE_PC *resources =
                    (CUSTOM_ACCESSORY_RESOURCE_PC *)world->customiser_resources[category];
                CUSTOM_ACCESSORY_RESOURCE_PC *resource;
                CUSTOM_PIECE_CONFIG_PC *piece;
                if (resources == NULL || customiser->piece_counts[category] <= 0 ||
                    customiser->categories[category]->uses_special == 0) {
                    continue;
                }
                resource = &resources[Game.customiser_start[side].pieces[category]];
                if (NuSpecialExistsFn(&resource->special) == 0) {
                    continue;
                }
                piece = &customiser->piece_sets[category][Game.customiser_start[side].pieces[category]];
                if (GetMenuID() == 12 && customiser_quit == 0 && category == selected &&
                    (active_count == 2 || side == preview_side)) {
                    NuFmod(CustomiseMenuTimePc[side], 0.3f);
                } else if (category == 0 && (piece->availability_flags & 0x20) != 0) {
                    continue;
                }
                matrix = runtime->joint_matrices[side][helmet];
                if (category == 0) {
                    NuMtxPreTranslate(&matrix, &CustomiseNoHatPosPc[side]);
                }
                NuSpecialDrawAt(&resource->special, &matrix);
            }
        }
        if (customiser->piece_counts[2] > 0) {
            i32 hand = character->weapon_joints[0];
            CUSTOM_PIECE_CONFIG_PC *weapon;
            LEVEL_OBJECT_RUNTIME_PC *object;
            if (hand == -1 || model->points_of_interest[hand] == NULL) {
                continue;
            }
            weapon = &customiser->piece_sets[2][Game.customiser_start[side].pieces[2]];
            if (weapon->weapon_model == -1) {
                continue;
            }
            object = &world->level_objects[weapon->weapon_model];
            if (object->active != 0) {
                i32 blade;
                NUSPECIALHANDLE *special;
                matrix = runtime->joint_matrices[side][hand];
                NuSpecialDrawAt(&object->special, &matrix);
                if (LightSabre_ColourFromObj(weapon->weapon_model, &blade) != -1) {
                    if (blade != -1) {
                        special = &world->level_objects[blade].special;
                        if (special != NULL) {
                            NuSpecialDrawAt(special, &matrix);
                        }
                    }
                    special = &world->level_objects[17].special;
                    if (special != NULL) {
                        NuSpecialDrawAt(special, &matrix);
                    }
                }
            }
        }
    }
    SetLevelLights(world->rtl_set, 1.0f);
}

static const char CustomiserMenuBackSfxPc[9] = "menuBack";

void __cdecl MenuUpdateCustomiserPc(MENU *menu) {
    i32 quit;
    (void)menu;
    if (customiser_save_done != 0) {
        CustomiserMenu_End();
        customiser_save_done = 0;
        return;
    }
    quit = CustomiserMenu_Update();
    customiser_changed =
        memcmp(Game.customiser_start, OldCustomiseGame.customiser_start, 0x70) != 0 ||
        memcmp(Game.customiser_start[0].name, OldCustomiseGame.customiser_start[0].name, 0x20) != 0 ||
        Game.customiser_start[0].use_saved_name != OldCustomiseGame.customiser_start[0].use_saved_name ||
        Game.customiser_start[1].use_saved_name != OldCustomiseGame.customiser_start[1].use_saved_name;
    if (quit == 0) {
        return;
    }
    PlaySfx((char *)CustomiserMenuBackSfxPc, NULL);
    customiser_quit = 1;
    if (GAMEDEMO == 0 && customiser_changed != 0) {
        Hub_ClearStats();
        Text_FillInExtendedSaveInfo();
        if (TriggerAutoSave() != 0) {
            CustomiserMenu_End();
            return;
        }
        customiser_save_done = 1;
        NewMenu(1000, -1, -1);
        return;
    }
    CustomiserMenu_End();
}

void __cdecl MenuDrawCustomiserPc(MENU *menu) {
    i32 active_index;
    i32 active_count;
    i32 player;
    i32 side;
    i32 select[2];
    i32 action[2];
    i32 cancel[2];
    i32 toggle[2];
    i32 colour;
    f32 x;
    f32 y;
    f32 alpha;
    (void)menu;
    if (MenuStopDraw != 0 || customiser_quit != 0 || CharacterCustomiser == NULL) {
        return;
    }
    CustomiserDrawNameEditorPc(CharacterCustomiser);
    Customiser_GetActiveWeirdoIndex(&active_index, &active_count);
    alpha = MissionMenuPacketPc.active_player[0] != 0 ? 1.0f : MissionDropInAlphaPc;
    DrawCharIcon(MissionMenuPacketPc.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 0xa6, alpha,
                 alpha, 1, NULL);
    alpha = MissionMenuPacketPc.active_player[1] != 0 ? 1.0f : MissionDropInAlphaPc;
    DrawCharIcon(MissionMenuPacketPc.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 0xa5, alpha,
                 alpha, 1, NULL);
    select[0] = select[1] = active_count == 1 ? TextIdPc880750 : -1;
    action[0] = action[1] = TextIdPc880758;
    toggle[0] = toggle[1] = -1;
    if (GAMEDEMO != 0) {
        if (MissionMenuPacketPc.active_player[0] != 0 && MissionMenuPacketPc.active_player[1] != 0) {
            if (MissionMenuPacketPc.selected[0] != 0) {
                select[0] = action[0] = -1;
                cancel[0] = TextIdPc880890;
            } else {
                cancel[0] = TextIdPc880760;
            }
            if (MissionMenuPacketPc.selected[1] != 0) {
                select[1] = action[1] = -1;
                cancel[1] = TextIdPc880890;
            } else {
                cancel[1] = TextIdPc880760;
            }
        } else {
            cancel[0] = cancel[1] = TextIdPc880858;
        }
    } else {
        for (player = 0; player < 2; ++player) {
            if (active_count == 1 && MissionMenuPacketPc.customise_other_player[player] != 0) {
                side = player == 0;
            } else {
                side = player;
            }
            if (CustomiseMode[side] == 1) {
                select[player] = tACCEPT;
                action[player] = cancel[player] = -1;
            }
            toggle[player] = Customise_GetToggleString(side);
            cancel[player] = -1;
            if (CustomiseMode[0] != 1 && CustomiseMode[1] != 1) {
                if (customiser_changed != 0) {
                    cancel[player] = TextIdPc880754;
                } else {
                    cancel[player] = TextIdPc880844;
                }
            }
        }
    }
    DrawPlayerIconPrompts(MissionMenuPacketPc.active_player[0], select[0], 1.0f, action[0], cancel[0], toggle[0],
                          TextIdPc880eac, MissionMenuPacketPc.active_player[1], select[1], 1.0f, action[1], cancel[1],
                          toggle[1], TextIdPc880eac);
    x = -ICONX;
    colour = menu_flash == 0 ? 255 : 191;
    for (player = 0; player < 2; ++player) {
        if (active_count == 1 && MissionMenuPacketPc.customise_other_player[player] != 0) {
            side = player == 0;
        } else {
            side = player;
        }
        if (MissionMenuPacketPc.active_player[player] != 0 && CustomiseMode[side] != 1) {
            y = (f32)(STATSPOSY - 0.22499999403953552);
            Text3D("<", (f32)(x - 0.05000000074505806 - (BlipL[player] > 0.0f ? 0.015000000596046448f : 0.0f)), y, 1.0f,
                   0.5f, 0.5f, 0.5f, 0, colour, colour, colour);
            Text3D(">", (f32)(x + 0.05000000074505806 + (BlipR[player] > 0.0f ? 0.015000000596046448f : 0.0f)), y, 1.0f,
                   0.5f, 0.5f, 0.5f, 0, colour, colour, colour);
            Text3DEx((char *)InitAsciiUpPc, x,
                     (f32)(y - -0.07000000029802322 - (BlipU[player] > 0.0f ? -0.021000001579523087f : 0.0f)), 1.0f,
                     0.5f, 0.5f, 0.5f, 0, colour, colour, colour, CustomiseMenuYPc[side] > 0 ? 128 : 32);
            Text3DEx((char *)TagAsciiDownPc, x,
                     (f32)(y - 0.07000000029802322 + (BlipD[player] > 0.0f ? -0.021000001579523087f : 0.0f)), 1.0f,
                     0.5f, 0.5f, 0.5f, 0, colour, colour, colour, CustomiseMenuYPc[side] < 8 ? 128 : 32);
            SmartTextEx(TTab[TextIdPc880888], x, (f32)(STATSPOSY - 0.375), 1.0f, 0.30000001192092896f,
                        0.30000001192092896f, 0.30000001192092896f, 0, colour, colour, colour, 0.15000000596046448f, 1,
                        NULL, 0, 128);
        }
        x += ICONX + ICONX;
    }
}
