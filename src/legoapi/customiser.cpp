#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/characters/core/customiser.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/misc/buffers.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nuptrblock.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufilepak.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>
#include <stdio.h>

static i32 CustomiserPieceAvailableDefaultPrivate(CUSTOM_PIECE_CONFIG *piece) {
    if ((piece->availability_flags & (CUSTOM_PIECE_100_PERCENT_SIDE_1 | CUSTOM_PIECE_100_PERCENT_SIDE_2)) != 0 &&
        Game_100PercentComplete() == 0) {
        return 0;
    }
    return 1;
}

static i32 CustomiserFindSetFromNamePrivate(char *name) {
    if (NuStrICmp(name, "hathair") == 0) {
        return 0;
    }
    if (NuStrICmp(name, "head") == 0) {
        return 1;
    }
    if (NuStrICmp(name, "cape") == 0) {
        return 5;
    }
    if (NuStrICmp(name, "body") == 0) {
        return 6;
    }
    if (NuStrICmp(name, "arms") == 0) {
        return 3;
    }
    if (NuStrICmp(name, "hands") == 0) {
        return 4;
    }
    if (NuStrICmp(name, "weapon") == 0) {
        return 2;
    }
    if (NuStrICmp(name, "underpants") == 0) {
        return 7;
    }
    if (NuStrICmp(name, "legs") == 0) {
        return 8;
    }
    return -1;
}

void Customiser_SetAnimsToLoad(CUSTOMISER_CONFIG *customiser, i32 enabled) {
    i32 character;
    if (customiser == NULL) {
        return;
    }
    for (character = 0; character < 2; ++character) {
        CHARACTERANIM_CONFIG *animation = CDataList[customiser->character_ids[character]].animations;
        if (animation == NULL) {
            continue;
        }
        for (; animation->name != NULL; ++animation) {
            i32 i;
            if (enabled == 0) {
                animation->flags &= ~CHARANIM_NO_LOAD;
                continue;
            }
            animation->flags |= CHARANIM_NO_LOAD;
            if (customiser->animation_ids_to_load == NULL) {
                continue;
            }
            for (i = 0; (i16)customiser->animation_ids_to_load[i] != -1; ++i) {
                if (customiser->animation_ids_to_load[i] == (u16)animation->action_id) {
                    animation->flags &= ~CHARANIM_NO_LOAD;
                    break;
                }
            }
        }
    }
}

/* Whether the hub has the resource of a piece to show (category 2 and other areas always do). */
static i32 Customiser_ResourceAvailable(CUSTOMISER_CONFIG *customiser, i32 index, i32 category) {
    WORLDINFO *world;
    CUSTOM_ACCESSORY_RESOURCE *resources;
    CUSTOM_ACCESSORY_RESOURCE *resource;
    if (category != 2) {
        world = WorldInfo_CurrentlyActive();
        if (world != NULL && HUB_ADATA != NULL && world->area == HUB_ADATA) {
            resources = world->customiser_resources[category];
            if (resources == NULL) {
                return 0;
            }
            resource = &resources[index];
            if (customiser->categories[category]->uses_special != 0) {
                if (NuSpecialExistsFn(&resource->special) == 0) {
                    return 0;
                }
            } else if (resource->texture_id == 0) {
                return 0;
            }
        }
    }
    return 1;
}

/* Includes a separate NULL return. */
i32 Customiser_NextPieceLeft(CUSTOMISER_CONFIG *customiser, i32 index, i32 count, i32 unused, i32 category) {
    i32 attempts;
    if (customiser != NULL) {
        attempts = 0;
        while (attempts++ < count) {
            --index;
            if (index < 0) {
                index += count;
            }
            if (Customiser_ResourceAvailable(customiser, index, category) == 0) {
                continue;
            }
            if (((customiser->piece_sets[category][index].availability_flags &
                  (CUSTOM_PIECE_100_PERCENT_SIDE_1 | CUSTOM_PIECE_100_PERCENT_SIDE_2)) == 0 ||
                 Game_100PercentComplete() != 0) &&
                customiser->piece_available(&customiser->piece_sets[category][index]) != 0) {
                break;
            }
        }
    }
    return index;
}

i32 Customiser_NextPieceRight(CUSTOMISER_CONFIG *customiser, i32 index, i32 count, i32 unused, i32 category) {
    i32 attempts;
    if (customiser == NULL) {
        return index;
    }
    for (attempts = 0; attempts < count; ++attempts) {
        ++index;
        if (index >= count) {
            index -= count;
        }
        if (category != 2) {
            WORLDINFO *world = WorldInfo_CurrentlyActive();
            if (world != NULL && HUB_ADATA != NULL && world->area == HUB_ADATA) {
                CUSTOM_ACCESSORY_RESOURCE *resources = world->customiser_resources[category];
                if (resources == NULL) {
                    continue;
                }
                if (customiser->categories[category]->uses_special != 0) {
                    if (NuSpecialExistsFn(&resources[index].special) == 0) {
                        continue;
                    }
                } else if (resources[index].texture_id == 0) {
                    continue;
                }
            }
        }
        if (((customiser->piece_sets[category][index].availability_flags &
              (CUSTOM_PIECE_100_PERCENT_SIDE_1 | CUSTOM_PIECE_100_PERCENT_SIDE_2)) == 0 ||
             Game_100PercentComplete() != 0) &&
            customiser->piece_available(&customiser->piece_sets[category][index]) != 0) {
            break;
        }
    }
    return index;
}

CUSTOM_ACCESSORY_RESOURCE KillPartsAccessories[2][9] = {0};
i32 Customiser_AccessoriesLoaded = 0;
RTL_DATA KillPartsZeroLighting = {0};

void Customiser_LoadAccessories(CUSTOMISER_CONFIG *customiser, APICHARACTERMODELLIST *models) {
    i32 side;
    i32 category;
    i32 material_index;
    i32 id;
    CHARACTER_MODEL_LAYERS *model;
    CUSTOM_ACCESSORY_RESOURCE *resource;
    char piece_name[0x80];
    char path[0x80];
    if (customiser == NULL) {
        return;
    }
    Customiser_AccessoriesLoaded = 1;
    for (; (id = models->model_id) != -1; ++models) {
        if (id == customiser->character_ids[0]) {
            side = 0;
        } else if (id == customiser->character_ids[1]) {
            side = 1;
        } else {
            continue;
        }
        model = &apicharsys->models[apicharsys->playermodelids[id]];
        resource = KillPartsAccessories[side];
        for (category = 0; category < 9; ++category, ++resource) {
            u16 piece_index;
            memset(resource, 0, sizeof(*resource));
            if (customiser->categories[category]->name == NULL || customiser->piece_counts[category] <= 0) {
                continue;
            }
            resource->character_model = model;
            piece_index = customiser->save[side].pieces[category];
            NuStrCpy(piece_name, customiser->piece_sets[category][piece_index].name);
            NuStrCpy(path, "chars\\weirdo\\");
            NuStrCat(path, customiser->categories[category]->name);
            NuStrCat(path, "\\");
            NuStrCat(path, piece_name);
            if (customiser->categories[category]->uses_special != 0) {
                NuStrCat(path, ".gsc");
                resource->scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, path);
                if (resource->scene != NULL) {
                    NuSpecialFind(resource->scene, &resource->special, piece_name, 1);
                }
            } else {
                if (customiser->categories[category]->material_tag == -1) {
                    continue;
                }
                NuStrCat(path, ".pnt");
                resource->texture_id = NuTexRead(path, &characterbuffer_ptr, characterbuffer_end);
                if (resource->texture_id == 0) {
                    continue;
                }
                for (material_index = 0; material_index < model->hierarchy->material_count; ++material_index) {
                    NUMTL *material = model->hierarchy->materials[material_index];
                    if (material->override_slot != customiser->categories[category]->material_tag) {
                        continue;
                    }
                    resource->material_index = material_index;
                    resource->original_texture_id = model->hierarchy->materials[material_index]->texture_id;
                    model->hierarchy->materials[material_index]->texture_id = (i16)resource->texture_id;
                    NuMtlUpdate(model->hierarchy->materials[material_index]);
                }
            }
        }
    }
}

void CustomiserDrawAccessories(CUSTOMISER_CONFIG *customiser, GAMEOBJECT *object, NUMTX *matrices) {
    i32 joint = object->character_data->game_character->helmet_locator;
    i32 side;
    i32 category;
    const u16 *pieces;
    CUSTOM_ACCESSORY_RESOURCE *resource;
    NUMTX matrix;
    NUMTX reflection;
    if (customiser == NULL || joint == -1 || object->character_model->points_of_interest[joint] == NULL) {
        return;
    }
    side = object->id != (u16)customiser->character_ids[0];
    resource = KillPartsAccessories[side];
    for (category = 0; category < 9; ++category, ++resource) {
        if (customiser->piece_counts[category] <= 0 || category == 2) {
            continue;
        }
        if (category == 0) {
            pieces = customiser->save[side].pieces;
            if ((customiser->piece_sets[0][pieces[0]].availability_flags & CUSTOM_PIECE_HAT_OFF) != 0 ||
                (customiser->piece_sets[1][pieces[1]].availability_flags & CUSTOM_PIECE_NO_HAT) != 0 || object->helmet != HELMET_NONE) {
                continue;
            }
        }
        if (NuSpecialExistsFn(&resource->special) == 0) {
            continue;
        }
        if (matrices != NULL) {
            matrix = matrices[joint];
        } else {
            matrix = object->player.locator_matrices[joint];
        }
        NuSpecialDrawAt(&resource->special, &matrix);
        if (object->reflection_active != 0 && MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                                               WORLD->current_level->reflection_y, &reflection) != 0) {
            NuRndrStartReflectionRender(0);
            NuSpecialDrawAt(&resource->special, &reflection);
            NuRndrEndReflectionRender();
        }
    }
}

void Customiser_AddPartAccessories(CUSTOMISER_CONFIG *customiser, GAMEOBJECT *object, i32 animation, i32 mode, f32 scale) {
    i32 side;
    CUSTOM_ACCESSORY_RESOURCE *resource;
    i32 category;
    i32 joint;
    joint = object->character_data->game_character->helmet_locator;
    if (customiser == NULL || joint == -1 || object->character_model->points_of_interest[joint] == NULL) {
        return;
    }
    side = ((i16)object->id) != customiser->character_ids[0];
    resource = KillPartsAccessories[side];
    for (category = 0; category < 9; ++category, ++resource) {
        nuhspecial_s *special;
        NUVEC momentum;
        ADDPART params;
        if (category == 2) {
            continue;
        }
        if (category == 0 &&
            ((customiser->piece_sets[1][customiser->save[side].pieces[1]].availability_flags & CUSTOM_PIECE_NO_HAT) != 0 ||
             (customiser->piece_sets[0][customiser->save[side].pieces[0]].availability_flags & CUSTOM_PIECE_HAT_OFF) != 0)) {
            continue;
        }
        special = &resource->special;
        if (!NuSpecialExistsFn(special)) {
            continue;
        }
        SetKillPartMom(&momentum);
        momentum.y += scale;
        if (animation != -1) {
            momentum.y += 1.0f;
        }
        params = Default_ADDPART;
        params.start_radius = 0.1f;
        params.end_radius = 0.1f;
        params.gravity = -5.0f;
        params.matrix = &object->player.locator_matrices[joint];
        params.velocity = &momentum;
        params.special = special;
        params.lighting_data = Cheats_CheckFlags(CHEAT_ZERO_LIGHTING) ? &KillPartsZeroLighting : &object->lighting_packet;
        params.time_step = FRAMETIME;
        params.flags = mode == 0 ? 0x480 : 0x90;
        params.stop_fn = PartStop_Flickerer;
        params.draw_fn = PartDraw_Flickerer;
        params.impact_fn = PartImpact_Brick;
        AddPart(&params);
    }
}

void Customiser_DumpAccessories(void *input) {
    CUSTOMISER_CONFIG *customiser = (CUSTOMISER_CONFIG *)input;
    i32 side;
    i32 category;
    CUSTOM_ACCESSORY_RESOURCE *resource;
    if (customiser == NULL || Customiser_AccessoriesLoaded != 1) {
        return;
    }
    for (side = 0; side < 2; ++side) {
        if (apicharsys->playermodelids[customiser->character_ids[side]] == -1) {
            continue;
        }
        resource = KillPartsAccessories[side];
        for (category = 0; category < 9; ++category, ++resource) {
            if (customiser->categories[category]->name == NULL) {
                continue;
            }
            if (resource->scene != NULL) {
                NuGScnRemove(resource->scene);
                resource->scene = NULL;
            } else if (resource->texture_id != 0) {
                i32 material_index = resource->material_index;
                resource->character_model->hierarchy->materials[material_index]->texture_id = (i16)resource->original_texture_id;
                NuMtlUpdate(resource->character_model->hierarchy->materials[material_index]);

                NuTexDestroy(resource->texture_id);
            }
        }
    }
}

static i32 CustomiserUseBigScenes = 1;

/* Ignored extra arguments remain explicit calls. */
void Customiser_LoadAll(CUSTOMISER_CONFIG *customiser, WORLDINFO *world) {
    char converted_path[0x80];
    char piece_name[0x80];
    char path[0x80];
    void *texture_pack;
    i32 category;
    i32 piece;
    if (customiser == NULL) {
        return;
    }
    Customiser_AccessoriesLoaded = 2;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    for (category = 0; category < 9; ++category) {
        if (customiser->piece_counts[category] > 0 && customiser->categories[category]->name != NULL) {
            world->customiser_resources[category] = (CUSTOM_ACCESSORY_RESOURCE *)world->giz_buffer.void_ptr;
            memset(world->giz_buffer.void_ptr, 0, customiser->piece_counts[category] * sizeof(CUSTOM_ACCESSORY_RESOURCE));
            world->giz_buffer.addr += customiser->piece_counts[category] * sizeof(CUSTOM_ACCESSORY_RESOURCE);
        } else {
            world->customiser_resources[category] = NULL;
        }
    }
    texture_pack = CHARPAK != 0 ? NuFilePakLoad("chars\\weirdo\\all_textures.fpk", &world->giz_buffer, world->buffer_end, 0x20) : NULL;
    for (category = 0; category < 9; ++category) {
        CUSTOM_ACCESSORY_RESOURCE *resources = world->customiser_resources[category];
        world->customiser_shared_scenes[category] = NULL;
        if (resources == NULL) {
            continue;
        }
        if (CustomiserUseBigScenes != 0 && customiser->categories[category]->uses_special != 0 &&
            customiser->categories[category]->scene_file != NULL) {
            world->customiser_shared_scenes[category] =
                NuGScnRead(&world->giz_buffer, world->buffer_end, customiser->categories[category]->scene_file);
            CustomiserLoadTrace(customiser->categories[category]->scene_file);
        }
        for (piece = 0; piece < customiser->piece_counts[category]; ++piece) {
            CUSTOM_ACCESSORY_RESOURCE *resource = &resources[piece];
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
                    CustomiserLoadTrace(path);
                }
                if (resource->scene != NULL) {
                    NuSpecialFind(resource->scene, &resource->special, piece_name, 1);
                }
            } else if (customiser->categories[category]->material_tag != -1) {
                NuStrCat(path, ".pnt");
                if (texture_pack != NULL) {
                    i32 item;
                    i32 size;
                    void *data;
                    ((i32(*)(char *, char *, i32))NuFileExtConvert)(converted_path, path, 0x80);
                    item = NuFilePakGetItem(texture_pack, converted_path);
                    if (item != -1 && NuFilePakGetItemInfo(texture_pack, item, &data, &size) != 0) {
                        data = NuPtrBlockFix(data);
                        resource->texture_id = NuTexCreateNative((NUTEXDIM *)data);
                        CustomiserLoadTrace(path);
                    }
                }
                if (resource->texture_id == 0) {
                    resource->texture_id = NuTexRead(path, &world->giz_buffer, world->buffer_end);
                    CustomiserLoadTrace(path);
                }
            }
        }
    }
}

void Customiser_DumpAll(void *input, WORLDINFO *world) {
    CUSTOMISER_CONFIG *customiser = (CUSTOMISER_CONFIG *)input;
    i32 category;
    i32 piece;
    if (customiser == NULL) {
        return;
    }
    for (category = 0; category < 9; ++category) {
        CUSTOM_ACCESSORY_RESOURCE *resources = world->customiser_resources[category];
        if (resources == NULL) {
            continue;
        }
        for (piece = 0; piece < customiser->piece_counts[category]; ++piece) {
            CUSTOM_ACCESSORY_RESOURCE *resource = &resources[piece];
            if (resource->scene != NULL) {
                if (resource->scene != world->customiser_shared_scenes[category]) {
                    NuGScnRemove(resource->scene);
                }
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

void Customiser_ResetModelTextureIDs(CUSTOMISER_CONFIG *customiser) {
    i32 side;
    i32 category;
    if (customiser == NULL) {
        return;
    }
    for (side = 0; side < 2; ++side) {
        for (category = 0; category < 9; ++category) {
            customiser->model_texture_ids[side * 9 + category] = 0;
        }
    }
}

void Customiser_SaveModelTextureIDs(void *input, CHARACTER_MODEL_LAYERS *model) {
    CUSTOMISER_CONFIG *customiser = (CUSTOMISER_CONFIG *)input;
    i32 side;
    i32 category;
    i32 material;
    if (customiser == NULL || model == NULL) {
        return;
    }
    for (side = 0; side < 2; ++side) {
        if (model->model_id != customiser->character_ids[side]) {
            continue;
        }
        for (category = 0; category < 9; ++category) {
            for (material = 0; material < model->hierarchy->material_count; ++material) {
                NUMTL *entry = model->hierarchy->materials[material];
                if (entry->override_slot == customiser->categories[category]->material_tag) {
                    customiser->model_texture_ids[side * 9 + category] = entry->texture_id;
                }
            }
        }
    }
}

void Customiser_RestoreModelTextureIDs(void *input) {
    CUSTOMISER_CONFIG *customiser = (CUSTOMISER_CONFIG *)input;
    i32 side;
    i32 category;
    i32 material;
    if (customiser == NULL) {
        return;
    }
    for (side = 0; side < 2; ++side) {
        CHARACTER_MODEL_LAYERS *model = APICharacterLoaded(customiser->character_ids[side]);
        if (model == NULL) {
            continue;
        }
        for (category = 0; category < 9; ++category) {
            if (customiser->model_texture_ids[side * 9 + category] == 0) {
                continue;
            }
            for (material = 0; material < model->hierarchy->material_count; ++material) {
                NUMTL *entry = model->hierarchy->materials[material];
                if (entry->override_slot == customiser->categories[category]->material_tag) {
                    model->hierarchy->materials[material]->texture_id = (i16)customiser->model_texture_ids[side * 9 + category];
                    NuMtlUpdate(model->hierarchy->materials[material]);
                }
            }
        }
    }
}

void Customiser_Set100PercentPieces(CUSTOMISER_CONFIG *input) {
    // MATCH-EXCEPTION: the cast is repeated at each use; a cast local changes register choice (U3)
    i32 category;
    i32 index;
    if (input == NULL || input->save == NULL) {
        return;
    }
    for (category = 0; category < 9; ++category) {
        if (input->piece_counts[category] <= 0) {
            continue;
        }
        for (index = 0; index < input->piece_counts[category]; ++index) {
            if ((input->piece_sets[category][index].availability_flags & CUSTOM_PIECE_100_PERCENT_SIDE_1) != 0) {
                input->save->pieces[category] = (u16)index;
            }
            if ((input->piece_sets[category][index].availability_flags & CUSTOM_PIECE_100_PERCENT_SIDE_2) != 0) {
                input->save[1].pieces[category] = (u16)index;
            }
        }
    }
}

void Customiser_CopyDefaultPiecesToSave(CUSTOMISER_CONFIG *customiser, void *save_input) {
    GAME_CUSTOMISER_START *save;
    if (customiser == NULL) {
        return;
    }
    save = (GAME_CUSTOMISER_START *)save_input;
    if (save == NULL) {
        save = customiser->save;
        if (save == NULL) {
            return;
        }
    }
    {
        i32 side;
        i32 i;
        for (side = 0; side < 2; ++side) {
            for (i = 0; i < 9; ++i) {
                save[side].pieces[i] = customiser->default_pieces[side][i];
            }
        }
    }
}

CUSTOM_PIECE_CONFIG *Customiser_FindPieceByName(CUSTOMISER_CONFIG *customiser, char *name, i32 *category, i32 *index) {
    i32 category_index;
    i32 piece_index;
    if (customiser == NULL) {
        return NULL;
    }
    for (category_index = 0; category_index < 9; ++category_index) {
        for (piece_index = 0; piece_index < customiser->piece_counts[category_index]; ++piece_index) {
            if (NuStrICmp(name, customiser->piece_sets[category_index][piece_index].name) == 0) {
                if (category != NULL) {
                    *category = category_index;
                }
                if (index != NULL) {
                    *index = piece_index;
                }
                return &customiser->piece_sets[category_index][piece_index];
            }
        }
    }
    return NULL;
}

/* Does not check the customiser/save arguments. */
i32 Customiser_GetIcon(CUSTOMISER_CONFIG *customiser, const GAME_CUSTOMISER_START *save, i32 unused) {
    CUSTOM_PIECE_CONFIG *hat = &customiser->piece_sets[0][save->pieces[0]];
    CUSTOM_PIECE_CONFIG *head;
    i32 character;
    if ((hat->availability_flags & CUSTOM_PIECE_HAT_OFF) == 0) {
        head = &customiser->piece_sets[1][save->pieces[1]];
        if ((head->availability_flags & CUSTOM_PIECE_NO_HAT) == 0) {
            character = hat->icon_character_id;
            if (character != -1) {
                goto found;
            }
            character = hat->character_id;
            if (character != -1) {
                goto found;
            }
        }
    }
    head = &customiser->piece_sets[1][save->pieces[1]];
    character = head->icon_character_id;
    if (character != -1) {
        goto found;
    }
    character = head->character_id;
    if (character != -1) {
    found:
        return CDataList[character].icon_model;
    }
    return PCArcadeUnpairedIconModel;
}

static CUSTOM_CATEGORY_CONFIG CustomiserSetDataPrivate[9] = {{"hat_hair", 1, -1, {1, 0}, "chars\\weirdo\\all\\hat_hair_all.gsc"},
                                                             {"head", 1, -1, {0, 0}, "chars\\weirdo\\all\\head_all.gsc"},
                                                             {NULL, 0, -1, {0, 0}, NULL},
                                                             {"arm", 0, 2, {1, 0}, NULL},
                                                             {"hand", 0, 4, {0, 0}, NULL},
                                                             {"cape", 0, 3, {2, 0}, NULL},
                                                             {"body", 0, 1, {3, 0}, NULL},
                                                             {"under_pants", 0, 6, {1, 0}, NULL},
                                                             {"leg", 0, 5, {2, 0}, NULL}};

CUSTOMISER_CONFIG *Game_Customiser = 0;

CUSTOMISER_CONFIG *Customiser_Configure(char *filename, VARIPTR *buffer, VARIPTR *buffer_end, i32 first_character, i32 second_character,
                                        i32 (*available)(CUSTOM_PIECE_CONFIG *), void (*configure_piece)(CUSTOM_PIECE_CONFIG *, NUFPAR *),
                                        i32 (*weapon_from_name)(char *), void *save_input, i16 *animations) {
    NUFPAR *parser;
    CUSTOMISER_CONFIG *customiser;
    GAME_CUSTOMISER_START *save = (GAME_CUSTOMISER_START *)save_input;
    i32 category;
    i32 index;
    i32 locators[9];
    i32 layers[9];
    f32 x_offsets[9];
    f32 y_offsets[9];
    u8 categories[440];
    CUSTOM_PIECE_CONFIG pieces[440];
    char *names;
    i32 count;
    if (first_character == -1 && second_character == -1) {
        return NULL;
    }
    if (Game_Customiser != NULL) {
        Customiser_Dump();
        return NULL;
    }
    parser = NuFParCreate(filename);
    if (parser == NULL) {
        return NULL;
    }

    for (category = 0; category < 9; ++category) {
        x_offsets[category] = y_offsets[category] = 0.0f;
        locators[category] = -1;
        layers[category] = -1;
    }
    memset(pieces, 0, sizeof(pieces));
    names = buffer->char_ptr;
    count = 0;
    while (count < 440) {
        if (NuFParGetLine(parser) == 0) {
            break;
        }
        if (NuFParGetWord(parser) == 0) {
            break;
        }
        category = CustomiserFindSetFromNamePrivate(parser->word_buffer_ptr);
        if (category != -1 && NuFParGetWord(parser) != 0) {
            CUSTOM_PIECE_CONFIG *piece;
            piece = &pieces[count];
            piece->weapon_model = -1;
            piece->character_id = -1;
            piece->icon_character_id = -1;
            piece->part_slots[0] = -1;
            piece->part_slots[1] = -1;
            piece->part_slots[2] = -1;
            piece->bonus_group = -1;
            piece->collection_type = -1;
            piece->name = names;
            piece->availability_flags = 0;
            piece->model_flags = 0;
            piece->gameplay_flags = 0;
            categories[count] = category;
            NuStrCpy(names, parser->word_buffer_ptr);
            names += NuStrLen(parser->word_buffer_ptr) + 1;
            NuStrLen(parser->word_buffer_ptr);
            if (CustomiserSetDataPrivate[category].name == NULL && weapon_from_name != NULL) {
                piece->weapon_model = weapon_from_name(piece->name);
            }
            while (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "from") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        piece->character_id = CharIDFromName(parser->word_buffer_ptr);
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "icon_from") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        piece->icon_character_id = CharIDFromName(parser->word_buffer_ptr);
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "from_variant") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        piece->collection_type = CharVariant_Find(parser->word_buffer_ptr);
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "no_hat") == 0) {
                    piece->availability_flags |= CUSTOM_PIECE_NO_HAT;
                } else if (NuStrICmp(parser->word_buffer_ptr, "hat_off") == 0) {
                    piece->availability_flags |= CUSTOM_PIECE_HAT_OFF;
                } else if (NuStrICmp(parser->word_buffer_ptr, "cape_off") == 0) {
                    piece->availability_flags |= CUSTOM_PIECE_CAPE_OFF;
                } else if (NuStrICmp(parser->word_buffer_ptr, "helmet") == 0) {
                    piece->availability_flags |= CUSTOM_PIECE_HELMET;
                } else if (NuStrICmp(parser->word_buffer_ptr, "big_head") == 0) {
                    piece->availability_flags |= CUSTOM_PIECE_BIG_HEAD;
                } else if (NuStrICmp(parser->word_buffer_ptr, "big_hat") == 0) {
                    piece->availability_flags |= CUSTOM_PIECE_BIG_HAT;
                } else if (NuStrICmp(parser->word_buffer_ptr, "not_demo") == 0) {
                    piece->availability_flags |= CUSTOM_PIECE_NOT_DEMO;
                } else if (NuStrICmp(parser->word_buffer_ptr, "100_percent") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        i32 side = NuAToI(parser->word_buffer_ptr);
                        if (side == 1) {
                            piece->availability_flags |= CUSTOM_PIECE_100_PERCENT_SIDE_1;
                        } else if (side == 2) {
                            piece->availability_flags |= CUSTOM_PIECE_100_PERCENT_SIDE_2;
                        }
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "default") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        i32 side = NuAToI(parser->word_buffer_ptr);
                        if (side == 1) {
                            piece->availability_flags |= CUSTOM_PIECE_DEFAULT_SIDE_1;
                        } else if (side == 2) {
                            piece->availability_flags |= CUSTOM_PIECE_DEFAULT_SIDE_2;
                        }
                    }
                } else if (configure_piece != NULL) {
                    configure_piece(piece, parser);
                }
            }
            ++count;
        } else if (NuStrICmp(parser->word_buffer_ptr, "locator") == 0) {
            if (NuFParGetWord(parser) != 0) {
                category = CustomiserFindSetFromNamePrivate(parser->word_buffer_ptr);
                if (category != -1) {
                    locators[category] = NuFParGetInt(parser);
                    if (locators[category] < 0 || locators[category] >= 16) {
                        locators[category] = -1;
                    } else {
                        while (NuFParGetWord(parser) != 0) {
                            if (NuStrICmp(parser->word_buffer_ptr, "xoffset") == 0) {
                                x_offsets[category] = NuFParGetFloat(parser);
                            } else if (NuStrICmp(parser->word_buffer_ptr, "yoffset") == 0) {
                                y_offsets[category] = NuFParGetFloat(parser);
                            }
                        }
                    }
                }
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "layer") == 0) {
            if (NuFParGetWord(parser) != 0) {
                category = CustomiserFindSetFromNamePrivate(parser->word_buffer_ptr);
                if (category != -1) {
                    layers[category] = NuFParGetInt(parser);
                    if (layers[category] < 0 || layers[category] >= 32) {
                        layers[category] = -1;
                    }
                }
            }
        }
    }
    NuFParDestroy(parser);
    if (count > 0) {
        buffer->addr = ((u32)names + 3) & ~3u;
        customiser = (CUSTOMISER_CONFIG *)buffer->void_ptr;
        memset(customiser, 0, sizeof(*customiser));
        buffer->addr = (buffer->addr + sizeof(*customiser) + 3) & ~3u;
        customiser->character_ids[0] = first_character;
        customiser->character_ids[1] = second_character;
        customiser->piece_available = available != NULL ? available : CustomiserPieceAvailableDefaultPrivate;
        customiser->save = save;
        customiser->animation_ids_to_load = (u16 *)animations;
        for (category = 0; category < 9; ++category) {
            CUSTOM_PIECE_CONFIG *output;
            customiser->categories[category] = &CustomiserSetDataPrivate[category];
            customiser->piece_sets[category] = (CUSTOM_PIECE_CONFIG *)buffer->void_ptr;
            customiser->piece_counts[category] = 0;
            customiser->locator_indices[category] = locators[category];
            customiser->layer_indices[category] = layers[category];
            output = customiser->piece_sets[category];
            customiser->locator_x_offsets[category] = x_offsets[category];
            customiser->locator_y_offsets[category] = y_offsets[category];
            for (index = 0; index < count; ++index) {
                if (categories[index] == category) {
                    PCByteCopy(output, &pieces[index], sizeof(*output));
                    ++customiser->piece_counts[category];
                    ++output;
                }
            }
            if (customiser->piece_counts[category] > 0) {
                buffer->addr = ((u32)output + 3) & ~3u;
            }
        }
        if (customiser->save == NULL) {
            customiser->save = (GAME_CUSTOMISER_START *)buffer->void_ptr;
            buffer->addr = (buffer->addr + 0x73) & ~3u;
        }
        for (category = 0; category < 9; ++category) {
            i32 first_found = 0;
            i32 second_found = 0;
            for (index = 0; index < customiser->piece_counts[category]; ++index) {
                if (first_found && second_found) {
                    break;
                }
                if ((customiser->piece_sets[category][index].availability_flags & CUSTOM_PIECE_DEFAULT_SIDE_1) != 0) {
                    customiser->default_pieces[0][category] = index;
                    first_found = 1;
                }
                if ((customiser->piece_sets[category][index].availability_flags & CUSTOM_PIECE_DEFAULT_SIDE_2) != 0) {
                    customiser->default_pieces[1][category] = index;
                    second_found = 1;
                }
            }
        }
        Customiser_CopyDefaultPiecesToSave(customiser, NULL);
        Game_Customiser = customiser;
        return customiser;
    }
    return NULL;
}
