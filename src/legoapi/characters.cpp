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

typedef struct CHARACTER_SCENE_WORLD_PC {
    u8 unknown_0000[0x2980];                  // 0x0000
    KILLPARTS_CHARSCENE_PC *character_scenes; // 0x2980
} CHARACTER_SCENE_WORLD_PC;                   // size 0x2984

typedef struct CHARACTER_SCENE_CUTSYS_PC {
    CUTINFO **records;       // 0x0
    i32 count;               // 0x4
    u32 *character_flags;    // 0x8
} CHARACTER_SCENE_CUTSYS_PC; // size 0xc

STATIC_ASSERT(offsetof(WORLDINFO, buffer_end) == 0x108);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, flags) == 0x40);
STATIC_ASSERT(offsetof(LEVELDATA_PC, flags) == 0x64);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_index) == 0x94);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_tracks) == 0x118);
STATIC_ASSERT(sizeof(LEVELDATA_PC) == 0x130);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, previous_animation) == 0x38);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, requested_animation) == 0x3a);
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
    if (list == NULL) {
        return 0;
    }
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

static void CharScene_Load(KILLPARTS_CHARSCENE_PC *entry, i32 model_id, VARIPTR *buffer, VARIPTR end) {
    char path[128];
    sprintf(path, "chars\\%s\\%s.gsc", CDataList[model_id].dir, CDataList[model_id].name);
    entry->scene = NuGScnRead(buffer, end, path);
    if (entry->scene != NULL) {
        NuSpecialFind(entry->scene, &entry->special_scene, CDataList[model_id].name, 1);
    }
}

extern "C" {

    void __cdecl CharScenes_LevelLoad(WORLDINFO *world) {
        for (i32 i = 0; i < CHARCOUNT; ++i) {
            ((CHARACTER_SCENE_WORLD_PC *)world)->character_scenes[i].scene = NULL;
            if ((KillPartsCharSceneAreaPc == NULL || KillPartsCharSceneAreaPc[i].scene == NULL) &&
                (CDataList[i].flags & 1) != 0 && world->cutscene_sys != NULL &&
                ((((CHARACTER_SCENE_CUTSYS_PC *)world->cutscene_sys)->character_flags[i / 32] & (1 << (i & 31))) !=
                 0)) {
                CharScene_Load(&((CHARACTER_SCENE_WORLD_PC *)world)->character_scenes[i], i, &world->giz_buffer,
                               world->buffer_end);
            }
        }
    }

    void __cdecl CharScenes_AreaLoad(APICHARACTERMODELLIST *list, VARIPTR *buffer, VARIPTR end) {
        if (KillPartsCharSceneAreaPc == NULL) {
            return;
        }
        for (i32 i = 0; i < apicharsys->loaded_model_count; ++i) {
            const i32 model_id = apicharsys->models[i].model_id;
            if (KillPartsCharSceneAreaPc[model_id].scene == NULL && (CDataList[model_id].flags & 1) != 0) {
                /* Retail reuses the loop counter as InModelList's output index. */
                if (InModelList(list, model_id, &i) != 0 && list[i].count != 0) {
                    CharScene_Load(&KillPartsCharSceneAreaPc[model_id], model_id, buffer, end);
                }
            }
        }
    }

    NUSPECIALHANDLE *__cdecl CharScene_FindHSpecial(WORLDINFO *world, i32 character_id) {
        KILLPARTS_WORLD_SCENES_PC *scenes = reinterpret_cast<KILLPARTS_WORLD_SCENES_PC *>(world);
        NUSPECIALHANDLE *special;
        if (KillPartsCharSceneAreaPc != NULL && KillPartsCharSceneAreaPc[character_id].scene != NULL) {
            special = &KillPartsCharSceneAreaPc[character_id].special_scene;
        } else if (scenes->character_scenes[character_id].scene != NULL) {
            special = &scenes->character_scenes[character_id].special_scene;
        } else {
            special = NULL;
        }
        if (special != NULL && NuSpecialExistsFn(special) == 0) {
            special = NULL;
        }
        return special;
    }
}

void __cdecl CharScene_Draw(WORLDINFO *world, i32 character_id, NUMTX *matrix, NUMTX *reflection_matrix) {
    NUSPECIALHANDLE *special = CharScene_FindHSpecial(world, character_id);
    if (special != NULL) {
        if (matrix != NULL) {
            NuSpecialDrawAt(special, matrix);
        }
        if (reflection_matrix != NULL) {
            NuSpecialDrawAt(special, reflection_matrix);
        }
    }
}

extern "C" {

    void __cdecl CharScenes_LevelDump(WORLDINFO *world) {
        CHARACTER_SCENE_WORLD_PC *resource = (CHARACTER_SCENE_WORLD_PC *)world;
        if (resource->character_scenes == NULL) {
            return;
        }
        for (i32 i = 0; i < CHARCOUNT; ++i) {
            if (resource->character_scenes[i].scene != NULL) {
                NuGScnRemove(resource->character_scenes[i].scene);
            }
            resource->character_scenes[i].scene = NULL;
        }
    }
}

extern "C" void CharScenes_AreaDump() {
    if (KillPartsCharSceneAreaPc == NULL) {
        return;
    }
    for (i32 i = 0; i < CHARCOUNT; ++i) {
        if (KillPartsCharSceneAreaPc[i].scene != NULL) {
            NuGScnRemove(KillPartsCharSceneAreaPc[i].scene);
        }
        KillPartsCharSceneAreaPc[i].scene = NULL;
    }
}

extern "C" {
#pragma bss_seg(".bss$IconScenePc")
    static NUGSCN **IconScenePc = 0;
#pragma bss_seg()

    static char AreaIconPathPrivate[0x40] = "stuff\\icons\\";
    STATIC_ASSERT(sizeof(AreaIconPathPrivate) == 0x40);
    STATIC_ASSERT(offsetof(NUGSCN, display_list) == 0x110);
    STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, render_flags) == 0x74);
    STATIC_ASSERT(offsetof(APICHARACTERMODELLIST, model_id) == 0);
    STATIC_ASSERT(offsetof(APICHARACTERMODELLIST, count) == 2);
    STATIC_ASSERT(offsetof(CHARACTERDATA_PC, icon_model) == 0x42);

    void __cdecl IconScenes_Init(char *path, VARIPTR *buffer, VARIPTR *end) {
        u32 table_size = (u32)CHARCOUNT * sizeof(*IconScenePc);
        u32 addr = (buffer->addr + 3) & ~3u;
        IconScenePc = (NUGSCN **)addr;
        buffer->addr = addr + table_size;
        memset((void *)addr, 0, table_size);
        if (path != NULL && NuStrLen(path) < 0x40) {
            NuStrCpy(AreaIconPathPrivate, path);
        }
    }

    NUGSCN *__cdecl IconScene_FindById(i32 id) {
        if (id < 0 || id >= CHARCOUNT) {
            return NULL;
        }
        return IconScenePc[id];
    }

    void __cdecl IconScenes_Load(APICHARACTERMODELLIST *list, i32 mode, VARIPTR *buffer, VARIPTR *end) {
        i32 id;
        NUGSCN *scene;
        NUGSCN **scenes;
        if (list == NULL) {
            return;
        }
        scenes = IconScenePc;
        if (scenes == NULL) {
            return;
        }
        for (; (id = list->model_id) != -1; ++list) {
            i32 object_index;
            char path[0x100];
            if (scenes[id] != NULL || list->count == 0) {
                continue;
            }
            object_index = CDataList[id].icon_model;
            if (object_index == -1) {
                continue;
            }
            NuStrCpy(path, AreaIconPathPrivate);
            NuStrCat(path, LevelObject_FindNameFromIndex(object_index));
            NuStrCat(path, ".gsc");
            scene = NuGScnRead(buffer, *end, path);
            scenes = IconScenePc;
            scenes[id] = scene;
            if (scene != NULL && scene->display_list != NULL) {
                scene->display_list->render_flags |= 0x10;
            }
        }
    }

    void __cdecl IconScenes_Dump(void) {
        i32 i;
        NUGSCN **scenes = IconScenePc;
        if (scenes == NULL) {
            return;
        }
        for (i = 0; i < CHARCOUNT; ++i) {
            NUGSCN *scene = scenes[i];
            if (scene != NULL) {
                NuGScnRemove(scene);
                scenes = IconScenePc;
                scenes[i] = NULL;
            }
        }
    }

    void __cdecl DrawCharIcon(i32 character_id, f32 x, f32 y, f32 z, f32 scale, i32 frame_object_id,
                              f32 character_alpha, f32 frame_alpha, i32 draw_character,
                              NUSPECIALHANDLE *override_special) {
        WORLDINFO *world = WorldInfo_CurrentlyActive();
        NUSPECIALHANDLE special;
        if (ObjTabList == NULL || world == NULL) {
            return;
        }
        if (drawcharicon_find != 0) {
            NuSpecialFind(things_scene, &special, ObjTabList[frame_object_id].name, 1);
        } else {
            special = world->level_objects[frame_object_id].special;
        }
        if (NuSpecialExistsFn(&special) != 0) {
            DrawPanel3DObject(x, y, (f32)(z + 1.0), scale, scale, scale, 0, 0, 0, &special, 0, frame_alpha);
        }
        if (draw_character != 0) {
            if (override_special != NULL && NuSpecialExistsFn(override_special) != 0) {
                DrawPanel3DObject(x, y, (f32)(z + 1.0), scale, scale, scale, 0, 0, 0, override_special, 0,
                                  character_alpha);
            } else {
                i32 icon_object_id;
                if (character_id == -1) {
                    icon_object_id = LegoObjectIconQuestionPc;
                } else {
                    icon_object_id = CDataList[character_id].icon_model;
                    if (icon_object_id != -1 && GCDataList[character_id].hover_fall_mode == 0 &&
                        icon_object_id != PCArcadeUnpairedIconModel) {
                        ++icon_object_id;
                    }
                }
                if (icon_object_id != -1) {
                    if (drawcharicon_find != 0) {
                        NUGSCN *icon_scene = character_id == -1 ? NULL : IconScene_FindById(character_id);
                        if (icon_scene != NULL) {
                            NuSpecialFind(icon_scene, &special, ObjTabList[icon_object_id].name, 1);
                        } else {
                            NuSpecialFind(things_scene, &special, ObjTabList[icon_object_id].name, 1);
                        }
                    } else {
                        special = world->level_objects[icon_object_id].special;
                    }
                    if (NuSpecialExistsFn(&special) != 0) {
                        DrawPanel3DObject(x, y, (f32)(z + 1.0), scale, scale, scale, 0, 0, 0, &special, 0,
                                          character_alpha);
                    }
                }
            }
        }
        drawcharicon_find = 0;
        drawcharicon_i_panel = -1;
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
        if (CharCategory == NULL) {
            return -1;
        }
        for (i32 index = 0; index < CHARCATEGORYCOUNT; ++index) {
            if (NuStrICmp(CharCategory[index].name, name) == 0) {
                return index;
            }
        }
        return -1;
    }

    i32 __cdecl CharCategory_IsCategory(GAMEOBJECT *object, i32 index) {
        CHAR_CATEGORY_RECORD_PC *category;
        if (index < 0 || index >= CHARCATEGORYCOUNT) {
            return 0;
        }
        category = &CharCategory[index];
        if (category->model_flags != 0) {
            if ((category->model_flags & 8) != 0 && (object->character_data->game_character->flags_094 & 0x8000) != 0) {
                return 0;
            }
            if ((object->character_data->model_flags & category->model_flags) != category->model_flags) {
                return 0;
            }
        }
        if (category->game_flags != 0 &&
            (object->character_data->game_character->flags_090 & category->game_flags) != category->game_flags) {
            return 0;
        }
        return 1;
    }

    void __cdecl SetMoveAndAnimateFunctions(u32 model_mask, u32 model_value, u32 game_mask, u32 game_value,
                                            i32 movement_type, CHARACTER_UPDATE_CALLBACK_PC move,
                                            CHARACTER_UPDATE_CALLBACK_PC animate,
                                            CHARACTER_UPDATE_CALLBACK_PC post_animate) {
        i32 index;
        for (index = 0; index < CHARCOUNT; ++index) {
            if (model_mask != 0 && (CDataList[index].model_flags & model_mask) != model_value) {
                continue;
            }
            if (game_mask != 0 && (GCDataList[index].flags_090 & game_mask) != game_value) {
                continue;
            }
            if (movement_type != -1 && GCDataList[index].weapon_action != movement_type) {
                continue;
            }
            if (move != NULL) {
                ((CHARACTER_CALLBACK_WRITER_PC *)&CDataList[index])->move = move;
            }
            if (animate != NULL) {
                ((CHARACTER_CALLBACK_WRITER_PC *)&CDataList[index])->animate = animate;
            }
            if (post_animate != NULL) {
                ((CHARACTER_CALLBACK_WRITER_PC *)&CDataList[index])->post_animate = post_animate;
            }
        }
    }

    void __cdecl GameLoadCharacterModels(APICHARACTERMODELLIST *list, i32 append_models, VARIPTR *buffer, VARIPTR *end,
                                         i32 area_models, i32 area) {
        if (area_models != 0 && CutScenePlayer_Active() != NULL && area != -1 && &ADataList[area] != HUB_ADATA) {
            area_models = 0;
        }
        APILoadCharacterModels(list, append_models, buffer, *end, area_models);
    }

    i32 __cdecl InModelListDataFlags(APICHARACTERMODELLIST *list, u32 model_flags, u32 game_flags, i32 require_hats,
                                     i32 reject_hidden) {
        if (list != NULL) {
            for (; list->model_id != -1; ++list) {
                i32 id;
                if (Collection_Got(list->model_id) == 0) {
                    continue;
                }
                id = list->model_id;
                if ((GCDataList[id].flags_090 & game_flags) != game_flags ||
                    (CDataList[id].model_flags & model_flags) != model_flags ||
                    (reject_hidden != 0 && (GCDataList[id].flags_094 & 0x4000) != 0) ||
                    ((model_flags & 8) != 0 && (GCDataList[id].flags_094 & 0x8000) != 0)) {
                    continue;
                }
                if (require_hats == 0 || CanWearHatsInFreePlay(id) != 0) {
                    return 1;
                }
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
            if (excluded != NULL && InModelList(excluded, id, NULL) != 0) {
                continue;
            }
            if (Game_Customiser != NULL && (id == ((CUSTOMISER_LAYERS_PC *)Game_Customiser)->character_ids[0] ||
                                            id == ((CUSTOMISER_LAYERS_PC *)Game_Customiser)->character_ids[1])) {
                continue;
            }
            runtime = character->game_character;
            if (character->move_fn == Move_DEFAULT || (runtime->flags_090 & game_flags) != game_flags ||
                (character->model_flags & model_flags) != model_flags ||
                (exclude_hidden != 0 && (runtime->flags_094 & 0x4000) != 0) ||
                ((model_flags & 8) != 0 && (runtime->flags_094 & 0x8000) != 0) || Collection_Got(id) == 0 ||
                (require_hat != 0 && CanWearHatsInFreePlay(id) == 0)) {
                continue;
            }
            if (count < 500) {
                candidates[count++] = (i16)id;
            }
        }
        if (count > 0) {
            if (count > 1) {
                return candidates[qrand() / (0xffff / count + 1)];
            }
            return candidates[0];
        }
        return -1;
    }

    static char **CharVariant = 0;
    static i32 CHARVARIANTCOUNT = 0;

    void __cdecl CharVariants_Init(char **variants, i32 count) {
        if (variants != NULL && count > 0) {
            CharVariant = variants;
            CHARVARIANTCOUNT = count;
        }
    }

    i32 __cdecl CharVariant_Find(char *name) {
        i32 index;
        if (CharVariant != NULL) {
            for (index = 0; index < CHARVARIANTCOUNT; ++index) {
                if (NuStrICmp(CharVariant[index], name) == 0) {
                    return index;
                }
            }
        }
        return -1;
    }
}
