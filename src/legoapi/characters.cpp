#include <stdio.h>
#include <string.h>
#include "saga.h"

/* Retail unit 0x55b1c0-0x55c0ec (legoapi/characters.c): definitions in retail address order,
   followed by the functions of other compilands that this file still holds. */

STATIC_ASSERT(sizeof(CHAR_CATEGORY_RECORD_PC) == 12);
STATIC_ASSERT(offsetof(CHAR_CATEGORY_RECORD_PC, name) == 0);
STATIC_ASSERT(offsetof(CHAR_CATEGORY_RECORD_PC, model_flags) == 4);
STATIC_ASSERT(offsetof(CHAR_CATEGORY_RECORD_PC, game_flags) == 8);
extern "C" {
    extern char AreaFixUpName_Jedi[8];
    extern const char PCContextName_Teleport[9];
}
typedef struct permanent_material_reader_pc_s {
    u8 unknown_0000[0x40];
    u32 attributes;
    u32 shader_flags;
    u8 unknown_0048[0xc];
    f32 diffuse[3];
    u8 unknown_0060[0x10];
    f32 opacity;
    i16 texture_id;
    u16 sort_priority;
} PERMANENT_MATERIAL_READER_PC;
STATIC_ASSERT(offsetof(PERMANENT_MATERIAL_READER_PC, attributes) == 0x40);
STATIC_ASSERT(offsetof(PERMANENT_MATERIAL_READER_PC, shader_flags) == 0x44);
STATIC_ASSERT(offsetof(PERMANENT_MATERIAL_READER_PC, diffuse) == 0x54);
STATIC_ASSERT(offsetof(PERMANENT_MATERIAL_READER_PC, opacity) == 0x70);
STATIC_ASSERT(offsetof(PERMANENT_MATERIAL_READER_PC, texture_id) == 0x74);
STATIC_ASSERT(offsetof(PERMANENT_MATERIAL_READER_PC, sort_priority) == 0x76);
extern "C" NUMTL *FadeMtl, *FadeMtl2, *ScreenFadeMtl;
typedef struct CHARACTER_SCENE_WORLD_PC {
    u8 unknown_0000[0x2980];
    KILLPARTS_CHARSCENE_PC *character_scenes;
} CHARACTER_SCENE_WORLD_PC;
STATIC_ASSERT(offsetof(CHARACTER_SCENE_WORLD_PC, character_scenes) == 0x2980);
typedef struct CHARACTER_SCENE_CUTSYS_PC {
    CUTINFO **records;
    i32 count;
    u32 *character_flags;
} CHARACTER_SCENE_CUTSYS_PC;
STATIC_ASSERT(offsetof(CHARACTER_SCENE_CUTSYS_PC, character_flags) == 8);
STATIC_ASSERT(offsetof(WORLDINFO, buffer_end) == 0x108);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, flags) == 0x40);
typedef struct permanent_customiser_save_pc_s {
    u16 pieces[9];
    u8 unknown_12[0x26];
    u16 secondary_pieces[9];
    u8 unknown_4a[0x26];
} PERMANENT_CUSTOMISER_SAVE_PC;
STATIC_ASSERT(offsetof(PERMANENT_CUSTOMISER_SAVE_PC, pieces) == 0);
STATIC_ASSERT(offsetof(PERMANENT_CUSTOMISER_SAVE_PC, secondary_pieces) == 0x38);
STATIC_ASSERT(sizeof(PERMANENT_CUSTOMISER_SAVE_PC) == 0x70);
typedef struct permanent_customiser_game_pc_s {
    u8 unknown_0000[0x7c24];
    PERMANENT_CUSTOMISER_SAVE_PC customiser;
} PERMANENT_CUSTOMISER_GAME_PC;
STATIC_ASSERT(offsetof(PERMANENT_CUSTOMISER_GAME_PC, customiser) == 0x7c24);
STATIC_ASSERT(offsetof(LEVELDATA_PC, flags) == 0x64);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_index) == 0x94);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_tracks) == 0x118);
STATIC_ASSERT(sizeof(LEVELDATA_PC) == 0x130);
typedef struct CHARACTER_IDLE_STATE_PC {
    u8 unknown_000[0x2b8];
    f32 idle_total_time;
    f32 idle_animation_time;
    f32 idle_animation_limit;
    i16 idle_animation;
    i16 previous_idle_animation;
} CHARACTER_IDLE_STATE_PC;
typedef struct CHARACTER_IDLE_POLICY_PC {
    u8 unknown_000[0x116];
    u8 use_standard_idle;
} CHARACTER_IDLE_POLICY_PC;
STATIC_ASSERT(offsetof(CHARACTER_IDLE_STATE_PC, idle_total_time) == 0x2b8);
STATIC_ASSERT(offsetof(CHARACTER_IDLE_STATE_PC, idle_animation_time) == 0x2bc);
STATIC_ASSERT(offsetof(CHARACTER_IDLE_STATE_PC, idle_animation_limit) == 0x2c0);
STATIC_ASSERT(offsetof(CHARACTER_IDLE_STATE_PC, idle_animation) == 0x2c4);
STATIC_ASSERT(offsetof(CHARACTER_IDLE_STATE_PC, previous_idle_animation) == 0x2c6);
STATIC_ASSERT(offsetof(CHARACTER_IDLE_POLICY_PC, use_standard_idle) == 0x116);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, previous_animation) == 0x38);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, requested_animation) == 0x3a);
extern "C" f32 animduration_blendouttime;
extern "C" i16 id_R2Q5;
typedef struct CHARACTER_COMMUNICATE_READER_PC {
    u8 unknown_000[0xd38];
    f32 communicate_blend;
} CHARACTER_COMMUNICATE_READER_PC;
STATIC_ASSERT(offsetof(CHARACTER_COMMUNICATE_READER_PC, communicate_blend) == 0xd38);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, model_draw_result) == 0x288);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_joint_ready) == 0x28c);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, locator_matrices) == 0x804);
STATIC_ASSERT(offsetof(NUMTX, translation) == 0x30);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, thingy_locator) == 0x110);
STATIC_ASSERT(offsetof(GAMEDEBRISENTRY_PC, effect) == 0);
STATIC_ASSERT(offsetof(GAMEOBJECT, saved_hitpoints) == 0xe39);
STATIC_ASSERT(sizeof(PLAYERCHARACTERCONFIG_PC) == 0x120);
STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, name_id) == 0);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, move_fn) == 0x18);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, animate_fn) == 0x1c);
STATIC_ASSERT(offsetof(GAMESAVE, customiser_start) + offsetof(GAME_CUSTOMISER_START_PC, name) == 0x7c38);
STATIC_ASSERT(offsetof(GAMESAVE, customiser_start) + sizeof(GAME_CUSTOMISER_START_PC) +
                  offsetof(GAME_CUSTOMISER_START_PC, name) ==
              0x7c70);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, name) == 0);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, flags) == 4);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, action_id) == 8);
extern "C" {
    i32 __cdecl CanWearHatsInFreePlay(i32 id);
}
typedef void(__cdecl *CHARACTER_UPDATE_CALLBACK_PC)(GAMEOBJECT *);
typedef struct character_callback_writer_pc_s {
    i32 name_id;
    u32 model_flags;
    u8 unknown_08[0x10];
    CHARACTER_UPDATE_CALLBACK_PC move;
    CHARACTER_UPDATE_CALLBACK_PC animate;
    CHARACTER_UPDATE_CALLBACK_PC post_animate;
} CHARACTER_CALLBACK_WRITER_PC;
struct KILLPARTS_WORLD_SCENES_PC {
    u8 unknown_0[0x2980];
    KILLPARTS_CHARSCENE_PC *character_scenes;
};

extern "C" {
#pragma const_seg(".rdata$StartupCharText_SuperJumpPc")
    const char StartupCharText_SuperJumpPc[10] = "SuperJump";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_SuperStrengthPc")
    const char StartupCharText_SuperStrengthPc[14] = "SuperStrength";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_TransformatronPc")
    const char StartupCharText_TransformatronPc[15] = "Transformatron";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_BypassSecurityPc")
    const char StartupCharText_BypassSecurityPc[15] = "BypassSecurity";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_HazardProtectionPc")
    const char StartupCharText_HazardProtectionPc[17] = "HazardProtection";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_JediBaddiePc")
    const char StartupCharText_JediBaddiePc[11] = "JediBaddie";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_BountyHunterPc")
    const char StartupCharText_BountyHunterPc[13] = "BountyHunter";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_HighJumpPc")
    const char StartupCharText_HighJumpPc[9] = "HighJump";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_AstromechPc")
    const char StartupCharText_AstromechPc[10] = "Astromech";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_ProtocolPc")
    const char StartupCharText_ProtocolPc[9] = "Protocol";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_walker_2legsPc")
    const char StartupCharText_walker_2legsPc[13] = "walker_2legs";
#pragma const_seg()
#pragma const_seg(".rdata$StartupCharText_walker_4legsPc")
    const char StartupCharText_walker_4legsPc[13] = "walker_4legs";
#pragma const_seg()
#pragma data_seg(".data$StartupCharCategoriesDefaultPc")
    CHAR_CATEGORY_RECORD_PC StartupCharCategoriesDefaultPc[6] = {
        {(char *)StartupCharText_SuperJumpPc, 0x0u, 0x400000u},
        {(char *)StartupCharText_SuperStrengthPc, 0x0u, 0x800000u},
        {(char *)StartupCharText_TransformatronPc, 0x0u, 0x1000000u},
        {(char *)StartupCharText_BypassSecurityPc, 0x0u, 0x2000000u},
        {(char *)StartupCharText_HazardProtectionPc, 0x0u, 0x4000000u},
        {NULL, 0x0u, 0x0u},
    };
#pragma data_seg()
#pragma data_seg(".data$StartupCharCategoriesLswPc")
    CHAR_CATEGORY_RECORD_PC StartupCharCategoriesLswPc[10] = {
        {(char *)AreaFixUpName_Jedi, 0x8u, 0x0u},
        {(char *)StartupCharText_JediBaddiePc, 0xcu, 0x0u},
        {(char *)StartupCharText_BountyHunterPc, 0x1100080u, 0x0u},
        {(char *)PCContextName_Teleport, 0x40000u, 0x0u},
        {(char *)StartupCharText_HighJumpPc, 0x0u, 0x400000u},
        {(char *)StartupCharText_AstromechPc, 0x40u, 0x0u},
        {(char *)StartupCharText_ProtocolPc, 0x20u, 0x0u},
        {(char *)PCContextName_ZipUp, 0x100080u, 0x0u},
        {"Blaster", 0x80u, 0x0u},
        {NULL, 0x0u, 0x0u},
    };
#pragma data_seg()
#pragma data_seg(".data$StartupCharVariantNamesPc")
    char *StartupCharVariantNamesPc[23] = {
        "weirdo",
        "stormtrooper",
        "fett",
        "macewindu",
        "battledroid",
        "wookiee",
        "obiwankenobi",
        "leia",
        "clone",
        "rebel",
        "tie",
        "lando",
        "luke",
        "hansolo",
        "padme",
        (char *)StartupCharText_walker_2legsPc,
        (char *)StartupCharText_walker_4legsPc,
        "hoverdroid",
        "critter",
        "naboostarfighter",
        "henchman",
        NULL,
        NULL,
    };
#pragma data_seg()
}

i32 __cdecl CharIDFromName(const char *name) {
    i32 index;

    for (index = 0; index < CHARCOUNT; ++index) {
        if (NuStrICmp(CDataList[index].name, name) == 0) {
            return index;
        }
    }
    return -1;
}

// Genuine character subsystem callback; arg1 prefix, arg4 output are PC order.
i32 __cdecl RedirectAnim(char *directory, void *list, CHARACTERANIM_CONFIG_PC *animation, char *output_path) {
    const ANIM_REDIRECT_VIEW_PC *redirect;
    const char *name;
    if (list == NULL)
        return 0;
    for (redirect = (const ANIM_REDIRECT_VIEW_PC *)list; (name = redirect->name) != NULL; ++redirect) {
        if (redirect->animation_id == animation->action_id && NuStrICmp(name, animation->name) == 0) {
            NuStrCpy(output_path, directory);
            NuStrCat(output_path, animation->name);
            animation->flags &= ~8u;
            return 1;
        }
    }
    return 0;
}

extern "C" void __cdecl CharScenes_Init(VARIPTR *buffer, VARIPTR *end) {
    u32 size = CHARCOUNT * sizeof(*KillPartsCharSceneAreaPc);
    KILLPARTS_CHARSCENE_PC *scenes = (KILLPARTS_CHARSCENE_PC *)((buffer->addr + 3) & ~3u);
    KillPartsCharSceneAreaPc = scenes;
    buffer->addr = (u32)scenes + size;
    memset(scenes, 0, size);
}

static void CharacterSceneLoadPrivatePc(KILLPARTS_CHARSCENE_PC *entry, i32 model_id, VARIPTR *buffer, VARIPTR end) {
    char path[128];
    sprintf(path, "chars\\%s\\%s.gsc", CDataList[model_id].dir, CDataList[model_id].name);
    entry->scene = NuGScnRead(buffer, end, path);
    if (entry->scene != NULL)
        NuSpecialFind(entry->scene, &entry->special_scene, CDataList[model_id].name, 1);
}
extern "C" {

    void __cdecl CharScenes_LevelLoad(WORLDINFO *world) {
        for (i32 i = 0; i < CHARCOUNT; ++i) {
            ((CHARACTER_SCENE_WORLD_PC *)world)->character_scenes[i].scene = NULL;
            if ((KillPartsCharSceneAreaPc == NULL || KillPartsCharSceneAreaPc[i].scene == NULL) &&
                (CDataList[i].flags & 1) != 0 && world->cutscene_sys != NULL &&
                ((((CHARACTER_SCENE_CUTSYS_PC *)world->cutscene_sys)->character_flags[i / 32] & (1 << (i & 31))) != 0))
                CharacterSceneLoadPrivatePc(&((CHARACTER_SCENE_WORLD_PC *)world)->character_scenes[i], i,
                                            &world->giz_buffer, world->buffer_end);
        }
    }

    void __cdecl CharScenes_AreaLoad(APICHARACTERMODELLIST *list, VARIPTR *buffer, VARIPTR end) {
        if (KillPartsCharSceneAreaPc == NULL)
            return;
        for (i32 i = 0; i < apicharsys->loaded_model_count; ++i) {
            const i32 model_id = apicharsys->models[i].model_id;
            if (KillPartsCharSceneAreaPc[model_id].scene == NULL && (CDataList[model_id].flags & 1) != 0) {
                /* Retail reuses the loop counter as InModelList's output index. */
                if (InModelList(list, model_id, &i) != 0 && list[i].count != 0)
                    CharacterSceneLoadPrivatePc(&KillPartsCharSceneAreaPc[model_id], model_id, buffer, end);
            }
        }
    }

    NUSPECIALHANDLE *__cdecl CharScene_FindHSpecial(WORLDINFO *world, i32 character_id) {
        KILLPARTS_WORLD_SCENES_PC *scenes = reinterpret_cast<KILLPARTS_WORLD_SCENES_PC *>(world);
        NUSPECIALHANDLE *special;
        if (KillPartsCharSceneAreaPc != NULL && KillPartsCharSceneAreaPc[character_id].scene != NULL)
            special = &KillPartsCharSceneAreaPc[character_id].special_scene;
        else if (scenes->character_scenes[character_id].scene != NULL)
            special = &scenes->character_scenes[character_id].special_scene;
        else
            special = NULL;
        if (special != NULL && NuSpecialExistsFn(special) == 0)
            special = NULL;
        return special;
    }
}

void __cdecl CharScene_Draw(WORLDINFO *world, i32 character_id, NUMTX *matrix, NUMTX *reflection_matrix) {
    NUSPECIALHANDLE *special = CharScene_FindHSpecial(world, character_id);
    if (special != NULL) {
        if (matrix != NULL)
            NuSpecialDrawAt(special, matrix);
        if (reflection_matrix != NULL)
            NuSpecialDrawAt(special, reflection_matrix);
    }
}
extern "C" {

    void __cdecl CharScenes_LevelDump(WORLDINFO *world) {
        CHARACTER_SCENE_WORLD_PC *resource = (CHARACTER_SCENE_WORLD_PC *)world;
        if (resource->character_scenes == NULL)
            return;
        for (i32 i = 0; i < CHARCOUNT; ++i) {
            if (resource->character_scenes[i].scene != NULL)
                NuGScnRemove(resource->character_scenes[i].scene);
            resource->character_scenes[i].scene = NULL;
        }
    }
}

extern "C" void CharScenes_AreaDump() {
    if (KillPartsCharSceneAreaPc == NULL) {
        return;
    }
    for (i32 i = 0; i < CHARCOUNT; ++i) {
        if (KillPartsCharSceneAreaPc[i].scene != NULL)
            NuGScnRemove(KillPartsCharSceneAreaPc[i].scene);
        KillPartsCharSceneAreaPc[i].scene = NULL;
    }
}
extern "C" {
    void __cdecl DrawCharIcon(i32 character_id, f32 x, f32 y, f32 z, f32 scale, i32 frame_object_id,
                              f32 character_alpha, f32 frame_alpha, i32 draw_character,
                              NUSPECIALHANDLE *override_special) {
        WORLDINFO *world = WorldInfo_CurrentlyActive();
        NUSPECIALHANDLE special;
        if (ObjTabList == NULL || world == NULL)
            return;
        if (DrawCharIconFindPc != 0)
            NuSpecialFind(things_scene, &special, ObjTabList[frame_object_id].name, 1);
        else
            special = world->level_objects[frame_object_id].special;
        if (NuSpecialExistsFn(&special) != 0)
            DrawPanel3DObject(x, y, (f32)(z + 1.0), scale, scale, scale, 0, 0, 0, &special, 0, frame_alpha);
        if (draw_character != 0) {
            if (override_special != NULL && NuSpecialExistsFn(override_special) != 0)
                DrawPanel3DObject(x, y, (f32)(z + 1.0), scale, scale, scale, 0, 0, 0, override_special, 0,
                                  character_alpha);
            else {
                i32 icon_object_id;
                if (character_id == -1)
                    icon_object_id = LegoObjectIconQuestionPc;
                else {
                    icon_object_id = CDataList[character_id].icon_model;
                    if (icon_object_id != -1 && GCDataList[character_id].hover_fall_mode == 0 &&
                        icon_object_id != PCArcadeUnpairedIconModel)
                        ++icon_object_id;
                }
                if (icon_object_id != -1) {
                    if (DrawCharIconFindPc != 0) {
                        NUGSCN *icon_scene = character_id == -1 ? NULL : IconScene_FindById(character_id);
                        if (icon_scene != NULL)
                            NuSpecialFind(icon_scene, &special, ObjTabList[icon_object_id].name, 1);
                        else
                            NuSpecialFind(things_scene, &special, ObjTabList[icon_object_id].name, 1);
                    } else
                        special = world->level_objects[icon_object_id].special;
                    if (NuSpecialExistsFn(&special) != 0)
                        DrawPanel3DObject(x, y, (f32)(z + 1.0), scale, scale, scale, 0, 0, 0, &special, 0,
                                          character_alpha);
                }
            }
        }
        DrawCharIconFindPc = 0;
        DrawCharIconPanelPc = -1;
    }

    i32 __cdecl LayerFromName(PLAYERCHARACTERCONFIG_PC *character, const char *name) {
        i32 i;
        for (i = 0; i < character->layer_count; ++i) {
            if (NuStrICmp(name, character->layers[i].name) == 0) {
                return character->layers[i].mask_bit;
            }
        }
        return -1;
    }

    i32 __cdecl MakeLayerList_Name(CHARACTER_MODEL_LAYERS_PC *model, i16 *output, u32 mask) {
        PLAYERCHARACTERCONFIG_PC *character;
        i32 count;
        i32 bit;
        u32 flag;
        if (model == NULL || output == NULL) {
            return 0;
        }
        character = &GCDataList[model->model_id];
        count = 0;
        flag = 1;
        for (bit = 0; bit < 32; ++bit, flag <<= 1) {
            i32 layer;
            i16 hierarchy_layer;
            if ((mask & flag) == 0 || bit >= character->layer_count) {
                continue;
            }
            if (character->layer_lookup != NULL) {
                layer = character->layer_lookup[bit];
            } else {
                for (layer = 0; layer < character->layer_count; ++layer) {
                    if (character->layers[layer].mask_bit == bit) {
                        break;
                    }
                }
                if (layer == character->layer_count) {
                    continue;
                }
            }
            if (layer == -1) {
                continue;
            }
            hierarchy_layer = character->layers[layer].hierarchy_layer_index;
            if (hierarchy_layer != -1) {
                *output++ = hierarchy_layer;
                ++count;
            }
        }
        return count;
    }

    void __cdecl CharCategories_Init(CHAR_CATEGORY_RECORD_PC *categories) {
        CharCategory = categories;
        CHARCATEGORYCOUNT = 0;
        if (categories != NULL) {
            while (categories->name != NULL) {
                ++CHARCATEGORYCOUNT;
                ++categories;
            }
        }
    }

    i32 __cdecl CharCategory_FindByName(char *name) {
        if (CharCategory == NULL)
            return -1;
        for (i32 index = 0; index < CHARCATEGORYCOUNT; ++index)
            if (NuStrICmp(CharCategory[index].name, name) == 0)
                return index;
        return -1;
    }

    i32 __cdecl CharCategory_IsCategory(GAMEOBJECT *object, i32 index) {
        CHAR_CATEGORY_RECORD_PC *category;
        if (index < 0 || index >= CHARCATEGORYCOUNT)
            return 0;
        category = &CharCategory[index];
        if (category->model_flags != 0) {
            if ((category->model_flags & 8) != 0 && (object->character_data->game_character->flags_094 & 0x8000) != 0)
                return 0;
            if ((object->character_data->model_flags & category->model_flags) != category->model_flags)
                return 0;
        }
        if (category->game_flags != 0 &&
            (object->character_data->game_character->flags_090 & category->game_flags) != category->game_flags)
            return 0;
        return 1;
    }

    void __cdecl SetMoveAndAnimateFunctions(u32 model_mask, u32 model_value, u32 game_mask, u32 game_value,
                                            i32 movement_type, CHARACTER_UPDATE_CALLBACK_PC move,
                                            CHARACTER_UPDATE_CALLBACK_PC animate,
                                            CHARACTER_UPDATE_CALLBACK_PC post_animate) {
        i32 index;
        for (index = 0; index < CHARCOUNT; ++index) {
            if (model_mask != 0 && (CDataList[index].model_flags & model_mask) != model_value)
                continue;
            if (game_mask != 0 && (GCDataList[index].flags_090 & game_mask) != game_value)
                continue;
            if (movement_type != -1 && (i32)(i8)GCDataList[index].weapon_action != movement_type)
                continue;
            if (move != NULL)
                ((CHARACTER_CALLBACK_WRITER_PC *)&CDataList[index])->move = move;
            if (animate != NULL)
                ((CHARACTER_CALLBACK_WRITER_PC *)&CDataList[index])->animate = animate;
            if (post_animate != NULL)
                ((CHARACTER_CALLBACK_WRITER_PC *)&CDataList[index])->post_animate = post_animate;
        }
    }

    void __cdecl GameLoadCharacterModels(APICHARACTERMODELLIST *list, i32 append_models, VARIPTR *buffer, VARIPTR *end,
                                         i32 area_models, i32 area) {
        if (area_models != 0 && CutScenePlayer_Active() != NULL && area != -1 && &ADataList[area] != HUB_ADATA)
            area_models = 0;
        APILoadCharacterModels(list, append_models, buffer, *end, area_models);
    }

    i32 __cdecl InModelListDataFlags(APICHARACTERMODELLIST *list, u32 model_flags, u32 game_flags, i32 require_hats,
                                     i32 reject_hidden) {
        if (list != NULL) {
            for (; list->model_id != -1; ++list) {
                i32 id;
                if (Collection_Got(list->model_id) == 0)
                    continue;
                id = list->model_id;
                if ((GCDataList[id].flags_090 & game_flags) != game_flags ||
                    (CDataList[id].model_flags & model_flags) != model_flags ||
                    (reject_hidden != 0 && (GCDataList[id].flags_094 & 0x4000) != 0) ||
                    ((model_flags & 8) != 0 && (GCDataList[id].flags_094 & 0x8000) != 0))
                    continue;
                if (require_hats == 0 || CanWearHatsInFreePlay(id) != 0)
                    return 1;
            }
        }
        return 0;
    }

    i32 __cdecl RandomIDFromFlags(u32 model_flags, u32 game_flags, i32 require_hat, APICHARACTERMODELLIST *excluded,
                                  i32 exclude_hidden) {
        i16 candidates[500];
        i32 count = 0;
        i32 id;
        CHARACTERDATA_PC *character = CDataList;
        for (id = 0; id < CHARCOUNT; ++id, ++character) {
            PLAYERCHARACTERCONFIG_PC *runtime;
            if (excluded != NULL && InModelList(excluded, id, NULL) != 0)
                continue;
            if (Game_Customiser != NULL && (id == ((CUSTOMISER_LAYERS_PC *)Game_Customiser)->character_ids[0] ||
                                            id == ((CUSTOMISER_LAYERS_PC *)Game_Customiser)->character_ids[1]))
                continue;
            runtime = character->game_character;
            if (character->move_fn == Move_DEFAULT || (runtime->flags_090 & game_flags) != game_flags ||
                (character->model_flags & model_flags) != model_flags ||
                (exclude_hidden != 0 && (runtime->flags_094 & 0x4000) != 0) ||
                ((model_flags & 8) != 0 && (runtime->flags_094 & 0x8000) != 0) || Collection_Got(id) == 0 ||
                (require_hat != 0 && CanWearHatsInFreePlay(id) == 0))
                continue;
            if (count < 500)
                candidates[count++] = (i16)id;
        }
        if (count > 0) {
            if (count > 1)
                return candidates[qrand() / (0xffff / count + 1)];
            return candidates[0];
        }
        return -1;
    }
    static char **CustomiserVariantNamesPc = 0;
    static i32 CustomiserVariantCountPc = 0;

    void __cdecl CharVariants_Init(char **variants, i32 count) {
        if (variants != NULL && count > 0) {
            CustomiserVariantNamesPc = variants;
            CustomiserVariantCountPc = count;
        }
    }

    i32 __cdecl CharVariant_Find(char *name) {
        i32 index;
        if (CustomiserVariantNamesPc != NULL) {
            for (index = 0; index < CustomiserVariantCountPc; ++index) {
                if (NuStrICmp(CustomiserVariantNamesPc[index], name) == 0)
                    return index;
            }
        }
        return -1;
    }
}

extern "C" void __cdecl CreateFadeMaterials(void) {
    FadeMtl = NuMtlCreate(1);
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes |= 0xc000;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0x20u) | 0x10u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0x10000u) | 0x20000u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0x200u) | 0x100u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0x800u) | 0x400u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes |= 0x40000;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0xcu) | 3u;
    NuMtlUpdate(FadeMtl);

    FadeMtl2 = NuMtlCreate(1);
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes |= 0xc000;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0x20u) | 0x10u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0x10000u) | 0x20000u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0x200u) | 0x100u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0x800u) | 0x400u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes |= 0x40000;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0xeu) | 1u;
    NuMtlUpdate(FadeMtl2);

    ScreenFadeMtl = NuMtlCreate(1);
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes |= 0xc000;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0x200u) | 0x100u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0x800u) | 0x400u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0x20u) | 0x10u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0x10000u) | 0x20000u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes |= 0x40000;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->opacity = 1.0f;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0xeu) | 1u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->texture_id = 0;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->sort_priority = 0x7ffe;
    NuMtlUpdate(ScreenFadeMtl);
}

static void NewCharacterIdle(GAMEOBJECT *object, i32 default_idle) {
    CHARACTER_IDLE_STATE_PC *state = (CHARACTER_IDLE_STATE_PC *)object;
    CHARACTERDATA_PC *character = object->character_data;
    i32 alternate = ((CHARACTER_IDLE_POLICY_PC *)character->game_character)->use_standard_idle == 0 &&
                            (character->model_flags & 0x80) != 0
                        ? 118
                        : 25;
    i32 candidates[233];
    i32 count = 0;
    i32 animation, repetitions, minimum, maximum;
    CHARACTERANIM_CONFIG_PC *info;
    for (animation = 0; animation < 233; ++animation) {
        if (object->character_model->action_animations[animation] == NULL)
            continue;
        info = object->character_model->animation_configs[animation];
        if ((info->flags & 0x10) == 0)
            continue;
        if (default_idle == alternate) {
            if ((info->flags & 0x800) == 0)
                continue;
        } else if ((info->flags & 0x800) != 0)
            continue;
        candidates[count++] = animation;
    }
    if (count > 0) {
        do {
            state->idle_animation = (i16)candidates[count > 1 ? qrand() / (65535 / count + 1) : 0];
        } while (count > 1 && state->idle_animation == state->previous_idle_animation);
        info = object->character_model->animation_configs[state->idle_animation];
        minimum = info->minimum_repetitions;
        repetitions = 1;
        if (minimum > 0) {
            maximum = info->maximum_repetitions;
            repetitions = minimum;
            if (maximum > 0 && maximum > minimum)
                repetitions += qrand() / (65535 / (maximum - minimum) + 1);
        }
        state->previous_idle_animation = state->idle_animation;
        if (repetitions > 1 && (object->character_model->animation_configs[state->idle_animation]->flags & 2) == 0)
            repetitions = 1;
        state->idle_animation_time = 0.0f;
        state->idle_animation_limit =
            AnimDuration((i16)object->id, state->idle_animation, 0.0f, 0.0f, 0) * repetitions -
            animduration_blendouttime;
    } else {
        ResetCharacterIdle(object, 0, default_idle);
    }
}

extern "C" void __cdecl UpdateCharacterIdle(GAMEOBJECT *object) {
    CHARACTER_IDLE_STATE_PC *state = (CHARACTER_IDLE_STATE_PC *)object;
    CHARACTERDATA_PC *character;
    i32 alternate, default_idle;
    i16 requested;
    if (object->character_model == NULL)
        return;
    character = object->character_data;
    alternate = ((CHARACTER_IDLE_POLICY_PC *)character->game_character)->use_standard_idle == 0 &&
                        (character->model_flags & 0x80) != 0
                    ? 118
                    : 25;
    requested = object->anim_packet.requested_animation;
    if (requested != 1 && requested != alternate)
        default_idle = GetDefaultIdle(object);
    else if (requested == 1 && object->anim_packet.previous_animation == alternate)
        default_idle = 1;
    else if (requested == alternate && object->anim_packet.previous_animation == 1)
        default_idle = alternate;
    else
        default_idle = -1;
    if (default_idle != -1) {
        ResetCharacterIdle(object, 1, default_idle);
        return;
    }
    if (requested == 1) {
        state->idle_total_time += FRAMETIME;
        state->idle_animation_time += FRAMETIME;
        if (state->idle_animation_time >= state->idle_animation_limit) {
            if (state->idle_animation == requested)
                NewCharacterIdle(object, 1);
            else
                ResetCharacterIdle(object, 1, 1);
        }
        object->anim_packet.requested_animation = state->idle_animation;
        return;
    }
    if (requested == alternate) {
        state->idle_total_time += FRAMETIME;
        state->idle_animation_time += FRAMETIME;
        if (state->idle_animation_time >= state->idle_animation_limit) {
            if (state->idle_animation == alternate)
                NewCharacterIdle(object, alternate);
            else
                ResetCharacterIdle(object, 1, alternate);
        }
        object->anim_packet.requested_animation = state->idle_animation;
        return;
    }
    ResetCharacterIdle(object, 1, GetDefaultIdle(object));
}

extern "C" void __cdecl PostAnimate_FETT(GAMEOBJECT *object) {
    i32 count, first_joint;
    if (object->dead == 0 &&
        (object->character_context == -1 || (object->character_context == 21 && object->field_7aa == 62) ||
         (object->character_context == 20 && object->field_7aa == 77) ||
         (object->character_context == 46 && object->field_7aa == 111) ||
         (object->character_context == 10 && object->field_7aa == 60)) &&
        object->jump_variant_state == 1 && object->origin_joint_ready != 0) {
        count = ParticlesPerSecond(100.0f, FRAMETIME);
        first_joint = object->id == (u16)id_BOBAFETT ? 10 : 7;
        if (object->character_model->points_of_interest[first_joint] != NULL)
            AddGameDebrisRot(WORLD->game_debris_sys, 27, &object->locator_matrices[first_joint].translation, count, 0,
                             object->facing_angle);
        if (object->character_model->points_of_interest[8] != NULL)
            AddGameDebrisRot(WORLD->game_debris_sys, 27, &object->locator_matrices[8].translation, count, 0,
                             object->facing_angle);
        PlaySfx((char *)"JgThrust", &object->collision_position);
    }
}

extern "C" void __cdecl PostAnimate_ASTROMECH(GAMEOBJECT *object) {
    CHARACTER_COMMUNICATE_READER_PC *state = (CHARACTER_COMMUNICATE_READER_PC *)object;
    i32 count, effect, locator;
    NUVEC position;
    if (object->dead == 0 && object->character_context == 0 && object->model_draw_result != 0 &&
        object->origin_joint_ready != 0 &&
        (object->jump_variant_state != 3 || (object->pad->buttons_held & GAMEPAD_JUMP) != 0)) {
        count = ParticlesPerFrame(object->jump_variant_state == 3 ? 0.25f : 1.0f, FRAMETIME);
        if (count > 0) {
            effect = object->id == (u16)id_R2Q5 ? 12 : 11;
            if (object->character_model->points_of_interest[2] != NULL)
                AddGameDebrisRot(WORLD->game_debris_sys, effect, &object->locator_matrices[2].translation, count,
                                 0x71c7, object->facing_angle);
            if (object->character_model->points_of_interest[3] != NULL)
                AddGameDebrisRot(WORLD->game_debris_sys, effect, &object->locator_matrices[3].translation, count,
                                 0x8e38, object->facing_angle);
        }
        PlaySfxAndSetVolume((char *)"R2Thrust", &object->collision_position,
                            object->jump_variant_state == 3 ? 0.5f : 1.0f);
    } else if (object->character_context == -1 && object->pad->input_magnitude > 0.0f) {
        if (state->communicate_blend > 0.0f)
            PlaySfx((char *)"drd_r2_mvt_water_lp", &object->upper_position);
        else if (object->field_281 != 0)
            PlaySfx((char *)"R2Move", &object->collision_position);
    }
    if (state->communicate_blend > 0.0f && state->communicate_blend < 1.0f) {
        effect = WORLD->game_debris_sys->entries[object->id == (u16)id_R2Q5 ? 1 : 3].effect;
        if (effect != -1) {
            count = ParticlesPerSecond(100.0f, FRAMETIME);
            if (count > 0) {
                locator = object->character_data->game_character->thingy_locator;
                if (locator != -1) {
                    position = object->locator_matrices[locator].translation;
                    if (object->field_283 == 9 && position.y < object->water_height)
                        position.y = object->water_height;
                    AddVariableShotDebrisEffect(effect, &position, count, 0, 0);
                }
            }
        }
    }
}

void __cdecl FinishWeirdoNames(i32 which) {
    char *name;
    i32 length;
    if (which != 1) {
        name = Game.customiser_start[0].name;
        NuStrUpr(name, name);
        for (length = NuStrLen(name); length < 15; ++length)
            name[length] = ' ';
        name[15] = '\0';
    }
    if (which == 0)
        return;
    name = Game.customiser_start[1].name;
    NuStrUpr(name, name);
    for (length = NuStrLen(name); length < 15; ++length)
        name[length] = ' ';
    name[15] = '\0';
}

void __cdecl LSW_SetIndy(i32 area_index) {
    if (area_index != -1 && (&ADataList[area_index] == LOSTTEMPLE_ADATA || &ADataList[area_index] == JIMTEST_ADATA)) {
        LEDGETERRAINLOOKAHEAD = 0.075f;
        PUNCHGAP = 0.3f;
        PUNCHCHARGAP = 0.3f;
        LedgeTerrain_CheckAnims = 0;
        StartupGameFlag0 = 1;
        DefaultMotionLedgeTerrainEnabledPc = 1;
        Grapples_Available = 1;
        Grapple_RopeSwingsAvailable = 1;
        SuperCarryEnablePc = 1;
        CombatRollEnablePc = 1;
        LEGOHINT_PUNCH = 0;
        TechnoSysPc.interaction_time = -0.2f;
        TechnoSysPc.idle_offsets[0] = v000;
        TechnoSysPc.idle_offsets[1] = v000;
        TechnoSysPc.idle_object_ids[1] = 0x2f;
        TechnoSysPc.idle_object_ids[2] = 0x30;
        TechnoSysPc.idle_object_ids[3] = 0x31;
        TechnoSysPc.idle_offsets[2].x = 0.0f;
        TechnoSysPc.idle_offsets[2].y = 0.0f;
        TechnoSysPc.idle_offsets[2].z = -0.1295f;
        TechnoSysPc.idle_object_ids[0] = 0x56;
        TechnoSysPc.hand_object_ids[0] = -1;
        TechnoSysPc.hand_object_ids[1] = -1;
        TechnoDefaultObjectPc = 0x56;
        GameAudio_SetActionMusicTimes(0.1f, 3.0f);
    } else {
        PUNCHGAP = 0.3f;
        PUNCHCHARGAP = 0.3f;
        StartupGameFlag0 = 0;
        DefaultMotionLedgeTerrainEnabledPc = 0;
        Grapples_Available = 0;
        SuperCarryEnablePc = 0;
        CombatRollEnablePc = 0;
        LEGOHINT_PUNCH = 1;
        TechnoSysPc.interaction_time = 0.2f;
        TechnoSysPc.idle_offsets[0] = v000;
        TechnoSysPc.idle_offsets[1] = v000;
        TechnoSysPc.idle_object_ids[1] = 0x2c;
        TechnoSysPc.idle_object_ids[2] = -1;
        TechnoSysPc.idle_object_ids[3] = -1;
        TechnoSysPc.idle_offsets[2] = v000;
        TechnoSysPc.idle_object_ids[0] = 0x55;
        TechnoSysPc.hand_object_ids[0] = 0x2d;
        TechnoSysPc.hand_object_ids[1] = 0x2e;
        TechnoDefaultObjectPc = 0x55;
        GameAudio_SetActionMusicTimes(1.0f, 6.0f);
    }
}

// Append to src/legoapi/characters.cpp. Retail 00419e90/284 and 00419fb0/808.
extern "C" i32 CharacterLoadAreaModelsPc;
extern "C" i16 CharacterFixupIdPc007f18c4;
extern "C" void __cdecl LoadSingleCharacter(BGPROCINFO *) {
    APICHARACTERMODELLIST list[2] = {{(i16)waiting_for_character, 1}, {-1, 0}};
    append = 1;
    nopakfile = CHARPAK == 0;
    APILoadCharacterModels(list, 0, &characterbuffer_ptr, characterbuffer_end, CharacterLoadAreaModelsPc);
    IconScenes_Load(list, 0, &characterbuffer_ptr, &characterbuffer_end);
    NUGSCN *scene = IconScene_FindById(list[0].model_id);
    if (scene != NULL) {
        for (i32 i = 0; i < LEVELOBJECTCOUNT; ++i) {
            if (ObjTab[i].kind != 3 || WORLD->level_objects[i].active != 0)
                continue;
            if (NuSpecialFind(scene, &WORLD->level_objects[i].special, ObjTab[i].name, 1) != 0)
                WORLD->level_objects[i].active = 1;
        }
    }
    Customiser_SaveModelTextureIDs(CharacterCustomiser, APICharacterLoaded(list[0].model_id));
    hub_character_ready = waiting_for_character;
    waiting_for_character = -1;
}

extern "C" void __cdecl UpdateCharacterLoad(void) {
    i32 candidates[340];
    i32 candidate;
    i32 menu = GetMenuID();
    if (menu == 13 || menu == 15 || menu == 16 || menu == 17 || LOADEROFF != 0 || BGLOAD == 0)
        return;
    if (waiting_for_character != -1 || hub_character_ready != -1 || bgGetProcActive() != NULL ||
        (i32)nudlist_field_9d8 > 0xfd)
        return;
    i32 required = (0x3ff - (i32)(((f64)CHARACTERBUFFERSIZE / 7077888.0) * -1048576.0)) & ~0x3ff;
    if ((i32)(characterbuffer_end.addr - characterbuffer_ptr.addr) < required)
        return;
    i32 count = 0;
    if (VehicleBoltId7f17d4Pc != -1 && APICharacterLoaded(VehicleBoltId7f17d4Pc) != NULL &&
        PunchRangeCompanionIdPc != -1 && APICharacterLoaded(PunchRangeCompanionIdPc) == NULL) {
        candidates[count++] = PunchRangeCompanionIdPc;
    } else if (id_BARMAN != -1 && APICharacterLoaded(id_BARMAN) == NULL) {
        candidates[count++] = id_BARMAN;
    } else if (id_CANTINABAND != -1 && APICharacterLoaded(id_CANTINABAND) == NULL) {
        candidates[count++] = id_CANTINABAND;
    } else if (PermanentWeirdo1IdPc != -1 && APICharacterLoaded(PermanentWeirdo1IdPc) == NULL) {
        candidates[count++] = PermanentWeirdo1IdPc;
    } else if (PermanentWeirdo2IdPc != -1 && APICharacterLoaded(PermanentWeirdo2IdPc) == NULL) {
        candidates[count++] = PermanentWeirdo2IdPc;
    } else if (DefaultAnimationJabbaIdPc != -1 && APICharacterLoaded(DefaultAnimationJabbaIdPc) == NULL) {
        candidates[count++] = DefaultAnimationJabbaIdPc;
    } else if (id_MOSEISLEYCITIZEN != -1 && APICharacterLoaded(id_MOSEISLEYCITIZEN) == NULL) {
        candidates[count++] = id_MOSEISLEYCITIZEN;
    } else if (id_CANTINAALIEN != -1 && APICharacterLoaded(id_CANTINAALIEN) == NULL) {
        candidates[count++] = id_CANTINAALIEN;
    } else if (CharacterFixupIdPc007f18c4 != -1 && APICharacterLoaded(CharacterFixupIdPc007f18c4) == NULL) {
        candidates[count++] = CharacterFixupIdPc007f18c4;
    } else {
        for (i32 id = 0; id < CHARCOUNT; ++id) {
            if (id == id_DROIDEKA || (CDataList[id].model_flags & 0x04002000) != 0 || APICharacterLoaded(id) != NULL)
                continue;
            if ((i32)CDataList[id].animation_count + apicharsys->loaded_animation_count >
                apicharsys->animation_capacity)
                continue;
            if (InCollectList_Index(id, NULL, 0) == -1 || Collection_Got(id) == 0)
                continue;
            candidates[count++] = id;
        }
    }
    if (count <= 0)
        return;
    if (count > 1)
        candidate = qrand() / ((0xffff / count) + 1);
    else
        candidate = 0;
    waiting_for_character = candidates[candidate];
    WaitingForCharacterTime = 0.0f;
    bgPostRequest(LoadSingleCharacter, NULL, NULL, 0);
}

extern "C" {
    i16 __cdecl FindAnimIX(CHARACTERDATA_PC *character, char *name) {
        CHARACTERANIM_CONFIG_PC *animation;
        if (character != NULL) {
            animation = character->animations;
            if (animation != NULL) {
                do {
                    if (animation->name == NULL)
                        break;
                    if (NuStrICmp(name, animation->name) == 0)
                        return animation->action_id;
                    ++animation;
                } while (animation != NULL);
            }
        }
        return -1;
    }
}
