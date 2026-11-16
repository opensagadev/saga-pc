#include <stdio.h>
#include <string.h>
#include "gameapi/edptl.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/customiser.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "legoapi/core/startup/text.h"
#include "legoapi/gizmo/base/gizmo.h"

typedef struct customiser_game_setting_s {
    char *name;
    u32 model_flags;
    u32 gameplay_flags;
} CUSTOMISER_GAME_SETTING;

/* Source-private views for customise.c. Config/piece/category types already defined there. */
typedef struct custom_piece_anim_s {
    f32 duration;
    f32 elapsed;
    f32 hold_time;
    u16 start_angle;
    u16 target_angle;
    u16 current_angle;
    u8 unknown_12[2];
} CUSTOM_PIECE_ANIM;

typedef struct custom_resource_update_s {
    u8 unknown_00[0x14];
    u32 texture_id;
    u8 unknown_18[8];
} CUSTOM_RESOURCE_UPDATE;

typedef struct custom_hierarchy_update_s {
    u8 unknown_00[0xc];
    NUMTL **materials;
    i32 material_count;
} CUSTOM_HIERARCHY_UPDATE;

typedef struct custom_material_update_s {
    u8 unknown_00[0x9a];
    u8 material_tag;
} CUSTOM_MATERIAL_UPDATE;

extern NUVEC CustomisePos[2];
i32 GameDrawCharacterModel(CHARACTER_MODEL_LAYERS *, ANIMPACKET *, NUMTX *, NUMTX *, NUMTX *, NUMTX *, GAMEOBJECT *, u32);

/* Native customiser menu drawing and private matrix helpers; append to customise.c. */

static void CustomiserMtxSetRotateZ(NUMTX *matrix, i32 angle) {
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

static void CustomiserMtxRotateX(NUMTX *matrix, i32 angle) {
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

static void CustomiserMtxRotateY(NUMTX *matrix, i32 angle) {
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

static i32 Customise_FindLetter(char character) {
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

void FinishWeirdoNames(i32 which) {
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

static void UpdateCustomPieceAnim(CUSTOM_PIECE_ANIM *anim, u16 minimum, u16 maximum) {
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

static CUSTOMISER_GAME_SETTING CustomiserGameSettingsPrivate[] = {
    {"bountyhunter", 0x1000000, 0},     {"jedi", 8, 0}, {"sith", 0xc, 2}, {"blaster", 0x100080, 0x40000000}, {"alreadygothat", 0, 0x10},
    {"stormtrooperhelmet", 0, 0x40000}, {NULL, 0, 0}};

void PermanentCustomPieceConfig(CUSTOM_PIECE_CONFIG *piece, NUFPAR *parser) {
    CUSTOMISER_GAME_SETTING *setting;
    for (setting = CustomiserGameSettingsPrivate; setting->name != NULL; ++setting) {
        if (NuStrICmp(parser->word_buffer_ptr, setting->name) == 0) {
            piece->model_flags |= setting->model_flags;
            piece->gameplay_flags |= setting->gameplay_flags;
            break;
        }
    }
}

void Customiser_InitNames(CUSTOMISER_CONFIG *customiser) {
    i32 index;
    if (customiser == NULL) {
        return;
    }
    for (index = 0; index < 2; ++index) {
        if (customiser->character_ids[index] != -1) {
            i32 text = CDataList[customiser->character_ids[index]].name_id;
            if (text != -1) {
                NuStrCpy(customiser->display_names[index], TTab[text]);
                TTab[text] = customiser->display_names[index];
            }
        }
    }
}

/* 00423c30 / 874. Native contains class/name-part accumulation even though
 * the final displayed name is the generic stranger label. */
void Customiser_SetNameAndIcon(CUSTOMISER_CONFIG *customiser, i32 index) {
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
            CUSTOM_PIECE_CONFIG *piece;
        } parts[2];

        CUSTOM_PIECE_CONFIG *third;
        const u16 *selection = Game.customiser_start[side].pieces;
        if (index != -1 && side != index) {
            continue;
        }
        customiser->display_names[side][0] = 0;
        third = NULL;
        parts[1].piece = NULL;
        parts[0].piece = NULL;
        for (group = 0; group < 11; ++group) {
            counts[group] = 0;
        }

        for (category = 0; category < 9; ++category) {
            CUSTOM_PIECE_CONFIG *piece;
            if (customiser->piece_counts[category] <= 0) {
                continue;
            }
            if (category == 0 && (customiser->piece_sets[1][selection[1]].availability_flags & CUSTOM_PIECE_NO_HAT) != 0) {
                continue;
            }
            if (category == 1 && (customiser->piece_sets[0][selection[0]].availability_flags & CUSTOM_PIECE_HELMET) != 0 &&
                (customiser->piece_sets[1][selection[1]].availability_flags & CUSTOM_PIECE_NO_HAT) == 0) {
                continue;
            }
            piece = &customiser->piece_sets[category][selection[category]];
            if (piece->bonus_group != -1) {
                counts[piece->bonus_group] += (i8)customiser->categories[category]->material_indices[0];
            }
            if (parts[0].piece == NULL && piece->part_slots[0] != 0) {
                parts[0].piece = piece;
            } else if (parts[1].piece == NULL && piece->part_slots[1] != 0) {
                parts[1].piece = piece;
            } else if (third == NULL && piece->part_slots[2] != 0) {
                third = piece;
            }
        }
        for (group = 0; group < 11; ++group) {
            if (counts[group] >= 4 && (best == -1 || counts[group] > counts[best])) {
                best = group;
            }
        }
        sprintf(customiser->display_names[side], "%s %i", TTab[dat_00880fa0], side + 1);
        CDataList[customiser->character_ids[side]].icon_model = (i16)Customiser_GetIcon(customiser, &Game.customiser_start[side], side);
    }
}

i32 PermanentCustomPieceAvailable(CUSTOM_PIECE_CONFIG *piece) {
    if (GAMEDEMO != 0) {
        if ((piece->availability_flags & CUSTOM_PIECE_NOT_DEMO) != 0) {
            return 0;
        }
    } else {
        u32 required_flags;
        if ((piece->availability_flags & (CUSTOM_PIECE_100_PERCENT_SIDE_1 | CUSTOM_PIECE_100_PERCENT_SIDE_2)) != 0 &&
            Game_100PercentComplete() == 0) {
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

f32 BlipD[2] = {0};
static CUSTOM_PIECE_ANIM CustomiseArmsAnim[2] = {0};
static CUSTOM_PIECE_ANIM CustomiseLegsAnim[2] = {0};
i32 CustomiseMode[2] = {0};
static CUSTOM_PIECE_ANIM CustomiseHeadAnim[2] = {0};
static u16 CustomiseTiltX[2] = {0};
static u16 CustomiseBob[2] = {0};
i32 customiser_changed = 0;
NUVEC CustomisePos[2] = {0};
f32 BlipR[2] = {0};
f32 BlipL[2] = {0};
static u16 CustomiseTiltZ[2] = {0};
static u16 CustomiseRotY[2] = {0};
static u16 CustomiseYRot[2] = {0};
CUSTOMISER_CONFIG *CharacterCustomiser = 0;
static f32 CustomiseNameAlpha = 0;
i32 customiser_save_done = 0;
static f32 CustomiseNameBoardTMul[2] = {0};
static f32 CustomiseNameBoardMul[2] = {0};
static f32 CustomiseNameLetterBlipScale[2] = {0};

/* 00424040 / 775 */
void Customiser_Init(CUSTOMISER_CONFIG *customiser) {
    i32 side;
    i32 category;
    if (customiser == NULL) {
        return;
    }
    for (side = 0; side < 2; ++side) {
        ResetAnimPacket(&customiser->animation_packets[side].packet, -1);
        customiser->animation_active[side] = 0;
        customiser->animation_values[side] = 0.0f;
        customiser->animation_state[side] = 2;
        CustomiseMode[side] = 2;
    }
    if (WORLD->camera_splines[18] != NULL) {
        for (side = 0; side < 2; ++side) {
            NUVEC *next;
            CustomisePos[side] = WORLD->camera_splines[18]->pts[side * 2];
            next = &WORLD->camera_splines[18]->pts[side * 2 + 1];
            CustomiseYRot[side] = NuAtan2D(next->x - CustomisePos[side].x, next->z - CustomisePos[side].z);
            CustomiseHeadAnim[side].current_angle = 0;
            CustomiseHeadAnim[side].hold_time = (f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5 * 0.5;
            CustomiseHeadAnim[side].start_angle = 0;
            CustomiseHeadAnim[side].target_angle = 0;
            CustomiseHeadAnim[side].elapsed = 0.0f;
            CustomiseHeadAnim[side].duration = 0.0f;
            CustomiseArmsAnim[side].current_angle = 0;
            CustomiseArmsAnim[side].hold_time = (f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5 * 0.5;
            CustomiseArmsAnim[side].start_angle = 0;
            CustomiseArmsAnim[side].target_angle = 0;
            CustomiseArmsAnim[side].elapsed = 0.0f;
            CustomiseArmsAnim[side].duration = 0.0f;
            CustomiseLegsAnim[side].current_angle = 0;
            CustomiseLegsAnim[side].hold_time = (f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5 * 0.5;
            CustomiseLegsAnim[side].start_angle = 0;
            CustomiseLegsAnim[side].target_angle = 0;
            CustomiseLegsAnim[side].elapsed = 0.0f;
            CustomiseLegsAnim[side].duration = 0.0f;
        }
        CustomiseRotY[0] = qrand();
        CustomiseRotY[1] = CustomiseRotY[0] + 0x4000;
        CustomiseTiltX[0] = qrand();
        CustomiseTiltX[1] = CustomiseTiltX[0] + 0x8000;
        CustomiseTiltZ[0] = qrand();
        CustomiseTiltZ[1] = CustomiseTiltZ[0] + 0x8000;
        CustomiseBob[0] = qrand();
        CustomiseBob[1] = CustomiseBob[0] + 0x8000;
    }
    Customiser_SetNameAndIcon(customiser, -1);
    CustomiseNameAlpha = 0.0f;
    for (side = 0; side < 2; ++side) {
        for (category = 0; category < 9; ++category) {
            if (customiser->piece_counts[category] > 0) {
                Game.customiser_start[side].pieces[category] = (u16)Customiser_NextPieceRight(
                    customiser, Game.customiser_start[side].pieces[category] - 1, customiser->piece_counts[category], side, category);
            }
        }
    }
    CustomiseNameBoardTMul[1] = 0.0f;
    CustomiseNameBoardMul[0] = 0.0f;
    CustomiseNameLetterBlipScale[1] = 1.0f;
    CustomiseNameLetterBlipScale[0] = 1.0f;
}

static f32 CustomiseMenuTime[2] = {0};
f32 BlipU[2] = {0};

void Customiser_Reset(CUSTOMISER_CONFIG *customiser) {
    i32 side;
    if (customiser == NULL) {
        return;
    }
    for (side = 0; side < 2; ++side) {
        CustomiseMenuTime[side] = 0.0f;
        ResetAnimPacket(&customiser->animation_packets[side].packet, -1);
        customiser->animation_active[side] = 0;
        customiser->animation_values[side] = 0.0f;
        customiser->animation_state[side] = 2;
    }
}

static NUVEC CustomiseScreenPos[2] = {0};

/* 004243b0 / 42 */
void Customiser_TransformToPanel(CUSTOMISER_CONFIG *unused) {
    NuCameraTransformScreenClip(&CustomiseScreenPos[0], &CustomisePos[0], 1, NULL);
    NuCameraTransformScreenClip(&CustomiseScreenPos[1], &CustomisePos[1], 1, NULL);
}

i32 customiser_quit = 0;

/* 004243e0 / 273; static native callee receives customiser in EDI. */
static u32 CustomiserPreviewLayerMask(CUSTOMISER_CONFIG *customiser, const u16 *selection, i32 selected, PLAYERCHARACTERCONFIG *character) {
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
        if ((selected == -1 || GetMenuID() != MENU_CUSTOMISER || customiser_quit != 0 || selected != 5) &&
            (customiser->piece_sets[5][selection[5]].availability_flags & CUSTOM_PIECE_CAPE_OFF) != 0) {
            mask &= ~(1u << character->cape_layer);
        }
    }
    return mask;
}

/* 00424500 / 491: applies both saved customisations to their characters'
 * layer masks, model and gameplay flags, weapon and blade colour. */
void Customiser_SetUpCharacterData(CUSTOMISER_CONFIG *customiser) {
    u32 side;
    i32 category;
    if (customiser == NULL) {
        return;
    }
    for (side = 0; side < sizeof(Game.customiser_start) / sizeof(Game.customiser_start[0]); ++side) {
        const u16 *selection = Game.customiser_start[side].pieces;
        CHARACTERDATA *character = &CDataList[customiser->character_ids[side]];
        PLAYERCHARACTERCONFIG *config = character->game_character;
        u32 mask;
        config->flags_090 = 0;
        mask = CustomiserPreviewLayerMask(customiser, selection, -1, config);
        config->layer_mask_special = mask;
        config->layer_mask = mask;
        config->layer_mask_medium = mask;
        config->layer_mask_low = mask;
        config->layer_mask_dead = mask;
        character->model_flags &= 3;
        for (category = 0; category < 9; ++category) {
            if (customiser->piece_counts[category] > 0) {
                if (category != 0 || (customiser->piece_sets[1][selection[1]].availability_flags & CUSTOM_PIECE_NO_HAT) == 0) {
                    character->model_flags |= customiser->piece_sets[category][selection[category]].model_flags;
                }
                if ((character->model_flags & CHARACTER_MODEL_BLASTER) != 0) {
                    character->model_flags |= CHARACTER_MODEL_CAN_FIRE;
                }
                config->flags_090 |= customiser->piece_sets[category][selection[category]].gameplay_flags;
            }
        }
        if ((customiser->piece_sets[1][selection[1]].availability_flags & CUSTOM_PIECE_NO_HAT) != 0) {
            config->flags_090 |= CHARACTER_RUNTIME_ALREADY_GOT_HAT;
        }
        config->weapon_model = customiser->piece_sets[2][selection[2]].weapon_model;
        config->blade_colour = (i8)LightSabre_ColourFromObj(config->weapon_model, NULL);
    }
    Customiser_SetNameAndIcon(customiser, -1);
}

void Customiser_GetActiveWeirdoIndex(i32 *index, i32 *count) {
    if (MissionMenuPacket.active_player[0] != 0) {
        if (MissionMenuPacket.active_player[1] == 0) {
            *index = 0;
            *count = 1;
            return;
        }
    } else if (MissionMenuPacket.active_player[1] != 0) {
        *index = 1;
        *count = 1;
        return;
    }
    *index = 0;
    *count = 2;
}

i32 Customiser_MenuAvailable(CUSTOMISER_CONFIG *customiser) {
    if (customiser == NULL || APICharacterLoaded(customiser->character_ids[0]) == NULL) {
        return 0;
    }
    if (customiser->animation_packets[0].packet.requested_animation != 190 &&
        customiser->animation_packets[0].packet.requested_animation != 99) {
        return 0;
    }
    if (APICharacterLoaded(customiser->character_ids[1]) == NULL) {
        return 0;
    }
    return customiser->animation_packets[1].packet.requested_animation == 190 ||
           customiser->animation_packets[1].packet.requested_animation == 99;
}

i32 Customise_GetToggleString(i32 index) {
    i32 mode = CustomiseMode[index];
    if (mode == 1) {
        return dat_00880890;
    }
    if (mode != 0) {
        do {
            mode = (mode + 1) % 3;
        } while (mode == 0);
    }
    return dat_00880780;
}

static i32 CustomiseMenuY[2] = {0};
static i32 CustomiseNameX[2] = {0};
GAMESAVE OldCustomiseGame = {0};
static i32 CustomiseNameY[2] = {0};

u8 rightrepeatcount[2] = {0};

/* 00424810..004252bc: PC control returns its quit decision to the wrapper. */
i32 CustomiserMenu_Update(void) {
    i32 quit = 0;
    i32 cancel_name = 0;
    i32 toggle = 0;
    CUSTOMISER_CONFIG *customiser = CharacterCustomiser;
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
        GAMEPAD_RUNTIME_INPUT *pad = (GAMEPAD_RUNTIME_INPUT *)&GamePadStorage[player];
        select[player] = 0;
        CustomiseNameBoardTMul[player] = CustomiseMode[player] == 1 ? 1.0f : 0.0f;
        CustomiseNameBoardMul[player] =
            SeekLinearF(CustomiseNameBoardMul[player], CustomiseNameBoardTMul[player], (f32)((f64)FRAMETIME * 4.0));
        CustomiseNameLetterBlipScale[player] = SeekLinearF(CustomiseNameLetterBlipScale[player], 1.0f, (f32)((f64)FRAMETIME * 5.0));
        if (MissionMenuPacket.active_player[player] != 0) {
            if ((pad->pressed & GAMEPAD_MENUSELECT) != 0) {
                select[player] = 1;
            } else if ((pad->pressed & GAMEPAD_MENUCANCEL) != 0) {
                cancel = 1;
            }
        }
    }
    if (cancel != 0 && CustomiseMode[0] != 1 && CustomiseMode[1] != 1) {
        if (GAMEDEMO != 0 && MissionMenuPacket.active_player[0] != 0 && MissionMenuPacket.active_player[1] != 0) {
            if ((((GAMEPAD_RUNTIME_INPUT *)&GamePadStorage[0])->pressed & GAMEPAD_MENUCANCEL) != 0) {
                MissionMenuPacket.selected[0] = MissionMenuPacket.selected[0] == 0;
            }
            if ((((GAMEPAD_RUNTIME_INPUT *)&GamePadStorage[1])->pressed & GAMEPAD_MENUCANCEL) != 0) {
                MissionMenuPacket.selected[1] = MissionMenuPacket.selected[1] == 0;
            }
            if (MissionMenuPacket.selected[0] != 0 && MissionMenuPacket.selected[1] != 0) {
                quit = 1;
            }
        } else {
            quit = 1;
        }
    } else if ((select[0] != 0 || select[1] != 0) && active_count == 1 && CustomiseMode[0] != 1 && CustomiseMode[1] != 1) {
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
            if (MissionMenuPacket.active_player[player] == 0) {
                continue;
            }
            held = ((GAMEPAD_RUNTIME_INPUT *)&GamePadStorage[player])->left_held | ((GAMEPAD_RUNTIME_INPUT *)&GamePadStorage[player])->held;
            pressed = ((GAMEPAD_RUNTIME_INPUT *)&GamePadStorage[player])->pressed |
                      ((GAMEPAD_RUNTIME_INPUT *)&GamePadStorage[player])->left_pressed;

            if (active_count == 1 && MissionMenuPacket.customise_other_player[player] != 0) {
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
                i32 category = CustomiseMenuY[side];
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
                MenuRepeat(&left_held, &left, &leftrepeattime[player], &leftrepeatcount[player], repeat_delay, FRAMETIME);
                MenuRepeat(&right_held, &right, &rightrepeattime[player], &rightrepeatcount[player], repeat_delay, FRAMETIME);
                if (CustomiseMode[side] == 1) {
                    i32 old_x = CustomiseNameX[side];
                    i32 old_y = CustomiseNameY[side];
                    if (left != 0) {
                        if (old_x > 0) {
                            Game.customiser_start[side].name[old_x] = CustomiseLetter[old_y];
                            CustomiseNameX[side] = old_x - 1;
                            CustomiseNameY[side] = Customise_FindLetter(Game.customiser_start[side].name[CustomiseNameX[side]]);
                        }
                    } else if (right != 0) {
                        i32 x;
                        NuStrLen(Game.customiser_start[side].name);
                        x = CustomiseNameX[side];
                        if (x < 14) {
                            Game.customiser_start[side].name[x] = CustomiseLetter[CustomiseNameY[side]];
                            CustomiseNameX[side] = x + 1;
                            CustomiseNameY[side] = Customise_FindLetter(Game.customiser_start[side].name[x + 1]);
                        }
                    } else if (up != 0) {
                        CustomiseNameY[side] = old_y + 1;
                        if (CustomiseNameY[side] == 38) {
                            CustomiseNameY[side] = 0;
                        }
                    } else if (down != 0) {
                        CustomiseNameY[side] = old_y - 1;
                        if (CustomiseNameY[side] == -1) {
                            CustomiseNameY[side] = 37;
                        }
                    }
                    if (CustomiseNameX[side] != old_x || CustomiseNameY[side] != old_y) {
                        GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                        CustomiseNameLetterBlipScale[side] = 1.5f;
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
                        next_piece = Customiser_NextPieceLeft(customiser, old_piece, piece_count, side, category);
                    } else if (right != 0) {
                        next_piece = Customiser_NextPieceRight(customiser, old_piece, piece_count, side, category);
                    }
                    *slot = (u16)next_piece;
                    CustomiseMenuY[side] = category;
                    if (next_piece != old_piece || category != old_category) {
                        GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                        CustomiseMenuTime[side] = 0.0f;
                    }
                    if (next_piece != old_piece) {
                        Customiser_SetNameAndIcon(customiser, side);
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
                    CustomiseNameY[side] = Customise_FindLetter(Game.customiser_start[side].name[CustomiseNameX[side]]);
                    CustomiseNameLetterBlipScale[side] = 1.0f;
                } else if (old_mode == 1) {
                    if (cancel_name == 0) {
                        i32 last = 14;
                        Game.customiser_start[side].name[CustomiseNameX[side]] = CustomiseLetter[CustomiseNameY[side]];
                        if (Game.customiser_start[side].name[last] == ' ') {
                            do {
                                --last;
                            } while (Game.customiser_start[side].name[last] == ' ');
                        }
                        if (last < 0) {
                            i16 text_id = side == 0 ? dat_00880894 : dat_00880898;
                            NuStrCpy(Game.customiser_start[side].name, TTab[text_id]);
                            FinishWeirdoNames(side);
                        } else {
                            Game.customiser_start[side].name[last + 1] = 0;
                        }
                        if (NuStringFilterBadWords((u8 *)Game.customiser_start[side].name, (u8 *)Game.customiser_start[side].name,
                                                   (u8 *)"Stranger") != 0) {
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
                PlaySfx("ToggleChar", position);
                NewBuzzFrames(((GAMEPAD_RUNTIME_INPUT *)&GamePadStorage[player])->bound_pad, 1, 0);
                for (category = 0; category < 9; ++category) {
                    if (customiser->piece_counts[category] > 0) {
                        i32 piece;
                        do {
                            piece = qrand() / (65535 / customiser->piece_counts[category] + 1);
                        } while (customiser->piece_available(&customiser->piece_sets[category][piece]) == 0);
                        Game.customiser_start[side].pieces[category] = (u16)piece;
                    }
                }
                Customiser_SetNameAndIcon(customiser, side);
                debris_position.x = 0.0f;
                debris_position.y = 0.21f;
                debris_position.z = 0.0f;
                angle = (i32)((f64)NuTrigTable[CustomiseTiltZ[side] >> 1] * 910.0);
                CustomiserMtxSetRotateZ(&matrix, angle);
                angle = (i32)((f64)NuTrigTable[CustomiseTiltX[side] >> 1] * 910.0);
                CustomiserMtxRotateX(&matrix, angle);
                angle = (i32)((f64)NuTrigTable[CustomiseRotY[side] >> 1] * 10922.0);
                CustomiserMtxRotateY(&matrix, angle);
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
        MissionMenuPacket.customise_other_player[active_index] = MissionMenuPacket.customise_other_player[active_index] == 0;
        GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
        object = Player[active_index];
        NewBuzzFrames(object->player.pad->bound_pad, 1, 0);
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

static f32 CustomiseNameLetterScale = 0.5f;
static f32 CustomiseNameGap = 0.0525f;
static f32 CustomiseNameSideGap = 0.06f;
static f32 CustomiseNameArrowScale = 0.6f;
static f32 CustomiseNameArrowOffset = 0.1f;
static f32 CustomiseNameBracketOffsetX = 0.01f;
static f32 CustomiseNameBracketOffsetY = 0.0075f;

static void CustomiserDrawNameEditor(CUSTOMISER_CONFIG *customiser) {
    i32 side;
    i32 active_count;
    i32 index;
    char name[64];
    if (customiser == NULL || CustomiseNameAlpha <= 0.0f) {
        return;
    }
    Customiser_GetActiveWeirdoIndex(&side, &active_count);
    if (active_count == 1 && MissionMenuPacket.customise_other_player[side] != 0) {
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
            f32 gap = CustomiseNameGap;
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
            x = (f32)(CustomiseScreenPos[side].x - gap * 7.0);
            if (active_count == 2) {
                x += side == 0 ? -CustomiseNameSideGap : CustomiseNameSideGap;
            }
            for (column = 0; column < 15; ++column) {
                f32 scale;
                i32 letter_red;
                i32 letter_green;
                i32 letter_blue;
                if (column == CustomiseNameX[side]) {
                    const i32 colour = menu_flash == 0 ? 255 : 191;
                    letter[0] = CustomiseLetter[CustomiseNameY[side]];
                    Text3D((char *)InitAsciiUp, x, (f32)(CustomiseNameArrowOffset - (f64)0.65f), 1.0f, CustomiseNameArrowScale,
                           CustomiseNameArrowScale, CustomiseNameArrowScale, 0, colour, colour, colour);
                    Text3D((char *)TagAsciiDown, x, (f32)(-(f64)0.65f - CustomiseNameArrowOffset), 1.0f, CustomiseNameArrowScale,
                           CustomiseNameArrowScale, CustomiseNameArrowScale, 0, colour, colour, colour);
                    scale = CustomiseNameLetterBlipScale[side];
                    letter_red = letter_blue = 255;
                    letter_green = menu_flash == 0 ? 255 : 0;
                } else {
                    letter[0] = Game.customiser_start[side].name[column];
                    if (column > CustomiseNameX[side] && column >= length && letter[0] == ' ') {
                        letter[0] = '.';
                    }
                    scale = 1.0f;
                    letter_red = red;
                    letter_green = green;
                    letter_blue = blue;
                }
                Text3D(letter, x, -0.65f, 1.0f, scale * CustomiseNameLetterScale, (f32)(scale * 0.5), 0.5f, 0, letter_red, letter_green,
                       letter_blue);
                if (column == 0) {
                    Text3D("[", (x - gap) + CustomiseNameBracketOffsetX, (f32)(CustomiseNameBracketOffsetY - (f64)0.65f), 1.0f, 0.5f, 0.6f,
                           0.5f, 0, red, green, blue);
                } else if (column == 14) {
                    Text3D("]", (x + gap) - CustomiseNameBracketOffsetX, (f32)(CustomiseNameBracketOffsetY - (f64)0.65f), 1.0f, 0.5f, 0.6f,
                           0.5f, 0, red, green, blue);
                }
                x += gap;
            }
        } else {
            GameObj_GetName(customiser->character_ids[side], NULL, name);
            SmartTextEx(name, CustomiseScreenPos[side].x, -0.65f, 1.0f, 0.5f, 0.5f, 0.5f, 0, red, green, blue, 0.75f, 2, NULL, 0,
                        (i32)(CustomiseNameAlpha * 128.0));
        }
        x = CustomiseScreenPos[side].x;
        alpha = (1.0f - CustomiseNameBoardMul[side]) * CustomiseNameAlpha;
        DrawPanel3DObject(x, -0.48f, 1.0f, MissionPlayerIconSize, MissionPlayerIconSize, MissionPlayerIconSize, 0, 0, 0,
                          &WORLD->level_objects[167].special, 0, alpha);
        icon = Customiser_GetIcon(customiser, &Game.customiser_start[side], index);
        if (icon != -1) {
            if (icon != PCArcadeUnpairedIconModel) {
                ++icon;
            }
            DrawPanel3DObject(x, -0.48f, 1.0f, MissionPlayerIconSize, MissionPlayerIconSize, MissionPlayerIconSize, 0, 0, 0,
                              &WORLD->level_objects[icon].special, 0, alpha);
        }
    }
}

static NUVEC CustomiseNoHatPos[2] = {0};
static f32 CustomiseHeadOffset = 0.21f;
char CustomiseLetter[39] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ- 0123456789";

void Customiser_Update(CUSTOMISER_CONFIG *customiser, WORLDINFO *world) {
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
    ADD_GAME_MESSAGE message;
    if (customiser == NULL) {
        return;
    }
    Customiser_GetActiveWeirdoIndex(&active_side, &active_count);
    preview_side = -1;
    if (active_count == 1) {
        preview_side = MissionMenuPacket.customise_other_player[active_side] != 0 ? active_side == 0 : active_side;
    }
    message = AddGameMsg_Default;
    colour = menu_flash == 0 ? 255 : 191;
    for (side = 0; side < 2; ++side) {
        const i32 control_side = active_count == 2                         ? side
                                 : MissionMenuPacket.active_player[0] != 0 ? 0
                                 : MissionMenuPacket.active_player[1] != 0 ? 1
                                                                           : -1;
        u32 torso_flags;
        CHARACTER_MODEL_LAYERS *model;
        ANIMPACKET *packet;
        i32 saber_colour;
        i32 intro;
        u16 angle;
        f32 no_hat;
        CustomiseRotY[side] += rotation_step;
        CustomiseTiltX[side] += tilt_x_step;
        CustomiseTiltZ[side] += tilt_z_step;
        CustomiseBob[side] += bob_step;
        UpdateCustomPieceAnim(&CustomiseHeadAnim[side], 0xd556, 0x2aaa);
        UpdateCustomPieceAnim(&CustomiseArmsAnim[side], 0x1555, 0x4000);
        UpdateCustomPieceAnim(&CustomiseLegsAnim[side], 0xe000, 0x4000);
        torso_flags = customiser->piece_sets[1][Game.customiser_start[side].pieces[1]].availability_flags;
        if ((torso_flags & 1) != 0) {
            if ((torso_flags & 4) != 0) {
                if ((customiser->piece_sets[0][Game.customiser_start[side].pieces[0]].availability_flags & CUSTOM_PIECE_BIG_HAT) != 0) {
                    no_hat = CustomiseHeadOffset;
                } else {
                    no_hat = (f64)CustomiseHeadOffset * (f64)0.8f;
                }
            } else {
                if ((customiser->piece_sets[0][Game.customiser_start[side].pieces[0]].availability_flags & CUSTOM_PIECE_BIG_HAT) != 0) {
                    no_hat = (f64)CustomiseHeadOffset * (f64)0.8f;
                } else {
                    no_hat = (f64)CustomiseHeadOffset * (f64)0.6f;
                }
            }
        } else {
            no_hat = 0.0f;
        }

        CustomiseNoHatPos[side].y = SeekValF(CustomiseNoHatPos[side].y, no_hat, 10.0f);
        CustomiseMenuTime[side] += FRAMETIME;
        model = APICharacterLoaded(customiser->character_ids[side]);
        if (model == NULL) {
            continue;
        }
        saber_colour = LightSabre_ColourFromObj(customiser->piece_sets[2][Game.customiser_start[side].pieces[2]].weapon_model, NULL);
        if (customiser->animation_state[side] > 0) {
            --customiser->animation_state[side];
        }
        packet = &customiser->animation_packets[side].packet;
        if (customiser->animation_active[side] == 0 && model->model_data_b[intro = saber_colour == -1 ? 0x62 : 0x61] != NULL) {
            if (customiser->animation_values[side] == 0.0f) {
                customiser->animation_values[side] = AnimDuration(model->model_id, intro, 0.0f, 0.0f, 1);
            } else {
                customiser->animation_values[side] -= FRAMETIME;
                if (customiser->animation_values[side] <= 0.0f) {
                    customiser->animation_active[side] = 1;
                }
            }
        } else {
            customiser->animation_active[side] = 1;
        }
        packet->previous_animation = packet->animation_index;
        if (saber_colour != -1) {
            packet->requested_animation = customiser->animation_active[side] == 0 ? 0x61 : 0xbe;
        } else {
            packet->requested_animation = customiser->animation_active[side] == 0 ? 0x62 : 0x63;
        }
        UpdateAnimPacket(model, packet, (f64)FRAMETIME * 30.0, 0.0f, FRAMETIME, 0.0f);
        angle = CustomiseYRot[side] + 0xc000;
        for (category = 0; category < 9; ++category) {
            i32 locator;
            CUSTOM_RESOURCE_UPDATE *resources;
            i32 material;
            if (category == CustomiseMenuY[side] && GetMenuID() == MENU_CUSTOMISER && customiser_quit == 0 &&
                (active_count == 2 || side == preview_side) && control_side != -1 &&
                (locator = customiser->locator_indices[category]) != -1 && model->points_of_interest[locator] != NULL) {
                NUVEC position;
                position = *(NUVEC *)&customiser->joint_matrices[side][locator].m30;
                message.text = InitAsciiLeft;
                message.slot_flags = 2;
                if (customiser->locator_x_offsets[category] != 0.0f) {
                    position.x += NuTrigTable[(angle >> 1) & 0x7fff] * customiser->locator_x_offsets[category];
                    position.z += NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * customiser->locator_x_offsets[category];
                }
                position.y = customiser->locator_y_offsets[category] + position.y;

                message.red = colour;
                message.green = colour;
                message.blue = colour;
                message.position = &position;
                message.scale = 1.5f;
                message.alpha = 0x60;
                message.flags = GAMEMSG_WORLD_POSITION | GAMEMSG_FOLLOW | GAMEMSG_WORLD_SCALE | GAMEMSG_ONE_FRAME;
                AddGameMsg(&message);
            }
            resources = (CUSTOM_RESOURCE_UPDATE *)world->customiser_resources[category];
            if (resources == NULL || customiser->piece_counts[category] <= 0) {
                continue;
            }
            if (customiser->categories[category]->uses_special != 0 || customiser->categories[category]->material_tag == -1) {
                continue;
            }
            resources += Game.customiser_start[side].pieces[category];
            if (resources->texture_id == 0) {
                continue;
            }
            for (material = 0; material < ((CUSTOM_HIERARCHY_UPDATE *)model->hierarchy)->material_count; ++material) {
                if (((CUSTOM_MATERIAL_UPDATE *)((CUSTOM_HIERARCHY_UPDATE *)model->hierarchy)->materials[material])->material_tag ==
                    customiser->categories[category]->material_tag) {
                    ((CUSTOM_HIERARCHY_UPDATE *)model->hierarchy)->materials[material]->texture_id = (u16)resources->texture_id;
                    NuMtlUpdate(((CUSTOM_HIERARCHY_UPDATE *)model->hierarchy)->materials[material]);
                }
            }
        }
    }
    {
        const f32 target = GetMenuID() == MENU_CUSTOMISER && customiser_quit == 0 ? 1.0f : 0.0f;
        CustomiseNameAlpha = SeekLinearF(CustomiseNameAlpha, target, 1.5 * FRAMETIME);
    }
}

/* 00425fa0 / 1087 */
void Customiser_Draw3D(CUSTOMISER_CONFIG *input) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    CUSTOMISER_CONFIG *customiser = input;
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
        preview_side = MissionMenuPacket.customise_other_player[active_side] != 0 ? active_side == 0 : active_side;
    }
    for (side = 0; side < 2; ++side) {
        i32 character_id = customiser->character_ids[side];
        CHARACTER_MODEL_LAYERS *model = APICharacterLoaded(character_id);
        PLAYERCHARACTERCONFIG *character;
        i32 helmet;
        i32 selected;
        i32 category;
        RTL_DATA light_data;
        NUMTX matrix;
        u32 mask;
        if (model == NULL || customiser->animation_state[side] > 0) {
            continue;
        }
        character = &GCDataList[character_id];
        helmet = character->helmet_locator;
        if (helmet != -1 && model->points_of_interest[helmet] == NULL) {
            helmet = -1;
        }
        selected = CustomiseMenuY[side];
        rtlResetEx(&light_data, 1);
        rtlApplySetScale(world->rtl_set, &light_data, &CustomisePos[side], NULL, -1, 1.0f);
        SetLights_RTLDATA(&light_data, 1.0f);
        mask = CustomiserPreviewLayerMask(customiser, Game.customiser_start[side].pieces, selected, character);
        NuMtxSetRotationY(&matrix, CustomiseYRot[side] + 0x8000);
        NuMtxTranslate(&matrix, &CustomisePos[side]);
        if (GameDrawCharacterModel(model, &customiser->animation_packets[side].packet, &matrix, NULL, NULL,
                                   customiser->joint_matrices[side], NULL, mask) == 0) {
            continue;
        }
        if (helmet != -1) {
            for (category = 0; category < 9; ++category) {
                CUSTOM_ACCESSORY_RESOURCE *resources = (CUSTOM_ACCESSORY_RESOURCE *)world->customiser_resources[category];
                CUSTOM_ACCESSORY_RESOURCE *resource;
                CUSTOM_PIECE_CONFIG *piece;
                if (resources == NULL || customiser->piece_counts[category] <= 0 || customiser->categories[category]->uses_special == 0) {
                    continue;
                }
                resource = &resources[Game.customiser_start[side].pieces[category]];
                if (NuSpecialExistsFn(&resource->special) == 0) {
                    continue;
                }
                piece = &customiser->piece_sets[category][Game.customiser_start[side].pieces[category]];
                if (GetMenuID() == MENU_CUSTOMISER && customiser_quit == 0 && category == selected &&
                    (active_count == 2 || side == preview_side)) {
                    NuFmod(CustomiseMenuTime[side], 0.3f);
                } else if (category == 0 && (piece->availability_flags & CUSTOM_PIECE_HAT_OFF) != 0) {
                    continue;
                }
                matrix = customiser->joint_matrices[side][helmet];
                if (category == 0) {
                    NuMtxPreTranslate(&matrix, &CustomiseNoHatPos[side]);
                }
                NuSpecialDrawAt(&resource->special, &matrix);
            }
        }
        if (customiser->piece_counts[2] > 0) {
            i32 hand = character->weapon_joints[0];
            CUSTOM_PIECE_CONFIG *weapon;
            LEVEL_OBJECT_RUNTIME *object;
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
                nuhspecial_s *special;
                matrix = customiser->joint_matrices[side][hand];
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

void MenuUpdateCustomiser(MENU *menu) {
    i32 quit;
    if (customiser_save_done != 0) {
        CustomiserMenu_End();
        customiser_save_done = 0;
        return;
    }
    quit = CustomiserMenu_Update();
    customiser_changed =
        memcmp(Game.customiser_start, OldCustomiseGame.customiser_start, sizeof(Game.customiser_start)) != 0 ||
        memcmp(Game.customiser_start[0].name, OldCustomiseGame.customiser_start[0].name, sizeof(Game.customiser_start[0].name)) != 0 ||
        Game.customiser_start[0].use_saved_name != OldCustomiseGame.customiser_start[0].use_saved_name ||
        Game.customiser_start[1].use_saved_name != OldCustomiseGame.customiser_start[1].use_saved_name;
    if (quit == 0) {
        return;
    }
    PlaySfx("menuBack", NULL);
    customiser_quit = 1;
    if (GAMEDEMO == 0 && customiser_changed != 0) {
        Hub_ClearStats();
        Text_FillInExtendedSaveInfo();
        if (TriggerAutoSave() != 0) {
            CustomiserMenu_End();
            return;
        }
        customiser_save_done = 1;
        NewMenu(MENU_SAVE, -1, -1);
        return;
    }
    CustomiserMenu_End();
}

void MenuDrawCustomiser(MENU *menu) {
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
    if (MenuStopDraw != 0 || customiser_quit != 0 || CharacterCustomiser == NULL) {
        return;
    }
    CustomiserDrawNameEditor(CharacterCustomiser);
    Customiser_GetActiveWeirdoIndex(&active_index, &active_count);
    alpha = MissionMenuPacket.active_player[0] != 0 ? 1.0f : MissionDropInAlpha;
    DrawCharIcon(MissionMenuPacket.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 0xa6, alpha, alpha, 1, NULL);
    alpha = MissionMenuPacket.active_player[1] != 0 ? 1.0f : MissionDropInAlpha;
    DrawCharIcon(MissionMenuPacket.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 0xa5, alpha, alpha, 1, NULL);
    select[0] = select[1] = active_count == 1 ? dat_00880750 : -1;
    action[0] = action[1] = dat_00880758;
    toggle[0] = toggle[1] = -1;
    if (GAMEDEMO != 0) {
        if (MissionMenuPacket.active_player[0] != 0 && MissionMenuPacket.active_player[1] != 0) {
            if (MissionMenuPacket.selected[0] != 0) {
                select[0] = action[0] = -1;
                cancel[0] = dat_00880890;
            } else {
                cancel[0] = dat_00880760;
            }
            if (MissionMenuPacket.selected[1] != 0) {
                select[1] = action[1] = -1;
                cancel[1] = dat_00880890;
            } else {
                cancel[1] = dat_00880760;
            }
        } else {
            cancel[0] = cancel[1] = dat_00880858;
        }
    } else {
        for (player = 0; player < 2; ++player) {
            if (active_count == 1 && MissionMenuPacket.customise_other_player[player] != 0) {
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
                    cancel[player] = dat_00880754;
                } else {
                    cancel[player] = dat_00880844;
                }
            }
        }
    }
    DrawPlayerIconPrompts(MissionMenuPacket.active_player[0], select[0], 1.0f, action[0], cancel[0], toggle[0], dat_00880eac,
                          MissionMenuPacket.active_player[1], select[1], 1.0f, action[1], cancel[1], toggle[1], dat_00880eac);
    x = -ICONX;
    colour = menu_flash == 0 ? 255 : 191;
    for (player = 0; player < 2; ++player) {
        if (active_count == 1 && MissionMenuPacket.customise_other_player[player] != 0) {
            side = player == 0;
        } else {
            side = player;
        }
        if (MissionMenuPacket.active_player[player] != 0 && CustomiseMode[side] != 1) {
            y = (f32)(STATSPOSY - 0.22499999403953552);
            Text3D("<", (f32)(x - 0.05000000074505806 - (BlipL[player] > 0.0f ? 0.015000001f : 0.0f)), y, 1.0f, 0.5f, 0.5f, 0.5f, 0, colour,
                   colour, colour);
            Text3D(">", (f32)(x + 0.05000000074505806 + (BlipR[player] > 0.0f ? 0.015000001f : 0.0f)), y, 1.0f, 0.5f, 0.5f, 0.5f, 0, colour,
                   colour, colour);
            Text3DEx((char *)InitAsciiUp, x, (f32)(y - -0.07000000029802322 - (BlipU[player] > 0.0f ? -0.021000002f : 0.0f)), 1.0f, 0.5f,
                     0.5f, 0.5f, 0, colour, colour, colour, CustomiseMenuY[side] > 0 ? 128 : 32);
            Text3DEx((char *)TagAsciiDown, x, (f32)(y - 0.07000000029802322 + (BlipD[player] > 0.0f ? -0.021000002f : 0.0f)), 1.0f, 0.5f,
                     0.5f, 0.5f, 0, colour, colour, colour, CustomiseMenuY[side] < 8 ? 128 : 32);
            SmartTextEx(TTab[dat_00880888], x, (f32)(STATSPOSY - 0.375), 1.0f, 0.3f, 0.3f, 0.3f, 0, colour, colour, colour, 0.15f, 1, NULL,
                        0, 128);
        }
        x += ICONX + ICONX;
    }
}
