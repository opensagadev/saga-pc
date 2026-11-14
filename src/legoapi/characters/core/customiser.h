#ifndef SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/characters/core/characters.h, legoapi/items/base/apiobject.h,
 * legoapi/menus/screens/customise.h, legoapi/world/world.h, nu2api/nu3d/nu3d_includes.h, nu2api/nucore/common.h,
 * nu2api/nucore/fixed_width.h, nu2api/nufile/nufpar.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/characters/core/characters.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nufile/nufpar.h"
#endif
/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART == 1 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP)

typedef struct CUSTOMPIECE_LAYERS_PC {
    u8 unknown_00[0x12];
    u8 layer_flags;
    u8 unknown_13[0x15];
} CUSTOMPIECE_LAYERS_PC;

STATIC_ASSERT(sizeof(CUSTOMPIECE_LAYERS_PC) == 0x28);
STATIC_ASSERT(offsetof(CUSTOMPIECE_LAYERS_PC, layer_flags) == 0x12);
#ifdef __cplusplus
extern "C" {
#endif
    void Customiser_Dump(void);
    void Customiser_DumpAccessories(void *customiser);
    void Customiser_RestoreModelTextureIDs(void *customiser);
    extern i32 customiser_quit;
    extern i32 customiser_quit;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART == 2 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP)

typedef struct KILLPARTS_ACCESSORY_PC {
    void *scene;
    NUSPECIALHANDLE special;
    u8 unknown_10[0x10];
} KILLPARTS_ACCESSORY_PC;

STATIC_ASSERT(sizeof(KILLPARTS_ACCESSORY_PC) == 0x20);
STATIC_ASSERT(offsetof(KILLPARTS_ACCESSORY_PC, special) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    extern KILLPARTS_ACCESSORY_PC KillPartsAccessoriesPc[2][9];
    void Customiser_DumpAll(void *, WORLDINFO *);
    void Customiser_SaveModelTextureIDs(void *customiser, struct CHARACTER_MODEL_LAYERS_PC *model);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART == 3 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif

    void Customiser_SetAnimsToLoad(CUSTOMISER_LAYERS_PC *, i32);
    void Customiser_ResetModelTextureIDs(CUSTOMISER_LAYERS_PC *);

    void Customiser_InitNames(CUSTOMISER_LAYERS_PC *);
    void Customiser_AddPartAccessories(CUSTOMISER_LAYERS_PC *, GAMEOBJECT *, i32, i32, f32);

    CUSTOMISER_LAYERS_PC *Customiser_Configure(char *, VARIPTR *, VARIPTR *, i32, i32, i32 (*)(CUSTOMPIECE_LAYERS_PC *),
                                               void (*)(CUSTOMPIECE_LAYERS_PC *, NUFPAR_PC *), i32 (*)(char *), void *,
                                               i16 *);
    void Customiser_CopyDefaultPiecesToSave(CUSTOMISER_LAYERS_PC *, void *);
    void Customiser_LoadAccessories(CUSTOMISER_LAYERS_PC *, APICHARACTERMODELLIST *);
    void Customiser_Set100PercentPieces(CUSTOMISER_LAYERS_PC *);
    CUSTOMPIECE_LAYERS_PC *Customiser_FindPieceByName(CUSTOMISER_LAYERS_PC *, char *, i32 *, i32 *);
    void Customiser_LoadAll(CUSTOMISER_LAYERS_PC *, WORLDINFO *);
    void Customiser_TransformToPanel(CUSTOMISER_LAYERS_PC *);
    i32 Customiser_NextPieceLeft(CUSTOMISER_LAYERS_PC *, i32, i32, i32, i32);
#ifdef __cplusplus
}
#endif
#ifndef TYPES_OFF_7

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
    i32 (*piece_available)(CUSTOMPIECE_LAYERS_PC *);
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

typedef struct custom_accessory_resource_pc_s {
    NUGSCN *scene;
    NUSPECIALHANDLE special;
    i32 original_texture_id;
    i32 texture_id;
    i32 material_index;
    CHARACTER_MODEL_LAYERS_PC *character_model;
} CUSTOM_ACCESSORY_RESOURCE_PC;

#endif /* TYPES_OFF_7 */
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_CHARACTERS_CORE_CUSTOMISER_H_STOP
