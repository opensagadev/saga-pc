#include "saga.h"
#include <string.h>
#include <stdio.h>

typedef struct customiser_anims_load_pc_s {
    u8 unknown_00[0x6c];
    i16 character_ids[2];
    u8 unknown_70[0x198];
    i32 model_texture_ids[18];
    u8 unknown_250[0x818];
    u16 *animation_ids_to_load;
} CUSTOMISER_ANIMS_LOAD_PC;

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

void __cdecl CustomiserLoadTracePc(char *path);

typedef struct custom_accessory_resource_pc_s {
    NUGSCN *scene;
    NUSPECIALHANDLE special;
    i32 original_texture_id;
    i32 texture_id;
    i32 material_index;
    CHARACTER_MODEL_LAYERS_PC *character_model;
} CUSTOM_ACCESSORY_RESOURCE_PC;

typedef struct custom_accessory_hierarchy_pc_s {
    u8 unknown_00[0xc];
    NUMTL **materials;
    i32 material_count;
} CUSTOM_ACCESSORY_HIERARCHY_PC;

typedef struct custom_accessory_material_pc_s {
    u8 unknown_00[0x74];
    i16 texture_id;
    u8 unknown_76[0x24];
    u8 material_tag;
} CUSTOM_ACCESSORY_MATERIAL_PC;

typedef struct KILLPARTS_OBJECT_LIGHT_PC {
    u8 unknown_0[0x4d8];
    PART_LIGHTING_DATA_PC lighting;
} KILLPARTS_OBJECT_LIGHT_PC;

typedef struct KILLPARTS_SAVED_PIECES_PC {
    i16 pieces[9];
    u8 unknown_12[0x26];
} KILLPARTS_SAVED_PIECES_PC;

typedef struct KILLPARTS_CUSTOMISER_PC {
    u8 unknown_0[0x24];
    CUSTOMPIECE_LAYERS_PC *head_pieces;
    CUSTOMPIECE_LAYERS_PC *helmet_pieces;
    u8 unknown_2c[0x40];
    i16 character_ids[2];
    u8 unknown_70[0x104];
    KILLPARTS_SAVED_PIECES_PC *save;
} KILLPARTS_CUSTOMISER_PC;

static i32 __cdecl CustomiserPieceAvailableDefaultPrivatePc(CUSTOMPIECE_LAYERS_PC *input) {
    CUSTOM_PIECE_CONFIG_PC *piece = (CUSTOM_PIECE_CONFIG_PC *)input;
    if ((piece->availability_flags & 0x180) != 0 && Game_100PercentComplete() == 0) {
        return 0;
    }
    return 1;
}

static i32 CustomiserFindSetFromNamePrivatePc(char *name) {
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

void __cdecl Customiser_SetAnimsToLoad(CUSTOMISER_LAYERS_PC *input, i32 enabled) {
    CUSTOMISER_ANIMS_LOAD_PC *customiser = (CUSTOMISER_ANIMS_LOAD_PC *)input;
    i32 character;
    if (customiser == NULL) {
        return;
    }
    for (character = 0; character < 2; ++character) {
        CHARACTERANIM_CONFIG_PC *animation = CDataList[customiser->character_ids[character]].animations;
        if (animation == NULL) {
            continue;
        }
        for (; animation->name != NULL; ++animation) {
            i32 i;
            if (enabled == 0) {
                animation->flags &= ~0x8000u;
                continue;
            }
            animation->flags |= 0x8000u;
            if (customiser->animation_ids_to_load == NULL) {
                continue;
            }
            for (i = 0; (i16)customiser->animation_ids_to_load[i] != -1; ++i) {
                if (customiser->animation_ids_to_load[i] == (u16)animation->action_id) {
                    animation->flags &= ~0x8000u;
                    break;
                }
            }
        }
    }
}

/* 00592820..00592939, including the separate native NULL return. */
i32 __cdecl Customiser_NextPieceLeft(CUSTOMISER_LAYERS_PC *input, i32 index, i32 count, i32 unused, i32 category) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 attempts;
    (void)unused;
    if (customiser != NULL) {
        attempts = 0;
        while (attempts++ < count) {
            --index;
            if (index < 0) {
                index += count;
            }
            if (category != 2) {
                WORLDINFO *world = WorldInfo_CurrentlyActive();
                if (world != NULL && HUB_ADATA != NULL && world->area == HUB_ADATA) {
                    CUSTOM_ACCESSORY_RESOURCE_PC *resources =
                        (CUSTOM_ACCESSORY_RESOURCE_PC *)world->customiser_resources[category];
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
            if (((customiser->piece_sets[category][index].availability_flags & 0x180) == 0 ||
                 Game_100PercentComplete() != 0) &&
                customiser->piece_available((CUSTOMPIECE_LAYERS_PC *)&customiser->piece_sets[category][index]) != 0) {
                break;
            }
        }
    }
    return index;
}

/* 00592940 / 291 */
i32 __cdecl Customiser_NextPieceRight(CUSTOMISER_LAYERS_PC *input, i32 index, i32 count, i32 unused, i32 category) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 attempts;
    (void)unused;
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
                CUSTOM_ACCESSORY_RESOURCE_PC *resources =
                    (CUSTOM_ACCESSORY_RESOURCE_PC *)world->customiser_resources[category];
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
        if (((customiser->piece_sets[category][index].availability_flags & 0x180) == 0 ||
             Game_100PercentComplete() != 0) &&
            customiser->piece_available((CUSTOMPIECE_LAYERS_PC *)&customiser->piece_sets[category][index]) != 0) {
            break;
        }
    }
    return index;
}

void __cdecl Customiser_LoadAccessories(CUSTOMISER_LAYERS_PC *input, APICHARACTERMODELLIST *models) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 side;
    i32 category;
    i32 material_index;
    i32 id;
    CHARACTER_MODEL_LAYERS_PC *model;
    CUSTOM_ACCESSORY_RESOURCE_PC *resource;
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
        resource = (CUSTOM_ACCESSORY_RESOURCE_PC *)KillPartsAccessoriesPc[side];
        for (category = 0; category < 9; ++category, ++resource) {
            u16 piece_index;
            memset(resource, 0, sizeof(*resource));
            if (customiser->categories[category]->name == NULL || customiser->piece_counts[category] <= 0) {
                continue;
            }
            resource->character_model = model;
            piece_index =
                customiser->save
                    ->pieces[side * (offsetof(CUSTOM_SAVE_CONFIG_PC, secondary_pieces) / sizeof(u16)) + category];
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
                for (material_index = 0;
                     material_index < ((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)->material_count;
                     ++material_index) {
                    NUMTL *material = ((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)->materials[material_index];
                    if (((CUSTOM_ACCESSORY_MATERIAL_PC *)material)->material_tag !=
                        customiser->categories[category]->material_tag) {
                        continue;
                    }
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
    if (customiser == NULL || joint == -1 || object->character_model->points_of_interest[joint] == NULL) {
        return;
    }
    side = object->id != (u16)customiser->character_ids[0];
    resource = KillPartsAccessoriesPc[side];
    for (category = 0; category < 9; ++category, ++resource) {
        if (customiser->piece_counts[category] <= 0 || category == 2) {
            continue;
        }
        if (category == 0) {
            pieces =
                customiser->save->pieces + side * (offsetof(CUSTOM_SAVE_CONFIG_PC, secondary_pieces) / sizeof(u16));
            if ((customiser->piece_sets[0][pieces[0]].availability_flags & 0x20) != 0 ||
                (customiser->piece_sets[1][pieces[1]].availability_flags & 1) != 0 || object->helmet != 0) {
                continue;
            }
        }
        if (NuSpecialExistsFn(&resource->special) == 0) {
            continue;
        }
        if (matrices != NULL) {
            matrix = matrices[joint];
        } else {
            matrix = object->locator_matrices[joint];
        }
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

void __cdecl Customiser_AddPartAccessories(CUSTOMISER_LAYERS_PC *customiser, GAMEOBJECT *object, i32 animation,
                                           i32 mode, f32 scale) {
    KILLPARTS_CUSTOMISER_PC *accessories;
    i32 side;
    KILLPARTS_ACCESSORY_PC *resource;
    i32 category;
    i32 joint;
    joint = object->character_data->game_character->helmet_locator;
    if (customiser == NULL || joint == -1 || object->character_model->points_of_interest[joint] == NULL) {
        return;
    }
    accessories = ((KILLPARTS_CUSTOMISER_PC *)customiser);
    side = ((i16)object->id) != accessories->character_ids[0];
    resource = KillPartsAccessoriesPc[side];
    for (category = 0; category < 9; ++category, ++resource) {
        NUSPECIALHANDLE *special;
        NUVEC momentum;
        ADDPART_PC params;
        if (category == 2) {
            continue;
        }
        if (category == 0 &&
            ((accessories->helmet_pieces[((u16)(accessories->save[side].pieces[1]))].layer_flags & 1) != 0 ||
             (accessories->head_pieces[((u16)(accessories->save[side].pieces[0]))].layer_flags & 0x20) != 0)) {
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
        params.field_14 = 0.1f;
        params.field_18 = 0.1f;
        params.gravity = -5.0f;
        params.matrix = &object->locator_matrices[joint];
        params.velocity = &momentum;
        params.special = special;
        params.lighting_data = Cheats_CheckFlags(1) ? ((PART_LIGHTING_DATA_PC *)(&KillPartsZeroLightingPc))
                                                    : &((KILLPARTS_OBJECT_LIGHT_PC *)object)->lighting;
        params.time_step = FRAMETIME;
        params.flags = mode == 0 ? 0x480 : 0x90;
        params.stop_fn = PartStop_Flickerer;
        params.draw_fn = PartDraw_Flickerer;
        params.impact_fn = PartImpact_Brick;
        AddPart(&params);
    }
}

void __cdecl Customiser_DumpAccessories(void *input) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 side;
    i32 category;
    CUSTOM_ACCESSORY_RESOURCE_PC *resource;
    if (customiser == NULL || Customiser_AccessoriesLoaded != 1) {
        return;
    }
    for (side = 0; side < 2; ++side) {
        if (apicharsys->playermodelids[customiser->character_ids[side]] == -1) {
            continue;
        }
        resource = (CUSTOM_ACCESSORY_RESOURCE_PC *)KillPartsAccessoriesPc[side];
        for (category = 0; category < 9; ++category, ++resource) {
            if (customiser->categories[category]->name == NULL) {
                continue;
            }
            if (resource->scene != NULL) {
                NuGScnRemove(resource->scene);
                resource->scene = NULL;
            } else if (resource->texture_id != 0) {
                i32 material_index = resource->material_index;
                ((CUSTOM_ACCESSORY_MATERIAL_PC *)((CUSTOM_ACCESSORY_HIERARCHY_PC *)resource->character_model->hierarchy)
                     ->materials[material_index])
                    ->texture_id = (i16)resource->original_texture_id;
                NuMtlUpdate(
                    ((CUSTOM_ACCESSORY_HIERARCHY_PC *)resource->character_model->hierarchy)->materials[material_index]);

                NuTexDestroy(resource->texture_id);
            }
        }
    }
}

static i32 CustomiserUseBigScenesPc = 1;

/* 005931a0 / 811. Retail ignored extra arguments remain explicit calls. */
void __cdecl Customiser_LoadAll(CUSTOMISER_LAYERS_PC *input, WORLDINFO *world) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
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
            world->customiser_resources[category] = world->giz_buffer.void_ptr;
            memset(world->giz_buffer.void_ptr, 0,
                   customiser->piece_counts[category] * sizeof(CUSTOM_ACCESSORY_RESOURCE_PC));
            world->giz_buffer.addr += customiser->piece_counts[category] * sizeof(CUSTOM_ACCESSORY_RESOURCE_PC);
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
        if (resources == NULL) {
            continue;
        }
        if (CustomiserUseBigScenesPc != 0 && customiser->categories[category]->uses_special != 0 &&
            customiser->categories[category]->scene_file != NULL) {
            world->customiser_shared_scenes[category] =
                NuGScnRead(&world->giz_buffer, world->buffer_end, customiser->categories[category]->scene_file);
            CustomiserLoadTracePc(customiser->categories[category]->scene_file);
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
                    CustomiserLoadTracePc(path);
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
                    ((i32(__cdecl *)(char *, char *, i32))NuFileExtConvert)(converted_path, path, 0x80);
                    item = NuFilePakGetItem(texture_pack, converted_path);
                    if (item != -1 && NuFilePakGetItemInfo(texture_pack, item, &data, &size) != 0) {
                        data = NuPtrBlockFix(data);
                        resource->texture_id = NuTexCreateNative((NUTEXDIM_PC *)data);
                        CustomiserLoadTracePc(path);
                    }
                }
                if (resource->texture_id == 0) {
                    resource->texture_id = NuTexRead(path, &world->giz_buffer, world->buffer_end);
                    CustomiserLoadTracePc(path);
                }
            }
        }
    }
}

void __cdecl Customiser_DumpAll(void *input, WORLDINFO *world) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 category;
    i32 piece;
    if (customiser == NULL) {
        return;
    }
    for (category = 0; category < 9; ++category) {
        CUSTOM_ACCESSORY_RESOURCE_PC *resources = (CUSTOM_ACCESSORY_RESOURCE_PC *)world->customiser_resources[category];
        if (resources == NULL) {
            continue;
        }
        for (piece = 0; piece < customiser->piece_counts[category]; ++piece) {
            CUSTOM_ACCESSORY_RESOURCE_PC *resource = &resources[piece];
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

void __cdecl Customiser_ResetModelTextureIDs(CUSTOMISER_LAYERS_PC *input) {
    CUSTOMISER_ANIMS_LOAD_PC *customiser = (CUSTOMISER_ANIMS_LOAD_PC *)input;
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

/* Append after existing CUSTOM_ACCESSORY_* types in src/legoapi/customise.c.
 * Retail 005935f0/179; no callees. Uses established local view types. */
void __cdecl Customiser_SaveModelTextureIDs(void *input, CHARACTER_MODEL_LAYERS_PC *model) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOMISER_ANIMS_LOAD_PC *textures = (CUSTOMISER_ANIMS_LOAD_PC *)input;
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
            for (material = 0; material < ((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)->material_count;
                 ++material) {
                NUMTL *entry = ((CUSTOM_ACCESSORY_HIERARCHY_PC *)model->hierarchy)->materials[material];
                if (((CUSTOM_ACCESSORY_MATERIAL_PC *)entry)->material_tag ==
                    customiser->categories[category]->material_tag) {
                    textures->model_texture_ids[side * 9 + category] = entry->texture_id;
                }
            }
        }
    }
}

void __cdecl Customiser_RestoreModelTextureIDs(void *input) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOMISER_ANIMS_LOAD_PC *textures = (CUSTOMISER_ANIMS_LOAD_PC *)input;
    i32 side;
    i32 category;
    i32 material;
    if (customiser == NULL) {
        return;
    }
    for (side = 0; side < 2; ++side) {
        CHARACTER_MODEL_LAYERS_PC *model = APICharacterLoaded(customiser->character_ids[side]);
        if (model == NULL) {
            continue;
        }
        for (category = 0; category < 9; ++category) {
            if (textures->model_texture_ids[side * 9 + category] == 0) {
                continue;
            }
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

void __cdecl Customiser_Set100PercentPieces(CUSTOMISER_LAYERS_PC *input) {
    // MATCH-EXCEPTION: the cast is repeated at each use; a cast local changes register choice (U3)
    i32 category;
    i32 index;
    if (input == NULL || ((CUSTOMISER_CONFIG_PC *)input)->save == NULL) {
        return;
    }
    for (category = 0; category < 9; ++category) {
        if (((CUSTOMISER_CONFIG_PC *)input)->piece_counts[category] <= 0) {
            continue;
        }
        for (index = 0; index < ((CUSTOMISER_CONFIG_PC *)input)->piece_counts[category]; ++index) {
            if ((((CUSTOMISER_CONFIG_PC *)input)->piece_sets[category][index].availability_flags & 0x80) != 0) {
                ((CUSTOMISER_CONFIG_PC *)input)->save->pieces[category] = (u16)index;
            }
            if ((((CUSTOMISER_CONFIG_PC *)input)->piece_sets[category][index].availability_flags & 0x100) != 0) {
                ((CUSTOMISER_CONFIG_PC *)input)->save->secondary_pieces[category] = (u16)index;
            }
        }
    }
}

void __cdecl Customiser_CopyDefaultPiecesToSave(CUSTOMISER_LAYERS_PC *input, void *save_input) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    CUSTOM_SAVE_CONFIG_PC *save;
    if (customiser == NULL) {
        return;
    }
    save = (CUSTOM_SAVE_CONFIG_PC *)save_input;
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
                save->pieces[side * (offsetof(CUSTOM_SAVE_CONFIG_PC, secondary_pieces) / sizeof(u16)) + i] =
                    customiser->default_pieces[side][i];
            }
        }
    }
}

CUSTOMPIECE_LAYERS_PC *__cdecl Customiser_FindPieceByName(CUSTOMISER_LAYERS_PC *input, char *name, i32 *category,
                                                          i32 *index) {
    CUSTOMISER_CONFIG_PC *customiser = (CUSTOMISER_CONFIG_PC *)input;
    i32 category_index;
    i32 piece_index;
    i32 *index_local = index; // MATCH-EXCEPTION: parameter copied to a local; VC8 register choice (U20)
    if (customiser == NULL) {
        return NULL;
    }
    for (category_index = 0; category_index < 9; ++category_index) {
        for (piece_index = 0; piece_index < customiser->piece_counts[category_index]; ++piece_index) {
            if (NuStrICmp(name, customiser->piece_sets[category_index][piece_index].name) == 0) {
                if (category != NULL) {
                    *category = category_index;
                }
                if (index_local != NULL) {
                    *index_local = piece_index;
                }
                return (CUSTOMPIECE_LAYERS_PC *)&customiser->piece_sets[category_index][piece_index];
            }
        }
    }
    return NULL;
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

CUSTOMISER_LAYERS_PC *__cdecl Customiser_Configure(char *filename, VARIPTR *buffer, VARIPTR *buffer_end,
                                                   i32 first_character, i32 second_character,
                                                   i32 (*available)(CUSTOMPIECE_LAYERS_PC *),
                                                   void (*configure_piece)(CUSTOMPIECE_LAYERS_PC *, NUFPAR_PC *),
                                                   i32 (*weapon_from_name)(char *), void *save_input, i16 *animations) {
    NUFPAR_PC *parser;
    CUSTOMISER_CONFIG_PC *customiser;
    CUSTOM_SAVE_CONFIG_PC *save = (CUSTOM_SAVE_CONFIG_PC *)save_input;
    i32 category;
    i32 index;
    i32 locators[9];
    i32 layers[9];
    f32 x_offsets[9];
    f32 y_offsets[9];
    u8 categories[440];
    CUSTOM_PIECE_CONFIG_PC pieces[440];
    char *names;
    i32 count;
    (void)buffer_end;
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
        category = CustomiserFindSetFromNamePrivatePc(parser->word_buffer_ptr);
        if (category != -1 && NuFParGetWord(parser) != 0) {
            CUSTOM_PIECE_CONFIG_PC *piece;
            piece = &pieces[count];
            piece->weapon_model = -1;
            piece->character_id = -1;
            piece->icon_character_id = -1;
            piece->unknown_indices_0a[0] = -1;
            piece->unknown_indices_0a[1] = -1;
            piece->unknown_indices_0a[2] = -1;
            piece->field_10 = -1;
            piece->collection_type = -1;
            piece->name = names;
            piece->availability_flags = 0;
            piece->model_flags = 0;
            piece->gameplay_flags = 0;
            categories[count] = category;
            NuStrCpy(names, parser->word_buffer_ptr);
            names += NuStrLen(parser->word_buffer_ptr) + 1;
            NuStrLen(parser->word_buffer_ptr);
            if (CustomiserSetDataPrivatePc[category].name == NULL && weapon_from_name != NULL) {
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
                        if (side == 1) {
                            piece->availability_flags |= 0x80;
                        } else if (side == 2) {
                            piece->availability_flags |= 0x100;
                        }
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "default") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        i32 side = NuAToI(parser->word_buffer_ptr);
                        if (side == 1) {
                            piece->availability_flags |= 0x200;
                        } else if (side == 2) {
                            piece->availability_flags |= 0x400;
                        }
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
                category = CustomiserFindSetFromNamePrivatePc(parser->word_buffer_ptr);
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
        customiser = (CUSTOMISER_CONFIG_PC *)buffer->void_ptr;
        memset(customiser, 0, sizeof(*customiser));
        buffer->addr = (buffer->addr + sizeof(*customiser) + 3) & ~3u;
        customiser->character_ids[0] = first_character;
        customiser->character_ids[1] = second_character;
        customiser->piece_available = available != NULL ? available : CustomiserPieceAvailableDefaultPrivatePc;
        customiser->save = save;
        customiser->animation_ids_to_load = (u16 *)animations;
        for (category = 0; category < 9; ++category) {
            CUSTOM_PIECE_CONFIG_PC *output;
            customiser->categories[category] = &CustomiserSetDataPrivatePc[category];
            customiser->piece_sets[category] = (CUSTOM_PIECE_CONFIG_PC *)buffer->void_ptr;
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
            customiser->save = (CUSTOM_SAVE_CONFIG_PC *)buffer->void_ptr;
            buffer->addr = (buffer->addr + 0x73) & ~3u;
        }
        for (category = 0; category < 9; ++category) {
            i32 first_found = 0;
            i32 second_found = 0;
            for (index = 0; index < customiser->piece_counts[category]; ++index) {
                if (first_found && second_found) {
                    break;
                }
                if ((customiser->piece_sets[category][index].availability_flags & 0x200) != 0) {
                    customiser->default_pieces[0][category] = index;
                    first_found = 1;
                }
                if ((customiser->piece_sets[category][index].availability_flags & 0x400) != 0) {
                    customiser->default_pieces[1][category] = index;
                    second_found = 1;
                }
            }
        }
        Customiser_CopyDefaultPiecesToSave((CUSTOMISER_LAYERS_PC *)customiser, NULL);
        Game_Customiser = customiser;
        return (CUSTOMISER_LAYERS_PC *)customiser;
    }
    return NULL;
}
