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
